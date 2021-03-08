#include "expch.h"
#include "ExampleMapScene.h"

#include "Scenes/IMapScene.h"
#include "Scenes/SceneManager.h"
#include "Utils/ServiceManager.h"

namespace ce
{
	namespace examples
	{
		class SceneA : public IMapScene
		{
		public:
			SceneA(int a_a, float a_b, const std::string& a_c)
				: m_a(a_a), m_b(a_b), m_c(a_c)
			{
				SelectMap("Derpmap!");
			};

			bool OnSceneSwitched() override
			{
				CE_CORE_INFO("Switched to different scene dispose of all data!");
				return LoadSelectedMap();
			}
			
			void OnDispose() override
			{
				CE_CORE_INFO("We switched to this scene: {0}", GetSceneName());
				
			}

			void OnUpdate(float) override
			{

			}

			void OnDraw(cr::RenderLayer&) override
			{
				
			}

			void OnMapLoaded(std::shared_ptr<LevelMap>&) override
			{
				
			}

			void OnPlan() override
			{
				CE_CORE_INFO("Planning started!");
			}

			void OnSimulation() override
			{
				CE_CORE_INFO("Simulation started!");
			}

			void Simulate(float) override
			{
				CE_CORE_INFO("Simulating!");
			}

			void Planning(float) override
			{
				CE_CORE_INFO("Planning!");
			}

		private:
			int m_a;
			float m_b;
			std::string m_c;
		};

		class SceneB : public IScene
		{
		public:

			// Inherited via IScene
			bool OnSceneSwitched() override
			{
				return true;
			}
			
			void OnDispose() override
			{
				
			}
			
			void OnUpdate(float) override
			{
				
			}
			void OnDraw(cr::RenderLayer&) override
			{
				
			}
		};
		
		void ExampleMapScene::RunExample()
		{
			ServiceManager::MakeService<LevelService>();
			auto sm = ServiceManager::MakeService<SceneManager>();

			sm->AddScene<SceneA>("SceneA", 1, 5.5f, "Test!");
			sm->AddScene<SceneB>("SceneB");

			int x = 0;
			bool running = true;
			while(running)
			{
				sm->Update(0.f);

				CE_CORE_INFO("Press 1 to change to 'SceneA' | Press 2 to change to 'SceneB' | Press 3 to exit!");
				std::cin >> x;

				if(x == 1)
				{
					auto sceneA = sm->SwitchScene<SceneA>("SceneA");
					int y = 0;
					CE_CORE_INFO("Press 1 to change to 'Planning' phase | Press 2 to change to 'Simulation' phase");

					std::cin >> y;

					if(y == 1)
					{
						sceneA->BeginPlan();
					}
					else if(y == 2)
					{
						sceneA->BeginSimulation();
					}
				}
				else if(x == 2)
				{
					sm->SwitchScene<SceneB>("SceneB");
				}
				else if(x == 3)
				{
					running = false;
				}

				if(sm->GetCurrentScene() != nullptr)
				{
					CE_CORE_INFO("Current scene is: {0}", sm->GetCurrentScene()->GetSceneName());
				}
			}
		}
	}
}
