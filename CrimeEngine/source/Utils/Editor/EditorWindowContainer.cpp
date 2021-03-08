#include "cepch.h"
#include "Utils/Editor/EditorWindowContainer.h"

namespace ce
{
	namespace editor
	{
		EditorWindowContainer::EditorWindowContainer(std::string a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative, ImGuiWindowFlags a_flags)
			: IEditorWindow(a_windowName, a_pos, a_size, a_relative, a_flags)
		{

		}

		void EditorWindowContainer::Update(float a_deltaTime)
		{
			for(auto &c : m_children)
			{
				c->Update(a_deltaTime);
			}
		}

		void EditorWindowContainer::DrawContext()
		{
			if(ImGui::BeginTabBar(m_name.c_str()))
			{
				for(auto &c : m_children)
				{
					if(ImGui::BeginTabItem(c->GetName().c_str()))
					{
						c->DrawContext();
						ImGui::EndTabItem();
					}
				}
				ImGui::EndTabBar();
			}
		}

		void EditorWindowContainer::AddChild(std::unique_ptr<IEditorWindow> a_child)
		{
			a_child->Init();
			m_children.push_back(std::move(a_child));
		}
	}
}