#include <QApplication>
#include "ui/booking_view.h"
#include "ui/login_dialog.h"
#include "net/api_client.h"
#include "common/logger.h"
#include "common/app_settings.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("DeskPP");

    // Load settings
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(app);

    // Initialize logger
    initLogger("DeskPP", settings.isVerboseLogging());
    LOG_INFO("Connecting to server: {}:{}", settings.getServerAddress(), settings.getServerPort());

    // Create API client
    ApiClient apiClient(settings.getServerAddress(), settings.getServerPort());

    // Create main window
    BookingView window(nullptr, apiClient);
    window.resize(800, 600);

    // Show login dialog first
    LoginDialog loginDialog(apiClient);
    if (loginDialog.exec() == QDialog::Accepted) {
        // User logged in, show main window
        window.refreshView();
        window.show();
        return app.exec();
    } else {
        // User canceled login
        return 0;
    }
}
