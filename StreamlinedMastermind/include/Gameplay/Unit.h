#pragma once
#include <vec2.hpp>

#include "Gameplay/Map/LevelService.h"
#include "Utils/ServiceManager.h"
#include "Gameplay/TileObject.h"
#include "Gfx/GFXHelper.h"
#include "Gfx/Drawables/Sprite.h"
#include "Pathfinding/Graph.h"
#include "Gameplay/Tile.h"

struct Stats
{
public:
	float m_speed;
	float m_strengh;
	float m_stealth;
	float m_sleightOfHand;
	float m_stability;

	Stats(const float& a_speed, const float& a_strengh, const float& a_stealth, const float& a_sleight, const float& a_stability) :
		m_speed(a_speed), m_strengh(a_strengh), m_stealth(a_stealth), m_sleightOfHand(a_sleight), m_stability(a_stability)
	{}
};

class Unit : public ce::TileObject
{
public:
	Unit(pf::Graph* a_gameMap, float a_speed = 5.f, glm::vec2 a_currentPosition = { 0,0 }, glm::vec2 a_size = { 1,1 }, glm::vec4 a_color = { 1,1,1,1 });
	~Unit() override = default;

	void OnCreate() override {}
	void OnUpdate(float a_deltaTime) override;
	void OnDraw(cr::RenderLayer& a_renderLayer) override;
	void OnDestroy() override {}

	void MoveTowardsTile(float a_deltaTime);
	void SetNewTargetPosition(glm::vec2 a_newTargetPosition);

	const Stats& GetUnitStats() const;
	void SetUnitStats(const Stats& a_newStats);

	void SetUnitSpeed(float a_newSpeed);
	void SetUnitStrengh(float a_newStrengh);
	void SetUnitStealth(float a_newStealth);
	void SetUnitSleightOfHand(float a_newSleight);
	void SetUnitStability(float a_newStability);

	float GetUnitSpeed() const;
	float GetUnitStrengh() const;
	float GetUnitStealth() const;
	float GetUnitSleathOfHand() const;
	float GetUnitStability() const;

	ce::Sprite m_sprite;
	pf::Graph* m_graph; //make this private later

	glm::vec2 m_currentPosition;
	bool m_moving = false;

protected:
	glm::vec2 m_nextPosition;
	glm::vec2 m_targetPosition;

	Stats m_unitStats;

	//How far is the unit from reaching the next tile 1 = reached;
	float m_reachTilePercentage = 0;

	ce::LevelService* m_levelService;
};

