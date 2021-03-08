#pragma once
#include "Gameplay/GameObject.h"
#include "Core/Math.h"

namespace ce
{
	class TileObject : public GameObject
	{
		friend class GameObjectManager;
		friend class LevelMap;
		
	public:
		TileObject() = default;
		~TileObject() override = default;
		
		const Vector3& GetPosition() const { return m_tilePosition; }
		const UVector2& GetGridPosition() const { return m_gridPosition; }
	protected:
		Vector3 m_tilePosition { 0,0,0 };
		
	private:
		UVector2 m_gridPosition { -1,-1 };
	};
}
