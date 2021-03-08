#pragma once
#include "Event.h"
#include <vector>

class KeyAction {
public:
	KeyAction() = default;
	KeyAction(const char* a_name, const int& a_id, const int& a_inputType, const int& a_key, std::function<void(const KeyEvent&)> a_actionEvent) :  m_name(a_name), m_inputType(a_inputType), m_id(a_id)
	{
		m_keys.push_back(a_key);
		m_keyEvent += a_actionEvent;
	}
	const char* m_name = "Error";
	int m_inputType = -1;
	int m_id = 0;
	std::vector<int> m_keys;
	Event m_keyEvent;
	bool m_active = true;
};