#include "cepch.h"
#include "Utils/Editor/EditorLayer.h"


#include "Core/Core.h"
#include "Core/ResourceManager.h"
#include "FontAwesome/FontAwesome.h"

#include "Utils/Editor/EditorMenuBar.h"
#include "Utils/Editor/EditorWindowContainer.h"
#include "Utils/ServiceManager.h"

namespace ce
{
	namespace editor
	{
		EditorLayer* EditorLayer::ms_instance = nullptr;
		
		EditorLayer::EditorLayer()
		{
			CE_CORE_ASSERT_M(ms_instance == nullptr, "Editor Layer already created!");
			ms_instance = this;
			
			// Menu bar
			m_editorWindows.insert(std::make_pair("MenuBar",
				std::make_unique<EditorMenuBar>("MenuBar",
					ImVec2(0.f, 0.f),
					ImVec2(1.f, 0.1f),
					true,
					ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking)
			));


			auto bottomContainer = std::make_unique<EditorWindowContainer>("BottomContainer",
				ImVec2(0.f, 0.7f),
				ImVec2(1.f, 0.3f),
				true,
				ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
			
			// Console
			bottomContainer->AddChild(std::make_unique<EditorConsole>("Console",
					ImVec2(0, 0),
					ImVec2(600.f, 250.f),
					false));
			
			m_editorWindows.insert(std::make_pair("BottomContainer", std::move(bottomContainer)));
		}

		void EditorLayer::OnAttach()
		{
			for (auto &w : m_editorWindows)
			{
				w.second->Init();
			}

			SetImGuiStyle();
		}

		void EditorLayer::OnDetach()
		{

		}

		void EditorLayer::OnUpdate(float a_deltaTime)
		{
			OPTICK_EVENT()
			for (auto &w : m_editorWindows)
			{
				w.second->Update(a_deltaTime);
			}
		}

		void EditorLayer::OnImGuiRender()
		{
			OPTICK_EVENT()
			for (auto &w : m_editorWindows)
			{
				w.second->RenderWindow();
			}
		}

		void EditorLayer::SetImGuiStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.WindowRounding = 0.0f;

			ImGuiIO& io = ImGui::GetIO();
			static ImWchar s_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			ImFontConfig config;
			config.MergeMode = true;
			const std::string devicePath = ServiceManager::GetService<ResourceManager>()->GetDevicePath("[Assets]fonts/fontawesome-webfont.ttf");
			io.Fonts->AddFontFromFileTTF(devicePath.c_str(), 12.0f, &config, s_ranges); // Merge icon font


			// -- Colors --
			ImVec4* colors = ImGui::GetStyle().Colors;

			//Text
			colors[ImGuiCol_Text] = ImColor(1.f, 1.f, 1.f, 1.f);;
			colors[ImGuiCol_TextDisabled] = ImColor(0.50f, 0.50f, 0.50f, 1.00f);
			colors[ImGuiCol_TextSelectedBg] = ImColor(0.26f, 0.59f, 0.98f, 0.35f);

			//Window
			colors[ImGuiCol_WindowBg] = ImColor(0.3f, 0.3f, 0.3f, 1.f);;
			colors[ImGuiCol_ChildBg] = ImColor(0.15f, 0.15f, 0.15f, 1.00f);
			colors[ImGuiCol_PopupBg] = ImColor(0.08f, 0.08f, 0.08f, 1.00f);
			colors[ImGuiCol_Border] = ImColor(0.43f, 0.43f, 0.50f, 1.00f);
			colors[ImGuiCol_BorderShadow] = ImColor(0.00f, 0.00f, 0.00f, 0.00f);

			//Frame
			colors[ImGuiCol_FrameBg] = ImColor(0.00f, 0.00f, 0.00f, 0.67f);
			colors[ImGuiCol_FrameBgHovered] = ImColor(0.00f, 0.00f, 0.00f, 0.40f);
			colors[ImGuiCol_FrameBgActive] = ImColor(0.26f, 0.59f, 0.98f, 1.00f);

			//Title
			colors[ImGuiCol_TitleBg] = ImColor(0.04f, 0.04f, 0.04f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImColor(0.6f, 0.6f, 0.6f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImColor(0.00f, 0.00f, 0.00f, 0.51f);

			//MenuBar
			colors[ImGuiCol_MenuBarBg] = ImColor(0.14f, 0.14f, 0.14f, 1.00f);

			//Scrollbar
			colors[ImGuiCol_ScrollbarBg] = ImColor(0.02f, 0.02f, 0.02f, 0.53f);
			colors[ImGuiCol_ScrollbarGrab] = ImColor(0.31f, 0.31f, 0.31f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(0.41f, 0.41f, 0.41f, 1.00f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImColor(0.51f, 0.51f, 0.51f, 1.00f);

			//CheckMark
			colors[ImGuiCol_CheckMark] = ImColor(1.00f, 1.00f, 1.00f, 1.00f);

			//Slider
			colors[ImGuiCol_SliderGrab] = ImColor(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_SliderGrabActive] = ImColor(0.27f, 0.07f, 0.07f, 1.00f);

			//Button
			colors[ImGuiCol_Button] = ImColor(0.45f, 0.46f, 0.48f, 1.00f);
			colors[ImGuiCol_ButtonHovered] = ImColor(0.28f, 0.28f, 0.28f, 1.00f);
			colors[ImGuiCol_ButtonActive] = ImColor(0.24f, 0.24f, 0.24f, 1.00f);

			//Header
			colors[ImGuiCol_Header] = ImColor(0.37f, 0.37f, 0.37f, 1.00f);
			colors[ImGuiCol_HeaderHovered] = ImColor(0.26f, 0.59f, 0.98f, 0.80f);
			colors[ImGuiCol_HeaderActive] = ImColor(0.26f, 0.59f, 0.98f, 1.00f);

			//Separator
			colors[ImGuiCol_Separator] = ImColor(0.43f, 0.43f, 0.50f, 0.50f);
			colors[ImGuiCol_SeparatorHovered] = ImColor(0.10f, 0.40f, 0.75f, 0.78f);
			colors[ImGuiCol_SeparatorActive] = ImColor(0.10f, 0.40f, 0.75f, 1.00f);

			//Resize
			colors[ImGuiCol_ResizeGrip] = ImColor(0.26f, 0.59f, 0.98f, 0.25f);
			colors[ImGuiCol_ResizeGripHovered] = ImColor(0.26f, 0.59f, 0.98f, 0.67f);
			colors[ImGuiCol_ResizeGripActive] = ImColor(0.26f, 0.59f, 0.98f, 0.95f);

			//Tab
			colors[ImGuiCol_Tab] = ImColor(0.45f, 0.46f, 0.47f, 1.00f);
			colors[ImGuiCol_TabHovered] = ImColor(0.27f, 0.27f, 0.27f, 1.00f);
			colors[ImGuiCol_TabActive] = ImColor(0.24f, 0.24f, 0.24f, 1.00f);
			colors[ImGuiCol_TabUnfocused] = ImColor(0.07f, 0.10f, 0.15f, 0.97f);
			colors[ImGuiCol_TabUnfocusedActive] = ImColor(0.14f, 0.26f, 0.42f, 1.00f);

			//Plot
			colors[ImGuiCol_PlotLines] = ImColor(0.61f, 0.61f, 0.61f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImColor(1.00f, 0.43f, 0.35f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImColor(0.90f, 0.70f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImColor(1.00f, 0.60f, 0.00f, 1.00f);

			//Drag & drop
			colors[ImGuiCol_DragDropTarget] = ImColor(1.00f, 1.00f, 0.00f, 0.90f);

			//Navigation
			colors[ImGuiCol_NavHighlight] = ImColor(0.26f, 0.59f, 0.98f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImColor(1.00f, 1.00f, 1.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImColor(0.80f, 0.80f, 0.80f, 0.20f);

			//Modal
			colors[ImGuiCol_ModalWindowDimBg] = ImColor(0.80f, 0.80f, 0.80f, 0.35f);
		}
	}
}
