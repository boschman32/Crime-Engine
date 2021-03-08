#pragma once
#include "Gameplay/Map/ITileObjectFactory.h"
#include "Pathfinding/Graph.h"

class GameTileObjectFactory : public ce::ITileObjectFactory
{
public:
    GameTileObjectFactory(pf::Graph* a_graph);

    ce::TileObject* CreateObject(const ce::Vector3& a_spawnPos, const std::string& a_name, const std::string& a_type, std::vector<tmx::Property> a_properties) const override;

private:
    static glm::vec4 ConvertColourToVec4(tmx::Colour a_color);
    pf::Graph* m_graph;
};


