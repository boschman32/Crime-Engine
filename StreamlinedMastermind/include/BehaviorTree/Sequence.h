#pragma once
#include "Composite.h"

/// The Sequence class will execute its children node in order from first to the last entry, but fails if a child fails.
namespace bt
{
	class Sequence : public Composite
	{
	public:
		std::vector<std::shared_ptr<Behavior>>::iterator m_currentChild;

		void Initialize() override;
		EStatus Update(float a_deltaTime) override;
	};
}
