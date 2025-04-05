
#ifndef KONTROLER_SERWERA_H
#define KONTROLER_SERWERA_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// Używamy biblioteki nlohmann/json do obsługi JSON
using json = nlohmann::json;

class KontrolerSerwera {
public:
    KontrolerSerwera();
    ~KontrolerSerwera();

    // Metody obsługi zapytań REST API
    json pobierzBiurka(const json& parametry);
    json pobierzRezerwacje(const json& parametry);
    json dodajRezerwacje(const json& parametry);
    json anulujRezerwacje(const json& parametry);

private:
    // Inicjalizacja przykładowych danych
    void inicjalizujPrzykladoweDane();

    // Przykładowe dane (w minimalnej wersji bez bazy danych)
    json przykladoweBiurka;
    json przykladoweRezerwacje;
};

#endif // KONTROLER_SERWERA_H