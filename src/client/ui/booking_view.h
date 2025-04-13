#ifndef BOOKING_VIEW_H
#define BOOKING_VIEW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDate>
#include <vector>
#include "../model/desk.h"
#include "../net/client_communication.h"

class BookingView : public QMainWindow {
    Q_OBJECT

public:
    explicit BookingView(QWidget *parent = nullptr);

    BookingView(QWidget *parent, ClientCommunication &comm);

    ~BookingView() override;

private slots:
    void buildingChanged(int index);

    void dateChanged(const QDate &date);

    void deskClicked();

    void refreshView();

private:
    void initializeUI();

    void updateDeskMap();

    std::vector<Desk> getDesksFromServer(int buildingId);

    QWidget *centralWidget;
    QCalendarWidget *calendar;
    QComboBox *selectBuilding;
    QComboBox *selectFloor;
    QLabel *infoLabel;

    QGridLayout *deskMapLayout;
    QWidget *deskMapContainer;

    ClientCommunication &communication;
    bool ownsCommunication;

    std::vector<Desk> desks;
    std::vector<QPushButton *> deskButtons;

    int selectedBuilding;
    int selectedFloor;
    QDate selectedDate;
};

#endif // BOOKING_VIEW_H
