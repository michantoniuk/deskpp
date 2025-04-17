#ifndef ADMIN_DIALOG_H
#define ADMIN_DIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QTableWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include "common/model/model.h"
#include "../net/api_client.h"

class AdminDialog : public QDialog {
    Q_OBJECT

public:
    explicit AdminDialog(ApiClient &apiClient, QWidget *parent = nullptr);

private slots:
    // Building management
    void refreshBuildingList();

    void addBuilding();

    void editBuilding();

    void deleteBuilding();

    void selectBuilding(int row, int column);

    // Desk management
    void refreshDeskList();

    void addDesk();

    void editDesk();

    void deleteDesk();

    void selectDesk(int row, int column);

    void populateBuildingCombo();

private:
    // Add these method declarations
    void setupBuildingsTab();

    void setupDesksTab();

    ApiClient &_apiClient;
    QTabWidget *_tabWidget;

    // Buildings tab
    QWidget *_buildingsTab;
    QTableWidget *_buildingsTable;
    QLineEdit *_buildingNameEdit;
    QLineEdit *_buildingAddressEdit;
    QPushButton *_addBuildingButton;
    QPushButton *_editBuildingButton;
    QPushButton *_deleteBuildingButton;

    // Desks tab
    QWidget *_desksTab;
    QTableWidget *_desksTable;
    QLineEdit *_deskIdEdit;
    QComboBox *_buildingCombo;
    QSpinBox *_floorNumberSpin;
    QSpinBox *_locationXSpin;
    QSpinBox *_locationYSpin;
    QPushButton *_addDeskButton;
    QPushButton *_editDeskButton;
    QPushButton *_deleteDeskButton;

    // Current selection
    int _selectedBuildingId = -1;
    int _selectedDeskId = -1;
};

#endif // ADMIN_DIALOG_H
