#pragma once
#include "Core/Math.h"

namespace ce
{
	namespace grid_helper
	{
		UVector2 ConvertToGridPosition(const Vector3& a_pos, const UVector2& a_gridSize, bool a_isometric = false);
		UVector2 ConvertToGridPosition(const Vector2& a_pos, const UVector2& a_gridSize, bool a_isometric = false);
		Vector3 ConvertGridToWorldPosition(const UVector2& a_gridPos, const UVector2& a_gridSize, bool a_isometric = false);
		Vector2 CartesianToIsometric(const Vector2& a_pos);
		Vector2 IsometricToCartesian(const Vector2& a_isoPos);
	}
}