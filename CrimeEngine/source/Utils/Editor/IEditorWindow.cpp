#include "cepch.h"
#include "Utils/Editor/IEditorWindow.h"

namespace ce
{
	namespace editor
	{
		IEditorWindow::IEditorWindow(std::string a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative, ImGuiWindowFlags a_flags)
			: m_name(std::move(a_windowName)),
			m_position(a_pos),
			m_size(a_size),
			m_flags(a_flags),
			m_isRelative(a_relative)
		{

		}

		void IEditorWindow::RenderWindow()
		{
			if(!m_isVisible)
			{
				return;
			}
			
			if (m_isRelative)
			{
				ImGuiIO& io = ImGui::GetIO();
				ImVec2 relativeSize;
				relativeSize.x = m_size.x * io.DisplaySize.x;
				relativeSize.y = m_size.y * io.DisplaySize.y;
				ImGui::SetNextWindowSize(relativeSize);

				ImVec2 relativePos;
				relativePos.x = m_position.x * io.DisplaySize.x;
				relativePos.y = m_position.y * io.DisplaySize.y;
				ImGui::SetNextWindowPos(relativePos);
			}

			ImGui::Begin(m_name.c_str(), nullptr, m_flags);
			m_windowId = ImGui::GetCurrentWindow()->ID;
			DrawContext();
			ImGui::End();
		}
	}
}