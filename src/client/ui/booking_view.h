#ifndef BOOKING_VIEW_H
#define BOOKING_VIEW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QDate>
#include <QPushButton>
#include <QMenu>
#include <QAction>

#include "common/model/model.h"
#include "../net/api_client.h"

/**
 * @class BookingView
 * @brief Główne okno aplikacji do rezerwacji biurek.
 *
 * Umożliwia przeglądanie dostępnych biurek, filtrowanie po budynkach i piętrach,
 * wyświetlanie statusu rezerwacji oraz zarządzanie rezerwacjami.
 */
class BookingView : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Konstruktor domyślny
     * @param parent Obiekt rodzica (opcjonalny)
     */
    explicit BookingView(QWidget *parent = nullptr);

    /**
     * @brief Konstruktor z parametrami
     * @param parent Obiekt rodzica
     * @param apiClient Referencja do klienta API
     */
    BookingView(QWidget *parent, ApiClient &apiClient);

    /**
     * @brief Odświeża widok i dane
     */
    void refreshView();

private slots:
    /**
     * @brief Obsługuje zmianę wybranego budynku
     * @param index Indeks wybranego budynku w ComboBox
     */
    void buildingChanged(int index);

    /**
     * @brief Obsługuje zmianę wybranej daty
     * @param date Nowa data
     */
    void dateChanged(const QDate &date);

    /**
     * @brief Obsługuje kliknięcie na biurko
     */
    void deskClicked();

    /**
     * @brief Pokazuje dialog logowania
     */
    void showLoginDialog();

    /**
     * @brief Obsługuje wylogowanie użytkownika
     */
    void handleUserLogout();

    /**
     * @brief Obsługuje błędy sieciowe
     * @param error Komunikat błędu
     */
    void handleNetworkError(const QString &error);

private:
    /**
     * @brief Inicjalizuje interfejs użytkownika
     */
    void setupUi();

    /**
     * @brief Inicjalizuje menu aplikacji
     */
    void setupMenus();

    /**
     * @brief Aktualizuje widok biurek
     */
    void updateDeskMap();

    /**
     * @brief Aktualizuje widoczność opcji menu w zależności od stanu logowania
     */
    void updateMenuVisibility();

    /**
     * @brief Pobiera i ładuje listę budynków
     */
    void loadBuildings();

    /**
     * @brief Sprawdza czy użytkownik jest zalogowany
     * @param action Opis akcji wymagającej logowania (opcjonalny)
     * @return Czy użytkownik jest zalogowany
     */
    bool checkLogin(const QString &action = QString());

    // Komponenty UI
    QCalendarWidget *calendar;
    QComboBox *buildingSelect;
    QComboBox *floorSelect;
    QLabel *infoLabel;
    QLabel *userLabel;
    QPushButton *refreshButton;
    QGridLayout *deskMapLayout;
    QWidget *deskMapContainer;
    QMenu *userMenu;
    QAction *loginAction;
    QAction *logoutAction;

    // Dane
    ApiClient &apiClient;
    std::vector<Building> buildings;
    std::vector<Desk> desks;

    // Stan
    int selectedBuildingId = -1;
    int selectedFloor = -1;
    QDate selectedDate;

    /**
     * @brief Ładuje listę pięter dla budynku
     * @param buildingId ID budynku
     */
    void loadFloors(int buildingId);

    /**
     * @brief Obsługuje zmianę wybranego piętra
     * @param index Indeks wybranego piętra w ComboBox
     */
    void floorChanged(int index);
};

#endif
