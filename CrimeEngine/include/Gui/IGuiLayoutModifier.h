#pragma once

namespace ce { class GuiElement; }

namespace ce
{
	class IGuiLayoutModifier
	{
	public:
		IGuiLayoutModifier() = default;
		virtual ~IGuiLayoutModifier() = default;

		virtual void OnLayout(GuiElement& a_guiElement) = 0;

	protected:
	private:
	};
}