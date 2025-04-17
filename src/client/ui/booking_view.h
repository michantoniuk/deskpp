#ifndef BOOKING_VIEW_H
#define BOOKING_VIEW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QDate>
#include "common/model/model.h"
#include "../net/api_client.h"

class BookingView : public QMainWindow {
    Q_OBJECT

public:
    explicit BookingView(QWidget *parent = nullptr);

    BookingView(QWidget *parent, ApiClient &apiClient);

private slots:
    void buildingChanged(int index);

    void dateChanged(const QDate &date);

    void deskClicked();

    void refreshView();

    // User actions
    void showLoginDialog();

    void handleUserLogin();

    void handleUserLogout();

    void showAdminDialog();

private:
    void setupUi();

    void setupMenus();

    void updateDeskMap();

    // UI components
    QCalendarWidget *calendar;
    QComboBox *buildingSelect;
    QLabel *infoLabel;
    QLabel *userLabel;
    QGridLayout *deskMapLayout;
    QWidget *deskMapContainer;

    // Data
    ApiClient &apiClient;
    std::vector<Desk> desks;

    // State
    int selectedBuildingId = 1;
    QDate selectedDate;
};

#endif // BOOKING_VIEW_H
