#pragma once
#include "Gui/IGuiLayoutModifier.h"

namespace ce { class GuiElement; }

namespace ce
{
	class MGuiAutoSize :
		public IGuiLayoutModifier
	{
	public:
		MGuiAutoSize(bool a_expandWidth = true, bool a_expandHeight = true);
		~MGuiAutoSize() override = default;

		void OnLayout(GuiElement& a_guiElement) override;

		bool m_expandWidth = true;
		bool m_expandHeight = true;

	protected:
	private:
	};
}