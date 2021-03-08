#include "cepch.h"
#include "Utils/Editor/EditorConsole.h"

namespace ce
{
	namespace editor
	{
		EditorConsole::EditorConsole(const std::string& a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative, ImGuiWindowFlags a_flags)
			: IEditorWindow(a_windowName, a_pos, a_size, a_relative, a_flags) {}

		void EditorConsole::DrawContext()
		{
			// Toggle which loggers to display
			if (ImGui::TreeNode("Loggers"))
			{
				for (auto& logger : Logger::GetAllLoggers())
				{
					bool loggerToggled = logger.second->should_backtrace();
					if (ImGui::Checkbox(logger.first.c_str(), &loggerToggled))
					{
						if (loggerToggled)
						{
							logger.second->enable_backtrace(100);
						}
						else if (!loggerToggled)
						{
							logger.second->disable_backtrace();
						}
					}
				}
				ImGui::TreePop();
			}

			// Toggle which log level to use
			const char* logLevels[] = { "Debug", "Info", "Trace", "Warn", "Error", "Critical", "Off" };

			if (ImGui::Combo("LogLevels", &m_currentItem, logLevels, IM_ARRAYSIZE(logLevels)))
			{
				std::string newLogLevel = std::string(logLevels[m_currentItem]);
				Logger::SetGlobalLogLevel(ConvertStringToLogLevel(newLogLevel));
			}

			// Print messages
			std::vector<std::string> messages = ce::Logger::GetMessages();
			for (size_t i = messages.size() - 1; i > 0 ; i--)
			{
				// Get the timestamp
				const size_t startTimeStamp = messages[i].find("[");
				const size_t endTimeStamp = messages[i].find_first_of("]");

				std::string timeStamp;
				if(startTimeStamp != std::string::npos || endTimeStamp != std::string::npos)
				{
					timeStamp = messages[i].substr(startTimeStamp, endTimeStamp - startTimeStamp) + "]";
				}
				

				// Get the logger
				const size_t startLogger = messages[i].find("(");
				const size_t endLogger = messages[i].find_first_of(")");

				std::string logger;
				if(startLogger != std::string::npos || endLogger != std::string::npos)
				{
					logger = messages[i].substr(startLogger, endLogger - startLogger) + ")";
				}
				
				std::string message = messages[i].substr(messages[i].find(")") + 1);

				ImGui::TextColored(ImVec4(1.f, 0.0f, 1.f, 1.f), "%s", timeStamp.c_str());
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(0, 1.f, 0, 1.f), "%s", logger.c_str());
				ImGui::SameLine();
				ImGui::Text("%s", message.c_str());
			}
		}

		Logger::LogLevel EditorConsole::ConvertStringToLogLevel(const std::string& a_logLevelName)
		{
			if (a_logLevelName == "Debug")
			{
				return Logger::LogLevel::Debug;
			}

			if (a_logLevelName == "Info")
			{
				return Logger::LogLevel::Info;
			}

			if (a_logLevelName == "Trace")
			{
				return Logger::LogLevel::Trace;
			}

			if (a_logLevelName == "Warn")
			{
				return Logger::LogLevel::Warn;
			}

			if (a_logLevelName == "Error")
			{
				return Logger::LogLevel::Error;
			}

			if (a_logLevelName == "Critical")
			{
				return Logger::LogLevel::Critical;
			}

			if (a_logLevelName == "Off")
			{
				return Logger::LogLevel::Off;
			}
			
			return Logger::LogLevel::Debug;
		}
	}
}