#include <QApplication>
#include "ui/booking_view.h"
#include "ui/login_dialog.h"
#include "net/api_client.h"
#include "common/logger.h"
#include "common/app_settings.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("DeskPP");

    // Wczytaj ustawienia i inicjalizuj logger
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(app);
    initLogger("DeskPP", settings.isVerboseLogging());

    // Utwórz klienta API
    ApiClient apiClient(settings.getServerAddress(), settings.getServerPort());

    // Utwórz główne okno
    BookingView window(nullptr, apiClient);
    window.resize(800, 600);
    window.show();

    // Załaduj początkowe dane jeśli użytkownik jest zalogowany
    window.refreshView();

    return app.exec();
}
