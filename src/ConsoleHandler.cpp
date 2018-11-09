#include "Actions.h"
#include "CommandBuffer.h"
#include "ConsoleHandler.h"

#include <iostream>

ConsoleHandler::ConsoleHandler(CommandBuffer& buffer)
    : _buffer(buffer), _outstream(std::cout)
{
    _prompt = "<prompt>\n";
    _command.reserve(256);
}

void ConsoleHandler::Run()
{
    bool playing = true;
    while (playing)
    {
        WriteToStdOut(_prompt);
        std::getline(std::cin, _command);

        if (!PreProcessCommand(_command, this))
        {
            if (!_buffer.AddCommand(_command.c_str()))
            {
                WriteToStdOut("Command buffer is full!\n");
            }
        }
    }
}

void ConsoleHandler::WriteToStdOut(const std::string& output)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _outstream << output;
}
