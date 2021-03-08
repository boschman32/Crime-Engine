#include "cepch.h"

#include "Gfx/RenderLayer.h"
#include "Gfx/Drawables/Text.h"

void cr::Text::Draw(RenderLayer& a_renderLayer)
{
	a_renderLayer.RenderText(m_textObject);
}

void cr::Text::SetPos(ce::Vector2 a_pos)
{
	m_textObject.m_pos = a_pos;
}

void cr::Text::SetSize(ce::Vector2 a_size)
{
	m_textObject.m_size = a_size;
}

void cr::Text::SetColour(ce::Vector4 a_color)
{
	m_textObject.m_color = a_color;
}

void cr::Text::SetText(std::string a_text)
{
	m_textObject.m_text = a_text;
}
