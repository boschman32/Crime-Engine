#pragma once
#include "Gfx/IDrawable.h"
#include "Core/Math.h"

namespace ce { class Texture; }
namespace ce { class Shader; }
namespace cr { class RenderLayer; }

namespace ce
{
	class Sprite : 
		public cr::IDrawable
	{
	public:
		Sprite();
		~Sprite() override;

		void Draw(cr::RenderLayer& a_renderLayer) override;
		void SetTexture(ce::Texture& a_texture);
		void SetTexture(ce::Texture& a_texture, ce::Vector4& a_rect);
		void SetRect(ce::Vector4& a_rect);
		void SetShader(ce::Shader& a_shader);

		void SetColor(Vector4 a_color);
		void SetPosition(Vector3 a_pos);
		void SetRotation(Vector3 a_rot);
		void SetRotationZ(float a_zRot);
		void SetScale(Vector2 a_scale);

		Vector3 GetPosition() const { return m_position; }
		Vector3 GetRotation() const { return m_rotation; }
		Vector2 GetScale() const { return m_scale; }

		void SetCullable(bool);
	protected:
		bool m_onTransform = true;
		void UpdateDrawData();

		Vector3 m_position = Vector3(0, 0, 0);
		Vector2 m_scale = Vector3(1, 1, 1);
		Vector3 m_rotation = Vector3(0, 0, 0);
		Vector4 m_color = Vector4(1, 0, 1, 1);
	private:
	};
}