#include <memory>
#include <crow.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "api/controller/booking_controller.h"
#include "api/controller/user_controller.h"
#include "api/controller/admin_controller.h"
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
    // Load settings
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(argc, argv);

    // Initialize logger
    initLogger("DeskPP", settings.isVerboseLogging());
    LOG_INFO("Starting DeskPP server on port {}", settings.getPort());

    try {
        // Create database connection
        auto db = std::make_shared<SQLite::Database>(
            settings.getDatabasePath(),
            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
        );

        // Check if tables need to be created
        bool tableExists = false;
        try {
            SQLite::Statement query(*db, "SELECT name FROM sqlite_master WHERE type='table' AND name='buildings'");
            tableExists = query.executeStep();
        } catch (...) {
            // Ignore exceptions during check
        }

        // Initialize database if needed
        if (!tableExists) {
            LOG_INFO("Creating database schema...");

            db->exec("BEGIN TRANSACTION;");

            // Create tables
            db->exec("CREATE TABLE buildings ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "address TEXT"
                ");");

            db->exec("CREATE TABLE desks ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "name TEXT NOT NULL,"
                "building_id INTEGER NOT NULL,"
                "floor_number INTEGER NOT NULL,"
                "location_x INTEGER DEFAULT 0,"
                "location_y INTEGER DEFAULT 0,"
                "FOREIGN KEY (building_id) REFERENCES buildings(id) ON DELETE CASCADE"
                ");");

            db->exec("CREATE TABLE users ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "username TEXT NOT NULL UNIQUE,"
                "password_hash TEXT NOT NULL,"
                "email TEXT NOT NULL UNIQUE,"
                "full_name TEXT,"
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

            // Add sample data
            db->exec("BEGIN TRANSACTION;");

            // Add buildings
            db->exec("INSERT INTO buildings (name, address) VALUES "
                "('Krakow A', 'Krakowska St. 123'),"
                "('Warsaw B', 'Warszawska St. 456');");

            // Get building IDs
            int krakAId = 0, wawBId = 0; {
                SQLite::Statement query(*db, "SELECT id FROM buildings WHERE name = 'Krakow A'");
                if (query.executeStep()) krakAId = query.getColumn(0).getInt();
            } {
                SQLite::Statement query(*db, "SELECT id FROM buildings WHERE name = 'Warsaw B'");
                if (query.executeStep()) wawBId = query.getColumn(0).getInt();
            }

            // Add desks
            if (krakAId > 0 && wawBId > 0) {
                db->exec("INSERT INTO desks (name, building_id, floor_number, location_x, location_y) VALUES "
                         "('KrakA-01-001', " + std::to_string(krakAId) + ", 1, 1, 1),"
                         "('KrakA-01-002', " + std::to_string(krakAId) + ", 1, 2, 1),"
                         "('KrakA-01-003', " + std::to_string(krakAId) + ", 1, 2, 2),"
                         "('WawB-01-001', " + std::to_string(wawBId) + ", 1, 1, 1),"
                         "('WawB-01-002', " + std::to_string(wawBId) + ", 1, 2, 1);");
            }

            // Add users with simple password hashing
            std::string passwordHash = std::to_string(std::hash<std::string>{}("password"));
            db->exec("INSERT INTO users (username, password_hash, email, full_name) VALUES "
                     "('admin', '" + passwordHash + "', 'admin@example.com', 'Admin User'),"
                     "('user1', '" + passwordHash + "', 'user1@example.com', 'Regular User'),"
                     "('user2', '" + passwordHash + "', 'user2@example.com', 'Another User');");

            db->exec("COMMIT;");
        }

        // Initialize repositories
        UserRepository userRepository(db);
        BuildingRepository buildingRepository(db);
        DeskRepository deskRepository(db);
        BookingRepository bookingRepository(db);

        // Initialize services
        UserService userService(userRepository);
        BookingService bookingService(buildingRepository, deskRepository, bookingRepository);

        // Initialize controllers
        BookingController bookingController(bookingService);
        UserController userController(userService);
        AdminController adminController(bookingService);

        // Initialize Crow server
        crow::SimpleApp app;

        // Register API routes
        registerRoutes(app, bookingController, userController, adminController);

        // Start server
        LOG_INFO("Server listening on port {}", settings.getPort());
        app.port(settings.getPort()).multithreaded().run();
    } catch (const std::exception &e) {
        LOG_ERROR("Error: {}", e.what());
        return 1;
    }

    return 0;
}
