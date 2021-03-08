#include "cepch.h"
#include "Gfx/Drawables/Sprite.h"

#include "Gfx/Camera.h"
#include "Gfx/RenderLayer.h"
#include "Core/ResourceManager.h"
#include "Utils/ServiceManager.h"
#include "Gfx/Shader.h"
#include "Gfx/Texture.h"

namespace ce
{
	Sprite::Sprite()
	{
		m_drawData.m_drawtype = cr::DrawType::Quad;
		SetShader(*RenderService::GetDefaultShader());
		SetTexture(*RenderService::GetDefaultTexture());
	}

	Sprite::~Sprite()
	{
	}

	void Sprite::Draw(cr::RenderLayer& a_renderLayer)
	{
		a_renderLayer.Draw(this);
	}

	void Sprite::SetTexture(ce::Texture& a_texture)
	{
		m_drawData.m_textureId = a_texture.GetID();
		SetColor({ 1,1,1,1 });
	}

	void Sprite::SetTexture(ce::Texture& a_texture, ce::Vector4& a_rect)
	{
		m_drawData.m_textureId = a_texture.GetID();
		m_drawData.m_rect = a_rect;
		SetColor({ 1,1,1,1 });
	}

	void Sprite::SetRect(ce::Vector4 & a_rect)
	{
		m_drawData.m_rect = a_rect;
	}

	void Sprite::SetShader(ce::Shader& a_shader)
	{
		m_drawData.m_shaderId = a_shader.GetID();
	}

	void Sprite::SetColor(Vector4 a_color)
	{
		m_color = a_color;
		UpdateDrawData();
	}

	void Sprite::SetPosition(Vector3 a_pos)
	{
		m_position = a_pos;
		//Invert the Y position since tiled uses TopLeft as 0,0 instead of BottomLeft.
		m_position.y = -m_position.y;
		UpdateDrawData();
	}

	void Sprite::SetRotation(Vector3 a_rot)
	{
		m_rotation = a_rot;
		UpdateDrawData();
	}

	void Sprite::SetRotationZ(float a_zRot)
	{
		m_rotation.z = a_zRot;
		UpdateDrawData();
	}

	void Sprite::SetScale(Vector2 a_scale)
	{
		m_scale = a_scale;
		UpdateDrawData();
	}

	void Sprite::SetCullable(bool a_active)
	{
		m_drawData.m_canBeCulled = a_active;
	}

	void Sprite::UpdateDrawData()
	{
#ifdef PLATFORM_WINDOWS
		m_drawData.m_pos = m_position;
		m_drawData.m_z = m_rotation.z;
		m_drawData.m_size = m_scale;
#else // PLATFORM_WINDOWS
		Matrix4 s = glm::scale(Matrix4(1.0f), Vector3(m_scale, 1));
		Quaternion q = Quaternion(m_rotation);
		Matrix4 r = glm::toMat4(q);
		Matrix4 p = glm::translate(Matrix4(1.0f), Vector3(m_position));

		m_drawData.m_model = p * s * r;
#endif

		m_drawData.m_color = m_color;
	}
}