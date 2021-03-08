#pragma once
#include "Gui/GuiElements/GuiSprite.h"

namespace ce
{
	class GuiButton :
		public GuiSprite
	{
	public:
		GuiButton(GuiElement* a_parentGuiElement);
		~GuiButton() override;

		void OnDraw(cr::RenderLayer& a_renderLayer) override;

		Vector4 m_disabledColor{ 0.1f, 0.1f, 0.1f, 1.0f };
		Vector4 m_enabledColor{ 1.0f, 1.0f, 1.0f, 1.0f };
		Vector4 m_highlightedColor{ 0.5f, 0.5f, 0.5f, 1.0f };
		Vector4 m_pressedColor{ 0.3f, 0.3f, 0.3f, 1.0f };
		Vector4 m_selectedColor{ 1.0f, 0.5f, 0.0f, 1.0f };
	private:
		void OnPointerEnter();
		void OnPointerExit();
		void OnPointerDown();
		void OnPointerUp();
		void UpdateStateColor();
	};
}