#pragma once
#include "IEditorWindow.h"

namespace ce
{
	namespace editor
	{
		class EditorConsole : public IEditorWindow
		{
		public:

			EditorConsole(const std::string& a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative = true, ImGuiWindowFlags a_flags = ImGuiBackendFlags_None);

			void DrawContext() override;

		private:

			Logger::LogLevel ConvertStringToLogLevel(const std::string& a_logLevelName);
			int m_currentItem = 0;
		};
	}
}
