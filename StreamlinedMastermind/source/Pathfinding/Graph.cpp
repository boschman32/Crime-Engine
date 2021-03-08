#include "smpch.h"
#include "Pathfinding/Graph.h"
#include "Core/Logger.h"
#include "Gameplay/Map/LevelService.h"
#include "Gameplay/Map/GridHelper.h"

pf::Graph::Graph(ce::LevelMap* a_map)
    : m_map(a_map)
{
    m_totalRows = m_map->GetMapSize().y;
    m_totalColumns = m_map->GetMapSize().x;
    m_tileMap = m_map->GetTiles();

    CreateGraph();
    SetUnwalkables(m_tileMap);

    m_algorithm = std::make_unique<Algorithms>(m_tileNodes, m_totalColumns);
}

void pf::Graph::SetupGraph(ce::LevelMap* a_map)
{
	m_tileNodes.clear();
    m_map = a_map;

    m_totalRows = m_map->GetMapSize().y;
    m_totalColumns = m_map->GetMapSize().x;
    m_tileMap = m_map->GetTiles();

    CreateGraph();
    SetUnwalkables(m_tileMap);

    m_algorithm = std::make_unique<Algorithms>(m_tileNodes, m_totalColumns);
}

glm::vec2 pf::Graph::AStarNextPos(const glm::vec2 a_source, const glm::vec2 a_target) const
{
    const auto sourceGrid = ce::grid_helper::ConvertToGridPosition(a_source, { m_totalColumns, m_totalRows }, true);
    const auto targetGrid = ce::grid_helper::ConvertToGridPosition(a_target, { m_totalColumns, m_totalRows }, true);

    const ce::UVector2 newGridPos = m_algorithm->AStar(sourceGrid, targetGrid);

    glm::vec3 newTilePos = { a_source, 1.0f };

    if (m_map->GetTileByLayer(newGridPos, "Floor") != nullptr)
    {
        newTilePos = m_map->GetTileByLayer(newGridPos, "Floor")->GetTilePosition();
    }
    else
    {
        CE_APP_WARN("No Tile Layer found");
    }

    return { newTilePos.x, newTilePos.y };
}
	
std::vector<glm::vec2> pf::Graph::AStarPath(glm::vec2 a_source, const glm::vec2 a_target) const
{
	OPTICK_EVENT();
    std::vector<glm::vec2> path;
    std::vector<glm::vec2> emptyPath;

    const auto targetGrid = ce::grid_helper::ConvertToGridPosition(a_target, { m_totalColumns, m_totalRows }, true);

    if (m_tileNodes[unsigned(targetGrid.x * float(m_totalColumns) + targetGrid.y)]->GetState() == TileNode::EState::Wall)
    {
        return emptyPath;
    }

    while (a_source != a_target)
    {
        auto oldSource = a_source;

        path.push_back(a_source);
        a_source = AStarNextPos(a_source, a_target);

        if (a_source == oldSource)
        {
            return emptyPath;
        }
    }

    path.push_back(a_source);

    return path;
}

void pf::Graph::CreateGraph()
{
    for (unsigned int row = 0; row < m_totalRows; row++)
    {
        for (unsigned int column = 0; column < m_totalColumns; column++)
        {
            const unsigned int index = row * m_totalColumns + column;

            //  Setup graph
            m_tileNodes.push_back(std::make_unique<TileNode>(index));

            m_tileNodes[index]->SetState(TileNode::EState::Unvisited);

            SetAdjacentTileNumbers(row, column);
        }
    }
}

void pf::Graph::SetAdjacentTileNumbers(const unsigned int a_row, const unsigned int a_column)
{
    const unsigned int index = a_row * m_totalColumns + a_column;

    //  TileNode up
    if (a_row > 0)
    {
        m_tileNodes[index]->SetAdjecentTileIndexes(m_totalColumns * (a_row - 1) + a_column);
    }

    //  TileNode down
    if (a_row < m_totalRows - 1)
    {
        m_tileNodes[index]->SetAdjecentTileIndexes(m_totalColumns * (a_row + 1) + a_column);
    }

    //  TileNode Right
    if (a_column < m_totalColumns - 1)
    {
        m_tileNodes[index]->SetAdjecentTileIndexes(m_totalColumns * a_row + a_column + 1);
    }

    //  TileNode Left
    if (a_column > 0)
    {
        m_tileNodes[index]->SetAdjecentTileIndexes(m_totalColumns * a_row + a_column - 1);
    }
}

void pf::Graph::SetUnwalkables(std::vector<ce::Tile*>& a_tileMap)
{
    for (auto& i : a_tileMap)
    {
        if (i->GetLayer() == "Walls")
        {
            //CE_APP_WARN("GridPos: {0}, {1}", a_tileMap[i]->GetGridPosition().x, a_tileMap[i]->GetGridPosition().y);
            const unsigned int index = i->GetGridPosition().x * m_totalColumns + i->GetGridPosition().y;
            m_tileNodes[index]->SetState(TileNode::EState::Wall);
        }
    }
}
