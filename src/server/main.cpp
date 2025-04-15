#include <iostream>
#include <string>
#include <crow.h>
#include "db/database.h"
#include "api/controllers.h"
#include "service/services.h"
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
        // Initialize database
        Database db(settings.getDatabasePath());

        // Initialize database schema if needed
        if (!db.tableExists("buildings")) {
            LOG_INFO("Initializing database schema...");
            if (!db.initializeSchema()) {
                LOG_ERROR("Failed to initialize database schema");
                return 1;
            }

            // Add sample data
            LOG_INFO("Adding sample data...");
            if (!db.seedDemoData()) {
                LOG_WARNING("Failed to add sample data");
            }
        }

        // Initialize services
        BookingService bookingService(db);
        UserService userService(db);

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
