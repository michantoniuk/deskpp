#include "dialog_rezerwacji.h"
#include <QHBoxLayout>
#include <QMessageBox>

DialogRezerwacji::DialogRezerwacji(Biurko& biurko, const QDate& dataRezerwacji, QWidget* parent)
    : QDialog(parent), _biurko(biurko), _dataRezerwacji(dataRezerwacji) {

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
    _biurko.zarezerwuj(_dataRezerwacji);
    QMessageBox::information(this, "Rezerwacja", "Biurko zostało zarezerwowane pomyślnie!");
    accept();
}

void DialogRezerwacji::anulujRezerwacje() {
    _biurko.anulujRezerwacje();
    QMessageBox::information(this, "Anulowanie rezerwacji", "Rezerwacja została anulowana.");
    accept();
}