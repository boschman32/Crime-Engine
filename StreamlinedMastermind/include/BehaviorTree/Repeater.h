#pragma once
#include "Decorator.h"

/// The Repeater Decorator executes its child Behavior by the input given by the user.
namespace bt
{
	class Repeater : public Decorator
	{
	public:
		Repeater() = default;
		Repeater(int a_limit) : m_limit(a_limit) {};
		EStatus Update(float a_deltaTime) override;

	private:
		int m_limit = 1;
		int m_counter = 0;
	};
}