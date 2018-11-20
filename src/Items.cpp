#include "Items.h"

#include <cassert>
#include <cstdio>

uint32_t Loot::GetTotalLootValue() const
{
    constexpr uint32_t valuePerUnit[5] = {5, 10, 20, 40, 100};
    uint32_t sum = 0;
    for (unsigned i = 0; i < 5; ++i)
        sum += loot[i] * valuePerUnit[i];
    printf("Calculated total loot value of {%hu, %hu, %hu, %hu, %hu} loot as %u\n",
           loot[0], loot[1], loot[2], loot[3], loot[4], sum);
    return sum;
}

void Loot::GainLoot(unsigned tier, uint16_t qty)
{
    printf("Gained qty %hu loot of tier %u\n", qty, tier);
    uint16_t lootCounter = 0;
    for (unsigned i = 0; i < 5; ++i)
        lootCounter += loot[i];
    assert(lootCounter <= maxloot);

    if (lootCounter + qty <= maxloot)
    {
        loot[tier] += qty; // Then insert all your loot
        return;
    }

    uint16_t overflowQty = lootCounter + qty - maxloot;
    uint16_t canFitQty = qty - overflowQty;
    loot[tier] += canFitQty;

    // Now insert the overflowQty by evicting less-valuable tiers, if any
    uint16_t canEvict[5] = {0};
    uint16_t overflowCount = overflowQty;
    for (unsigned i = 0; i <= tier; ++i)
    {
        if (overflowCount <= loot[i])
        {
            canEvict[i] = overflowCount;
            break;
        }
        else
        {
            overflowCount -= loot[i];
            canEvict[i] = loot[i];
        }
    } // Possible to do this in a single pass if we like

    // Finally, apply the canEvicts to all loot tiers, and then insert the overflowQty
    for (unsigned i = 0; i <= tier; ++i)
        loot[i] -= canEvict[i];
    loot[tier] += overflowQty;
}

// Returning 0 means impossible to upgrade
uint32_t Loot::UpgradeLootCapacityCost()
{
    uint16_t currentTier = maxloot / 2000 - 1;
    switch (currentTier)
    {
    case 0:
        return 5000;
    case 1:
        return 10000;
    case 2:
        return 20000;
    case 3:
        return 35000;
    case 4:
        return 50000;
    default:
        break;
    }
    return 0;
}
