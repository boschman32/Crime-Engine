#include "cepch.h"
#include "Gameplay/Map/LevelService.h"


#include "Utils/ServiceManager.h"
#include "Core/ResourceManager.h"
#include "Events/EventService.h"
#include "Gameplay/Map/MapEvent.h"

#define MAPS_FOLDER "./data/maps/"

namespace ce
{
	LevelService::~LevelService()
	{
		
	}

	void LevelService::OnCreate()
	{
		LookForAvailableMaps();

		m_resourceManager = ServiceManager::GetService<ResourceManager>();
	}

	void LevelService::OnDestroy()
	{
		UnloadCurrentMap();
	}

#if DEBUG_GRID
	void LevelService::DebugDraw(cr::RenderLayer& a_renderLayer) const
	{
		if(m_currentMap != nullptr)
		{
			m_currentMap->GetGrid().DrawDebug(a_renderLayer, m_currentMap->GetMapOrientation());
		}
	}
#endif

	void LevelService::LookForAvailableMaps()
	{
		m_availableMaps.clear();
#ifdef PLATFORM_WINDOWS
		namespace fs = std::experimental::filesystem;

		for(const auto& entry : fs::directory_iterator(MAPS_FOLDER))
		{
			if(entry.path().has_extension() && entry.path().extension() == ".tmx")
			{
				m_availableMaps.push_back(entry.path().string());
			}
		}
#endif
	}

	bool LevelService::LoadMap(const std::string& a_path, std::shared_ptr<LevelMap>& a_map, ITileObjectFactory* a_objectFactory)
	{
		const std::string devicePath = m_resourceManager->GetDevicePath(a_path);
		//Check if the current map is already loaded if so we can just return it.
		if(m_currentMap != nullptr && m_currentMap->GetPath() == devicePath)
		{
			a_map = m_currentMap;
			MapLoadedEvent evt{m_currentMap.get()};
			EventService::Dispatch(evt);
			return true;
		}

		//Unload the current map (if we have any).
		UnloadCurrentMap();
	
		// Store map on stack since we just need to save it's data
		tmx::Map tmxMap;

		if (tmxMap.load(devicePath))
		{
			MapData data;
			
			auto& tmxLayers = tmxMap.getLayers();
			for (auto& tmxLayer : tmxLayers)
			{
				if(tmxLayer->getType() == tmx::Layer::Type::Tile)
				{
					const auto& tileLayer = tmxLayer->getLayerAs<tmx::TileLayer>();

					unsigned int numTiles = 0;
					for(auto &t : tileLayer.getTiles())
					{
						if(t.ID != 0)
						{
							numTiles++;
						}
					}
					data.m_tileCount += numTiles;
					data.m_tileLayers.push_back(tileLayer);
				}
				else if(tmxLayer->getType() == tmx::Layer::Type::Object)
				{
					const auto& objectLayer = tmxLayer->getLayerAs<tmx::ObjectGroup>();
					data.m_objectLayers.push_back(objectLayer);
				}
			}

			data.m_mapSize = glm::uvec2(tmxMap.getTileCount().x, tmxMap.getTileCount().y);
			data.m_tileSize = glm::uvec2(tmxMap.getTileSize().x, tmxMap.getTileSize().y);
			
			for(auto& tileset : tmxMap.getTilesets())
			{
				data.m_tilesets.push_back(tileset);
			}

			MapOrientation orientation = (tmxMap.getOrientation() == tmx::Orientation::Orthogonal ? MapOrientation::Orthogonal : MapOrientation::Isometric);
			m_currentMap = ServiceManager::GetService<ResourceManager>()->LoadResource<LevelMap>(devicePath, std::move(data), a_objectFactory, orientation);
			CE_APP_INFO("[LevelService] Map loaded!");
			a_map = m_currentMap;

			//Tell anyone who registered himself to the callbacks that a new map has been loaded.
			MapLoadedEvent evt { m_currentMap.get() };
			EventService::Dispatch(evt);
			return true;
		}

		m_currentMap = nullptr;
		a_map = nullptr;
		CE_CORE_ERROR("[LevelService] Could not load map!");
		return false;
	}

	void LevelService::UnloadCurrentMap()
	{
		if(m_currentMap != nullptr)
		{
			ServiceManager::GetService<ResourceManager>()->ReleaseResource(m_currentMap);
		}
	}
}