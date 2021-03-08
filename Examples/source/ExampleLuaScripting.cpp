#include "expch.h"
#include "ExampleLuaScripting.h"
#include "Gameplay/Scripting/LuaService.h"
#include "Gameplay/GameObjectManager.h"
#include "Gameplay/Map/LevelService.h"
#include "Core/Application.h"
#include "Utils/ServiceManager.h"

namespace ce
{
	namespace examples
	{
		//Player class
		class Player : public GameObject
		{
		public:
			Player(scripting::LuaService& a_luaService)
				: m_luaService(&a_luaService) { };
			
			void OnCreate() override
			{
				//Register two function for retrieving either X and Y to keep it simplistic (not able to return the glm::vec2 itself).
				m_luaService->RegisterFunction("GetPlayerPositionX", [pos = &m_pos]() -> int
				{
					return int(pos->x);
				});
				m_luaService->RegisterFunction("GetPlayerPositionY", [pos = &m_pos]() -> int
				{
					return int(pos->y);
				});
			}
			
			void OnUpdate(float) override { }
			void OnDraw(cr::RenderLayer &) override { }
			
			void OnDestroy() override
			{
				//Deregister any function so lua won't call those any more.
				ServiceManager::GetService<scripting::LuaService>()->UnRegisterFunction("GetPlayerPositionX");
				ServiceManager::GetService<scripting::LuaService>()->UnRegisterFunction("GetPlayerPositionY");
			}

			//Do damage to the player!
			void DoDamage(int a_damage)
			{
				CE_CORE_ERROR("Do damage({0}) to player!", a_damage);
				m_health -= a_damage;
				CE_CORE_INFO("Player health left: {0}", m_health);

				if(m_health <= 0)
				{
					CE_CORE_CRITICAL("Player is dead!");
				}
			}
		private:
			glm::vec2 m_pos { 1, 2 };
			int m_health { 300 };
			
			scripting::LuaService* m_luaService;
		};

		//Base class for the guard
		class IGuard : public GameObject
		{
		public:
			IGuard(scripting::LuaService& a_luaService)
				: m_luaService(&a_luaService)
			{ }
			// Inherited via GameObject
			void OnCreate() override { }
			void OnUpdate(float) override { }
			void OnDraw(cr::RenderLayer&) override { }
			void OnDestroy() override { }

			//What happens if we have seen a player?
			virtual void PlayerSeen(uint32_t a_playerId) = 0;

			//Move the guard to a position.
			void OnMove(const glm::vec2& a_pos)
			{
				CE_CORE_INFO("Move guard from {0} to {1}, ", glm::to_string(m_pos), glm::to_string(a_pos));
				m_pos = a_pos;
			}

			//Set the angle of the field of vision.
			void SetFoV(float a_fov)
			{
				m_fov = a_fov;
				CE_CORE_INFO("Guard({0}) set fov to: {1}", GetName(), m_fov);
			}

			const glm::vec2& GetPos() const { return m_pos; }
		protected:
			float m_fov { 0.f };
			glm::vec2 m_pos { 0, 0 };

			//We can store the luaService if we want (up to the implementation).
			scripting::LuaService* m_luaService;
		};

		//GuardA extending form the base.
		class GuardA : public IGuard
		{
		public:
			GuardA(scripting::LuaService& a_luaService)
				: IGuard(a_luaService) { };
			
			void OnCreate() override
			{
				//Calling different function inside (since we don't support objects) a different lua script (GuardA.lua).
				m_luaService->InvokeFunction("OnStartGuardA", GetId().Value());
			}

			void PlayerSeen(uint32_t a_playerId) override
			{
				//Tell the lua script that we have seen the player let Lua handle whatever it needs to do.
				m_luaService->InvokeFunction("OnPlayerSeenGuardA", GetId().Value(), a_playerId);
			}

			void Shoot(int a_gunDmg, uint32_t a_playerId) const
			{
				const auto player = ServiceManager::GetService<GameObjectManager>()->GetGameObject<Player>(GameObjectId(a_playerId));

				if(player != nullptr)
				{
					CE_CORE_INFO("Guard({0}) is going to shoot for: {1} damage", GetName(), a_gunDmg);
					player->DoDamage(a_gunDmg);
				}
			}
		};

		class GuardB : public IGuard
		{
		public:
			GuardB(scripting::LuaService& a_luaService)
				: IGuard(a_luaService) { };
			
			void OnCreate() override
			{
				//Calling different function inside (since we don't support objects) a different lua script (GuardB.lua).
				m_luaService->InvokeFunction("OnStartGuardB", GetId().Value());
			}

			void PlayerSeen(uint32_t a_playerId) override
			{
				//Tell the lua script that we have seen the player let Lua handle whatever it needs to do.
				m_luaService->InvokeFunction("OnPlayerSeenGuardB", GetId().Value(), a_playerId);
			}

			void Melee(int a_meleeDmg, uint32_t a_playerId) const
			{
				const auto player = ServiceManager::GetService<GameObjectManager>()->GetGameObject<Player>(GameObjectId(a_playerId));

				if(player != nullptr)
				{
					CE_CORE_INFO("Guard({0}) is going to shoot for: {1} damage", GetName(), a_meleeDmg);
					player->DoDamage(a_meleeDmg);
				}
			}
		};
		
		void OnSetFov(uint32_t a_guardId, float a_fov);

		void ExampleLuaScripting::RunExample()
		{
			ServiceManager::MakeService<LevelService>();
			auto gm = ServiceManager::MakeService<GameObjectManager>();
			auto ls = ServiceManager::MakeService<scripting::LuaService>();

			const auto player = gm->CreateGameObject<Player>("Player", *ls);
			
			//Try to find any script files inside ./data/scripts and attempt to run them.
			ls->FindAndRunScriptFiles();

			//Also able to run and load a script separately.
			ls->RunScriptFile("./data/scripts/GuardA.lua");

			//Register global functions that will grab the returned Id and check who it belongs to.
			//Doesn't have to use lambda's supports any function type. (even member functions).
			ls->RegisterFunction("GuardSetFoV", OnSetFov);
			ls->RegisterFunction("GetGuardPosition", [gm](uint32_t a_guardId) -> glm::vec2
			{
				const auto go = gm->GetGameObject<IGuard>(GameObjectId(a_guardId));
				if(go != nullptr)
				{
					return go->GetPos();
				}
				return {0, 0};
			});
			ls->RegisterFunction("GuardMove", [gm](uint32_t a_guardId, int a_x, int a_y)
			{
				const auto go = gm->GetGameObject<IGuard>(GameObjectId(a_guardId));
				if(go != nullptr)
				{
					go->OnMove({a_x, a_y});
				}
			});

			ls->RegisterFunction("GuardShoot", [gm](uint32_t a_guardId, uint32_t a_otherId, int a_gunDmg)
			{
				const auto go = gm->GetGameObject<GuardA>(GameObjectId(a_guardId));
				if(go != nullptr)
				{
					go->Shoot(a_gunDmg, a_otherId);
				}
			});
			ls->RegisterFunction("GuardMelee", [gm](uint32_t a_guardId, uint32_t a_otherId, int a_meleeDmg)
			{
				const auto go = gm->GetGameObject<GuardB>(GameObjectId(a_guardId));
				if(go != nullptr)
				{
					go->Melee(a_meleeDmg, a_otherId);
				}
			});

			//Create x Number of guards
			const int numGuards = 10;
			std::vector<IGuard*> createdGuards;
			createdGuards.reserve(numGuards);
			
			const int numGuardsA = numGuards / 2;
			for(int i = 0; i < numGuardsA; ++i)
			{
				createdGuards.push_back(gm->CreateGameObject<GuardA>("GuardA_" + std::to_string(i), *ls));
			}
			
			const int numGuardsB = numGuards / 2;
			for(int i = 0; i < numGuardsB; ++i)
			{
				createdGuards.push_back(gm->CreateGameObject<GuardB>("GuardB_" + std::to_string(i), *ls));
			}
			//Tell the GameObjectManager to "update" the GameObjects so they get pushed inside the GameObject list.
			//Calls OnCreate()
			gm->Update(0, AppState::Playing);

			//Tell all the guards that they have seen the player!
			for(auto g : createdGuards)
			{
				g->PlayerSeen(player->GetId().Value());
			}
		}

		void OnSetFov(uint32_t a_guardId, float a_fov)
		{
			auto go = ServiceManager::GetService<GameObjectManager>()->GetGameObject<IGuard>(GameObjectId(a_guardId));
			if(go != nullptr)
			{
				go->SetFoV(a_fov);
			}
		}
	}
}

