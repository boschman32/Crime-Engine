#include "smpch.h"
#include "CrimeEngine.h"

#include "Game.h"
#include "Utils/ServiceManager.h"
#include "Gameplay/Scenes/HubScene.h"

#if defined(CE_DEBUG) && defined(PLATFORM_WINDOWS)
#include "Tools/WaypointTool.h"
#include "Utils/Editor/EditorLayer.h"
#include "Utils/Editor/EditorWindowContainer.h"
#endif

Game::Game()
{
	ce::ServiceManager::GetService<ce::SceneManager>()->AddScene<HubScene>("Hub");
}

void Game::OnInitialized()
{
#if defined(CE_DEBUG) && defined(PLATFORM_WINDOWS)

	ce::editor::EditorWindowContainer* bottomContainer = ce::editor::EditorLayer::Get().GetWindow<ce::editor::EditorWindowContainer>("BottomContainer");

	if(bottomContainer != nullptr)
	{
		bottomContainer->AddChild(std::make_unique<WaypointTool>("WayPointTool",
			ImVec2(200.f, 100.f),
			ImVec2(300.f, 200.f),
			false));
	}
#endif

	ce::ServiceManager::GetService<ce::SceneManager>()->SwitchScene<HubScene>("Hub");
}

void Game::OnClose()
{
	ce::ServiceManager::GetService<ce::SceneManager>()->RemoveScene("Hub");
}

Game::~Game()
{
	
}

//Create the game "Application"
ce::Application* ce::CreateApplication()
{
	return new Game();
}