#pragma once
#include "Behavior.h"

/// The Composite class can hold multiple children nodes and functions the same as the Composite node.
namespace bt
{
	class Composite : public Behavior
	{
	public:
		void AddChild(std::shared_ptr<Behavior> a_child);
		void RemoveChild(std::shared_ptr<Behavior> a_child);
		void ClearAllChildren();
	protected:
		std::vector<std::shared_ptr<Behavior>> m_childrenNodes;
	};
}
