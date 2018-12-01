#include "Attributes.h"
#include "CharacterFileLoader.h"
#include "CommandBuffer.h"
#include "Game.h"
#include "TcpConnection.h"
#include "TcpServer.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <iostream>
#include <thread>

namespace
{

void ToProperCase(std::string& str)
{
    str[0] = toupper(str[0]);
    for (size_t i = 1; i < str.length(); ++i)
        str[i] = tolower(str[i]);
}

}

Game::Game() : _world("world.json", *this)
{
    _disconnects.reserve(2);
}

void Game::MainLoop()
{
    std::cout << "Game begins MainLoop\n";
    while (_up)
    {
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now());

        ExecuteGameCycle();

        auto end = std::chrono::time_point_cast<std::chrono::microseconds>
            (std::chrono::system_clock::now());
        std::chrono::microseconds elapsed = end - start;
        std::chrono::microseconds wait{CYCLE_USEC - elapsed.count()};
        std::this_thread::sleep_for(wait);
    }
}

void Game::Shutdown()
{
    _up = false;
}

void Game::AddNewConnection(TcpConnection* connection)
{
    std::cout << "Game adds a new connection ID " << connection->GetId() << "\n";
    _connections.emplace(std::make_pair(connection->GetId(), connection));
}

/**
 * Game should wait until the next cycle before destroying connections, to allow
 * final words to be sent through the socket via async_write.
 */

void Game::DisconnectById(size_t id)
{
    printf("Game::DisconnectById user connection of ID %lu\n", id);
    _disconnects.push_back(id);
}

void Game::ExecuteGameCycle()
{
    ++_elapsed;
    --_toTick;

    /**
     * When a given user is removed, we must handle destruction in several places:
     * 1. Destroy associated Character here in Game, and the name-to-ID mapping
     * 2. Destroy Character pointers in the corresponding Zone in the World
     * 3. Destroy TcpConnection: this also closes the socket
     */
    for (auto id : _disconnects)
    {
        _connections.erase(id);

        // Not all connections are associated with a logged-in character
        auto iter = _characters.find(id);
        if (iter != _characters.end())
        {
            Character* character = &iter->second;
            Location loc = character->GetLocation();
            _world.CharacterExitZone(loc.major, character);
            std::string nameToDelete = _characters.at(id).GetName();
            _characters.erase(id);
            _nameToId.erase(nameToDelete);
        }

        _server->DestroyConnection(id);
    }
    _disconnects.clear();

    if (_toTick == 0)
    {
        Tick(); // e.g. damage over time, debuff fading
        _toTick = CYCLES_PER_TICK;
    }

    std::string command;
    for (auto& c : _connections)
    {
        TcpConnection* con = c.second;
        if (!con)
        {
            _disconnects.push_back(c.first);
            continue;
        }

        ConnectState state = con->GetConnectState();
        if (state == ConnectState::PLAYING)
        {
            Character* character = con->GetCharacter();
            character->DecrementDelay();
            if (!con->HasNextCommand())
                continue;
            if (character->GetDelay() > 0)
                continue;

            command = con->GetNextCommand(); // pop the command
            command = command.substr(0, command.find_first_of("\n\r"));
            character->ExecuteCommand(command);
        }
        else
        {
            if (!con->HasNextCommand())
                continue;
            command = con->GetNextCommand();
            command = command.substr(0, command.find_first_of("\n\r"));
            if (state == ConnectState::LOGGING_IN)
                HandleCharacterPassword(con, command);
            else if (state == ConnectState::CREATE_CHARACTER)
                HandleCharacterCreation(con, command);
            else if (state == ConnectState::NEW)
                HandleCharacterLogin(con, command);
        }
    }

    for (auto& c : _connections)
        c.second->StartWrite(); // Flush all buffered messages to all connected clients
}

void Game::Tick()
{
    printf("Game tick has happened!\n");
    for (auto& pairit : _characters)
    {
        pairit.second.Regen();
        pairit.second.PeriodicEffects();
    }
}

void Game::HandleCharacterLogin(TcpConnection* connection, const std::string& cmd)
{
    // connection->GetOutputBuffer() = "Feature coming soon! You attempted to LOGIN as ";
    // connection->GetOutputBuffer().append(cmd);
    // connection->StartWrite();

    // cmd contains a string which is the name of the character he wants to login as
    // Validate name (all letters no spaces)
    auto it = std::find_if(cmd.begin(), cmd.end(), [](char c)
                           {
                               return !isalpha(c);
                           });
    if (it != cmd.end())
    {
        std::string& output = connection->GetOutputBuffer();
        output.append("Invalid login name. Only letters allowed, no spaces. Try again.\n");
        return;
    }
    if (cmd.length() > 16)
    {
        connection->GetOutputBuffer().append("Invalid login name. Maximum is 16 characters.\n");
        return;
    }

    std::string name = cmd;
    ToProperCase(name);

    // Then, ask for password and set user state to "trying login"
    connection->SetConnectState(ConnectState::LOGGING_IN);
    connection->SetLogin(name);
    connection->GetOutputBuffer().append("Trying to login as player character ");
    connection->GetOutputBuffer().append(name);
    connection->GetOutputBuffer().append(". Password:");
}

void Game::HandleCharacterPassword(TcpConnection* connection, const std::string& cmd)
{
    const std::string& name = connection->GetLogin();

    auto iter = _nameToId.find(name);
    if (iter != _nameToId.end())
    {
        size_t oldId = iter->second;
        Character existing = _characters.find(oldId)->second;
        auto pairiter = _characters.emplace(std::make_pair(connection->GetId(), std::move(existing)));
        _nameToId.emplace(std::make_pair(name, connection->GetId()));

        _disconnects.push_back(oldId);

        connection->GetOutputBuffer().append("Character is already logged on. Reconnecting.\n");
        connection->AttachCharacter(&pairiter.first->second);
        connection->SetConnectState(ConnectState::PLAYING);
        return;
    }

    CharacterFileLoader loader(name.c_str(), cmd);
    if (loader)
    {
        auto pairiter = _characters.emplace(std::piecewise_construct,
                                            std::forward_as_tuple(connection->GetId()),
                                            std::forward_as_tuple(&connection->GetOutputBuffer(),
                                                                  &_world, loader, connection->GetId()));
        _nameToId.emplace(std::make_pair(name, connection->GetId()));

        std::cout << "Loaded Character whose ID is " << connection->GetId() << "\n";

        connection->GetOutputBuffer().append("Welcome to AgeOfWujin MUD, ");
        connection->GetOutputBuffer().append(name);
        connection->GetOutputBuffer().append("!");
        connection->AttachCharacter(&pairiter.first->second);
        connection->SetConnectState(ConnectState::PLAYING);
    }
    else
    {
        std::cout << "Failed to load character file " << loader.GetName() << "\n";
        std::cout << "Attempt to do character creation!\n";
        connection->GetOutputBuffer().append("Character does not exist. Please create your new character.\n");
        connection->GetOutputBuffer().append("Choose your attributes [strength dexterity constitution intelligence willpower perception].\n");
        connection->GetOutputBuffer().append("Each attribute must be a number from 0 to 10 inclusive, and all 6 attributes must sum to 42.\n");
        connection->GetOutputBuffer().append("Example: '10 7 10 7 3 5' to choose 10 strength, 7 dexterity, 10 constitution, 7 intelligence, 3 willpower, and 5 perception.\n");
        connection->SetNewCharacterPassword(cmd);
        connection->SetConnectState(ConnectState::CREATE_CHARACTER);
    }
}

void Game::HandleCharacterCreation(TcpConnection* connection, const std::string& cmd)
{
    Attributes attr;
    if (ParseAttributes(cmd, attr))
    {
        connection->GetOutputBuffer().append("You have created a new character called ");
        connection->GetOutputBuffer().append(connection->GetLogin());
        connection->GetOutputBuffer().append(".\nWelcome to Fort Victory, where new adventurers begin!\n");

        // This ctor makes a new playerfile
        auto loader = CharacterFileLoader(connection->GetLogin().c_str(),
                                          connection->GetNewCharacterPassword(), attr);

        auto pairiter = _characters.emplace(std::piecewise_construct,
                                            std::forward_as_tuple(connection->GetId()),
                                            std::forward_as_tuple(&connection->GetOutputBuffer(),
                                                                  &_world, loader, connection->GetId()));
        CharacterFileLoader::SaveCharacterToFile(pairiter.first->second);

        connection->AttachCharacter(&pairiter.first->second);
        connection->SetConnectState(ConnectState::PLAYING);
    }
    else
    {
        connection->GetOutputBuffer().append("Invalid chosen attributes. Please reconnect and try again!\n");
        _disconnects.push_back(connection->GetId());
    }
}
