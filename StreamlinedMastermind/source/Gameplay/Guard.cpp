#include "smpch.h"
#include "Gameplay/Guard.h"

#include "Gfx/RenderService.h"

#include "Gameplay/GameObjectManager.h"
#include "Core/ResourceManager.h"

#include "Core/Input/InputManager.h"
#include "Core/Input/KeyEnums.h"

#include "BehaviorTree/Selector.h"
#include "BehaviorTree/Sequence.h"
#include "BehaviorTree/Condition.h"
#include "BehaviorTree/Action.h"
#include "Gameplay/Character.h"

#include "Pathfinding/Graph.h"
#include "Gameplay/Map/Grid.h"
#include "Gameplay/Map/GridHelper.h"


Guard::Guard(pf::Graph* a_playMap, float a_speed, glm::vec2 a_currentTilePosition, glm::vec2 a_size, glm::vec4 a_color) :
	Unit(a_playMap, a_speed, a_currentTilePosition, a_size, a_color)
{
	m_grid = &ce::ServiceManager::GetService<ce::LevelService>()->GetCurrentLoadedMap()->GetGrid();
	m_tex = ce::ServiceManager::GetService<ce::ResourceManager>()->LoadResource<ce::Texture>("data/textures/thief.png");
	ce::Vector4 rect = { 256,0,128,128 };
	m_sprite.SetTexture(*m_tex, rect);
}

void Guard::OnCreate()
{
	SetupBehavior();
	m_characters = ce::ServiceManager::GetService<ce::GameObjectManager>()->GetGameObjectsByType<Character>();
}

void Guard::OnDestroy()
{
	ce::ServiceManager::GetService<ce::ResourceManager>()->ReleaseResource(m_tex);
}

void Guard::OnUpdate(float a_deltaTime)
{
	Unit::OnUpdate(a_deltaTime);
	if (!m_idle) {
		m_guardBehavior->Tick(a_deltaTime);
		CalculateFoV();
	}
}

void Guard::SetPatrolPath(const std::vector<glm::vec2>& a_newPath, bool a_loopable)
{
	m_currPoint = -1;
	m_patrolPoints = a_newPath;
	m_loopPath = a_loopable;

	if (!m_patrolPoints.empty()) 
	{
		glm::vec2 nextpath = m_patrolPoints[0];

		for (auto patrolPoint : m_patrolPoints) 
		{
			std::vector<glm::vec2> allPositions = m_graph->AStarPath(nextpath, patrolPoint);
			nextpath = patrolPoint;

			for (auto& allPosition : allPositions)
			{
				//ce::GridCell* colorCell = map->GetGrid().GetCellWithWorld(allPosition.x, allPosition.y);
				ce::Tile* t = ce::ServiceManager::GetService<ce::LevelService>()->GetCurrentLoadedMap()->GetTileByLayer({ allPosition.x, allPosition.y, 0.f }, "Floor");

				t->m_sprite.SetColor(glm::vec4(1, 0, 0, 1));
			}
		}

		std::vector<glm::vec2> allPositions = m_graph->AStarPath(m_patrolPoints[m_patrolPoints.size() - 1], m_patrolPoints[0]);
		for (auto& allPosition : allPositions)
		{
			//ce::GridCell* colorCell = map->GetGrid().GetCellWithWorld(allPosition.x, allPosition.y);
			ce::Tile* t = ce::ServiceManager::GetService<ce::LevelService>()->GetCurrentLoadedMap()->GetTileByLayer({ allPosition.x, allPosition.y, 0.f }, "Floor");

			t->m_sprite.SetColor(glm::vec4(1, 0, 0, 1));
		}
	}
}

void Guard::SetupBehavior()
{
	//Setup of the behavior tree
	m_guardBehavior = std::make_unique<bt::BehaviorTree>();
	std::shared_ptr<bt::Selector> startSelector = std::make_unique<bt::Selector>();
	//Chase behavior
	std::shared_ptr<bt::Sequence> chaseSeq = std::make_unique<bt::Sequence>();
	std::shared_ptr<bt::Condition> chaseCond = std::make_unique<bt::Condition>();
	chaseCond->m_canRun = [&]
	{
		return m_chasing;
	};
	std::shared_ptr<bt::Sequence> search = std::make_unique<bt::Sequence>();
	std::shared_ptr<bt::Condition> cannotFind = std::make_unique<bt::Condition>();
	cannotFind->m_canRun = [&]
	{
		// Check if the player is inside the Fov
		if (!SearchPlayerInView()) {
			return true;
		}
		SetNewTargetPosition(m_targetPlayer->m_currentPosition);
		return false;
	};
	std::shared_ptr<bt::Condition> hasReachedPoint = std::make_unique<bt::Condition>();
	hasReachedPoint->m_canRun = [&]
	{
		return HasReachedPosition() || !m_moving;
	};
	std::shared_ptr<bt::Action> backToPatrol = std::make_unique<bt::Action>();
	backToPatrol->m_actionFunction = [&]
	{
		m_chasing = false;
		FindNearestPatrolPoint();
		return bt::EStatus::Success;
	};
	search->AddChild(cannotFind);
	search->AddChild(hasReachedPoint);
	chaseSeq->AddChild(chaseCond);
	chaseSeq->AddChild(search);
	chaseSeq->AddChild(backToPatrol);

	//Search behavior
	std::shared_ptr<bt::Sequence> searchSeq = std::make_unique<bt::Sequence>();
	std::shared_ptr<bt::Condition> canFindTarget = std::make_unique<bt::Condition>();
	canFindTarget->m_canRun = [&]
	{
		return SearchPlayerInView();
	};
	std::shared_ptr<bt::Action> startChase = std::make_unique<bt::Action>();
	startChase->m_actionFunction = [&]
	{
		ce::Logger::PrintMessageToConsole("Start chasing!!!!");
		StartChasing(m_targetPlayer);
		return bt::EStatus::Success;
	};
	searchSeq->AddChild(canFindTarget);
	searchSeq->AddChild(startChase);

	//Patrol behavior
	std::shared_ptr<bt::Sequence> patrolSeq = std::make_unique<bt::Sequence>();
	std::shared_ptr<bt::Action> getNextPatrolPoint = std::make_unique<bt::Action>();
	getNextPatrolPoint->m_actionFunction = [&]
	{
		GetNextPatrolPoint();
		return bt::EStatus::Success;
	};
	patrolSeq->AddChild(hasReachedPoint);
	patrolSeq->AddChild(getNextPatrolPoint);

	m_guardBehavior->SetRoot(startSelector);
	startSelector->AddChild(chaseSeq);
	startSelector->AddChild(searchSeq);
	startSelector->AddChild(patrolSeq);
}

void Guard::FindNearestPatrolPoint()
{
	if (m_patrolPoints.empty()) {
		return;
	}

	float nearestDist = std::numeric_limits<float>::max();
	int nearestPoint = 0;

	glm::vec2 currentPosition = m_sprite.GetPosition();
	//Gets the point with the lowest amount of distance between it and the guard
	//TODO make this calculation with the pathfinding to really get the closest point
	for (int i = 0; i < m_patrolPoints.size(); i++)
	{
		float currDist = glm::distance(m_patrolPoints[i], currentPosition);
		if (currDist < nearestDist) {
			nearestDist = currDist;
			nearestPoint = i;
		}
	}

	SetNewTargetPosition(m_patrolPoints[nearestPoint]);
	m_currPoint = nearestPoint;
}

bool Guard::HasReachedPosition() 
{
	return m_currentPosition == m_targetPosition;
}

void Guard::GetNextPatrolPoint() 
{
	if (m_patrolPoints.empty()) {
		return;
	}

	m_currPoint++;
	if (m_currPoint > m_patrolPoints.size() - 1) {
		if (!m_loopPath) {
			std::reverse(m_patrolPoints.begin(), m_patrolPoints.end());
		}
		m_currPoint = 0;
	}
	SetNewTargetPosition(m_patrolPoints[m_currPoint]);
	NewFovAngle(m_patrolPoints[m_currPoint]);
}

void Guard::NewFovAngle(glm::vec2 a_lookPos)
{
	m_directionAngle = atan2(a_lookPos.y - m_currentPosition.y, a_lookPos.x - m_currentPosition.x) * 180 / 3.14159f;
	if (m_directionAngle < 0)
	{
		m_directionAngle += 360;
	}
	m_startingAngle = m_directionAngle;
}

void Guard::StartChasing(Unit * a_targetUnit)
{
	if (a_targetUnit == nullptr) {
		printf("No unit given! \n");
		return;
	}
	SetNewTargetPosition(a_targetUnit->m_currentPosition);
	m_chasing = true;
}

void Guard::CalculateFoV()
{
	m_angleIncrease = m_fov / static_cast<float>(m_rayCount);

	m_angle = m_startingAngle;
	std::vector<glm::vec2> vertices;
	glm::vec2 guardPos = ce::grid_helper::IsometricToCartesian(GetPosition());
	std::vector<std::vector<glm::vec2>> rays;

	for (int i = 0; i <= m_rayCount; i++)
	{
		glm::vec2 vertex = guardPos + GetVectorFromAngle(m_angle) * m_viewDistance;
		// might not be needed anymore.
		vertices.push_back(vertex);
		rays.push_back(BresenhamLine(guardPos, vertex));

		glm::vec2 rayDir = vertex - guardPos;

		if (rayDir.x > 0 && rayDir.y > 0)
		{
			std::sort(rays[i].begin(), rays[i].end(),
						[guardPos](const glm::vec2& a_lhs, const glm::vec2& a_rhs) { return glm::distance(guardPos, a_lhs) > glm::distance(guardPos, a_rhs); });
		}
		else if (rayDir.x < 0 && rayDir.y < 0)
		{
			std::sort(rays[i].begin(), rays[i].end(),
						[guardPos](const glm::vec2& a_lhs, const glm::vec2& a_rhs) { return glm::distance(guardPos, a_lhs) < glm::distance(guardPos, a_rhs); });
		}
		else if (rayDir.x < 0 && rayDir.y > 0)
		{
			std::sort(rays[i].begin(), rays[i].end(),
						[guardPos](const glm::vec2& a_lhs, const glm::vec2& a_rhs) { return glm::distance(guardPos, a_lhs) < glm::distance(guardPos, a_rhs); });
		}
		else if (rayDir.x > 0 && rayDir.y < 0)
		{
			std::sort(rays[i].begin(), rays[i].end(),
						[guardPos](const glm::vec2& a_lhs, const glm::vec2& a_rhs) { return glm::distance(guardPos, a_lhs) > glm::distance(guardPos, a_rhs); });
		}
		m_angle -= m_angleIncrease;
	}
	m_angle = 90;

	/// Search tiles within FoV range.
	for (auto & ray : rays)
	{
		// loop through rays.
		bool wallHit = false;
		
		for (size_t k = ray.size(); k-- > 0;)
		{
			ce::GridCell* cell = m_grid->GetCellWithWorld(ray[k].x, ray[k].y);
			
			// Is the cell within the grid/valid?
			if (cell != nullptr)
			{
				std::pair<int, int> key = { cell->m_x, cell->m_y };
				const std::vector<ce::Tile*> &tiles = cell->GetTiles();

				for (auto t : tiles)
				{
					if (t->GetTileType() == ce::TileTypes::Wall)
					{
						wallHit = true;
						break;
					}
#if DEBUG_GRID
					/// Color all Tiles within FoV
					cell->m_debugColor = { 0.f, 0.f, 1.f, 1.f };
#endif
					m_detectedTiles.insert(std::pair<std::pair<int, int>, std::vector<ce::Tile*>>(key, tiles));
				}
				
				if (wallHit)
				{
					break;
				}
			}
		}
	}
}

bool Guard::SearchPlayerInView()
{
	if (!m_detectedTiles.empty())
	{
		std::pair<int, int> key;
		std::map<std::pair<int, int>, std::vector<ce::Tile*>>::iterator it;
		
		for (auto character : m_characters)
		{
			key = {character->GetGridPosition().x, character->GetGridPosition().y };

			it = m_detectedTiles.find(key);
			if (it != m_detectedTiles.end())
			{
				// Call Logic to catch player here.
				m_targetPlayer = character;
				m_detectedTiles.clear();
				ce::Logger::PrintMessageToConsole("Player found!");
				return true;
			}
		}
	#if DEBUG_GRID
		for (auto element : m_detectedTiles)
		{
			for (auto & tiles : element.second)
			{
				m_grid->GetCell(tiles->GetGridPosition())->m_debugColor = { 1.f, 0.f, 0.f, 1.f };
			}
			
		}
	#endif
	}

	m_detectedTiles.clear();
	return false;
}

void Guard::PauseGuardMovement()
{
	m_idle = true;
}

void Guard::ResumeGuardMovement()
{
	m_idle = false;
}

glm::vec2 Guard::GetVectorFromAngle(float a_angle) const
{
	float angleRad = (glm::pi<float>() / 180.0f) * a_angle;
	return glm::vec2(std::cos(angleRad), std::sin(angleRad));
}

// Returns the list of points from a_p0 to a_p1 
std::vector<glm::vec2> Guard::BresenhamLine(glm::vec2 a_p0, glm::vec2 a_p1)
{
	return BresenhamLine(int(a_p0.x), int(a_p0.y), int(a_p1.x), int(a_p1.y));
}

// Returns the list of points from (a_x0, a_y0) to (a_x1, a_y1)
std::vector<glm::vec2> Guard::BresenhamLine(int a_x0, int a_y0, int a_x1, int a_y1)
{
	std::vector<glm::vec2> result;

	bool steep = std::abs(a_y1 - a_y0) > std::abs(a_x1 - a_x0);
	if (steep)
	{
		std::swap(a_x0, a_y0);
		std::swap(a_x1, a_y1);
	}
	if (a_x0 > a_x1) {
		std::swap(a_x0, a_x1);
		std::swap(a_y0, a_y1);
	}

	int deltaX = a_x1 - a_x0;
	int deltaY = std::abs(a_y1 - a_y0);
	int error = 0;
	int ystep;
	int y = a_y0;

	if (a_y0 < a_y1)
	{
		ystep = 1;
	}
	else
	{
		ystep = -1;
	}

	for (int x = a_x0; x <= a_x1; x += 1)
	{
		if (steep)
		{
			result.emplace_back(ce::grid_helper::CartesianToIsometric(glm::vec2(y, x)));
		}
		else
		{
			result.emplace_back(ce::grid_helper::CartesianToIsometric(glm::vec2(x, y)));
		}

		error += deltaY;
		if (2 * error >= deltaX)
		{
			y += ystep;
			error -= deltaX;
		}
	}

	return result;
}
