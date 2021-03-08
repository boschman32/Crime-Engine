#include "cepch.h"
#include "Gameplay/Map/Grid.h"

#include "Gameplay/TileObject.h"
#include "Gameplay/Map/GridHelper.h"
#include "Gameplay/Map/LevelService.h"

#if DEBUG_GRID
#include "Gfx/RenderLayer.h"
#include "Utils/ServiceManager.h"
#include "Core/ResourceManager.h"
#include "Gfx/Texture.h"
#include "Gameplay/Map/LevelMap.h"
#endif

namespace ce
{
	/////////////////////////////
	///--------- Grid -----------
	/////////////////////////////
	
	Grid::Grid(unsigned a_width, unsigned a_height, unsigned a_cellW, unsigned a_cellH, MapOrientation a_orientation)
		: m_gridSize(a_width, a_height), m_cellSize(a_cellW, a_cellH), m_orientation(a_orientation)
	{
#if DEBUG_GRID
		if(m_debugTexture == nullptr)
		{
			if(m_orientation == MapOrientation::Isometric)
			{
				m_debugTexture = ServiceManager::GetService<ResourceManager>()->LoadResource<Texture>("./data/textures/debug_grid_iso.png");
			}
			else
			{
				m_debugTexture = ServiceManager::GetService<ResourceManager>()->LoadResource<Texture>("./data/textures/debug_grid_ortho.png");
			}
	}
#endif
		
		m_cells.reserve(m_gridSize.x * m_gridSize.y);
		for(unsigned row = 0; row < m_gridSize.y; ++row)
		{
			for(unsigned column = 0; column < m_gridSize.x; ++column)
			{
				AddCell(column, row);
			}
		}
	}

	Grid::~Grid()
	{
		m_cells.clear();

#if DEBUG_GRID
		ServiceManager::GetService<ResourceManager>()->ReleaseResource(m_debugTexture);
#endif
	}

	void Grid::AddCell(unsigned a_x, unsigned a_y)
	{
		GridCell newCell (a_x, a_y);
#if DEBUG_GRID
		newCell.m_debugSprite.SetTexture(*m_debugTexture);
#endif
		m_cells.push_back(std::move(newCell));
	}

	void Grid::AddCell(const UVector2& a_pos)
	{
		AddCell(a_pos.x, a_pos.y);
	}

	void Grid::OccupyCell(unsigned a_x, unsigned a_y, TileObject& a_object)
	{
		if(auto cell = GetCell(a_x, a_y))
		{
			cell->PushTileObject(a_object);
		}
	}

	void Grid::OccupyCell(const UVector2& a_pos, TileObject& a_object)
	{
		OccupyCell(a_pos.x, a_pos.y, a_object);
	}

	void Grid::ClearCellOf(unsigned a_x, unsigned a_y, TileObject& a_object)
	{
		if(auto cell = GetCell(a_x, a_y))
		{
			cell->RemoveTileObject(a_object);
		}
	}

	void Grid::ClearCellOf(const UVector2& a_pos, TileObject& a_object)
	{
		ClearCellOf(a_pos.x, a_pos.y, a_object);
	}

	GridCell* Grid::GetCell(unsigned a_x, unsigned a_y)
	{
		const unsigned index = a_y * m_gridSize.x + a_x;
		if(index < m_cells.size())
		{
			return &m_cells[index];
		}
		return nullptr;
	}

	GridCell* Grid::GetCell(const UVector2& a_pos)
	{
		return GetCell(a_pos.x, a_pos.y);
	}

	GridCell* Grid::GetCellWithWorld(float a_worldX, float a_worldY)
	{
		const UVector2 gridPos = grid_helper::ConvertToGridPosition({a_worldX, a_worldY }, m_gridSize, m_orientation == MapOrientation::Isometric);
		return GetCell(gridPos);
	}

#if DEBUG_GRID
	void Grid::DrawDebug(cr::RenderLayer& a_renderLayer, MapOrientation a_orientation)
	{
		OPTICK_EVENT()
		if(!m_isDebugEnabled)
		{
			return;
		}
		
		for(auto &c : m_cells)
		{
			const Vector3 pos = Vector3(float(c.m_x) - m_gridSize.x / 2.f, float(c.m_y) - m_gridSize.y / 2.f, 0.f);

			switch (a_orientation)
			{
			case MapOrientation::Orthogonal:
				c.m_debugSprite.SetPosition(pos);
				break;
			case MapOrientation::Isometric:
				Vector2 isoPos = grid_helper::CartesianToIsometric(pos);
				c.m_debugSprite.SetPosition(Vector3(isoPos, 0.f));
				break;
			}

			if(!c.GetTileObjectBucket().empty())
			{
				c.m_debugSprite.SetColor({0, 1, 0, 1});
			}
			else
			{
				c.m_debugSprite.SetColor(c.m_debugColor);
			}
			
			c.m_debugSprite.Draw(a_renderLayer);
		}
	}
#endif
	void Grid::ClearAllCells()
	{
		for(auto &c : m_cells)
		{
			c.Clear();
		}
	}
		//////////////////////////////////
	///--------- Grid Cell -----------
	//////////////////////////////////
	
	GridCell::GridCell(unsigned a_x, unsigned a_y)
		: m_x(a_x), m_y(a_y)
	{

	}

	void GridCell::AddTile(Tile& a_tile)
	{
		m_tiles.push_back(&a_tile);
	}

	void GridCell::PushTileObject(TileObject& a_object)
	{
		const auto it = m_tileObjectBucket.find(a_object.GetId().Value());

		//Don't add the same object twice to this.
		if(it == m_tileObjectBucket.end())
		{
			m_tileObjectBucket.insert(std::make_pair(a_object.GetId().Value(), &a_object));
		}
	}

	void GridCell::RemoveTileObject(TileObject& a_object)
	{
		const auto it = m_tileObjectBucket.find(a_object.GetId().Value());

		if(it != m_tileObjectBucket.end())
		{
			m_tileObjectBucket.erase(it);
		}
	}

	void GridCell::Clear()
	{
		m_tileObjectBucket.clear();
		m_tiles.clear();
	}
}
