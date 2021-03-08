#include "smpch.h"
#include "BehaviorTree/Repeater.h"

namespace bt
{
	EStatus Repeater::Update(float a_deltaTime)
	{
		while (m_child != nullptr)
		{
			m_child->Tick(a_deltaTime);
			
			if (m_child->Status() == EStatus::Running)
			{
				return EStatus::Running;
			}
			
			if (m_child->Status() == EStatus::Failure)
			{
				return EStatus::Failure;
			}
			
			if (++m_counter == m_limit)
			{
				m_counter = 0;
				return EStatus::Success;
			}

			// Child has Succeeded, but limit isn't reached.
			return EStatus::Running;
		}
		return EStatus::Invalid;
	}
}
