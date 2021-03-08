#include "smpch.h"
#include "./BehaviorTree/Behavior.h"

namespace bt
{
	Behavior::Behavior()
	{
		m_status = EStatus::Invalid;
	}

	EStatus Behavior::Tick(float a_deltaTime)
	{
		if (m_status != EStatus::Running)
		{
			Initialize();
		}

		m_status = Update(a_deltaTime);

		if (m_status != EStatus::Running)
		{
			Terminate(m_status);
		}

		return m_status;
	}
}
