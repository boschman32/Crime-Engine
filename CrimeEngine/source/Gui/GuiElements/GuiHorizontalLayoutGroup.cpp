#include "cepch.h"
#include "Gui/GuiElements/GuiHorizontalLayoutGroup.h"

namespace ce
{
	GuiHorizontalLayoutGroup::GuiHorizontalLayoutGroup(GuiElement* a_parentGuiElement, bool a_controlChildWidth /*= true*/, bool a_controlChildHeight /*= true*/) :
		GuiElement(a_parentGuiElement)
	{
		m_controlChildWidth = a_controlChildWidth;
		m_controlChildHeight = a_controlChildHeight;
	}

	void GuiHorizontalLayoutGroup::OnLayoutChange()
	{
		Vector2 s = GetSize();
		Vector2 ls{ 0, 0 };
		int ch = int(m_children.size());

		//Set base scale.
		ls.x = s.x;
		ls.y = s.y;

		//Apply spacing.
		if (ch > 0)
		{
			ls.x = ls.x - (m_spacing * float(ch - 1));
		}

		//Apply offset.
		ls.x = ls.x - m_offset.x - m_offset.z;
		ls.y = ls.y - m_offset.y - m_offset.w;

		//Divide over children.
		ls.x /= float(ch);

		Vector2 p{ 0, 0 };
		//Apply offset.
		p.x += m_offset.x;
		p.y += m_offset.y;

		for (auto& c : m_children)
		{
			Vector2 cs = c->GetSize();

			if (m_controlChildWidth)
			{
				cs.x = ls.x;
			}

			if (m_controlChildHeight)
			{
				cs.y = ls.y;
			}

			c->SetAnchor(anchor::TopLeft);
			c->SetPivot(pivot::TopLeft);
			c->SetRect(p.x, p.y, cs.x, cs.y);

			p.x += cs.x;
			//Add spacing.
			p.x += m_spacing;
		}
	}
}