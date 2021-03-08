#include "cepch.h"
#include "Scenes/SceneManager.h"

namespace ce
{
	void SceneManager::OnCreate()
	{
		
	}

	void SceneManager::OnDestroy()
	{
		for(auto &s : m_scenes)
		{
			s.second->OnDispose();
		}
	}
	
	void SceneManager::RemoveScene(const std::string& a_sceneName)
	{
		const auto it = m_scenes.find(a_sceneName);

		if(it != m_scenes.end())
		{
			(*it).second->OnDispose();
			m_scenes.erase(it);
		}
	}

	void SceneManager::Update(float a_deltaTime) const
	{
		if(m_currentScene != nullptr)
		{
			OPTICK_CATEGORY(m_currentScene->GetSceneName().c_str(), Optick::Category::Scene);
			m_currentScene->OnUpdate(a_deltaTime);
		}
	}

	void SceneManager::PauseUpdate(float a_deltaTime) const
	{
		if(m_currentScene != nullptr)
		{
			m_currentScene->OnPauseUpdate(a_deltaTime);
		}
	}

	void SceneManager::Draw(cr::RenderLayer& a_renderLayer) const
	{
		if(m_currentScene != nullptr)
		{
			m_currentScene->OnDraw(a_renderLayer);
		}
	}
}