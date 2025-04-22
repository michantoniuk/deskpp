# Biurko++ - System Rezerwacji Biurek

Biurko++ to system do zarządzania rezerwacjami biurek w środowisku biurowym. Aplikacja umożliwia rezerwację biurek w różnych budynkach i na różnych piętrach, co ułatwia organizację pracy w biurze.

## Funkcje

- **Zarządzanie budynkami i biurkami** - obsługa wielu budynków, pięter i biurek
- **Konta użytkowników** - rejestracja i logowanie użytkowników
- **Rezerwacje** - możliwość rezerwacji biurek na wybrany okres (od-do)
- **Filtrowanie** - wyszukiwanie biurek według budynków i pięter
- **Kalendarz** - wybór dat rezerwacji z widokiem kalendarza

## Architektura systemu

DeskPP składa się z dwóch głównych komponentów:

1. **Serwer** - aplikacja backend w C++ z wykorzystaniem biblioteki Crow do implementacji REST API. Serwer zarządza danymi w bazie SQLite.

2. **Klient** - aplikacja desktopowa w Qt, która komunikuje się z serwerem poprzez API i umożliwia użytkownikom przeglądanie i rezerwację biurek.

## Technologie

- **C++20** - język programowania
- **Qt 6** - framework GUI dla klienta
- **Crow** - biblioteka C++ do tworzenia REST API
- **SQLite/SQLiteCpp** - baza danych i interfejs do niej
- **nlohmann/json** - obsługa formatu JSON
- **spdlog** - logowanie

## Uruchomienie

1. Skompiluj projekt używając CMake:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

2. Uruchom serwer:
```bash
./deskpp_server
```

3. Uruchom klienta:
```bash
./deskpp_client
```

Opcje dla serwera:
- `--port`, `-p` - port serwera (domyślnie 8080)
- `--database`, `-db` - ścieżka do pliku bazy danych (domyślnie deskpp.sqlite)
- `--verbose`, `-v` - włącza szczegółowe logowanie

Opcje dla klienta:
- `--server`, `-s` - adres serwera (domyślnie localhost)
- `--port`, `-p` - port serwera (domyślnie 8080)
- `--verbose`, `-v` - włącza szczegółowe logowanie

## Struktura projektu

```
deskpp/
├── CMakeLists.txt
├── src/
│   ├── common/          # Wspólne komponenty klienta i serwera
│   │   ├── logger.h     # System logowania
│   │   ├── app_settings.h # Ustawienia aplikacji
│   │   └── model/       # Modele danych
│   ├── client/          # Aplikacja kliencka
│   │   ├── main.cpp     # Punkt wejścia klienta
│   │   ├── ui/          # Interfejs użytkownika
│   │   └── net/         # Komunikacja z serwerem
│   └── server/          # Aplikacja serwerowa
│       ├── main.cpp     # Punkt wejścia serwera
│       ├── api/         # Endpointy API
│       ├── repository/  # Dostęp do bazy danych
│       └── service/     # Logika biznesowa
```

## Autor

Michał Antoniuk

*Projekt studencki na przedmiot Zaawansowane C++ (Politechnika Warszawska)*
