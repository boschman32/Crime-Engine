#include "smpch.h"
#include "BehaviorTree/Sequence.h"

namespace bt
{
	void Sequence::Initialize()
	{
		m_currentChild = m_childrenNodes.begin();
	}

	EStatus Sequence::Update(float a_deltaTime)
	{
		while (true)
		{
			EStatus s = (*m_currentChild)->Tick(a_deltaTime);

			if (s != EStatus::Success)
			{
				return s;
			}

			// Check if the next node isn't the last one.
			if (++m_currentChild == m_childrenNodes.end())
			{
				return EStatus::Success;
			}
		}
		// Unexpected loop exit.
		return EStatus::Invalid;
	}
}
