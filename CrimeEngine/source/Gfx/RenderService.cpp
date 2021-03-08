#include "cepch.h"
#include "Gfx/RenderService.h"

#include "Gfx/RenderLayer.h"
#include "Gfx/Window.h"
#include "Core/Core.h"
#include "Gfx/Camera2D.h"
#include "Gfx/Camera.h"
#include "Gfx/IDrawable.h"

#include "Utils/ServiceManager.h"
#include "Core/ResourceManager.h"
#include "Gfx/Shader.h"
#include "Gfx/Texture.h"

namespace ce
{
	std::shared_ptr<ce::Shader> RenderService::ms_defaultShader;
	std::shared_ptr<ce::Shader> RenderService::ms_defaultUIShader;
	std::shared_ptr<ce::Texture> RenderService::ms_defaultTexture;

	void RenderService::OnCreate()
	{
		m_camera = new Camera2D();
		m_window = cr::Window::CreateNewWindow(cr::RenderLayer::GetUsedAPI(), 720, 480);
		m_renderer = cr::RenderLayer::CreateRenderLayer(*m_window);

		auto rs = ServiceManager::GetService<ResourceManager>();
		RenderService::ms_defaultShader = rs->LoadResource<Shader>("data/shaders/DefaultShader.hlsl");
		RenderService::ms_defaultUIShader = rs->LoadResource<Shader>("data/shaders/DefaultUIShader.hlsl");
		RenderService::ms_defaultTexture = rs->LoadResource<Texture>("data/textures/DefaultTex.png");

#if !defined(CE_DEBUG) || !defined(_DEBUG)
		m_window->ShowMouse(false);
#endif
	}

	void RenderService::OnDestroy()
	{
		auto rs = ServiceManager::GetService<ResourceManager>();
		rs->ReleaseResource<Shader>(ms_defaultShader);
		rs->ReleaseResource<Texture>(ms_defaultTexture);
		rs->ReleaseResource<Shader>(ms_defaultUIShader);


		delete m_camera;
		delete m_window;
		delete m_renderer;
	}

	void RenderService::Clear(float a_colour[4])
	{
		m_renderer->Clear(a_colour);
	}

	void RenderService::Peak()
	{
		m_window->Peak();
	}

	void RenderService::PreRender()
	{
		m_renderer->PreRender(*m_camera);
	}

	void RenderService::Render()
	{
		m_renderer->Render();
	}

	void RenderService::LoadResouce(ce::Resource & a_texture, cr::ResourceData &a_data)
	{
		m_renderer->LoadResource(a_texture, a_data);
	}

	void RenderService::UnloadResource(ce::Resource& a_texture)
	{
		m_renderer->UnloadResource(a_texture);
	}

	bool RenderService::IsWindowActive()
	{
		return m_window->IsActive();
	}

	Vector2 RenderService::GetSize()
	{
		CE_CORE_ASSERT(m_window);
		return m_window->GetSize();
	}

	cr::RenderLayer* RenderService::GetRenderLayer()
	{
		CE_CORE_ASSERT(m_renderer);
		return m_renderer;
	}

	std::shared_ptr<ce::Shader> RenderService::GetDefaultShader()
	{
		CE_CORE_ASSERT(RenderService::ms_defaultShader);
		return RenderService::ms_defaultShader;
	}

	std::shared_ptr<ce::Shader> RenderService::GetDefaultUIShader()
	{
		CE_CORE_ASSERT(RenderService::ms_defaultShader);
		return RenderService::ms_defaultUIShader;
	}

	std::shared_ptr<ce::Texture> RenderService::GetDefaultTexture()
	{
		CE_CORE_ASSERT(RenderService::ms_defaultTexture);
		return RenderService::ms_defaultTexture;
	}

}