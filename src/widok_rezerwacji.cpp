#include "widok_rezerwacji.h"
#include "dialog_rezerwacji.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QStatusBar>

WidokRezerwacji::WidokRezerwacji(QWidget* parent)
    : QMainWindow(parent),
      centralnyWidget(new QWidget(this)),
      kalendarz(new QCalendarWidget(this)),
      wybierzBudynek(new QComboBox(this)),
      wybierzPietro(new QComboBox(this)),
      etykietaInfo(new QLabel(this)),
      ukladMapyBiurek(new QGridLayout()),
      kontenerMapyBiurek(new QWidget(this)),
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

    // Pokaż informację w pasku statusu
    statusBar()->showMessage("Wybierz biurko, aby zobaczyć szczegóły lub dokonać rezerwacji", 5000);

    // Inicjalne wypełnienie mapy biurek
    zmianaBudynku(0);
}

void WidokRezerwacji::zmianaBudynku(int indeks) {
    wybranyBudynek = indeks + 1;

    // Pobierz biurka dla wybranego budynku
    biurka = generujBiurka(wybranyBudynek);

    // Zaktualizuj mapę biurek
    aktualizujMapeBiurek();
}

void WidokRezerwacji::zmianaData(const QDate& data) {
    wybranaData = data;
    etykietaInfo->setText(QString("Plan biur na dzień %1").arg(data.toString("dd.MM.yyyy")));

    // Odśwież widok biurek, bo dostępność może się zmieniać w zależności od daty
    aktualizujMapeBiurek();
}

void WidokRezerwacji::klikniecieBiurka() {
    // Pobierz nadawcę sygnału (przycisk)
    QPushButton* przycisk = qobject_cast<QPushButton*>(sender());
    if (!przycisk) return;

    // Pobierz indeks biurka z właściwości przycisku
    bool ok;
    int indeksBiurka = przycisk->property("indeks").toInt(&ok);
    if (!ok || indeksBiurka < 0 || indeksBiurka >= static_cast<int>(biurka.size())) return;

    // Otwórz dialog rezerwacji dla tego biurka
    DialogRezerwacji dialog(biurka[indeksBiurka], wybranaData, this);
    dialog.exec();

    // Po zamknięciu dialogu odśwież widok
    odswiezWidok();
}

void WidokRezerwacji::odswiezWidok() {
    // Odśwież widok biurek
    aktualizujMapeBiurek();
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

    // Utwórz kontener mapy biurek
    kontenerMapyBiurek->setLayout(ukladMapyBiurek);

    // Utwórz obszar przewijania dla mapy biurek
    auto obszarPrzewijania = new QScrollArea(this);
    obszarPrzewijania->setWidgetResizable(true);
    obszarPrzewijania->setWidget(kontenerMapyBiurek);

    ukladMapa->addWidget(obszarPrzewijania);
    ukladGlowny->addWidget(grupaPanelMapa, 1); // Ustaw stretch, aby mapa zajmowała więcej miejsca
}

std::vector<Biurko> WidokRezerwacji::generujBiurka(int idBudynku) {
    std::vector<Biurko> biurka;

    // Generowanie przykładowych biurek dla budynku Kraków A
    if (idBudynku == 1) {
        biurka.emplace_back(1, "KrakA-01-001", 1, 1);
        biurka.emplace_back(2, "KrakA-01-002", 1, 1);
        biurka.emplace_back(3, "KrakA-01-003", 1, 1, false);
        biurka.emplace_back(4, "KrakA-01-004", 1, 1);
        biurka.emplace_back(5, "KrakA-01-005", 1, 1);
        biurka.emplace_back(6, "KrakA-01-006", 1, 1);
        biurka.emplace_back(7, "KrakA-01-007", 1, 1, false);
        biurka.emplace_back(8, "KrakA-01-008", 1, 1);
        biurka.emplace_back(9, "KrakA-01-009", 1, 1);
    }

    // Generowanie przykładowych biurek dla budynku Warszawa B
    else if (idBudynku == 2) {
        biurka.emplace_back(10, "WawB-01-001", 2, 1);
        biurka.emplace_back(11, "WawB-01-002", 2, 1);
        biurka.emplace_back(12, "WawB-01-003", 2, 1);
        biurka.emplace_back(13, "WawB-01-004", 2, 1);

        // Ustawienie jednego biurka jako niedostępne
        biurka[1].ustawDostepnosc(false);
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