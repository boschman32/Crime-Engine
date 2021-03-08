#pragma once
#include "Core/Core.h"
#include "Core/Logger.h"

namespace cr { class RenderLayer; }

namespace ce
{	
	class GameObject
	{
		friend class GameObjectManager;
	public:
		GameObject(bool a_onPauseUpdate = false)
			: m_updateOnPause(a_onPauseUpdate) { }
		virtual ~GameObject() = default;

		virtual void OnCreate() = 0;
		virtual void OnUpdate(float a_deltaTime) = 0;
		virtual void OnDraw(cr::RenderLayer& a_renderLayer) = 0;
		virtual void OnDestroy() = 0;

		const std::string& GetName() const { return m_name; };
		const GameObjectId& GetId() const { return m_id; }

		void SetActive(bool a_active) { m_isActive = a_active; }
		bool IsActive() const { return m_isActive; }
	private:
		//Called from the GameObjectManager
		void Init(const std::string& a_name, const GameObjectId& a_id)
		{
			m_name = a_name;
			m_id = a_id;
		};
		
		std::string m_name		{ "Default" };
		GameObjectId m_id		{ 0 };
		bool m_isActive			{ true };
		bool m_isLoaded			{ false };
		bool m_markedForDestroy { false };
		bool m_updateOnPause	{ false };
	};
}
