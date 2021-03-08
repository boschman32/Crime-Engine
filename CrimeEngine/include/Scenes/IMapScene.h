#pragma once

#include "IScene.h"
#include "Gameplay/Map/LevelService.h"

namespace ce
{
	class MapBuiltEvent;

	enum class SimulationState
	{
		None,
		Planning,
		Simulating,
	};

	class IMapScene : public IScene
	{
	public:
		IMapScene();
		~IMapScene() override;
		
		void OnUpdate(float a_deltaTime) override;
		
		void BeginPlan();
		void BeginSimulation();

		const SimulationState& GetCurrentSimulationState() const { return m_simState; };

		void SelectMap(const std::string& a_selectedMap);
		bool LoadSelectedMap(ITileObjectFactory* a_objectFactory = nullptr);
	protected:
		SimulationState m_simState { SimulationState::None };
		LevelService* m_levelService { nullptr };
		ResourceManager* m_resourceManager;
		std::shared_ptr<LevelMap> m_loadedMap { nullptr };

		bool m_mapIsLoaded { false };
		std::string m_selectedMapPath;

		/* Called when the a new map has been loaded. */
		virtual void OnMapLoaded(std::shared_ptr<LevelMap>& a_loadedMap) = 0;

		/* Called when the current loaded map is built */
		virtual void OnMapBuilt(MapBuiltEvent& a_builtMap) = 0;

		//Will be called when the state has been set to "Planning"
		virtual void OnPlan() = 0;
		//Will be called when the state has been set to "Simulating"
		virtual void OnSimulation() = 0;

		//Will be called when the simulation sate is on "Simulation".
		virtual void Simulate(float a_deltaTime) = 0;

		//Will be called when the simulation sate is on "Planning".
		virtual void Planning(float a_deltaTime) = 0;
	};
}
