#include "smpch.h"
#include "BehaviorTree/Wait.h"

namespace bt
{
	void Wait::Initialize()
	{
		m_startTime = std::chrono::high_resolution_clock::now();
	}
	
	EStatus Wait::Update(float)
	{
		m_endTime = std::chrono::high_resolution_clock::now();
		m_duration = m_endTime - m_startTime;

		if (m_duration.count() >= m_maxTime)
		{
			return EStatus::Success;
		}
		
		return EStatus::Running;
	}
}
