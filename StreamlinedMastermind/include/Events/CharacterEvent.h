#pragma once

class Character;

class CharacterEvent
{
public:
	virtual void ExecuteEvent(Character* a_character) = 0;
	virtual ~CharacterEvent();
	enum State {
		Complete,
		InProgress,
		Idle
	};
	State m_state = State::Idle;
};

