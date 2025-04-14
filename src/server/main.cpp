#include <iostream>
#include <string>
#include <crow.h>
#include "db/database.h"
#include "api/booking_controller.h"
#include "api/user_controller.h"
#include "api/routing.h"
#include "service/booking_service.h"
#include "service/user_service.h"
#include "repository/booking_repository.h"
#include "repository/desk_repository.h"
#include "repository/building_repository.h"
#include "repository/user_repository.h"
#include "util/logger.h"
#include "util/cmd_args.h"

int main(int argc, char *argv[]) {
    // Parse command-line arguments
    Args args;
    args.parse(argc, argv);

    // Initialize logger
    initLogger(spdlog::level::info);
    LOG_INFO("Starting DeskPP server on port {}", args.port);
    LOG_INFO("Using database: {}", args.dbPath);

    try {
        // Initialize database
        Database db(args.dbPath);

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

        // Initialize repositories
        BookingRepository bookingRepo(db);
        DeskRepository deskRepo(db);
        BuildingRepository buildingRepo(db);
        UserRepository userRepo(db);

        // Initialize services
        BookingService bookingService(bookingRepo, deskRepo, buildingRepo);
        UserService userService(userRepo);

        // Initialize controllers
        BookingController bookingController(bookingService);
        UserController userController(userService);

        // Initialize Crow server
        crow::SimpleApp app;

        // Register API routes
        registerRoutes(app, bookingController, userController);

        // Start server
        LOG_INFO("Server started and listening on port {}", args.port);
        app.port(args.port).multithreaded().run();
    } catch (const std::exception &e) {
        LOG_ERROR("Fatal error: {}", e.what());
        return 1;
    }

    return 0;
}
