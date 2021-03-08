#pragma once

#include <sstream>

#include "Events/EventService.h"
#include "LevelMap.h"

namespace ce
{
	class MapLoadedEvent : public IEvent
	{
	public:
		MapLoadedEvent(LevelMap* a_loadedMap)
			: m_loadedMap(a_loadedMap) { }

		LevelMap& GetLoadedMap() const { return *m_loadedMap; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[Map loaded event]: (" << m_loadedMap->GetFileName() << " )";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType, MapLoaded);
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMap);
	private:
		LevelMap* m_loadedMap;
	};

	class MapBuiltEvent : public IEvent
	{
	public:
		MapBuiltEvent(LevelMap* a_builtMap)
			: m_builtMap(a_builtMap) { }
		
		LevelMap& GetBuiltMap() const { return *m_builtMap; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[Map built event]: (" << m_builtMap->GetFileName() << " )";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType, MapBuilt);
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryMap);
	private:
		LevelMap* m_builtMap;
	};
}
