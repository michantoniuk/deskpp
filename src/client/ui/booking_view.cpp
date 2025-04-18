#include "booking_view.h"
#include "booking_dialog.h"
#include "login_dialog.h"
#include "admin_dialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>

#include "common/logger.h"

BookingView::BookingView(QWidget *parent, ApiClient &apiClient)
    : QMainWindow(parent), apiClient(apiClient), selectedDate(QDate::currentDate()) {
    setupUi();
    setupMenus();
    setWindowTitle("DeskPP - Desk Booking System");
}

BookingView::BookingView(QWidget *parent)
    : BookingView(parent, *(new ApiClient(this))) {
}

void BookingView::setupUi() {
    // Main widget and layout
    auto central = new QWidget(this);
    setCentralWidget(central);
    auto mainLayout = new QVBoxLayout(central);

    // Top section with calendar and options
    auto topLayout = new QHBoxLayout();

    // Calendar panel
    auto calendarPanel = new QGroupBox("Select Date", this);
    auto calendarLayout = new QVBoxLayout(calendarPanel);
    calendar = new QCalendarWidget(this);
    calendar->setSelectedDate(selectedDate);
    calendarLayout->addWidget(calendar);
    connect(calendar, &QCalendarWidget::clicked, this, &BookingView::dateChanged);
    topLayout->addWidget(calendarPanel);

    // Options panel
    auto optionsPanel = new QGroupBox("Options", this);
    auto optionsLayout = new QVBoxLayout(optionsPanel);

    // Building selection
    auto bldgLayout = new QHBoxLayout();
    bldgLayout->addWidget(new QLabel("Building:", this));
    buildingSelect = new QComboBox(this);
    buildingSelect->addItem("Krakow A");
    buildingSelect->addItem("Warsaw B");
    connect(buildingSelect, &QComboBox::currentIndexChanged, this, &BookingView::buildingChanged);
    bldgLayout->addWidget(buildingSelect);
    optionsLayout->addLayout(bldgLayout);

    // Info labels
    infoLabel = new QLabel(QString("Desk plan for %1").arg(selectedDate.toString("MM/dd/yyyy")), this);
    optionsLayout->addWidget(infoLabel);

    // User label - display login status
    userLabel = new QLabel(this);
    updateLoginStatus(); // Set initial value
    optionsLayout->addWidget(userLabel);

    topLayout->addWidget(optionsPanel);
    mainLayout->addLayout(topLayout);

    // Desk map panel
    auto mapPanel = new QGroupBox("Desk Map", this);
    auto mapLayout = new QVBoxLayout(mapPanel);

    auto refreshBtn = new QPushButton("Refresh", this);
    connect(refreshBtn, &QPushButton::clicked, this, &BookingView::refreshView);
    mapLayout->addWidget(refreshBtn);

    // Scrollable desk map
    deskMapLayout = new QGridLayout();
    deskMapContainer = new QWidget(this);
    deskMapContainer->setLayout(deskMapLayout);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(deskMapContainer);

    mapLayout->addWidget(scrollArea);
    mainLayout->addWidget(mapPanel, 1);

    // Status bar
    statusBar()->showMessage("Welcome to DeskPP");
}

void BookingView::setupMenus() {
    // User menu
    auto userMenu = menuBar()->addMenu("User");

    auto loginAction = new QAction("Login", this);
    connect(loginAction, &QAction::triggered, this, &BookingView::showLoginDialog);
    userMenu->addAction(loginAction);

    auto logoutAction = new QAction("Logout", this);
    connect(logoutAction, &QAction::triggered, this, &BookingView::handleUserLogout);
    userMenu->addAction(logoutAction);

    // Admin menu
    auto adminMenu = menuBar()->addMenu("Admin");

    auto adminModeAction = new QAction("Enable Admin Mode", this);
    adminModeAction->setCheckable(true);
    connect(adminModeAction, &QAction::triggered, this, [this](bool checked) {
        apiClient.setAdminMode(checked);
    });
    adminMenu->addAction(adminModeAction);

    auto adminPanelAction = new QAction("Admin Panel", this);
    connect(adminPanelAction, &QAction::triggered, this, &BookingView::showAdminDialog);
    adminMenu->addAction(adminPanelAction);
}

void BookingView::updateLoginStatus() {
    if (apiClient.isLoggedIn() && apiClient.getCurrentUser()) {
        userLabel->setText(QString("Logged in as: %1")
            .arg(QString::fromStdString(apiClient.getCurrentUser()->getUsername())));
    } else {
        userLabel->setText("Not logged in");
    }
}

void BookingView::buildingChanged(int index) {
    selectedBuildingId = index + 1;
    refreshView();
}

void BookingView::dateChanged(const QDate &date) {
    selectedDate = date;
    infoLabel->setText(QString("Desk plan for %1").arg(date.toString("MM/dd/yyyy")));
    updateDeskMap();
}

void BookingView::refreshView() {
    // Verify login status
    if (!apiClient.isLoggedIn()) {
        showLoginDialog();
        return;
    }

    updateLoginStatus();
    desks = apiClient.getDesks(selectedBuildingId);
    updateDeskMap();
    statusBar()->showMessage(QString("Loaded %1 desks").arg(desks.size()), 3000);
}

void BookingView::updateDeskMap() {
    // Clear existing items
    while (QLayoutItem *item = deskMapLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    // Add desk buttons to grid
    for (size_t i = 0; i < desks.size(); ++i) {
        const auto &desk = desks[i];
        bool isBooked = desk.isBookedOnDate(selectedDate);

        // Create button
        auto button = new QPushButton(this);
        button->setMinimumSize(100, 80);

        if (isBooked) {
            auto bookings = desk.getBookingsContainingDate(selectedDate);
            if (!bookings.empty()) {
                const auto &booking = bookings[0];

                // Get end date of booking
                QString endDate = QDate::fromString(
                    QString::fromStdString(booking.getDateToString()),
                    "yyyy-MM-dd").toString("MM/dd/yyyy");

                // Get current username if logged in
                std::string currentUsername = "";
                if (apiClient.isLoggedIn() && apiClient.getCurrentUser()) {
                    currentUsername = apiClient.getCurrentUser()->getUsername();
                }

                // For now, use a hardcoded check to see if it works
                if (currentUsername == "user1") {
                    // Blue for user's own bookings
                    button->setStyleSheet("background-color: #2196F3; color: white;");
                    button->setText(QString::fromStdString(desk.getDeskId()) +
                                    "\nBooked by you until " + endDate);
                } else {
                    // Red for bookings by others
                    button->setStyleSheet("background-color: #F44336; color: white;");
                    button->setText(QString::fromStdString(desk.getDeskId()) +
                                    "\nBooked until " + endDate);
                }
            }
        } else {
            // Green for available desks
            button->setText(QString::fromStdString(desk.getDeskId()) + "\nAvailable");
            button->setStyleSheet("background-color: #4CAF50; color: white;");
        }

        // Store desk index for click handler
        button->setProperty("index", static_cast<int>(i));
        connect(button, &QPushButton::clicked, this, &BookingView::deskClicked);

        // Add to grid - either use coordinates or default layout
        int x = desk.getLocationX();
        int y = desk.getLocationY();

        if (x > 0 || y > 0) {
            deskMapLayout->addWidget(button, y, x);
        } else {
            // Default grid layout - 4 columns
            deskMapLayout->addWidget(button, i / 4, i % 4);
        }
    }
}

void BookingView::deskClicked() {
    // Check login first
    if (!apiClient.isLoggedIn()) {
        QMessageBox::warning(this, "Login Required", "You must be logged in to book a desk");
        showLoginDialog();
        return;
    }

    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    int deskIndex = button->property("index").toInt();
    if (deskIndex < 0 || deskIndex >= static_cast<int>(desks.size())) return;

    // Show booking dialog
    BookingDialog dialog(desks[deskIndex], selectedDate, apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Refresh after booking
        refreshView();
    }
}

void BookingView::showLoginDialog() {
    LoginDialog dialog(apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        updateLoginStatus();
        refreshView();
    }
}

void BookingView::handleUserLogin() {
    updateLoginStatus();
    refreshView();
}

void BookingView::handleUserLogout() {
    apiClient.logoutUser();
    updateLoginStatus();

    // Show login dialog after logout
    showLoginDialog();
}

void BookingView::showAdminDialog() {
    if (!apiClient.isLoggedIn()) {
        QMessageBox::warning(this, "Login Required", "You must be logged in as admin to access this feature");
        showLoginDialog();
        return;
    }

    AdminDialog dialog(apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshView();
    }
}
