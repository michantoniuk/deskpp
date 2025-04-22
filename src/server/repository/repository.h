#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <vector>
#include <optional>

/**
 * @class Repository
 * @brief Interfejs bazowy dla repozytoriów danych.
 *
 * Definiuje wspólne operacje CRUD dla wszystkich repozytoriów.
 *
 * @tparam T Typ encji obsługiwanej przez repozytorium
 */
template<typename T>
class Repository {
public:
    /**
     * @brief Wirtualny destruktor
     */
    virtual ~Repository() = default;

    /**
     * @brief Pobiera wszystkie encje
     * @return Wektor wszystkich encji
     */
    virtual std::vector<T> findAll() = 0;

    /**
     * @brief Pobiera encję po identyfikatorze
     * @param id Identyfikator encji
     * @return Opcjonalny obiekt encji (brak w przypadku nieznalezienia)
     */
    virtual std::optional<T> findById(int id) = 0;

    /**
     * @brief Dodaje nową encję
     * @param entity Obiekt encji do dodania
     * @return Dodana encja (z zaktualizowanym identyfikatorem)
     */
    virtual T add(const T &entity) = 0;

    /**
     * @brief Aktualizuje istniejącą encję
     * @param entity Obiekt encji do aktualizacji
     * @return Czy operacja się powiodła
     */
    virtual bool update(const T &entity) = 0;

    /**
     * @brief Usuwa encję
     * @param id Identyfikator encji do usunięcia
     * @return Czy operacja się powiodła
     */
    virtual bool remove(int id) = 0;
};

#endif
