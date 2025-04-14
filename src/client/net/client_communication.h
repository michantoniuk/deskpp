#ifndef CLIENT_COMMUNICATION_H
#define CLIENT_COMMUNICATION_H

#include <string>
#include <vector>
#include <optional>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>
#include "../model/user.h"

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

    // User-related methods
    std::optional<User> registerUser(const std::string &username, const std::string &password,
                                     const std::string &email, const std::string &fullName);

    std::optional<User> loginUser(const std::string &username, const std::string &password);

    std::optional<User> getCurrentUser() const;

    void setCurrentUser(const User &user);

    void logoutUser();

    bool isLoggedIn() const;

private:
    json executeRequest(http::verb method,
                        const std::string &endpoint,
                        const json &data = json::object());

    std::string _serverAddress;
    int _port;
    net::io_context _io_context;
    bool _connected;

    // Current logged-in user
    std::optional<User> _currentUser;
};

#endif // CLIENT_COMMUNICATION_H
