#include "kontroler_serwera.h"
#include "logger.h"
#include <iostream>

KontrolerSerwera::KontrolerSerwera() {
    inicjalizujPrzykladoweDane();
}

KontrolerSerwera::~KontrolerSerwera() {
    LOG_INFO("Finalizacja kontrolera serwera");
}

void KontrolerSerwera::inicjalizujPrzykladoweDane() {
    LOG_INFO("Inicjalizacja kontrolera serwera");

    // Inicjalizacja przykładowych biurek
    przykladoweBiurka = json::array();
    przykladoweBiurka.push_back({
        {"id", 1},
        {"etykieta", "KrakA-01-001"},
        {"idBudynku", 1},
        {"numerPietra", 1},
        {"dostepne", true},
        {"zarezerwowane", false}
    });
    przykladoweBiurka.push_back({
        {"id", 2},
        {"etykieta", "KrakA-01-002"},
        {"idBudynku", 1},
        {"numerPietra", 1},
        {"dostepne", true},
        {"zarezerwowane", false}
    });
    przykladoweBiurka.push_back({
        {"id", 3},
        {"etykieta", "KrakA-01-003"},
        {"idBudynku", 1},
        {"numerPietra", 1},
        {"dostepne", false},
        {"zarezerwowane", false}
    });

    // Przykładowe biurka w budynku 2
    przykladoweBiurka.push_back({
        {"id", 4},
        {"etykieta", "WawB-01-001"},
        {"idBudynku", 2},
        {"numerPietra", 1},
        {"dostepne", true},
        {"zarezerwowane", false}
    });
    przykladoweBiurka.push_back({
        {"id", 5},
        {"etykieta", "WawB-01-002"},
        {"idBudynku", 2},
        {"numerPietra", 1},
        {"dostepne", true},
        {"zarezerwowane", false}
    });

    // Inicjalizacja pustej tablicy rezerwacji
    przykladoweRezerwacje = json::array();
}

json KontrolerSerwera::pobierzBiurka(const json& parametry) {
    LOG_INFO("Wywołanie metody pobierzBiurka");

    int idBudynku = -1;
    if (parametry.contains("idBudynku")) {
        idBudynku = parametry["idBudynku"];
        LOG_DEBUG("Parametr idBudynku = {}", idBudynku);
    }

    json wynik = json::array();
    for (const auto& biurko : przykladoweBiurka) {
        if (idBudynku == -1 || biurko["idBudynku"] == idBudynku) {
            wynik.push_back(biurko);
        }
    }

    LOG_INFO("Znaleziono {} biurek", wynik.size());
    return {
        {"status", "success"},
        {"biurka", wynik}
    };
}

json KontrolerSerwera::pobierzRezerwacje(const json& parametry) {
    LOG_INFO("Wywołanie metody pobierzRezerwacje");

    if (!parametry.contains("idBiurka") || !parametry.contains("data")) {
        LOG_WARNING("Brak wymaganych parametrów idBiurka lub data");
        return {
            {"status", "error"},
            {"message", "Brak wymaganych parametrów idBiurka lub data"}
        };
    }

    int idBiurka = parametry["idBiurka"];
    std::string data = parametry["data"];

    LOG_DEBUG("Parametry: idBiurka={}, data={}", idBiurka, data);

    json wynik = json::array();
    for (const auto& rezerwacja : przykladoweRezerwacje) {
        if (rezerwacja["idBiurka"] == idBiurka && rezerwacja["data"] == data) {
            wynik.push_back(rezerwacja);
        }
    }

    LOG_INFO("Znaleziono {} rezerwacji", wynik.size());
    return {
        {"status", "success"},
        {"rezerwacje", wynik}
    };
}

json KontrolerSerwera::dodajRezerwacje(const json& parametry) {
    LOG_INFO("Wywołanie metody dodajRezerwacje");

    // Walidacja parametrów
    if (!parametry.contains("idBiurka") || !parametry.contains("idUzytkownika") ||
        !parametry.contains("data")) {
        LOG_WARNING("Brak wymaganych parametrów");
        return {
            {"status", "error"},
            {"message", "Brak wymaganych parametrów (idBiurka, idUzytkownika, data)"}
        };
    }

    int idBiurka = parametry["idBiurka"];
    int idUzytkownika = parametry["idUzytkownika"];
    std::string data = parametry["data"];

    LOG_DEBUG("Parametry: idBiurka={}, idUzytkownika={}, data={}",
            idBiurka, idUzytkownika, data);

    // Sprawdzenie, czy biurko istnieje
    for (auto& biurko : przykladoweBiurka) {
        if (biurko["id"] == idBiurka) {
            // Sprawdzenie, czy biurko jest dostępne
            if (!biurko["dostepne"]) {
                LOG_WARNING("Biurko {} jest niedostępne", idBiurka);
                return {
                    {"status", "error"},
                    {"message", "Biurko jest niedostępne"}
                };
            }

            // Sprawdzenie, czy biurko jest już zarezerwowane
            if (biurko["zarezerwowane"]) {
                LOG_WARNING("Biurko {} jest już zarezerwowane", idBiurka);
                return {
                    {"status", "error"},
                    {"message", "Biurko jest już zarezerwowane"}
                };
            }

            // Dodanie rezerwacji
            int idRezerwacji = przykladoweRezerwacje.size() + 1;
            json nowaRezerwacja = {
                {"id", idRezerwacji},
                {"idBiurka", idBiurka},
                {"idUzytkownika", idUzytkownika},
                {"data", data},
                {"status", "POTWIERDZONA"}
            };

            przykladoweRezerwacje.push_back(nowaRezerwacja);
            LOG_INFO("Dodano rezerwację id={} dla biurka {}", idRezerwacji, idBiurka);

            // Aktualizacja statusu biurka
            biurko["zarezerwowane"] = true;
            biurko["dataRezerwacji"] = data;

            return {
                {"status", "success"},
                {"rezerwacja", nowaRezerwacja}
            };
        }
    }

    LOG_WARNING("Biurko o id={} nie istnieje", idBiurka);
    return {
        {"status", "error"},
        {"message", "Biurko o podanym ID nie istnieje"}
    };
}

json KontrolerSerwera::anulujRezerwacje(const json& parametry) {
    LOG_INFO("Wywołanie metody anulujRezerwacje");

    if (!parametry.contains("idRezerwacji")) {
        LOG_WARNING("Brak wymaganego parametru idRezerwacji");
        return {
            {"status", "error"},
            {"message", "Brak wymaganego parametru idRezerwacji"}
        };
    }

    int idRezerwacji = parametry["idRezerwacji"];
    LOG_DEBUG("Parametr: idRezerwacji={}", idRezerwacji);

    // Szukanie rezerwacji
    for (size_t i = 0; i < przykladoweRezerwacje.size(); ++i) {
        if (przykladoweRezerwacje[i]["id"] == idRezerwacji) {
            // Znajdź biurko i oznacz jako dostępne
            int idBiurka = przykladoweRezerwacje[i]["idBiurka"];
            LOG_INFO("Znaleziono rezerwację id={} dla biurka {}", idRezerwacji, idBiurka);

            for (auto& biurko : przykladoweBiurka) {
                if (biurko["id"] == idBiurka) {
                    biurko["zarezerwowane"] = false;
                    biurko.erase("dataRezerwacji");
                    LOG_DEBUG("Zaktualizowano status biurka id={}", idBiurka);
                    break;
                }
            }

            // Usuń rezerwację
            przykladoweRezerwacje.erase(przykladoweRezerwacje.begin() + i);
            LOG_INFO("Usunięto rezerwację id={}", idRezerwacji);

            return {
                {"status", "success"},
                {"message", "Rezerwacja została anulowana"}
            };
        }
    }

    LOG_WARNING("Rezerwacja o id={} nie została znaleziona", idRezerwacji);
    return {
        {"status", "error"},
        {"message", "Rezerwacja o podanym ID nie istnieje"}
    };
}