#pragma once
#include <functional>

class KeyEvent;

class EventHandler
{
public:
	EventHandler() = default;
	EventHandler(const std::function<void(const KeyEvent&)>& a_func);
	~EventHandler() = default;

	void operator()(const KeyEvent& a_notifyKey) const;
	bool operator==(EventHandler& a_otherHandle) const;
	bool operator!=(std::nullptr_t) const;

	std::function<void(const KeyEvent&)> m_func;
	int m_id { 0 };
private:
	static int ms_currId;
};

