#include <QApplication>
#include <QMessageBox>
#include "ui/booking_view.h"
#include "net/api_client.h"
#include "util/logger.h"
#include "util/app_settings.h"

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);
    application.setApplicationName("DeskPP");
    application.setOrganizationName("DeskPP");

    // Parse command line arguments and load settings
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(application);

    // Initialize logger
    initLogger(settings.isVerboseLogging());
    LOG_INFO("Starting DeskPP client");

    // Get server settings
    std::string serverAddress = settings.getServerAddress();
    int port = settings.getServerPort();
    LOG_INFO("Using server: {}:{}", serverAddress, port);

    // Create API client
    ApiClient apiClient(serverAddress, port);

    if (!apiClient.isConnected()) {
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
        // Attempt auto-login if credentials are available and auto-login is enabled
        if (settings.isAutoLoginEnabled() &&
            !settings.getUsername().empty() &&
            !settings.getPassword().empty()) {
            LOG_INFO("Attempting auto-login with username: {}", settings.getUsername());

            auto user = apiClient.loginUser(settings.getUsername(), settings.getPassword());
            if (user) {
                LOG_INFO("Auto-login successful for user: {}", user->getUsername());
            } else {
                LOG_WARNING("Auto-login failed for user: {}", settings.getUsername());
            }
        }
    }

    // Create and show the main window
    BookingView window(nullptr, apiClient);

    // Restore window size if needed
    if (settings.rememberWindowSize()) {
        window.resize(settings.getWindowSize());
    }

    window.show();

    // Run the application
    int result = application.exec();

    // Save window size before exit if needed
    if (settings.rememberWindowSize()) {
        settings.saveWindowSize(window.width(), window.height());
    }

    return result;
}
