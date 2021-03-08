#include "smpch.h"
#include "Gameplay/CameraController.h"
#include "Gameplay/PlayerController.h"
#include "Core/Input/InputManager.h"

PlayerController::PlayerController(CameraController* a_cameraToControl)
	: GameObject(true), m_camController(a_cameraToControl)
{
}

PlayerController::~PlayerController()
{
}

void PlayerController::OnCreate()
{
	
}

void PlayerController::OnUpdate(float a_deltaTime)
{
	if (m_camController == nullptr) {
		return;
	}

	if (InputManager::IsKeyDown(Keys::A))
	{
		m_camController->AddMovement(glm::vec3(1.5f, 0, 0) * a_deltaTime);
	}
	if (InputManager::IsKeyDown(Keys::D))
	{
		m_camController->AddMovement(glm::vec3(-1.5f, 0, 0) * a_deltaTime);
	}
	if (InputManager::IsKeyDown(Keys::W))
	{
		m_camController->AddMovement(glm::vec3(0, -1.5f, 0) * a_deltaTime);
	}
	if (InputManager::IsKeyDown(Keys::S))
	{
		m_camController->AddMovement(glm::vec3(0, 1.5f, 0) * a_deltaTime);
	}
}

void PlayerController::OnDraw(cr::RenderLayer &)
{
}

void PlayerController::OnDestroy()
{
}
