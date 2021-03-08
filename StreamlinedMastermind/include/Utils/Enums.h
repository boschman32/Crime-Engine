#pragma once
#include "Events/EventService.h"

enum class GameEvents : std::underlying_type_t<ce::EventType>
{
	None = std::underlying_type_t<ce::EventType>(ce::EventType::Last)
};