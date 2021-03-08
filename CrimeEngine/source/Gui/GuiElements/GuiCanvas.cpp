#include "cepch.h"
#include "Gui/GuiElements/GuiCanvas.h"

#include "Gfx/Camera.h"
#include "Core/Input/InputManager.h"

namespace ce
{
	GuiCanvas::GuiCanvas(ce::Camera& a_camera) :
		GuiElement(nullptr),
		m_camera(a_camera)
	{
		SetAnchor(anchor::MiddleCenter);
		//Offset the canvas from the center.
		SetPivot(pivot::MiddleCenter);
	}

	void GuiCanvas::OnCreate()
	{
	}

	void GuiCanvas::OnUpdate(float)
	{
		OPTICK_EVENT();

		if (GuiElement::IsActive())
		{
			Vector2 size = m_camera.GetSize();

			if (size != GetSize())
			{
				SetRect(0, 0, size.x, size.y);

				UpdateValues();

				//Update layout.
				for (auto& c : m_children)
				{
					c->SetDirty();

					//TODO: Remove temp fix for Gui sorting.
					ms_offsetZ = 1.0f;
					c->OnLayout();
				}
			}

			if (AnyDirt())
			{
				//Update children.
				m_allChildren.clear();
				GetChildren(m_allChildren);
				ClearDirt();
			}

			//GuiInteraction.
			UpdateInteractables();
		}
	}

	void GuiCanvas::OnDraw(cr::RenderLayer& a_renderLayer)
	{
		GuiElement::OnDraw(a_renderLayer);
	}

	void GuiCanvas::OnDestroy()
	{
	}

	void GuiCanvas::UpdateInteractables()
	{
		OPTICK_EVENT();

		bool didHit = false;
		bool isMouseDown = InputManager::IsMouseButtonDown(MouseKeys::LButton);
		Vector2 mousePosition = InputManager::GetMousePosition();
		mousePosition -= (GetSize() * 0.5f);

		for (auto* c : m_allChildren)
		{
			if (c != nullptr)
			{
				//Is this element enabled?
				if (c->m_interaction.m_state != InteractionState::DISABLED)
				{
					Vector2 s = c->GetSize() * 0.5f;
					Vector2 p = c->GetPosition() + s;

					//Are we interacting with it?
					if (IsInteracting(mousePosition, { p.x, p.y, s.x, s.y }))
					{
						didHit = true;

						//Is it already being hovered.
						if (c->m_interaction.m_state == InteractionState::ENABLED)
						{
							c->m_interaction.m_state = InteractionState::HIGHLIGHTED;
							c->m_interaction.m_pointerEnter.Invoke();
						}

						//Press element.
						if (isMouseDown &&
							c->m_interaction.m_state != InteractionState::PRESSED)
						{
							c->m_interaction.m_state = InteractionState::PRESSED;
							c->m_interaction.m_pointerDown.Invoke();
						}

						//Select element.
						if (!isMouseDown &&
							c->m_interaction.m_state == InteractionState::PRESSED)
						{
							//Update selection.
							if (m_selectedElement != nullptr)
							{
								m_selectedElement->m_interaction.m_state = InteractionState::ENABLED;
							}
							m_selectedElement = c;

							c->m_interaction.m_state = InteractionState::SELECTED;
							c->m_interaction.m_pointerUp.Invoke();
						}
					}
					else if (c->m_interaction.m_state != InteractionState::ENABLED)
					{
						c->m_interaction.m_pointerExit.Invoke();
					}
				}
			}
		}

		//De-select on miss click.
		if (!didHit && isMouseDown && m_selectedElement != nullptr)
		{
			m_selectedElement->m_interaction.m_state = InteractionState::ENABLED;
		}
	}

	bool GuiCanvas::IsInteracting(const Vector2& a_point, const Vector4& a_rect) const
	{
		OPTICK_EVENT();

		bool isInteracting = false;

		if (a_point.x > a_rect.x - a_rect.z && a_point.x < a_rect.x + a_rect.z)
		{
			if (a_point.y > a_rect.y - a_rect.w && a_point.y < a_rect.y + a_rect.w)
			{
				isInteracting = true;
			}
		}

		return isInteracting;
	}
}