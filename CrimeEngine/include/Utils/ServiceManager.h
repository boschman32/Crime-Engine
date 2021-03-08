#pragma once
#include "Core/Core.h"
#include "Core/Logger.h"
#include "IService.h"

#include <unordered_map>

namespace ce
{
	class ServiceManager
	{
	public:
		//Singleton
		static ServiceManager& Get()
		{
			static ServiceManager s_instance;

			return s_instance;
		}
		//Delete the copy constructor and assignment as there should be only one resource manager.
		ServiceManager(const ServiceManager&) = delete;
		void operator=(const ServiceManager&) = delete;
		
		~ServiceManager()
		{
			for(auto &s : m_services)
			{
				s.second->OnDestroy();
			}
		};
		
		template<typename T, typename ...Args, typename = typename std::enable_if<std::is_base_of<IService, T>::value, T>::type>
		static T* MakeService(Args&& ... a_args)
		{
			return Get().MakeServiceImpl<T>(std::forward<T>(a_args)...);
		}

		template<typename T, typename = std::enable_if<std::is_base_of<IService, T>::value, T>>
		static void DestroyService()
		{
			Get().DestroyServiceImpl<T>();
		}

		template<typename T, typename = typename std::enable_if<std::is_base_of<IService, T>::value, T>::type>
		static T* GetService()
		{
			return Get().GetServiceImpl<T>();
		}

		template<typename T, typename = typename std::enable_if<std::is_base_of<IService, T>::value, T>::type>
		static bool HasService()
		{
			return Get().HasServiceImpl<T>();
		}
	private:
		ServiceManager() = default;

		template<typename T, typename ...Args>
		T* MakeServiceImpl(Args&& ... a_args);
		template<typename T>
		void DestroyServiceImpl();
		template<typename T>
		T* GetServiceImpl();
		template<typename T>
		bool HasServiceImpl();
		
		template<typename T>
		auto FindService();
		
		template<typename T>
		static std::size_t GetTypeHashCode();

		std::unordered_map<std::size_t, std::unique_ptr<IService>> m_services;
	};

	template<typename T, typename ...Args>
	T* ServiceManager::MakeServiceImpl(Args&& ... a_args)
	{
		auto it = FindService<T>();
		if(it == m_services.end())
		{
			T* service = dynamic_cast<T*>((m_services.emplace(GetTypeHashCode<T>(), std::make_unique<T>(std::forward<Args>(a_args)...)).first)->second.get());
			service->OnCreate();
			return service;
		}
		
		return dynamic_cast<T*>(it->second.get());
	}

	template<typename T>
	void ServiceManager::DestroyServiceImpl()
	{
		auto it = FindService<T>();
		if(it == m_services.end())
		{
			CE_CORE_ASSERT_M(false, "[ServiceManager] Can't destroy service({0}) as it not exists!", typeid(T).name());
			return;
		}

		it->second->OnDestroy();
		m_services.erase(it);
	}

	template<typename T>
	T* ServiceManager::GetServiceImpl()
	{
		auto it = FindService<T>();
		if(it == m_services.end())
		{
			CE_CORE_ASSERT_M(false, "Getting service({0}) that doesn't exist!", typeid(T).name());
			return nullptr;
		}

		return dynamic_cast<T*>(it->second.get());
	}

	template<typename T>
	bool ServiceManager::HasServiceImpl()
	{
		return FindService<T>() != m_services.end();
	}

	template <typename T>
	auto ServiceManager::FindService()
	{
		return m_services.find(GetTypeHashCode<T>());
	}

	template<typename T>
	std::size_t ServiceManager::GetTypeHashCode()
	{
		return typeid(T).hash_code();
	}
}
