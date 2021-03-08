#pragma once
#include "Core/Core.h"
#include "Core/Math.h"

namespace ce
{
	class Camera
	{
	public:
		Camera();
		virtual ~Camera();

		virtual Matrix4 GetProjectionMatrix() = 0;
		virtual Matrix4 GetInverseProjectionMatrix() = 0;
		virtual Matrix4 GetViewMatrix() = 0;
		virtual Matrix4 GetInverseViewMatrix() = 0;
		Matrix4 GetViewProjectionMatrix();

		void SetPosition(const Vector3& a_position);
		const Vector3& GetPosition();
		void SetRotation(const Quaternion& a_rotation);
		const Quaternion& GetRotation();
		const Vector2& GetSize();

		static Camera* GetMainCamera();
		Matrix4 GetTranslationMatrix();

		glm::vec3 CalculateMouseRay();

	protected:
		virtual void UpdateProjectionMatrix() = 0;
		virtual void UpdateViewMatrix() = 0;

		Matrix4 GetRotationMatrix();

		bool m_dirtyView = true;
		bool m_dirtyProjection = true;
		bool m_dirtyViewProjection = true;

		Vector3 m_position;
		Quaternion m_rotation;
		//Current window/render size.
		Vector2 m_size;

		Matrix4 m_projectionMatrix;
		Matrix4 m_inverseProjectionMatrix;
		Matrix4 m_viewMatrix;
		Matrix4 m_inverseViewMatrix;
		Matrix4 m_viewProjectionMatrix;

		static Camera* ms_mainCamera;
	};
}