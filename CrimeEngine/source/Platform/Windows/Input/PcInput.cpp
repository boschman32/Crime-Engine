#include "cepch.h"
#include "Core/Input/InputManager.h"
#include "Core/Input/KeyEnums.h"

void InputManager::Init() {
	FindController();
}

void InputManager::Update()
{
	//Tnx Katy hope you are still alive
	if (XInputGetState(m_joystick.m_controllerId, &m_joystick.m_controllerState) != ERROR_SUCCESS) {
		FindController();
		return;
	}
	
	OnButtonChange(JoystickButtons::A, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
	OnButtonChange(JoystickButtons::B, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_B) != 0);
	OnButtonChange(JoystickButtons::Y, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0);
	OnButtonChange(JoystickButtons::X, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0);

	OnButtonChange(JoystickButtons::Up, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
	OnButtonChange(JoystickButtons::Down, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
	OnButtonChange(JoystickButtons::Left, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0);
	OnButtonChange(JoystickButtons::Right, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0);

	OnButtonChange(JoystickButtons::Start, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);
	OnButtonChange(JoystickButtons::Back, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0);

	OnButtonChange(JoystickButtons::L3, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0);
	OnButtonChange(JoystickButtons::R3, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0);

	OnButtonChange(JoystickButtons::LB, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
	OnButtonChange(JoystickButtons::RB, (m_joystick.m_controllerState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);

	//Checking of the two back triggers of the controllers
	float leftBackTrigger = float(m_joystick.m_controllerState.Gamepad.bLeftTrigger) / 255;
	OnJoystickBackChange(JoystickBack::BackL, leftBackTrigger);

	float rightBackTrigger = float(m_joystick.m_controllerState.Gamepad.bRightTrigger) / 255;
	OnJoystickBackChange(JoystickBack::BackR, rightBackTrigger);

	//Checking the position of the two thumbsticks
	ce::Vector2 normL;
	normL.x = fmaxf(-1, float(m_joystick.m_controllerState.Gamepad.sThumbLX) / 32767);
	normL.y = fmaxf(-1, float(m_joystick.m_controllerState.Gamepad.sThumbLY) / 32767);

	normL.x = (abs(normL.x) < m_joystick.m_deadZone ? 0 : normL.x);
	normL.y = (abs(normL.y) < m_joystick.m_deadZone ? 0 : normL.y);

	OnThumbstickChange(JoystickThumb::ThumbL, normL);

	ce::Vector2 normR;
	normR.x = fmaxf(-1, float(m_joystick.m_controllerState.Gamepad.sThumbRX) / 32767);
	normR.y = fmaxf(-1, float(m_joystick.m_controllerState.Gamepad.sThumbRY) / 32767);

	normR.x = (abs(normR.x) < m_joystick.m_deadZone ? 0 : normR.x);
	normR.y = (abs(normR.y) < m_joystick.m_deadZone ? 0 : normR.y);

	OnThumbstickChange(JoystickThumb::ThumbR, normR);
}

bool InputManager::FindController()
{
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		ZeroMemory(&m_joystick.m_controllerState, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &m_joystick.m_controllerState) == ERROR_SUCCESS) {
			m_joystick.m_controllerId = i;
			return true;
		}
	}
	return false;
}