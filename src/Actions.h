#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * When player inputs commands, we can try to intercept special commands
 * which do not affect the world and give them an instant reply.
 * This is like a "gateway" that sits before the actual game logic,
 * so let's call these "gateway commands".
 *
 * Other commands must pass through to the command buffer, which the game
 * logic then handles in its main loop. Only one command is handled at each
 * loop iteration, throttling the player input rate to the game's "cycle" length.
 */

class ConsoleHandler;

enum class ActionType : uint8_t
{
    INVALID = 0,
    ADMIN = 1,
    DIRECTION = 2,
    ACTIVITY = 3,
    SKILL = 4,
    SPELL = 5,
    COMBATACTION = 6,
    COMBATSKILL = 7,
    COMBATSPELL = 8
};

ActionType GetActionType(const std::string& command, bool inCombat);

bool PreProcessCommand(const std::string& command, ConsoleHandler* console);

void GetHelpFile(const std::vector<std::string>& tokens, ConsoleHandler* console);
