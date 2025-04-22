#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <crow.h>
#include <nlohmann/json.hpp>
#include <optional>
#include "common/logger.h"

using json = nlohmann::json;

/**
 * @class Controller
 * @brief Klasa bazowa dla kontrolerów API.
 *
 * Zapewnia wspólne funkcje pomocnicze do obsługi zapytań HTTP,
 * przetwarzania odpowiedzi i walidacji danych.
 */
class Controller {
protected:
    /**
     * @brief Parsuje dane JSON z ciała żądania
     * @param body Ciało żądania HTTP
     * @return Obiekt JSON
     */
    json parseJson(const std::string &body) {
        try {
            return json::parse(body);
        } catch (...) {
            return json::object();
        }
    }

    /**
     * @brief Tworzy odpowiedź o błędzie
     * @param statusCode Kod statusu HTTP
     * @param message Komunikat błędu
     * @return Odpowiedź HTTP z informacją o błędzie
     */
    crow::response errorResponse(int statusCode, const std::string &message) {
        json response = {
            {"status", "error"},
            {"message", message}
        };
        return crow::response(statusCode, response.dump());
    }

    /**
     * @brief Tworzy odpowiedź o sukcesie
     * @param data Dane do dołączenia do odpowiedzi
     * @return Odpowiedź HTTP z informacją o sukcesie
     */
    crow::response successResponse(const json &data) {
        return crow::response(200, data.dump());
    }

    /**
     * @brief Waliduje żądanie i sprawdza wymagane pola
     * @param req Żądanie HTTP
     * @param requiredFields Lista wymaganych pól
     * @return Opcjonalny obiekt JSON z danymi żądania lub nullopt w przypadku braku wymaganych pól
     */
    std::optional<json> validateRequest(const crow::request &req, const std::vector<std::string> &requiredFields) {
        json params = parseJson(req.body);
        // Sprawdź wymagane pola
        for (const auto &field: requiredFields) {
            if (!params.contains(field)) {
                return std::nullopt;
            }
        }
        return params;
    }
};

#endif
