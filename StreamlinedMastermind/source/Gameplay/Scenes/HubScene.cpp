#include "smpch.h"
#include "Gameplay/Scenes/HubScene.h"

//Core
#include "Core/ResourceManager.h"
#include "Core/Input/InputManager.h"
#include "Core/Input/KeyEnums.h"

//Gameplay
#include "Gameplay/GameObjectManager.h"
#include "Gameplay/Guard.h"
#include "Gameplay/Character.h"

#include "Gameplay/Tile.h"
#include "Gameplay/Map/LevelService.h"
#include "Gameplay/GameTileObjectFactory.h"

//Gfx
#include "Gfx/Camera.h"

//Utils
#include "Utils/ServiceManager.h"
#include "Events/Move.h"
#include "Gameplay/Map/MapEvent.h"

HubScene::HubScene()
{
	m_graph = std::make_unique<pf::Graph>();
	m_gameTileObjectFactory = std::make_unique<GameTileObjectFactory>(m_graph.get());

	m_gameObjectManager = ce::ServiceManager::GetService<ce::GameObjectManager>();

	//Select a map to start with.
	SelectMap("[Assets]maps/Level_Test.tmx");
}

//Called when we switch to this scene from another scene.
bool HubScene::OnSceneSwitched()
{
	m_btTest = std::make_shared<bt::BehaviorTreeTestObject>();
	m_stash = std::make_unique<Stash>();
	m_charEventSystem = std::make_unique<CharacterEventSystem>();

	//Camera setup
	ce::Camera2D* cam = dynamic_cast<ce::Camera2D*>(ce::Camera::GetMainCamera());
	cam->SetBounds(glm::vec2(18.f, 9.f));
	cam->SetRenderDistance(glm::vec2(0.1f, 1000.0f));
	cam->SetOrthographicProjection(ce::ServiceManager::GetService<ce::RenderService>()->GetSize());
	cam->LookDir(glm::vec3(0, 0, 1), glm::vec3(0, -1, 0));

	m_camController = m_gameObjectManager->CreateGameObject<CameraController>("CamController", cam);
	m_playerController = m_gameObjectManager->CreateGameObject<PlayerController>("Player", m_camController);
	m_canvas = m_gameObjectManager->CreateGameObject<ce::GuiCanvas>("Canvas", *m_camController->m_cam);

	m_planningPhaseBtn = new ce::GuiButton(m_canvas);
	m_planningPhaseBtn->SetAnchor(ce::anchor::TopCenter);
	m_planningPhaseBtn->SetPivot(ce::pivot::TopCenter);
	m_planningPhaseBtn->SetRect(0, 30, 100, 60);
	m_buttonTexture = ce::ServiceManager::GetService<ce::ResourceManager>()->LoadResource<ce::Texture>("data/textures/SimulateButton.png");
	m_planningPhaseBtn->GetSprite()->SetTexture(*m_buttonTexture);
	m_planningPhaseBtn->m_interaction.m_pointerUp.AddListener<IMapScene, &IMapScene::BeginSimulation>(this);

	m_explainText = new cr::Text();
	m_explainText->SetPos({ 0.01f, 0.15f });
	m_explainText->SetSize(glm::vec2(0.4f, 0.4f));
	m_explainText->SetColour(glm::vec4(0, 0, 0, 1));

	m_resultText = new cr::Text();
	m_resultText->SetPos({ 0.4f, 0.8f });
	m_resultText->SetSize(glm::vec2(1.0f, 1.0f));
	m_resultText->SetColour(glm::vec4(0, 0, 0, 1));

	//Load the selected map.
	LoadSelectedMap(m_gameTileObjectFactory.get());

	m_ending = false;
	m_intervalTime = 0.0f;

	return true;
}

//Called when a map has been loaded through "LoadSelectedMap"
void HubScene::OnMapLoaded(std::shared_ptr<ce::LevelMap>&)
{
	//Do stuff here before the map is actually built.
}

//Called when the current map has been built (all objects have been added).
void HubScene::OnMapBuilt(ce::MapBuiltEvent& a_event)
{
	m_graph->SetupGraph(&a_event.GetBuiltMap());
	
	//Change color of the guard and player
	if (auto character = ce::ServiceManager::GetService<ce::GameObjectManager>()->GetGameObject<Character>("testChar"))
	{
		character->m_sprite.SetColor(glm::vec4(0, 1, 0, 1));

		m_guards = ce::ServiceManager::GetService<ce::GameObjectManager>()->GetGameObjectsByType<Guard>();
		for (Guard* guard : m_guards) {
			guard->m_sprite.SetColor(glm::vec4(1, 0, 0, 1));
		}

		BeginPlan();
	}
}

void HubScene::OnDispose()
{
	ce::ServiceManager::GetService<ce::ResourceManager>()->ReleaseResource(m_buttonTexture);
	m_gameObjectManager->DestroyGameObject(m_canvas);
	m_gameObjectManager->DestroyGameObject(m_camController);
	m_gameObjectManager->DestroyGameObject(m_playerController);
	m_character = nullptr;

	m_charEventSystem.reset();

	delete m_planningPhaseBtn;
	delete m_explainText;
	delete m_resultText;
}

void HubScene::OnUpdate(const float a_deltaTime)
{
	IMapScene::OnUpdate(a_deltaTime);
	m_btTest->Update(a_deltaTime);

	if (m_ending)
	{
        for (auto unit : m_gameObjectManager->GetGameObjectsByType<Unit>())
        {
            unit->SetActive(false);
        }

		if (m_timer.GetTimeSinceStart() > m_intervalTime + 2.0f)
		{
			ce::ServiceManager::GetService<ce::SceneManager>()->SwitchScene<HubScene>("Hub");
		}
	}
}

void HubScene::OnDraw(cr::RenderLayer & a_renderLayer)
{
	if (m_ending)
	{
		m_resultText->Draw(a_renderLayer);
	}
	m_explainText->Draw(a_renderLayer);
}

void HubScene::OnPlan()
{
	m_planningPhaseBtn->SetActive(true);
	
	m_explainText->SetText("Plan your moves by right clicking on a tile.\nWhen your path is set press the simulate\nbutton to start the heist!");
	std::vector<Guard*> allGuards = ce::ServiceManager::GetService<ce::GameObjectManager>()->GetGameObjectsByType<Guard>();
	for (Guard* guard : allGuards) {
		guard->PauseGuardMovement();
	}
	if (m_character == nullptr)
	{
		m_character = ce::ServiceManager::GetService<ce::GameObjectManager>()->GetGameObject<Character>("testChar");
	}
}

void HubScene::OnSimulation()
{
	m_planningPhaseBtn->SetActive(false);
	
	std::vector<Guard*> allGuards = ce::ServiceManager::GetService<ce::GameObjectManager>()->GetGameObjectsByType<Guard>();
	for (Guard* guard : allGuards) {
		guard->ResumeGuardMovement();
	}

	m_money = m_gameObjectManager->GetGameObjectsByType<items::MoneyPack>();
}

void::HubScene::Simulate(const float a_deltaTime)
{
	m_charEventSystem->Update(a_deltaTime);

	for (auto moneyPack : m_money)
	{
		if (glm::vec2(moneyPack->GetPosition().x, moneyPack->GetPosition().y) == m_character->m_currentPosition)
		{
			m_stash->AddItem(EInventory::Character1, *moneyPack);
			moneyPack->SetPosition(glm::vec3(5.0f, 5.0f, -1.0f));
		}
	}

	if (m_stash->GetMoneyAmount(EInventory::Character1) > 500 && !m_ending)
	{
		m_resultText->SetText("You Win!");
		m_intervalTime = m_timer.GetTimeSinceStart();
		m_ending = true;
	}

	if (!m_ending)
	{
		for (auto guard : m_guards)
		{
			if (m_character->m_currentPosition == guard->m_currentPosition)
			{
				m_resultText->SetText("Game Over!");
				m_intervalTime = m_timer.GetTimeSinceStart();
				m_ending = true;
			}
		}
	}
}

void HubScene::Planning(const float)
{
	OPTICK_EVENT();
	if (InputManager::IsMouseButtonDown(MouseKeys::RButton))
	{
		if (m_character != nullptr && !m_mousePressed)
		{
			m_mousePressed = true;
			glm::vec2 mouseVec = glm::vec2(ce::Camera::GetMainCamera()->CalculateMouseRay().x, ce::Camera::GetMainCamera()->CalculateMouseRay().y);

			ce::GridCell* cell = m_loadedMap->GetGrid().GetCellWithWorld(mouseVec.x, mouseVec.y);

			//Add a check here to see if there is a tile
			glm::vec2 pos = cell->GetTiles()[0]->GetTilePosition();

			Move* move = new Move(pos);

			std::vector<glm::vec2> allPositions = m_graph->AStarPath(m_character->m_finalPosition, pos);

			if (!allPositions.empty()) {
				m_charEventSystem->AddEvent(m_character, move);
			}
			else
			{
				delete move;
			}
			

			if (!allPositions.empty()) {
				m_character->m_finalPosition = pos;
			}

			for (auto& allPosition : allPositions)
			{
				//ce::GridCell* colorCell = map->GetGrid().GetCellWithWorld(allPosition.x, allPosition.y);
				ce::Tile* t = m_loadedMap->GetTileByLayer({ allPosition.x, allPosition.y, 0.f }, "Floor");

				t->m_sprite.SetColor(glm::vec4(0, 1, 0, 1));
			}
		}
	}
	else
	{
		m_mousePressed = false;
	}
}
