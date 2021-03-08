#pragma once
#include "GuiDelegate.h"
#include "Core/Math.h"

namespace ce
{
	namespace pivot
	{
		constexpr Vector2 TopLeft{ 0.0f, 0.0f };
		constexpr Vector2 TopCenter{ 0.5f, 0.0f };
		constexpr Vector2 TopRight{ 1.0f, 0.0f };
		constexpr Vector2 MiddleLeft{ 0.0f, 0.5f };
		constexpr Vector2 MiddleCenter{ 0.5f, 0.5f };
		constexpr Vector2 MiddleRight{ 1.0f, 0.5f };
		constexpr Vector2 BottomLeft{ 0.0f, 1.0f };
		constexpr Vector2 BottomCenter{ 0.5f, 1.0f };
		constexpr Vector2 BottomRight{ 1.0f, 1.0f };
	}

	namespace anchor
	{
		//constexpr Vector4 TopLeft		{ 0.0f, 0.0f, 1.0f, 1.0f };
		//constexpr Vector4 TopCenter		{ 0.5f, 0.5f, 1.0f, 1.0f };
		//constexpr Vector4 TopRight		{ 1.0f, 1.0f, 1.0f, 1.0f };
		//constexpr Vector4 MiddleLeft	{ 0.0f, 0.0f, 0.5f, 0.5f };
		//constexpr Vector4 MiddleCenter	{ 0.5f, 0.5f, 0.5f, 0.5f };
		//constexpr Vector4 MiddleRight	{ 1.0f, 1.0f, 0.5f, 0.5f };
		//constexpr Vector4 BottomLeft	{ 0.0f, 0.0f, 0.0f, 0.0f };
		//constexpr Vector4 BottomCenter	{ 0.5f, 0.5f, 0.0f, 0.0f };
		//constexpr Vector4 BottomRight	{ 1.0f, 1.0f, 0.0f, 0.0f };
		//constexpr Vector4 TopStretch	{ 0.0f, 1.0f, 1.0f, 1.0f };
		//constexpr Vector4 MiddleStretch	{ 0.0f, 1.0f, 0.5f, 0.5f };
		//constexpr Vector4 BottomStretch	{ 0.0f, 1.0f, 0.0f, 0.0f };
		//constexpr Vector4 StretchLeft	{ 0.0f, 0.0f, 0.0f, 1.0f };
		//constexpr Vector4 StretchCenter	{ 0.5f, 0.5f, 0.0f, 1.0f };
		//constexpr Vector4 StretchRight	{ 1.0f, 1.0f, 0.0f, 1.0f };
		//constexpr Vector4 StretchStretch{ 0.0f, 1.0f, 0.0f, 1.0f };

		constexpr Vector4 BottomLeft{ 0.0f, 0.0f, 1.0f, 1.0f };
		constexpr Vector4 BottomCenter{ 0.5f, 0.5f, 1.0f, 1.0f };
		constexpr Vector4 BottomRight{ 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr Vector4 MiddleLeft{ 0.0f, 0.0f, 0.5f, 0.5f };
		constexpr Vector4 MiddleCenter{ 0.5f, 0.5f, 0.5f, 0.5f };
		constexpr Vector4 MiddleRight{ 1.0f, 1.0f, 0.5f, 0.5f };
		constexpr Vector4 TopLeft{ 0.0f, 0.0f, 0.0f, 0.0f };
		constexpr Vector4 TopCenter{ 0.5f, 0.5f, 0.0f, 0.0f };
		constexpr Vector4 TopRight{ 1.0f, 1.0f, 0.0f, 0.0f };
		constexpr Vector4 BottomStretch{ 0.0f, 1.0f, 1.0f, 1.0f };
		constexpr Vector4 MiddleStretch{ 0.0f, 1.0f, 0.5f, 0.5f };
		constexpr Vector4 TopStretch{ 0.0f, 1.0f, 0.0f, 0.0f };
		constexpr Vector4 StretchLeft{ 0.0f, 0.0f, 0.0f, 1.0f };
		constexpr Vector4 StretchCenter{ 0.5f, 0.5f, 0.0f, 1.0f };
		constexpr Vector4 StretchRight{ 1.0f, 1.0f, 0.0f, 1.0f };
		constexpr Vector4 StretchStretch{ 0.0f, 1.0f, 0.0f, 1.0f };
	}

	enum class PointType
	{
		Offset,
		Inset
	};

	enum class InteractionState
	{
		DISABLED = 0,
		ENABLED = 1,
		HIGHLIGHTED = 2,
		PRESSED = 3,
		SELECTED = 4
	};

	struct GuiInteraction
	{
		InteractionState m_state = InteractionState::DISABLED;
		GuiDelegate m_pointerEnter;
		GuiDelegate m_pointerExit;
		GuiDelegate m_pointerDown;
		GuiDelegate m_pointerUp;
	};
}