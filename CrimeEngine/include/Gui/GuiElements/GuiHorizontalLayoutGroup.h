#pragma once
#include "Gui/GuiElement.h"

namespace ce
{
	class GuiHorizontalLayoutGroup :
		public GuiElement
	{
	public:
		GuiHorizontalLayoutGroup(GuiElement* a_parentGuiElement, bool a_controlChildWidth = true, bool a_controlChildHeight = true);
		~GuiHorizontalLayoutGroup() override = default;

		void OnLayoutChange() override;

		bool m_controlChildWidth = true;
		bool m_controlChildHeight = true;

		Vector4 m_offset{ 0, 0, 0, 0 };
		float m_spacing{ 0 };
	};
}