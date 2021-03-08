#include "cepch.h"
#include "Gui/GuiElements/GuiGridLayoutGroup.h"

namespace ce
{
	GuiGridLayoutGroup::GuiGridLayoutGroup(GuiElement* a_parent) :
		GuiElement(a_parent)
	{

	}

	void GuiGridLayoutGroup::OnLayoutChange()
	{
		float sqrRt = static_cast<float>(glm::sqrt(m_children.size()));
		int rows = static_cast<int>(glm::ceil(sqrRt));
		int columns = static_cast<int>(glm::ceil(sqrRt));

		Vector2 s = GetSize();
		Vector2 cs = { 0, 0 };
			
		//Set base scale.
		cs.x = s.x;
		cs.y = s.y;

		//Apply spacing.
		if (rows > 0)
		{
			cs.x = cs.x - (m_spacing.x * float(rows - 1));
		}

		if (columns > 0)
		{
			cs.y = cs.y - (m_spacing.y * float(columns - 1));
		}

		//Apply offset.
		cs.x = cs.x - m_offset.x - m_offset.z;
		cs.y = cs.y - m_offset.y - m_offset.w;

		//Divide over children.
		cs.x /= float(rows);
		cs.y /= float(columns);

		float rc = 0;
		float cc = 0;

		for (int i = 0; i < m_children.size(); i++)
		{
			rc = glm::floor(float(i) / float(columns));
			cc = glm::ceil(float(i % columns));

			float x = (cs.x * cc) + m_offset.x + (m_spacing.x * cc);
			float y = (cs.y * rc) + m_offset.y + (m_spacing.y * rc);

			m_children[i]->SetAnchor(anchor::TopLeft);
			m_children[i]->SetPivot(pivot::TopLeft);
			m_children[i]->SetRect(x, y, cs.x, cs.y);
		}
	}
}