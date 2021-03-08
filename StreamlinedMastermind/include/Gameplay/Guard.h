#pragma once
#include <vector>
#include "Gameplay/Unit.h"
#include "Utils/ServiceManager.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Gfx/GFXHelper.h"

namespace ce
{
	class Grid;
}

class Character;

class Guard : public Unit
{
public:
	Guard(pf::Graph* a_playMap, float a_speed = 5.f, glm::vec2 a_currentTilePosition = { 0,0 }, glm::vec2 a_size = { 1,1 }, glm::vec4 a_color = { 1,1,1,1 });
	~Guard() override = default;

	void OnCreate() override;
	void OnDestroy() override;
	void OnUpdate(float a_deltaTime) override;
	void SetPatrolPath(const std::vector<glm::vec2>& a_newPath, bool a_loopable);

	void PauseGuardMovement();
	void ResumeGuardMovement();

private:
	void SetupBehavior();
	void FindNearestPatrolPoint();
	bool HasReachedPosition();
	void GetNextPatrolPoint();

	void NewFovAngle(glm::vec2 a_lookPos);
	void CalculateFoV();
	bool SearchPlayerInView();
	
	void StartChasing(Unit* a_targetUnit);
	
	bool m_chasing = false;
	bool m_idle = true;

	std::unique_ptr<bt::BehaviorTree> m_guardBehavior;

	std::shared_ptr<ce::Texture> m_tex;
	
	int m_currPoint = 0;
	std::vector<glm::vec2> m_patrolPoints;
	bool m_loopPath = true;
	std::vector<Character*> m_characters;
	Unit* m_targetPlayer = nullptr;
	
	// FoV
	ce::Grid* m_grid = nullptr;
	glm::vec2 GetVectorFromAngle(float a_angle) const;
	std::vector<glm::vec2> BresenhamLine(glm::vec2 a_p0, glm::vec2 a_p1);
	std::vector<glm::vec2> BresenhamLine(int a_x0, int a_y0, int a_x1, int a_y1);

	std::map <std::pair<int, int>, std::vector<ce::Tile*>> m_detectedTiles;

	int m_rayCount = 8;
	float m_directionAngle;
	float m_fov = 90;
	float m_angle = 0;
	float m_startingAngle = 0;
	float m_angleIncrease = 0;
	float m_viewDistance = 4;
};
