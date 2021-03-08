#include "cepch.h"

#include "Platform/Windows/WinWindow.h"
#include "Core/Input/InputManager.h"

#include <xinput.h>

#include "Events/ApplicationEvent.h"
#include "Events/EventService.h"

cr::WinWindow::WinWindow(int a_width, int a_height, const std::string& a_title)
{

	RECT wr;
	wr.left = 100;
	wr.right = a_width + wr.left;
	wr.top = 100;
	wr.bottom = a_height + wr.top;

	m_inputManager = InputManager::Get();

	m_windowHandle = CreateWindow(WindowClass::GetName(), a_title.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this);

	ShowWindow(m_windowHandle, SW_SHOWDEFAULT);
	m_isActive = true;
	m_size = glm::vec2(a_width, a_height);
}

cr::WinWindow::~WinWindow()
{
	DestroyWindow(m_windowHandle);
}

void cr::WinWindow::SetTitle(const std::string& a_title)
{

	SetWindowText(m_windowHandle, a_title.c_str());
}

bool cr::WinWindow::Peak()
{
	MSG msg;
	while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE) != 0)
	{
		if (msg.message == WM_QUIT)
		{
			m_isActive = false;
			return false;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	return true;
}

void cr::WinWindow::Resize()
{
	m_resizeWindow = false;
	RECT r;
	GetClientRect(m_windowHandle, &r);
	m_size.x = static_cast<float>(r.right - r.left);
	m_size.y = static_cast<float>(r.bottom - r.top);

	ce::WindowResizeEvent evt = { unsigned(m_size.x), unsigned(m_size.y) };
	ce::EventService::Dispatch(evt);
}

void cr::WinWindow::ShowMouse(bool a_active)
{
	ShowCursor(static_cast<int>(a_active));
}

LRESULT __stdcall cr::WinWindow::HandleMsgSetup(HWND a_hWnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
{
	if (a_msg == WM_NCCREATE)
	{
		//extract ptr to window class from creation data!
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(a_lParam);
		WinWindow* const pWnd = static_cast<WinWindow*>(pCreate->lpCreateParams);
		//Set WinAPI-managed user data to store window class ptr
		SetWindowLongPtr(a_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		//Set Message proc to normal handler in stead of the setup one!
		SetWindowLongPtr(a_hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&WinWindow::HandleMsgThunk));
		// forward message to window class handler
		return pWnd->HandleMsg(a_hWnd, a_msg, a_wParam, a_lParam);
	}
	return DefWindowProc(a_hWnd, a_msg, a_wParam, a_lParam);
}

LRESULT __stdcall cr::WinWindow::HandleMsgThunk(HWND a_hWnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
{
	WinWindow* const pWnd = reinterpret_cast<WinWindow*>(GetWindowLongPtr(a_hWnd, GWLP_USERDATA));

	//Handle the messages!
	return pWnd->HandleMsg(a_hWnd, a_msg, a_wParam, a_lParam);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND a_hWnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam); // NOLINT
LRESULT cr::WinWindow::HandleMsg(HWND a_hWnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam)
{
	// ImGui Input Handles
	if (ImGui_ImplWin32_WndProcHandler(a_hWnd, a_msg, a_wParam, a_lParam) != 0)
	{
		return 1;
	}
	float scrollDelta = 0.0f;
	switch (a_msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}
		case WM_SIZE :
		{ 

			int nWidth = LOWORD(a_lParam);
			int nHeight = HIWORD(a_lParam);

			m_size = glm::vec2(nWidth, nHeight);
			m_resizeWindow = true;
		}
		case WM_KILLFOCUS: 
		{
			m_inputManager->ClearKeys();
			break;
		}
		case WM_KEYDOWN:
		{
			// syskey is for system keys inputs like alt and f10 etc which aren't registered for KEYDOWN 
		case WM_SYSKEYDOWN:
			//Checks bit 30 to see if key was pressed before message sent or after!
			if ((a_lParam & 0x40000000) == 0 || m_inputManager->IsAutorepeatOn()) 
			{
				m_inputManager->OnKeyChange(static_cast<Keys>(a_wParam), true);
			}
			break;
		}
		case WM_KEYUP:
		{
			// syskey is for system keys inputs like alt and f10 etc which aren't registered for KEYUP 
		case WM_SYSKEYUP:
			m_inputManager->OnKeyChange(static_cast<Keys>(a_wParam), false);
			break;
		}
		//MOUSE IMPLEMENTATION
		case WM_MOUSEMOVE: {
			const auto mousePos = MAKEPOINTS(a_lParam); //NOLINT
			if (mousePos.x >= 0 && float(mousePos.x) < m_size.x && mousePos.y >= 0 && float(mousePos.y) < m_size.y) {
				m_inputManager->OnMousePositionChange({ mousePos.x, mousePos.y });
				SetCapture(a_hWnd);
			}
			else {
				if ((a_wParam & (MK_LBUTTON | MK_RBUTTON)) != 0u) {
					m_inputManager->OnMousePositionChange({mousePos.x, mousePos.y});
				}
				else {
					ReleaseCapture();
				}
			}
			break;
		}
		case WM_MOUSEWHEEL: {
			int wheelDelta = GET_WHEEL_DELTA_WPARAM(a_wParam);
			//120 is the speed the scroll wheel scrolls this is done because it allows for mouses with a quicker 
			scrollDelta = float(wheelDelta) / 120;
			break;
		}
		case WM_LBUTTONDOWN: {
			m_inputManager->OnMouseButtonChange(MouseKeys::LButton, true);
			break;
		}
		case WM_RBUTTONDOWN: {
			m_inputManager->OnMouseButtonChange(MouseKeys::RButton, true);
			break;
		}
		case WM_MBUTTONDOWN: {
			m_inputManager->OnMouseButtonChange(MouseKeys::Middle, true);
			break;
		}
		case WM_LBUTTONUP: {
			m_inputManager->OnMouseButtonChange(MouseKeys::LButton, false);
			break;
		}
		case WM_RBUTTONUP : {
			m_inputManager->OnMouseButtonChange(MouseKeys::RButton, false);
			break;
		}
		case WM_MBUTTONUP: {
			m_inputManager->OnMouseButtonChange(MouseKeys::Middle, false);
			break;
		}
	}
	m_inputManager->OnMouseScrollDeltaChange(scrollDelta);
	return DefWindowProc(a_hWnd, a_msg, a_wParam, a_lParam);
}

#pragma region WindowClass

const char * cr::WinWindow::WindowClass::GetName()
{
	return WndClassName;
}

HINSTANCE cr::WinWindow::WindowClass::GetInstance()
{
	return  ms_windowClass.m_hInstance;
}

cr::WinWindow::WindowClass cr::WinWindow::WindowClass::ms_windowClass;

cr::WinWindow::WindowClass::WindowClass()
: m_hInstance(nullptr)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	//Load image from resources
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	//Load image from resources
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);
}

cr::WinWindow::WindowClass::~WindowClass()
{
	UnregisterClass(WndClassName, GetInstance());
}


#pragma endregion