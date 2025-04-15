#include <iostream>
#include <string>
#include <memory>
#include <crow.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include "api/booking_controller.h"
#include "api/user_controller.h"
#include "api/routes.h"
#include "service/user_service.h"
#include "service/booking_service.h"
#include "repository/user_repository.h"
#include "repository/building_repository.h"
#include "repository/desk_repository.h"
#include "repository/booking_repository.h"
#include "util/logger.h"
#include "util/app_settings.h"

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    auto &settings = AppSettings::getInstance();
    settings.parseCommandLine(argc, argv);

    // Initialize logger
    initLogger(settings.isVerboseLogging());
    LOG_INFO("Starting DeskPP server on port {}", settings.getPort());
    LOG_INFO("Using database: {}", settings.getDatabasePath());

    try {
        // Create shared database connection
        auto db = std::make_shared<SQLite::Database>(
            settings.getDatabasePath(),
            SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE
        );

        // Check if schema needs to be initialized
        bool tableExists = false;

        try {
            SQLite::Statement query(*db, "SELECT name FROM sqlite_master WHERE type='table' AND name='buildings'");
            tableExists = query.executeStep();
        } catch (const SQLite::Exception &e) {
            LOG_ERROR("Error checking if tables exist: {}", e.what());
        }

        if (!tableExists) {
            LOG_INFO("Initializing database schema...");

            try {
                db->exec("BEGIN TRANSACTION;");

                // Buildings table
                db->exec("CREATE TABLE buildings ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL,"
                    "address TEXT"
                    ");");

                // Desks table
                db->exec("CREATE TABLE desks ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "name TEXT NOT NULL,"
                    "building_id INTEGER NOT NULL,"
                    "floor_number INTEGER NOT NULL,"
                    "FOREIGN KEY (building_id) REFERENCES buildings(id) ON DELETE CASCADE"
                    ");");

                // Users table
                db->exec("CREATE TABLE users ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "username TEXT NOT NULL UNIQUE,"
                    "password_hash TEXT NOT NULL,"
                    "email TEXT NOT NULL UNIQUE,"
                    "full_name TEXT,"
                    "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
                    ");");

                // Bookings table
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
                LOG_INFO("Database schema initialization completed successfully");

                // Add sample data
                LOG_INFO("Adding sample data...");

                try {
                    db->exec("BEGIN TRANSACTION;");

                    // Add buildings
                    db->exec("INSERT INTO buildings (name, address) VALUES "
                        "('Krakow A', 'Krakowska St. 123'),"
                        "('Warsaw B', 'Warszawska St. 456');");

                    // Get building IDs
                    int krakAId = 0, wawBId = 0; {
                        SQLite::Statement queryKrakA(*db, "SELECT id FROM buildings WHERE name = 'Krakow A'");
                        if (queryKrakA.executeStep()) {
                            krakAId = queryKrakA.getColumn(0).getInt();
                        }
                    } {
                        SQLite::Statement queryWawB(*db, "SELECT id FROM buildings WHERE name = 'Warsaw B'");
                        if (queryWawB.executeStep()) {
                            wawBId = queryWawB.getColumn(0).getInt();
                        }
                    }

                    if (krakAId > 0 && wawBId > 0) {
                        // Add desks
                        db->exec("INSERT INTO desks (name, building_id, floor_number) VALUES "
                                 "('KrakA-01-001', " + std::to_string(krakAId) + ", 1),"
                                 "('KrakA-01-002', " + std::to_string(krakAId) + ", 1),"
                                 "('KrakA-01-003', " + std::to_string(krakAId) + ", 1),"
                                 "('WawB-01-001', " + std::to_string(wawBId) + ", 1),"
                                 "('WawB-01-002', " + std::to_string(wawBId) + ", 1);");
                    }

                    // Add users with hashed password ("password")
                    std::string passwordHash = std::to_string(std::hash<std::string>{}("password"));

                    db->exec("INSERT INTO users (username, password_hash, email, full_name) VALUES "
                             "('admin', '" + passwordHash + "', 'admin@example.com', 'Admin User'),"
                             "('user1', '" + passwordHash + "', 'user1@example.com', 'Regular User'),"
                             "('user2', '" + passwordHash + "', 'user2@example.com', 'Another User');");

                    db->exec("COMMIT;");
                    LOG_INFO("Successfully added sample data to database");
                } catch (const SQLite::Exception &e) {
                    db->exec("ROLLBACK;");
                    LOG_ERROR("Error adding sample data: {}", e.what());
                }
            } catch (const SQLite::Exception &e) {
                db->exec("ROLLBACK;");
                LOG_ERROR("Database schema initialization failed: {}", e.what());
                return 1;
            }
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

        // Initialize Crow server
        crow::SimpleApp app;

        // Register API routes
        registerRoutes(app, bookingController, userController);

        // Log startup info
        LOG_INFO("Server initialized and ready to accept connections");
        LOG_INFO("Server listening on port {}", settings.getPort());

        // Start server
        app.port(settings.getPort()).multithreaded().run();
    } catch (const std::exception &e) {
        LOG_ERROR("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}
