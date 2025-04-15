#ifndef BOOKING_DIALOG_H
#define BOOKING_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDate>
#include <QDateEdit>
#include <QGroupBox>
#include "common/models.h"
#include "../net/api_client.h"

class BookingDialog : public QDialog {
    Q_OBJECT

public:
    BookingDialog(Desk &desk, const QDate &bookingDate, ApiClient &apiClient,
                  QWidget *parent = nullptr);

private slots:
    void bookDesk();

    void cancelBooking();

    void dateFromChanged(const QDate &date);

    void updateBookButtonState();

private:
    Desk &_desk;
    QDate _bookingDate;
    ApiClient &_apiClient;
    int _bookingId;

    QLabel *_deskLabel;
    QLabel *_dateLabel;
    QLabel *_statusLabel;
    QDateEdit *_dateFromEdit;
    QDateEdit *_dateToEdit;
    QPushButton *_bookButton;
    QPushButton *_cancelButton;
    QPushButton *_closeButton;
};

#endif // BOOKING_DIALOG_H
