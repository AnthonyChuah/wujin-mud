#pragma once

#include "CommandBuffer.h"

#include <boost/asio.hpp>
#include <memory>
#include <string>

class Character;

class TcpConnection
{
public:
    static std::unique_ptr<TcpConnection> Make(boost::asio::io_service& ioService, size_t id);

    TcpConnection(boost::asio::io_service& ioService, size_t id);

    boost::asio::ip::tcp::socket& GetSocket()
    {
        return _socket;
    }

    void AttachCharacter(Character* character)
    {
        _character = character;
    }

    void DetachCharacter()
    {
        _character = nullptr;
    }

    void StartWrite();
    void StartRead();

    void Stop();

    std::string& GetOutputBuffer()
    {
        return _message;
    }
    size_t GetId() const
    {
        return _id;
    }
    std::string GetNextCommand();
    Character* GetCharacter()
    {
        return _character;
    }
    bool UserInGame() const
    {
        return _character != nullptr;
    }

    friend bool operator==(const TcpConnection& left, const TcpConnection& right)
    {
        return left._id == right._id;
    }
    friend bool operator!=(const TcpConnection& left, const TcpConnection& right)
    {
        return !(left == right);
    }

private:
    void HandleWrite(const boost::system::error_code& error);
    void HandleRead(const boost::system::error_code& error);
    void CheckTimeout();

    size_t _id;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::deadline_timer _timeout;
    boost::asio::streambuf _inputBuffer;
    std::string _message;
    CommandBuffer _commands;
    Character* _character = nullptr;
    bool _connected = true;
};

namespace std
{

template <>
struct hash<TcpConnection>
{
    size_t operator()(const TcpConnection& key) const
    {
        return key.GetId();
    }
};

} // namespace std
