#pragma once

#include "Items/Item.h"
#include "Items/MoneyPack.h"

enum class EInventory {Global, Character1, Character2, Character3, Character4};

class Stash
{
public:
    Stash();
    ~Stash() = default;

    void AddItem(EInventory a_inventory, items::Item& a_item);
    items::Item* GetItem(EInventory a_inventory, items::EItemName a_itemName) const;
    void RemoveItem(EInventory a_inventory, items::EItemName a_itemName);
    //std::vector<Item*>& GetInventory(EInventory a_inventory);
    void EmptyInventory(EInventory a_inventory);
    items::MoneyPack* GetMoney(EInventory a_inventory) const;
    void RemoveMoney(EInventory a_inventory, unsigned int a_amount) const;
    unsigned int GetMoneyAmount(EInventory a_inventory) const;
    float GetMoneyWeight(EInventory a_inventory) const;

private:
    static std::string GetInventoryName(EInventory a_inventory);
    static std::string GetItemName(items::EItemName a_itemName);
    void GetInventoryItemNames(EInventory a_inventory);
    std::map<EInventory, std::vector<items::Item*>> m_stash;

    
};
