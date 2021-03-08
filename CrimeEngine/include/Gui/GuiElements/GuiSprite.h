#pragma once
#include "Gui/GuiElement.h"
#include "Gfx/Drawables/Sprite.h"
#include <memory>

namespace ce { class Sprite; }

namespace ce
{
	class GuiSprite :
		public GuiElement
	{
	public:
		GuiSprite(GuiElement* a_parentGuiElement);
		~GuiSprite() override;

		void OnLayoutChange() override;
		void OnDraw(cr::RenderLayer& a_renderLayer) override;

		Sprite* GetSprite() const;
	protected:
		Sprite* m_sprite = nullptr;
	private:


	};
}