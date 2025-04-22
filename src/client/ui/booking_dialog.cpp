#include "booking_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

#include "common/logger.h"

BookingDialog::BookingDialog(Desk &desk, const QDate &date, ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), desk(desk), bookingDate(date), apiClient(apiClient), bookingId(0) {
    setWindowTitle("Desk Booking");
    setMinimumWidth(300);

    auto layout = new QVBoxLayout(this);

    // Desk info with floor
    layout->addWidget(new QLabel(QString("Desk: %1 (Floor %2)")
        .arg(QString::fromStdString(desk.getName()))
        .arg(desk.getFloor())));

    // Determine if desk is booked
    isBooked = desk.isBookedOnDate(date);

    // Get current user ID
    int currentUserId = apiClient.getCurrentUser() ? apiClient.getCurrentUser()->getId() : -1;
    bool isOwnBooking = false;

    // Create booking info section
    auto bookingInfoGroup = new QGroupBox("Booking Information", this);
    auto bookingInfoLayout = new QVBoxLayout(bookingInfoGroup);

    // Find booking details if booked
    if (isBooked) {
        auto bookings = desk.getBookingsContainingDate(date);
        if (!bookings.empty()) {
            const auto &booking = bookings[0];
            bookingId = booking.getId();
            int bookingUserId = booking.getUserId();
            isOwnBooking = (currentUserId == bookingUserId);

            // Show booking period
            QString bookedFrom = QDate::fromString(
                QString::fromStdString(booking.getDateFromString()),
                "yyyy-MM-dd").toString("MM/dd/yyyy");
            QString bookedTo = QDate::fromString(
                QString::fromStdString(booking.getDateToString()),
                "yyyy-MM-dd").toString("MM/dd/yyyy");

            bookingInfoLayout->addWidget(new QLabel(QString("Status: Booked")));

            // Show if booking is by current user
            if (isOwnBooking) {
                bookingInfoLayout->addWidget(new QLabel("Booked by: You"));
            } else {
                bookingInfoLayout->addWidget(new QLabel(QString("Booked by: User #%1").arg(bookingUserId)));
            }

            bookingInfoLayout->addWidget(new QLabel(QString("From: %1").arg(bookedFrom)));
            bookingInfoLayout->addWidget(new QLabel(QString("To: %1").arg(bookedTo)));
        }
    } else {
        bookingInfoLayout->addWidget(new QLabel("Status: Available"));
    }

    layout->addWidget(bookingInfoGroup);

    // Date range for new booking (only shown when desk is available)
    auto dateGroup = new QGroupBox("New Booking", this);
    auto dateLayout = new QVBoxLayout(dateGroup);

    auto dateRangeLayout = new QHBoxLayout();

    dateRangeLayout->addWidget(new QLabel("From:"));
    dateFromEdit = new QDateEdit(date, this);
    dateFromEdit->setCalendarPopup(true);
    dateFromEdit->setMinimumDate(QDate::currentDate());
    dateRangeLayout->addWidget(dateFromEdit);

    dateRangeLayout->addWidget(new QLabel("To:"));
    dateToEdit = new QDateEdit(date, this);
    dateToEdit->setCalendarPopup(true);
    dateToEdit->setMinimumDate(date);
    dateRangeLayout->addWidget(dateToEdit);

    dateLayout->addLayout(dateRangeLayout);

    // Add note about availability
    QLabel *noteLabel = new QLabel("Select the date range for your new booking.");
    noteLabel->setWordWrap(true);
    dateLayout->addWidget(noteLabel);

    layout->addWidget(dateGroup);

    // Only show booking controls for available desks
    dateGroup->setVisible(!isBooked);

    // Buttons
    auto buttonLayout = new QHBoxLayout();

    bookButton = new QPushButton("Book", this);
    connect(bookButton, &QPushButton::clicked, this, &BookingDialog::bookDesk);
    buttonLayout->addWidget(bookButton);
    bookButton->setVisible(!isBooked);

    cancelButton = new QPushButton("Cancel Booking", this);
    connect(cancelButton, &QPushButton::clicked, this, &BookingDialog::cancelBooking);
    buttonLayout->addWidget(cancelButton);

    // Only enable cancel button if this is the user's own booking
    cancelButton->setVisible(isBooked && isOwnBooking);

    auto closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);
}

void BookingDialog::bookDesk() {
    QDate dateFrom = dateFromEdit->date();
    QDate dateTo = dateToEdit->date();

    // Validate dates
    if (dateFrom > dateTo) {
        QMessageBox::warning(this, "Invalid Dates", "End date must be after or equal to start date");
        return;
    }

    std::string dateFromStr = dateFrom.toString("yyyy-MM-dd").toStdString();
    std::string dateToStr = dateTo.toString("yyyy-MM-dd").toStdString();

    // Get the current user's ID
    if (!apiClient.isLoggedIn() || !apiClient.getCurrentUser()) {
        QMessageBox::warning(this, "Error", "You must be logged in to book a desk");
        return;
    }

    int userId = apiClient.getCurrentUser()->getId();
    int deskId = desk.getId();

    LOG_INFO("Booking desk {} for user {} from {} to {}",
             deskId, userId, dateFromStr, dateToStr);

    // Make the API call with error handling
    auto [success, errorMsg] = apiClient.addBooking(deskId, userId, dateFromStr, dateToStr);

    if (success) {
        LOG_INFO("Booking successful");
        accept();
    } else {
        LOG_ERROR("Booking failed: {}", errorMsg.toStdString());
        QMessageBox::warning(this, "Booking Error", errorMsg);
    }
}

void BookingDialog::cancelBooking() {
    if (bookingId > 0 && apiClient.cancelBooking(bookingId)) {
        desk.cancelBooking(bookingId);
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Could not cancel the booking");
    }
}
