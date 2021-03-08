#include "cepch.h"
#include "Utils/Editor/EditorMenuBar.h"

#include "Core/Application.h"

#include "Scenes/SceneManager.h"
#include "Scenes/IMapScene.h"
#include "Utils/FileSystem.h"

namespace ce
{
	namespace editor
	{
		EditorMenuBar::EditorMenuBar(const std::string& a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative, ImGuiWindowFlags a_flags)
			: IEditorWindow(a_windowName, a_pos, a_size, a_relative, a_flags), m_sceneManager(nullptr), m_levelService(nullptr), m_renderService(nullptr)
		{

		}

		void EditorMenuBar::Init()
		{
			m_sceneManager = ServiceManager::GetService<SceneManager>();
			m_levelService = ServiceManager::GetService<LevelService>();
			m_renderService = ServiceManager::GetService<RenderService>();
		}

		void EditorMenuBar::DrawContext()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					IScene* currentScene = m_sceneManager->GetCurrentScene();

					if (currentScene != nullptr)
					{
						auto mapScene = dynamic_cast<IMapScene*>(currentScene);
						if (ImGui::BeginMenu("Open map...", 
							mapScene != nullptr && (Application::Get().GetAppState() != AppState::Playing && Application::Get().GetAppState() != AppState::Paused) ))
						{
							m_levelService->LookForAvailableMaps();

							auto& maps = m_levelService->GetAvailableMaps();
							LevelMap* currentMap = m_levelService->GetCurrentLoadedMap();

							for (auto &m : maps)
							{
								const bool isSelected = (currentMap != nullptr && FileSystem::ComparePath(m, currentMap->GetPath()));
								if (ImGui::MenuItem(m.c_str(), nullptr, isSelected))
								{
									//If this is the same map.
									if (isSelected)
									{
										currentMap->Reload();
										break;
									}

									mapScene->SelectMap(m);
									//Reload the scene.
									m_sceneManager->SwitchScene<IMapScene>(currentScene->GetSceneName());
									break;
								}
							}
							ImGui::EndMenu();
						}
					}
					ImGui::EndMenu();
				}

				const bool isPlaying = Application::Get().GetAppState() == AppState::Playing;
				const bool isPaused = Application::Get().GetAppState() == AppState::Paused;
				ImGui::SameLine((ImGui::GetWindowContentRegionWidth() / 2.f) - 25.f);

				const bool disablePlayButton = isPlaying || isPaused;
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disablePlayButton);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disablePlayButton ? 0.5f : 1.f));
				if (ImGui::Button(ICON_FA_PLAY))
				{
					Application::Get().Play();
				}
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();

				const bool disablePauseButton = !isPlaying && !isPaused;
				if (isPaused)
				{
					ImGui::PushStyleColor(ImGuiCol_Button, { 0.25f, 0.25f, 0.8f, 1.f });
				}
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disablePauseButton);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disablePauseButton ? 0.5f : 1.f));
				if (ImGui::Button(ICON_FA_PAUSE))
				{
					Application::Get().Pause();
				}
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				if (isPaused)
				{
					ImGui::PopStyleColor();
				}

				const bool disableStopButton = !isPlaying && !isPaused;

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disableStopButton);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disableStopButton ? 0.5f : 1.f));
				ImGui::PushStyleColor(ImGuiCol_Text, disableStopButton ? ImVec4(1.f, 1.f, 1.f, 1.f) : ImVec4(0.8f, 0.25f, 0.2f, 1.f));
				if (ImGui::Button(ICON_FA_STOP))
				{
					Application::Get().Stop();
				}
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();

				const bool disableProfileButton = isPlaying || isPaused;

				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, disableProfileButton);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * (disableProfileButton ? 0.5f : 1.f));
				if (ImGui::Button(ICON_FA_TACHOMETER_ALT))
				{
					Application::Get().Play(true);
				}
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					{
						ImGui::SetTooltip("Starts the game and begins a capture for Optick Profiling. \nStopping the game will save the capture and save it in the project folder.");
					}
					ImGui::EndTooltip();
				}

				if (isPlaying || isPaused)
				{
					ImGui::LabelText("%s", "FPS: %f", Application::Get().GetFps());
				}
#if DEBUG_GRID
				LevelMap* curMap = m_levelService->GetCurrentLoadedMap();

				if (curMap != nullptr)
				{
					const float checkBoxOffset = 100.f;
					ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - checkBoxOffset);
					bool enabled = curMap->GetGrid().IsDebugEnabled();
					if (ImGui::Checkbox(" Show grid", &enabled))
					{
						curMap->GetGrid().SetEnableDebug(enabled);
					}
				}
#endif		
				ImGui::EndMainMenuBar();
			}
		}
	}
}
