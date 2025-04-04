#ifndef DIALOG_REZERWACJI_H
#define DIALOG_REZERWACJI_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDate>
#include "biurko.h"

class DialogRezerwacji : public QDialog {
    Q_OBJECT

public:
    DialogRezerwacji(Biurko& biurko, const QDate& dataRezerwacji, QWidget* parent = nullptr);

    private slots:
        void zarezerwujBiurko();
    void anulujRezerwacje();

private:
    Biurko& _biurko;
    QDate _dataRezerwacji;

    QLabel* _etykietaBiurko;
    QLabel* _etykietaData;
    QLabel* _etykietaStatus;
    QPushButton* _przyciskZarezerwuj;
    QPushButton* _przyciskAnuluj;
    QPushButton* _przyciskZamknij;
};

#endif // DIALOG_REZERWACJI_H