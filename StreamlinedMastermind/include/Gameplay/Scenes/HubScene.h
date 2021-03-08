#pragma once
#include "Scenes/IMapScene.h"
#include "Scenes/SceneManager.h"
#include "BehaviorTree/BehaviorTreeTestObject.h"
#include "Pathfinding/Graph.h"
#include "Events/CharacterEventSystem.h"
#include "Gameplay/Stash.h"
#include "Gameplay/CameraController.h"
#include "Gameplay/PlayerController.h"

#include "Gui/Gui.h"
#include "Gfx/Drawables/Text.h"
#include "Gameplay/GameTileObjectFactory.h"
#include "Utils/Timer.h"

class Guard;

class HubScene : public ce::IMapScene
{
public:
	HubScene();
	~HubScene() override = default;

protected:
	//Called when we switch to another scenes
	bool OnSceneSwitched() override;

	void OnDispose() override;

	void OnUpdate(float a_deltaTime) override;
	void OnDraw(cr::RenderLayer& a_renderLayer) override;

	void OnMapLoaded(std::shared_ptr<ce::LevelMap>& a_loadedMap) override;

	void OnMapBuilt(ce::MapBuiltEvent& a_event) override;

	//Will be called when the state has been set to "Planning"
	void OnPlan() override;

    //Will be called when the simulation sate is on "Planning".
    void Planning(float a_deltaTime) override;

	//Will be called when the state has been set to "Simulating"
    void OnSimulation() override;

	//Will be called when the simulation sate is on "Simulation".
	void Simulate(float a_deltaTime) override;

private:
	std::shared_ptr<bt::BehaviorTreeTestObject> m_btTest = nullptr;
	std::unique_ptr<pf::Graph> m_graph;
	std::unique_ptr<Stash> m_stash;
	std::unique_ptr<CharacterEventSystem> m_charEventSystem = nullptr;
    
    std::unique_ptr<GameTileObjectFactory> m_gameTileObjectFactory;

	CameraController* m_camController = nullptr;
	PlayerController* m_playerController = nullptr;

	Character* m_character = nullptr;
    std::vector<Guard*> m_guards = {};
    std::vector<items::MoneyPack*> m_money = {};

	bool m_mousePressed = false;

	ce::GuiCanvas* m_canvas = nullptr;
	ce::GuiButton* m_planningPhaseBtn = nullptr;
	std::shared_ptr<ce::Texture> m_buttonTexture;

	cr::Text* m_explainText = nullptr;
    cr::Text* m_resultText = nullptr;

    ce::Timer<float> m_timer;

	//Services
	ce::GameObjectManager* m_gameObjectManager;

    bool m_ending;
    float m_intervalTime;
};

