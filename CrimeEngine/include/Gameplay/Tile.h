#pragma once
#include "Gameplay/GameObject.h"
#include "Gfx/Drawables/Sprite.h"

namespace cr { class RenderLayer; }

namespace ce
{
	enum class TileTypes
	{
		Void,
		Floor,
		Wall,
		Prop,
	};

	static std::unordered_map<std::string, TileTypes> gs_tileTypeMap =
	{
		{ "Void", TileTypes::Void },
		{ "Floor", TileTypes::Floor },
		{ "Wall", TileTypes::Wall },
		{ "Prop", TileTypes::Prop }
	};

	class Tile final : public GameObject
	{
	public:
		Tile(const Vector3& a_position,
			const UVector2& a_gridPos,
			uint32_t a_tileGid,
			const std::string& a_layer,
			TileTypes a_type,
			const Sprite& a_sprite);
		~Tile() override = default;

		void OnCreate() override;
		void OnUpdate(float a_deltaTime) override;
		void OnDraw(cr::RenderLayer& a_renderLayer) override;
		void OnDestroy() override;

		int32_t GetTileID() const { return m_tileGid; }
		const std::string& GetLayer() const { return m_layer; }
		const Vector3& GetTilePosition() const { return m_position; }
		const UVector2& GetGridPosition() const { return m_gridPosition; }
		const TileTypes& GetTileType() const { return m_tileType; }

		Sprite m_sprite;

	private:
		
		uint32_t m_tileGid{ 0 }; // 0 = nothing/default
		std::string m_layer;
		TileTypes m_tileType{ TileTypes::Void };
		Vector3 m_position{ 0, 0, 0 };
		UVector2 m_gridPosition{ 0, 0 };
	};
}