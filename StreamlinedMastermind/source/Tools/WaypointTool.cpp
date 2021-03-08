#include "smpch.h"
#include "Tools/WaypointTool.h"


#include "Events/EventService.h"
#include "Utils/ServiceManager.h"
#include "Gameplay/Map/LevelService.h"
#include "Gameplay/GameObjectManager.h"
#include "Core/Input/InputManager.h"

#include "Gfx/Camera.h"
#include "Gameplay/Map/LevelMap.h"
#include "Gameplay/Map/GridHelper.h"
#include "Gameplay/Guard.h"
#include "Gameplay/Map/MapEvent.h"

#include "tmxlite/src/detail/pugixml.hpp"

constexpr ce::Vector4 SelectableCell = ce::Vector4(1, 0, 0, 1);
constexpr ce::Vector4 HoveredCell = ce::Vector4(1, 1, 0, 1);
constexpr ce::Vector4 SelectedCell = ce::Vector4(0, 1, 1, 1);

WaypointTool::WaypointTool(const std::string& a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative, ImGuiWindowFlags a_flags)
	: IEditorWindow(a_windowName, a_pos, a_size, a_relative, a_flags)
{
	ce::EventService::Subscribe<ce::MapBuiltEvent>(ce::EventType::MapBuilt, BIND_EVENT_CALLBACK(WaypointTool::OnMapBuilt));
}

WaypointTool::~WaypointTool()
{
	ce::EventService::Unsubscribe<ce::MapBuiltEvent>(ce::EventType::MapBuilt, BIND_EVENT_CALLBACK(WaypointTool::OnMapBuilt));
}

void WaypointTool::Init()
{
	m_gameobjectManager = ce::ServiceManager::GetService<ce::GameObjectManager>();
}

void WaypointTool::Update(float)
{
	if(m_currentLoadedMap == nullptr)
	{
		return;
	}
	
	// Check if a guard is clicked
	const ce::UVector2 currMouseRayOnGrid = CalculateMouseRayOnGrid();
	m_guards = m_gameobjectManager->GetGameObjectsByType<Guard>();

	for (auto& guard : m_guards)
	{
		if (currMouseRayOnGrid == guard->GetGridPosition() && InputManager::IsMouseButtonDown(MouseKeys::LButton))
		{
			m_selectedGuard = guard;
		}
	}

	m_guards.clear();
}

void WaypointTool::DrawContext()
{
#ifdef CE_DEBUG
	if(m_currentLoadedMap == nullptr)
	{
		return;
	}
	
	if (m_selectedGuard != nullptr)
	{
		ImGui::Text("%s", std::string("Selected Guard: " + m_selectedGuard->GetName()).c_str());

		// Allow placement mode to be toggled on or off
		if (ImGui::Button(std::string("Toggle placement " + m_togglePlacement).c_str(), ImVec2(0, 0)))
		{
			m_placementMode = !m_placementMode;

			if (m_placementMode)
			{
				m_togglePlacement = "Off";
			}

			if (!m_placementMode)
			{
				m_togglePlacement = "On";
			}
		}

		if (ImGui::Button("Assign path to guard"))
		{
			// Disable placement mode
			m_placementMode = false;
			m_togglePlacement = "On";
			
			std::vector<glm::vec2> newPatrolPath;

			const ce::UVector2 currGridSize = m_currentLoadedMap->GetGrid().GetGridSize();
			const bool isIsometric = m_currentLoadedMap->GetMapOrientation() == ce::MapOrientation::Isometric;

			for (auto waypoint : m_selectedGridCells)
			{
				newPatrolPath.emplace_back(glm::vec2(ce::grid_helper::ConvertGridToWorldPosition(ce::UVector2(waypoint->m_x, waypoint->m_y), currGridSize, isIsometric)));
			}

			// Assign the new patrol path to the selected guard
			m_selectedGuard->SetPatrolPath(newPatrolPath, true);

			// Save waypoints to map .tmx file
			SaveWaypoints(newPatrolPath);
		}

		if (ImGui::Button("Reset guard path"))
		{
			for (auto& selectedGridCell : m_selectedGridCells)
			{
				selectedGridCell->m_debugColor = SelectableCell;
			}

			m_selectedGridCells.clear();
		}
	}

	// If placement mode is enabled
	if (m_placementMode)
	{
		const ce::UVector2 currMouseRayOnGrid = CalculateMouseRayOnGrid();

		m_currGridCell = m_currentLoadedMap->GetGrid().GetCell(currMouseRayOnGrid);
		
		if (m_hoveredGridCell != nullptr)
		{
			// Show which cell is being hovered above
			if (m_currGridCell == m_hoveredGridCell)
			{
				m_hoveredGridCell->m_debugColor = HoveredCell;

				// Place waypoint if hovered cell is clicked
				if (InputManager::IsMouseButtonDown(MouseKeys::Middle))
				{
					// Check if cell hasn't been selected already
					const auto iter2 = std::find(m_selectedGridCells.begin(), m_selectedGridCells.end(), m_hoveredGridCell);
					if (iter2 == m_selectedGridCells.end())
					{
						std::vector<ce::TileTypes> unselectableTileTypes;
						unselectableTileTypes.emplace_back(ce::TileTypes::Wall);
						unselectableTileTypes.emplace_back(ce::TileTypes::Void);
						unselectableTileTypes.emplace_back(ce::TileTypes::Prop);

						std::vector<ce::TileTypes> tileTypes;
						for (const auto& tile : m_hoveredGridCell->GetTiles())
						{
							tileTypes.emplace_back(tile->GetTileType());
						}

						auto result = std::find_first_of(unselectableTileTypes.begin(), unselectableTileTypes.end(), tileTypes.begin(), tileTypes.end());
						if (result == unselectableTileTypes.end())
						{
							m_selectedGridCells.push_back(m_hoveredGridCell);
						}
					}
				}
			}
			else
			{
				m_hoveredGridCell->m_debugColor = SelectableCell;
			}
		}

		m_hoveredGridCell = m_currGridCell;

		if (!m_selectedGridCells.empty())
		{
			for (auto& selectedGridCell : m_selectedGridCells)
			{
				selectedGridCell->m_debugColor = SelectedCell;
			}
		}
	}
#endif
}

void WaypointTool::OnMapBuilt(ce::MapBuiltEvent& a_event)
{
	m_currentLoadedMap = &a_event.GetBuiltMap();
	
	m_placementMode = false;
	m_togglePlacement = "On";
	
	m_selectedGuard = nullptr;
	
	if (!m_selectedGridCells.empty())
	{
		for (auto& selectedGridCell : m_selectedGridCells)
		{
#ifdef _DEBUG
			selectedGridCell->m_debugColor = SelectableCell;
#endif
		}
	}
	m_selectedGridCells.clear();
	
	ReadAndSetWaypoints();
}

ce::UVector2 WaypointTool::CalculateMouseRayOnGrid()
{	
	ce::Vector3 currMouseRay = ce::Camera::GetMainCamera()->CalculateMouseRay();
	ce::UVector2 currGridSize = m_currentLoadedMap->GetGrid().GetGridSize();
	bool isIsometric = m_currentLoadedMap->GetMapOrientation() == ce::MapOrientation::Isometric;

	return ce::grid_helper::ConvertToGridPosition(currMouseRay, currGridSize, isIsometric);
}

void WaypointTool::ReadAndSetWaypoints()
{
	// Read from current map tmx file and set selected waypoints
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(std::string("./data/maps/" + m_currentLoadedMap->GetFileName() + "_Waypoints.xml").c_str(), pugi::parse_default);

	if (!result)
	{
		CE_APP_ERROR("Could not read waypoints from map .tmx file. Reason: Map .tmx file not found");
	}

	std::vector<Guard*> guards = m_gameobjectManager->GetGameObjectsByType<Guard>();
	std::vector<glm::vec2> assignedPatrolPath;

	for (auto guard : guards)
	{
		pugi::xml_node nodeChild = doc.child(guard->GetName().c_str());

		for (auto& it : nodeChild)
		{
			float waypointPosX = it.first_attribute().as_float();
			float waypointPosY = it.last_attribute().as_float();

			assignedPatrolPath.emplace_back(glm::vec2(waypointPosX, waypointPosY));
		}

		guard->SetPatrolPath(assignedPatrolPath, true);
	}
}

void WaypointTool::SaveWaypoints(const std::vector<glm::vec2>& a_patrolPath)
{
	// Load current map tmx file			
	pugi::xml_document doc;

	// Save waypoints to map tmx file
	doc.remove_child(m_selectedGuard->GetName().c_str());
	pugi::xml_node guard = doc.append_child(m_selectedGuard->GetName().c_str());

	for (size_t i = 0; i < a_patrolPath.size(); i++)
	{
		pugi::xml_node waypoint = guard.append_child(std::string("Waypoint" + std::to_string(i)).c_str());
		waypoint.append_attribute("X") = std::string(std::to_string(a_patrolPath[i].x)).c_str();
		waypoint.append_attribute("Y") = std::string(std::to_string(a_patrolPath[i].y)).c_str();
	}

	std::string a = m_currentLoadedMap->GetPath();
	
	const bool saveSucceeded = doc.save_file(std::string("./data/maps/" + m_currentLoadedMap->GetFileName() + "_Waypoints.xml").c_str(), PUGIXML_TEXT("  "));
	CE_ASSERT((saveSucceeded));
}