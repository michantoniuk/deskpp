#include <crow.h>
#include "logger.h"
#include "kontroler_serwera.h"
#include <iostream>
#include <csignal>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

// Funkcja generująca nazwę pliku logów z aktualną datą i czasem
std::string generujNazwePlikuLogow() {
    auto now = std::chrono::system_clock::now();
    auto timePoint = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << "biurkopp_server_" << std::put_time(std::localtime(&timePoint), "%Y%m%d_%H%M%S") << ".log";
    return ss.str();
}

int main(int argc, char* argv[]) {
    // Inicjalizacja loggera
    std::string nazwaPliku = generujNazwePlikuLogow();
    initLogger(nazwaPliku);

    LOG_INFO("========== Serwer BiurkoPP REST API - Start ==========");
    LOG_INFO("Logi będą zapisywane do pliku: {}", nazwaPliku);

    // Ustawienie portu
    int port = 8080;
    if (argc > 1) {
        try {
            port = std::stoi(argv[1]);
            LOG_INFO("Użyto portu z argumentów wiersza poleceń: {}", port);
        } catch (const std::exception& e) {
            LOG_ERROR("Błąd: Nieprawidłowy format portu. Używanie domyślnego portu 8080.");
            std::cerr << "Błąd: Nieprawidłowy format portu. Używanie domyślnego portu 8080." << std::endl;
        }
    } else {
        LOG_INFO("Używanie domyślnego portu: {}", port);
    }

    try {
        // Inicjalizacja kontrolera biznesowego
        KontrolerSerwera kontroler;

        // Utworzenie aplikacji Crow
        crow::SimpleApp app;

        // Konfiguracja loggera Crow
        crow::LogLevel crowLogLevel = crow::LogLevel::INFO;
        app.loglevel(crowLogLevel);

        // Endpoint 1: GET /api/biurka - pobranie listy biurek
        CROW_ROUTE(app, "/api/biurka")
        .methods(crow::HTTPMethod::GET)
        ([&kontroler](const crow::request& req) {
            LOG_INFO("Obsługa zapytania GET /api/biurka");

            // Parsowanie parametrów zapytania
            json params = json::object();

            // Pobranie parametru idBudynku, jeśli został podany
            auto idBudynkuParam = req.url_params.get("idBudynku");
            if (idBudynkuParam) {
                try {
                    params["idBudynku"] = std::stoi(idBudynkuParam);
                    LOG_DEBUG("Parametr idBudynku = {}", params["idBudynku"].get<int>());
                } catch (const std::exception& e) {
                    LOG_WARNING("Nieprawidłowy format idBudynku: {}", e.what());
                }
            }

            // Wywołanie kontrolera
            json wynik = kontroler.pobierzBiurka(params);

            // Przygotowanie odpowiedzi
            crow::response response(wynik.dump());
            response.set_header("Content-Type", "application/json");
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        });

        // Endpoint 2: GET /api/rezerwacje - pobranie rezerwacji
        CROW_ROUTE(app, "/api/rezerwacje")
        .methods(crow::HTTPMethod::GET)
        ([&kontroler](const crow::request& req) {
            LOG_INFO("Obsługa zapytania GET /api/rezerwacje");

            // Parsowanie parametrów zapytania
            json params = json::object();

            // Pobranie parametrów
            auto idBiurkaParam = req.url_params.get("idBiurka");
            auto dataParam = req.url_params.get("data");

            if (idBiurkaParam) {
                try {
                    params["idBiurka"] = std::stoi(idBiurkaParam);
                } catch (...) {
                    LOG_WARNING("Nieprawidłowy format idBiurka");
                }
            }

            if (dataParam) {
                params["data"] = std::string(dataParam);
            }

            LOG_DEBUG("Parametry zapytania o rezerwacje: {}", params.dump());

            // Wywołanie kontrolera
            json wynik = kontroler.pobierzRezerwacje(params);

            // Przygotowanie odpowiedzi
            crow::response response(wynik.dump());
            response.set_header("Content-Type", "application/json");
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        });

        // Endpoint 3: POST /api/rezerwacje - dodanie rezerwacji
        CROW_ROUTE(app, "/api/rezerwacje")
        .methods(crow::HTTPMethod::POST)
        ([&kontroler](const crow::request& req) {
            LOG_INFO("Obsługa zapytania POST /api/rezerwacje");

            // Parsowanie ciała żądania
            json params;
            try {
                params = json::parse(req.body);
                LOG_DEBUG("Parametry rezerwacji: {}", params.dump());
            } catch (const std::exception& e) {
                LOG_WARNING("Błąd parsowania JSON: {}", e.what());
                crow::response res(400);
                res.set_header("Content-Type", "application/json");
                res.body = json{{"status", "error"}, {"message", "Nieprawidłowy format JSON"}}.dump();
                return res;
            }

            // Wywołanie kontrolera
            json wynik = kontroler.dodajRezerwacje(params);

            // Przygotowanie odpowiedzi
            crow::response response(wynik.dump());
            response.set_header("Content-Type", "application/json");
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        });

        // Endpoint 4: DELETE /api/rezerwacje/:id - anulowanie rezerwacji
        CROW_ROUTE(app, "/api/rezerwacje/<int>")
        .methods(crow::HTTPMethod::DELETE)
        ([&kontroler](int idRezerwacji) {
            LOG_INFO("Obsługa zapytania DELETE /api/rezerwacje/{}", idRezerwacji);

            // Przygotowanie parametrów
            json params = {{"idRezerwacji", idRezerwacji}};
            LOG_DEBUG("ID rezerwacji do anulowania: {}", idRezerwacji);

            // Wywołanie kontrolera
            json wynik = kontroler.anulujRezerwacje(params);

            // Przygotowanie odpowiedzi
            crow::response response(wynik.dump());
            response.set_header("Content-Type", "application/json");
            response.set_header("Access-Control-Allow-Origin", "*");
            return response;
        });

        // Obsługa CORS dla preflight requests
        CROW_ROUTE(app, "/api/<path>")
        .methods(crow::HTTPMethod::OPTIONS)
        ([](const crow::request&, std::string) {
            crow::response res;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            return res;
        });

        // Wyświetlenie informacji o uruchomieniu serwera
        std::cout << "Serwer BiurkoPP REST uruchomiony na porcie " << port << ". Naciśnij Ctrl+C, aby zakończyć." << std::endl;
        std::cout << "Logi są zapisywane do pliku: " << nazwaPliku << std::endl;
        std::cout << "Dostępne endpointy:" << std::endl;
        std::cout << "  GET    /api/biurka?idBudynku=<id>   - pobieranie listy biurek" << std::endl;
        std::cout << "  GET    /api/rezerwacje?idBiurka=<id>&data=<yyyy-mm-dd> - pobieranie rezerwacji" << std::endl;
        std::cout << "  POST   /api/rezerwacje              - dodawanie rezerwacji" << std::endl;
        std::cout << "  DELETE /api/rezerwacje/<id>         - anulowanie rezerwacji" << std::endl;

        // Uruchomienie serwera
        LOG_INFO("Uruchamianie serwera Crow na porcie {}", port);
        app.port(port).multithreaded().run();

    } catch (const std::exception& e) {
        LOG_ERROR("Krytyczny błąd podczas uruchamiania serwera: {}", e.what());
        std::cerr << "Błąd podczas uruchamiania serwera: " << e.what() << std::endl;
        return 1;
    }

    LOG_INFO("========== Serwer BiurkoPP REST API - Zakończenie ==========");
    return 0;
}