#include "Equipment.h"

void EquipmentSet::CalculateBonuses()
{
    uint32_t weight = 0;
    uint32_t damage = 0;
    uint32_t deflect = 0;
    uint32_t reduction = 0;
    uint32_t speed = _mount.second.speed * _mount.first;
    uint8_t reach = _mainhand.second.reach * _mainhand.first >
        _offhand.second.reach * _offhand.first ?
        _mainhand.second.reach * _mainhand.first :
        _offhand.second.reach * _offhand.first;

    damage += _mainhand.second.damage * _mainhand.first;
    weight += _mainhand.second.weight * _mainhand.first;
    damage += _offhand.second.damage * _offhand.first;
    weight += _offhand.second.weight * _offhand.first;

    deflect += _head.second.deflect * _head.first;
    weight += _head.second.weight * _head.first;
    reduction += _head.second.reduction * _head.first;
    deflect += _neck.second.deflect * _neck.first;
    weight += _neck.second.weight * _neck.first;
    reduction += _neck.second.reduction * _neck.first;
    deflect += _torso.second.deflect * _torso.first;
    weight += _torso.second.weight * _torso.first;
    reduction += _torso.second.reduction * _torso.first;
    deflect += _arms.second.deflect * _arms.first;
    weight += _arms.second.weight * _arms.first;
    reduction += _arms.second.reduction * _arms.first;
    deflect += _hands.second.deflect * _hands.first;
    weight += _hands.second.weight * _hands.first;
    reduction += _hands.second.reduction * _hands.first;
    deflect += _waist.second.deflect * _waist.first;
    weight += _waist.second.weight * _waist.first;
    reduction += _waist.second.reduction * _waist.first;
    deflect += _legs.second.deflect * _legs.first;
    weight += _legs.second.weight * _legs.first;
    reduction += _legs.second.reduction * _legs.first;
    deflect += _feet.second.deflect * _feet.first;
    weight += _feet.second.weight * _feet.first;
    reduction += _feet.second.reduction * _feet.first;

    _bonuses = EquipmentBonuses{damage, deflect, reduction, speed, weight, reach};
}
