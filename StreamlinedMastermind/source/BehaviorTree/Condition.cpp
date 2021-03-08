#include "smpch.h"
#include "BehaviorTree/Condition.h"

namespace bt
{
	EStatus Condition::Update(float a_deltaTime)
	{
		EStatus s = EStatus::Failure;
		
		if (m_canRun != nullptr && m_canRun())
		{
			s = EStatus::Success;
			if (m_child != nullptr)
			{
				s = m_child->Tick(a_deltaTime);
			}
			
			return s;
		}
		
		return s;
	}
}
