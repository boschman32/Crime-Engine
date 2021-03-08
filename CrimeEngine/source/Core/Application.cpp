#include "cepch.h"
#include "Core/Application.h"

//GFX
#include "Gfx/RenderLayer.h"
#include "Gfx/Camera2D.h"

//Services
#include "Utils/ServiceManager.h"
#include "Gfx/RenderService.h"
#include "Core/ResourceManager.h"
#include "Core/Input/InputManager.h"
#include "Gameplay/GameObjectManager.h"
#include "Gameplay/Scripting/LuaService.h"
#include "Gameplay/Map/LevelService.h"
#include "Scenes/SceneManager.h"
#include "Events/EventService.h"

//Events
#include "Events/ApplicationEvent.h"

//Utils
#include "Utils/Timer.h"

#if defined(CE_DEBUG) && defined(PLATFORM_WINDOWS)
#include "Utils/Editor/EditorLayer.h"
#endif

#if !defined(CE_DEBUG) || !defined(_DEBUG)
#include "Gfx/Drawables/Sprite.h"
#include "Gfx/Texture.h"
#include "Gfx/Shader.h"
#endif

namespace ce
{
	Application* Application::ms_instance{ nullptr };

	Application::Application()
	{
		CE_CORE_ASSERT_M(!ms_instance, "Application already exists!");
		ms_instance = this;

		ServiceManager::MakeService<EventService>();
		ServiceManager::MakeService<InputManager>();
		ServiceManager::MakeService<ResourceManager>();
		m_renderService = ServiceManager::MakeService<RenderService>();
		ServiceManager::MakeService<LevelService>();
		ServiceManager::MakeService<GameObjectManager>();
		ServiceManager::MakeService<scripting::LuaService>();
		ServiceManager::MakeService<SceneManager>();
	}

	Application::~Application()
	{
		ServiceManager::DestroyService<LevelService>();
		ServiceManager::DestroyService<SceneManager>();
		ServiceManager::DestroyService<GameObjectManager>();
		ServiceManager::DestroyService<InputManager>();
		ServiceManager::DestroyService<scripting::LuaService>();
		ServiceManager::DestroyService<RenderService>();

		ServiceManager::DestroyService<ResourceManager>();
		ServiceManager::DestroyService<EventService>();

		m_renderService = nullptr;
		ms_instance = nullptr;
	}

	Layer* Application::PushLayer(std::unique_ptr<Layer> a_layer)
	{
		return m_layerStack.PushLayer(std::move(a_layer));
	}

	Layer* Application::PushOverlay(std::unique_ptr<Layer> a_overlay)
	{
		return m_layerStack.PushOverlay(std::move(a_overlay));
	}

	void Application::PopLayer(Layer& a_layer)
	{
		m_layerStack.PopLayer(a_layer);
	}

	void Application::PopOverlay(Layer& a_overlay)
	{
		m_layerStack.PopOverlay(a_overlay);
	}

	bool Application::Run()
	{
		//Initialize the logger to trace level.
		Logger::SetGlobalLogLevel(Logger::LogLevel::Trace);

		//Core services.
		auto gm = ServiceManager::GetService<GameObjectManager>();
		const auto sm = ServiceManager::GetService<SceneManager>();
		const auto rl = m_renderService->GetRenderLayer();
#if DEBUG_GRID
		const auto ls = ServiceManager::GetService<LevelService>();
#endif

		//No render layer created!
		if (rl == nullptr)
		{
			CE_CORE_ERROR("Failed to create render layer!");
			return false;
		}
		//Start listening to any application events.
		EventService::Subscribe<WindowResizeEvent>(EventType::WindowResize, BIND_EVENT_CALLBACK(Application::OnWindowResizeEvent));
		
//Begin play immediately or show the editor first.
#if !defined(CE_DEBUG)
		Play();
#elif defined(PLATFORM_WINDOWS)
		auto editorLayer = PushOverlay(std::make_unique<editor::EditorLayer>());
#endif

#if !defined(CE_DEBUG) && !_DEBUG
		auto rs = ServiceManager::GetService<ResourceManager>();
		auto shader = rs->LoadResource<Shader>("data/shaders/DefaultUIShader.hlsl");
		auto texture2 = rs->LoadResource<Texture>("data/textures/mouse_01.png");

		Sprite cursor;
		cursor.SetColor(Vector4(1, 1, 1, 1));
		cursor.SetScale(Vector2(16, 16));
		cursor.SetShader(*shader);
		cursor.SetTexture(*texture2);
		cursor.SetCullable(false);

		auto cam = dynamic_cast<ce::Camera2D*>(Camera::GetMainCamera());
#endif
		Timer<float> deltaTimer;
		float totalTime = 0.f;
		unsigned frameCount = 0;
		float clearColor[] = { 0.29f,0.59f,0.82f,0 };

		//Tell that we are done initializing;
		OnInitialized();

		while (m_renderService->IsWindowActive())
		{
			//Profiling
			if (!m_isProfiling)
			{
				//Is not supported when using automated profiling.
				OPTICK_FRAME("MainThread");
			}

			//-- Update part --
			OPTICK_EVENT();
			const float deltaSeconds = deltaTimer.GetElapsedTime();

			//Clear the screen
			m_renderService->Clear(clearColor);

			//-- Check for any events happening ---
			CheckForEvents();

			//Update game objects.
			gm->Update(deltaSeconds, m_currentState);

			for (const auto& layer : m_layerStack)
			{
				if (layer->IsEnabled())
				{
					layer->OnUpdate(deltaSeconds);
				}
			}

			//We are currently playing the game!
			if (m_currentState == AppState::Playing)
			{
				//Update the scene
				sm->Update(deltaSeconds);
			}
			else
			{
				//Update the scene on pause.
				sm->PauseUpdate(deltaSeconds);
			}

#if !defined(CE_DEBUG) && !_DEBUG
			Vector2 mousePosition = InputManager::GetMousePosition();
			mousePosition -= (cam->GetSize() * 0.5f) - (cursor.GetScale() * 0.5f);
			cursor.SetPosition(Vector3(mousePosition, 0));
#endif
			// -- Render part --
			for (const auto& layer : m_layerStack)
			{
				if (layer->IsEnabled())
				{
					layer->OnImGuiRender();
				}
			}

			//Tell the game objects to draw.
			gm->Draw(*rl);
			//Tell the scenes to draw.
			sm->Draw(*rl);
#if DEBUG_GRID
			//For grid drawing.
			ls->DebugDraw(*rl);
#endif

#if !defined(CE_DEBUG) && !defined(_DEBUG)
			cursor.Draw(*rl);
#endif

			//Pre render any instances and render data.
			m_renderService->PreRender();

			//Render to the actual window.
			m_renderService->Render();

			//Calculate the fps
			totalTime += deltaSeconds;
			frameCount++;
			if (totalTime >= 1.f)
			{
				m_fps = float(frameCount) / totalTime;
				frameCount = 0;
				totalTime = 0.f;
			}
		}

#if !defined(CE_DEBUG) && !defined(DEBUG_GRID)
		rs->ReleaseResource(shader);
		rs->ReleaseResource(texture2);
#endif

		EventService::Unsubscribe<WindowResizeEvent>(EventType::WindowResize, BIND_EVENT_CALLBACK(Application::OnWindowResizeEvent));
		
		OPTICK_SHUTDOWN();

#if defined(CE_DEBUG) && defined(PLATFORM_WINDOWS)
		PopOverlay(*editorLayer);
#endif
		OnClose();

		return true;
	}

	void Application::Play(bool a_profile)
	{
		m_currentState = AppState::Playing;

		if (a_profile)
		{
			OPTICK_START_CAPTURE();
			m_isProfiling = true;
		}

		//Find any lua script files and run them.
		ServiceManager::GetService<scripting::LuaService>()->FindAndRunScriptFiles();

		std::for_each(m_layerStack.begin(), m_layerStack.end(), [](std::unique_ptr<Layer>& a_layer)
		{
			a_layer->OnPlay();
		});
	}

	void Application::Pause()
	{
		if (m_currentState != AppState::Paused)
		{
			m_currentState = AppState::Paused;
			std::for_each(m_layerStack.begin(), m_layerStack.end(), [](std::unique_ptr<Layer>& a_layer)
			{
				a_layer->OnPause();
			});
		}
		else
		{
			m_currentState = AppState::Playing;
		}
	}

	void Application::Stop()
	{
		m_currentState = AppState::Editor;

		if (m_isProfiling)
		{
			OPTICK_STOP_CAPTURE();

#ifdef PLATFORM_WINDOWS
			const std::string optickSavePath = "../OptickCaptures/";
			if (!std::experimental::filesystem::exists(optickSavePath))
			{
				std::experimental::filesystem::create_directory(optickSavePath);
			}
			OPTICK_SAVE_CAPTURE((optickSavePath + "CrimeEngine").c_str());
#endif
			m_isProfiling = false;
		}

		IScene* curScene = ServiceManager::GetService<SceneManager>()->GetCurrentScene();

		if (curScene != nullptr)
		{
			const std::string curSceneName = curScene->GetSceneName();
			//Reload the current scene (by switching back to calling destroy and then init).
			ServiceManager::GetService<SceneManager>()->SwitchScene<IScene>(curSceneName);
		}

		std::for_each(m_layerStack.begin(), m_layerStack.end(), [](std::unique_ptr<Layer>& a_layer)
		{
			a_layer->OnEditorStop();
		});
	}

	void Application::CheckForEvents() const
	{
		m_renderService->Peak();
		InputManager::Get()->Update();
	}

	void Application::OnWindowResizeEvent(WindowResizeEvent& a_event) const
	{
		auto cam = dynamic_cast<ce::Camera2D*>(Camera::GetMainCamera());

		if(cam != nullptr)
		{
			cam->SetOrthographicProjection(m_renderService->GetSize());
		}
		CE_CORE_INFO(a_event);
	}
}
