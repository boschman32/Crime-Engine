#pragma once

#include "Pathfinding/TileNode.h"
#include "Pathfinding/Heuristics.h"
#include "Core/Math.h"

namespace pf
{
    class Algorithms
    {
    public:
        Algorithms() = delete;
        Algorithms(const std::vector<std::unique_ptr<TileNode>>& a_tiles, unsigned int a_totalColumns);
        ~Algorithms() = default;

        glm::vec2 AStar(ce::UVector2 a_source, ce::UVector2 a_target) const;

    private:
        unsigned int GetTileIndex(ce::UVector2 a_gridPos) const;
        ce::UVector2 GetGridPos(unsigned int a_index) const;

        unsigned int m_totalColumns;

        std::unique_ptr<Heuristics> m_heuristic;

        const std::vector<std::unique_ptr<TileNode>>& m_tiles;
    };
}


