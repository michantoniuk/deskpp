#include <QApplication>
#include "ui/booking_view.h"
#include "ui/login_dialog.h"
#include "net/api_client.h"
#include "common/logger.h"
#include "common/app_settings.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("DeskPP");

    // Load settings and init logger
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(app);
    initLogger("DeskPP", settings.isVerboseLogging());

    // Create API client
    ApiClient apiClient(settings.getServerAddress(), settings.getServerPort());

    // Create main window
    BookingView window(nullptr, apiClient);
    window.resize(800, 600);

    // Show login dialog first
    LoginDialog loginDialog(apiClient);
    if (loginDialog.exec() == QDialog::Accepted) {
        window.refreshView();
        window.show();
        return app.exec();
    }

    return 0;
}
