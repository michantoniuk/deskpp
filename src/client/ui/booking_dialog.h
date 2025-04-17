#ifndef BOOKING_DIALOG_H
#define BOOKING_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDateEdit>
#include "common/model/model.h"
#include "../net/api_client.h"

class BookingDialog : public QDialog {
    Q_OBJECT

public:
    BookingDialog(Desk &desk, const QDate &date, ApiClient &apiClient, QWidget *parent = nullptr);

private slots:
    void bookDesk();

    void cancelBooking();

private:
    Desk &desk;
    QDate bookingDate;
    ApiClient &apiClient;

    QDateEdit *dateFromEdit;
    QDateEdit *dateToEdit;
    QPushButton *bookButton;
    QPushButton *cancelButton;
};

#endif // BOOKING_DIALOG_H
