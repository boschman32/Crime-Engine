#include "cepch.h"
#include "Gui/GuiModifiers/MGuiAutoSize.h"

#include "Gui/GuiElement.h"

namespace ce
{

	MGuiAutoSize::MGuiAutoSize(bool a_expandWidth /*= true*/, bool a_expandHeight /*= true*/)
	{
		m_expandWidth = a_expandWidth;
		m_expandHeight = a_expandHeight;
	}

	void MGuiAutoSize::OnLayout(GuiElement& a_guiElement)
	{
		auto* parent = a_guiElement.GetParent();
		if (parent != nullptr)
		{	
			Vector4 rect = a_guiElement.GetRect();

			if (m_expandWidth)
			{
				switch (a_guiElement.GetXWLR())
				{
				case PointType::Offset:
				{
					rect.x = 0;
					rect.z = parent->GetSize().x;
					break;
				}
				case PointType::Inset:
				{
					rect.x = 0;
					rect.z = 0;
					break;
				}
				default:
					CE_APP_ERROR("Invalid PointType!");
					break;
				}

				a_guiElement.SetDirty();
			}

			if (m_expandHeight)
			{
				switch (a_guiElement.GetXWLR())
				{
				case PointType::Offset:
				{
					rect.y = 0;
					rect.w = parent->GetSize().y;
					break;
				}
				case PointType::Inset:
				{
					rect.y = 0;
					rect.w = 0;
					break;
				}
				default:
					CE_APP_ERROR("Invalid PointType!");
					break;
				}

				a_guiElement.SetDirty();
			}
		}
	}
}