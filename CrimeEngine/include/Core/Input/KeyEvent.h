#pragma once
#include "Core/Math.h"

enum EventType {
	KeyPress,
	AnalogMove,
	VectorMove,
	Invalid
};

enum KeyType {
	Pressed,
	Released
};

class KeyEvent {
public:
	KeyEvent() = default;
	KeyEvent(int a_code, int a_inputType, KeyType a_state) : m_type(EventType::KeyPress), m_keyCode(a_code), m_inputType(a_inputType),  m_state(a_state) {}
	KeyEvent(int a_code, int a_inputType, float a_analogValue) : m_type(EventType::AnalogMove), m_keyCode(a_code), m_inputType(a_inputType),  m_anaValue(a_analogValue) {}
	KeyEvent(int a_code, int a_inputType, ce::Vector2 a_vecValue) : m_type(EventType::VectorMove), m_keyCode(a_code), m_inputType(a_inputType),  m_vecValue(a_vecValue) {}

	~KeyEvent() = default;

	EventType GetEventType() const {
		return m_type;
	}

	int GetInputType() const {
		return m_inputType;
	}

	float GetAnalogValue() const {
		return m_anaValue;
	}

	ce::Vector2 GetVec2Value() const {
		return m_vecValue;
	}

	bool IsPressed() const
	{
		return m_state == KeyType::Pressed;
	}
	bool IsReleased() const
	{
		return m_state == KeyType::Released;
	}
	int GetScanCode() const
	{
		return m_keyCode;
	}
private:
	EventType m_type = EventType::Invalid;
	//For KeyPress
	int m_keyCode = -1;
	int m_inputType = -1;
	KeyType m_state = KeyType::Pressed;

	//For AnalogMove
	float m_anaValue = 0.0f;

	//For VectorMove
	ce::Vector2 m_vecValue = {0.f,0.f};
};