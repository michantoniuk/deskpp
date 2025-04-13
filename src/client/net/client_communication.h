#ifndef CLIENT_COMMUNICATION_H
#define CLIENT_COMMUNICATION_H

#include <string>
#include <vector>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using json = nlohmann::json;

class ClientCommunication {
public:
    ClientCommunication();

    ClientCommunication(const std::string &serverAddress, int port);

    ~ClientCommunication();

    bool testConnection();

    bool isConnected() const;

    std::vector<json> getDesks(int buildingId = -1);

    bool addBooking(int deskId, int userId, const std::string &dateFrom, const std::string &dateTo);

    bool cancelBooking(int bookingId);

private:
    json executeRequest(http::verb method,
                        const std::string &endpoint,
                        const json &data = json::object());

    std::string _serverAddress;
    int _port;
    net::io_context _io_context;
    bool _connected;
};

#endif // CLIENT_COMMUNICATION_H
