#include "CppUnitTest.h"
#include <memory>
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Condition.h"
#include "BehaviorTree/Behavior.h"
#include "BehaviorTree/Sequence.h"
#include "BehaviorTree/Composite.h"
#include "BehaviorTree/Selector.h"
#include "BehaviorTree/Repeater.h"
#include "BehaviorTree/IBehavior.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GameplayTests
{
	class MockBehavior : public bt::Behavior
	{
	public:
		bool m_initializeCalled = false;
		bool m_updateCalled = false;
		bool m_terminateCalled = false;

		void Initialize()
		{
			m_initializeCalled = true;
		};

		bt::EStatus Update(float a_deltaTime = 0)
		{
			m_updateCalled = true;
			return bt::EStatus::Success;
		};

		void Terminate(bt::EStatus)
		{
			m_terminateCalled = true;
			return;
		};
	};

	TEST_CLASS(BehaviorTree_Test)
	{
	public:

		TEST_METHOD(Behavior_Initialized)
		{
			std::unique_ptr<bt::BehaviorTree> tree = std::make_unique<bt::BehaviorTree>();
			std::shared_ptr<MockBehavior> testBehavior = std::make_shared<MockBehavior>();
			tree->SetRoot(testBehavior);

			tree->Tick();
			Assert::IsTrue(testBehavior->m_initializeCalled);
		}

		TEST_METHOD(Behavior_Updated)
		{
			std::unique_ptr<bt::BehaviorTree> tree = std::make_unique<bt::BehaviorTree>();
			std::shared_ptr<MockBehavior> testBehavior = std::make_shared<MockBehavior>();
			tree->SetRoot(testBehavior);

			tree->Tick();
			Assert::IsTrue(testBehavior->m_updateCalled);
		}

		TEST_METHOD(Behavior_Terminated)
		{
			std::unique_ptr<bt::BehaviorTree> tree = std::make_unique<bt::BehaviorTree>();
			std::shared_ptr<MockBehavior> testBehavior = std::make_shared<MockBehavior>();
			tree->SetRoot(testBehavior);

			tree->Tick();
			Assert::IsTrue(testBehavior->m_terminateCalled);
		}

		TEST_METHOD(Sequence_Single_Child_Node_Pass)
		{
			std::unique_ptr <bt::Sequence> sequence = std::make_unique<bt::Sequence>();
			std::shared_ptr<MockBehavior> child = std::make_shared<MockBehavior>();

			sequence->AddChild(child);

			sequence->Tick();
			Assert::IsTrue(sequence->Status() == static_cast<int>(bt::EStatus::Success));
			Assert::IsTrue(child->m_terminateCalled);
		}

		TEST_METHOD(Sequence_Multiple_Child_Node_Pass)
		{
			std::unique_ptr <bt::Sequence> sequence = std::make_unique<bt::Sequence>();
			std::shared_ptr<MockBehavior> child1 = std::make_shared<MockBehavior>();
			std::shared_ptr<MockBehavior> child2 = std::make_shared<MockBehavior>();

			sequence->AddChild(child1);
			sequence->AddChild(child2);

			sequence->Tick();
			Assert::IsTrue(sequence->Status() == static_cast<int>(bt::EStatus::Success));
			Assert::IsTrue(child1->m_terminateCalled);
			Assert::IsTrue(child2->m_terminateCalled);
		}

		TEST_METHOD(Condition_True_Node_Pass)
		{
			std::shared_ptr<bt::Condition> condition = std::make_shared<bt::Condition>();
			int money = 65;

			condition->m_canRun = [&] {
				if (money > 50)
				{
					return true;
				}
				return false;
			};

			condition->Tick();
			Assert::IsTrue(condition->Status() == static_cast<int>(bt::EStatus::Success));
		}
		
		TEST_METHOD(Condition_False_Node_Pass)
		{
			std::shared_ptr<bt::Condition> condition = std::make_shared<bt::Condition>();
			int health = 80;

			condition->m_canRun = [&] {
				if (health < 60)
				{
					return true;
				}
				return false;
			};

			condition->Tick();
			Assert::IsTrue(condition->Status() == static_cast<int>(bt::EStatus::Failure));
		}

		TEST_METHOD(Condition_Child_Node_Pass)
		{
			std::shared_ptr<bt::Condition> condition = std::make_shared<bt::Condition>();
			std::shared_ptr<bt::Condition> conditionChild = std::make_shared<bt::Condition>();
			std::shared_ptr<MockBehavior> behaviorChild = std::make_shared<MockBehavior>();
			condition->SetChild(conditionChild);
			conditionChild->SetChild(behaviorChild);

			condition->m_canRun = [&] {
				if (true)
				{
					return true;
				}
				return false;
			};

			conditionChild->m_canRun = [&] {
				if (true)
				{
					return true;
				}
				return false;
			};

			condition->Tick();
			Assert::IsTrue(condition->Status() == static_cast<int>(bt::EStatus::Success));
		}

		TEST_METHOD(Selector_First_Child_Node_Failed_Pass)
		{
			std::unique_ptr<bt::Selector> selector = std::make_unique<bt::Selector>();
			std::shared_ptr<bt::Condition> condition = std::make_shared<bt::Condition>();
			std::shared_ptr<MockBehavior> behavior = std::make_shared<MockBehavior>();
			std::shared_ptr<MockBehavior> conditionChild = std::make_shared<MockBehavior>();

			condition->m_canRun = [&] {
				if (false)
				{
					return true;
				}
				return false;
			};

			selector->AddChild(condition);
			condition->SetChild(conditionChild);
			selector->AddChild(behavior);

			selector->Tick();
			Assert::IsTrue(selector->Status() == static_cast<int>(bt::EStatus::Success));
			Assert::IsTrue(behavior->m_terminateCalled);
			Assert::IsFalse(conditionChild->m_terminateCalled);
		}

		TEST_METHOD(Selector_Second_Child_Node_Not_Ticked_Pass)
		{
			std::unique_ptr<bt::Selector> selector = std::make_unique<bt::Selector>();
			std::shared_ptr<bt::Condition> child1 = std::make_shared<bt::Condition>();
			std::shared_ptr<MockBehavior> child2 = std::make_shared<MockBehavior>();

			child1->m_canRun = [&] {
				if (true)
				{
					return true;
				}
				return false;
			};

			selector->AddChild(child1);
			selector->AddChild(child2);

			selector->Tick();
			Assert::IsTrue(selector->Status() == static_cast<int>(bt::EStatus::Success));
			Assert::IsFalse(child2->m_terminateCalled);
		}

		TEST_METHOD(Repeater_Node_Pass)
		{
			std::unique_ptr<bt::Repeater> repeater = std::make_unique<bt::Repeater>(3);
			std::shared_ptr<MockBehavior> child = std::make_shared<MockBehavior>();

			repeater->SetChild(child);

			repeater->Tick();
			Assert::IsTrue(repeater->Status() == static_cast<int>(bt::EStatus::Running));
			repeater->Tick();
			Assert::IsTrue(repeater->Status() == static_cast<int>(bt::EStatus::Running));
			repeater->Tick();
			Assert::IsTrue(repeater->Status() == static_cast<int>(bt::EStatus::Success));
		}
	};

	TEST_CLASS(DummyClass)
	{
	public:

		TEST_METHOD(DummyTest)
		{
			Assert::IsTrue(true);
		}
	};
}
