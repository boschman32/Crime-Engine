#pragma once
#include "Utils/IService.h"
#include "GFXHelper.h"
#include "Core/Math.h"

#include <memory>

namespace ce 
{ 
	class Camera;
	class Resource;

	class Texture;
	class Shader;
}
namespace cr
{
	class IDrawable;
	class Window;
	class RenderLayer;

	struct ResourceData;
}

namespace ce
{
	class RenderService :
		public IService
	{
	public:
		RenderService() = default;
		~RenderService() override = default;

		void OnCreate() override;
		void OnDestroy() override;

		void Clear(float a_colour[4]);
		void Peak();
		void PreRender();
		void Render();

		void LoadResouce(ce::Resource&, cr::ResourceData&);
		void UnloadResource(ce::Resource&);

		bool IsWindowActive();
		Vector2 GetSize();
		cr::RenderLayer* GetRenderLayer();
		cr::Window& GetWindow() const { return *m_window; }

		static std::shared_ptr<Shader> GetDefaultShader();
		static std::shared_ptr<Shader> GetDefaultUIShader();
		static std::shared_ptr<Texture> GetDefaultTexture();

	protected:
		
	private:
		cr::Window* m_window = nullptr;
		cr::RenderLayer* m_renderer = nullptr;
		Camera* m_camera = nullptr;

		static std::shared_ptr<Shader> ms_defaultShader;
		static std::shared_ptr<Shader> ms_defaultUIShader;
		static std::shared_ptr<Texture> ms_defaultTexture;
	};
}