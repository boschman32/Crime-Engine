#include "cepch.h"
#include "Gameplay/Tile.h"

#include "Core/ResourceManager.h"
#include "Gfx/RenderService.h"
#include "Gfx/Texture.h"
#include "Utils/ServiceManager.h"

namespace ce
{
	Tile::Tile(const Vector3& a_position,
		const UVector2& a_gridPos,
		uint32_t a_tileGid,
		const std::string& a_layer,
		TileTypes a_type,
		const Sprite& a_sprite)
		: m_sprite(a_sprite),
		m_tileGid(a_tileGid),
		m_layer(a_layer),
		m_tileType(a_type),
		m_position(a_position),
		m_gridPosition(a_gridPos)
	{
		// Set tileQuad
		m_sprite.SetPosition(m_position);
	}

	void Tile::OnCreate()
	{
	}

	void Tile::OnUpdate(float)
	{
	}

	void Tile::OnDraw(cr::RenderLayer& a_renderLayer)
	{
		m_sprite.Draw(a_renderLayer);
	}

	void Tile::OnDestroy()
	{

	}
}
