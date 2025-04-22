#include "booking_view.h"
#include "booking_dialog.h"
#include "login_dialog.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QMenuBar>
#include <QMessageBox>
#include "common/logger.h"

BookingView::BookingView(QWidget *parent, ApiClient &apiClient)
    : QMainWindow(parent), apiClient(apiClient), selectedDate(QDate::currentDate()) {
    setupUi();
    setupMenus();
    setWindowTitle("Biurko++ - System rezerwacji biurek");

    // Połącz sygnały
    connect(&apiClient, &ApiClient::networkError, this, &BookingView::handleNetworkError);
}

BookingView::BookingView(QWidget *parent)
    : BookingView(parent, *(new ApiClient(this))) {
}

void BookingView::setupUi() {
    auto central = new QWidget(this);
    setCentralWidget(central);
    auto mainLayout = new QVBoxLayout(central);

    // Górna sekcja z kalendarzem i opcjami
    auto topLayout = new QHBoxLayout();

    // Panel kalendarza
    auto calendarPanel = new QGroupBox("Wybierz datę", this);
    auto calendarLayout = new QVBoxLayout(calendarPanel);
    calendar = new QCalendarWidget(this);
    calendar->setSelectedDate(selectedDate);
    connect(calendar, &QCalendarWidget::clicked, this, &BookingView::dateChanged);
    calendarLayout->addWidget(calendar);
    topLayout->addWidget(calendarPanel);

    // Panel opcji
    auto optionsPanel = new QGroupBox("Opcje", this);
    auto optionsLayout = new QVBoxLayout(optionsPanel);

    // Wybór budynku
    auto bldgLayout = new QHBoxLayout();
    bldgLayout->addWidget(new QLabel("Budynek:", this));
    buildingSelect = new QComboBox(this);
    buildingSelect->setMinimumWidth(250);
    buildingSelect->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(buildingSelect, &QComboBox::currentIndexChanged, this, &BookingView::buildingChanged);
    bldgLayout->addWidget(buildingSelect);
    optionsLayout->addLayout(bldgLayout);

    // Wybór piętra
    auto floorLayout = new QHBoxLayout();
    floorLayout->addWidget(new QLabel("Piętro:", this));
    floorSelect = new QComboBox(this);
    floorSelect->setMinimumWidth(250);
    floorSelect->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    connect(floorSelect, &QComboBox::currentIndexChanged, this, &BookingView::floorChanged);
    floorLayout->addWidget(floorSelect);
    optionsLayout->addLayout(floorLayout);

    // Etykiety informacyjne
    infoLabel = new QLabel(QString("Plan biurek na %1").arg(selectedDate.toString("dd.MM.yyyy")), this);
    optionsLayout->addWidget(infoLabel);

    // Etykieta użytkownika
    userLabel = new QLabel("Nie zalogowano", this);
    optionsLayout->addWidget(userLabel);

    // Przycisk odświeżania
    refreshButton = new QPushButton("Odśwież status biurek", this);
    connect(refreshButton, &QPushButton::clicked, this, &BookingView::refreshView);
    optionsLayout->addWidget(refreshButton);

    topLayout->addWidget(optionsPanel);
    mainLayout->addLayout(topLayout);

    // Panel mapy biurek
    auto mapPanel = new QGroupBox("Mapa biurek", this);
    auto mapLayout = new QVBoxLayout(mapPanel);

    // Przewijalna mapa biurek
    deskMapLayout = new QGridLayout();
    deskMapContainer = new QWidget(this);
    deskMapContainer->setLayout(deskMapLayout);

    auto scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(deskMapContainer);

    mapLayout->addWidget(scrollArea);
    mainLayout->addWidget(mapPanel, 1);
}

void BookingView::setupMenus() {
    // Menu użytkownika
    userMenu = menuBar()->addMenu("Użytkownik");

    loginAction = new QAction("Zaloguj", this);
    connect(loginAction, &QAction::triggered, this, &BookingView::showLoginDialog);
    userMenu->addAction(loginAction);

    logoutAction = new QAction("Wyloguj", this);
    connect(logoutAction, &QAction::triggered, this, &BookingView::handleUserLogout);
    userMenu->addAction(logoutAction);

    // Początkowy stan przycisków
    updateMenuVisibility();
}

bool BookingView::checkLogin(const QString &action) {
    if (!apiClient.isLoggedIn()) {
        if (!action.isEmpty()) {
            QMessageBox::information(this, "Wymagane logowanie",
                                     QString("Musisz się zalogować, aby %1").arg(action));
        }
        showLoginDialog();
        return false;
    }
    return true;
}

void BookingView::dateChanged(const QDate &date) {
    selectedDate = date;
    infoLabel->setText(QString("Plan biurek na %1").arg(date.toString("dd.MM.yyyy")));
    updateDeskMap();
}

void BookingView::buildingChanged(int index) {
    if (index >= 0 && index < buildingSelect->count()) {
        selectedBuildingId = buildingSelect->itemData(index).toInt();
        loadFloors(selectedBuildingId);

        // Odśwież widok tylko jeśli wybrano konkretny budynek
        if (selectedBuildingId > 0) {
            refreshView();
        } else {
            // Wyczyść widok biurek jeśli wybrano "Wybierz budynek"
            desks.clear();
            updateDeskMap();
        }
    }
}

void BookingView::floorChanged(int index) {
    if (index >= 0 && index < floorSelect->count()) {
        selectedFloor = floorSelect->itemData(index).toInt();

        // Odśwież tylko jeśli wybrano budynek i piętro
        if (selectedBuildingId > 0 && selectedFloor > 0) {
            refreshView();
        } else {
            // Wyczyść widok biurek jeśli wybrano "Wybierz piętro"
            desks.clear();
            updateDeskMap();
        }
    }
}

void BookingView::loadBuildings() {
    buildingSelect->clear();

    // Pobierz budynki z serwera
    buildings = apiClient.getBuildings();

    // Dodaj budynki do combobox
    buildingSelect->addItem("Wybierz budynek", -1);

    for (const auto &building: buildings) {
        QString displayText = QString::fromStdString(building.getName());
        if (!building.getAddress().empty()) {
            displayText += " (" + QString::fromStdString(building.getAddress()) + ")";
        }
        buildingSelect->addItem(displayText, building.getId());
    }
}

void BookingView::loadFloors(int buildingId) {
    floorSelect->clear();

    // Dodaj "Wybierz piętro" na górze listy
    floorSelect->addItem("Wybierz piętro", -1);

    // Pomiń jeśli wybrano "Wybierz budynek"
    if (buildingId <= 0) {
        selectedFloor = -1;
        return;
    }

    // Znajdź wybrany budynek
    auto it = std::find_if(buildings.begin(), buildings.end(),
                           [buildingId](const Building &building) {
                               return building.getId() == buildingId;
                           });

    if (it != buildings.end()) {
        // Pobierz liczbę pięter z budynku i utwórz elementy listy
        int numFloors = it->getNumFloors();

        for (int floor = 1; floor <= numFloors; floor++) {
            floorSelect->addItem(QString("Piętro %1").arg(floor), floor);
        }
    }
}

void BookingView::refreshView() {
    // Aktualizacja statusu logowania w UI
    if (apiClient.isLoggedIn() && apiClient.getCurrentUser()) {
        userLabel->setText(QString("Zalogowano jako: %1")
            .arg(QString::fromStdString(apiClient.getCurrentUser()->getUsername())));
    } else {
        userLabel->setText("Nie zalogowano");
    }

    // Aktualizuj widoczność opcji menu
    updateMenuVisibility();

    // Załaduj budynki jeśli potrzeba
    if (buildings.empty()) {
        loadBuildings();
    }

    // Zawsze wyczyść istniejące biurka, aby pobrać świeże dane
    desks.clear();

    // Pobierz biurka tylko jeśli użytkownik jest zalogowany ORAZ wybrano budynek/piętro
    if (apiClient.isLoggedIn() && selectedBuildingId > 0 && selectedFloor > 0) {
        desks = apiClient.getDesks(selectedBuildingId, selectedFloor);
    }

    // Aktualizuj widok biurek
    updateDeskMap();
}

void BookingView::updateMenuVisibility() {
    bool isLoggedIn = apiClient.isLoggedIn();
    loginAction->setVisible(!isLoggedIn);
    logoutAction->setVisible(isLoggedIn);
}

void BookingView::updateDeskMap() {
    // Wyczyść istniejące elementy
    while (QLayoutItem *item = deskMapLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    // Sprawdź status logowania
    if (!apiClient.isLoggedIn()) {
        auto loginButton = new QPushButton("Zaloguj się, aby zobaczyć biurka", this);
        connect(loginButton, &QPushButton::clicked, this, &BookingView::showLoginDialog);
        deskMapLayout->addWidget(loginButton, 0, 0);
        return;
    }

    // Jeśli nie wybrano budynku lub piętra, pokaż komunikat
    if (selectedBuildingId <= 0 || selectedFloor <= 0) {
        auto promptLabel = new QLabel("Wybierz budynek i piętro, aby zobaczyć dostępne biurka", this);
        promptLabel->setAlignment(Qt::AlignCenter);
        promptLabel->setWordWrap(true);
        deskMapLayout->addWidget(promptLabel, 0, 0);
        return;
    }

    // Brak biurek dla tej kombinacji
    if (desks.empty()) {
        auto noDesksLabel = new QLabel(QString("Nie znaleziono biurek dla Budynku ID %1, Piętro %2")
                                       .arg(selectedBuildingId).arg(selectedFloor), this);
        noDesksLabel->setAlignment(Qt::AlignCenter);
        noDesksLabel->setWordWrap(true);
        deskMapLayout->addWidget(noDesksLabel, 0, 0);
        return;
    }

    // Wyświetl biurka
    int currentUserId = apiClient.getCurrentUser() ? apiClient.getCurrentUser()->getId() : -1;

    for (size_t i = 0; i < desks.size(); ++i) {
        const auto &desk = desks[i];
        bool isBooked = desk.isBookedOnDate(selectedDate);

        // Utwórz przycisk
        auto button = new QPushButton(this);
        button->setMinimumSize(100, 80);

        // Przycisk z informacją o piętrze
        QString deskName = QString::fromStdString(desk.getName());
        QString floorInfo = QString("Piętro %1").arg(desk.getFloor());

        if (isBooked) {
            auto bookings = desk.getBookingsContainingDate(selectedDate);
            if (!bookings.empty()) {
                const auto &booking = bookings[0];
                int bookingUserId = booking.getUserId();

                if (currentUserId == bookingUserId) {
                    // Niebieski dla własnych rezerwacji
                    button->setStyleSheet("background-color: #2196F3; color: white;");
                    button->setText(deskName + "\n" + floorInfo + "\nZarezerwowane przez Ciebie");
                } else {
                    // Czerwony dla rezerwacji innych osób
                    button->setStyleSheet("background-color: #F44336; color: white;");
                    button->setText(deskName + "\n" + floorInfo + "\nZarezerwowane przez Użytkownika #" +
                                    QString::number(bookingUserId));
                }
            }
        } else {
            // Zielony dla dostępnych biurek
            button->setText(deskName + "\n" + floorInfo + "\nDostępne");
            button->setStyleSheet("background-color: #4CAF50; color: white;");
        }

        // Przechowaj indeks biurka dla obsługi kliknięcia
        button->setProperty("index", static_cast<int>(i));
        button->setProperty("is_booked", isBooked);
        connect(button, &QPushButton::clicked, this, &BookingView::deskClicked);

        // Użyj domyślnego układu siatki - 4 kolumny
        deskMapLayout->addWidget(button, i / 4, i % 4);
    }
}

void BookingView::deskClicked() {
    // Najpierw sprawdź logowanie
    if (!checkLogin("zobaczyć szczegóły biurka")) {
        return;
    }

    QPushButton *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    int deskIndex = button->property("index").toInt();
    if (deskIndex < 0 || deskIndex >= static_cast<int>(desks.size())) return;

    // Pokaż dialog rezerwacji z aktualnymi danymi biurka
    BookingDialog dialog(desks[deskIndex], selectedDate, apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        // Odśwież po zmianie rezerwacji
        refreshView();
    }
}

void BookingView::showLoginDialog() {
    LoginDialog dialog(apiClient, this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshView();
    }
}

void BookingView::handleUserLogout() {
    apiClient.logoutUser();
    refreshView();
}

void BookingView::handleNetworkError(const QString &error) {
    if (error.contains("connection") || error.contains("timeout") || error.contains("server")) {
        QMessageBox::warning(this, "Błąd sieci", error);
    } else {
        LOG_ERROR("Błąd sieci: {}", error.toStdString());
    }
}
