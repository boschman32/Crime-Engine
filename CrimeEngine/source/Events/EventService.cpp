#include "cepch.h"
#include "Events/EventService.h"

namespace ce
{
	EventService* EventService::ms_instance{ nullptr };

	EventService::EventService()
	{
		CE_CORE_ASSERT_M(ms_instance == nullptr, "Event service is already created!");

		ms_instance = this;
	}

	void EventService::OnCreate()
	{

	}

	void EventService::OnDestroy()
	{
		m_eventBookList.clear();
		ms_instance = nullptr;
	}

	void EventService::UnsubscribeImpl(EventType a_eventType, EventHandle a_handle)
	{
		m_dispatcher.removeListener(a_eventType, a_handle);
	}

	void EventService::DispatchImpl(IEvent& a_event) const
	{
		m_dispatcher.dispatch(a_event);
	}
}
