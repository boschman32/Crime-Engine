#include "smpch.h"
#include "Items/MoneyPack.h"
#include "Utils/ServiceManager.h"
#include "Core/ResourceManager.h"


items::MoneyPack::MoneyPack(const glm::vec3 a_pos, const unsigned int a_amount, const float a_weightMultiplier)
    : Item(a_pos)
    , m_amount(a_amount)
    , m_weightMultiplier(a_weightMultiplier)
{
    m_itemName = EItemName::MoneyPack;
    m_tex = ce::ServiceManager::GetService<ce::ResourceManager>()->LoadResource<ce::Texture>("data/textures/MoneyPack.png");
    ce::Vector4 rect = { 0,0,60,56 };
    m_sprite.SetTexture(*m_tex, rect);
}

unsigned items::MoneyPack::GetAmount() const
{
    return m_amount;
}

float items::MoneyPack::GetWeight() const
{
    return float(m_amount) * m_weightMultiplier;
}

void items::MoneyPack::SetAmount(const unsigned a_amount)
{
    m_amount = a_amount;
}





