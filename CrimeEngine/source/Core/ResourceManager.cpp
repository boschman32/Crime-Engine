#include "cepch.h"
#include "Core/ResourceManager.h"
#include "Core/Core.h"
#include "Utils/Utilities.h"

#ifdef PLATFORM_SWITCH
#include <nn/fs.h>
#include <nn/nn_Assert.h>
#include <nn/nn_Abort.h>
#include <nn/nn_Log.h>
#include "Platform/Switch/NVN_RenderLayer.h"
#endif

namespace ce
{
	void ResourceManager::OnCreate()
	{
#if PLATFORM_WINDOWS
		AddWildcard("[Assets]", "./data/");
#elif PLATFORM_SWITCH
		nn::Result result;
		size_t cacheSize = 0;
		char* cacheBuffer = nullptr;
		{
			/* Set functions that the file system uses for allocation/deallocation */
			nn::fs::SetAllocator(cr::allocfunc::Allocate, cr::allocfunc::Deallocate);
			
			//Mount the file system.
			CE_CORE_INFO("Mounting Rom...");

			result = nn::fs::QueryMountRomCacheSize(&cacheSize);
			CE_CORE_ASSERT(result.IsSuccess());

			cacheBuffer = new char[cacheSize];
			if (cacheBuffer == nullptr || cacheSize == 0)
			{
				CE_CORE_ASSERT_M(false, "Not enough cache available: {0}!", cacheSize);
				return;
			}

			result = nn::fs::MountRom("rom", cacheBuffer, cacheSize);
			CE_CORE_ASSERT_M(result.IsSuccess(), "Failed to mount rom!");
		}

		//Add wildcards
		AddWildcard("[Assets]", "rom:/");
#endif
	}

	void ResourceManager::OnDestroy()
	{
		//Check if all the resources are only being used by one instance (the resource manager).
		for (auto it = m_resources.begin(); it != m_resources.end();)
		{
			if (it->second.use_count() > 1)
			{
				CE_CORE_ASSERT_M(false, "Resource still in use: {0}\n", it->second->GetFileName());
			}
			else
			{
				CE_CORE_INFO("[ResourceManager]: Unloaded resource filename: {0}", it->second->m_fileName.c_str());
				it = m_resources.erase(it);
			}
		}
	}

	void ResourceManager::AddWildcard(const std::string& a_wildcard, const std::string& a_value)
	{
		auto found = m_wildcards.find(a_wildcard);

		//Replace existing wildcard with the new value.
		if (found != m_wildcards.end())
		{
			found->second = a_value;
		}
		else
		{
			m_wildcards.insert(std::make_pair(a_wildcard, a_value));
		}
	}

	std::string ResourceManager::GetDevicePath(const std::string& a_path) const
	{
		std::string devicePath = a_path;

		for (const auto& kvp : m_wildcards)
		{
			if (devicePath.find(kvp.first) != std::string::npos)
			{
				utils::ReplaceString(devicePath, kvp.first, kvp.second);
			}
		}

		return devicePath;
	}

	void ResourceManager::UnloadResource(const ResourceId& a_resourceId)
	{
		const auto it = m_resources.find(a_resourceId.Value());

		CE_CORE_ASSERT_M(it != m_resources.end(), "Couldn't unload resource as it doesn't exist!");
		
		m_resources.erase(it);

		CE_CORE_INFO("[ResourceManager] Unloaded resource!");
	}

}
