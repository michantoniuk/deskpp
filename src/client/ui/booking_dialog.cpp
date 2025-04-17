#include "booking_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>

BookingDialog::BookingDialog(Desk &desk, const QDate &date, ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), desk(desk), bookingDate(date), apiClient(apiClient) {
    setWindowTitle("Desk Booking");
    setMinimumWidth(300);

    auto layout = new QVBoxLayout(this);

    // Desk info
    layout->addWidget(new QLabel(QString("Desk: %1").arg(QString::fromStdString(desk.getDeskId()))));
    layout->addWidget(new QLabel(QString("Date: %1").arg(date.toString("MM/dd/yyyy"))));

    // Status
    QString status = desk.isBookedOnDate(date) ? "Status: Booked" : "Status: Available";
    layout->addWidget(new QLabel(status));

    // Date range
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

    // Buttons
    auto buttonLayout = new QHBoxLayout();

    bookButton = new QPushButton("Book", this);
    connect(bookButton, &QPushButton::clicked, this, &BookingDialog::bookDesk);
    buttonLayout->addWidget(bookButton);

    cancelButton = new QPushButton("Cancel Booking", this);
    connect(cancelButton, &QPushButton::clicked, this, &BookingDialog::cancelBooking);
    buttonLayout->addWidget(cancelButton);

    auto closeButton = new QPushButton("Close", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);

    // Set button states
    bool isBooked = desk.isBookedOnDate(date);
    bookButton->setEnabled(!isBooked);
    cancelButton->setEnabled(isBooked);
}

void BookingDialog::bookDesk() {
    QDate dateFrom = dateFromEdit->date();
    QDate dateTo = dateToEdit->date();

    std::string dateFromStr = dateFrom.toString("yyyy-MM-dd").toStdString();
    std::string dateToStr = dateTo.toString("yyyy-MM-dd").toStdString();

    // Get user ID
    int userId = 1; // Default
    if (apiClient.getCurrentUser()) {
        userId = apiClient.getCurrentUser()->getId();
    }

    if (apiClient.addBooking(desk.getId(), userId, dateFromStr, dateToStr)) {
        // Add booking to local desk object
        Booking newBooking(0, desk.getId(), userId, dateFrom, dateTo);
        desk.addBooking(newBooking);

        QMessageBox::information(this, "Success", "Desk booked successfully");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Could not book the desk");
    }
}

void BookingDialog::cancelBooking() {
    // Find booking ID
    int bookingId = 0;
    auto bookingsOnDate = desk.getBookingsContainingDate(bookingDate);

    if (!bookingsOnDate.empty()) {
        bookingId = bookingsOnDate[0].getId();
    }

    if (bookingId > 0 && apiClient.cancelBooking(bookingId)) {
        desk.cancelBooking(bookingId);
        QMessageBox::information(this, "Success", "Booking canceled");
        accept();
    } else {
        QMessageBox::warning(this, "Error", "Could not cancel the booking");
    }
}
