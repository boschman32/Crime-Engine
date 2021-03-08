#pragma once
#include "Gameplay/GameObject.h"
#include "Gui/GuiElement.h"

namespace ce { class Camera; }

namespace ce
{
	class GuiCanvas :
		public GameObject, 
		public GuiElement
	{
	public:
		GuiCanvas(ce::Camera& a_camera);
		~GuiCanvas() override = default;

		void OnCreate() override;
		void OnUpdate(float a_deltaTime) override;
		void OnDraw(cr::RenderLayer& a_renderLayer) override;
		void OnDestroy() override;

	protected:
	private:
		void UpdateInteractables();
		bool IsInteracting(const Vector2& a_point, const Vector4& a_rect) const;
		GuiElement* m_selectedElement = nullptr;

		float m_scale = 1.0f;
		ce::Camera& m_camera;

		std::vector<GuiElement*> m_allChildren = std::vector<GuiElement*>();
	};
}