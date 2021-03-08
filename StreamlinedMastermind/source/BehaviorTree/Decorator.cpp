#include "smpch.h"
#include "BehaviorTree/Decorator.h"

namespace bt
{
	void Decorator::SetChild(std::shared_ptr<Behavior> a_child)
	{
		m_child = std::move(a_child);
	}
}
