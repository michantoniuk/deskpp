#include "database.h"

Database::Database(const std::string &dbPath) : db(nullptr) {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Cannot open database: {}", sqlite3_errmsg(db));
        sqlite3_close(db);
        db = nullptr;
    } else {
        LOG_INFO("Connected to database: {}", dbPath);
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
        LOG_INFO("Closed database connection");
    }
}

bool Database::execute(const std::string &sql) {
    if (!db) return false;

    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        LOG_ERROR("SQL Error: {}", errMsg);
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Database::execute(const std::string &sql, const std::map<std::string, std::string> &params) {
    if (!db) return false;

    sqlite3_stmt *stmt = nullptr;
    if (!prepareStatement(sql, params, &stmt)) {
        return false;
    }

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_DONE;
}

bool Database::query(const std::string &sql,
                     std::function<void(std::map<std::string, std::string>)> callback) {
    if (!db) return false;

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Query preparation error: {}", sqlite3_errmsg(db));
        return false;
    }

    int columnCount = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;

        for (int i = 0; i < columnCount; i++) {
            std::string columnName = sqlite3_column_name(stmt, i);

            // Check column type
            int type = sqlite3_column_type(stmt, i);
            std::string value;

            if (type == SQLITE_NULL) {
                value = "";
            } else if (type == SQLITE_INTEGER) {
                value = std::to_string(sqlite3_column_int64(stmt, i));
            } else if (type == SQLITE_FLOAT) {
                value = std::to_string(sqlite3_column_double(stmt, i));
            } else if (type == SQLITE_TEXT) {
                value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
            } else if (type == SQLITE_BLOB) {
                value = "BLOB"; // Simplified approach to BLOB
            }

            row[columnName] = value;
        }

        callback(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::query(const std::string &sql,
                     const std::map<std::string, std::string> &params,
                     std::function<void(std::map<std::string, std::string>)> callback) {
    if (!db) return false;

    sqlite3_stmt *stmt = nullptr;
    if (!prepareStatement(sql, params, &stmt)) {
        return false;
    }

    int columnCount = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> row;

        for (int i = 0; i < columnCount; i++) {
            std::string columnName = sqlite3_column_name(stmt, i);

            // Check column type
            int type = sqlite3_column_type(stmt, i);
            std::string value;

            if (type == SQLITE_NULL) {
                value = "";
            } else if (type == SQLITE_INTEGER) {
                value = std::to_string(sqlite3_column_int64(stmt, i));
            } else if (type == SQLITE_FLOAT) {
                value = std::to_string(sqlite3_column_double(stmt, i));
            } else if (type == SQLITE_TEXT) {
                value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
            } else if (type == SQLITE_BLOB) {
                value = "BLOB"; // Simplified approach to BLOB
            }

            row[columnName] = value;
        }

        callback(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

int64_t Database::lastInsertId() {
    if (!db) return 0;
    return sqlite3_last_insert_rowid(db);
}

bool Database::prepareStatement(const std::string &sql,
                                const std::map<std::string, std::string> &params,
                                sqlite3_stmt **stmt) {
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, stmt, nullptr) != SQLITE_OK) {
        LOG_ERROR("Query preparation error: {}", sqlite3_errmsg(db));
        return false;
    }

    for (const auto &[name, value]: params) {
        int index = sqlite3_bind_parameter_index(*stmt, name.c_str());
        if (index > 0) {
            sqlite3_bind_text(*stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
        }
    }

    return true;
}

bool Database::initializeSchema() {
    if (!db) return false;
    execute("BEGIN TRANSACTION;");

    bool success = execute(
        "CREATE TABLE IF NOT EXISTS buildings ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    name TEXT NOT NULL,"
        "    address TEXT"
        ");"
    );

    if (success) {
        success = execute(
            "CREATE TABLE IF NOT EXISTS desks ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    name TEXT NOT NULL,"
            "    building_id INTEGER NOT NULL,"
            "    floor_number INTEGER NOT NULL,"
            "    FOREIGN KEY (building_id) REFERENCES buildings(id) ON DELETE CASCADE"
            ");"
        );
    }

    if (success) {
        success = execute(
            "CREATE TABLE IF NOT EXISTS users ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    username TEXT NOT NULL UNIQUE,"
            "    password_hash TEXT NOT NULL,"
            "    email TEXT NOT NULL UNIQUE,"
            "    full_name TEXT,"
            "    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
            ");"
        );
    }

    if (success) {
        success = execute(
            "CREATE TABLE IF NOT EXISTS bookings ("
            "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "    desk_id INTEGER NOT NULL,"
            "    user_id INTEGER NOT NULL,"
            "    date TEXT NOT NULL,"
            "    date_to TEXT NOT NULL,"
            "    FOREIGN KEY (desk_id) REFERENCES desks(id) ON DELETE CASCADE,"
            "    FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
            ");"
        );
    }

    if (success) {
        execute("COMMIT;");
        LOG_INFO("Database schema initialization completed successfully");
    } else {
        execute("ROLLBACK;");
        LOG_ERROR("Database schema initialization failed");
    }

    return success;
}

bool Database::seedDemoData() {
    if (!db) return false;

    execute("BEGIN TRANSACTION;");

    // Check if buildings table has data
    bool hasBuildingsData = false;
    query("SELECT COUNT(*) as count FROM buildings", [&hasBuildingsData](std::map<std::string, std::string> row) {
        hasBuildingsData = std::stoi(row["count"]) > 0;
    });

    // Add buildings if table is empty
    bool success = true;
    if (!hasBuildingsData) {
        success = execute(
            "INSERT INTO buildings (name, address) VALUES "
            "('Krakow A', 'Krakowska St. 123'),"
            "('Warsaw B', 'Warszawska St. 456');"
        );

        if (!success) {
            execute("ROLLBACK;");
            LOG_ERROR("Error adding sample buildings");
            return false;
        }
        LOG_INFO("Added sample buildings to database");
    }

    // Check if desks table has data
    bool hasDesksData = false;
    query("SELECT COUNT(*) as count FROM desks", [&hasDesksData](std::map<std::string, std::string> row) {
        hasDesksData = std::stoi(row["count"]) > 0;
    });

    // Add desks if table is empty
    if (!hasDesksData) {
        // Get building IDs from database
        int krakAId = 0, wawBId = 0;
        query("SELECT id FROM buildings WHERE name = 'Krakow A'",
              [&krakAId](std::map<std::string, std::string> row) {
                  krakAId = std::stoi(row["id"]);
              });
        query("SELECT id FROM buildings WHERE name = 'Warsaw B'",
              [&wawBId](std::map<std::string, std::string> row) {
                  wawBId = std::stoi(row["id"]);
              });

        // Add desks with proper building IDs
        success = execute(
            "INSERT INTO desks (name, building_id, floor_number) VALUES "
            "('KrakA-01-001', " + std::to_string(krakAId) + ", 1),"
            "('KrakA-01-002', " + std::to_string(krakAId) + ", 1),"
            "('KrakA-01-003', " + std::to_string(krakAId) + ", 1),"
            "('WawB-01-001', " + std::to_string(wawBId) + ", 1),"
            "('WawB-01-002', " + std::to_string(wawBId) + ", 1);"
        );

        if (!success) {
            execute("ROLLBACK;");
            LOG_ERROR("Error adding sample desks");
            return false;
        }
        LOG_INFO("Added sample desks to database");
    }

    // Check if users table has data
    bool hasUsersData = false;
    query("SELECT COUNT(*) as count FROM users", [&hasUsersData](std::map<std::string, std::string> row) {
        hasUsersData = std::stoi(row["count"]) > 0;
    });

    // Add users if table is empty
    if (!hasUsersData) {
        // Add some demo users - password is "password" hashed with our simple hash function
        success = execute(
            "INSERT INTO users (username, password_hash, email, full_name) VALUES "
            "('admin', '5199103695992879776', 'admin@example.com', 'Admin User'),"
            "('user1', '5199103695992879776', 'user1@example.com', 'Regular User'),"
            "('user2', '5199103695992879776', 'user2@example.com', 'Another User');"
        );

        if (!success) {
            execute("ROLLBACK;");
            LOG_ERROR("Error adding sample users");
            return false;
        }
        LOG_INFO("Added sample users to database");
    }

    // Commit changes if everything succeeded
    if (success) {
        execute("COMMIT;");
        LOG_INFO("Successfully added sample data to database");
    } else {
        execute("ROLLBACK;");
        LOG_ERROR("Error adding sample data");
    }

    return success;
}

bool Database::tableExists(const std::string &tableName) {
    if (!db) return false;

    bool exists = false;
    std::string sql = "SELECT name FROM sqlite_master WHERE type='table' AND name=?";

    sqlite3_stmt *stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, tableName.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        exists = true;
    }

    sqlite3_finalize(stmt);
    return exists;
}
