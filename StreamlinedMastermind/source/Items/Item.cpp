#include "smpch.h"
#include "Items/Item.h"
#include "Utils/ServiceManager.h"
#include "Gameplay/GameObjectManager.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"



items::Item::Item(const glm::vec3 a_pos)
    : m_pos(a_pos)
{
    SetActive(true);
    m_sprite = ce::Sprite();
    m_sprite.SetPosition(glm::vec3(m_pos.x, m_pos.y, m_pos.z));
    m_sprite.SetScale(glm::vec3(0.5f, 0.5f, 1.0f));
    m_sprite.SetColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_tilePosition = glm::vec3(m_pos.x, m_pos.y, 0.0f);
}

void items::Item::OnUpdate(float)
{
    m_sprite.SetPosition(glm::vec3(m_pos.x, m_pos.y, m_pos.z));
}

void items::Item::OnDraw(cr::RenderLayer& a_renderLayer)
{
    m_sprite.Draw(a_renderLayer);
}

void items::Item::OnDestroy()
{
    ce::ServiceManager::GetService<ce::ResourceManager>()->ReleaseResource(m_tex);
}

items::EItemName items::Item::GetItemName() const
{
    return m_itemName;
}

glm::vec3 items::Item::GetPosition() const
{
    return m_pos;
}

void items::Item::SetPosition(const glm::vec3 a_pos)
{
    m_pos = a_pos;
}
