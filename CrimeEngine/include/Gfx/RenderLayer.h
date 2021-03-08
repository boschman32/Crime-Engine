#pragma once
#include "Gfx/GFXHelper.h"

namespace ce {
	class Resource;
	class Camera;
}

namespace cr
{
	enum class RenderAPI
	{
		None,
		DX12,
		NVN
	};

	class IDrawable;
	class Window;

	class RenderLayer
	{
	public:
		virtual ~RenderLayer() = default;
		static RenderLayer* CreateRenderLayer(Window&);

		virtual void Clear(float[4]) = 0;
		virtual void Draw(IDrawable*) = 0; // Require a Drawable which contains data : Transform, VertexBuffer, IndexBuffer, Material etc.
		virtual void RenderText(TextObject&) = 0;
		virtual void PreRender(ce::Camera&) = 0;
		virtual void Render() = 0;

		virtual void InitImGui() = 0;
		virtual void LoadResource(ce::Resource&, ResourceData&, bool = false) = 0;
		virtual void UnloadResource(ce::Resource&) = 0;
		static RenderAPI GetUsedAPI();
	};
}