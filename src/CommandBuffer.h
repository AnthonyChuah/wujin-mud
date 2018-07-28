#pragma once

#include <mutex>
#include <string>

class CommandBuffer
{
public:
    bool AddCommand(const char* command);

    std::string GetCommand();

    void Reset()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _head = 0;
        _tail = 0;
        _size = 0;
    }

    size_t GetSize() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _size;
    }

private:
    static constexpr size_t MAX_CMD_LEN = 256;
    static constexpr size_t ARRAY_SIZE = 64;
    static constexpr size_t MAX_NUM_CMDS = ARRAY_SIZE - 1;

    using Command = char[MAX_CMD_LEN];
    Command _buffer[ARRAY_SIZE];
    size_t _head = 0;
    size_t _tail = 0;
    size_t _size = 0;
    mutable std::mutex _mutex;
};
