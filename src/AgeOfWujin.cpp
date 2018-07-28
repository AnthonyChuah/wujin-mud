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

    TcpServer server{port};
    server.Run();

    return 0;
}
