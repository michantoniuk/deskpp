#include <iostream>
#include <string>
#include "widok_rezerwacji.h"
#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QMessageBox>

#include "komunikacja_klienta.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Domyślne wartości
    std::string adresSerwera = "localhost";
    int portSerwera = 8080;  // Używamy portu 8080, zgodnego z serwerem

    // Obsługa argumentów wiersza poleceń
    if (argc > 1) {
        adresSerwera = argv[1];
    }
    if (argc > 2) {
        try {
            portSerwera = std::stoi(argv[2]);
        } catch (...) {
            std::cerr << "Nieprawidłowy format portu. Używanie domyślnego portu 8080." << std::endl;
        }
    }

    std::cout << "Łączenie z serwerem " << adresSerwera << " na porcie " << portSerwera << std::endl;

    // Inicjalizacja komunikacji z serwerem
    KomunikacjaKlienta komunikacja(adresSerwera, portSerwera);

    // Sprawdź połączenie z serwerem
    if (!komunikacja.isConnected()) {
        // Pokaż dialog z ostrzeżeniem, ale kontynuuj uruchamianie aplikacji
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Ostrzeżenie");
        msgBox.setText("Nie można połączyć się z serwerem. Aplikacja będzie działać w trybie ograniczonej funkcjonalności.");
        msgBox.setInformativeText("Czy chcesz kontynuować?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);

        int ret = msgBox.exec();
        if (ret == QMessageBox::No) {
            return 1;
        }
    }

    // Uruchomienie aplikacji
    WidokRezerwacji okno(nullptr, adresSerwera, portSerwera);
    okno.show();

    return app.exec();
}