#ifndef BUILDING_H
#define BUILDING_H

#include "entity.h"
#include <vector>

/**
 * @class Building
 * @brief Klasa reprezentująca budynek w systemie.
 *
 * Przechowuje informacje o budynku, takie jak nazwa, adres i liczba pięter.
 */
class Building : public Entity {
public:
    /**
     * @brief Konstruktor domyślny
     */
    Building() = default;

    /**
     * @brief Konstruktor z parametrami
     * @param id Identyfikator budynku
     * @param name Nazwa budynku
     * @param address Adres budynku
     * @param numFloors Liczba pięter (domyślnie 1)
     */
    Building(int id, const std::string &name, const std::string &address, int numFloors = 1);

    /**
     * @brief Konwertuje obiekt na format JSON
     * @return Reprezentacja JSON obiektu
     */
    json toJson() const override;

    /**
     * @brief Konwertuje obiekt na string
     * @return Tekstowa reprezentacja obiektu
     */
    std::string toString() const override;

    /**
     * @brief Pobiera nazwę budynku
     * @return Nazwa budynku
     */
    const std::string &getName() const { return _name; }

    /**
     * @brief Pobiera adres budynku
     * @return Adres budynku
     */
    const std::string &getAddress() const { return _address; }

    /**
     * @brief Pobiera liczbę pięter budynku
     * @return Liczba pięter
     */
    int getNumFloors() const { return _numFloors; }

    /**
     * @brief Pobiera listę numerów pięter
     * @return Wektor z numerami pięter
     */
    std::vector<int> getFloors() const;

    /**
     * @brief Ustawia nazwę budynku
     * @param name Nowa nazwa budynku
     */
    void setName(const std::string &name) { _name = name; }

    /**
     * @brief Ustawia adres budynku
     * @param address Nowy adres budynku
     */
    void setAddress(const std::string &address) { _address = address; }

    /**
     * @brief Ustawia liczbę pięter budynku
     * @param numFloors Nowa liczba pięter
     */
    void setNumFloors(int numFloors) { _numFloors = numFloors; }

private:
    std::string _name;
    std::string _address;
    int _numFloors = 1;
};
#endif
