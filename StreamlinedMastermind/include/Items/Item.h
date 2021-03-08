#pragma once
#include "Gameplay/TileObject.h"
#include "Gfx/Drawables/Sprite.h"
#include "Gameplay/Map/LevelService.h"

namespace items
{
    enum class EItemName { MoneyPack, KeyCard };

    class Item : public ce::TileObject
    {
    public:
        Item() = default;
        Item(glm::vec3 a_pos);
        ~Item() override = default;

        void OnCreate() override {}
        void OnUpdate(float a_deltaTime) override;
        void OnDraw(cr::RenderLayer& a_renderLayer) override;
        void OnDestroy() override;

        EItemName GetItemName() const;
        glm::vec3 GetPosition() const;

        void SetPosition(glm::vec3 a_pos);

    protected:
        EItemName m_itemName;
        ce::Sprite m_sprite;
        glm::vec3 m_pos;
        std::shared_ptr<ce::Texture> m_tex;
    };
}

