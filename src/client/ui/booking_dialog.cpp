#include "booking_dialog.h"
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include "../util/logger.h"

BookingDialog::BookingDialog(Desk &desk, const QDate &bookingDate, ApiClient &apiClient,
                             QWidget *parent)
    : QDialog(parent), _desk(desk), _bookingDate(bookingDate), _apiClient(apiClient), _bookingId(0) {
    setWindowTitle("Desk Booking");
    setMinimumWidth(400);

    _deskLabel = new QLabel(QString("Desk: %1").arg(QString::fromStdString(desk.getDeskId())), this);
    _dateLabel = new QLabel(QString("Date: %1").arg(bookingDate.toString("MM/dd/yyyy")), this);

    // Set status text based on booking status
    QString statusText;

    // Check for booking on the selected date
    bool isBookedOnDate = desk.isBookedOnDate(bookingDate);

    // Get ALL bookings for this desk and sort them by start date
    auto allBookings = desk.getBookings();
    std::sort(allBookings.begin(), allBookings.end(),
              [](const Booking &a, const Booking &b) {
                  return a.getDateFrom() < b.getDateFrom();
              }
    );

    if (isBookedOnDate) {
        // Desk is booked on this date - find the booking
        auto bookingsOnDate = desk.getBookingsContainingDate(bookingDate);
        if (!bookingsOnDate.empty()) {
            const auto &booking = bookingsOnDate[0];

            // Set booking ID for cancel operation
            _bookingId = booking.getId();

            if (booking.getDateFrom() == booking.getDateTo()) {
                // Single day booking
                statusText = "Status: Booked for selected date";
            } else {
                // Multi-day booking
                statusText = QString("Status: Booked as part of period %1 to %2")
                        .arg(booking.getDateFrom().toString("MM/dd/yyyy"),
                             booking.getDateTo().toString("MM/dd/yyyy"));
            }

            // Check if this booking is by the current user
            auto currentUser = apiClient.getCurrentUser();
            if (currentUser && booking.getUserId() == currentUser->getId()) {
                statusText += " (your booking)";
            }
        } else {
            // Fallback
            statusText = "Status: Booked for selected date";
        }
    } else {
        // Desk is available for this date
        statusText = "Status: Available for selected date";

        // Check if there are any upcoming bookings
        bool hasUpcomingBooking = false;
        for (const auto &booking: allBookings) {
            if (booking.getDateFrom() > bookingDate) {
                hasUpcomingBooking = true;
                statusText += QString("\nNote: Booked %1").arg(
                    booking.getDateFrom() == booking.getDateTo()
                        ? QString("on %1").arg(booking.getDateFrom().toString("MM/dd/yyyy"))
                        : QString("from %1 to %2").arg(
                            booking.getDateFrom().toString("MM/dd/yyyy"),
                            booking.getDateTo().toString("MM/dd/yyyy"))
                );
                break; // Just show the next booking
            }
        }
    }

    _statusLabel = new QLabel(statusText, this);

    // Add date range selection
    auto dateGroupBox = new QGroupBox("Booking Period", this);
    auto dateLayout = new QHBoxLayout(dateGroupBox);

    auto fromLabel = new QLabel("From:", this);
    _dateFromEdit = new QDateEdit(this);
    _dateFromEdit->setDate(bookingDate);
    _dateFromEdit->setCalendarPopup(true);
    _dateFromEdit->setMinimumDate(QDate::currentDate());

    auto toLabel = new QLabel("To:", this);
    _dateToEdit = new QDateEdit(this);
    _dateToEdit->setDate(bookingDate);
    _dateToEdit->setCalendarPopup(true);
    _dateToEdit->setMinimumDate(bookingDate);

    dateLayout->addWidget(fromLabel);
    dateLayout->addWidget(_dateFromEdit);
    dateLayout->addWidget(toLabel);
    dateLayout->addWidget(_dateToEdit);

    connect(_dateFromEdit, &QDateEdit::dateChanged, this, &BookingDialog::dateFromChanged);
    connect(_dateToEdit, &QDateEdit::dateChanged, this, &BookingDialog::updateBookButtonState);

    _bookButton = new QPushButton("Book", this);
    _cancelButton = new QPushButton("Cancel Booking", this);
    _closeButton = new QPushButton("Close", this);

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(_bookButton);
    buttonLayout->addWidget(_cancelButton);
    buttonLayout->addWidget(_closeButton);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_deskLabel);
    mainLayout->addWidget(_dateLabel);
    mainLayout->addWidget(_statusLabel);
    mainLayout->addWidget(dateGroupBox);
    mainLayout->addLayout(buttonLayout);

    // Set button availability
    updateBookButtonState();

    // Enable cancel button only if there's a booking on this date
    _cancelButton->setEnabled(isBookedOnDate);

    connect(_bookButton, &QPushButton::clicked, this, &BookingDialog::bookDesk);
    connect(_cancelButton, &QPushButton::clicked, this, &BookingDialog::cancelBooking);
    connect(_closeButton, &QPushButton::clicked, this, &QDialog::accept);
}

void BookingDialog::dateFromChanged(const QDate &date) {
    // Update minimum date for "To" field
    _dateToEdit->setMinimumDate(date);
    if (_dateToEdit->date() < date) {
        _dateToEdit->setDate(date);
    }

    // Check if the new date range would overlap with existing bookings
    updateBookButtonState();
}

void BookingDialog::updateBookButtonState() {
    QDate dateFrom = _dateFromEdit->date();
    QDate dateTo = _dateToEdit->date();

    // Get current user ID (if logged in)
    int currentUserId = -1;
    if (_apiClient.isLoggedIn() && _apiClient.getCurrentUser()) {
        currentUserId = _apiClient.getCurrentUser()->getId();
    }

    // Check for overlapping bookings
    // We need to check all bookings except our own current booking (if we're modifying)
    bool hasOverlap = false;
    QString overlapInfo;

    // Get all bookings for this desk
    const auto &allBookings = _desk.getBookings();

    for (const auto &booking: allBookings) {
        // Skip checking our own booking that we might be modifying
        if (booking.getId() == _bookingId && booking.getUserId() == currentUserId) {
            continue;
        }

        // Check for overlap using our modified algorithm
        if (booking.overlapsWithPeriod(dateFrom, dateTo)) {
            hasOverlap = true;

            if (!overlapInfo.isEmpty()) {
                overlapInfo += ", ";
            }

            overlapInfo += booking.getDateFrom().toString("MM/dd") + "-" +
                    booking.getDateTo().toString("MM/dd");
        }
    }

    // Enable book button only if there's no overlap
    _bookButton->setEnabled(!hasOverlap);

    // Update status message
    if (hasOverlap) {
        _statusLabel->setText(QString("Status: Cannot book - overlaps with existing booking (%1)")
            .arg(overlapInfo));
        _statusLabel->setStyleSheet("color: red;");
    } else {
        // Get bookings on the selected date (if any)
        auto bookingsOnDate = _desk.getBookingsContainingDate(_bookingDate);
        bool isOwnBooking = false;

        for (const auto &booking: bookingsOnDate) {
            if (booking.getUserId() == currentUserId) {
                isOwnBooking = true;
                break;
            }
        }

        if (isOwnBooking) {
            _statusLabel->setText("Status: You can modify your booking for this date");
            _statusLabel->setStyleSheet("color: blue;");
            _bookButton->setText("Update Booking");
        } else if (!bookingsOnDate.empty()) {
            _statusLabel->setText("Status: Booked by someone else for this date");
            _statusLabel->setStyleSheet("");
            _bookButton->setText("Book");
        } else {
            if (dateFrom == dateTo) {
                _statusLabel->setText("Status: Available for selected date");
            } else {
                _statusLabel->setText(QString("Status: Available for period %1 to %2")
                    .arg(dateFrom.toString("MM/dd"), dateTo.toString("MM/dd")));
            }
            _statusLabel->setStyleSheet("color: green;");
            _bookButton->setText("Book");
        }
    }
}

void BookingDialog::bookDesk() {
    QDate dateFrom = _dateFromEdit->date();
    QDate dateTo = _dateToEdit->date();

    std::string dateFromStr = dateFrom.toString("yyyy-MM-dd").toStdString();
    std::string dateToStr = dateTo.toString("yyyy-MM-dd").toStdString();

    LOG_INFO("Attempting to book desk {} for period from {} to {}", _desk.getId(), dateFromStr, dateToStr);

    // Log all existing bookings for this desk to help with debugging
    LOG_INFO("Current bookings for desk {}:", _desk.getId());
    for (const auto &booking: _desk.getBookings()) {
        LOG_INFO("  Booking: ID={}, Period={} to {}, User={}",
                 booking.getId(),
                 booking.getDateFrom().toString("yyyy-MM-dd").toStdString(),
                 booking.getDateTo().toString("yyyy-MM-dd").toStdString(),
                 booking.getUserId());
    }

    // Double-check for overlapping bookings
    bool hasClientSideOverlap = _desk.hasOverlappingBooking(dateFrom, dateTo);
    LOG_INFO("Client-side overlap check result: {}", hasClientSideOverlap ? "Overlap detected" : "No overlap");

    if (hasClientSideOverlap) {
        QMessageBox::warning(this, "Booking Error",
                             "This desk is already booked during part of your selected period. "
                             "Please select different dates or choose another desk.");
        return;
    }

    // Get the current user's ID
    int userId = 1; // Default user ID as fallback

    auto currentUser = _apiClient.getCurrentUser();
    if (currentUser) {
        userId = currentUser->getId();
        LOG_INFO("Using logged-in user ID: {}", userId);
    } else {
        LOG_WARNING("No logged-in user found, using default user ID: {}", userId);
    }

    bool success = _apiClient.addBooking(_desk.getId(), userId, dateFromStr, dateToStr);
    if (success) {
        // Add the booking to the desk object for UI display
        Booking newBooking(0, _desk.getId(), userId, dateFrom, dateTo);
        _desk.addBooking(newBooking);

        QMessageBox::information(this, "Booking",
                                 QString("Desk has been booked successfully for the period from %1 to %2!")
                                 .arg(dateFrom.toString("MM/dd/yyyy"), dateTo.toString("MM/dd/yyyy")));
        accept();
    } else {
        // Get detailed error information from the server response if possible
        QMessageBox::warning(this, "Booking Error",
                             "Failed to book the desk. It may already be booked for this period or the server is unavailable.\n\n"
                             "Try refreshing the desk data and check if someone else booked it first.");

        LOG_ERROR("Server rejected booking for desk {} from {} to {}",
                  _desk.getId(), dateFromStr, dateToStr);
    }
}

void BookingDialog::cancelBooking() {
    LOG_INFO("Attempting to cancel booking for desk {}, booking ID: {}", _desk.getId(), _bookingId);

    bool success = false;

    if (_bookingId > 0) {
        // Cancel specific booking by ID
        success = _apiClient.cancelBooking(_bookingId);
    } else {
        // Get bookings for the selected date
        auto bookingsOnDate = _desk.getBookingsContainingDate(_bookingDate);
        if (!bookingsOnDate.empty()) {
            const Booking &booking = bookingsOnDate[0]; // Use first booking if multiple exist
            int bookingId = booking.getId();

            if (bookingId > 0) {
                success = _apiClient.cancelBooking(bookingId);
            } else {
                // Fallback to using desk ID if no booking ID is available
                success = _apiClient.cancelBooking(_desk.getId());
            }
        } else {
            QMessageBox::warning(this, "Cancellation Error",
                                 "No booking found for the selected date.");
            return;
        }
    }

    if (success) {
        if (_bookingId > 0) {
            // Remove only the specific booking
            _desk.cancelBooking(_bookingId);
        } else {
            // Remove all bookings containing the selected date
            auto bookingsOnDate = _desk.getBookingsContainingDate(_bookingDate);
            for (const auto &booking: bookingsOnDate) {
                if (booking.getId() > 0) {
                    _desk.cancelBooking(booking.getId());
                }
            }
        }

        QMessageBox::information(this, "Booking Cancellation", "Booking has been canceled.");
        accept();
    } else {
        QMessageBox::warning(this, "Cancellation Error",
                             "Failed to cancel the booking. The server may be unavailable.");
    }
}
