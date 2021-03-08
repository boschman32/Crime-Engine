#include "cepch.h"
#include "Gameplay/Map/GridHelper.h"

namespace ce
{
	namespace grid_helper
	{
		UVector2 ConvertToGridPosition(const Vector2& a_pos, const UVector2& a_gridSize, bool a_isometric)
		{
			Vector2 cartPos = a_pos;
			if(a_isometric)
			{
				cartPos = IsometricToCartesian(a_pos);
			}

			const float x = (cartPos.x + a_gridSize.x / 2.f);
			const float y = (cartPos.y + a_gridSize.y / 2.f);
			return
			{
				unsigned(x),
				unsigned(y)
			};
		}

		UVector2 ConvertToGridPosition(const Vector3& a_pos, const UVector2& a_gridSize, bool a_isometric)
		{
			return ConvertToGridPosition({ a_pos.x, a_pos.y}, a_gridSize, a_isometric);
		}

		Vector3 ConvertGridToWorldPosition(const UVector2& a_gridPos, const UVector2& a_gridSize, bool a_isometric)
		{
			Vector3 pos;
			if(a_isometric)
			{
				const Vector2 cartPos = Vector2( float(a_gridPos.x) - a_gridSize.x / 2.f,float(a_gridPos.y) - a_gridSize.y / 2.f ) ;
				const Vector2 isoPos = CartesianToIsometric(cartPos);
				pos.x = isoPos.x;
				pos.y = isoPos.y;
				pos.z = 0.f;
			}
			else
			{
				pos.x = float(a_gridPos.x) - a_gridSize.x / 2.f;
				pos.y = float(a_gridPos.y) - a_gridSize.y / 2.f;
				pos.z = 0.f;
			}

			return pos;
		}


		Vector2 CartesianToIsometric(const Vector2& a_pos)
		{
			//Convert cartesian to Iso.
			return
			{
				((a_pos.x - a_pos.y) / 2.f),
				((a_pos.x + a_pos.y) / 2.f) * 0.5f
			};
		}

		Vector2 IsometricToCartesian(const Vector2& a_isoPos)
		{
			//Convert iso to cartesian.
			return
			{
				(2.f * (2.f * a_isoPos.y + a_isoPos.x)) / 2.f,
				(2.f * (2.f * a_isoPos.y - a_isoPos.x)) / 2.f
			};
		}
	}
}
