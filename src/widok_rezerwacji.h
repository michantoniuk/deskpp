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
#include "biurko.h"

class WidokRezerwacji : public QMainWindow {
    Q_OBJECT

public:
    explicit WidokRezerwacji(QWidget* parent = nullptr);

    private slots:
        void zmianaBudynku(int indeks);
    void zmianaData(const QDate& data);
    void klikniecieBiurka();
    void odswiezWidok();

private:
    void inicjalizujUI();
    void aktualizujMapeBiurek();
    std::vector<Biurko> generujBiurka(int idBudynku);

    QWidget* centralnyWidget;
    QCalendarWidget* kalendarz;
    QComboBox* wybierzBudynek;
    QComboBox* wybierzPietro;
    QLabel* etykietaInfo;

    QGridLayout* ukladMapyBiurek;
    QWidget* kontenerMapyBiurek;

    std::vector<Biurko> biurka;
    std::vector<QPushButton*> przyciskiBiurek;

    int wybranyBudynek;
    int wybranePietro;
    QDate wybranaData;
};

#endif // WIDOK_REZERWACJI_H