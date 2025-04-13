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
#include "../model/desk.h"
#include "../net/client_communication.h"

class BookingDialog : public QDialog {
    Q_OBJECT

public:
    BookingDialog(Desk &desk, const QDate &bookingDate, ClientCommunication &communication,
                  QWidget *parent = nullptr);

private slots:
    void bookDesk();

    void cancelBooking();

    void dateFromChanged(const QDate &date);

    void updateBookButtonState();

private:
    Desk &_desk;
    QDate _bookingDate;
    ClientCommunication &_communication;
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
