#include "booking_view.h"
#include "booking_dialog.h"
#include "login_dialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QMenuBar>
#include <QMessageBox>
#include "common/logger.h"

BookingView::BookingView(QWidget *parent, ApiClient &apiClient)
    : QMainWindow(parent), apiClient(apiClient), selectedDate(QDate::currentDate()) {
    setupUi();
    setupMenus();
    setWindowTitle("DeskPP - Desk Booking System");

    // Connect signals
    connect(&apiClient, &ApiClient::networkError, this, &BookingView::handleNetworkError);
}

BookingView::BookingView(QWidget *parent)
    : BookingView(parent, *(new ApiClient(this))) {
}

void BookingView::setupUi() {
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
    connect(calendar, &QCalendarWidget::clicked, this, &BookingView::dateChanged);
    calendarLayout->addWidget(calendar);
    topLayout->addWidget(calendarPanel);

    // Options panel
    auto optionsPanel = new QGroupBox("Options", this);
    auto optionsLayout = new QVBoxLayout(optionsPanel);

    // Building selection
    auto bldgLayout = new QHBoxLayout();
    bldgLayout->addWidget(new QLabel("Building:", this));
    buildingSelect = new QComboBox(this);
    buildingSelect->setMinimumWidth(250);
    buildingSelect->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(buildingSelect, &QComboBox::currentIndexChanged, this, &BookingView::buildingChanged);
    bldgLayout->addWidget(buildingSelect);
    optionsLayout->addLayout(bldgLayout);

    // Floor selection
    auto floorLayout = new QHBoxLayout();
    floorLayout->addWidget(new QLabel("Floor:", this));
    floorSelect = new QComboBox(this);
    floorSelect->setMinimumWidth(250);
    floorSelect->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(floorSelect, &QComboBox::currentIndexChanged, this, &BookingView::floorChanged);
    floorLayout->addWidget(floorSelect);
    optionsLayout->addLayout(floorLayout);

    // Info labels
    infoLabel = new QLabel(QString("Desk plan for %1").arg(selectedDate.toString("MM/dd/yyyy")), this);
    optionsLayout->addWidget(infoLabel);

    // User label
    userLabel = new QLabel("Not logged in", this);
    optionsLayout->addWidget(userLabel);

    // Add refresh button
    refreshButton = new QPushButton("Refresh Desk Status", this);
    connect(refreshButton, &QPushButton::clicked, this, &BookingView::refreshView);
    optionsLayout->addWidget(refreshButton);

    topLayout->addWidget(optionsPanel);
    mainLayout->addLayout(topLayout);

    // Desk map panel
    auto mapPanel = new QGroupBox("Desk Map", this);
    auto mapLayout = new QVBoxLayout(mapPanel);

    // Scrollable desk map
    deskMapLayout = new QGridLayout();
    deskMapContainer = new QWidget(this);
    deskMapContainer->setLayout(deskMapLayout);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(deskMapContainer);

    mapLayout->addWidget(scrollArea);
    mainLayout->addWidget(mapPanel, 1);
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
}

bool BookingView::checkLogin(const QString &action) {
    if (!apiClient.isLoggedIn()) {
        if (!action.isEmpty()) {
            QMessageBox::information(this, "Login Required",
                                     QString("You need to log in to %1").arg(action));
        }
        showLoginDialog();
        return false;
    }
    return true;
}

void BookingView::dateChanged(const QDate &date) {
    selectedDate = date;
    infoLabel->setText(QString("Desk plan for %1").arg(date.toString("MM/dd/yyyy")));
    updateDeskMap();
}

void BookingView::buildingChanged(int index) {
    if (index >= 0 && index < buildingSelect->count()) {
        selectedBuildingId = buildingSelect->itemData(index).toInt();
        loadFloors(selectedBuildingId);

        // Only refresh view if a specific building is selected
        if (selectedBuildingId > 0) {
            refreshView();
        } else {
            // Clear desk display if "Select Building" is chosen
            desks.clear();
            updateDeskMap();
        }
    }
}

void BookingView::floorChanged(int index) {
    if (index >= 0 && index < floorSelect->count()) {
        selectedFloor = floorSelect->itemData(index).toInt();

        // Only refresh if both building and floor are selected
        if (selectedBuildingId > 0 && selectedFloor > 0) {
            LOG_INFO("Selected floor changed to {} for building {}", selectedFloor, selectedBuildingId);
            refreshView();
        } else {
            // Clear desk display if "Select Floor" is chosen
            desks.clear();
            updateDeskMap();
        }
    }
}

void BookingView::loadBuildings() {
    buildingSelect->clear();

    // Get buildings from server
    buildings = apiClient.getBuildings();

    // Add buildings to combo box
    buildingSelect->addItem("Select Building", -1);

    for (const auto &building: buildings) {
        QString displayText = QString::fromStdString(building.getName());
        if (!building.getAddress().empty()) {
            displayText += " (" + QString::fromStdString(building.getAddress()) + ")";
        }
        buildingSelect->addItem(displayText, building.getId());
    }
}

void BookingView::loadFloors(int buildingId) {
    floorSelect->clear();

    // Add "Select Floor" item at the top
    floorSelect->addItem("Select Floor", -1);

    // Skip if "Select Building" is selected
    if (buildingId <= 0) {
        selectedFloor = -1;
        return;
    }

    // Find selected building
    auto it = std::find_if(buildings.begin(), buildings.end(),
                           [buildingId](const Building &building) {
                               return building.getId() == buildingId;
                           });

    if (it != buildings.end()) {
        // Get numFloors from the building and create floor items
        int numFloors = it->getNumFloors();
        LOG_INFO("Loading floors for building {} - found {} floors", buildingId, numFloors);

        for (int floor = 1; floor <= numFloors; floor++) {
            floorSelect->addItem(QString("Floor %1").arg(floor), floor);
            LOG_INFO("Added Floor {} to dropdown", floor);
        }
    } else {
        LOG_WARNING("Building {} not found in local data", buildingId);
    }
}

void BookingView::refreshView() {
    // Update login status in UI
    if (apiClient.isLoggedIn() && apiClient.getCurrentUser()) {
        userLabel->setText(QString("Logged in as: %1")
            .arg(QString::fromStdString(apiClient.getCurrentUser()->getUsername())));
    } else {
        userLabel->setText("Not logged in");
    }

    // Load buildings if needed
    if (buildings.empty()) {
        loadBuildings();
    }

    // Always clear existing desks to get fresh data
    desks.clear();

    // Only try to load desks if user is logged in AND building/floor are selected
    if (apiClient.isLoggedIn() && selectedBuildingId > 0 && selectedFloor > 0) {
        LOG_INFO("Loading desks for building {} floor {}", selectedBuildingId, selectedFloor);
        desks = apiClient.getDesks(selectedBuildingId, selectedFloor);
        LOG_INFO("Loaded {} desks", desks.size());
    }

    // Update desk display
    updateDeskMap();
}

void BookingView::updateDeskMap() {
    // Clear existing items
    while (QLayoutItem *item = deskMapLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    // Check login status first
    if (!apiClient.isLoggedIn()) {
        auto loginButton = new QPushButton("Login to view desks", this);
        connect(loginButton, &QPushButton::clicked, this, &BookingView::showLoginDialog);
        deskMapLayout->addWidget(loginButton, 0, 0);
        return;
    }

    // If building or floor not selected, show prompt
    if (selectedBuildingId <= 0 || selectedFloor <= 0) {
        auto promptLabel = new QLabel("Please select a building and floor to view available desks", this);
        promptLabel->setAlignment(Qt::AlignCenter);
        promptLabel->setWordWrap(true);
        deskMapLayout->addWidget(promptLabel, 0, 0);
        return;
    }

    // No desks available for this combination
    if (desks.empty()) {
        auto noDesksLabel = new QLabel(QString("No desks found for Building ID %1, Floor %2")
                                       .arg(selectedBuildingId).arg(selectedFloor), this);
        noDesksLabel->setAlignment(Qt::AlignCenter);
        noDesksLabel->setWordWrap(true);
        deskMapLayout->addWidget(noDesksLabel, 0, 0);
        LOG_WARNING("No desks found for building {} floor {}", selectedBuildingId, selectedFloor);
        return;
    }

    // Display the desks
    int currentUserId = apiClient.getCurrentUser() ? apiClient.getCurrentUser()->getId() : -1;

    for (size_t i = 0; i < desks.size(); ++i) {
        const auto &desk = desks[i];
        bool isBooked = desk.isBookedOnDate(selectedDate);

        // Create button
        auto button = new QPushButton(this);
        button->setMinimumSize(100, 80);

        // Button with floor info
        QString deskName = QString::fromStdString(desk.getName());
        QString floorInfo = QString("Floor %1").arg(desk.getFloor());

        if (isBooked) {
            auto bookings = desk.getBookingsContainingDate(selectedDate);
            if (!bookings.empty()) {
                const auto &booking = bookings[0];
                int bookingUserId = booking.getUserId();

                if (currentUserId == bookingUserId) {
                    // Blue for user's own bookings
                    button->setStyleSheet("background-color: #2196F3; color: white;");
                    button->setText(deskName + "\n" + floorInfo + "\nBooked by you");
                } else {
                    // Red for bookings by others
                    button->setStyleSheet("background-color: #F44336; color: white;");
                    button->setText(deskName + "\n" + floorInfo + "\nBooked by User #" +
                                    QString::number(bookingUserId));
                }
            }
        } else {
            // Green for available desks
            button->setText(deskName + "\n" + floorInfo + "\nAvailable");
            button->setStyleSheet("background-color: #4CAF50; color: white;");
        }

        // Store desk index for click handler
        button->setProperty("index", static_cast<int>(i));
        button->setProperty("is_booked", isBooked);
        connect(button, &QPushButton::clicked, this, &BookingView::deskClicked);

        // Use default grid layout - 4 columns
        deskMapLayout->addWidget(button, i / 4, i % 4);
    }
}

void BookingView::deskClicked() {
    // Check login first
    if (!checkLogin("view desk details")) {
        return;
    }

    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    int deskIndex = button->property("index").toInt();
    if (deskIndex < 0 || deskIndex >= static_cast<int>(desks.size())) return;

    // Show booking dialog with current desk data
    BookingDialog dialog(desks[deskIndex], selectedDate, apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Refresh after booking action
        refreshView();
    }
}

void BookingView::showLoginDialog() {
    LoginDialog dialog(apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshView();
    }
}

void BookingView::handleUserLogout() {
    apiClient.logoutUser();
    refreshView();
}

void BookingView::handleNetworkError(const QString &error) {
    if (error.contains("connection") || error.contains("timeout") || error.contains("server")) {
        QMessageBox::warning(this, "Network Error", error);
    } else {
        LOG_ERROR("Network error: {}", error.toStdString());
    }
}
