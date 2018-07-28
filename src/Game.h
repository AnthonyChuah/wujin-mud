#pragma once

#include "Character.h"
#include "World.h"

class CommandBuffer;
class ConsoleHandler;

/**
 * The smallest unit of time in the game is the Cycle (50 milliseconds)
 * Events that require interaction with the world / other players must happen
 * at a Cycle.
 *
 * Validation or helpfiles or prompts can happen at the user-input-handler level,
 * so they may happen off-Cycle.
 */
class Game
{
public:
    static constexpr size_t CYCLE_US = 50000; // microseconds
    static constexpr size_t CYCLES_PER_ROUND = 10;
    static constexpr size_t ROUNDS_PER_TICK = 60;

    Game(CommandBuffer& buffer);

    void LaunchAllThreads();
    void Run();
    void LaunchConsole();
    void Shutdown();
private:
    void ExecuteGameCycle();

    // Eventually decouple into Player-oriented output streams and command buffers
    CommandBuffer& _buffer;
    ConsoleHandler _console;
    std::ostream& _outstream;
    World _world;
    Character _character;
    bool _up = true;
};
