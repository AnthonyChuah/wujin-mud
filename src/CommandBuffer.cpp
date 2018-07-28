#include "CommandBuffer.h"

#include <cstring>

bool CommandBuffer::AddCommand(const char* command)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_size == MAX_NUM_CMDS)
        return false;

    constexpr size_t LAST_INDEX = MAX_CMD_LEN - 1;
    strncpy(_buffer[_tail], command, LAST_INDEX);
    _buffer[_tail][LAST_INDEX - 1] = '\0';
    _tail = (_tail + 1) % ARRAY_SIZE;
    ++_size;
    return true;
}

std::string CommandBuffer::GetCommand()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_size == 0)
        return nullptr;

    std::string output{_buffer[_head]};
    _head = (_head + 1) % ARRAY_SIZE;
    --_size;
    return output;
}
