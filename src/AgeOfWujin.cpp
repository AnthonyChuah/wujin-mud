#include "CommandBuffer.h"
#include "ConsoleHandler.h"
#include "Game.h"
#include "TcpServer.h"

#include <cstdio>
#include <thread>

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("Error: launch program with port number arg, e.g. ./Program 8888\n");
        return 1;
    }

    unsigned port = atoi(argv[1]);

    Game game;
    TcpServer server{port, &game};
    std::thread gameThread(&Game::MainLoop, &game);
    server.Run();
    gameThread.join();

    return 0;
}
