#pragma once
#include "Gameplay/TileObject.h"

namespace ce
{
	class ITileObjectFactory
	{
	public:
		ITileObjectFactory() = default;
		virtual ~ITileObjectFactory() = default;
		
		virtual TileObject* CreateObject(const Vector3& a_spawnPos, const std::string& a_name, const std::string& a_type, std::vector<tmx::Property> a_properties) const = 0;
	};
}
