#ifndef ENTITY_H
#define ENTITY_H

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

/**
 * @class Entity
 * @brief Klasa bazowa dla wszystkich encji w systemie.
 *
 * Zapewnia podstawową funkcjonalność dla wszystkich encji taką jak
 * identyfikator oraz metody konwersji do formatu JSON.
 */
class Entity {
public:
    /**
     * @brief Konstruktor domyślny
     */
    Entity() : _id(0) {
    }

    /**
     * @brief Konstruktor z identyfikatorem
     * @param id Identyfikator encji
     */
    explicit Entity(int id) : _id(id) {
    }

    /**
     * @brief Wirtualny destruktor
     */
    virtual ~Entity() = default;

    /**
     * @brief Pobiera identyfikator encji
     * @return Identyfikator encji
     */
    int getId() const { return _id; }

    /**
     * @brief Ustawia identyfikator encji
     * @param id Nowy identyfikator
     */
    void setId(int id) { _id = id; }

    /**
     * @brief Konwertuje obiekt na format JSON
     * @return Reprezentacja JSON obiektu
     */
    virtual json toJson() const = 0;

    /**
     * @brief Konwertuje obiekt na string
     * @return Tekstowa reprezentacja obiektu
     */
    virtual std::string toString() const { return "Entity ID: " + std::to_string(_id); }

protected:
    int _id;
};

#endif
