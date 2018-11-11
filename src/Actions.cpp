#include "Actions.h"
#include "CommandParser.h"
#include "ConsoleHandler.h"

const std::vector<std::string> admin = {
    "quit",
    "sc", "score",
    "stats",
    "i", "items"
};

const std::vector<std::string> movement = {
    "n", "north",
    "s", "south",
    "e", "east",
    "w", "west",
    "ne", "northeast",
    "nw", "northwest",
    "se", "southeast",
    "sw", "southwest",
    "m", "move",
    "tr", "travel"
};

const std::vector<std::string> action = {
    "quest",
    "r", "rest",
    "st", "stand",
    "buy",
    "sell",
    "creep"
};

const std::vector<std::string> skill = {
    "sk", "skill"
};

const std::vector<std::string> skills = {
};

const std::vector<std::string> spell = {
    "c", "cast",
    "sp", "spell"
};

const std::vector<std::string> spells = {
};

const std::vector<std::string> combataction = {
    "f", "flee"
};

const std::vector<std::string> combatskills = {
    "str", "strike"
};

const std::vector<std::string> combatspells = {
};

// Gateway commands are commands which can be handled with the player
// without affecting the wider "world"
using GatewayHandlerFunction = void (*)(const std::vector<std::string>&, ConsoleHandler*);

std::unordered_map<std::string, GatewayHandlerFunction> gatewayCommandHandler = {
    {"help", &GetHelpFile}
};

bool StringIn(const std::string& str,
              const std::vector<std::string>& list)
{
    for (const std::string& ii : list)
    {
        if (str == ii)
            return true;
    }
    return false;
}

template <typename K, typename V>
bool KeyIn(const K& key, const std::unordered_map<K, V>& map)
{
    for (const auto& pair : map)
    {
        if (pair.first == key)
            return true;
    }
    return false;
}

ActionType GetActionType(const std::string& command, bool inCombat)
{
    if (StringIn(command, admin))
        return ActionType::ADMIN;
    else if (StringIn(command, movement))
        return ActionType::DIRECTION;
    else if (StringIn(command, action))
        return ActionType::ACTIVITY;
    else if (StringIn(command, skill))
    {
        if (inCombat)
            return ActionType::COMBATSKILL;
        return ActionType::SKILL;
    }
    else if (StringIn(command, spell))
    {
        if (inCombat)
            return ActionType::COMBATSPELL;
        return ActionType::SPELL;
    }
    else if (StringIn(command, combataction))
        return ActionType::COMBATACTION;
    return ActionType::INVALID;
}

void GetHelpFile(const std::vector<std::string>& tokens, ConsoleHandler* console)
{
    if (tokens.size() > 1)
    {
        console->WriteToStdOut("Feature not yet implemented: you attempted to get helpfile for "
                              + tokens[1] + "\n");
    }
    else
        console->WriteToStdOut("Feature not yet implemented: you attempted to get the root helpfile\n");
}

bool PreProcessCommand(const std::string& line, ConsoleHandler* console)
{
    const std::vector<std::string> tokens = TokenizeCommand(line);

    if (tokens.empty())
        return false;

    if (KeyIn(tokens[0], gatewayCommandHandler))
    {
        (*gatewayCommandHandler[tokens[0]])(tokens, console);
    }
    return true;
}
