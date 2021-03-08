#include "smpch.h"
#include "BehaviorTree/BehaviorTree.h"

namespace bt
{
	void BehaviorTree::Tick(float a_deltaTime) const
	{
		if (m_root != nullptr)
		{
			m_root->Tick(a_deltaTime);
		}
	}
	void BehaviorTree::SetRoot(std::shared_ptr<Behavior> a_root)
	{
		m_root = std::move(a_root);
	}
}
