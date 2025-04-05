#ifndef WIDOK_REZERWACJI_H
#define WIDOK_REZERWACJI_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDate>
#include <vector>
#include "src/common/biurko.h"
#include "komunikacja_klienta.h"

class WidokRezerwacji : public QMainWindow {
    Q_OBJECT

public:
    explicit WidokRezerwacji(QWidget* parent = nullptr, const std::string& adresSerwera = "localhost", int port = 8080);

    private slots:
        void zmianaBudynku(int indeks);
    void zmianaData(const QDate& data);
    void klikniecieBiurka();
    void odswiezWidok();

private:
    void inicjalizujUI();
    void aktualizujMapeBiurek();
    std::vector<Biurko> pobierzBiurkaZSerwera(int idBudynku);

    QWidget* centralnyWidget;
    QCalendarWidget* kalendarz;
    QComboBox* wybierzBudynek;
    QComboBox* wybierzPietro;
    QLabel* etykietaInfo;

    QGridLayout* ukladMapyBiurek;
    QWidget* kontenerMapyBiurek;

    KomunikacjaKlienta komunikacja;  // Instancja do komunikacji z serwerem

    std::vector<Biurko> biurka;
    std::vector<QPushButton*> przyciskiBiurek;

    int wybranyBudynek;
    int wybranePietro;
    QDate wybranaData;
};

#endif // WIDOK_REZERWACJI_H