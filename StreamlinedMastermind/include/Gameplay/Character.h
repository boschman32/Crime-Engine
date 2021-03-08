#pragma once
#include "Gameplay/Unit.h"
#include "Events/CharacterEvent.h"

class Character : public Unit
{
public:
	Character(pf::Graph* a_gameMap, float a_speed = 5.f, glm::vec2 a_currentTilePosition = { 0,0 }, glm::vec2 a_size = { 1,1 }, glm::vec4 a_color = { 1,1,1,1 });

	void OnCreate() override;
	void OnUpdate(float a_deltaTime) override;
	void OnDraw(cr::RenderLayer& a_renderLayer) override;
	void OnDestroy() override;

	CharacterEvent* m_currentEvent = nullptr;

	std::shared_ptr<ce::Texture> m_tex;
	glm::vec2 m_finalPosition { 0.f, 0.f };
};

