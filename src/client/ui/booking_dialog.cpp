#include "booking_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include "common/logger.h"

BookingDialog::BookingDialog(Desk &desk, const QDate &date, ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), desk(desk), bookingDate(date), apiClient(apiClient), bookingId(0) {
    setWindowTitle("Rezerwacja biurka");
    setMinimumWidth(300);

    auto layout = new QVBoxLayout(this);

    // Informacje o biurku
    layout->addWidget(new QLabel(QString("Biurko: %1 (Piętro %2)")
        .arg(QString::fromStdString(desk.getName()))
        .arg(desk.getFloor())));

    // Sprawdź czy biurko jest zarezerwowane
    isBooked = desk.isBookedOnDate(date);

    // Pobierz ID aktualnego użytkownika
    int currentUserId = apiClient.getCurrentUser() ? apiClient.getCurrentUser()->getId() : -1;
    bool isOwnBooking = false;

    // Sekcja z informacjami o rezerwacji
    auto bookingInfoGroup = new QGroupBox("Informacje o rezerwacji", this);
    auto bookingInfoLayout = new QVBoxLayout(bookingInfoGroup);

    // Jeśli biurko jest zarezerwowane, wyświetl szczegóły
    if (isBooked) {
        auto bookings = desk.getBookingsContainingDate(date);
        if (!bookings.empty()) {
            const auto &booking = bookings[0];
            bookingId = booking.getId();
            int bookingUserId = booking.getUserId();
            isOwnBooking = (currentUserId == bookingUserId);

            // Pokaż okres rezerwacji
            QString bookedFrom = QDate::fromString(
                QString::fromStdString(booking.getDateFromString()),
                "yyyy-MM-dd").toString("dd.MM.yyyy");
            QString bookedTo = QDate::fromString(
                QString::fromStdString(booking.getDateToString()),
                "yyyy-MM-dd").toString("dd.MM.yyyy");

            bookingInfoLayout->addWidget(new QLabel(QString("Status: Zarezerwowane")));

            // Pokaż czy rezerwacja należy do zalogowanego użytkownika
            if (isOwnBooking) {
                bookingInfoLayout->addWidget(new QLabel("Zarezerwowane przez: Ciebie"));
            } else {
                bookingInfoLayout->addWidget(
                    new QLabel(QString("Zarezerwowane przez: Użytkownik #%1").arg(bookingUserId)));
            }

            bookingInfoLayout->addWidget(new QLabel(QString("Od: %1").arg(bookedFrom)));
            bookingInfoLayout->addWidget(new QLabel(QString("Do: %1").arg(bookedTo)));
        }
    } else {
        bookingInfoLayout->addWidget(new QLabel("Status: Dostępne"));
    }

    layout->addWidget(bookingInfoGroup);

    // Zakres dat dla nowej rezerwacji (tylko gdy biurko jest dostępne)
    auto dateGroup = new QGroupBox("Nowa rezerwacja", this);
    auto dateLayout = new QVBoxLayout(dateGroup);

    auto dateRangeLayout = new QHBoxLayout();

    dateRangeLayout->addWidget(new QLabel("Od:"));
    dateFromEdit = new QDateEdit(date, this);
    dateFromEdit->setCalendarPopup(true);
    dateFromEdit->setMinimumDate(QDate::currentDate());
    dateRangeLayout->addWidget(dateFromEdit);

    dateRangeLayout->addWidget(new QLabel("Do:"));
    dateToEdit = new QDateEdit(date, this);
    dateToEdit->setCalendarPopup(true);
    dateToEdit->setMinimumDate(date);
    dateRangeLayout->addWidget(dateToEdit);

    dateLayout->addLayout(dateRangeLayout);

    // Dodaj informację o dostępności
    QLabel *noteLabel = new QLabel("Wybierz zakres dat dla nowej rezerwacji.");
    noteLabel->setWordWrap(true);
    dateLayout->addWidget(noteLabel);

    layout->addWidget(dateGroup);

    // Pokaż opcje rezerwacji tylko dla dostępnych biurek
    dateGroup->setVisible(!isBooked);

    // Przyciski
    auto buttonLayout = new QHBoxLayout();

    bookButton = new QPushButton("Zarezerwuj", this);
    connect(bookButton, &QPushButton::clicked, this, &BookingDialog::bookDesk);
    buttonLayout->addWidget(bookButton);
    bookButton->setVisible(!isBooked);

    cancelButton = new QPushButton("Anuluj rezerwację", this);
    connect(cancelButton, &QPushButton::clicked, this, &BookingDialog::cancelBooking);
    buttonLayout->addWidget(cancelButton);

    // Przycisk anulowania rezerwacji tylko dla własnych rezerwacji
    cancelButton->setVisible(isBooked && isOwnBooking);

    auto closeButton = new QPushButton("Zamknij", this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(closeButton);

    layout->addLayout(buttonLayout);
}

void BookingDialog::bookDesk() {
    QDate dateFrom = dateFromEdit->date();
    QDate dateTo = dateToEdit->date();

    // Walidacja dat
    if (dateFrom > dateTo) {
        QMessageBox::warning(this, "Nieprawidłowe daty",
                             "Data końcowa musi być późniejsza lub równa dacie początkowej");
        return;
    }

    std::string dateFromStr = dateFrom.toString("yyyy-MM-dd").toStdString();
    std::string dateToStr = dateTo.toString("yyyy-MM-dd").toStdString();

    // Pobierz ID aktualnego użytkownika
    if (!apiClient.isLoggedIn() || !apiClient.getCurrentUser()) {
        QMessageBox::warning(this, "Błąd", "Musisz być zalogowany, aby zarezerwować biurko");
        return;
    }

    int userId = apiClient.getCurrentUser()->getId();
    int deskId = desk.getId();

    // Wywołaj API i obsłuż błędy
    auto [success, errorMsg] = apiClient.addBooking(deskId, userId, dateFromStr, dateToStr);

    if (success) {
        accept();
    } else {
        QMessageBox::warning(this, "Błąd rezerwacji", errorMsg);
    }
}

void BookingDialog::cancelBooking() {
    if (bookingId > 0 && apiClient.cancelBooking(bookingId)) {
        desk.cancelBooking(bookingId);
        accept();
    } else {
        QMessageBox::warning(this, "Błąd", "Nie można anulować rezerwacji");
    }
}
