#pragma once
#include <functional>
#include "Decorator.h"

namespace bt
{
	class Condition : public Decorator
	{	
	public:
		std::function<bool()> m_canRun;

		EStatus Update(float a_deltaTime) override;
	};
}
