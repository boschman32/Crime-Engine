#pragma once
#include "Core/Core.h"
#include "IScene.h"
#include "Utils/IService.h"

namespace cr
{
	class RenderLayer;
}

namespace ce
{
	class SceneManager : public IService
	{
	public:
		void OnCreate() override;
		void OnDestroy() override;

		void Update(float a_deltaTime) const;
		void PauseUpdate(float a_deltaTime) const;
		void Draw(cr::RenderLayer& a_renderLayer) const;
		
		template<typename T, typename = typename std::enable_if<std::is_base_of<IScene, T>::value, T>::type, typename ... Args>
		void AddScene(const std::string& a_sceneName, Args&& ... a_args);

		template<typename T, typename = typename std::enable_if<std::is_base_of<IScene, T>::value, T>::type>
		T* SwitchScene(const std::string& a_sceneName);

		void RemoveScene(const std::string& a_sceneName);

		IScene* GetCurrentScene() const { return m_currentScene; }
	private:
		std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes;
		IScene* m_currentScene { nullptr };
	};

	template <typename T, typename, typename ... Args>
	void SceneManager::AddScene(const std::string& a_sceneName, Args&&... a_args)
	{
		//Check if scene is already added.
		if(m_scenes.find(a_sceneName) != m_scenes.end())
		{
			CE_CORE_ERROR("[SceneManagement] Scene({0}) has already been added!", a_sceneName);
			return;
		}

		auto scene = std::make_unique<T>(std::forward<Args>(a_args)...);
		scene->m_sceneName = a_sceneName;
		m_scenes.insert(std::make_pair(a_sceneName, std::move(scene)));
	}

	template <typename T, typename>
	T* SceneManager::SwitchScene(const std::string& a_sceneName)
	{
		const auto it = m_scenes.find(a_sceneName);

		CE_CORE_ASSERT_M(it != m_scenes.end(), "Couldn't switch to scene({0} as it has been added yet!", a_sceneName);
		
		if(m_currentScene != nullptr)
		{
			m_currentScene->OnDispose();
			m_currentScene = nullptr;
		}
		
		m_currentScene = it->second.get();
		if(!m_currentScene->OnSceneSwitched())
		{
			CE_CORE_CRITICAL("[SceneManager] Couldn't initialize scene({0})!", a_sceneName);
			return nullptr;
		}
		return dynamic_cast<T*>(m_currentScene);
	}
}
