#include "smpch.h"

#include "Pathfinding/Heuristics.h"

pf::Heuristics::Heuristics(const unsigned a_totalRows, const unsigned a_totalColumns)
    : m_totalRows(a_totalRows)
    , m_totalColumns(a_totalColumns)
{
    m_totalRows = 0;
}

float pf::Heuristics::Manhattan(const unsigned a_tileIndexA, const unsigned a_tileIndexB) const
{
    const int rowA = int(a_tileIndexA / m_totalColumns);
    const int columnA = int(a_tileIndexA % m_totalColumns);
    const int rowB = int(a_tileIndexB / m_totalColumns);
    const int columnB = int(a_tileIndexB % m_totalColumns);

    return float(abs(rowA - rowB) + abs(columnA - columnB));
}

float pf::Heuristics::Manhattan(const glm::vec2 a_tilePosA, const glm::vec2 a_tilePosB) const
{
    return abs(a_tilePosA.x - a_tilePosB.x) + abs(a_tilePosA.y - a_tilePosB.y);
}

//float pf::Heuristics::Diagonal(const unsigned a_tileIndexA, const unsigned a_tileIndexB)
//{
//    return 0.0f;
//}
//
//float pf::Heuristics::Diagonal(const Vector2f a_tilePosA, const Vector2f a_tilePosB)
//{
//    return 0.0f;
//}
//
//float pf::Heuristics::Euclidean(const unsigned a_tileIndexA, const unsigned a_tileIndexB) const
//{
//    return 0.0f;
//}
//
//float pf::Heuristics::Euclidean(const Vector2f a_tilePosA, const Vector2f a_tilePosB) const
//{
//    return 0.0f;
//}