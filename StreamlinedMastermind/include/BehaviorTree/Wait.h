#pragma once
#include <chrono>
#include "Behavior.h"

namespace bt
{
	class Wait : public Behavior
	{
	public:
		Wait(float a_seconds) : m_duration(), m_maxTime(a_seconds)
		{
		};
		
		void Initialize() override;
		EStatus Update(float) override;
		
	private:
		std::chrono::time_point<std::chrono::steady_clock> m_startTime;
		std::chrono::time_point<std::chrono::steady_clock> m_endTime;

		std::chrono::duration<float> m_duration;
		float m_maxTime;
	};
}
#pragma once
