#include "smpch.h"
#include "BehaviorTree/BehaviorTreeTestObject.h"

#include <iostream>

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Sequence.h"
#include "BehaviorTree/Repeater.h"
#include "BehaviorTree/Wait.h"
#include "BehaviorTree/Action.h"

namespace bt
{
	BehaviorTreeTestObject::BehaviorTreeTestObject()
	{
		// Drawing Setup.
		ce::Sprite d;
		m_sprite = d;
		
		m_rs = ce::ServiceManager::GetService<ce::RenderService>();

		SetPosition(glm::vec3(0, 0, 0));
		SetSize(glm::vec3(2, 2, 5.5f));
		SetColor(glm::vec4(1, 1, 1, 1));

		// Behavior Tree
		m_behaviorTree = std::make_unique<BehaviorTree>();
		std::shared_ptr<Sequence> sequence = std::make_shared<Sequence>();
		std::shared_ptr<Repeater> repeater = std::make_shared<Repeater>(2);
		std::shared_ptr<Wait> wait = std::make_shared<Wait>(1.0f);
		std::shared_ptr<Action> action = std::make_shared<Action>();

		action->m_actionFunction = [&]	
		{
			SetColor(glm::vec4(0, 1, 1, 1));
			return EStatus::Success;
		};
		repeater->SetChild(wait);
		m_mockBehavior = std::make_shared<MockBehaviorTest>();
		m_mockBehavior->m_behaviorTreeTestObject = this;

		
		m_behaviorTree->SetRoot(sequence);
		sequence->AddChild(m_mockBehavior);
		sequence->AddChild(repeater);
		sequence->AddChild(action);
		sequence->AddChild(repeater);
	}
	
	void BehaviorTreeTestObject::Update(float a_deltaTime)
	{
		m_behaviorTree->Tick(a_deltaTime);
		Draw();
	}
	
	void BehaviorTreeTestObject::Draw()
	{
		//TODO: Render layer should be passed by the OnDraw function, or store it somewhere.
		//m_sprite.Draw(*rs->GetRenderLayer());
	}
	
	void BehaviorTreeTestObject::SetPosition(glm::vec3 a_pos)
	{
		m_sprite.SetPosition(a_pos);
	}

	void BehaviorTreeTestObject::SetRotation(float a_rot)
	{
		m_sprite.SetRotationZ(a_rot);
	}

	void BehaviorTreeTestObject::SetColor(glm::vec4 a_color)
	{
		m_sprite.SetColor(a_color);
	}

	void BehaviorTreeTestObject::SetSize(glm::vec3 a_size)
	{
		m_sprite.SetScale(a_size);
	}

	EStatus MockBehaviorTest::Update(float)
	{
		if (m_counter % 2 == 0)
		{
			m_behaviorTreeTestObject->SetColor(glm::vec4(1, 0, 1, 1));
		}
		else
		{
			m_behaviorTreeTestObject->SetColor(glm::vec4(1, 1, 1, 1));
		}

		m_counter++;

		return EStatus::Success;
	}
}
