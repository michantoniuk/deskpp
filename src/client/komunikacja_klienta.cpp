#include "komunikacja_klienta.h"
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

using tcp = boost::asio::ip::tcp;

KomunikacjaKlienta::KomunikacjaKlienta(const std::string& adresSerwera, int port)
    : adresSerwera(adresSerwera), port(port), connected(false) {
    std::cout << "Inicjalizacja komunikacji z serwerem REST: " << adresSerwera << ":" << port << std::endl;
    // Próba początkowego połączenia, aby sprawdzić dostępność serwera
    testConnection();
}

KomunikacjaKlienta::~KomunikacjaKlienta() {
    // Nic do sprzątania
}

bool KomunikacjaKlienta::testConnection() {
    try {
        // Konfiguracja połączenia
        tcp::resolver resolver(io_context);
        beast::tcp_stream stream(io_context);

        // Próba połączenia
        auto const results = resolver.resolve(adresSerwera, std::to_string(port));

        // Ustawienie timeoutu na 5 sekund
        stream.expires_after(std::chrono::seconds(5));

        // Próba połączenia
        stream.connect(results);

        // Jeśli udało się połączyć, zamknij połączenie
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // Ignorowanie błędu związanego z zamknięciem przez serwer
        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ec};
        }

        connected = true;
        std::cout << "Połączenie z serwerem nawiązane pomyślnie" << std::endl;
        return true;
    } catch (const std::exception& e) {
        connected = false;
        std::cerr << "Błąd połączenia z serwerem: " << e.what() << std::endl;
        return false;
    }
}

json KomunikacjaKlienta::wykonajZapytanie(http::verb metoda, const std::string& endpoint, const json& dane) {
    if (!connected && !testConnection()) {
        return {{"status", "error"}, {"message", "Brak połączenia z serwerem"}};
    }

    try {
        // Resetowanie io_context
        io_context.restart();

        // Konfiguracja połączenia
        tcp::resolver resolver(io_context);
        beast::tcp_stream stream(io_context);

        // Nawiązanie połączenia
        auto const results = resolver.resolve(adresSerwera, std::to_string(port));

        // Ustawienie timeoutu na 5 sekund dla wszystkich operacji
        stream.expires_after(std::chrono::seconds(5));

        stream.connect(results);

        // Przygotowanie zapytania HTTP
        http::request<http::string_body> req{metoda, endpoint, 11};
        req.set(http::field::host, adresSerwera);
        req.set(http::field::user_agent, "BiurkoPP-Klient");

        // Dla zapytań POST ustawiamy treść i nagłówki
        if (metoda == http::verb::post) {
            req.set(http::field::content_type, "application/json");
            std::string body = dane.dump();
            req.body() = body;
            req.set(http::field::content_length, std::to_string(body.size()));
        }

        // Wysłanie zapytania
        http::write(stream, req);

        // Bufor na odpowiedź
        beast::flat_buffer buffer;
        http::response<http::string_body> res;

        // Odbiór odpowiedzi
        http::read(stream, buffer, res);

        // Zamknięcie połączenia
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);

        // Ignorowanie błędu związanego z zamknięciem przez serwer
        if (ec && ec != beast::errc::not_connected) {
            throw beast::system_error{ec};
        }

        // Parsowanie odpowiedzi
        if (res.result() == http::status::ok) {
            try {
                return json::parse(res.body());
            } catch (const std::exception& e) {
                std::cerr << "Błąd parsowania JSON: " << e.what() << std::endl;
                return {{"status", "error"}, {"message", "Nieprawidłowy format odpowiedzi"}};
            }
        } else {
            std::cerr << "Błąd HTTP: " << res.result_int() << " " << res.reason() << std::endl;
            return {{"status", "error"}, {"message", "Błąd HTTP: " + std::to_string(res.result_int())}};
        }
    } catch (const std::exception& e) {
        connected = false;
        std::cerr << "Błąd komunikacji: " << e.what() << std::endl;
        return {{"status", "error"}, {"message", std::string("Błąd komunikacji: ") + e.what()}};
    }
}

std::vector<json> KomunikacjaKlienta::pobierzBiurka(int idBudynku) {
    std::cout << "Pobieranie biurek z serwera (budynek ID: " << idBudynku << ")" << std::endl;

    std::string target = "/api/biurka";
    if (idBudynku > 0) {
        target += "?idBudynku=" + std::to_string(idBudynku);
    }

    json odpowiedz = wykonajZapytanie(http::verb::get, target);

    std::vector<json> wynik;

    if (odpowiedz.contains("status") && odpowiedz["status"] == "success" &&
        odpowiedz.contains("biurka") && odpowiedz["biurka"].is_array()) {
        for (const auto& biurko : odpowiedz["biurka"]) {
            wynik.push_back(biurko);
        }
        std::cout << "Pobrano " << wynik.size() << " biurek" << std::endl;
    } else {
        std::cerr << "Błąd podczas pobierania biurek: "
                 << (odpowiedz.contains("message") ? odpowiedz["message"].get<std::string>() : "Nieznany błąd")
                 << std::endl;
    }

    return wynik;
}

bool KomunikacjaKlienta::dodajRezerwacje(int idBiurka, int idUzytkownika, const std::string& data) {
    std::cout << "Dodawanie rezerwacji: biurko=" << idBiurka << ", użytkownik=" << idUzytkownika
              << ", data=" << data << std::endl;

    json dane = {
        {"idBiurka", idBiurka},
        {"idUzytkownika", idUzytkownika},
        {"data", data}
    };

    json odpowiedz = wykonajZapytanie(http::verb::post, "/api/rezerwacje", dane);

    bool sukces = odpowiedz.contains("status") && odpowiedz["status"] == "success";

    if (sukces) {
        std::cout << "Rezerwacja dodana pomyślnie" << std::endl;
    } else {
        std::cerr << "Błąd podczas dodawania rezerwacji: "
                 << (odpowiedz.contains("message") ? odpowiedz["message"].get<std::string>() : "Nieznany błąd")
                 << std::endl;
    }

    return sukces;
}

bool KomunikacjaKlienta::anulujRezerwacje(int idRezerwacji) {
    std::cout << "Anulowanie rezerwacji: id=" << idRezerwacji << std::endl;

    std::string target = "/api/rezerwacje/" + std::to_string(idRezerwacji);

    json odpowiedz = wykonajZapytanie(http::verb::delete_, target);

    bool sukces = odpowiedz.contains("status") && odpowiedz["status"] == "success";

    if (sukces) {
        std::cout << "Rezerwacja anulowana pomyślnie" << std::endl;
    } else {
        std::cerr << "Błąd podczas anulowania rezerwacji: "
                 << (odpowiedz.contains("message") ? odpowiedz["message"].get<std::string>() : "Nieznany błąd")
                 << std::endl;
    }

    return sukces;
}

bool KomunikacjaKlienta::isConnected() const {
    return connected;
}