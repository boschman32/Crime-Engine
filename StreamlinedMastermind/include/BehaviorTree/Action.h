#pragma once
#include "Behavior.h"

namespace bt
{
	class Action : public Behavior
	{
	public:
		std::function<EStatus()> m_actionFunction;
		EStatus Update(float a_deltaTime) override;
	};
}
