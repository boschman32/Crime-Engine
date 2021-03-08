#pragma once
#include "Events/CharacterEvent.h"

class Move : public CharacterEvent
{
public:
	Move(glm::vec2 a_target);
	~Move() override;
	void ExecuteEvent(Character* a_character) override;
private:
	glm::vec2 m_target;
	bool m_doOnce = false;
};

