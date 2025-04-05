#include "widok_rezerwacji.h"
#include "dialog_rezerwacji.h"
#include "komunikacja_klienta.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QStatusBar>
#include <iostream>

WidokRezerwacji::WidokRezerwacji(QWidget* parent, const std::string& adresSerwera, int port)
    : QMainWindow(parent),
      centralnyWidget(new QWidget(this)),
      kalendarz(new QCalendarWidget(this)),
      wybierzBudynek(new QComboBox(this)),
      wybierzPietro(new QComboBox(this)),
      etykietaInfo(new QLabel(this)),
      ukladMapyBiurek(new QGridLayout()),
      kontenerMapyBiurek(new QWidget(this)),
      komunikacja(adresSerwera, port), // Użyj przekazanego adresu i portu
      wybranyBudynek(1),
      wybranePietro(1),
      wybranaData(QDate::currentDate()) {

    setCentralWidget(centralnyWidget);
    inicjalizujUI();

    // Połącz sygnały i sloty
    connect(kalendarz, &QCalendarWidget::clicked, this, &WidokRezerwacji::zmianaData);
    connect(wybierzBudynek, &QComboBox::currentIndexChanged, this, &WidokRezerwacji::zmianaBudynku);

    // Ustaw tytuł i rozmiar okna
    setWindowTitle("Biurko++ - System Rezerwacji Biurek");
    resize(800, 600);

    // Sprawdź połączenie z serwerem i wyświetl odpowiednią informację
    if (komunikacja.isConnected()) {
        statusBar()->showMessage("Połączono z serwerem. Wybierz biurko, aby zobaczyć szczegóły lub dokonać rezerwacji", 5000);
        // Inicjalne wypełnienie mapy biurek
        zmianaBudynku(0);
    } else {
        QString errorMsg = "Nie można połączyć się z serwerem. Sprawdź, czy serwer jest uruchomiony.";
        statusBar()->showMessage(errorMsg, 0); // Pokazuj komunikat aż do odwołania
        QMessageBox::critical(this, "Błąd połączenia",
            errorMsg + "\nAplikacja będzie działać w trybie offline z ograniczoną funkcjonalnością.");
    }
}

void WidokRezerwacji::zmianaBudynku(int indeks) {
    wybranyBudynek = indeks + 1;

    try {
        // Sprawdź połączenie przed pobraniem danych
        if (!komunikacja.isConnected() && !komunikacja.testConnection()) {
            statusBar()->showMessage("Nie można połączyć się z serwerem. Działanie w trybie offline.", 5000);
            QMessageBox::warning(this, "Błąd komunikacji", "Nie można połączyć się z serwerem.");
            return;
        }

        // Pobierz biurka dla wybranego budynku z serwera
        biurka = pobierzBiurkaZSerwera(wybranyBudynek);

        // Zaktualizuj mapę biurek
        aktualizujMapeBiurek();

        statusBar()->showMessage(QString("Załadowano %1 biurek z serwera").arg(biurka.size()), 3000);
    }
    catch (const std::exception& e) {
        QString errorMsg = QString("Błąd podczas pobierania danych z serwera: %1").arg(e.what());
        statusBar()->showMessage(errorMsg, 5000);
        QMessageBox::warning(this, "Błąd komunikacji", errorMsg);
        std::cerr << "Błąd: " << e.what() << std::endl;
    }
}

void WidokRezerwacji::zmianaData(const QDate& data) {
    wybranaData = data;
    etykietaInfo->setText(QString("Plan biur na dzień %1").arg(data.toString("dd.MM.yyyy")));

    // Odśwież widok biurek, bo dostępność może się zmieniać w zależności od daty
    odswiezWidok();
}

void WidokRezerwacji::klikniecieBiurka() {
    // Pobierz nadawcę sygnału (przycisk)
    QPushButton* przycisk = qobject_cast<QPushButton*>(sender());
    if (!przycisk) return;

    // Pobierz indeks biurka z właściwości przycisku
    bool ok;
    int indeksBiurka = przycisk->property("indeks").toInt(&ok);
    if (!ok || indeksBiurka < 0 || indeksBiurka >= static_cast<int>(biurka.size())) return;

    // Sprawdź połączenie przed otwarciem dialogu
    if (!komunikacja.isConnected() && !komunikacja.testConnection()) {
        QMessageBox::warning(this, "Brak połączenia",
            "Nie można wykonać operacji rezerwacji bez połączenia z serwerem.");
        return;
    }

    // Otwórz dialog rezerwacji dla tego biurka
    DialogRezerwacji dialog(biurka[indeksBiurka], wybranaData, komunikacja, this);
    dialog.exec();

    // Po zamknięciu dialogu odśwież widok, pobierając dane z serwera
    odswiezWidok();
}

void WidokRezerwacji::odswiezWidok() {
    // Sprawdź połączenie przed próbą pobrania danych
    if (!komunikacja.isConnected()) {
        if (komunikacja.testConnection()) {
            statusBar()->showMessage("Połączono z serwerem. Odświeżanie danych...", 3000);
        } else {
            statusBar()->showMessage("Nie można połączyć się z serwerem. Działanie w trybie offline.", 5000);
            return;
        }
    }

    // Pobierz zaktualizowane dane z serwera
    try {
        biurka = pobierzBiurkaZSerwera(wybranyBudynek);

        // Aktualizuj mapę biurek
        aktualizujMapeBiurek();

        statusBar()->showMessage("Dane odświeżone pomyślnie", 3000);
    }
    catch (const std::exception& e) {
        QString errorMsg = QString("Błąd podczas odświeżania danych: %1").arg(e.what());
        statusBar()->showMessage(errorMsg, 5000);
        QMessageBox::warning(this, "Błąd komunikacji", errorMsg);
    }
}

void WidokRezerwacji::inicjalizujUI() {
    // Układ główny
    auto ukladGlowny = new QVBoxLayout(centralnyWidget);

    // Górny panel z kalendarzem i opcjami wyboru
    auto ukladGorny = new QHBoxLayout();

    // Panel z kalendarzem
    auto grupaPanelKalendarz = new QGroupBox("Wybierz datę", this);
    auto ukladKalendarz = new QVBoxLayout(grupaPanelKalendarz);
    ukladKalendarz->addWidget(kalendarz);
    ukladGorny->addWidget(grupaPanelKalendarz);

    // Panel z opcjami wyboru
    auto grupaPanelOpcje = new QGroupBox("Opcje wyboru", this);
    auto ukladOpcje = new QVBoxLayout(grupaPanelOpcje);

    // Dodaj elementy do panelu opcji
    auto ukladBudynek = new QHBoxLayout();
    ukladBudynek->addWidget(new QLabel("Budynek:", this));
    ukladBudynek->addWidget(wybierzBudynek);
    ukladOpcje->addLayout(ukladBudynek);

    auto ukladPietro = new QHBoxLayout();
    ukladPietro->addWidget(new QLabel("Piętro:", this));
    ukladPietro->addWidget(wybierzPietro);
    ukladOpcje->addLayout(ukladPietro);

    // Dodaj informację o wybranej dacie
    etykietaInfo = new QLabel(QString("Plan biur na dzień %1").arg(QDate::currentDate().toString("dd.MM.yyyy")), this);
    etykietaInfo->setAlignment(Qt::AlignCenter);
    ukladOpcje->addWidget(etykietaInfo);

    // Wypełnij combobox z budynkami
    wybierzBudynek->addItem("Kraków A");
    wybierzBudynek->addItem("Warszawa B");

    // Wypełnij combobox z piętrami
    wybierzPietro->addItem("1 piętro");

    ukladGorny->addWidget(grupaPanelOpcje);

    // Dodaj górny panel do głównego układu
    ukladGlowny->addLayout(ukladGorny);

    // Panel z mapą biurek
    auto grupaPanelMapa = new QGroupBox("Mapa biurek", this);
    auto ukladMapa = new QVBoxLayout(grupaPanelMapa);

    // Dodaj przycisk odświeżania
    auto przyciskOdswiez = new QPushButton("Odśwież dane", this);
    connect(przyciskOdswiez, &QPushButton::clicked, this, &WidokRezerwacji::odswiezWidok);
    ukladMapa->addWidget(przyciskOdswiez);

    // Utwórz kontener mapy biurek
    kontenerMapyBiurek->setLayout(ukladMapyBiurek);

    // Utwórz obszar przewijania dla mapy biurek
    auto obszarPrzewijania = new QScrollArea(this);
    obszarPrzewijania->setWidgetResizable(true);
    obszarPrzewijania->setWidget(kontenerMapyBiurek);

    ukladMapa->addWidget(obszarPrzewijania);
    ukladGlowny->addWidget(grupaPanelMapa, 1); // Ustaw stretch, aby mapa zajmowała więcej miejsca
}

std::vector<Biurko> WidokRezerwacji::pobierzBiurkaZSerwera(int idBudynku) {
    std::cout << "Pobieranie biurek z serwera dla budynku " << idBudynku << std::endl;

    // Pobierz dane z serwera za pomocą KomunikacjaKlienta
    std::vector<json> biurkaJson = komunikacja.pobierzBiurka(idBudynku);

    std::cout << "Otrzymano " << biurkaJson.size() << " biurek z serwera" << std::endl;

    std::vector<Biurko> biurka;

    // Konwertuj dane JSON na obiekty Biurko
    for (const auto& biurkoJson : biurkaJson) {
        std::cout << "Przetwarzanie biurka: " << biurkoJson.dump() << std::endl;

        int id = biurkoJson.contains("id") ? biurkoJson["id"].get<int>() : 0;
        std::string etykieta = biurkoJson.contains("etykieta") ? biurkoJson["etykieta"].get<std::string>() : "Nieznane";
        int idBudynku = biurkoJson.contains("idBudynku") ? biurkoJson["idBudynku"].get<int>() : 0;
        int numerPietra = biurkoJson.contains("numerPietra") ? biurkoJson["numerPietra"].get<int>() : 0;
        bool dostepne = biurkoJson.contains("dostepne") ? biurkoJson["dostepne"].get<bool>() : true;

        Biurko biurko(id, etykieta, idBudynku, numerPietra, dostepne);

        // Sprawdź, czy biurko jest zarezerwowane
        if (biurkoJson.contains("zarezerwowane") && biurkoJson["zarezerwowane"].get<bool>()) {
            // Jeśli zawiera datę, ustaw ją
            if (biurkoJson.contains("dataRezerwacji") && !biurkoJson["dataRezerwacji"].is_null()) {
                std::string dataStr = biurkoJson["dataRezerwacji"].get<std::string>();
                QDate date = QDate::fromString(QString::fromStdString(dataStr), "yyyy-MM-dd");
                biurko.zarezerwuj(date);
            } else {
                // Brak daty, używamy dzisiejszej daty
                biurko.zarezerwuj(QDate::currentDate());
            }
        }

        biurka.push_back(biurko);
    }

    return biurka;
}

void WidokRezerwacji::aktualizujMapeBiurek() {
    // Wyczyść stare przyciski
    for (auto przycisk : przyciskiBiurek) {
        ukladMapyBiurek->removeWidget(przycisk);
        delete przycisk;
    }
    przyciskiBiurek.clear();

    // Liczba kolumn na mapie biurek
    const int liczbaKolumn = 4;

    // Utwórz nowe przyciski biurek
    for (size_t i = 0; i < biurka.size(); ++i) {
        const auto& biurko = biurka[i];

        // Utwórz przycisk dla biurka
        auto przycisk = new QPushButton(QString::fromStdString(biurko.etykieta()), this);
        przycisk->setMinimumSize(100, 80);
        przycisk->setMaximumSize(150, 120);

        // Ustaw kolor tła w zależności od dostępności i rezerwacji
        QString stylPrzycisku;
        if (!biurko.dostepne()) {
            stylPrzycisku = "background-color: #ef9a9a; color: black;"; // Czerwony (niedostępne)
        } else if (biurko.zarezerwowane()) {
            if (biurko.dataRezerwacji() == wybranaData) {
                stylPrzycisku = "background-color: #ffcc80; color: black;"; // Pomarańczowy (zarezerwowane na wybraną datę)
            } else {
                stylPrzycisku = "background-color: #ffe0b2; color: black;"; // Jasny pomarańcz (zarezerwowane na inną datę)
            }
        } else {
            stylPrzycisku = "background-color: #a5d6a7; color: black;"; // Zielony (dostępne)
        }
        przycisk->setStyleSheet(stylPrzycisku);

        // Dodaj informację o rezerwacji
        if (biurko.zarezerwowane()) {
            QString text = QString::fromStdString(biurko.etykieta());
            text += "\n";
            text += biurko.dataRezerwacji().toString("dd.MM");
            przycisk->setText(text);
        }

        // Zapisz indeks biurka jako właściwość przycisku
        przycisk->setProperty("indeks", static_cast<int>(i));

        // Połącz sygnał kliknięcia
        connect(przycisk, &QPushButton::clicked, this, &WidokRezerwacji::klikniecieBiurka);

        // Dodaj przycisk do układu
        int wiersz = i / liczbaKolumn;
        int kolumna = i % liczbaKolumn;
        ukladMapyBiurek->addWidget(przycisk, wiersz, kolumna);

        // Dodaj przycisk do listy
        przyciskiBiurek.push_back(przycisk);
    }
}