#pragma once
#include <iostream>
#include <map>

class Character;
class CharacterEvent;

class CharacterEventSystem
{
public:

	void AddEvent(Character* a_character, CharacterEvent* a_characterEvent);
	void RemoveEvent(Character* a_character, CharacterEvent* a_charaterEvent, bool a_removeRemainingEvents);
	void Update(float a_deltaTime);
	~CharacterEventSystem();

private:
	std::map<Character*, std::vector<CharacterEvent*>> m_allCharacterEvents;
};

