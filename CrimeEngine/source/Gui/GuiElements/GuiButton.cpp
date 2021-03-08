#include "cepch.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gfx/Drawables/Sprite.h"

namespace ce
{
	GuiButton::GuiButton(GuiElement* a_parentGuiElement) :
		GuiSprite(a_parentGuiElement)
	{
		m_interaction.m_state = InteractionState::ENABLED;

		m_interaction.m_pointerEnter.AddListener<GuiButton, &GuiButton::OnPointerEnter>(this);
		m_interaction.m_pointerExit.AddListener<GuiButton, &GuiButton::OnPointerExit>(this);
		m_interaction.m_pointerDown.AddListener<GuiButton, &GuiButton::OnPointerDown>(this);
		m_interaction.m_pointerUp.AddListener<GuiButton, &GuiButton::OnPointerUp>(this);
	}

	GuiButton::~GuiButton()
	{
		m_interaction.m_pointerEnter.RemoveListener<GuiButton, &GuiButton::OnPointerEnter>(this);
		m_interaction.m_pointerExit.RemoveListener<GuiButton, &GuiButton::OnPointerExit>(this);
		m_interaction.m_pointerDown.RemoveListener<GuiButton, &GuiButton::OnPointerDown>(this);
		m_interaction.m_pointerUp.RemoveListener<GuiButton, &GuiButton::OnPointerUp>(this);
	}

	void GuiButton::OnDraw(cr::RenderLayer& a_renderLayer)
	{
		UpdateStateColor();
		GuiSprite::OnDraw(a_renderLayer);
	}

	void GuiButton::OnPointerEnter()
	{
		m_interaction.m_state = InteractionState::HIGHLIGHTED;
	}

	void GuiButton::OnPointerExit()
	{
		if (m_interaction.m_state != InteractionState::SELECTED)
		{
			m_interaction.m_state = InteractionState::ENABLED;
		}
	}

	void GuiButton::OnPointerDown()
	{
		m_interaction.m_state = InteractionState::PRESSED;
	}

	void GuiButton::OnPointerUp()
	{
		m_interaction.m_state = InteractionState::SELECTED;
	}

	void GuiButton::UpdateStateColor()
	{
		switch (m_interaction.m_state)
		{
		case InteractionState::DISABLED:
			m_sprite->SetColor(m_disabledColor);
			break;
		case InteractionState::ENABLED:
			m_sprite->SetColor(m_enabledColor);
			break;
		case InteractionState::HIGHLIGHTED:
			m_sprite->SetColor(m_highlightedColor);
			break;
		case InteractionState::PRESSED:
			m_sprite->SetColor(m_pressedColor);
			break;
		case InteractionState::SELECTED:
			m_sprite->SetColor(m_selectedColor);
			break;
		default:
			break;
		}
	}
}