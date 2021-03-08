#include "smpch.h"
#include "Gameplay/Character.h"

#include "Pathfinding/Graph.h"
#include "Core/ResourceManager.h"


Character::Character(pf::Graph* a_gameMap, float a_speed, glm::vec2 a_currentTilePosition, glm::vec2 a_size, glm::vec4 a_color) : Unit::Unit(a_gameMap, a_speed, a_currentTilePosition, a_size, a_color)
{
	m_tex = ce::ServiceManager::GetService<ce::ResourceManager>()->LoadResource<ce::Texture>("data/textures/thief.png");
	ce::Vector4 rect = { 256,0,128,128 };
	m_sprite.SetTexture(*m_tex, rect);
	m_finalPosition = a_currentTilePosition;
}

void Character::OnCreate()
{
}

void Character::OnUpdate(float a_deltaTime)
{
	Unit::OnUpdate(a_deltaTime);
}

void Character::OnDraw(cr::RenderLayer& a_renderLayer)
{
	Unit::OnDraw(a_renderLayer);
}

void Character::OnDestroy()
{
	ce::ServiceManager::GetService<ce::ResourceManager>()->ReleaseResource(m_tex);
}
