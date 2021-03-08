#pragma once
#include "Core/Math.h"

namespace cr
{
	enum class RenderAPI;

	class Window
	{
	public:
		static Window* CreateNewWindow(RenderAPI, int, int);
		virtual ~Window() = default;
		virtual void SetTitle(const std::string&) = 0;
		virtual bool Peak() = 0;
		virtual void Resize() = 0; // give new size
		bool IsActive() const { return m_isActive; }
		const ce::Vector2& GetSize() const { return m_size; }
		virtual void ShowMouse(bool) = 0;
	protected:
		bool m_isActive { true };
		ce::Vector2 m_size { 0.f, 0.f};
	};
}