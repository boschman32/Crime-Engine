#pragma once
#include "IEditorWindow.h"

namespace ce
{
	namespace editor
	{
		class EditorWindowContainer : public IEditorWindow
		{
		public:
			EditorWindowContainer(std::string a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative = true, ImGuiWindowFlags a_flags = ImGuiBackendFlags_None);

			void Update(float) override;
			void DrawContext() override;
			
			void AddChild(std::unique_ptr<IEditorWindow> a_child);
		private:
			std::vector<std::unique_ptr<IEditorWindow>> m_children;
		};
	}
}
