#include "widok_rezerwacji.h"
#include <qt6/QtWidgets/QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    WidokRezerwacji okno;
    okno.show();

    return QApplication::exec();
}
