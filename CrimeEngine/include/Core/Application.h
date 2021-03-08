#pragma once
#include "Core/LayerStack.h"

namespace ce
{
	class IEvent;
	class WindowResizeEvent;
	class RenderService;

	enum class AppState
	{
		Playing,
		Paused,
		Editor
	};
	
	class Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void OnInitialized() = 0;
		virtual void OnClose() = 0;

		const AppState& GetAppState() const { return m_currentState; }
		float GetFps() const { return m_fps; }

		Layer* PushLayer(std::unique_ptr<Layer> a_layer);
		Layer* PushOverlay(std::unique_ptr<Layer> a_overlay);
		void PopLayer(Layer& a_layer);
		void PopOverlay(Layer& a_overlay);

		void Play(bool a_profile = false);
		void Pause();
		void Stop();

		bool Run();

		static Application& Get() { return *ms_instance; }
	private:
		static Application* ms_instance;
		
		AppState m_currentState { AppState::Editor };
		
		LayerStack m_layerStack;
		RenderService* m_renderService { nullptr };

		float m_fps { 0.f };
		bool m_isProfiling { false };
		
		void CheckForEvents() const;

		void OnWindowResizeEvent(WindowResizeEvent& a_event) const;
	};

	//To be defined by client.
	Application* CreateApplication();
}