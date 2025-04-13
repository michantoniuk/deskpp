#include "booking_view.h"
#include "booking_dialog.h"
#include "../net/client_communication.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QStatusBar>
#include <iostream>
#include "../util/logger.h"

// Default constructor - creates its own communication instance
BookingView::BookingView(QWidget *parent)
    : QMainWindow(parent),
      centralWidget(new QWidget(this)),
      calendar(new QCalendarWidget(this)),
      selectBuilding(new QComboBox(this)),
      selectFloor(new QComboBox(this)),
      infoLabel(new QLabel(this)),
      deskMapLayout(new QGridLayout()),
      deskMapContainer(new QWidget(this)),
      communication(*(new ClientCommunication())), // Create a new instance on the heap
      ownsCommunication(true), // This constructor owns the communication instance
      selectedBuilding(1),
      selectedFloor(1),
      selectedDate(QDate::currentDate()) {
    // This constructor will create a communication instance with default settings (localhost:8080)
    LOG_INFO("BookingView created with default communication (localhost:8080)");

    setCentralWidget(centralWidget);
    initializeUI();

    connect(calendar, &QCalendarWidget::clicked, this, &BookingView::dateChanged);
    connect(selectBuilding, &QComboBox::currentIndexChanged, this, &BookingView::buildingChanged);

    setWindowTitle("DeskPP - Desk Booking System");
    resize(800, 600);

    if (communication.isConnected()) {
        statusBar()->showMessage("Connected to server. Select a desk to view details or make a booking", 5000);
        buildingChanged(0); // Select first building to load
    } else {
        QString errorMsg = "Cannot connect to server. Check if the server is running.";
        statusBar()->showMessage(errorMsg, 0); // Show message until canceled
        QMessageBox::critical(this, "Connection Error",
                              errorMsg + "\nApplication will run in offline mode with limited functionality.");
    }
}

// Constructor that accepts an existing ClientCommunication instance
BookingView::BookingView(QWidget *parent, ClientCommunication &comm)
    : QMainWindow(parent),
      centralWidget(new QWidget(this)),
      calendar(new QCalendarWidget(this)),
      selectBuilding(new QComboBox(this)),
      selectFloor(new QComboBox(this)),
      infoLabel(new QLabel(this)),
      deskMapLayout(new QGridLayout()),
      deskMapContainer(new QWidget(this)),
      communication(comm), // Use the passed instance
      ownsCommunication(false), // This constructor doesn't own the communication instance
      selectedBuilding(1),
      selectedFloor(1),
      selectedDate(QDate::currentDate()) {
    setCentralWidget(centralWidget);
    initializeUI();

    connect(calendar, &QCalendarWidget::clicked, this, &BookingView::dateChanged);
    connect(selectBuilding, &QComboBox::currentIndexChanged, this, &BookingView::buildingChanged);

    setWindowTitle("DeskPP - Desk Booking System");
    resize(800, 600);

    if (communication.isConnected()) {
        statusBar()->showMessage("Connected to server. Select a desk to view details or make a booking", 5000);
        buildingChanged(0); // Select first building to load
    } else {
        QString errorMsg = "Cannot connect to server. Check if the server is running.";
        statusBar()->showMessage(errorMsg, 0); // Show message until canceled
        QMessageBox::critical(this, "Connection Error",
                              errorMsg + "\nApplication will run in offline mode with limited functionality.");
    }
}

// Destructor to clean up owned resources
BookingView::~BookingView() {
    // Clean up the communication instance if we own it
    if (ownsCommunication) {
        delete &communication;
    }
}

void BookingView::buildingChanged(int index) {
    selectedBuilding = index + 1;

    try {
        if (!communication.isConnected() && !communication.testConnection()) {
            statusBar()->showMessage("Cannot connect to server. Working in offline mode.", 5000);
            QMessageBox::warning(this, "Communication Error", "Cannot connect to server.");
            return;
        }

        desks = getDesksFromServer(selectedBuilding);
        updateDeskMap();
        statusBar()->showMessage(QString("Loaded %1 desks from server").arg(desks.size()), 3000);
    } catch (const std::exception &e) {
        QString errorMsg = QString("Error while retrieving data from server: %1").arg(e.what());
        statusBar()->showMessage(errorMsg, 5000);
        QMessageBox::warning(this, "Communication Error", errorMsg);
        LOG_ERROR("Error: {}", e.what());
    }
}

void BookingView::dateChanged(const QDate &date) {
    selectedDate = date;
    infoLabel->setText(QString("Office plan for %1").arg(date.toString("MM/dd/yyyy")));
    refreshView();
}

void BookingView::deskClicked() {
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    bool ok;
    int deskIndex = button->property("index").toInt(&ok);
    if (!ok || deskIndex < 0 || deskIndex >= static_cast<int>(desks.size())) return;

    if (!communication.isConnected() && !communication.testConnection()) {
        QMessageBox::warning(this, "No Connection",
                             "Cannot perform booking operations without server connection.");
        return;
    }

    BookingDialog dialog(desks[deskIndex], selectedDate, communication, this);
    dialog.exec();
    refreshView();
}

void BookingView::refreshView() {
    if (!communication.isConnected()) {
        if (communication.testConnection()) {
            statusBar()->showMessage("Connected to server. Refreshing data...", 3000);
        } else {
            statusBar()->showMessage("Cannot connect to server. Working in offline mode.", 5000);
            return;
        }
    }

    try {
        desks = getDesksFromServer(selectedBuilding);
        updateDeskMap();
        statusBar()->showMessage("Data refreshed successfully", 3000);
    } catch (const std::exception &e) {
        QString errorMsg = QString("Error while refreshing data: %1").arg(e.what());
        statusBar()->showMessage(errorMsg, 5000);
        QMessageBox::warning(this, "Communication Error", errorMsg);
    }
}

void BookingView::initializeUI() {
    auto mainLayout = new QVBoxLayout(centralWidget);
    auto topLayout = new QHBoxLayout();
    auto calendarPanel = new QGroupBox("Select Date", this);
    auto calendarLayout = new QVBoxLayout(calendarPanel);
    calendarLayout->addWidget(calendar);
    topLayout->addWidget(calendarPanel);

    auto optionsPanel = new QGroupBox("Selection Options", this);
    auto optionsLayout = new QVBoxLayout(optionsPanel);

    auto buildingLayout = new QHBoxLayout();
    buildingLayout->addWidget(new QLabel("Building:", this));
    buildingLayout->addWidget(selectBuilding);
    optionsLayout->addLayout(buildingLayout);

    auto floorLayout = new QHBoxLayout();
    floorLayout->addWidget(new QLabel("Floor:", this));
    floorLayout->addWidget(selectFloor);
    optionsLayout->addLayout(floorLayout);

    infoLabel = new QLabel(QString("Office plan for %1").arg(QDate::currentDate().toString("MM/dd/yyyy")), this);
    infoLabel->setAlignment(Qt::AlignCenter);
    optionsLayout->addWidget(infoLabel);

    selectBuilding->addItem("Krakow A");
    selectBuilding->addItem("Warsaw B");
    selectFloor->addItem("1st floor");

    topLayout->addWidget(optionsPanel);
    mainLayout->addLayout(topLayout);

    auto mapPanel = new QGroupBox("Desk Map", this);
    auto mapLayout = new QVBoxLayout(mapPanel);

    auto refreshButton = new QPushButton("Refresh Data", this);
    connect(refreshButton, &QPushButton::clicked, this, &BookingView::refreshView);
    mapLayout->addWidget(refreshButton);

    deskMapContainer->setLayout(deskMapLayout);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(deskMapContainer);

    mapLayout->addWidget(scrollArea);
    mainLayout->addWidget(mapPanel, 1); // Set stretch so map takes more space
}

std::vector<Desk> BookingView::getDesksFromServer(int buildingId) {
    LOG_INFO("Getting desks from server for building ID: {}", buildingId);
    std::vector<json> desksJson = communication.getDesks(buildingId);
    LOG_INFO("Received {} desks from server", desksJson.size());

    std::vector<Desk> desks;
    for (const auto &deskJson: desksJson) {
        LOG_INFO("Processing desk JSON: {}", deskJson.dump());

        int id = deskJson.contains("id") ? deskJson["id"].get<int>() : 0;

        // Read deskId which maps to the 'name' field in database
        std::string deskId = deskJson.contains("deskId") ? deskJson["deskId"].get<std::string>() : "Unknown";

        // Read buildingId
        std::string buildingId;
        if (deskJson.contains("buildingId")) {
            try {
                if (deskJson["buildingId"].is_string()) {
                    buildingId = deskJson["buildingId"].get<std::string>();
                } else if (deskJson["buildingId"].is_number()) {
                    // Convert number to string
                    buildingId = std::to_string(deskJson["buildingId"].get<int>());
                } else {
                    buildingId = "Unknown";
                }
            } catch (const std::exception &e) {
                LOG_ERROR("Error reading buildingId: {}", e.what());
                buildingId = "Unknown";
            }
        } else {
            buildingId = "Unknown";
        }

        int floorNumber = deskJson.contains("floorNumber") ? deskJson["floorNumber"].get<int>() : 0;

        // Create desk with the simplified constructor
        Desk desk(id, deskId, buildingId, floorNumber);

        // Handle multiple bookings if present in response
        if (deskJson.contains("bookings") && deskJson["bookings"].is_array()) {
            // Process multiple bookings
            for (const auto &bookingJson: deskJson["bookings"]) {
                try {
                    // Convert json object to string for parsing
                    std::string bookingStr = bookingJson.dump();
                    // Create booking from json
                    Booking booking = Booking::fromJson(bookingStr);

                    // Only add valid bookings (with valid dates)
                    if (booking.getDateFrom().isValid() && booking.getDateTo().isValid()) {
                        // Set the desk ID if not already set
                        if (booking.getDeskId() == 0) {
                            booking.setDeskId(id);
                        }
                        desk.addBooking(booking);
                    }
                } catch (const std::exception &e) {
                    LOG_ERROR("Error processing booking JSON: {}", e.what());
                }
            }
        } else if (deskJson.contains("booked") && deskJson["booked"].get<bool>()) {
            // Handle legacy single booking information
            int bookingId = 0;
            if (deskJson.contains("bookingId")) {
                bookingId = deskJson["bookingId"].get<int>();
            }

            QDate dateFrom, dateTo;

            if (deskJson.contains("bookingDateFrom") && !deskJson["bookingDateFrom"].is_null()) {
                std::string dateFromStr = deskJson["bookingDateFrom"].get<std::string>();
                dateFrom = QDate::fromString(QString::fromStdString(dateFromStr), "yyyy-MM-dd");
            } else if (deskJson.contains("bookingDate") && !deskJson["bookingDate"].is_null()) {
                // For compatibility with old format
                std::string dateStr = deskJson["bookingDate"].get<std::string>();
                dateFrom = QDate::fromString(QString::fromStdString(dateStr), "yyyy-MM-dd");
            } else {
                dateFrom = QDate::currentDate();
            }

            if (deskJson.contains("bookingDateTo") && !deskJson["bookingDateTo"].is_null()) {
                std::string dateToStr = deskJson["bookingDateTo"].get<std::string>();
                dateTo = QDate::fromString(QString::fromStdString(dateToStr), "yyyy-MM-dd");
            } else {
                dateTo = dateFrom; // If no end date, use start date
            }

            // Create a Booking object and add it to the desk
            if (dateFrom.isValid() && dateTo.isValid()) {
                Booking booking(bookingId, id, 0, dateFrom, dateTo);
                desk.addBooking(booking);
            }
        }

        desks.push_back(desk);
    }

    return desks;
}

void BookingView::updateDeskMap() {
    // Clear existing buttons
    for (auto button: deskButtons) {
        deskMapLayout->removeWidget(button);
        delete button;
    }
    deskButtons.clear();

    const int numColumns = 4;

    for (size_t i = 0; i < desks.size(); ++i) {
        const auto &desk = desks[i];

        auto button = new QPushButton(QString::fromStdString(desk.getDeskId()), this);
        button->setMinimumSize(100, 80);
        button->setMaximumSize(150, 120);

        QString buttonStyle;
        QString text = QString::fromStdString(desk.getDeskId());

        // Get all bookings sorted by date
        auto allBookings = desk.getBookings();

        // Sort bookings by start date
        std::sort(allBookings.begin(), allBookings.end(),
                  [](const Booking &a, const Booking &b) {
                      return a.getDateFrom() < b.getDateFrom();
                  }
        );

        // Check if the desk is booked specifically on the selected date
        if (desk.isBookedOnDate(selectedDate)) {
            // Desk is booked for the selected date - show as unavailable
            buttonStyle = "background-color: #d7ccc8; color: black;"; // Light brown for booked

            // Get booking that contains this date
            auto bookingsOnDate = desk.getBookingsContainingDate(selectedDate);
            if (!bookingsOnDate.empty()) {
                const Booking &booking = bookingsOnDate[0];
                text += "\nBooked";

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
                if (booking.getDateFrom() >= selectedDate) {
                    // This is a future booking
                    if (nextBookingDate.isNull() || booking.getDateFrom() < nextBookingDate) {
                        nextBookingDate = booking.getDateFrom();
                        nextBookingEndDate = booking.getDateTo();
                    }
                }
            }

            // Add information about the next booking if it exists
            if (!nextBookingDate.isNull()) {
                if (nextBookingDate == selectedDate.addDays(1)) {
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

        button->setStyleSheet(buttonStyle);
        button->setText(text);

        button->setProperty("index", static_cast<int>(i));
        connect(button, &QPushButton::clicked, this, &BookingView::deskClicked);

        int row = i / numColumns;
        int column = i % numColumns;
        deskMapLayout->addWidget(button, row, column);

        deskButtons.push_back(button);
    }
}
