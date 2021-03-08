#include "cepch.h"
#include "Core/Input/Controller.h"
#include "Core/Input/KeyAction.h"
#include "Core/Input/KeyEvent.h"
#include "Core/Input/InputManager.h"
#include "Core/Input/Event.h"

Controller* Controller::ms_instance = nullptr;

Controller* Controller::Get()
{
	//Create a instance
	if (ms_instance == nullptr) {
		ms_instance = new Controller();
	}
	return ms_instance;
}

int Controller::CreateKeyAction(const char* a_actionName, const int& a_inputType, const int& a_activationKey, std::function<void(const KeyEvent&)> a_funcToInvoke)
{
	//Check if there already exists a action using this name
	int checkId = GetKeyAction(a_actionName);
	if (checkId != -1)
	{
		printf("There already exists a action called %s /n", a_actionName);
		return checkId;
	}
	//Create the keyAction and at it to the two lists
	m_actions.insert(std::pair<int, KeyAction>(m_newestId, KeyAction(a_actionName, m_newestId, a_inputType, a_activationKey, a_funcToInvoke)));
	m_keyMappings[a_inputType][a_activationKey].push_back(&m_actions[m_newestId]);

	return m_newestId++;
}

bool Controller::RemoveKeyAction(int a_actionId)
{
	if (CheckIfActionExists(a_actionId)) {
		printf("Action could not be found! action Id: %i", a_actionId);
		return false;
	}
	KeyAction& actionToRemove = m_actions[a_actionId];
	//Remove the keyaction from all the keys it
	for (size_t i = 0; i < actionToRemove.m_keys.size(); i++)
	{
		m_keyMappings[actionToRemove.m_inputType][actionToRemove.m_keys[i]].erase(std::find(m_keyMappings[actionToRemove.m_inputType][actionToRemove.m_keys[i]].begin(), m_keyMappings[actionToRemove.m_inputType][actionToRemove.m_keys[i]].end(), &actionToRemove));
		//Remove the key from the map since there is nothing in it anymore and we don't need it
		if (m_keyMappings[actionToRemove.m_inputType][actionToRemove.m_keys[i]].empty()) {
			m_keyMappings[actionToRemove.m_inputType].erase(m_keyMappings[actionToRemove.m_inputType].find(actionToRemove.m_keys[i]));
		}
	}
	m_actions.erase(m_actions.find(a_actionId));
	return true;
}

bool Controller::AddKeyToAction(const int& a_actionId, const int& a_newActivationKey)
{
	if (CheckIfActionExists(a_actionId)) {
		printf("Action could not be found! action Id: %i", a_actionId);
		return false;
	}
	KeyAction& actionToAddKeyTo = m_actions[a_actionId];
	actionToAddKeyTo.m_keys.push_back(a_newActivationKey);
	m_keyMappings[actionToAddKeyTo.m_inputType][a_newActivationKey].push_back(&actionToAddKeyTo);
	return true;
}

bool Controller::RemoveKeyFromAction(const int& a_actionId, const int& a_keyToRemove)
{
	if (CheckIfActionExists(a_actionId)) {
		printf("Action could not be found! action Id: %i", a_actionId);
		return false;
	}
	KeyAction& actionToRemoveFrom = m_actions[a_actionId];
	actionToRemoveFrom.m_keys.erase(std::find(actionToRemoveFrom.m_keys.begin(), actionToRemoveFrom.m_keys.end(), a_keyToRemove));

	m_keyMappings[actionToRemoveFrom.m_inputType][a_keyToRemove].erase(std::find(m_keyMappings[actionToRemoveFrom.m_inputType][a_keyToRemove].begin(), m_keyMappings[actionToRemoveFrom.m_inputType][a_keyToRemove].end(), &actionToRemoveFrom));
	if (m_keyMappings[a_keyToRemove].empty()) {
		m_keyMappings[actionToRemoveFrom.m_inputType].erase(m_keyMappings[actionToRemoveFrom.m_inputType].find(a_keyToRemove));
	}
	return true;
}

bool Controller::AddActionToKey(const int& a_actionId, std::function<void(const KeyEvent&)> a_newFunction) {
	if (CheckIfActionExists(a_actionId)) {
		printf("Action could not be found! action Id: %i", a_actionId);
		return false;
	}
	KeyAction& actionToAddEventTo = m_actions[a_actionId];
	actionToAddEventTo.m_keyEvent += a_newFunction;

	return true;
}

bool Controller::EnableKeyAction(const int& a_actionId, const bool& a_enabled) {
	if (CheckIfActionExists(a_actionId)) {
		printf("Action could not be found! action Id: %i", a_actionId);
		return false;
	}
	KeyAction& actionToAddEventTo = m_actions[a_actionId];
	actionToAddEventTo.m_active = a_enabled;
	return true;
}

int Controller::GetKeyAction(const char* a_actionName)
{
	for (auto action : m_actions) {
		if (action.second.m_name == a_actionName) {
			return action.first;
		}
	}
	return -1;
}

bool Controller::CheckIfActionExists(const int& a_actionId) {
	return m_actions.find(a_actionId) == m_actions.end();
}

void Controller::Update()
{
	KeyEvent nextKey;
	while ((nextKey = m_inputManager->GetNextKeyEvent()).GetEventType() != EventType::Invalid) {
		if (m_keyMappings[nextKey.GetInputType()].find(nextKey.GetScanCode()) != m_keyMappings[nextKey.GetInputType()].end()) {
			auto actionsForKey = m_keyMappings[nextKey.GetInputType()][nextKey.GetScanCode()];
			for (auto & action : actionsForKey)
			{
				action->m_keyEvent(nextKey);
			}
		}
	}
}

Controller::Controller() {
	m_inputManager = InputManager::Get();
}