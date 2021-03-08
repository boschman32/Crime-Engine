#pragma once
#include "Core/Math.h"

namespace cr
{
	class RenderLayer;
	class Text
	{
	public:
		Text() = default;
		~Text() = default;

		void Draw(RenderLayer&);

		void SetPos(ce::Vector2);
		void SetSize(ce::Vector2);
		void SetColour(ce::Vector4);
		void SetText(std::string);
	private:
		TextObject m_textObject;
	};
}