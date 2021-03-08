#pragma once
#include "GFXHelper.h"
#include "Core/Math.h"
#include "Core/Core.h"

namespace cr { class RenderLayer; }

namespace cr
{
	struct DrawData
	{
		DrawType m_drawtype = DrawType::Quad;
		ce::ResourceId m_shaderId = ce::ResourceId(0);
		ce::ResourceId m_textureId = ce::ResourceId(0);
#ifdef PLATFORM_WINDOWS
		ce::Vector3 m_pos { 0,0,0 };
		ce::Vector2 m_size{1,1};
		float m_z = 0;
#else // PLATFORM_WINDOWS
		ce::Matrix4 m_model;
#endif
		ce::Vector4 m_color = ce::Vector4(1, 0, 1, 1);
		ce::Vector4 m_rect = ce::Vector4(0, 0, 1, 1);
		bool m_canBeCulled = true;
	};

	class IDrawable
	{
	public:
		IDrawable() = default;
		virtual ~IDrawable() = default;

		virtual void Draw(RenderLayer&) = 0;

		DrawData m_drawData;
	};
}