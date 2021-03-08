#include "Gameplay/GameObject.h"

class CameraController;
class PlayerController : public ce::GameObject
{
public:
	PlayerController(CameraController* a_cameraToControl);
	~PlayerController() override;

	void OnCreate() override;
	void OnUpdate(float a_deltaTime) override;
	void OnDraw(cr::RenderLayer& a_renderLayer) override;
	void OnDestroy() override;

private:
	CameraController* m_camController;
};

