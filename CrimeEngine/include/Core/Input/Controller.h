#pragma once
#include <functional>
#include <map>
#include <vector>

class KeyEvent;
class KeyAction;
class InputManager;

class Controller
{
public:
	Controller(const Controller&) = delete;

	static Controller* Get();
	int CreateKeyAction(const char* a_actionName,const int& a_inputType, const int& a_activationKey, std::function<void(const KeyEvent&)> a_funcToInvoke);
	bool RemoveKeyAction(int a_actionId);
	bool AddKeyToAction(const int& a_actionId, const int& a_newActivationKey);
	bool RemoveKeyFromAction(const int& a_actionId, const int& a_keyToRemove);
	bool AddActionToKey(const int& a_actionId, std::function<void(const KeyEvent&)> a_newFunction);
	bool EnableKeyAction(const int& a_actionId, const bool& a_enabled);
	int GetKeyAction(const char* a_actionName);

	void Update();

private:
	Controller();
	~Controller() = default;

	bool CheckIfActionExists(const int& a_actionId);

	std::map<int, std::vector<KeyAction*>> m_keyMappings[3];
	std::map<int, KeyAction> m_actions;
	int m_newestId = 0;
	InputManager* m_inputManager;
	static Controller* ms_instance;
};

