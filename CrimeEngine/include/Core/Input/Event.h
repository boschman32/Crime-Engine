#pragma once
#include <memory>
#include <functional>
#include <vector>
#include "EventHandler.h"

class KeyEvent;

class Event
{
public:
	void NotifyHandlers(const KeyEvent& a_notifyKey);
	void AddHandler(EventHandler& a_newHandler);
	void AddHandler(std::function<void(const KeyEvent&)> a_newHandler);
	void RemoveHandler(EventHandler& a_handlerToRemove);
	void operator()(const KeyEvent& a_notifyKey);

	Event& operator+=(EventHandler& a_newHandler);
	Event& operator+=(std::function<void(const KeyEvent&)> a_newHandler);
	Event& operator-=(EventHandler& a_handlerToRemove);

private:
	std::vector<std::shared_ptr<EventHandler>> m_handlers;

};

