#include "cepch.h"
#include "Gfx/Camera.h"

#include "Core/Input/InputManager.h"
#include "Utils/ServiceManager.h"
#include "Gfx/RenderService.h"

namespace ce
{
	static Camera* gs_camera = nullptr;

	Camera::Camera()
	{
		if (gs_camera == nullptr)
		{
			gs_camera = this;
		}
	}

	Camera::~Camera()
	{
		if (gs_camera == this)
		{
			gs_camera = nullptr;
		}
	}

	Matrix4 Camera::GetViewProjectionMatrix()
	{
		if (m_dirtyViewProjection)
		{
			m_dirtyViewProjection = false;
			m_viewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();
		}

		return m_viewProjectionMatrix;
	}

	void Camera::SetPosition(const Vector3& a_position)
	{
		m_position = a_position;

		m_dirtyView = true;
		m_dirtyViewProjection = true;
	}

	const Vector3& Camera::GetPosition()
	{
		return m_position;
	}

	void Camera::SetRotation(const Quaternion& a_rotation)
	{
		m_rotation = a_rotation; 

		m_dirtyView = true;
		m_dirtyViewProjection = true;
	}

	const Quaternion& Camera::GetRotation()
	{
		return m_rotation;
	}

	const Vector2& Camera::GetSize()
	{
		return m_size;
	}

	Camera* Camera::GetMainCamera()
	{
		return gs_camera;
	}

	Matrix4 Camera::GetTranslationMatrix()
	{
		return glm::translate(Matrix4(1.0f), m_position);
	}

	glm::vec3 Camera::CalculateMouseRay()
	{
		ce::Vector2 mousePos = InputManager::GetMousePosition();

		glm::vec2 size = ServiceManager::GetService<RenderService>()->GetSize();

		//Normalized Device Coordinates
		float x = (2.0f * mousePos.x) / size.x - 1.0f - GetPosition().x;
		float y = (1.0f - (2.0f * mousePos.y) / size.y - GetPosition().y) * -1;
		float z = 1.0f;
		glm::vec3 rayNds = glm::vec3(x, y, z);

		//Homogeneous Clip Coordinates
		glm::vec4 rayClip = glm::vec4(rayNds.x, rayNds.y, -1.0, 1.0);

		//Eye Coordinates
		glm::vec4 rayEye = glm::inverse(m_projectionMatrix) * rayClip;
		rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

		//World Coordinates
		glm::vec3 rayWor = glm::inverse(m_viewMatrix) * rayEye;

		return rayWor;
	}

	Matrix4 Camera::GetRotationMatrix()
	{
		return glm::toMat4(m_rotation);
	}
}