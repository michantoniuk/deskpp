#include <QApplication>
#include "ui/booking_view.h"
#include "net/api_client.h"
#include "common/logger.h"
#include "common/app_settings.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("DeskPP");

    // Load settings
    auto &settings = AppSettings::getInstance();

    // Initialize logger
    initLogger("DeskPP", false);

    // Create API client with default settings
    ApiClient apiClient("localhost", 8080);

    // Create and show main window
    BookingView window(nullptr, apiClient);
    window.resize(800, 600);
    window.show();

    return app.exec();
}
