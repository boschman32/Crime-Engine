#pragma once
#include "Gui/GuiElement.h"

namespace ce
{
	class GuiGridLayoutGroup :
		public GuiElement
	{
	public:
		GuiGridLayoutGroup(GuiElement* a_parent);
		~GuiGridLayoutGroup() override = default;

		void OnLayoutChange() override;

		Vector4 m_offset{ 0, 0, 0, 0 };
		Vector2 m_spacing{ 0, 0 };
	};
}