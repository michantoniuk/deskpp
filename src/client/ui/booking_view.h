#ifndef BOOKING_VIEW_H
#define BOOKING_VIEW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDate>
#include <QTimer>
#include <vector>
#include "common/model/model.h"
#include "../net/api_client.h"

class BookingView : public QMainWindow {
    Q_OBJECT

public:
    explicit BookingView(QWidget *parent = nullptr);

    BookingView(QWidget *parent, ApiClient &apiClient);

    ~BookingView() override;

private slots:
    void buildingChanged(int index);

    void dateChanged(const QDate &date);

    void deskClicked();

    void refreshView();

    // User-related slots
    void showLoginDialog();

    void handleUserLogin(const User &user);

    void handleUserLogout();

    void updateUserInterface();

private:
    void setupUi();

    void initializeMenus();

    void setupConnections();

    void checkServerConnection();

    void updateDeskMap();

    // Helper functions
    QPushButton *createDeskButton(const Desk &desk, int index);

    void formatDeskStatus(const Desk &desk, QString &text, QString &buttonStyle, int currentUserId);

    // UI components
    QWidget *centralWidget;
    QCalendarWidget *calendar;
    QComboBox *selectBuilding;
    QComboBox *selectFloor;
    QLabel *infoLabel;
    QLabel *userInfoLabel;
    QGridLayout *deskMapLayout;
    QWidget *deskMapContainer;

    // API client
    ApiClient &_apiClient;
    bool _ownsApiClient;

    // Data
    std::vector<Desk> _desks;
    std::vector<QPushButton *> _deskButtons;

    // Current selection
    int _selectedBuilding;
    int _selectedFloor;
    QDate _selectedDate;

    // Menu items
    QMenu *_userMenu;
    QAction *_loginAction;
    QAction *_logoutAction;
    QAction *_userProfileAction;
};

#endif // BOOKING_VIEW_H
