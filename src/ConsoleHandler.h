#pragma once

#include <istream>
#include <mutex>
#include <ostream>

class CommandBuffer;

// Input stream cin, output stream cout
class ConsoleHandler
{
public:
    ConsoleHandler(CommandBuffer& buffer);

    std::ostream& GetStream()
    {
        return _outstream;
    }

    void Run();
    void WriteToStdOut(const std::string& output);
private:
    CommandBuffer& _buffer;
    std::ostream& _outstream;
    std::string _command;
    std::string _prompt;
    mutable std::mutex _mutex;
};
