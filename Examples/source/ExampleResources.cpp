#include "expch.h"
#include "ExampleResources.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"
#include "Utils/ServiceManager.h"

namespace ce
{
	namespace examples
	{
		class TestResource : public ce::Resource
		{
		public:
			TestResource(int a_resourceArg)
				: ce::Resource(ce::ResourceType::Texture), m_resourceArg(a_resourceArg)
			{

			}
		private:
			int m_resourceArg = 0;
		};

		class ResourceUser
		{
		public:
			ResourceUser()
			{
				m_resourceManager = ce::ServiceManager::GetService<ce::ResourceManager>();
				m_resource = m_resourceManager->LoadResource<TestResource>("[Assets]/resource.png", 5);
			}
			~ResourceUser()
			{
				m_resourceManager->ReleaseResource(m_resource);
			}

		private:
			ce::ResourceManager *m_resourceManager;
			std::shared_ptr<TestResource> m_resource;
		};

		void ExampleResources::RunExample()
		{
			ServiceManager::MakeService<ResourceManager>();
			{
				ResourceUser user;
			}
		}
	}
}