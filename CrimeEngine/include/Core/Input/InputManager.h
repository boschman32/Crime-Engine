#pragma once
#include "KeyEvent.h"
#include "Utils/IService.h"
#include "Core/Math.h"
#include "Core/Input/KeyEnums.h"

#include <queue>
#include <bitset>
#include <map>

struct Keyboard
{
	std::bitset<256u> m_keyStates;
};

struct Mouse
{
	std::bitset<3u> m_mouseButtonStates;
	float m_mouseValues[3];
};

struct Joystick
{
	std::bitset<15u> m_joystickStates;
	int m_controllerId = -1;
	float m_joystickValues[6];
	float m_deadZone = 0.2f;
#ifdef PLATFORM_WINDOWS
	XINPUT_STATE m_controllerState;
#endif
};

namespace cr
{
	class WinWindow;
}

class InputManager : public ce::IService
{
	friend class cr::WinWindow;
public:

	InputManager() = default;
	~InputManager() override = default;

	static InputManager* Get();
	KeyEvent GetNextKeyEvent();

	static bool IsKeyDown(Keys a_keyCode);
	static bool IsButtonDown(JoystickButtons a_buttonCode);
	static ce::Vector2 GetThumbStickValue(JoystickThumb a_thumbStick);
	static float GetJoystickAxis(JoystickBack a_backTrigger);
	static bool IsMouseButtonDown(MouseKeys a_keyCode);
	static ce::Vector2 GetMousePosition();
	static float GetMouseScrollDelta();

	void OnCreate() override;
	void OnDestroy() override;

	bool IsAutorepeatOn() const;
	void SetAutorepeat(bool a_onOff);

	void ClearKeys();

	void Update();

private:

	static InputManager* ms_instance;

	void Init();
	bool FindController();

	bool OnKeyChange(Keys a_scanCode, bool a_isPressed);
	bool OnMouseButtonChange(MouseKeys a_mouseCode, bool a_isPressed);
	bool OnMousePositionChange(ce::Vector2 a_newMousePos);
	bool OnMouseScrollDeltaChange(float a_mouseDelta);
	bool OnButtonChange(JoystickButtons a_buttonCode, bool a_isPressed);
	bool OnThumbstickChange(JoystickThumb a_thumbStick, ce::Vector2 a_newThumbPos);
	bool OnJoystickBackChange(JoystickBack a_backButton, float a_axisValue);

	Keyboard m_keyboard;
	Mouse m_mouse;
	Joystick m_joystick;

	std::map<int, float> m_anaStates;
	std::map<int, std::pair<float, float>> m_vec2States;
	std::queue<KeyEvent> m_keyBuffer;

	bool m_autoRepeat = false;
};

