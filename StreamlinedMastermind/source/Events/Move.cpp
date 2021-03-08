#include "smpch.h"
#include "Events/Move.h"
#include <iostream>
#include "Gameplay/Character.h"

Move::Move(glm::vec2 a_target)
{
	m_target = a_target;
}

Move::~Move()
{
}

void Move::ExecuteEvent(Character* a_character)
{
	if (!m_doOnce) 
	{
		a_character->SetNewTargetPosition(m_target);
		m_doOnce = true;
	}
	if (!a_character->m_moving) //Meaning he got there
	{
		m_state = State::Complete;
	}
}
