#include <memory>
#include <crow.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "api/controller/booking_controller.h"
#include "api/controller/user_controller.h"
#include "api/routes.h"
#include "service/user_service.h"
#include "service/booking_service.h"
#include "repository/user_repository.h"
#include "repository/building_repository.h"
#include "repository/desk_repository.h"
#include "repository/booking_repository.h"
#include "common/logger.h"
#include "common/app_settings.h"

int main(int argc, char *argv[]) {
    // Wczytaj ustawienia
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(argc, argv);

    // Inicjalizuj logger
    initLogger("DeskPP", settings.isVerboseLogging());
    LOG_INFO("Uruchamianie serwera DeskPP na porcie {}", settings.getPort());

    try {
        // Utwórz połączenie z bazą danych
        auto db = std::make_shared<SQLite::Database>(
            settings.getDatabasePath(),
            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
        );

        // Sprawdź czy tabele muszą zostać utworzone
        bool tableExists = false;
        try {
            SQLite::Statement query(*db, "SELECT name FROM sqlite_master WHERE type='table' AND name='buildings'");
            tableExists = query.executeStep();
        } catch (...) {
            // Ignoruj wyjątki podczas sprawdzania
        }

        // Inicjalizuj bazę danych jeśli potrzeba
        if (!tableExists) {
            LOG_INFO("Tworzenie schematu bazy danych...");

            db->exec("BEGIN TRANSACTION;");

            // Utwórz tabele
            db->exec("CREATE TABLE buildings ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "address TEXT,"
                "num_floors INTEGER DEFAULT 1"
                ");");

            db->exec("CREATE TABLE desks ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "building_id INTEGER NOT NULL,"
                "floor INTEGER DEFAULT 1,"
                "FOREIGN KEY (building_id) REFERENCES buildings(id) ON DELETE CASCADE"
                ");");

            db->exec("CREATE TABLE users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "username TEXT NOT NULL UNIQUE,"
                "password_hash TEXT NOT NULL,"
                "email TEXT NOT NULL UNIQUE,"
                "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                ");");

            db->exec("CREATE TABLE bookings ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "desk_id INTEGER NOT NULL,"
                "user_id INTEGER NOT NULL,"
                "date TEXT NOT NULL,"
                "date_to TEXT NOT NULL,"
                "FOREIGN KEY (desk_id) REFERENCES desks(id) ON DELETE CASCADE,"
                "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
                ");");

            db->exec("COMMIT;");

            // Dodaj przykładowe dane
            db->exec("BEGIN TRANSACTION;");

            // Dodaj budynki
            db->exec("INSERT INTO buildings (name, address, num_floors) VALUES "
                "('Budynek A', 'ul. Krakowska 123, Warszawa', 2),"
                "('Budynek B', 'ul. Krakowska 125, Warszawa', 3),"
                "('Budynek C', 'ul. Warszawska 45, Kraków', 1),"
                "('Digital Hub', 'ul. Pomorska 12, Gdańsk', 1);");

            // Pobierz ID budynków
            int buildingA = 0, buildingB = 0, buildingC = 0, buildingD = 0; {
                SQLite::Statement query(*db, "SELECT id FROM buildings WHERE name = 'Budynek A'");
                if (query.executeStep()) buildingA = query.getColumn(0).getInt();
            } {
                SQLite::Statement query(*db, "SELECT id FROM buildings WHERE name = 'Budynek B'");
                if (query.executeStep()) buildingB = query.getColumn(0).getInt();
            } {
                SQLite::Statement query(*db, "SELECT id FROM buildings WHERE name = 'Budynek C'");
                if (query.executeStep()) buildingC = query.getColumn(0).getInt();
            } {
                SQLite::Statement query(*db, "SELECT id FROM buildings WHERE name = 'Digital Hub'");
                if (query.executeStep()) buildingD = query.getColumn(0).getInt();
            }

            // Dodaj biurka dla budynku A (2 piętra)
            if (buildingA > 0) {
                // Biurka na 1. piętrze
                for (int i = 1; i <= 15; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "A1-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingA) + ", 1);");
                }
                // Biurka na 2. piętrze
                for (int i = 1; i <= 12; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "A2-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingA) + ", 2);");
                }
            }

            // Dodaj biurka dla budynku B (3 piętra)
            if (buildingB > 0) {
                // Biurka na 1. piętrze
                for (int i = 1; i <= 10; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "B1-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingB) + ", 1);");
                }
                // Biurka na 2. piętrze
                for (int i = 1; i <= 8; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "B2-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingB) + ", 2);");
                }
                // Biurka na 3. piętrze
                for (int i = 1; i <= 6; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "B3-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingB) + ", 3);");
                }
            }

            // Dodaj biurka dla budynku C (1 piętro)
            if (buildingC > 0) {
                for (int i = 1; i <= 20; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "C-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingC) + ", 1);");
                }
            }

            // Dodaj biurka dla budynku D (1 piętro)
            if (buildingD > 0) {
                for (int i = 1; i <= 15; i++) {
                    std::string deskNum = (i < 10) ? "0" + std::to_string(i) : std::to_string(i);
                    std::string deskName = "D-" + deskNum;
                    db->exec("INSERT INTO desks (name, building_id, floor) VALUES ('" +
                             deskName + "', " + std::to_string(buildingD) + ", 1);");
                }
            }

            // Dodaj użytkowników z prostym haszowaniem haseł
            std::string passwordHash = std::to_string(std::hash<std::string>{}("password"));
            db->exec("INSERT INTO users (username, password_hash, email) VALUES "
                     "('admin', '" + passwordHash + "', 'admin@example.com'),"
                     "('jan.kowalski', '" + passwordHash + "', 'jan.kowalski@example.com'),"
                     "('anna.nowak', '" + passwordHash + "', 'anna.nowak@example.com'),"
                     "('user1', '" + passwordHash + "', 'user1@example.com');");

            db->exec("COMMIT;");
        }

        // Inicjalizuj repozytoria
        UserRepository userRepository(db);
        BuildingRepository buildingRepository(db);
        DeskRepository deskRepository(db);
        BookingRepository bookingRepository(db);

        // Inicjalizuj serwisy
        UserService userService(userRepository);
        BookingService bookingService(buildingRepository, deskRepository, bookingRepository);

        // Inicjalizuj kontrolery
        BookingController bookingController(bookingService);
        UserController userController(userService);

        // Inicjalizuj serwer Crow
        crow::SimpleApp app;

        // Zarejestruj trasy API
        registerRoutes(app, bookingController, userController);

        // Uruchom serwer
        LOG_INFO("Serwer nasłuchuje na porcie {}", settings.getPort());
        app.port(settings.getPort()).multithreaded().run();
    } catch (const std::exception &e) {
        LOG_ERROR("Błąd: {}", e.what());
        return 1;
    }

    return 0;
}
