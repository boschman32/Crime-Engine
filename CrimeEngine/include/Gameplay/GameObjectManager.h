#pragma once
#include "Utils/IService.h"
#include "Core/Core.h"
#include "Gameplay/GameObject.h"
#include "TileObject.h"

namespace cr { class RenderLayer; }

namespace ce
{
	enum class AppState;
	class LevelService;

	class GameObjectManager : public IService
	{
	public:
		GameObjectManager() = default;
		~GameObjectManager() override = default;

		void OnCreate() override;
		void OnDestroy() override;

		template<typename T, typename ... Args, typename = typename std::enable_if<std::is_base_of<GameObject, T>::value, T>::type>
		T* CreateGameObject(const std::string& a_name, Args&& ... a_args);

		template<typename T, typename = typename std::enable_if<std::is_base_of<GameObject, T>::value, T>::type>
		T* GetGameObject(const std::string& a_name);
		template<typename T, typename = typename std::enable_if<std::is_base_of<GameObject, T>::value, T>::type>
		T* GetGameObject(const GameObjectId& a_id);

		template<typename T, typename = typename std::enable_if<std::is_base_of<GameObject, T>::value, T>::type>
		T* GetGameObjectByType();

		template<typename T, typename = typename std::enable_if<std::is_base_of<GameObject, T>::value, T>::type>
		std::vector<T*> GetGameObjectsByType();

		void DestroyGameObject(GameObject* a_gameObject);
		void DestroyGameObject(const GameObjectId& a_id);
		void DestroyGameObject(const char* a_name);

		void Update(float a_deltaTime, const AppState& a_appState);
		void Draw(cr::RenderLayer& a_renderLayer);
	private:
		std::vector<std::unique_ptr<GameObject>> m_gameObjects;											//List of all the gameobjects.
		std::vector<TileObject*> m_tileObjects;															//List of tile objects in the game.

		std::deque<std::pair<std::size_t, std::unique_ptr<GameObject>>> m_gameObjectAddQueue;			//Queue with indexes pointing to objects to load.
		std::set<std::size_t> m_gameObjectRemoveQueue;													//Queue with indexes pointing to objects to unload.
		std::stack<std::size_t> m_availableIndices;														//Queue with available indexes in the GameObject list to place new GameObjects on.

		std::size_t m_numIndices{ 0 };
		std::size_t m_numCreatedGameObjects{ 0 };
		std::size_t m_numActiveGameObjects{ 0 };

		LevelService* m_levelService;

		void AddGameObject(size_t a_indexToPlace, std::unique_ptr<GameObject>&& a_gameObject);
		void RemoveGameObject(size_t a_indexToRemove);

		void FlushAddQueue();
		void FlushRemoveQueue();

		void UpdateTileObjectPositions();

		template<typename T, typename FunctorA, typename FunctorB>
		T* FindGameObject(FunctorA&& a_findPredicate, FunctorB&& a_findPredicateAddQueue);
	};

	template <typename T, typename ... Args, typename>
	T* GameObjectManager::CreateGameObject(const std::string& a_name, Args&& ... a_args)
	{
		size_t availableIndex = 0;
		if (!m_availableIndices.empty())
		{
			availableIndex = m_availableIndices.top();
			m_availableIndices.pop();
		}
		else
		{
			availableIndex = m_numIndices++;
		}

		OPTICK_EVENT();
		GameObjectId id
		{
			uint32_t(std::hash<std::string>{}(a_name)+m_numCreatedGameObjects)
		};

		//Create the GameObject.
		std::unique_ptr<T> go = std::make_unique<T>(std::forward<Args>(a_args)...);
		go->Init(a_name, id);

		const auto goPointer = go.get();

		//If we are also dealing with an TileObject we have to keep track of that.
		if (const auto tObj = dynamic_cast<TileObject*>(goPointer))
		{
			m_tileObjects.push_back(tObj);
		}

		//Tell the queue to add the spot to later on load the actual game object.
		m_gameObjectAddQueue.push_back(std::make_pair(availableIndex, std::move(go)));
		m_numCreatedGameObjects++;

		return dynamic_cast<T*>(goPointer);
	}

	template <typename T, typename>
	T* GameObjectManager::GetGameObject(const GameObjectId& a_id)
	{
		return FindGameObject<T>([a_id](const std::unique_ptr<GameObject>& a_go)
		{
			return a_go != nullptr && !a_go->m_markedForDestroy && a_go->IsActive() && a_go->GetId() == a_id;
		},
			[a_id](const std::pair<size_t, std::unique_ptr<GameObject>>& a_go)
		{
			const auto g = a_go.second.get();
			return g != nullptr && g->IsActive() && !g->m_markedForDestroy && g->GetId() == a_id;
		});
	}

	template <typename T, typename>
	T* GameObjectManager::GetGameObject(const std::string& a_name)
	{
		return FindGameObject<T>([a_name](const std::unique_ptr<GameObject>& a_go)
		{
			return a_go != nullptr && !a_go->m_markedForDestroy && a_go->IsActive() && a_go->GetName() == a_name;
		},
			[a_name](const std::pair<size_t, std::unique_ptr<GameObject>>& a_go)
		{
			const auto g = a_go.second.get();
			return g != nullptr && g->IsActive() && !g->m_markedForDestroy && g->GetName() == a_name;
		});
	}

	template <typename T, typename>
	T* GameObjectManager::GetGameObjectByType()
	{
		return FindGameObject<T>([](const std::unique_ptr<GameObject>& a_go)
		{
			const auto g = dynamic_cast<T*>(a_go.get());
			return g != nullptr && g->IsActive() && !g->m_markedForDestroy && typeid(g) == typeid(T*);
		}, 
		[](const std::pair<size_t, std::unique_ptr<GameObject>>& a_go)
		{
			const auto g = dynamic_cast<T*>(a_go.second.get());
			return g != nullptr && g->IsActive() && !g->m_markedForDestroy && typeid(g) == typeid(T*);
		});
	}

	template <typename T, typename FunctorA, typename FunctorB>
	T* GameObjectManager::FindGameObject(FunctorA&& a_findPredicate, FunctorB&& a_findPredicateAddQueue)
	{
		auto find = std::find_if(m_gameObjects.begin(), m_gameObjects.end(), std::forward<FunctorA>(a_findPredicate));

		//Check if it is in the add queue.
		if (find == m_gameObjects.end())
		{
			const auto aIt = std::find_if(m_gameObjectAddQueue.begin(), m_gameObjectAddQueue.end(), std::forward<FunctorB>(a_findPredicateAddQueue));
			if (aIt != m_gameObjectAddQueue.end())
			{
				return dynamic_cast<T*>(aIt->second.get());
			}
		}
		else if (find != m_gameObjects.end())
		{
			return dynamic_cast<T*>(find->get());
		}
		return nullptr;
	}

	template <typename T, typename>
	std::vector<T*> GameObjectManager::GetGameObjectsByType()
	{
		std::vector<T*> gos;
		for (auto &go : m_gameObjects)
		{
			const auto g = dynamic_cast<T*>(go.get());
			if (g != nullptr && g->IsActive() && !g->m_markedForDestroy && typeid(g) == typeid(T*))
			{
				gos.push_back(g);
			}
		}

		for (auto &go : m_gameObjectAddQueue)
		{
			const auto g = dynamic_cast<T*>(go.second.get());
			if (g != nullptr && g->IsActive() && !g->m_markedForDestroy && typeid(g) == typeid(T*))
			{
				gos.push_back(g);
			}
		}

		return gos;
	}
}
