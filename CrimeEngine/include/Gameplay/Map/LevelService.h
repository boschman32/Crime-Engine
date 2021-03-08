#pragma once
#include "LevelMap.h"
#include "Utils/IService.h"

namespace ce
{
	enum class EventTypes
	{
		OnMapLoaded
	};

	class ITileObjectFactory;
	class ResourceManager;
	
	class LevelService : public IService
	{
	public:

		LevelService() = default;
		~LevelService() override;

		void OnCreate() override;
		void OnDestroy() override;
		
		void DebugDraw(cr::RenderLayer& a_renderLayer) const;

		LevelMap* GetCurrentLoadedMap() const { return m_currentMap.get(); }
		const std::vector<std::string>& GetAvailableMaps() const { return m_availableMaps; }

		bool LoadMap(const std::string& a_path, std::shared_ptr<LevelMap>& a_map, ITileObjectFactory* a_objectFactory = nullptr);
		void UnloadCurrentMap();

		void LookForAvailableMaps();
	private:
		std::shared_ptr<LevelMap> m_currentMap { nullptr };

		std::vector<std::string> m_availableMaps;

		ResourceManager* m_resourceManager;
	};
}