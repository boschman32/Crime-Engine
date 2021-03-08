#include "smpch.h"
#include "Events/CharacterEventSystem.h"
#include "Events/CharacterEvent.h"
#include "Gameplay/Character.h"

void CharacterEventSystem::AddEvent(Character * a_character, CharacterEvent* a_characterEvent)
{
	std::vector<CharacterEvent*> e;

	bool userExists = false;

	for (auto& c : m_allCharacterEvents)
	{
		if (c.first == a_character)
		{
			e = c.second;
			e.push_back(a_characterEvent);
			c.second = e;
			userExists = true;
		}
	}

	if (!userExists) 
	{
		e.push_back(a_characterEvent);
		m_allCharacterEvents.insert(std::pair<Character*, std::vector<CharacterEvent*>>(a_character, e));
	}
}

void CharacterEventSystem::RemoveEvent(Character * a_character, CharacterEvent * a_charaterEvent, bool a_removeRemainingEvents)
{
	std::vector<CharacterEvent*> e;

	for (auto& c : m_allCharacterEvents) 
	{
		if (c.first == a_character) 
		{
			e = c.second;
			for (int i = 0; i < e.size(); i++) 
			{
				if (e[i] == a_charaterEvent) 
				{
					if (a_removeRemainingEvents) 
					{
						for (int j = 0; j < e.size() - i; j++) 
						{
							e.erase(e.begin() + i + j);
						}
					}
					e.erase(e.begin() + i);
				}
			}
			c.second = e;
		}
	}
}

void CharacterEventSystem::Update(float)
{
	for (auto& c : m_allCharacterEvents) 
	{
		//Checking if the character already has an event and if that event has been finished
		if (c.first->m_currentEvent != nullptr) 
		{
			c.first->m_currentEvent->ExecuteEvent(c.first);
			if (c.first->m_currentEvent->m_state == CharacterEvent::State::Complete) 
			{
				for (int i = 0; i < c.second.size(); i++) 
				{
					if (c.second[i] == c.first->m_currentEvent) 
					{
						c.second.erase(c.second.begin() + i);
						c.first->m_currentEvent = nullptr;
					}
				}
			}
		}
		//Checking if there is a new event for the character
		for (int i = 0; i < c.second.size(); i++) 
		{
			if (c.first->m_currentEvent == nullptr) 
			{
				c.first->m_currentEvent = c.second[i];
				//c.first->m_reachedTarget = false;
			}
		}
	}
}

CharacterEventSystem::~CharacterEventSystem()
{
	for (auto& c : m_allCharacterEvents)
	{
		for (auto & i : c.second) {
			delete i;
		}
	}
}
