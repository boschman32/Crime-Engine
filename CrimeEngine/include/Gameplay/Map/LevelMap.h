#pragma once
#include "Core/Resource.h"
#include "Core/Math.h"
#include "Gfx/Texture.h"

#include "Grid.h"
#include "ITileObjectFactory.h"

namespace ce
{
	enum class TileTypes;
	class GameObjectManager;
	class ResourceManager;
	
	struct MapData
	{
		glm::uvec2 m_mapSize { 0, 0 };
		glm::uvec2 m_tileSize { 0, 0 };
		std::vector<tmx::TileLayer> m_tileLayers;
		std::vector<tmx::ObjectGroup> m_objectLayers;
		std::vector<tmx::Tileset> m_tilesets;
		unsigned int m_tileCount { 0 };

		const tmx::Tileset* GetTilesetByGid(uint32_t a_tileGid)
		{
			OPTICK_EVENT();
			for(auto it = m_tilesets.rbegin(); it != m_tilesets.rend(); ++it)
			{
				if(it->getFirstGID() <= a_tileGid)
				{
					return &(*it);
				}
			}
			return nullptr;
		}
	};

	enum class MapOrientation
	{
		Orthogonal,
		Isometric
	};
	
	class LevelMap : public Resource
	{
	public:
		LevelMap(MapData& a_mapData, ITileObjectFactory* a_objectFactory = nullptr, MapOrientation a_orientation = MapOrientation::Isometric);
		~LevelMap() override;

		bool BuildMap(bool a_dispose = true);
		void Reload() override;

		Grid& GetGrid() const { return *m_grid; }
		bool IsBuilt() const { return m_isBuilt; }

		const UVector2& GetMapSize() const { return m_mapSize; }
		const MapOrientation& GetMapOrientation() const { return m_orientation; }

		const std::vector<Tile*>& GetTiles() const { return m_tiles; }
		std::vector<Tile*>::iterator begin() //NOLINT
		{ return m_tiles.begin(); }
		std::vector<Tile*>::iterator end() //NOLINT
		{ return m_tiles.end(); }

		Tile* GetTile(unsigned a_gridX, unsigned a_gridY, int a_index) const;
		Tile* GetTile(const UVector2& a_gridPos, int a_index) const;
		Tile* GetTile(const Vector3& a_pos, int a_index) const;

		std::vector<Tile*> GetTilesByType(unsigned a_gridX, unsigned a_gridY, TileTypes a_type) const;
		std::vector<Tile*> GetTilesByType(const UVector2& a_gridPos, TileTypes a_type) const;
		std::vector<Tile*> GetTilesByType(const Vector3& a_pos, TileTypes a_type) const;

		std::vector<Tile*> GetTilesByType(TileTypes a_type) const;

		Tile* GetTileByLayer(unsigned a_gridX, unsigned a_gridY, const std::string& a_layer) const;
		Tile* GetTileByLayer(const UVector2& a_gridPos, const std::string& a_layer) const;
		Tile* GetTileByLayer(const Vector3& a_pos, const std::string& a_layer) const;
		
		std::vector<Tile*> GetTilesByLayer(const std::vector<std::string>& a_layers) const;
		
		void Show();
		void Hide();
	private:
		bool m_isBuilt { false };
		std::unique_ptr<Grid> m_grid;

		std::vector<Tile*> m_tiles;
		std::vector<TileObject*> m_tileObjects;
		
		UVector2 m_mapSize { 0, 0 };
		MapData m_mapData;
		MapOrientation m_orientation;

		std::set<std::shared_ptr<Texture>> m_loadedTileTextures;

		GameObjectManager* m_gameObjectManager;
		ResourceManager* m_resourceManager;
		ITileObjectFactory* m_tileObjectFactory;

		void Dispose();
	};
}
