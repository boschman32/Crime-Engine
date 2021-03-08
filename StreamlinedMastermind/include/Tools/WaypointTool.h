#pragma once
#include "Utils/Editor/IEditorWindow.h"
#include "Core/Math.h"

namespace ce
{
	class MapBuiltEvent;
	class LevelService;
	class LevelMap;
	class GridCell;
	class GameObjectManager;
}

class Guard;

class WaypointTool final : public ce::editor::IEditorWindow
{
public:
	WaypointTool(const std::string& a_windowName, const ImVec2& a_pos, const ImVec2& a_size, bool a_relative = true, ImGuiWindowFlags a_flags = ImGuiBackendFlags_None);
	~WaypointTool() override;
	
	void Init() override;
	void Update(float) override;
	void DrawContext() override;

private:
	void OnMapBuilt(ce::MapBuiltEvent& a_event);
	ce::UVector2 CalculateMouseRayOnGrid();
	void ReadAndSetWaypoints();
	void SaveWaypoints(const std::vector<glm::vec2>& a_patrolPath);

	ce::GameObjectManager* m_gameobjectManager = nullptr;
	ce::GridCell* m_currGridCell = nullptr;
	ce::GridCell* m_hoveredGridCell = nullptr;
	Guard* m_selectedGuard = nullptr;

	bool m_placementMode = false;
	std::string m_togglePlacement = "On";
	std::vector<ce::GridCell*> m_selectedGridCells;
	std::vector<Guard*> m_guards;

	ce::LevelMap* m_currentLoadedMap { nullptr };
};