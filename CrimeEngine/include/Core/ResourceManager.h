#pragma once
#include "Resource.h"

#include "Utils/FileSystem.h"
#include "Utils/IService.h"

namespace ce
{
	class ResourceManager : public IService
	{
	public:
		ResourceManager() = default;
		~ResourceManager() override = default;

		void OnCreate() override;
		void OnDestroy() override;

		/**
	      * \brief Load a particular resource with the given path, any arguments passed here will be passed to the constructor
	      * of the resource class.
	      * 
	      * \param a_path The path to the resource
	      * \param a_args any arguments passed to the resource class.
	      *
	     **/
		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type, typename ... Args>
		std::shared_ptr<T> LoadResource(const std::string& a_path, Args... a_args);

		/**
	      * \brief Release a resource, tell the resource manager that we are no longer using this resource.
	      * If this was the last resource that was used it will be unloaded.
	      * 
	      * \param a_resource The resource that needs to be released, Note: the resource will be set to NULL.
	      *
	     **/
		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type>
        void ReleaseResource(std::shared_ptr<T>& a_resource);

		/**
	      * \brief Release a resource tell the resource manager we are no longer using this resource.
	      * If this was the last resource that was used it will be unloaded.
	      * 
	      * \param a_resourceId The Id of the resource to release.
	      *
	     **/
		template<typename T, typename = typename std::enable_if<std::is_base_of<Resource, T>::value, T>::type>
        void ReleaseResource(const ResourceId& a_resourceId);

		/**
	      * \brief Get a particular resource with the given type T and the corresponding Id.
	      * 
	      * \param a_resourceId The Id of the resource to get.
	      *
	     **/
		template<typename T>
		std::shared_ptr<T> GetResource(const ResourceId& a_resourceId);

		/**
	      * \brief Adds a wildcard that maps to a particular path.
	      * Example: add wildcard [Assets] and map it do ./data.
	      * Any time you use [Assets] it will be converted to ./data.
	      * 
	      * \param a_wildcard The wildcard to map the value to.
	      * \param a_value The value mapped to the wildcard.
	      *
	     **/
		void AddWildcard(const std::string& a_wildcard, const std::string& a_value);

		/**
	      * \brief Parse path to a device path that uses a wildcard.
	      * 
	      * \param a_path the path to convert to a device path.
	      *
	     **/
		std::string GetDevicePath(const std::string& a_path) const;
	private:
		std::unordered_map<std::uint64_t, std::shared_ptr<Resource>> m_resources;
		std::unordered_map<std::string, std::string> m_wildcards;

		void UnloadResource(const ResourceId& a_resourceId);
	};

	template <typename T, typename, class... Args>
	std::shared_ptr<T> ResourceManager::LoadResource(const std::string& a_path, Args... a_args)
	{
		OPTICK_EVENT();
		//Hash both the path and name so we can have an unique hash.
		ResourceId idHandle(std::hash<std::string>{}(a_path));
		
		//Check if resource is already loaded in.
		std::shared_ptr<T> resource = GetResource<T>(idHandle);
		if(resource != nullptr)
		{
			return resource;
		}

		resource = std::make_shared<T>(a_args...);
		const std::string devicePath = GetDevicePath(a_path);
		resource->Create(idHandle, devicePath, ExtractFilenameFromPath(a_path));
        resource->OnInitialize();
		m_resources.insert(std::make_pair(idHandle.Value(), resource));

		CE_CORE_INFO("[ResourceManager]: Loaded resource(filename: {0},\npath: {1})", resource->GetFileName(), devicePath);
		return resource;
	}

	template<typename T, typename>
	void ResourceManager::ReleaseResource(std::shared_ptr<T>& a_resource)
    {
		OPTICK_EVENT();
        if (a_resource != nullptr)
        {
            if (a_resource.use_count() <= 2)
            {
            	CE_CORE_INFO("[ResourceManager] Unloading resource({0})...", a_resource->GetFileName());
            	UnloadResource(a_resource->GetID());
            }
        	a_resource = nullptr;
        }
    }

	template<typename T, typename>
	void ResourceManager::ReleaseResource(const ResourceId& a_resourceId)
	{
		OPTICK_EVENT();
		std::weak_ptr<T> r = GetResource<T>(a_resourceId);

		if(!r.expired())
		{
			if(r.use_count() == 1)
			{
				CE_CORE_INFO("[ResourceManager] Unloading resource({0})...", r.lock()->GetFileName());
				UnloadResource(a_resourceId);
			}
		}
	}

	template <typename T>
	std::shared_ptr<T> ResourceManager::GetResource(const ResourceId& a_resourceId)
	{
		const auto it = m_resources.find(a_resourceId.Value());
		if(it != m_resources.end())
		{
			return std::dynamic_pointer_cast<T>(it->second);
		}
		return nullptr;
	}
}
