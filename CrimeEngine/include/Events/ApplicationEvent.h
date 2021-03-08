#pragma once
#include <sstream>

#include "EventService.h"

namespace ce
{
	class WindowResizeEvent : public IEvent
	{
	public:
		WindowResizeEvent(unsigned a_width, unsigned a_height)
			: m_width(a_width), m_height(a_height) { }

		unsigned GetWidth() const { return m_width; }
		unsigned GetHeight() const { return m_height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "[Window resize event]: (" << m_width << " x " << m_height << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType, WindowResize);
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication);
	private:
		unsigned m_width;
		unsigned m_height;
	};
}
