#include <iostream>
#include <string>
#include "ui/booking_view.h"
#include <QApplication>
#include <QMessageBox>

#include "net/client_communication.h"
#include "util/logger.h"
#include "util/cmd_args.h"
#include "model/user.h"

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);

    // Parse command line arguments
    ClientArgs args;
    args.parse(argc, argv);

    // Initialize logger (console only)
    initLogger(spdlog::level::info);

    LOG_INFO("Starting DeskPP client");

    // Extract server address and port from serverUrl
    std::string serverAddress = "localhost";
    int port = 8080;

    // Check if a specific port was provided as a command line argument (via --port or -p)
    if (args.port > 0) {
        port = args.port;
        LOG_INFO("Using specified port (from command line): {}", port);
    } else {
        // Only extract from server URL if no specific port was provided
        // The serverUrl is in the format "http://address:port"
        size_t protocolEnd = args.serverUrl.find("://");
        if (protocolEnd != std::string::npos) {
            size_t addressStart = protocolEnd + 3;
            size_t portSeparator = args.serverUrl.find(':', addressStart);

            if (portSeparator != std::string::npos) {
                serverAddress = args.serverUrl.substr(addressStart, portSeparator - addressStart);
                std::string portStr = args.serverUrl.substr(portSeparator + 1);

                // Remove any trailing path
                size_t pathSeparator = portStr.find('/');
                if (pathSeparator != std::string::npos) {
                    portStr = portStr.substr(0, pathSeparator);
                }

                try {
                    port = std::stoi(portStr);
                } catch (...) {
                    LOG_WARNING("Invalid port in server URL, using default port 8080");
                }
            } else {
                serverAddress = args.serverUrl.substr(addressStart);
                // Remove any trailing path
                size_t pathSeparator = serverAddress.find('/');
                if (pathSeparator != std::string::npos) {
                    serverAddress = serverAddress.substr(0, pathSeparator);
                }
            }
        }
    }

    LOG_INFO("Connecting to server: {}:{}", serverAddress, port);

    // Create a single ClientCommunication instance to be used everywhere
    ClientCommunication communication(serverAddress, port);

    if (!communication.isConnected()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Cannot connect to server. The application will run with limited functionality.");
        msgBox.setInformativeText("Do you want to continue?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        int result = msgBox.exec();
        if (result == QMessageBox::No) {
            return 1;
        }
    } else {
        // Try auto-login if username and password are provided
        if (args.autoLogin && !args.username.empty() && !args.password.empty()) {
            LOG_INFO("Attempting auto-login with username: {}", args.username);

            auto user = communication.loginUser(args.username, args.password);
            if (user) {
                LOG_INFO("Auto-login successful for user: {}", user->getUsername());
            } else {
                LOG_WARNING("Auto-login failed for user: {}", args.username);
            }
        }
    }

    // Pass the existing communication instance to BookingView
    BookingView window(nullptr, communication); // Pass in our communication instance with the correct port
    window.show();

    return application.exec();
}
