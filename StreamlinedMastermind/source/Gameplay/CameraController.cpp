#include "smpch.h"
#include "Gameplay/CameraController.h"
#include "Utils/ServiceManager.h"
#include "Gfx/RenderService.h"
#include "Gfx/RenderLayer.h"
#include "Core/Input/InputManager.h"

CameraController::CameraController(ce::Camera* a_cameraToControl)
{
	m_cam = a_cameraToControl;
	m_cam->SetPosition(glm::vec3(0, 0, -1));
}

CameraController::~CameraController()
{
}

void CameraController::OnCreate()
{

}

void CameraController::OnUpdate(float)
{
	
}

void CameraController::OnDraw(cr::RenderLayer &)
{
	
}

void CameraController::OnDestroy()
{

}

void CameraController::AddMovement(ce::Vector3 a_moveAmount)
{
	m_cam->SetPosition(m_cam->GetPosition() + a_moveAmount);
}
