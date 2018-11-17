#include "Items.h"

uint32_t Loot::GetTotalLootValue() const
{
    constexpr uint32_t valuePerUnit[5] = {5, 10, 20, 40, 100};
    uint32_t sum = 0;
    for (unsigned i = 0; i < 5; ++i)
        sum += loot[i] * valuePerUnit[i];
    printf("Calculated total loot value of {%hu, %hu, %hu, %hu, %hu} loot as %u",
           loot[0], loot[1], loot[2], loot[3], loot[4], sum);
    return sum;
}

// Quite horrible, should add unit test
void Loot::GainLoot(unsigned tier, uint16_t qty)
{
    printf("Gained qty %hu loot of tier %u", qty, tier);
    uint16_t lootCounter = 0;
    for (unsigned i = 0; i < tier; ++i)
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
    uint16_t evictCount = 0;
    uint16_t overflowCount = overflowQty;
    for (unsigned i = 0; i < tier; ++i)
    {
        if (overflowCount <= loot[tier])
            canEvict[tier] = overflowCount;
        else
        {
            overflowCount -= loot[tier];
            canEvict[tier] = loot[tier];
        }
    }

    // Finally, apply the canEvicts to all loot tiers, and then insert the overflowQty
    for (unsigned i = 0; i < tier; ++i)
        loot[i] -= canEvict[i];
    loot[tier] += overflowQty;
}
