#include "Attributes.h"

#include "CommandParser.h"

bool ParseAttributes(const std::string& choice, Attributes& attrib)
{
    std::vector<std::string> tokens = TokenizeCommand(choice);

    if (tokens.size() != 6)
        return false;

    try
    {
        int strength = std::stoi(tokens[0]);
        int dexterity = std::stoi(tokens[1]);
        int constitution = std::stoi(tokens[2]);
        int intelligence = std::stoi(tokens[3]);
        int willpower = std::stoi(tokens[4]);
        int perception = std::stoi(tokens[5]);

        if (strength < 0 || strength > 10)
            return false;
        if (dexterity < 0 || dexterity > 10)
            return false;
        if (constitution < 0 || constitution > 10)
            return false;
        if (intelligence < 0 || intelligence > 10)
            return false;
        if (willpower < 0 || willpower > 10)
            return false;
        if (perception < 0 || perception > 10)
            return false;

        if (strength + dexterity + constitution + intelligence + willpower + perception != ATTR_SUM)
            return false;

        attrib.strength = strength;
        attrib.dexterity = dexterity;
        attrib.constitution = constitution;
        attrib.intelligence = intelligence;
        attrib.willpower = willpower;
        attrib.perception = perception;
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string Attributes::PrettyPrint() const
{
    std::string output("[");
    output.append(std::to_string(strength));
    output.append("str ");
    output.append(std::to_string(dexterity));
    output.append("dex ");
    output.append(std::to_string(constitution));
    output.append("con ");
    output.append(std::to_string(intelligence));
    output.append("int ");
    output.append(std::to_string(willpower));
    output.append("wil ");
    output.append(std::to_string(perception));
    output.append("per]");
    return output;
}
