#pragma once
#include "Gfx/Camera.h"
#include "Core/Math.h"

namespace ce
{
	class Camera2D :
		public Camera
	{
	public:
		Camera2D();
		~Camera2D() override = default;

		//The bounds of what always needs to be on screen.
		//Usually a value aligned with the aspect ratio.
		void SetBounds(const Vector2& a_bounds);
		Vector2 inline GetBounds() const { return m_bounds; }
		//Near and far render distance.
		void SetRenderDistance(const Vector2& a_z);
		//Usually the render or window size.
		void SetOrthographicProjection(const Vector2& a_size);
		//Look at a target from the current direction.
		void LookAt(const Vector3& a_target, const Vector3& a_up);
		//Look in a direction.
		void LookDir(const Vector3& a_dir, const Vector3& a_up);

		Matrix4 GetProjectionMatrix() override;
		Matrix4 GetInverseProjectionMatrix() override;
		Matrix4 GetViewMatrix() override;
		Matrix4 GetInverseViewMatrix() override;

	protected:
		void UpdateProjectionMatrix() override;
		void UpdateViewMatrix() override;

	private:
		//What to keep on screen.
		Vector2 m_bounds = Vector2(16, 9);
		//Near, far clipping plane Z.
		Vector2 m_z = Vector2(0.1f, 100.0f);
	};
}