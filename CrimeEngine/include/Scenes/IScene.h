#pragma once

namespace cr
{
	class RenderLayer;
}

namespace ce
{
	class IScene
	{
		friend class SceneManager;
	public:
		IScene() = default;
		virtual ~IScene() = default;
		
		/* Called when we have switched to this scene. */
		virtual bool OnSceneSwitched() = 0;

		/* Called when the current scene is no longer opened. */
		virtual void OnDispose() = 0;

		/* Called when the scene is update per frame */
		virtual void OnUpdate(float a_deltaTime) = 0;

		/* Called when the scene is update per frame even when paused. */
		virtual void OnPauseUpdate(float) { };
		
		virtual void OnDraw(cr::RenderLayer& a_renderLayer) = 0;

		const std::string& GetSceneName() const { return m_sceneName; }
		
	private:
		std::string m_sceneName { "" };
	};
}