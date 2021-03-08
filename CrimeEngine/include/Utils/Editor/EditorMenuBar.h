#pragma once
#include "IEditorWindow.h"

namespace ce
{
	class SceneManager;
	class LevelService;
	class RenderService;
	
	namespace editor
	{
		class EditorMenuBar : public IEditorWindow
		{
		public:

			EditorMenuBar(const std::string& a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative = true, ImGuiWindowFlags a_flags = ImGuiBackendFlags_None);

			void Init() override;
			void DrawContext() override;
		private:
			SceneManager* m_sceneManager;
			LevelService* m_levelService;
			RenderService* m_renderService;
		};
	}
}
