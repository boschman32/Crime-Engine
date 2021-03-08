#pragma once
#include "BehaviorTree/Behavior.h"

namespace bt
{
	class BehaviorTree
	{
	public:
		void Tick(float a_deltaTime = 0) const;
		void SetRoot(std::shared_ptr<Behavior> a_root);
		
	private:
		std::shared_ptr<Behavior> m_root = nullptr;	
	};
}
