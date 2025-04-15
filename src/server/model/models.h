#ifndef SERVER_MODEL_MODELS_H
#define SERVER_MODEL_MODELS_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * User model for the server application
 */
class User {
public:
    User() = default;
    User(int id, const std::string& username, const std::string& passwordHash,
         const std::string& email, const std::string& fullName);
         
    // JSON conversion
    json toJson() const;
    
    // Getters and setters
    int getId() const { return _id; }
    const std::string& getUsername() const { return _username; }
    const std::string& getPasswordHash() const { return _passwordHash; }
    const std::string& getEmail() const { return _email; }
    const std::string& getFullName() const { return _fullName; }
    
    void setId(int id) { _id = id; }
    void setUsername(const std::string& username) { _username = username; }
    void setPasswordHash(const std::string& passwordHash) { _passwordHash = passwordHash; }
    void setEmail(const std::string& email) { _email = email; }
    void setFullName(const std::string& fullName) { _fullName = fullName; }
    
private:
    int _id = 0;
    std::string _username;
    std::string _passwordHash;  // Store hashed password, not plaintext
    std::string _email;
    std::string _fullName;
};

/**
 * Building model for the server application
 */
class Building {
public:
    Building() = default;
    Building(int id, const std::string& name, const std::string& address);
    
    // JSON conversion
    json toJson() const;
    
    // Getters and setters
    int getId() const { return _id; }
    const std::string& getName() const { return _name; }
    const std::string& getAddress() const { return _address; }
    
    void setId(int id) { _id = id; }
    void setName(const std::string& name) { _name = name; }
    void setAddress(const std::string& address) { _address = address; }
    
private:
    int _id = 0;
    std::string _name;
    std::string _address;
};

/**
 * Desk model for the server application
 */
class Desk {
public:
    Desk() = default;
    Desk(int id, const std::string& deskId, int buildingId, int floorNumber);
    
    // JSON conversion
    json toJson() const;
    
    // Getters and setters
    int getId() const { return _id; }
    const std::string& getDeskId() const { return _deskId; }
    int getBuildingId() const { return _buildingId; }
    int getFloorNumber() const { return _floorNumber; }
    
    void setId(int id) { _id = id; }
    void setDeskId(const std::string& deskId) { _deskId = deskId; }
    void setBuildingId(int buildingId) { _buildingId = buildingId; }
    void setFloorNumber(int floorNumber) { _floorNumber = floorNumber; }
    
private:
    int _id = 0;
    std::string _deskId;
    int _buildingId = 0;
    int _floorNumber = 0;
};

/**
 * Booking model for the server application
 */
class Booking {
public:
    Booking() = default;
    Booking(int id, int deskId, int userId, const std::string& dateFrom, const std::string& dateTo);
    
    // Date operations
    bool containsDate(const std::string& date) const;
    bool overlapsWithPeriod(const std::string& dateFrom, const std::string& dateTo) const;
    
    // JSON conversion
    json toJson() const;
    static Booking fromJson(const json& j);
    
    // Getters and setters
    int getId() const { return _id; }
    int getDeskId() const { return _deskId; }
    int getUserId() const { return _userId; }
    const std::string& getDateFrom() const { return _dateFrom; }
    const std::string& getDateTo() const { return _dateTo; }
    
    void setId(int id) { _id = id; }
    void setDeskId(int deskId) { _deskId = deskId; }
    void setUserId(int userId) { _userId = userId; }
    void setDateFrom(const std::string& dateFrom) { _dateFrom = dateFrom; }
    void setDateTo(const std::string& dateTo) { _dateTo = dateTo; }
    
private:
    int _id = 0;
    int _deskId = 0;
    int _userId = 0;
    std::string _dateFrom;  // Format: "yyyy-MM-dd"
    std::string _dateTo;    // Format: "yyyy-MM-dd"
    
    // Helper methods for date comparison
    bool isDateBefore(const std::string& date1, const std::string& date2) const;
    bool isDateAfter(const std::string& date1, const std::string& date2) const;
    bool isDateEqual(const std::string& date1, const std::string& date2) const;
};

#endif // SERVER_MODEL_MODELS_H