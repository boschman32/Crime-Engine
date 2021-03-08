#include "cepch.h"
#include "Core/Input/InputManager.h"
#include "Core/Input/KeyEvent.h"
#include "Utils/ServiceManager.h"

InputManager* InputManager::ms_instance;

InputManager* InputManager::Get()
{
	if (ms_instance == nullptr) {
		ms_instance = ce::ServiceManager::GetService<InputManager>();
	}
	return ms_instance;
}

KeyEvent InputManager::GetNextKeyEvent()
{
	if (!m_keyBuffer.empty()) 
	{
		KeyEvent nextKeyEvent = m_keyBuffer.front();
		m_keyBuffer.pop();
		return nextKeyEvent;
	}
	return KeyEvent();
}

bool InputManager::IsKeyDown(Keys a_keyCode) 
{
	return Get()->m_keyboard.m_keyStates[int(a_keyCode)];
}

bool InputManager::IsButtonDown(JoystickButtons a_buttonCode) 
{
	return Get()->m_joystick.m_joystickStates[int(a_buttonCode)];
}

ce::Vector2 InputManager::GetThumbStickValue(JoystickThumb a_thumbStick) 
{
	ce::Vector2 returnVec;
	returnVec.x = Get()->m_joystick.m_joystickValues[int(a_thumbStick == JoystickThumb::ThumbL ? JoystickAxis::LThumbHorizontal : JoystickAxis::RThumbHorizontal)];
	returnVec.y = Get()->m_joystick.m_joystickValues[int(a_thumbStick == JoystickThumb::ThumbL ? JoystickAxis::LThumbVertical : JoystickAxis::RThumbVertical)];
	return returnVec;
}

float InputManager::GetJoystickAxis(JoystickBack a_backTrigger) 
{
	return Get()->m_joystick.m_joystickValues[int(a_backTrigger == JoystickBack::BackL ? JoystickAxis::L2 : JoystickAxis::R2)];
}

bool InputManager::IsMouseButtonDown(MouseKeys a_keyCode) 
{
	return Get()->m_mouse.m_mouseButtonStates[int(a_keyCode)];
}

ce::Vector2 InputManager::GetMousePosition() 
{
	ce::Vector2 returnVec;
	returnVec.x = Get()->m_mouse.m_mouseValues[int(MouseAxis::PositionHorizontal)];
	returnVec.y = Get()->m_mouse.m_mouseValues[int(MouseAxis::PositionVertical)];
	return returnVec;
}

float InputManager::GetMouseScrollDelta() 
{
	return Get()->m_mouse.m_mouseValues[int(MouseAxis::ScrollDelta)];
}

void InputManager::OnCreate() 
{
	Init();
}

void InputManager::OnDestroy() 
{

}

bool InputManager::OnKeyChange(Keys a_scanCode, bool a_isPressed)
{
	if (IsKeyDown(a_scanCode) == a_isPressed && !m_autoRepeat) 
	{
		return false;
	}
	m_keyboard.m_keyStates[int(a_scanCode)] = a_isPressed;
	m_keyBuffer.push(KeyEvent(int(a_scanCode), 0, a_isPressed ? KeyType::Pressed : KeyType::Released));
	return true;
}

bool InputManager::OnMouseButtonChange(MouseKeys a_mouseCode, bool a_isPressed) 
{
	if (IsMouseButtonDown(a_mouseCode) == a_isPressed) 
	{
		return false;
	}
	m_mouse.m_mouseButtonStates[int(a_mouseCode)] = a_isPressed;
	m_keyBuffer.push(KeyEvent(int(a_mouseCode), 1, a_isPressed ? KeyType::Pressed : KeyType::Released));
	return true;
}

bool InputManager::OnMousePositionChange(ce::Vector2 a_newMousePos) 
{
	if (m_mouse.m_mouseValues[int(MouseAxis::PositionHorizontal)] == a_newMousePos.x && m_mouse.m_mouseValues[int(MouseAxis::PositionVertical)] == a_newMousePos.y && !m_autoRepeat)
	{
		return false;
	}
	m_mouse.m_mouseValues[int(MouseAxis::PositionHorizontal)] = a_newMousePos.x;
	m_mouse.m_mouseValues[int(MouseAxis::PositionVertical)] = a_newMousePos.y;
	m_keyBuffer.push(KeyEvent(0, 1, a_newMousePos));
	return true;
}

bool InputManager::OnMouseScrollDeltaChange(float a_mouseDelta) 
{
	if (m_mouse.m_mouseValues[int(MouseAxis::ScrollDelta)] == a_mouseDelta && !m_autoRepeat) 
	{
		return false;
	}
	m_mouse.m_mouseValues[int(MouseAxis::ScrollDelta)] = a_mouseDelta;
	m_keyBuffer.push(KeyEvent(0, 1, a_mouseDelta));
	return true;
}

bool InputManager::OnButtonChange(JoystickButtons a_buttonCode, bool a_isPressed)
{
	if (IsButtonDown(a_buttonCode) == a_isPressed && !m_autoRepeat) 
	{
		return false;
	}
	m_joystick.m_joystickStates[int(a_buttonCode)] = a_isPressed;
	m_keyBuffer.push(KeyEvent(int(a_buttonCode), 2, a_isPressed ? KeyType::Pressed : KeyType::Released));
	return true;
}

bool InputManager::OnThumbstickChange(JoystickThumb a_thumbStick, ce::Vector2 a_newThumbPos) 
{
	ce::Vector2 currThumbValue = GetThumbStickValue(a_thumbStick);
	if (currThumbValue == a_newThumbPos && !m_autoRepeat)
	{
		return false;
	}
	m_joystick.m_joystickValues[a_thumbStick == JoystickThumb::ThumbL ? int(JoystickAxis::LThumbHorizontal) : int(JoystickAxis::RThumbHorizontal)] = a_newThumbPos.x;
	m_joystick.m_joystickValues[a_thumbStick == JoystickThumb::ThumbL ? int(JoystickAxis::LThumbVertical) : int(JoystickAxis::RThumbVertical)] = a_newThumbPos.y;
	m_keyBuffer.push(KeyEvent(int(a_thumbStick), 2, a_newThumbPos));
	return true;
}

bool InputManager::OnJoystickBackChange(JoystickBack a_backButton, float a_axisValue) 
{
	float currAxis = GetJoystickAxis(a_backButton);
	if (currAxis == a_axisValue && !m_autoRepeat) 
	{
		return false;
	}
	m_joystick.m_joystickValues[a_backButton == JoystickBack::BackL ? int(JoystickAxis::L2) : int(JoystickAxis::R2)] = a_axisValue;
	m_keyBuffer.push(KeyEvent(int(a_backButton), 2, a_axisValue));
	return true;
}

bool InputManager::IsAutorepeatOn() const 
{
	return m_autoRepeat;
}

void InputManager::SetAutorepeat(bool a_onOff) 
{
	m_autoRepeat = a_onOff;
}

void InputManager::ClearKeys() 
{
	m_keyBuffer = std::queue<KeyEvent>();
	m_keyboard.m_keyStates.reset();
	m_mouse.m_mouseButtonStates.reset();
	m_joystick.m_joystickStates.reset();
}
