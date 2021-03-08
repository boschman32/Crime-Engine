#pragma once

#include "GFXHelper.h"

#include "Core/Resource.h"
#include "Utils/ServiceManager.h"
#include "Gfx/RenderService.h"

namespace ce
{
	class Shader : public Resource
	{
		friend class ResourceManager;
	public:
		Shader() : Resource(ResourceType::Shader)
		{ };

		~Shader() override
		{
			ce::ServiceManager::GetService<RenderService>()->UnloadResource(*this);
		};

		void Reload() override
		{
			m_isReloaded = false;

			ce::ServiceManager::GetService<RenderService>()->LoadResouce(*this, m_shaderData);

			m_isReloaded = true;
		};

		cr::ShaderData GetShaderData() const { return m_shaderData; }

	protected:

		void OnInitialize() override
		{
			m_isReloaded = true;

			ce::ServiceManager::GetService<RenderService>()->LoadResouce(*this, m_shaderData);
		};

		cr::ShaderData m_shaderData;
	};
}
