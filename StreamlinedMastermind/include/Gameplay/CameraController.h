#pragma once
#include "Core/Math.h"
#include "Gfx/Camera2D.h"
#include "Gameplay/GameObject.h"

class CameraController : public ce::GameObject
{
public:
	CameraController(ce::Camera* a_cameraToControl);
	~CameraController() override;

	void OnCreate() override;
	void OnUpdate(float a_deltaTime) override;
	void OnDraw(cr::RenderLayer& a_renderLayer) override;
	void OnDestroy() override;

	void AddMovement(ce::Vector3 a_moveAmount);

	ce::Camera* m_cam;
private:
};
