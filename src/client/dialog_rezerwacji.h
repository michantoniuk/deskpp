#ifndef DIALOG_REZERWACJI_H
#define DIALOG_REZERWACJI_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include "src/common/biurko.h"
#include "komunikacja_klienta.h"

class DialogRezerwacji : public QDialog {
    Q_OBJECT

public:
    DialogRezerwacji(Biurko& biurko, const QDate& dataRezerwacji, KomunikacjaKlienta& komunikacja, QWidget* parent = nullptr);

    private slots:
        void zarezerwujBiurko();
    void anulujRezerwacje();

private:
    Biurko& _biurko;
    QDate _dataRezerwacji;
    KomunikacjaKlienta& _komunikacja;
    int _idRezerwacji;

    QLabel* _etykietaBiurko;
    QLabel* _etykietaData;
    QLabel* _etykietaStatus;
    QPushButton* _przyciskZarezerwuj;
    QPushButton* _przyciskAnuluj;
    QPushButton* _przyciskZamknij;
};

#endif // DIALOG_REZERWACJI_H