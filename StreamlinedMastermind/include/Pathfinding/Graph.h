#pragma once
#include "Pathfinding/TileNode.h"
#include "Pathfinding/Algorithms.h"
#include "Gameplay/Tile.h"
#include "Gameplay/Map/LevelMap.h"

#include <vector>



namespace pf
{
    class Graph
    {
    public:
        Graph() = default;
        Graph(ce::LevelMap* a_map);
        ~Graph() = default;

        void SetupGraph(ce::LevelMap* a_map);
        glm::vec2 AStarNextPos(glm::vec2 a_source, glm::vec2 a_target) const;
        std::vector<glm::vec2> AStarPath(glm::vec2 a_source, glm::vec2 a_target) const;


    private:
        void CreateGraph();
        void SetAdjacentTileNumbers(unsigned int a_row, unsigned int a_column);
        void SetUnwalkables(std::vector<ce::Tile*>& a_tileMap);

        unsigned int    m_totalRows;
        unsigned int    m_totalColumns;

        std::vector<std::unique_ptr<TileNode>> m_tileNodes;

        std::vector<ce::Tile*> m_tileMap{};
        ce::LevelMap* m_map;

        std::unique_ptr<Algorithms> m_algorithm;
    };
}

