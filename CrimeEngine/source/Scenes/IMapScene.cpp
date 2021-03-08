#include "cepch.h"
#include "Scenes/IMapScene.h"

#include "Core/ResourceManager.h"

#include "Events/EventService.h"
#include "Gameplay/Map/MapEvent.h"

#include "Gfx/Shader.h"

namespace ce
{
	IMapScene::IMapScene()
	{
		m_levelService = ServiceManager::GetService<LevelService>();
		m_resourceManager = ServiceManager::GetService<ResourceManager>();

		EventService::Subscribe<MapBuiltEvent>(EventType::MapBuilt, BIND_EVENT_CALLBACK(IMapScene::OnMapBuilt), true);
	}

	IMapScene::~IMapScene()
	{
		EventService::Unsubscribe<MapBuiltEvent>(EventType::MapBuilt, BIND_EVENT_CALLBACK(IMapScene::OnMapBuilt));
		if (m_mapIsLoaded)
		{
			ServiceManager::GetService<ResourceManager>()->ReleaseResource(m_loadedMap);
		}
	}

	void IMapScene::OnUpdate(float a_deltaTime)
	{
		switch (m_simState)
		{
		case SimulationState::Planning:
			Planning(a_deltaTime);
			break;
		case SimulationState::Simulating:
			Simulate(a_deltaTime);
			break;
		default:;
		}
	}

	void IMapScene::BeginPlan()
	{
		CE_CORE_ASSERT_M(m_mapIsLoaded, "Map hasn't been loaded in yet so can't start planning!");

		if (m_simState != SimulationState::Planning)
		{
			CE_CORE_INFO("Changing to planning phase");
			m_simState = SimulationState::Planning;
			OnPlan();
		}
	}

	void IMapScene::BeginSimulation()
	{
		CE_CORE_ASSERT_M(m_mapIsLoaded, "Map hasn't been loaded in yet so can't start simulation!");

		if (m_simState != SimulationState::Simulating)
		{
			CE_CORE_INFO("Changing to simulation phase");
			m_simState = SimulationState::Simulating;
			OnSimulation();
		}
	}

	void IMapScene::SelectMap(const std::string& a_selectedMap)
	{
		const std::string devicePath = m_resourceManager->GetDevicePath(a_selectedMap);
		m_levelService->LookForAvailableMaps();

		for (auto m : m_levelService->GetAvailableMaps())
		{
			if (FileSystem::ComparePath(m, devicePath))
			{
				m_selectedMapPath = m;
				CE_CORE_INFO("Selected map: {0}", m_selectedMapPath);
				return;
			}
		}

		CE_CORE_ASSERT_M(false, "No map with name {0} does exist in the data folder!", devicePath);
	}

	bool IMapScene::LoadSelectedMap(ITileObjectFactory* a_objectFactory)
	{
		if (m_selectedMapPath.empty())
		{
			CE_CORE_CRITICAL("No map has been selected, call select map first!");
			return false;
		}

		m_mapIsLoaded = false;
		m_simState = SimulationState::None;
		m_selectedMapPath = m_resourceManager->GetDevicePath(m_selectedMapPath);

		if (m_loadedMap != nullptr)
		{
			//If we load in the same map we can just reload it.
			if (m_loadedMap->GetPath() == m_selectedMapPath)
			{
				CE_CORE_INFO("Map({0}) reloaded!", m_selectedMapPath);
				m_mapIsLoaded = true;
				m_loadedMap->Reload();
				return true;
			}
			//Otherwise unload it.
			ServiceManager::GetService<ResourceManager>()->ReleaseResource(m_loadedMap);
		}

		CE_CORE_INFO("Loading map({0})...", m_selectedMapPath);
		if (m_levelService->LoadMap(m_selectedMapPath, m_loadedMap, a_objectFactory))
		{
			OnMapLoaded(m_loadedMap);
			CE_CORE_INFO("Map({0}) loaded!", m_selectedMapPath);
			m_mapIsLoaded = true;

			if (m_loadedMap->BuildMap())
			{
				CE_CORE_INFO("Map({0}) built!", m_selectedMapPath);
				return true;
			}
		}

		return false;
	}
}
