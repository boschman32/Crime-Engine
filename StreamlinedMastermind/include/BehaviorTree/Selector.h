#pragma once
#include "Composite.h"

/// The Selector class will execute its children node in order from first to the last entry and wont stop if a child fails.
namespace bt
{
	class Selector : public Composite
	{
	public:
		std::vector<std::shared_ptr<Behavior>>::iterator m_currentChild;

		void Initialize() override;
		EStatus Update(float a_deltaTime) override;
	};
}

