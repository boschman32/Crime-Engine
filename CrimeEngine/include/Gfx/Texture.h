#pragma once

#include "GFXHelper.h"

#include "Core/Resource.h"
#include "Utils/ServiceManager.h"
#include "Gfx/RenderService.h"

namespace ce 
{
	class Texture : public Resource
	{
		friend class ResourceManager;
	public:
		Texture() : Resource(ResourceType::Texture)
		{ };

		~Texture() override
		{
			ce::ServiceManager::GetService<RenderService>()->UnloadResource(*this);
		};
		void Reload() override
		{
			m_isReloaded = false;

			ce::ServiceManager::GetService<RenderService>()->LoadResouce(*this, m_textureData);
		};

		cr::TextureData GetTextureData() const { return m_textureData; }

	protected:
		void OnInitialize() override
		{
			m_isReloaded = true;

			ce::ServiceManager::GetService<RenderService>()->LoadResouce(*this, m_textureData);
		};

		cr::TextureData m_textureData;
	};
}