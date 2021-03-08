#include "cepch.h"
#include "Gui/GuiElements/GuiSprite.h"

#include "Gfx/Drawables/Sprite.h"
#include "Gfx/RenderService.h"

#include "Gfx/GFXHelper.h"

namespace ce
{
	GuiSprite::GuiSprite(GuiElement* a_parentGuiElement) :
		GuiElement(a_parentGuiElement)
	{
		m_sprite = new Sprite();
		GetSprite()->SetShader(*RenderService::GetDefaultUIShader());
		GetSprite()->SetCullable(false);
	}

	GuiSprite::~GuiSprite()
	{
		delete GetSprite();
	}

	void GuiSprite::OnLayoutChange()
	{
		ms_offsetZ -= 0.0001f;
		Vector2 s = GetSize();
		Vector2 p = GetPosition() + (s * 0.5f);

		GetSprite()->SetScale(Vector3(s, 1));
		//Add extra offset to compensate for the sprite mesh center pivot point.
		GetSprite()->SetPosition(Vector3(p, ms_offsetZ));
	}

	void GuiSprite::OnDraw(cr::RenderLayer& a_renderLayer)
	{
		if(IsActive())
		{
			GetSprite()->Draw(a_renderLayer);
			GuiElement::OnDraw(a_renderLayer);
		}
	}

	Sprite* GuiSprite::GetSprite() const
	{
		CE_CORE_ASSERT(m_sprite);
		return m_sprite;
	}
}