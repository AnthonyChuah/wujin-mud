#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <string>

class TcpConnection
{
public:
    static std::unique_ptr<TcpConnection> Make(boost::asio::io_service& ioService, size_t id);

    TcpConnection(boost::asio::io_service& ioService, size_t id);

    boost::asio::ip::tcp::socket& GetSocket()
    {
        return _socket;
    }

    void Start();

    size_t GetId() const
    {
        return _id;
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
    void HandleWrite(const boost::system::error_code& error, size_t bytes);

    size_t _id;
    boost::asio::ip::tcp::socket _socket;
    std::string _message;
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
