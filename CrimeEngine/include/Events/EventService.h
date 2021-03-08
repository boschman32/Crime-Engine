#pragma once
#include "Core/Core.h"
#include "Core/Logger.h"
#include "Utils/IService.h"

namespace ce
{
	//Defines
#define EVENT_CLASS_TYPE(enumClass, type)\
static enumClass GetStaticType() { return enumClass::type; }\
EventType GetEventType() const override { return static_cast<EventType>(GetStaticType()); }\
const char* GetName() const override { return #type; }
#define EVENT_CLASS_CATEGORY(category)\
static EventCategory GetStaticCategory() { return category; }\
int GetCategoryFlags() const override { return int(GetStaticCategory()); }
#define BIND_EVENT_CALLBACK(func)	std::bind(&func, this, std::placeholders::_1)

	//Event types if you want to add to this use this:
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowLostFocus, WindowMoved, WindowMinimized,
		MapLoaded, MapBuilt,
		Last
	};

	enum class EventCategory
	{
		None = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryMap = 1 << 1,
		EventCategoryCustom = 1 << 2
	};

	inline EventCategory operator | (EventCategory a_lhs, EventCategory a_rhs)
	{
		return static_cast<EventCategory>(
			static_cast<std::underlying_type<EventCategory>::type>(a_lhs) |
			static_cast<std::underlying_type<EventCategory>::type>(a_rhs)
			);
	}

	class IEvent
	{
	public:
		virtual ~IEvent() = default;

		virtual EventType GetEventType()	const = 0;
		virtual const char* GetName()		const = 0;
		virtual int GetCategoryFlags()		const = 0;
		virtual std::string ToString()		const = 0;

		template<typename OStream>
		friend OStream& operator<<(OStream &a_os, const IEvent& a_event)
		{
			return a_os << a_event.ToString();
		}

		bool IsInCategory(EventCategory a_category) const
		{
			return (GetCategoryFlags() & int(a_category)) != 0;
		}

		//Use this to tell whether other listeners can still receive this event.
		mutable bool m_isConsumed{ false };
	};

	class EventService : public IService
	{
		//Policies used by eventpp
		struct EventPolicies
		{
			static EventType getEvent(const IEvent& a_event) //NOLINT <Called from eventpp>
			{
				return a_event.GetEventType();
			}

			static bool canContinueInvoking(const IEvent& a_event) //NOLINT <Called from eventpp>
			{
				return !a_event.m_isConsumed;
			}
		};
	public:
		//Function that allows the use of easy callbacks with derived events.
		template<typename T>
		using CustomEventFunction = std::function<void(T&)>;
		
		using EventCallback = void(IEvent&);
		using EventFunction = std::function<void(IEvent&)>;
		using EventDispatcher = eventpp::EventDispatcher<EventType, EventCallback, EventPolicies>;
		using EventHandle = EventDispatcher::Handle;

		using EventBookkeepingList = std::unordered_map<EventType, std::vector<std::pair<EventFunction, EventHandle>>>;
		
		EventService();

		void OnCreate() override;
		void OnDestroy() override;

		/* --- Subscribing --- */
		
		/**
		  * \brief Subscribe to an event and assign a callback for it to be invoked when the event happens.
		  *
		  * \param a_eventType The type of event you want to listen to.
		  * \param a_callback The callback to be assigned (use BIND_EVENT_CALLBACK for easy assignment).
		  * \param a_hasPriority When set to true the callback will be placed in-front of the event list.
		  *
		  * \return The handle given back by eventpp you can save this and then with unsubscribing use this or use the callback again.
		  *
		 **/
		template<typename EventClass, typename Type,
			typename = typename std::enable_if<std::is_base_of<IEvent, EventClass>::value, EventClass>::type,
			typename = typename std::enable_if<std::is_enum<Type>::value, Type>::type>
		static EventHandle Subscribe(Type a_eventType, CustomEventFunction<EventClass>&& a_callback, bool a_hasPriority = false);

		/* --- Unsubscribing --- */
		
		/**
		  * \brief Unsubscribe from the event to stop listening to it.
		  *
		  * \param a_eventType The type of event you want to unsubscribe from.
		  * \param a_handle The handle given back when you subscribe to the event.
		  *
		 **/
		template<typename Type, typename = std::enable_if<std::is_enum<EventType>::value, EventType>::type>
		static void Unsubscribe(Type a_eventType, EventHandle a_handle);

		/**
		  * \brief Unsubscribe from the event to stop listening to it.
		  *
		  * \param a_eventType The type of event you want to unsubscribe from.
		  * \param a_callback The callback you used to listen to this event.
		  *
		 **/
		template<typename EventClass, typename Type,
			typename = typename std::enable_if<std::is_base_of<IEvent, EventClass>::value, EventClass>::type,
			typename = typename std::enable_if<std::is_enum<Type>::value, Type>::type>
		static void Unsubscribe(Type a_eventType, CustomEventFunction<EventClass> a_callback);

		/**
		  * \brief Dispatch an event and let anyone listening to it that it happened
		  *
		  * \param a_event The event you want to send out.
		  *
		 **/
		static void Dispatch(IEvent& a_event) { Get().DispatchImpl(a_event); };
	private:
		//Use a static get function to make access to the EventService easier and quicker.
		static EventService& Get()
		{
			CE_CORE_ASSERT_M(ms_instance != nullptr, "EventService hasn't been created, call MakeService first!");
			return *ms_instance;
		}
		static EventService* ms_instance;

		//The dispatcher that takes care of all the events to be dispatched.
		EventDispatcher m_dispatcher;

		//Keep track of the handles that are attached to the callbacks for easy unsubscribing.
		EventBookkeepingList m_eventBookList;

		/* -- Subscribing -- */
		template<typename EventClass>
		EventHandle SubscribeImpl(EventType a_eventType, CustomEventFunction<EventClass> a_callback, bool a_hasPriority);

		/* -- Unsubscribing -- */
		void UnsubscribeImpl(EventType a_eventType, EventHandle a_handle);
		
		template<typename EventClass>
		void UnsubscribeImpl(EventType a_eventType, CustomEventFunction<EventClass> a_callback);

		/* -- Dispatch -- */
		void DispatchImpl(IEvent& a_event) const;
	};

	/* -- Subscribing -- */
	
	template<typename EventClass, typename Type, typename, typename>
	EventService::EventHandle EventService::Subscribe(Type a_eventType, CustomEventFunction<EventClass>&& a_callback, bool a_hasPriority)
	{
		return Get().SubscribeImpl(static_cast<EventType>(a_eventType), std::forward<CustomEventFunction<EventClass>>(a_callback), a_hasPriority);
	}

	/* -- Unsubscribing -- */

	template <typename Type, typename>
	void EventService::Unsubscribe(Type a_eventType, EventHandle a_handle)
	{
		Get().UnsubscribeImpl(static_cast<EventType>(a_eventType), a_handle);
	}

	template <typename EventClass, typename Type, typename, typename>
	void EventService::Unsubscribe(Type a_eventType, CustomEventFunction<EventClass> a_callback)
	{
		Get().UnsubscribeImpl(static_cast<EventType>(a_eventType), std::forward<CustomEventFunction<EventClass>>(a_callback));
	}

	/* -- Subscribe implementation */

	template<typename EventClass>
	EventService::EventHandle EventService::SubscribeImpl(EventType a_eventType, CustomEventFunction<EventClass> a_callback, bool a_hasPriority)
	{
		//We reinterpret the custom function back to the base event function allowing use to store it.
		EventFunction& func = *reinterpret_cast<EventFunction*>(&a_callback);
		
		EventHandle handle;
		//If the subscriber prepend it to the list.
		if (a_hasPriority)
		{	
			handle = m_dispatcher.prependListener(a_eventType, func);
		}
		else
		{
			handle = m_dispatcher.appendListener(a_eventType, func);
		}

		if (m_eventBookList.find(a_eventType) != m_eventBookList.end())
		{
			m_eventBookList[a_eventType].emplace_back(func, handle);
		}
		else
		{
			std::vector<std::pair<EventFunction, EventHandle>> callbacks{ std::make_pair( func, handle ) };
			m_eventBookList.insert(std::make_pair(a_eventType, callbacks));
		}
		return handle;
	}

	/* -- Unsubscribe implementation */

	template<typename EventClass>
	void EventService::UnsubscribeImpl(EventType a_eventType, CustomEventFunction<EventClass> a_callback)
	{
		if(m_eventBookList.find(a_eventType) != m_eventBookList.end())
		{
			auto& callbacks = m_eventBookList[a_eventType];
			for(auto it = callbacks.begin(); it != callbacks.end(); ++it)
			{
				if(it->first.target<EventCallback>() == a_callback.template target<EventCallback>())
				{
					UnsubscribeImpl(a_eventType, it->second);
					m_eventBookList[a_eventType].erase(it);
					break;
				}
			}

			if(callbacks.empty())
			{
				m_eventBookList.erase(a_eventType);
			}
		}
		else
		{
			CE_CORE_ASSERT_M(false, "[EventService] Couldn't Unsubscribe from event({0}) as you're probably not subscribed to it!", a_eventType);
		}
	}
}
