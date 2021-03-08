#include "smpch.h"

#include "Pathfinding/TileNode.h"

pf::TileNode::TileNode(const unsigned int a_index)
    : m_index(a_index)
{
    m_previousTileIndex = 0;
    m_cost = 0;
    m_pass = 0;
    m_state = EState::Unvisited;

}

pf::TileNode::EState pf::TileNode::GetState() const
{
    return m_state;
}

std::vector<int> pf::TileNode::GetAdjecentTileIndexes() const
{
    return m_adjecentTileIndexes;
}

unsigned pf::TileNode::GetPreviousTileIndex() const
{
    return m_previousTileIndex;
}

unsigned pf::TileNode::GetIndex() const
{
    return m_index;
}

unsigned pf::TileNode::GetCost() const
{
    return m_cost;
}

unsigned pf::TileNode::GetPass() const
{
    return m_pass;
}

void pf::TileNode::SetState(const EState a_state)
{
    m_state = a_state;
}

void pf::TileNode::SetAdjecentTileIndexes(const unsigned int a_number)
{
    m_adjecentTileIndexes.push_back(a_number);
}

void pf::TileNode::SetPreviousTileIndex(const unsigned int a_number)
{
    m_previousTileIndex = a_number;
}

void pf::TileNode::SetIndex(const unsigned a_index)
{
    m_index = a_index;
}

void pf::TileNode::SetCost(const unsigned a_cost)
{
    m_cost = a_cost;
}

void pf::TileNode::SetPass(const unsigned a_pass)
{
    m_pass = a_pass;
}