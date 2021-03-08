#pragma once

#include "Items/Item.h"

namespace items
{
    class MoneyPack : public Item
    {
    public:
        MoneyPack() = default;
        MoneyPack(glm::vec3 a_pos, unsigned int a_amount, float a_weightMultiplier);
        ~MoneyPack() override = default;

        unsigned int GetAmount() const;
        float GetWeight() const;

        void SetAmount(unsigned int a_amount);

    private:
        unsigned int m_amount;
        float m_weightMultiplier;
    };
}

