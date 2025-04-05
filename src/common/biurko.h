#ifndef BIURKO_H
#define BIURKO_H

#include <string>
#include <QDate>

class Biurko {
public:
    Biurko(int id, std::string etykieta, int idBudynku, int nrPietra);
    Biurko(int id, std::string etykieta, int idBudynku, int nrPietra, bool dostepne);

    int id() const;
    std::string etykieta() const;
    int idBudynku() const;
    int numerPietra() const;
    bool dostepne() const;
    bool zarezerwowane() const;
    QDate dataRezerwacji() const;

    void ustawDostepnosc(bool dostepne);
    void zarezerwuj(const QDate& data);
    void anulujRezerwacje();

private:
    int _id;
    std::string _etykieta;
    int _idBudynku;
    int _nrPietra;
    bool _dostepne;
    bool _zarezerwowane = false;
    QDate _dataRezerwacji;
};

#endif // BIURKO_H