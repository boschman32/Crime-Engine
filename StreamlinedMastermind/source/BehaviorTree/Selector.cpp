#include "smpch.h"
#include "BehaviorTree/Selector.h"

namespace bt
{
	void Selector::Initialize()
	{
		m_currentChild = m_childrenNodes.begin();
	}

	EStatus Selector::Update(float a_deltaTime)
	{
		while (m_currentChild != m_childrenNodes.end())
		{
			EStatus s = (*m_currentChild)->Tick(a_deltaTime);
			
			if (s != EStatus::Failure)
			{
				return s;
			}
			
			// Check if the next node isn't the last one.
			if (++m_currentChild == m_childrenNodes.end())
			{
				return EStatus::Failure;
			}
		}
		// Unexpected loop exit.
		return EStatus::Invalid;
	}
}
