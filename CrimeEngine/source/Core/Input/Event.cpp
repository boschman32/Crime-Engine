#include "cepch.h"
#include "Core/Input/Event.h"
#include "Core/Input/KeyEvent.h"

void Event::NotifyHandlers(const KeyEvent& a_notifyKey)
{
	std::vector<std::shared_ptr<EventHandler>>::iterator handle = m_handlers.begin();
	for (; handle != m_handlers.end(); ++handle)
	{
		if (*handle != nullptr) {
			(*(*handle))(a_notifyKey);
		}
	}
}

void Event::AddHandler(EventHandler& a_newHandler)
{
	m_handlers.push_back(std::make_shared<EventHandler>(a_newHandler));
}

void Event::AddHandler(std::function<void(const KeyEvent&)> a_newHandler)
{
	m_handlers.push_back(std::make_shared<EventHandler>(a_newHandler));
}

void Event::RemoveHandler(EventHandler& a_handlerToRemove)
{
	std::vector<std::shared_ptr<EventHandler>>::iterator handle = m_handlers.begin();
	for (; handle != m_handlers.end(); ++handle)
	{
		if (*(*handle) == a_handlerToRemove) {
			m_handlers.erase(handle);
		}
	}
}

void Event::operator()(const KeyEvent& a_notifyKey)
{
	NotifyHandlers(a_notifyKey);
}

Event& Event::operator+=(EventHandler& a_newHandler)
{
	AddHandler(a_newHandler);
	return *this;
}

Event& Event::operator+=(std::function<void(const KeyEvent&)> a_newHandler)
{
	AddHandler(EventHandler(a_newHandler));
	return *this;
}

Event& Event::operator-=(EventHandler& a_handlerToRemove)
{
	RemoveHandler(a_handlerToRemove);
	return *this;
}