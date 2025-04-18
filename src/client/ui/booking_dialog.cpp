#include "booking_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

BookingDialog::BookingDialog(Desk &desk, const QDate &date, ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), desk(desk), bookingDate(date), apiClient(apiClient) {
    setWindowTitle("Desk Booking");
    setMinimumWidth(350);

    auto layout = new QVBoxLayout(this);

    // Desk info
    layout->addWidget(new QLabel(QString("Desk: %1").arg(QString::fromStdString(desk.getDeskId()))));
    layout->addWidget(new QLabel(QString("Date: %1").arg(date.toString("MM/dd/yyyy"))));

    // Booking status section
    bool isBooked = desk.isBookedOnDate(date);

    QGroupBox *statusGroup = new QGroupBox("Booking Status", this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

    QString statusText = isBooked ? "Status: Booked" : "Status: Available";
    statusLayout->addWidget(new QLabel(statusText));

    // If booked, show booking details
    if (isBooked) {
        auto bookings = desk.getBookingsContainingDate(date);
        if (!bookings.empty()) {
            const auto &booking = bookings[0];

            // Try to get booker's username
            QString bookedBy = QString::number(booking.getUserId());
            // In a real app, you would fetch the username based on ID

            // Show if the current user is the owner of this booking
            bool isOwner = apiClient.isLoggedIn() &&
                           apiClient.getCurrentUser()->getId() == booking.getUserId();
            if (isOwner) {
                bookedBy += " (You)";
            }

            statusLayout->addWidget(new QLabel(QString("Booked by: %1").arg(bookedBy)));
            statusLayout->addWidget(new QLabel(QString("From: %1").arg(
                QString::fromStdString(booking.getDateFromString()))));
            statusLayout->addWidget(new QLabel(QString("To: %1").arg(
                QString::fromStdString(booking.getDateToString()))));
        }
    }

    layout->addWidget(statusGroup);

    // Date range for new booking
    auto dateGroup = new QGroupBox("Booking Period", this);
    auto dateLayout = new QHBoxLayout(dateGroup);

    dateLayout->addWidget(new QLabel("From:"));
    dateFromEdit = new QDateEdit(date, this);
    dateFromEdit->setCalendarPopup(true);
    dateLayout->addWidget(dateFromEdit);

    dateLayout->addWidget(new QLabel("To:"));
    dateToEdit = new QDateEdit(date, this);
    dateToEdit->setCalendarPopup(true);
    dateLayout->addWidget(dateToEdit);

    layout->addWidget(dateGroup);
    dateGroup->setVisible(!isBooked); // Only show when desk is available

    // Buttons
    auto buttonLayout = new QHBoxLayout();

    bookButton = new QPushButton("Book", this);
    connect(bookButton, &QPushButton::clicked, this, &BookingDialog::bookDesk);
    buttonLayout->addWidget(bookButton);

    cancelButton = new QPushButton("Cancel Booking", this);
    connect(cancelButton, &QPushButton::clicked, this, &BookingDialog::cancelBooking);
    buttonLayout->addWidget(cancelButton);

    auto closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);

    // Set button states
    bookButton->setEnabled(!isBooked);

    // Only enable cancel button if user is owner or admin
    bool canCancel = false;
    if (isBooked && apiClient.isLoggedIn()) {
        auto bookings = desk.getBookingsContainingDate(date);
        if (!bookings.empty()) {
            int bookingUserId = bookings[0].getUserId();
            int currentUserId = apiClient.getCurrentUser()->getId();
            canCancel = (bookingUserId == currentUserId) || apiClient.isAdmin();
        }
    }
    cancelButton->setEnabled(canCancel);
}

void BookingDialog::bookDesk() {
    // Verify login
    if (!apiClient.isLoggedIn()) {
        QMessageBox::warning(this, "Login Required", "You must be logged in to book a desk");
        return;
    }

    QDate dateFrom = dateFromEdit->date();
    QDate dateTo = dateToEdit->date();

    // Validate date range
    if (dateFrom > dateTo) {
        QMessageBox::warning(this, "Invalid Dates", "End date must be after or equal to start date");
        return;
    }

    std::string dateFromStr = dateFrom.toString("yyyy-MM-dd").toStdString();
    std::string dateToStr = dateTo.toString("yyyy-MM-dd").toStdString();

    // Get user ID
    int userId = apiClient.getCurrentUser()->getId();

    if (apiClient.addBooking(desk.getId(), userId, dateFromStr, dateToStr)) {
        // Add booking to local desk object for UI update
        Booking newBooking(0, desk.getId(), userId, dateFrom, dateTo);
        desk.addBooking(newBooking);

        QMessageBox::information(this, "Success", "Desk booked successfully");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Could not book the desk");
    }
}

void BookingDialog::cancelBooking() {
    // Verify login
    if (!apiClient.isLoggedIn()) {
        QMessageBox::warning(this, "Login Required", "You must be logged in to cancel a booking");
        return;
    }

    // Find booking ID
    int bookingId = 0;
    auto bookingsOnDate = desk.getBookingsContainingDate(bookingDate);

    if (!bookingsOnDate.empty()) {
        const auto &booking = bookingsOnDate[0];
        bookingId = booking.getId();

        // Check if the current user is allowed to cancel this booking
        int bookingUserId = booking.getUserId();
        int currentUserId = apiClient.getCurrentUser()->getId();
        bool isOwner = (bookingUserId == currentUserId);
        bool isAdmin = apiClient.isAdmin();

        if (!isOwner && !isAdmin) {
            QMessageBox::warning(this, "Permission Denied",
                                 "You can only cancel your own bookings unless you are an administrator");
            return;
        }
    }

    if (bookingId > 0 && apiClient.cancelBooking(bookingId)) {
        desk.cancelBooking(bookingId);
        QMessageBox::information(this, "Success", "Booking canceled");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Could not cancel the booking");
    }
}
