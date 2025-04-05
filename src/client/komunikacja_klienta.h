#ifndef KOMUNIKACJA_KLIENTA_H
#define KOMUNIKACJA_KLIENTA_H

#include <string>
#include <vector>
#include <boost/asio/io_context.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using json = nlohmann::json;

class KomunikacjaKlienta {
public:
    KomunikacjaKlienta(const std::string& adresSerwera = "localhost", int port = 8080);
    ~KomunikacjaKlienta();

    // Test połączenia z serwerem
    bool testConnection();

    // Czy klient jest połączony z serwerem
    bool isConnected() const;

    // Pobierz listę biurek
    std::vector<json> pobierzBiurka(int idBudynku = -1);

    // Dodaj rezerwację
    bool dodajRezerwacje(int idBiurka, int idUzytkownika, const std::string& data);

    // Anuluj rezerwację
    bool anulujRezerwacje(int idRezerwacji);

private:
    // Metoda wykonująca zapytanie HTTP
    json wykonajZapytanie(http::verb metoda,
                         const std::string& endpoint,
                         const json& dane = json::object());

    std::string adresSerwera;
    int port;
    net::io_context io_context;
    bool connected;  // Stan połączenia z serwerem
};

#endif // KOMUNIKACJA_KLIENTA_H