#include "cepch.h"
#include "Core/Input/EventHandler.h"
#include "Core/Input/KeyEvent.h"

int EventHandler::ms_currId = 0;

EventHandler::EventHandler(const std::function<void(const KeyEvent&)>& a_func) : m_func(a_func)
{
	m_id = ++ms_currId;
}

void EventHandler::operator()(const KeyEvent& a_notifyKey) const
{
	m_func(a_notifyKey);
}

bool EventHandler::operator==(EventHandler& a_otherHandle) const
{
	return this->m_id == a_otherHandle.m_id;
}

bool EventHandler::operator!=(std::nullptr_t) const
{
	return this->m_func != nullptr;
}
