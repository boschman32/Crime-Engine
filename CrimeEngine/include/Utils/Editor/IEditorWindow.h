#pragma once
#include "imgui/imgui.h"

namespace ce
{
	namespace editor
	{
		class IEditorWindow
		{
		public:
			IEditorWindow(std::string a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative = true, ImGuiWindowFlags a_flags = ImGuiBackendFlags_None);
			virtual ~IEditorWindow() = default;

			void SetWindowFlags(ImGuiWindowFlags a_flags) { m_flags = a_flags; };
			
			virtual void Init() { };
			virtual void Update(float) { };
			virtual void DrawContext() = 0;

			void RenderWindow();

			const std::string& GetName() const { return m_name; }
			bool IsVisible() const { return m_isVisible; };
			bool IsRelative() const { return m_isRelative; };
			ImGuiID GetImGuiWindowId() const { return m_windowId; }
			
		protected:
			std::string m_name { "" };
			bool m_isVisible { true };

			ImVec2 m_position { 0.f, 0.f };
			ImVec2 m_size { 0.f, 0.f };
			ImGuiWindowFlags m_flags;
			ImGuiID m_windowId { 0 };
			
			bool m_isRelative = false;
		};
	}
}