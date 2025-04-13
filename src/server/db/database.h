#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "../util/logger.h"

class Database {
public:
    Database(const std::string &dbPath);

    ~Database();

    bool execute(const std::string &sql);

    bool execute(const std::string &sql, const std::map<std::string, std::string> &params);

    bool query(const std::string &sql, std::function<void(std::map<std::string, std::string>)> callback);

    bool query(const std::string &sql,
               const std::map<std::string, std::string> &params,
               std::function<void(std::map<std::string, std::string>)> callback);

    int64_t lastInsertId();

    bool initializeSchema();

    bool seedDemoData();

    bool tableExists(const std::string &tableName);

private:
    sqlite3 *db;

    bool prepareStatement(const std::string &sql,
                          const std::map<std::string, std::string> &params,
                          sqlite3_stmt **stmt);
};

#endif // DATABASE_H
