#include "booking_view.h"
#include "booking_dialog.h"
#include "login_dialog.h"
#include "common/logger.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QStatusBar>
#include <algorithm>

BookingView::BookingView(QWidget *parent, ApiClient &apiClient)
    : QMainWindow(parent),
      _apiClient(apiClient),
      _ownsApiClient(false),
      _selectedBuilding(1),
      _selectedFloor(1),
      _selectedDate(QDate::currentDate()) {
    setupUi();
    initializeMenus();
    setupConnections();

    setWindowTitle("DeskPP - Desk Booking System");
    resize(800, 600);

    // Show login dialog automatically on startup if not logged in
    QTimer::singleShot(500, this, &BookingView::showLoginDialog);

    checkServerConnection();
}

// Default constructor - creates its own ApiClient instance
BookingView::BookingView(QWidget *parent)
    : BookingView(parent, *(new ApiClient(this))) {
    _ownsApiClient = true; // This constructor owns the API client
    LOG_INFO("BookingView created with default ApiClient (localhost:8080)");
}

// Destructor to clean up owned resources
BookingView::~BookingView() {
    if (_ownsApiClient) {
        delete &_apiClient;
    }
}

void BookingView::setupUi() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto mainLayout = new QVBoxLayout(centralWidget);

    // Setup top section with calendar and options
    auto topLayout = new QHBoxLayout();

    // Calendar panel
    auto calendarPanel = new QGroupBox("Select Date", this);
    auto calendarLayout = new QVBoxLayout(calendarPanel);
    calendar = new QCalendarWidget(this);
    calendarLayout->addWidget(calendar);
    topLayout->addWidget(calendarPanel);

    // Options panel
    auto optionsPanel = new QGroupBox("Selection Options", this);
    auto optionsLayout = new QVBoxLayout(optionsPanel);

    // Building selection
    auto buildingLayout = new QHBoxLayout();
    buildingLayout->addWidget(new QLabel("Building:", this));
    selectBuilding = new QComboBox(this);
    buildingLayout->addWidget(selectBuilding);
    optionsLayout->addLayout(buildingLayout);

    // Floor selection
    auto floorLayout = new QHBoxLayout();
    floorLayout->addWidget(new QLabel("Floor:", this));
    selectFloor = new QComboBox(this);
    floorLayout->addWidget(selectFloor);
    optionsLayout->addLayout(floorLayout);

    // Info labels
    infoLabel = new QLabel(QString("Office plan for %1").arg(QDate::currentDate().toString("MM/dd/yyyy")), this);
    infoLabel->setAlignment(Qt::AlignCenter);
    optionsLayout->addWidget(infoLabel);

    userInfoLabel = new QLabel("Not logged in", this);
    userInfoLabel->setAlignment(Qt::AlignCenter);
    userInfoLabel->setStyleSheet("font-weight: bold; color: blue;");
    optionsLayout->addWidget(userInfoLabel);

    // Populate combo boxes
    selectBuilding->addItem("Krakow A");
    selectBuilding->addItem("Warsaw B");
    selectFloor->addItem("1st floor");

    topLayout->addWidget(optionsPanel);
    mainLayout->addLayout(topLayout);

    // Desk map panel
    auto mapPanel = new QGroupBox("Desk Map", this);
    auto mapLayout = new QVBoxLayout(mapPanel);

    auto refreshButton = new QPushButton("Refresh Data", this);
    connect(refreshButton, &QPushButton::clicked, this, &BookingView::refreshView);
    mapLayout->addWidget(refreshButton);

    // Desk map grid
    deskMapLayout = new QGridLayout();
    deskMapContainer = new QWidget(this);
    deskMapContainer->setLayout(deskMapLayout);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(deskMapContainer);

    mapLayout->addWidget(scrollArea);
    mainLayout->addWidget(mapPanel, 1); // Set stretch so map takes more space
}

void BookingView::initializeMenus() {
    // Create user menu
    _userMenu = menuBar()->addMenu("User");

    // Login action
    _loginAction = new QAction("Login", this);
    connect(_loginAction, &QAction::triggered, this, &BookingView::showLoginDialog);
    _userMenu->addAction(_loginAction);

    // Logout action
    _logoutAction = new QAction("Logout", this);
    connect(_logoutAction, &QAction::triggered, this, &BookingView::handleUserLogout);
    _userMenu->addAction(_logoutAction);

    // User profile action
    _userProfileAction = new QAction("My Profile", this);
    _userMenu->addAction(_userProfileAction);

    // Create admin menu
    _adminMenu = menuBar()->addMenu("Admin");

    // Admin mode action
    _adminModeAction = new QAction("Enable Admin Mode", this);
    _adminModeAction->setCheckable(true);
    connect(_adminModeAction, &QAction::triggered, this, [this](bool checked) {
        _apiClient.setAdminMode(checked);
        _adminPanelAction->setEnabled(checked);

        if (checked) {
            statusBar()->showMessage("Admin mode enabled", 3000);
        } else {
            statusBar()->showMessage("Admin mode disabled", 3000);
        }
    });
    _adminMenu->addAction(_adminModeAction);

    // Admin panel action
    _adminPanelAction = new QAction("Admin Panel", this);
    connect(_adminPanelAction, &QAction::triggered, this, &BookingView::showAdminDialog);
    _adminMenu->addAction(_adminPanelAction);

    // Initially disable admin panel until admin mode is enabled
    _adminPanelAction->setEnabled(false);

    // Initially set visibility based on login state
    updateUserInterface();
}

void BookingView::setupConnections() {
    connect(calendar, &QCalendarWidget::clicked, this, &BookingView::dateChanged);
    connect(selectBuilding, &QComboBox::currentIndexChanged, this, &BookingView::buildingChanged);
}

void BookingView::checkServerConnection() {
    if (_apiClient.isConnected()) {
        statusBar()->showMessage("Connected to server. Select a desk to view details or make a booking", 5000);
        buildingChanged(0); // Select first building to load
    } else {
        QString errorMsg = "Cannot connect to server. Check if the server is running.";
        statusBar()->showMessage(errorMsg, 0); // Show message until canceled
        QMessageBox::critical(this, "Connection Error",
                              errorMsg + "\nApplication will run in offline mode with limited functionality.");
    }
}

void BookingView::buildingChanged(int index) {
    _selectedBuilding = index + 1;

    try {
        if (!_apiClient.isConnected() && !_apiClient.testConnection()) {
            statusBar()->showMessage("Cannot connect to server. Working in offline mode.", 5000);
            QMessageBox::warning(this, "Communication Error", "Cannot connect to server.");
            return;
        }

        _desks = _apiClient.getDesks(_selectedBuilding);
        updateDeskMap();
        statusBar()->showMessage(QString("Loaded %1 desks from server").arg(_desks.size()), 3000);
    } catch (const std::exception &e) {
        QString errorMsg = QString("Error while retrieving data from server: %1").arg(e.what());
        statusBar()->showMessage(errorMsg, 5000);
        QMessageBox::warning(this, "Communication Error", errorMsg);
        LOG_ERROR("Error: {}", e.what());
    }
}

void BookingView::dateChanged(const QDate &date) {
    _selectedDate = date;
    infoLabel->setText(QString("Office plan for %1").arg(date.toString("MM/dd/yyyy")));
    refreshView();
}

void BookingView::deskClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    bool ok;
    int deskIndex = button->property("index").toInt(&ok);
    if (!ok || deskIndex < 0 || deskIndex >= static_cast<int>(_desks.size())) return;

    if (!_apiClient.isConnected() && !_apiClient.testConnection()) {
        QMessageBox::warning(this, "No Connection",
                             "Cannot perform booking operations without server connection.");
        return;
    }

    // Check if user is logged in
    if (!_apiClient.isLoggedIn()) {
        QMessageBox::warning(this, "Login Required",
                             "You must be logged in to book a desk.");
        showLoginDialog();
        return;
    }

    BookingDialog dialog(_desks[deskIndex], _selectedDate, _apiClient, this);
    dialog.exec();
    refreshView();
}

void BookingView::refreshView() {
    if (!_apiClient.isConnected()) {
        if (_apiClient.testConnection()) {
            statusBar()->showMessage("Connected to server. Refreshing data...", 3000);
        } else {
            statusBar()->showMessage("Cannot connect to server. Working in offline mode.", 5000);
            return;
        }
    }

    try {
        _desks = _apiClient.getDesks(_selectedBuilding);
        updateDeskMap();
        statusBar()->showMessage("Data refreshed successfully", 3000);
    } catch (const std::exception &e) {
        QString errorMsg = QString("Error while refreshing data: %1").arg(e.what());
        statusBar()->showMessage(errorMsg, 5000);
        QMessageBox::warning(this, "Communication Error", errorMsg);
    }
}

void BookingView::showLoginDialog() {
    // Skip if already logged in
    if (_apiClient.isLoggedIn()) {
        return;
    }

    LoginDialog loginDialog(_apiClient, this);
    connect(&loginDialog, &LoginDialog::userLoggedIn, this, &BookingView::handleUserLogin);

    // Show the dialog as modal
    if (loginDialog.exec() == QDialog::Accepted) {
        LOG_INFO("Login dialog accepted");
    } else {
        LOG_INFO("Login dialog canceled");
    }
}

void BookingView::handleUserLogin(const User &user) {
    LOG_INFO("User logged in: id={}, username={}", user.getId(), user.getUsername());

    // Update the UI to reflect logged-in state
    updateUserInterface();

    // Refresh the view to show user-specific bookings
    refreshView();

    // Show welcome message
    statusBar()->showMessage(QString("Welcome, %1!").arg(QString::fromStdString(user.getFullName())), 5000);
}

void BookingView::handleUserLogout() {
    LOG_INFO("User logged out");

    // Logout the user
    _apiClient.logoutUser();

    // Update the UI to reflect logged-out state
    updateUserInterface();

    // Refresh the view
    refreshView();

    // Show login dialog
    QTimer::singleShot(500, this, &BookingView::showLoginDialog);

    // Show message
    statusBar()->showMessage("You have been logged out", 5000);
}

void BookingView::updateUserInterface() {
    bool isLoggedIn = _apiClient.isLoggedIn();

    // Update menu items
    _loginAction->setVisible(!isLoggedIn);
    _logoutAction->setVisible(isLoggedIn);
    _userProfileAction->setVisible(isLoggedIn);

    // Update user info label
    if (isLoggedIn) {
        auto user = _apiClient.getCurrentUser();
        if (user) {
            userInfoLabel->setText(QString("Logged in as: %1").arg(QString::fromStdString(user->getUsername())));
        }
    } else {
        userInfoLabel->setText("Not logged in");
    }

    // Show admin menu only for logged-in users
    _adminMenu->menuAction()->setVisible(isLoggedIn);

    // Reset admin mode when logging out
    if (!isLoggedIn) {
        _apiClient.setAdminMode(false);
        _adminModeAction->setChecked(false);
    }
}

void BookingView::updateDeskMap() {
    // Clear existing desk buttons
    while (QLayoutItem *item = deskMapLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }
    _deskButtons.clear();

    // Set up grid properties
    deskMapLayout->setSpacing(10);
    deskMapLayout->setContentsMargins(10, 10, 10, 10);

    LOG_INFO("Updating desk map with {} desks", _desks.size());

    // Check if we have any desks with non-zero coordinates
    bool useGridLayout = false;
    for (const auto &desk: _desks) {
        if (desk.getLocationX() > 0 || desk.getLocationY() > 0) {
            useGridLayout = true;
            LOG_INFO("Using grid layout because desk {} has coordinates ({},{})",
                     desk.getDeskId(), desk.getLocationX(), desk.getLocationY());
            break;
        }
    }

    if (useGridLayout) {
        // Grid layout based on coordinates
        LOG_INFO("Using grid layout for desk positioning");

        for (size_t i = 0; i < _desks.size(); ++i) {
            const auto &desk = _desks[i];
            auto button = createDeskButton(desk, i);

            // Use coordinates for positioning (default to auto-placement if both are zero)
            int x = desk.getLocationX();
            int y = desk.getLocationY();

            if (x == 0 && y == 0) {
                // Auto-assign position for desks with zero coordinates
                x = i % 3; // 3 columns
                y = i / 3;
                LOG_INFO("Auto-placing desk {} at position ({},{})", desk.getDeskId(), y, x);
            } else {
                LOG_INFO("Placing desk {} at coordinates ({},{})", desk.getDeskId(), y, x);
            }

            deskMapLayout->addWidget(button, y, x);
            _deskButtons.push_back(button);
        }
    } else {
        // Standard column layout as fallback
        LOG_INFO("Using column layout for desk positioning");
        const int numColumns = 4;

        for (size_t i = 0; i < _desks.size(); ++i) {
            const auto &desk = _desks[i];
            auto button = createDeskButton(desk, i);

            int row = i / numColumns;
            int col = i % numColumns;

            LOG_INFO("Placing desk {} at position ({},{})", desk.getDeskId(), row, col);
            deskMapLayout->addWidget(button, row, col);
            _deskButtons.push_back(button);
        }
    }
}

QPushButton *BookingView::createDeskButton(const Desk &desk, int index) {
    auto button = new QPushButton(QString::fromStdString(desk.getDeskId()), this);
    button->setMinimumSize(100, 80);
    button->setMaximumSize(150, 120);

    // Style and text
    QString buttonStyle;
    QString text = QString::fromStdString(desk.getDeskId());

    // Get current user ID (if logged in)
    int currentUserId = -1;
    if (_apiClient.isLoggedIn() && _apiClient.getCurrentUser()) {
        currentUserId = _apiClient.getCurrentUser()->getId();
    }

    // Format the desk status
    formatDeskStatus(desk, text, buttonStyle, currentUserId);

    button->setStyleSheet(buttonStyle);
    button->setText(text);

    button->setProperty("index", static_cast<int>(index));
    connect(button, &QPushButton::clicked, this, &BookingView::deskClicked);

    return button;
}

void BookingView::formatDeskStatus(const Desk &desk, QString &text, QString &buttonStyle, int currentUserId) {
    // Get all bookings sorted by date
    auto allBookings = desk.getBookings();

    // Sort bookings by start date
    std::sort(allBookings.begin(), allBookings.end(),
              [](const Booking &a, const Booking &b) {
                  return a.getDateFrom() < b.getDateFrom();
              }
    );

    // Check if the desk is booked specifically on the selected date
    if (desk.isBookedOnDate(_selectedDate)) {
        // Desk is booked for the selected date - show as unavailable
        buttonStyle = "background-color: #d7ccc8; color: black;"; // Light brown for booked

        // Get booking that contains this date
        auto bookingsOnDate = desk.getBookingsContainingDate(_selectedDate);
        if (!bookingsOnDate.empty()) {
            const Booking &booking = bookingsOnDate[0];

            // Check if this is the user's own booking
            if (booking.getUserId() == currentUserId) {
                buttonStyle = "background-color: #bbdefb; color: black;"; // Light blue for own booking
                text += "\nYours on";
            } else {
                text += "\nBooked";
            }

            if (booking.getDateFrom() != booking.getDateTo()) {
                text += " " + booking.getDateFrom().toString("MM/dd") +
                        "-" + booking.getDateTo().toString("MM/dd");
            } else {
                text += " " + booking.getDateFrom().toString("MM/dd");
            }
        }
    } else {
        // Desk is available for the selected date - show as available
        buttonStyle = "background-color: #a5d6a7; color: black;"; // Green for available

        // Find the next upcoming booking after the selected date
        QDate nextBookingDate;
        QDate nextBookingEndDate;

        for (const auto &booking: allBookings) {
            if (booking.getDateFrom() >= _selectedDate) {
                // This is a future booking
                if (nextBookingDate.isNull() || booking.getDateFrom() < nextBookingDate) {
                    nextBookingDate = booking.getDateFrom();
                    nextBookingEndDate = booking.getDateTo();
                }
            }
        }

        // Add information about the next booking if it exists
        if (!nextBookingDate.isNull()) {
            if (nextBookingDate == _selectedDate.addDays(1)) {
                // If the next booking is tomorrow, show it as "Tomorrow"
                text += "\nBooked tomorrow";
            } else {
                // Otherwise show the date
                text += "\nNext booking:";
                if (nextBookingDate == nextBookingEndDate) {
                    text += " " + nextBookingDate.toString("MM/dd");
                } else {
                    text += " " + nextBookingDate.toString("MM/dd") +
                            "-" + nextBookingEndDate.toString("MM/dd");
                }
            }
        }
    }
}

void BookingView::showAdminDialog() {
    if (!_apiClient.isAdmin()) {
        QMessageBox::warning(this, "Access Denied", "Admin mode must be enabled to access admin panel.");
        return;
    }

    AdminDialog adminDialog(_apiClient, this);
    adminDialog.exec();

    // Refresh the view after admin operations
    refreshView();
}
