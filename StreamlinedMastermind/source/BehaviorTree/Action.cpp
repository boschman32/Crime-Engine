#include "smpch.h"
#include "BehaviorTree\Action.h"

namespace bt
{
	EStatus Action::Update(float)
	{
		if (m_actionFunction)
		{
			return m_actionFunction();
		}
		return EStatus::Invalid;
	}
}
