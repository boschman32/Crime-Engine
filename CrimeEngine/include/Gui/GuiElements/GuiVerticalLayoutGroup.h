#pragma once
#include "Gui/GuiElement.h"

namespace ce
{
	class GuiVerticalLayoutGroup :
		public GuiElement
	{
	public:
		GuiVerticalLayoutGroup(GuiElement* a_parent, bool a_controlChildWidth = true, bool a_controlChildHeight = true);
		~GuiVerticalLayoutGroup() override = default;

		void OnLayoutChange() override;

		bool m_controlChildWidth = true;
		bool m_controlChildHeight = true;

		Vector4 m_offset{ 0, 0, 0, 0 };
		float m_spacing{ 0 };
	};
}