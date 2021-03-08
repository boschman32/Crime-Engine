#include "smpch.h"
#include "Gameplay/Unit.h"
#include "Gfx/RenderService.h"
#include "Pathfinding/Graph.h"
#include <iostream>

Unit::Unit(pf::Graph* a_gameMap, float a_speed, glm::vec2 a_currentPosition, glm::vec2 a_size, glm::vec4 a_color)
	: m_graph(a_gameMap), m_currentPosition(a_currentPosition), m_nextPosition(a_currentPosition), m_unitStats(a_speed, 1.f, 1.f, 1.f, 1.f)
{
	SetActive(true);
	m_sprite = ce::Sprite();
	m_sprite.SetPosition(glm::vec3(m_currentPosition.x, m_currentPosition.y, 10.f));
	m_sprite.SetScale(glm::vec3(a_size.x, a_size.y, 1));
	m_sprite.SetColor(a_color);
	m_tilePosition = glm::vec3(m_currentPosition.x, m_currentPosition.y, 10.f);
}

void Unit::OnUpdate(float a_deltaTime)
{
	if (m_moving) 
	{
		MoveTowardsTile(a_deltaTime);
	}
}

void Unit::OnDraw(cr::RenderLayer& a_renderLayer)
{
	m_sprite.Draw(a_renderLayer);
}

void Unit::MoveTowardsTile(float a_deltaTime)
{
	if (glm::distance(glm::vec2(m_tilePosition), m_nextPosition) <= 0.05f)
	{
		m_reachTilePercentage = 0;
		glm::vec3 newSpritePosition = glm::vec3(m_nextPosition.x, m_nextPosition.y, 0);
		m_sprite.SetPosition(newSpritePosition);
		m_currentPosition = m_nextPosition;

		if (m_currentPosition == m_targetPosition) 
		{
			m_moving = false;
		}
		else 
		{
            glm::vec2 source = glm::vec2(m_currentPosition.x, m_currentPosition.y);
            glm::vec2 target = glm::vec2(m_targetPosition.x, m_targetPosition.y);
            glm::vec2 newPosition = m_graph->AStarNextPos(source, target);

			m_nextPosition = newPosition;
		}
	}
	else 
	{
		m_reachTilePercentage += a_deltaTime * m_unitStats.m_speed;

		glm::vec3 newPos;
		newPos.x = m_currentPosition.x + m_reachTilePercentage * (m_nextPosition.x - m_currentPosition.x);
		newPos.y = m_currentPosition.y + m_reachTilePercentage * (m_nextPosition.y - m_currentPosition.y);
		newPos.z = 0;

		m_sprite.SetPosition(newPos);
		m_tilePosition = newPos;
	}
}

void Unit::SetNewTargetPosition(glm::vec2 a_newTargetPosition)
{
	if (a_newTargetPosition != m_currentPosition) 
	{
		m_targetPosition = a_newTargetPosition;
		m_moving = true;

        glm::vec2 source = glm::vec2(m_currentPosition.x, m_currentPosition.y);
        glm::vec2 target = glm::vec2(m_targetPosition.x, m_targetPosition.y);
        glm::vec2 newPosition = m_graph->AStarNextPos(source, target);

		m_nextPosition = newPosition;
	}
}

const Stats & Unit::GetUnitStats() const
{
	return m_unitStats;
}

void Unit::SetUnitStats(const Stats & a_newStats)
{
	m_unitStats = a_newStats;
}

void Unit::SetUnitSpeed(float a_newSpeed)
{
	m_unitStats.m_speed = a_newSpeed;
}

void Unit::SetUnitStrengh(float a_newStrengh)
{
	m_unitStats.m_strengh = a_newStrengh;
}

void Unit::SetUnitStealth(float a_newStealth)
{
	m_unitStats.m_stealth = a_newStealth;
}

void Unit::SetUnitSleightOfHand(float a_newSleight)
{
	m_unitStats.m_sleightOfHand = a_newSleight;
}

void Unit::SetUnitStability(float a_newStability)
{
	m_unitStats.m_stability = a_newStability;
}

float Unit::GetUnitSpeed() const
{
	return m_unitStats.m_speed;
}

float Unit::GetUnitStrengh() const
{
	return m_unitStats.m_strengh;
}

float Unit::GetUnitStealth() const
{
	return m_unitStats.m_stealth;
}

float Unit::GetUnitSleathOfHand() const
{
	return m_unitStats.m_sleightOfHand;
}

float Unit::GetUnitStability() const
{
	return m_unitStats.m_stability;
}
