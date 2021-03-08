#include "cepch.h"
#include "Gameplay/GameObjectManager.h"
#include "Gfx/RenderLayer.h"
#include "Core/Application.h"

#include "Gameplay/Map/LevelService.h"
#include "Gameplay/Map/GridHelper.h"
#include "Utils/ServiceManager.h"

namespace ce
{
	void GameObjectManager::OnCreate()
	{
		m_levelService = ServiceManager::GetService<LevelService>();
	}

	void GameObjectManager::OnDestroy()
	{
		//Tell all active GameObjects that they're being destroyed (since the GameObjectManager is gone).
		for (auto &it : m_gameObjects)
		{
			if (it != nullptr)
			{
				it->OnDestroy();
			}
		}
		m_gameObjects.clear();
		m_tileObjects.clear();
		m_numIndices = 0;
		m_numCreatedGameObjects = 0;
		m_numActiveGameObjects = 0;
	}

	void GameObjectManager::Draw(cr::RenderLayer& a_renderLayer)
	{
		OPTICK_EVENT()
			for (auto &go : m_gameObjects)
			{
				if (go != nullptr && go->m_isLoaded)
				{
					go->OnDraw(a_renderLayer);
				}
			}
	}

	void GameObjectManager::Update(float a_deltaTime, const AppState& a_appState)
	{
		OPTICK_CATEGORY("GameObjects Update", Optick::Category::GameLogic);
		FlushAddQueue();

		UpdateTileObjectPositions();

		//Update the game objects
		for (auto &go : m_gameObjects)
		{
			if (go != nullptr && go->m_isLoaded && go->IsActive())
			{
				if (go->m_updateOnPause || a_appState == AppState::Playing)
				{
					go->OnUpdate(a_deltaTime);
				}
			}
		}

		FlushAddQueue();
		FlushRemoveQueue();
	}

	void GameObjectManager::FlushAddQueue()
	{
		//Check the queue for GameObjects to add in the scene.
		while (!m_gameObjectAddQueue.empty())
		{
			auto &pair = m_gameObjectAddQueue.front();
			AddGameObject(pair.first, std::move(pair.second));
			m_gameObjectAddQueue.pop_front();
		}
	}

	void GameObjectManager::FlushRemoveQueue()
	{
		while (!m_gameObjectRemoveQueue.empty())
		{
			auto itr = std::prev(m_gameObjectRemoveQueue.end());

			RemoveGameObject(*itr);
			m_gameObjectRemoveQueue.erase(itr);
		}
	}

	void GameObjectManager::AddGameObject(size_t a_indexToPlace, std::unique_ptr<GameObject>&& a_gameObject)
	{
		m_numActiveGameObjects++;

		//Check if we can place it inside the list or need to push it back.
		if(a_indexToPlace < m_gameObjects.size())
		{
			m_gameObjects[a_indexToPlace] = std::move(a_gameObject);
		}
		else
		{
			m_gameObjects.push_back(std::move(a_gameObject));
		}

		m_gameObjects[a_indexToPlace]->m_isLoaded = true;
		m_gameObjects[a_indexToPlace]->OnCreate();
	}

	void GameObjectManager::RemoveGameObject(size_t a_indexToRemove)
	{
		m_availableIndices.push(a_indexToRemove);
		auto &go = m_gameObjects[a_indexToRemove];

		//Check if we are dealing with an tile object as well.
		if (const auto tileObj = dynamic_cast<TileObject*>(go.get()))
		{
			m_tileObjects.erase(std::remove(m_tileObjects.begin(), m_tileObjects.end(), tileObj), m_tileObjects.end());
		}

		go->OnDestroy();
		m_gameObjects[a_indexToRemove] = nullptr;
		m_numActiveGameObjects--;

		//If no more GameObjects are present we can just clear the list and start anew.
		if (m_numActiveGameObjects <= 0)
		{
			m_gameObjects.clear();
			m_tileObjects.clear();
			m_numIndices = 0;
			m_numCreatedGameObjects = 0;
		}
	}

	void GameObjectManager::UpdateTileObjectPositions()
	{
		OPTICK_EVENT()
			if (const auto map = m_levelService->GetCurrentLoadedMap())
			{
				Grid& grid = map->GetGrid();
				for (auto tObj : m_tileObjects)
				{
					if (tObj->m_isLoaded && tObj->IsActive())
					{
						const UVector2& prevPos = tObj->GetGridPosition();
						const UVector2 newPos = grid_helper::ConvertToGridPosition(
							tObj->GetPosition(),
							grid.GetGridSize(),
							map->GetMapOrientation() == MapOrientation::Isometric);

						if (newPos != prevPos)
						{
							grid.ClearCellOf(prevPos.x, prevPos.y, *tObj);
							grid.OccupyCell(newPos.x, newPos.y, *tObj);
							tObj->m_gridPosition = newPos;
						}
					}
				}
			}
	}

	void GameObjectManager::DestroyGameObject(GameObject* a_gameObject)
	{
		if (a_gameObject != nullptr)
		{
			DestroyGameObject(a_gameObject->GetId());
			a_gameObject = nullptr;
		}
	}

	void GameObjectManager::DestroyGameObject(const GameObjectId& a_id)
	{
		const auto find = std::find_if(m_gameObjects.begin(), m_gameObjects.end(), [a_id](const std::unique_ptr<GameObject>& a_go)
		{
			return a_go != nullptr && a_go->GetId() == a_id;
		});

		if (find != m_gameObjects.end())
		{
			(*find)->m_markedForDestroy = true;
			const auto idx = std::distance(m_gameObjects.begin(), find);
			m_gameObjectRemoveQueue.insert(idx);
		}
	}

	void GameObjectManager::DestroyGameObject(const char* a_name)
	{
		const auto find = std::find_if(m_gameObjects.begin(), m_gameObjects.end(), [a_name](const std::unique_ptr<GameObject>& a_go)
		{
			return a_go != nullptr && a_go->GetName() == a_name;
		});

		if (find != m_gameObjects.end())
		{
			(*find)->m_markedForDestroy = true;
			const auto idx = std::distance(m_gameObjects.begin(), find);
			m_gameObjectRemoveQueue.insert(idx);
		}
	}
}
