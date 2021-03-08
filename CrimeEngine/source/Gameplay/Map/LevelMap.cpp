#include "cepch.h"
#include "Gameplay/Map/LevelMap.h"


#include "Core/ResourceManager.h"
#include "Events/EventService.h"
#include "Gameplay/GameObjectManager.h"
#include "Gameplay/Tile.h"

#include "Gfx/Texture.h"

#include "Gameplay/Map/GridHelper.h"
#include "Gameplay/Map/ITileObjectFactory.h"
#include "Gameplay/Map/MapEvent.h"

namespace ce
{
	LevelMap::LevelMap(MapData& a_mapData, ITileObjectFactory* a_objectFactory, MapOrientation a_orientation)
		: Resource(ResourceType::Map),
		m_mapData(std::move(a_mapData)),
		m_orientation(a_orientation),
		m_tileObjectFactory(a_objectFactory)
	{
		m_mapSize = m_mapData.m_mapSize;

		m_grid = std::make_unique<Grid>(m_mapSize.x, m_mapSize.y, m_mapData.m_tileSize.x, m_mapData.m_tileSize.y, m_orientation);

		m_gameObjectManager = ServiceManager::GetService<GameObjectManager>();
		m_resourceManager = ServiceManager::GetService<ResourceManager>();
	}

	LevelMap::~LevelMap()
	{
		if (m_isBuilt)
		{
			Dispose();
		}
	}

	bool LevelMap::BuildMap(bool a_dispose)
	{
		OPTICK_EVENT();
		//If the map was build already dispose of it.
		if (m_isBuilt && a_dispose)
		{
			Dispose();
		}

		CE_CORE_INFO("Building map({0})!", GetFileName());

		m_tiles.reserve(m_mapData.m_tileCount);

		unsigned layerId = 0;
		float startZOffset = 90.f;
		const float zOffset = float(m_mapSize.y + m_mapSize.x) / 2.f;
		//Loop through all the tilemaps which are layered.
		for (auto layer = m_mapData.m_tileLayers.rbegin(); layer != m_mapData.m_tileLayers.rend(); ++layer)
		{
			layerId++;

			if (layer->getType() == tmx::Layer::Type::Tile)
			{
				startZOffset += zOffset * float(layerId);
				//Loop through the tilemap itself.
				for (unsigned row = 0; row < m_mapSize.y; ++row)
				{
					for (unsigned column = 0; column < m_mapSize.x; ++column)
					{
						auto cell = m_grid->GetCell(column, row);
						const unsigned tileIndex = row * m_grid->GetGridSize().x + column;
						const uint32_t tileGid = layer->getTiles()[tileIndex].ID;

						const auto setIt = m_mapData.GetTilesetByGid(tileGid);	//Get the tileset associated with the global ID of the tile.

						//Make sure the tileset we got by the GID exist.
						if (setIt != nullptr)
						{
							const tmx::Tileset& tileset = *setIt;

							//The tileset for this tile doesn't exist probably because it's a void tile.
							if (const auto tmxTile = tileset.getTile(tileGid))
							{
								Vector3 pos = grid_helper::ConvertGridToWorldPosition({ column, row }, m_mapSize, m_orientation == MapOrientation::Isometric);
								pos.z = startZOffset - float(row) - float(column);

								//Load in the tileset map.
								const std::shared_ptr<Texture> tileTexture = m_resourceManager->LoadResource<Texture>(tmxTile->imagePath);
								m_loadedTileTextures.insert(tileTexture);
								Sprite sprite;

								//The tile data inside the tileset.
								Vector4 texRect(tmxTile->imagePosition.x, tmxTile->imagePosition.y, tmxTile->imageSize.x, tmxTile->imageSize.y);
								sprite.SetTexture(*tileTexture, texRect);

								auto it = gs_tileTypeMap.find(tmxTile->type);

								TileTypes type;
								if(it != gs_tileTypeMap.end())
								{
									type = it->second;
								}
								else
								{
									type = TileTypes::Void;
								}

								auto tile = m_gameObjectManager->CreateGameObject<Tile>(
									layer->getName() + "_Tile_" + std::to_string(row) + "_" + std::to_string(column),
									pos,
									UVector2(column, row),
									tmxTile->ID,
									layer->getName(),
									type,
									sprite);

								//Add this tile to the current cell.
								if (tile != nullptr)
								{
									cell->AddTile(*tile);

									m_tiles.push_back(tile);
								}
								else
								{
									CE_CORE_ERROR("TILE IS NULL!");
								}
							}
						}
					}
				}
			}
		}

		//Create tile objects if a factory exists to create them.
		if (m_tileObjectFactory != nullptr)
		{
			for (auto &objGroup : m_mapData.m_objectLayers)
			{
				for (auto &obj : objGroup.getObjects())
				{
					//Divide by y both times since Tiled gives back x * 2 when working in isometric (and we don't want that, since the object position isn't in isometric).
					const UVector2 gridPos = { (obj.getPosition().x / m_mapData.m_tileSize.y) - 1, (obj.getPosition().y / m_mapData.m_tileSize.y) - 1 };

					GridCell* cell = m_grid->GetCell(gridPos);

					if (cell != nullptr)
					{
						const Vector3 spawnPos = grid_helper::ConvertGridToWorldPosition(gridPos, m_mapSize, m_orientation == MapOrientation::Isometric);

						//Tell the factory to create the correct tile object.
						TileObject* createdObject = m_tileObjectFactory->CreateObject(spawnPos, obj.getName(), obj.getType(), obj.getProperties());

						if (createdObject != nullptr)
						{
							cell->PushTileObject(*createdObject);

							m_tileObjects.push_back(createdObject);
							createdObject->m_gridPosition = gridPos;
						}
					}
				}
			}
		}
		else if (!m_mapData.m_objectLayers.empty())
		{
			CE_CORE_WARN("[LevelMap({0})] Couldn't create TileObjects as there is not a TileObjectFactory defined, pass it along to the LoadMap function!", GetFileName());
		}

		m_isBuilt = true;
		MapBuiltEvent evt {this};
		EventService::Dispatch(evt);
		return m_isBuilt;
	}

	void LevelMap::Reload()
	{
		m_grid->ClearAllCells();
		for(auto tile : m_tiles)
		{
			m_gameObjectManager->DestroyGameObject(tile);
		}
		m_tiles.clear();
		
		for (auto obj : m_tileObjects)
		{
			m_gameObjectManager->DestroyGameObject(obj->GetId());
		}
		m_tileObjects.clear();

		//Just rebuild the map.
		BuildMap(false);
	}

	Tile* LevelMap::GetTile(unsigned a_gridX, unsigned a_gridY, int a_index) const
	{
		GridCell* cell = m_grid->GetCell(a_gridX, a_gridY);

		if (cell != nullptr)
		{
			return cell->GetTiles()[a_index];
		}
		return nullptr;
	}

	// --- Getting single tile by index ---

	Tile* LevelMap::GetTile(const UVector2& a_gridPos, int a_index) const
	{
		return GetTile(a_gridPos.x, a_gridPos.y, a_index);
	}

	Tile* LevelMap::GetTile(const Vector3& a_pos, int a_index) const
	{
		return GetTile(grid_helper::ConvertToGridPosition(a_pos, m_mapSize, m_orientation == MapOrientation::Isometric), a_index);
	}

	// --- Getting tile list by type ---

	std::vector<Tile*> LevelMap::GetTilesByType(unsigned a_gridX, unsigned a_gridY, TileTypes a_type) const
	{
		GridCell* cell = m_grid->GetCell(a_gridX, a_gridY);

		if (cell != nullptr)
		{
			const std::vector<Tile*>& tiles = cell->GetTiles();
			std::vector<Tile*> tilesByType;
			tilesByType.reserve(tiles.size());

			for (auto t : tiles)
			{
				if (t->GetTileType() == a_type)
				{
					tilesByType.push_back(t);
				}
			}
			return tilesByType;
		}
		return { };
	}

	std::vector<Tile*> LevelMap::GetTilesByType(const UVector2& a_gridPos, TileTypes a_type) const
	{
		return GetTilesByType(a_gridPos.x, a_gridPos.y, a_type);
	}

	std::vector<Tile*> LevelMap::GetTilesByType(const Vector3& a_pos, TileTypes a_type) const
	{
		return GetTilesByType(grid_helper::ConvertToGridPosition(a_pos, m_mapSize, m_orientation == MapOrientation::Isometric), a_type);
	}

	std::vector<Tile*> LevelMap::GetTilesByType(TileTypes a_type) const
	{
		std::vector<Tile*> tiles;
		for (auto t : m_tiles)
		{
			if (t->GetTileType() == a_type)
			{
				tiles.push_back(t);
			}
		}
		return tiles;
	}


	// --- Getting tile list by layer name ---

	Tile* LevelMap::GetTileByLayer(unsigned a_gridX, unsigned a_gridY, const std::string& a_layer) const
	{
		GridCell* cell = m_grid->GetCell(a_gridX, a_gridY);

		if (cell != nullptr)
		{
			const std::vector<Tile*>& tiles = cell->GetTiles();

			for (auto t : tiles)
			{
				if (t->GetLayer() == a_layer)
				{
					return t;
				}
			}
		}
		return nullptr;
	}

	Tile* LevelMap::GetTileByLayer(const UVector2& a_gridPos, const std::string& a_layer) const
	{
		return GetTileByLayer(a_gridPos.x, a_gridPos.y, a_layer);
	}

	Tile* LevelMap::GetTileByLayer(const Vector3& a_pos, const std::string& a_layer) const
	{
		return GetTileByLayer(grid_helper::ConvertToGridPosition(a_pos, m_mapSize, m_orientation == MapOrientation::Isometric), a_layer);
	}

	std::vector<Tile*> LevelMap::GetTilesByLayer(const std::vector<std::string>& a_layers) const
	{
		std::vector<Tile*> tiles;
		for (auto t : m_tiles)
		{
			if (std::find(a_layers.begin(), a_layers.end(), t->GetLayer()) != a_layers.end())
			{
				tiles.push_back(t);
			}
		}
		return tiles;
	}

	void LevelMap::Show()
	{
		for (auto t : m_tiles)
		{
			t->SetActive(true);
		}
	}

	void LevelMap::Hide()
	{
		for (auto t : m_tiles)
		{
			t->SetActive(false);
		}
	}

	void LevelMap::Dispose()
	{
		OPTICK_EVENT();
		m_grid->ClearAllCells();
		
		for (auto t : m_tiles)
		{
			m_gameObjectManager->DestroyGameObject(t->GetId());
		}
		m_tiles.clear();
		for (auto obj : m_tileObjects)
		{
			m_gameObjectManager->DestroyGameObject(obj->GetId());
		}
		m_tileObjects.clear();

		while (!m_loadedTileTextures.empty())
		{
			auto it = std::prev(m_loadedTileTextures.end());
			const auto id = (*it)->GetID();
			m_loadedTileTextures.erase(it);

			m_resourceManager->ReleaseResource<Texture>(id);
		}
		m_loadedTileTextures.clear();

		m_isBuilt = false;
	}
}
