#include "Score.h"

#include "Progress.h"

void Score::RecalculateMaxes(uint8_t level, uint8_t constitution, uint8_t willpower)
{
    constexpr uint16_t baseHpPerLevel = basehp / Progress::maxLevel;
    uint16_t levelGrantedHp = baseHpPerLevel * level;
    constexpr uint16_t baseMpPerLevel = basemp / Progress::maxLevel;
    uint16_t levelGrantedMp = baseMpPerLevel * level;
    constexpr uint16_t baseStaPerLevel = basesta / Progress::maxLevel;
    uint16_t levelGrantedSta = baseStaPerLevel * level;

    uint16_t conHpBonusFixed = (fixedhpcon * constitution) / 10;
    uint16_t conHpBonusPerLevel = (baseHpPerLevel * constitution) / 10;
    uint16_t totalConVarHpBonus = conHpBonusPerLevel * level;

    uint16_t wilBonusFixed = (fixedmpwil * willpower) / 10;
    uint16_t wilBonusPerLevel = (baseMpPerLevel * willpower) / 10;
    uint16_t totalWilVarBonus = wilBonusPerLevel * level;

    uint16_t conStaBonusFixed = (fixedstacon * constitution) / 10;
    uint16_t conStaBonusPerLevel = (baseStaPerLevel * constitution) / 10;
    uint16_t totalConVarStaBonus = conStaBonusPerLevel * level;

    // Example for level=3, con=10: 500 + 60 + 300 + 60 = 920
    // Example for level=25, con=10: 500 + 500 + 300 + 500 = 1800
    // Example for level=3, con=0: 500 + 60 + 0 + 0 = 560
    // Example for level=25, con=0: 500 + 500 + 0 + 0 = 1000
    maxhp = basehp + levelGrantedHp + conHpBonusFixed + totalConVarHpBonus;
    maxmp = basemp + levelGrantedMp + wilBonusFixed + totalWilVarBonus;
    maxsta = basesta + levelGrantedSta + conStaBonusFixed + totalConVarStaBonus;
}

/**
 * If supplies = 0, no regen. If in combat, regen slow (2% of max per tick), unless buffed
 * If out of combat, regen normal (4% of max per tick).
 * If resting, regen fast (8% of max per tick). Medicine/Discipline can boost to 12%. Both together 16%.
 */
void Score::Regen(uint16_t hpMult, uint16_t mpMult, uint16_t staMult)
{
    uint16_t hpRegen = (maxhp * hpMult) / 50;
    uint16_t mpRegen = (maxmp * mpMult) / 50;
    uint16_t staRegen = (maxsta * staMult) / 50;

    hp += hpRegen;
    if (hp > maxhp) hp = maxhp;
    mp += mpRegen;
    if (mp > maxmp) mp = maxmp;
    sta += staRegen;
    if (sta > maxsta) sta = maxsta;
}
