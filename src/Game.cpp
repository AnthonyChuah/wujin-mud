#include "CommandBuffer.h"
#include "ConsoleHandler.h"
#include "Game.h"

#include <chrono>
#include <thread>

Game::Game(CommandBuffer& buffer)
    : _buffer(buffer)
    , _console(buffer)
    , _outstream(_console.GetStream())
    , _world("world.dat")
    , _character(_outstream, _world)
{}

void Game::LaunchAllThreads()
{
    std::thread consoleThread(&Game::LaunchConsole, this);
    std::thread gameThread(&Game::Run, this);
    consoleThread.join();
    gameThread.join();
}

void Game::Run()
{
    while (_up)
    {
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now());

        ExecuteGameCycle();

        auto end = std::chrono::time_point_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now());
        std::chrono::microseconds elapsed = end - start;
        std::chrono::microseconds wait{CYCLE_US - elapsed.count()};
        std::this_thread::sleep_for(wait);
    }
}

void Game::LaunchConsole()
{
    _console.Run();
}

void Game::Shutdown()
{
    _up = false;
}

void Game::ExecuteGameCycle()
{
    const std::string cmd = _buffer.GetCommand();
    _character.ExecuteCommand(cmd);
}
