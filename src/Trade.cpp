#include "Trade.h"

#include "Character.h"

#include <cstdio>

void Trade::BuySupplies(Character& character, uint16_t qty)
{
    constexpr uint32_t suppliesCostPerQty = 2;
    printf("Trade::BuySupplies Character wants to buy %u supplies, and currently has %u/%u, has %u money\n",
           qty, character._items.supplies, character._items.maxsupplies, character._items.money);

    if (character._items.supplies + qty > character._items.maxsupplies)
    {
        qty = character._items.maxsupplies - character._items.supplies;
        character._output->append("You attempted to buy more than you can hold. Reduced buy qty to ");
        character._output->append(std::to_string(qty));
        character._output->append(".\n");
    }

    uint32_t cost = suppliesCostPerQty * qty;
    if (cost > character._items.money)
    {
        character._output->append("You cannot afford to buy these supplies. You need ");
        character._output->append(std::to_string(cost));
        character._output->append(" money to do it.\n");
        return; // Make no purchase.
    }

    character._items.money -= cost;
    character._items.supplies += qty;
    character._output->append("You buy the supplies by spending ");
    character._output->append(std::to_string(cost));
    character._output->append(" money.\n");
}

void Trade::BuyAmmo(Character& character, uint16_t qty)
{
    constexpr uint32_t ammoCostPerQty = 2;
    printf("Trade::BuyAmmo Character wants to buy %u ammo, and currently has %u/%u, has %u money\n",
           qty, character._items.ammo, character._items.maxammo, character._items.money);

    if (character._items.ammo + qty > character._items.maxammo)
    {
        qty = character._items.maxammo - character._items.ammo;
        character._output->append("You attempted to buy more than you can hold. Reduced buy qty to ");
        character._output->append(std::to_string(qty));
        character._output->append(".\n");
    }

    uint32_t cost = ammoCostPerQty * qty;
    if (cost > character._items.money)
    {
        character._output->append("You cannot afford to buy these ammo. You need ");
        character._output->append(std::to_string(cost));
        character._output->append(" money to do it.\n");
        return; // Make no purchase.
    }

    character._items.money -= cost;
    character._items.ammo += qty;
    character._output->append("You buy the ammo by spending ");
    character._output->append(std::to_string(cost));
    character._output->append(" money.\n");
}
