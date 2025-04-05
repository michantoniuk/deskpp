#include "dialog_rezerwacji.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <iostream>

DialogRezerwacji::DialogRezerwacji(Biurko& biurko, const QDate& dataRezerwacji, KomunikacjaKlienta& komunikacja, QWidget* parent)
    : QDialog(parent), _biurko(biurko), _dataRezerwacji(dataRezerwacji), _komunikacja(komunikacja), _idRezerwacji(0) {

    setWindowTitle("Rezerwacja biurka");
    setMinimumWidth(350);

    // Tworzenie elementów UI
    _etykietaBiurko = new QLabel(QString("Biurko: %1").arg(QString::fromStdString(biurko.etykieta())), this);
    _etykietaData = new QLabel(QString("Data: %1").arg(dataRezerwacji.toString("dd.MM.yyyy")), this);

    QString statusText;
    if (biurko.zarezerwowane()) {
        if (biurko.dataRezerwacji() == dataRezerwacji) {
            statusText = "Status: Zarezerwowane na wybraną datę";
        } else {
            statusText = QString("Status: Zarezerwowane na %1").arg(biurko.dataRezerwacji().toString("dd.MM.yyyy"));
        }
    } else {
        statusText = "Status: Dostępne";
    }
    _etykietaStatus = new QLabel(statusText, this);

    // Przyciski
    _przyciskZarezerwuj = new QPushButton("Zarezerwuj", this);
    _przyciskAnuluj = new QPushButton("Anuluj rezerwację", this);
    _przyciskZamknij = new QPushButton("Zamknij", this);

    // Układ przycisków
    auto ukladPrzyciskow = new QHBoxLayout();
    ukladPrzyciskow->addWidget(_przyciskZarezerwuj);
    ukladPrzyciskow->addWidget(_przyciskAnuluj);
    ukladPrzyciskow->addWidget(_przyciskZamknij);

    // Główny układ
    auto ukladGlowny = new QVBoxLayout(this);
    ukladGlowny->addWidget(_etykietaBiurko);
    ukladGlowny->addWidget(_etykietaData);
    ukladGlowny->addWidget(_etykietaStatus);
    ukladGlowny->addLayout(ukladPrzyciskow);

    // Sterowanie dostępnością przycisków
    _przyciskZarezerwuj->setEnabled(biurko.dostepne() && !biurko.zarezerwowane());
    _przyciskAnuluj->setEnabled(biurko.zarezerwowane() && biurko.dataRezerwacji() == dataRezerwacji);

    // Połączenia
    connect(_przyciskZarezerwuj, &QPushButton::clicked, this, &DialogRezerwacji::zarezerwujBiurko);
    connect(_przyciskAnuluj, &QPushButton::clicked, this, &DialogRezerwacji::anulujRezerwacje);
    connect(_przyciskZamknij, &QPushButton::clicked, this, &QDialog::accept);
}

void DialogRezerwacji::zarezerwujBiurko() {
    std::cout << "Próba rezerwacji biurka " << _biurko.id() << " na datę " << _dataRezerwacji.toString("yyyy-MM-dd").toStdString() << std::endl;

    // ID użytkownika - w minimalnej implementacji używamy stałej wartości
    // W pełnej implementacji można użyć ID zalogowanego użytkownika
    const int idUzytkownika = 1;

    // Wywołaj funkcję dodajRezerwacje z komunikacja_klienta.cpp
    bool sukces = _komunikacja.dodajRezerwacje(
        _biurko.id(),
        idUzytkownika,
        _dataRezerwacji.toString("yyyy-MM-dd").toStdString()
    );

    if (sukces) {
        // Aktualizuj lokalnie obiekt biurka aby UI był spójny
        _biurko.zarezerwuj(_dataRezerwacji);
        QMessageBox::information(this, "Rezerwacja", "Biurko zostało zarezerwowane pomyślnie!");
        accept();
    } else {
        QMessageBox::warning(this, "Błąd rezerwacji",
            "Nie udało się zarezerwować biurka. Może być już zarezerwowane lub serwer jest niedostępny.");
    }
}

void DialogRezerwacji::anulujRezerwacje() {
    std::cout << "Próba anulowania rezerwacji biurka " << _biurko.id() << std::endl;

    // W minimalnej wersji musimy znać id rezerwacji
    // W tej implementacji zakładamy, że możemy anulować rezerwację po id biurka
    // (co jest uproszczeniem, ponieważ biurko może mieć wiele rezerwacji na różne daty)

    // Używamy ID biurka jako ID rezerwacji (w rzeczywistości powinniśmy znać prawidłowe ID)
    int idRezerwacji = _biurko.id();

    bool sukces = _komunikacja.anulujRezerwacje(idRezerwacji);

    if (sukces) {
        // Aktualizuj lokalnie obiekt biurka
        _biurko.anulujRezerwacje();
        QMessageBox::information(this, "Anulowanie rezerwacji", "Rezerwacja została anulowana.");
        accept();
    } else {
        QMessageBox::warning(this, "Błąd anulowania",
            "Nie udało się anulować rezerwacji. Serwer może być niedostępny.");
    }
}