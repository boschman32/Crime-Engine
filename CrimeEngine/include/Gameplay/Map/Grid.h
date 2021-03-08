#pragma once
#include "Core/Math.h"

#ifdef CE_DEBUG
#define DEBUG_GRID 1

#if DEBUG_GRID
#include "Gfx/Drawables/Sprite.h"
#endif
#endif


namespace cr {
	class RenderLayer;
}

namespace ce
{
	//Forward declaration.
	enum class MapOrientation;
	class Tile;
	class TileObject;

	class GridCell
	{
	public:
		GridCell(unsigned a_x, unsigned a_y);
		~GridCell() = default;

		void AddTile(Tile& a_tile);

		void PushTileObject(TileObject& a_object);
		void RemoveTileObject(TileObject& a_object);
		void Clear();
		
		const std::vector<Tile*>& GetTiles() const { return m_tiles; };
		const std::unordered_map<uint32_t, TileObject*>& GetTileObjectBucket() const { return m_tileObjectBucket; };

		const unsigned m_x = { 0 };
		const unsigned m_y = { 0 };

#if DEBUG_GRID
		Vector4 m_debugColor { 1.f, 0.f, 0.f, 1.f };
		Sprite m_debugSprite;
#endif
	private:
		std::vector<Tile*> m_tiles;
		std::unordered_map<uint32_t, TileObject*> m_tileObjectBucket;
	};

	class Grid
	{
	public:
		Grid(unsigned a_width, unsigned a_height, unsigned a_cellW, unsigned a_cellH, MapOrientation a_orientation);
		~Grid();
		
		void AddCell(unsigned a_x, unsigned a_y);
		void AddCell(const UVector2& a_pos);

		void OccupyCell(unsigned a_x, unsigned a_y, TileObject& a_object);
		void OccupyCell(const UVector2& a_pos, TileObject& a_object);

		void ClearCellOf(unsigned a_x, unsigned a_y, TileObject& a_object);
		void ClearCellOf(const UVector2& a_pos, TileObject& a_object);

		GridCell* GetCell(unsigned a_x, unsigned a_y);
		GridCell* GetCell(const UVector2& a_pos);
		
		GridCell* GetCellWithWorld(float a_worldX, float a_worldY);

		const std::vector<GridCell>& GetCells() const { return m_cells; }

		const UVector2& GetGridSize() const { return m_gridSize; }
		const UVector2& GetCellSize() const { return m_cellSize; }

		void ClearAllCells();

#if DEBUG_GRID
		void DrawDebug(cr::RenderLayer& a_renderLayer,  MapOrientation a_orientation);

		bool IsDebugEnabled() const { return m_isDebugEnabled; }
		void SetEnableDebug(bool a_enable) { m_isDebugEnabled = a_enable; }
#endif
	private:
		UVector2 m_gridSize;
		UVector2 m_cellSize;
		MapOrientation m_orientation;
		
		std::vector<GridCell> m_cells;

#if DEBUG_GRID
		Sprite m_debugSprite;
		std::shared_ptr<Texture> m_debugTexture;
		bool m_isDebugEnabled { false };
#endif
	};
}
