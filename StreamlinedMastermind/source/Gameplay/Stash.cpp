#include "smpch.h"
#include "Gameplay/Stash.h"
#include "Core/Logger.h"

//#define show_info 

Stash::Stash()
{
    //const std::unique_ptr<items::MoneyPack> moneyPack = std::make_unique<items::MoneyPack>(200 , 0.1f);
    //const std::unique_ptr<items::MoneyPack> moneyPack2 = std::make_unique<items::MoneyPack>(200, 0.2f);

    //AddItem(EInventory::Character1, *moneyPack);
    //AddItem(EInventory::Character1, *moneyPack);

    //RemoveMoney(EInventory::Character1, 300);

    //CE_APP_WARN(GetMoneyAmount(EInventory::Character1));
    //CE_APP_WARN(GetMoneyWeight(EInventory::Character1));
}

void Stash::AddItem(const EInventory a_inventory, items::Item& a_item)
{
    auto totalMoney = dynamic_cast<items::MoneyPack*>(GetItem(a_inventory, items::EItemName::MoneyPack));

    if (a_item.GetItemName() == items::EItemName::MoneyPack && totalMoney != nullptr)
    {
        const auto addedMoney = dynamic_cast<items::MoneyPack*>(&a_item);

        totalMoney->SetAmount(totalMoney->GetAmount() + addedMoney->GetAmount());
    }
    else
    {
        m_stash[a_inventory].push_back(&a_item);
    }

#ifdef show_info
    CE_APP_INFO("Item: " + GetItemName(a_item.GetItemName()) + " added to inventory: " + GetInventoryName(a_inventory));
#endif
}

items::Item* Stash::GetItem(const EInventory a_inventory, const items::EItemName a_itemName) const
{
    for (const auto& i : m_stash)
    {
        if (i.first == a_inventory)
        {
            for (const auto& j : i.second)
            {
                if (j->GetItemName() == a_itemName)
                {
                    return j;
                }
            }
        }
        else
        {
#ifdef show_info
            CE_APP_WARN("The stash doesn't contain inventory: " + GetInventoryName(a_inventory));
#endif
            return nullptr;
        }
    }
#ifdef show_info
    CE_APP_WARN(GetInventoryName(a_inventory) + " Doesn't contain item: " + GetItemName(a_itemName));
#endif
    return nullptr;
}

void Stash::RemoveItem(const EInventory a_inventory, const items::EItemName a_itemName)
{
    if (GetItem(a_inventory, a_itemName) != nullptr)
    {
        //std::cout << "alright" << std::endl;

        for (auto& i : m_stash)
        {
            if (i.first == a_inventory)
            {
                auto it = i.second.begin();

                for (const auto& j : i.second)
                {
                    if (j->GetItemName() == a_itemName)
                    {  
                        i.second.erase(it);
#ifdef show_info
                        CE_APP_INFO("Item: " + GetItemName(a_itemName) + " removed from inventory: " + GetInventoryName(a_inventory));
#endif
                        break;
                    }

                    ++it;
                }
            }
        }
        
    }
}

//std::vector<Item*>& Stash::GetInventory(const EInventory a_inventory)
//{
//    for (auto& i : m_stash)
//    {
//        if (i.first == a_inventory)
//        {
//            return i.second;
//        } 
//    }
//
//#ifdef show_info
//    CE_APP_WARN("The stash doesn't contain inventory: " + GetInventoryName(a_inventory));
//#endif
//    std::vector<Item*> empty;
//    return empty;
//}

void Stash::EmptyInventory(const EInventory a_inventory)
{
    for (auto& i : m_stash)
    {
        if (i.first == a_inventory)
        {
            i.second.clear();
#ifdef show_info
            CE_APP_INFO(GetInventoryName(a_inventory) + " is emptied");
#endif
        }
    }
}

items::MoneyPack* Stash::GetMoney(const EInventory a_inventory) const
{
    const auto money = GetItem(a_inventory, items::EItemName::MoneyPack);

    if (money != nullptr)
    {
        return dynamic_cast<items::MoneyPack*>(money);
    }

    return nullptr; 
}

void Stash::RemoveMoney(const EInventory a_inventory, const unsigned a_amount) const
{
    auto money = GetMoney(a_inventory);
    if (money != nullptr)
    {
        if (int(money->GetAmount() - a_amount) > 0)
        {
            money->SetAmount(money->GetAmount() - a_amount);
        }
        else
        {
            money->SetAmount(0);
        }

    }
}

unsigned Stash::GetMoneyAmount(const EInventory a_inventory) const
{
    const auto money = GetMoney(a_inventory);

    if (money != nullptr)
    {
        return money->GetAmount();
    }

    return 0;
}

float Stash::GetMoneyWeight(const EInventory a_inventory) const
{
    const auto money = GetMoney(a_inventory);

    if (money != nullptr)
    {
        return money->GetWeight();
    }

    return 0;
}

std::string Stash::GetInventoryName(const EInventory a_inventory)
{
    switch (a_inventory)
    {
    case EInventory::Global:
        return "Global";
    case EInventory::Character1:
        return "Character1";
    case EInventory::Character2:
        return "Character2";
    case EInventory::Character3:
        return "Character3";
    case EInventory::Character4:
        return "Character4";
    default:
        return "(No string)";
    }
}

std::string Stash::GetItemName(const items::EItemName a_itemName)
{
    switch (a_itemName)
    {
    case items::EItemName::MoneyPack:
        return "MoneyPack";
    case items::EItemName::KeyCard:
        return "KeyCard";
    default:
        return "(No string)";
    }
}

void Stash::GetInventoryItemNames(const EInventory a_inventory)
{
    for (auto& i : m_stash)
    {
        if (i.first == a_inventory)
        {
            CE_APP_WARN("inventory: " + GetInventoryName(a_inventory) + " contains the following items:");

            for (auto j : i.second)
            {
                CE_APP_WARN(GetItemName(j->GetItemName()));
            }
        }
    }
}



