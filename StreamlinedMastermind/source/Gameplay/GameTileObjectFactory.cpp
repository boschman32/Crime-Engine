#include "smpch.h"
#include "Gameplay/GameTileObjectFactory.h"
#include "Gameplay/GameObjectManager.h"
#include "Gameplay/Guard.h"
#include "Gameplay/Character.h"
#include "Core/Logger.h"
#include "Items/MoneyPack.h"

GameTileObjectFactory::GameTileObjectFactory(pf::Graph* a_graph)
    : m_graph(a_graph)
{
}

ce::TileObject* GameTileObjectFactory::CreateObject(const ce::Vector3& a_spawnPos, const std::string& a_name,
    const std::string& a_type, std::vector<tmx::Property> a_properties) const
{
    if (a_type == "Character")
    {
        float speed;
        glm::vec2 size = { 1.0f, 1.0f };
        glm::vec4 color;

        for (const auto& i : a_properties)
        {
            if (i.getName() == "Color")
            {
                color = ConvertColourToVec4(i.getColourValue());
            }
            if (i.getName() == "Speed")
            {
                speed = i.getFloatValue();
            }
        }

        return ce::ServiceManager::GetService<ce::GameObjectManager>()->CreateGameObject<Character>(a_name, m_graph, speed, a_spawnPos, size, color);
    }
    if (a_type == "Guard")
    {
        float speed;
        glm::vec2 size = { 1.0f, 1.0f };
        glm::vec4 color;

        for (const auto& i : a_properties)
        {
            if (i.getName() == "Color")
            {
                color = ConvertColourToVec4(i.getColourValue());
            }
            if (i.getName() == "Speed")
            {
                speed = i.getFloatValue();
            }
        }
        return ce::ServiceManager::GetService<ce::GameObjectManager>()->CreateGameObject<Guard>(a_name, m_graph, speed, a_spawnPos, size, color);
    }
    if (a_type == "MoneyPack")
    {
        int amount;
        float weightMultiplier = 0.1f;

        for (const auto& i : a_properties)
        {
            if (i.getName() == "Amount")
            {
                amount = i.getIntValue();
            }
        }

        return ce::ServiceManager::GetService<ce::GameObjectManager>()->CreateGameObject<items::MoneyPack>(a_name, a_spawnPos, amount, weightMultiplier);
    }
   
    CE_APP_WARN(a_type + " isn't set!");
    return nullptr;
}

glm::vec4 GameTileObjectFactory::ConvertColourToVec4(const tmx::Colour a_color)
{
    const float x = float(a_color.r) / 255.0f;
    const float y = float(a_color.g) / 255.0f;
    const float z = float(a_color.b) / 255.0f;
    const float w = float(a_color.a) / 255.0f;

    return { x, y, z, w };
}

