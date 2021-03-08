#include "smpch.h"
#include "BehaviorTree/Composite.h"

namespace bt
{
	void Composite::AddChild(std::shared_ptr<Behavior> a_child)
	{
		m_childrenNodes.push_back(a_child);
	}

	void Composite::RemoveChild(std::shared_ptr<Behavior> a_child)
	{
		auto it = std::find(m_childrenNodes.begin(), m_childrenNodes.end(), a_child);
		if (it != m_childrenNodes.end()) { m_childrenNodes.erase(it); }
	}

	void Composite::ClearAllChildren()
	{
		m_childrenNodes.clear();
	}
}
