#ifndef ADMIN_DIALOG_H
#define ADMIN_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include "../net/api_client.h"

class AdminDialog : public QDialog {
    Q_OBJECT

public:
    explicit AdminDialog(ApiClient &apiClient, QWidget *parent = nullptr);

private slots:
    // Building management
    void refreshBuildings();

    void addBuilding();

    void updateBuilding();

    void deleteBuilding();

    void selectBuilding(int row, int column);

    // Desk management
    void refreshDesks();

    void addDesk();

    void updateDesk();

    void deleteDesk();

    void selectDesk(int row, int column);

private:
    ApiClient &apiClient;
    QTabWidget *tabWidget;

    // Buildings tab
    QTableWidget *buildingsTable;
    QLineEdit *buildingNameEdit;
    QLineEdit *buildingAddressEdit;
    int selectedBuildingId = -1;

    // Desks tab
    QTableWidget *desksTable;
    QLineEdit *deskIdEdit;
    QComboBox *buildingCombo;
    QSpinBox *floorSpin;
    QSpinBox *xSpin;
    QSpinBox *ySpin;
    int selectedDeskId = -1;
};

#endif
