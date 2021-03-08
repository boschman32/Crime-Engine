#include "expch.h"
#include "ExampleEvents.h"

#include "Events/EventService.h"
#include "Utils/ServiceManager.h"

#include <sstream>

namespace ce
{
	namespace examples
	{
		/*
		 * If you want your own custom events to be used instead of using the core events
		 * it is important to grab the underlying type and to make sure the first item begins at the Last enum value
		 * of the core EventType.
		 */
		enum class CustomEventType : std::underlying_type_t<EventType>
		{
			TestEventOne = std::underlying_type_t<EventType>(EventType::Last),
			TestEventTwo,
			TestEventThree,
			TestEventFour,
			Last
		};

		/*
		 * Continuing the enum it is important to grab again the last value from the previous list.
		 */
		enum CustomEventTypeTwo : std::underlying_type_t<EventType>
		{
			TestEventFive = std::underlying_type_t<EventType>(CustomEventType::Last),
			TestEventSix,
			TestEventSeven,
			TestEventEight,
			Last
		};
		
		class TestEvent : public IEvent
		{
		public:
			TestEvent(int a_argument)
				: m_argument(a_argument) { };

			int GetArgument() const { return m_argument; }

			//Converting event to string (for logging/debug purpose).
			std::string ToString() const override
			{
				std::stringstream ss;
				ss << "[Test event]: (" << m_argument << ")";
				return ss.str();
			}

			//Generate the class functions for the type
			EVENT_CLASS_TYPE(CustomEventType, TestEventTwo);

			//Generate the class functions for the category.
			EVENT_CLASS_CATEGORY(EventCategory::EventCategoryCustom)
		private:
			int m_argument { 0 };
		};

		void ExampleEvents::RunExample()
		{
			//Will be called from application no need to make the service yourself.
			ServiceManager::MakeService<EventService>();

			/*
			 * How to use the EventService
			 *
			 * EventService supports static calls using EventService::Function();
			 *
			 * But is also able to be used through the ServiceManager if needed (although this beats the purpose).
			 *
			 * ServiceManager::GetService<EventService>().Function();
			 */

			/* --- Callback use --- */
			
			//Subscribe no handle used
			{
				EventService::Subscribe<TestEvent>(CustomEventType::TestEventTwo, BIND_EVENT_CALLBACK(ExampleEvents::OnTestEvent));
			}

			//Let event happen
			{
				TestEvent evt { 1337 };
				EventService::Dispatch(evt);
			}

			//Unsubscribe using callback
			{
				EventService::Unsubscribe<TestEvent>(CustomEventType::TestEventTwo, BIND_EVENT_CALLBACK(ExampleEvents::OnTestEvent));
			}

			/* --- Handle use --- */

			//Subscribe handle used
			EventService::EventHandle handle; 
			{
				handle = EventService::Subscribe<TestEvent>(CustomEventType::TestEventTwo, BIND_EVENT_CALLBACK(ExampleEvents::OnTestEvent));
			}

			//Let event happen
			{
				TestEvent evt { 9183123 };
				EventService::Dispatch(evt);
			}

			//Unsubscribe using the handle.
			{
				EventService::Unsubscribe(CustomEventType::TestEventTwo, handle);
			}

			//Send the last event (no one should be listening to this).
			{
				TestEvent evt { 999 };
				EventService::Dispatch(evt);
			}
		}

		void ExampleEvents::OnTestEvent(TestEvent& a_event) const
		{
			CE_CORE_INFO("Test event argument: {0}", a_event.GetArgument());
			CE_CORE_CRITICAL("Test event has happened: {0}", a_event);
		}

	}
}
