#ifndef SERVICE_H
#define SERVICE_H

#include <nlohmann/json.hpp>
#include <vector>
#include <string>
#include "../repository/repository.h"

using json = nlohmann::json;

/**
 * @class Service
 * @brief Klasa bazowa dla serwisów aplikacji.
 *
 * Zapewnia wspólne funkcje do obsługi operacji na danych
 * i komunikacji między kontrolerami a repozytoriami.
 *
 * @tparam T Typ encji obsługiwanej przez serwis
 */
template<typename T>
class Service {
protected:
    Repository<T> &_repository;

    /**
     * @brief Konstruktor
     * @param repository Referencja do repozytorium
     */
    Service(Repository<T> &repository) : _repository(repository) {
    }

    /**
     * @brief Tworzy odpowiedź o sukcesie
     * @param data Dane do dołączenia do odpowiedzi (opcjonalne)
     * @return Obiekt JSON z informacją o sukcesie
     */
    json successResponse(const json &data = {}) {
        json response = {{"status", "success"}};
        if (!data.empty()) {
            response.merge_patch(data);
        }
        return response;
    }

    /**
     * @brief Tworzy odpowiedź o błędzie
     * @param message Komunikat błędu
     * @return Obiekt JSON z informacją o błędzie
     */
    json errorResponse(const std::string &message) {
        return {{"status", "error"}, {"message", message}};
    }

    /**
     * @brief Konwertuje listę encji na tablicę JSON
     * @param entities Lista encji
     * @param key Klucz dla tablicy w odpowiedzi
     * @return Obiekt JSON z encjami
     */
    json entityListToJson(const std::vector<T> &entities, const std::string &key) {
        json array = json::array();
        for (const auto &entity: entities) {
            array.push_back(entity.toJson());
        }
        return successResponse({{key, array}});
    }
};

#endif
