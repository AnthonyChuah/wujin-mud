#pragma once

#include <cstdint>
#include <string>

struct Attributes
{
    uint8_t strength;
    uint8_t dexterity;
    uint8_t constitution;
    uint8_t intelligence;
    uint8_t willpower;
    uint8_t perception;
};

constexpr uint8_t ATTR_SUM = 42;

bool ParseAttributes(const std::string& choice, Attributes& attrib);
