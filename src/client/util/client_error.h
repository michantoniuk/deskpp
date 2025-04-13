// src/client/util/client_error.h
#ifndef CLIENT_ERROR_H
#define CLIENT_ERROR_H

#include <string>

class ClientError {
public:
    enum class ErrorType {
        ConnectionError,
        ServerError,
        ValidationError
    };

    ClientError(ErrorType type, const std::string& message)
        : _type(type), _message(message) {}

    std::string toString() const {
        return _message;
    }

    ErrorType type() const { return _type; }

private:
    ErrorType _type;
    std::string _message;
};

#endif // CLIENT_ERROR_H