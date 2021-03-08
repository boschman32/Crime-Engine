#pragma once
#include "BehaviorTree/IBehavior.h"

namespace bt
{
	class Behavior : public IBehavior
	{
	public:
		Behavior();
		virtual ~Behavior() = default;
		EStatus Tick(float a_deltaTime = 0);

		EStatus Status() { return m_status; };
	private:
		EStatus m_status;
	};
}
