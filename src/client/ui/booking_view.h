#ifndef BOOKING_VIEW_H
#define BOOKING_VIEW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QDate>
#include <QPushButton>

#include "common/model/model.h"
#include "../net/api_client.h"

class BookingView : public QMainWindow {
    Q_OBJECT

public:
    explicit BookingView(QWidget *parent = nullptr);

    BookingView(QWidget *parent, ApiClient &apiClient);

    void refreshView();

private slots:
    void buildingChanged(int index);

    void dateChanged(const QDate &date);

    void deskClicked();

    void showLoginDialog();

    void handleUserLogout();

    void handleNetworkError(const QString &error);

private:
    void setupUi();

    void setupMenus();

    void updateDeskMap();

    void loadBuildings();

    bool checkLogin(const QString &action = QString());

    // UI components
    QCalendarWidget *calendar;
    QComboBox *buildingSelect;
    QComboBox *floorSelect;
    QLabel *infoLabel;
    QLabel *userLabel;
    QPushButton *refreshButton;
    QGridLayout *deskMapLayout;
    QWidget *deskMapContainer;

    // Data
    ApiClient &apiClient;
    std::vector<Building> buildings;
    std::vector<Desk> desks;

    // State
    int selectedBuildingId = -1;
    int selectedFloor = -1;
    QDate selectedDate;

    void loadFloors(int buildingId);

    void floorChanged(int index);
};

#endif
