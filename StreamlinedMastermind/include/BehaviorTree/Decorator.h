#pragma once
#include "Behavior.h"

/// The Decorator class can only hold 1 child node and functions the same as the Composite node.
namespace bt
{
	class Decorator : public Behavior
	{
	public:
		Decorator() = default;
		Decorator(std::shared_ptr<Behavior> a_child) : m_child(std::move(a_child)) {};

		void SetChild(std::shared_ptr<Behavior> a_child);
		
	protected:
		std::shared_ptr<Behavior> m_child;
	};
}
