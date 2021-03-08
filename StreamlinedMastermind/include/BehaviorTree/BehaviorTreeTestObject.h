#pragma once
#include "Gfx/RenderService.h"
#include "Utils/ServiceManager.h"
#include "BehaviorTree/Behavior.h"
#include "Gfx/Drawables/Sprite.h"

namespace bt
{
	class BehaviorTree;
	class MockBehaviorTest;
	class Wait;
	class Repeater;
	
	class BehaviorTreeTestObject
	{
	public:
		BehaviorTreeTestObject();

		void Update(float a_deltaTime);
		void Draw();
		void SetPosition(glm::vec3 a_pos);
		void SetRotation(float a_rot);
		void SetColor(glm::vec4 a_color);
		void SetSize(glm::vec3 a_size);

		ce::Sprite m_sprite;
		ce::RenderService* m_rs;
		std::shared_ptr <BehaviorTree> m_behaviorTree = nullptr;
		std::shared_ptr<MockBehaviorTest> m_mockBehavior = nullptr;
	};

	class MockBehaviorTest : public Behavior
	{
	public:
		EStatus Update(float) override;
		BehaviorTreeTestObject* m_behaviorTreeTestObject = nullptr;
	private:
		int m_counter = 0;
	};
}
