#ifndef BOOKING_DIALOG_H
#define BOOKING_DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDateEdit>
#include "common/model/model.h"
#include "../net/api_client.h"

/**
 * @class BookingDialog
 * @brief Dialog do zarządzania rezerwacjami biurek.
 *
 * Dialog umożliwiający rezerwację biurka na wybrany okres
 * lub anulowanie istniejącej rezerwacji.
 */
class BookingDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Konstruktor
     * @param desk Referencja do obiektu biurka
     * @param date Data rezerwacji
     * @param apiClient Referencja do klienta API
     * @param parent Obiekt rodzica (opcjonalny)
     */
    BookingDialog(Desk &desk, const QDate &date, ApiClient &apiClient, QWidget *parent = nullptr);

private slots:
    /**
     * @brief Obsługuje rezerwację biurka
     */
    void bookDesk();

    /**
     * @brief Obsługuje anulowanie rezerwacji
     */
    void cancelBooking();

private:
    Desk &desk;
    QDate bookingDate;
    ApiClient &apiClient;
    QDateEdit *dateFromEdit;
    QDateEdit *dateToEdit;
    QPushButton *bookButton;
    QPushButton *cancelButton;
    bool isBooked;
    int bookingId;
};

#endif
