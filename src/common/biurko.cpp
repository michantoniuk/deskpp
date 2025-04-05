#include "biurko.h"
#include <utility>

Biurko::Biurko(const int id, std::string etykieta, int idBudynku, const int nrPietra)
    : _id(id), _etykieta(std::move(etykieta)), _idBudynku(idBudynku), _nrPietra(nrPietra), _dostepne(true),
      _zarezerwowane(false) {
}

Biurko::Biurko(const int id, std::string etykieta, int idBudynku, const int nrPietra, bool dostepne)
    : Biurko(id, std::move(etykieta), idBudynku, nrPietra) {
    _dostepne = dostepne;
}

int Biurko::id() const {
    return _id;
}

std::string Biurko::etykieta() const {
    return _etykieta;
}

int Biurko::idBudynku() const {
    return _idBudynku;
}

int Biurko::numerPietra() const {
    return _nrPietra;
}

bool Biurko::dostepne() const {
    return _dostepne;
}

bool Biurko::zarezerwowane() const {
    return _zarezerwowane;
}

QDate Biurko::dataRezerwacji() const {
    return _dataRezerwacji;
}

void Biurko::ustawDostepnosc(bool dostepne) {
    _dostepne = dostepne;
}

void Biurko::zarezerwuj(const QDate& data) {
    if (_dostepne) {
        _zarezerwowane = true;
        _dataRezerwacji = data;
    }
}

void Biurko::anulujRezerwacje() {
    _zarezerwowane = false;
    _dataRezerwacji = QDate();
}