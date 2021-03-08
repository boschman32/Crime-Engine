#include "cepch.h"
#include "Gui/GuiElement.h"

#include "Core/Logger.h"
#include "Gui/IGuiLayoutModifier.h"

namespace ce
{
	float GuiElement::ms_offsetZ = 0.0f;
	bool GuiElement::ms_anyDirt = false;

	GuiElement::GuiElement(GuiElement* a_parent)
	{
		if (a_parent == nullptr)
		{
			m_canvas = this;
		}
		else
		{
			SetParent(*a_parent);
			m_canvas = GetCanvas();
		}

		CE_CORE_ASSERT(m_canvas);
	}

	GuiElement::~GuiElement()
	{
		if (m_parent != nullptr)
		{
			m_parent->RemoveChild(*this);

			//Add children to parent as backup.
			for (auto& c : m_children)
			{
				c->SetParent(*m_parent);
			}
		}
	}

	void GuiElement::OnLayout()
	{
		OPTICK_EVENT();

		if (IsActive() && IsDirty())
		{
			for (IGuiLayoutModifier* m : m_modifiers)
			{
				if (m != nullptr)
				{
					m->OnLayout(*this);
				}
			}

			UpdateValues();

			OnLayoutChange();

			for (auto& c : m_children)
			{
				c->SetDirty();
				c->OnLayout();
			}
		}
	}

	void GuiElement::OnLayoutChange()
	{

	}

	void GuiElement::OnDraw(cr::RenderLayer& a_renderLayer)
	{
		if (IsActive())
		{
			for (auto& c : m_children)
			{
				c->OnDraw(a_renderLayer);
			}

			m_isDirty = false;
		}
	}

	void GuiElement::SetPivot(const Vector2& a_pivot)
	{
		m_pivotPoint = a_pivot;

		SetDirty();
	}

	void GuiElement::SetAnchor(const Vector4& a_anchor)
	{
		m_anchor = a_anchor;

		if (m_anchor[0] == m_anchor[1])
		{
			m_xwlr = PointType::Offset;
		}
		else
		{
			m_xwlr = PointType::Inset;
		}

		if (m_anchor[2] == m_anchor[3])
		{
			m_yhtb = PointType::Offset;
		}
		else
		{
			m_yhtb = PointType::Inset;
		}

		SetDirty();
	}

	void GuiElement::SetRect(float a_xl, float a_yt, float a_wr, float a_hb)
	{
		SetRect({ a_xl, a_yt, a_wr, a_hb });
	}

	void GuiElement::SetRect(const Vector4& a_rect)
	{
		m_rect = a_rect;

		SetDirty();
	}

	void GuiElement::AddLayoutModifier(IGuiLayoutModifier& a_guiLayoutModifier)
	{
		m_modifiers.push_back(&a_guiLayoutModifier);

		SetDirty();
	}

	void GuiElement::RemoveLayoutModifier(IGuiLayoutModifier& a_guiLayoutModifier)
	{
		m_modifiers.erase(std::remove(m_modifiers.begin(), m_modifiers.end(), &a_guiLayoutModifier), m_modifiers.end());

		SetDirty();
	}

	void GuiElement::SetDirty()
	{
		m_isDirty = true;
		ms_anyDirt = true;
	}

	void GuiElement::SetActive(bool a_active)
	{
		if (a_active && !IsActive())
		{
			SetDirty();
		}
		
		m_isActive = a_active;
	}

	void GuiElement::GetChildren(std::vector<GuiElement*>& a_children)
	{
		OPTICK_EVENT();

		if (!m_children.empty())
		{
			a_children.insert(a_children.end(), m_children.begin(), m_children.end());
			for (const auto& c : m_children)
			{
				c->GetChildren(a_children);
			}
		}
	}

	void GuiElement::SetParent(GuiElement& a_parent)
	{
		if (&a_parent != this && &a_parent != m_parent)
		{
			if (m_parent != nullptr)
			{
				m_parent->RemoveChild(*this);
			}

			m_parent = &a_parent;
			m_parent->AddChild(*this);

			SetDirty();
		}
		else
		{
			CE_APP_WARN("Couldn't set parent, either parent is already set to this object or parent is this object!");
		}
	}

	const GuiElement* GuiElement::GetCanvas() const
	{
		if (m_canvas == nullptr && m_parent != nullptr)
		{
			m_canvas = m_parent->GetCanvas();
		}

		return m_canvas;
	}

	void GuiElement::UpdateValues()
	{
		OPTICK_EVENT();

		CalculateRectOffsetMin();
		CalculateRectOffsetMax();
		CalculateSize();
		CalculatePivotOffset();
		CalculatePosition();
	}

	void GuiElement::ClearDirt() const
	{
		ms_anyDirt = false;
	}

	void GuiElement::AddChild(GuiElement& a_child)
	{
		if (&a_child != this)
		{
			m_children.push_back(&a_child);

			SetDirty();
		}
		else
		{
			CE_APP_WARN("Couldn't set child, can't add youself!");
		}
	}

	void GuiElement::RemoveChild(const GuiElement& a_child)
	{
		m_children.erase(std::remove(m_children.begin(), m_children.end(), &a_child), m_children.end());

		SetDirty();
	}

	void GuiElement::CalculateSize()
	{
		OPTICK_EVENT();

		Vector2 size{ 0, 0 };

		Vector2 roMin = GetRectOffsetMin();
		Vector2 roMax = GetRectOffsetMax();
		Vector2 ps{ 0, 0 };

		if (m_parent != nullptr)
		{
			ps = m_parent->GetSize();
		}

		switch (m_xwlr)
		{

		case PointType::Offset:
		{
			size.x = m_rect.z;
			break;
		}
		case PointType::Inset:
		{
			size.x = ps.x - roMin.x - roMax.x - m_rect.x - m_rect.z;
			break;
		}
		default:
			CE_APP_ERROR("Invalid PointType!");
			break;
		}

		switch (m_yhtb)
		{
		case ce::PointType::Offset:
		{
			size.y = m_rect.w;
			break;
		}
		case ce::PointType::Inset:
		{
			size.y = ps.y - roMin.y - roMax.y - m_rect.y - m_rect.w;
			break;
		}
		default:
			CE_APP_ERROR("Invalid PointType!");
			break;
		}

		m_size = size;
	}

	void GuiElement::CalculatePosition()
	{
		OPTICK_EVENT();

		Vector2 position{ 0, 0 };

		Vector2 po = GetPivotOffset();
		Vector2 ro = GetRectOffsetMin();
		Vector2 pp{ 0, 0 };

		if (m_parent != nullptr)
		{
			pp = m_parent->GetPosition();
		}

		switch (m_xwlr)
		{
		case PointType::Offset:
		{
			position.x = m_rect.x + po.x + ro.x + pp.x;
			break;
		}
		case PointType::Inset:
		{
			position.x = m_rect.x + ro.x + pp.x;
			break;
		}
		default:
			CE_APP_ERROR("Invalid PointType!");
			break;
		}

		switch (m_yhtb)
		{
		case ce::PointType::Offset:
		{
			position.y = m_rect.y + po.y + ro.y + pp.y;
			break;
		}
		case ce::PointType::Inset:
		{
			position.y = m_rect.y + ro.y + pp.y;
			break;
		}
		default:
			CE_APP_ERROR("Invalid PointType!");
			break;
		}

		m_position = position;
	}

	void GuiElement::CalculatePivotOffset()
	{
		OPTICK_EVENT();

		Vector2 offset{ 0, 0 };

		offset -= (GetSize() * m_pivotPoint);

		m_pivotOffset = offset;
	}

	void GuiElement::CalculateRectOffsetMin()
	{
		OPTICK_EVENT();

		Vector2 offset{ 0, 0 };

		if (m_parent != nullptr)
		{
			Vector2 ps = GetParent()->GetSize();
			offset.x = ps.x * m_anchor.x;
			offset.y = ps.y * m_anchor.z;
		}

		m_rectOffsetMin = offset;
	}

	void GuiElement::CalculateRectOffsetMax()
	{
		OPTICK_EVENT();

		Vector2 offset{ 0, 0 };

		if (m_parent != nullptr)
		{
			Vector2 ps = GetParent()->GetSize();
			offset.x = ps.x - (ps.x * m_anchor.y);
			offset.y = ps.y - (ps.y * m_anchor.w);
		}

		m_rectOffsetMax = offset;
	}
}