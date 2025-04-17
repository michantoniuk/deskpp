#include "admin_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include "common/logger.h"

AdminDialog::AdminDialog(ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), _apiClient(apiClient) {
    setWindowTitle("Admin Management");
    resize(800, 600);

    // Create tab widget
    _tabWidget = new QTabWidget(this);

    // Create buildings tab
    setupBuildingsTab();

    // Create desks tab
    setupDesksTab();

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(_tabWidget);

    // Load initial data
    refreshBuildingList();
    refreshDeskList();
    populateBuildingCombo();
}

void AdminDialog::setupBuildingsTab() {
    _buildingsTab = new QWidget();
    _tabWidget->addTab(_buildingsTab, "Buildings");

    // Create table for buildings
    _buildingsTable = new QTableWidget(0, 3);
    _buildingsTable->setHorizontalHeaderLabels({"ID", "Name", "Address"});
    _buildingsTable->horizontalHeader()->setStretchLastSection(true);
    _buildingsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _buildingsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _buildingsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connect selection signal
    connect(_buildingsTable, &QTableWidget::cellClicked, this, &AdminDialog::selectBuilding);

    // Create form for adding/editing buildings
    QFormLayout *buildingForm = new QFormLayout();

    _buildingNameEdit = new QLineEdit();
    _buildingAddressEdit = new QLineEdit();

    buildingForm->addRow("Name:", _buildingNameEdit);
    buildingForm->addRow("Address:", _buildingAddressEdit);

    // Create buttons
    _addBuildingButton = new QPushButton("Add");
    _editBuildingButton = new QPushButton("Update");
    _deleteBuildingButton = new QPushButton("Delete");

    // Disable edit/delete until a building is selected
    _editBuildingButton->setEnabled(false);
    _deleteBuildingButton->setEnabled(false);

    // Connect buttons
    connect(_addBuildingButton, &QPushButton::clicked, this, &AdminDialog::addBuilding);
    connect(_editBuildingButton, &QPushButton::clicked, this, &AdminDialog::editBuilding);
    connect(_deleteBuildingButton, &QPushButton::clicked, this, &AdminDialog::deleteBuilding);

    // Create button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(_addBuildingButton);
    buttonLayout->addWidget(_editBuildingButton);
    buttonLayout->addWidget(_deleteBuildingButton);

    // Create refresh button
    QPushButton *refreshButton = new QPushButton("Refresh");
    connect(refreshButton, &QPushButton::clicked, this, &AdminDialog::refreshBuildingList);

    // Create layout for the buildings tab
    QVBoxLayout *buildingsLayout = new QVBoxLayout(_buildingsTab);
    buildingsLayout->addWidget(_buildingsTable);
    buildingsLayout->addLayout(buildingForm);
    buildingsLayout->addLayout(buttonLayout);
    buildingsLayout->addWidget(refreshButton);
}

void AdminDialog::setupDesksTab() {
    _desksTab = new QWidget();
    _tabWidget->addTab(_desksTab, "Desks");

    // Create table for desks
    _desksTable = new QTableWidget(0, 6);
    _desksTable->setHorizontalHeaderLabels({"ID", "Desk Number", "Building", "Floor", "X", "Y"});
    _desksTable->horizontalHeader()->setStretchLastSection(true);
    _desksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    _desksTable->setSelectionMode(QAbstractItemView::SingleSelection);
    _desksTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Connect selection signal
    connect(_desksTable, &QTableWidget::cellClicked, this, &AdminDialog::selectDesk);

    // Create form for adding/editing desks
    QFormLayout *deskForm = new QFormLayout();

    _deskIdEdit = new QLineEdit();
    _buildingCombo = new QComboBox();
    _floorNumberSpin = new QSpinBox();
    _floorNumberSpin->setMinimum(1);
    _floorNumberSpin->setMaximum(100);
    _locationXSpin = new QSpinBox();
    _locationXSpin->setMinimum(0);
    _locationXSpin->setMaximum(1000);
    _locationYSpin = new QSpinBox();
    _locationYSpin->setMinimum(0);
    _locationYSpin->setMaximum(1000);

    deskForm->addRow("Desk Number:", _deskIdEdit);
    deskForm->addRow("Building:", _buildingCombo);
    deskForm->addRow("Floor Number:", _floorNumberSpin);
    deskForm->addRow("Position X:", _locationXSpin);
    deskForm->addRow("Position Y:", _locationYSpin);

    // Create buttons
    _addDeskButton = new QPushButton("Add");
    _editDeskButton = new QPushButton("Update");
    _deleteDeskButton = new QPushButton("Delete");

    // Disable edit/delete until a desk is selected
    _editDeskButton->setEnabled(false);
    _deleteDeskButton->setEnabled(false);

    // Connect buttons
    connect(_addDeskButton, &QPushButton::clicked, this, &AdminDialog::addDesk);
    connect(_editDeskButton, &QPushButton::clicked, this, &AdminDialog::editDesk);
    connect(_deleteDeskButton, &QPushButton::clicked, this, &AdminDialog::deleteDesk);

    // Create button layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(_addDeskButton);
    buttonLayout->addWidget(_editDeskButton);
    buttonLayout->addWidget(_deleteDeskButton);

    // Create refresh button
    QPushButton *refreshButton = new QPushButton("Refresh");
    connect(refreshButton, &QPushButton::clicked, this, &AdminDialog::refreshDeskList);

    // Create layout for the desks tab
    QVBoxLayout *desksLayout = new QVBoxLayout(_desksTab);
    desksLayout->addWidget(_desksTable);
    desksLayout->addLayout(deskForm);
    desksLayout->addLayout(buttonLayout);
    desksLayout->addWidget(refreshButton);
}

void AdminDialog::refreshBuildingList() {
    _buildingsTable->clearContents();
    _buildingsTable->setRowCount(0);

    try {
        // Use the higher-level methods or public executeRequest
        json response = _apiClient.executeRequest("GET", "/api/buildings");

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("buildings") && response["buildings"].is_array()) {
            const auto &buildings = response["buildings"];
            _buildingsTable->setRowCount(buildings.size());

            int row = 0;
            for (const auto &building: buildings) {
                int id = building.contains("id") ? building["id"].get<int>() : 0;
                std::string name = building.contains("name") ? building["name"].get<std::string>() : "";
                std::string address = building.contains("address") ? building["address"].get<std::string>() : "";

                _buildingsTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
                _buildingsTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(name)));
                _buildingsTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(address)));

                row++;
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error refreshing building list: {}", e.what());
        QMessageBox::warning(this, "Error", "Error refreshing building list: " + QString(e.what()));
    }

    // Clear selection
    _selectedBuildingId = -1;
    _buildingNameEdit->clear();
    _buildingAddressEdit->clear();
    _editBuildingButton->setEnabled(false);
    _deleteBuildingButton->setEnabled(false);

    // Update building combo in desks tab
    populateBuildingCombo();
}

void AdminDialog::addBuilding() {
    std::string name = _buildingNameEdit->text().trimmed().toStdString();
    std::string address = _buildingAddressEdit->text().trimmed().toStdString();

    if (name.empty()) {
        QMessageBox::warning(this, "Error", "Building name cannot be empty.");
        return;
    }

    try {
        // Use the new method
        bool success = _apiClient.addBuilding(name, address);

        if (success) {
            QMessageBox::information(this, "Success", "Building added successfully.");
            refreshBuildingList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to add building.");
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error adding building: {}", e.what());
        QMessageBox::warning(this, "Error", "Error adding building: " + QString(e.what()));
    }
}

void AdminDialog::editBuilding() {
    if (_selectedBuildingId < 0) {
        QMessageBox::warning(this, "Error", "No building selected.");
        return;
    }

    std::string name = _buildingNameEdit->text().trimmed().toStdString();
    std::string address = _buildingAddressEdit->text().trimmed().toStdString();

    if (name.empty()) {
        QMessageBox::warning(this, "Error", "Building name cannot be empty.");
        return;
    }

    try {
        // Use the new method
        bool success = _apiClient.updateBuilding(_selectedBuildingId, name, address);

        if (success) {
            QMessageBox::information(this, "Success", "Building updated successfully.");
            refreshBuildingList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update building.");
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error updating building: {}", e.what());
        QMessageBox::warning(this, "Error", "Error updating building: " + QString(e.what()));
    }
}

void AdminDialog::deleteBuilding() {
    if (_selectedBuildingId < 0) {
        QMessageBox::warning(this, "Error", "No building selected.");
        return;
    }

    // Confirm deletion
    QMessageBox::StandardButton confirm = QMessageBox::question(
        this, "Confirm Deletion",
        "Are you sure you want to delete this building?\nThis cannot be undone.",
        QMessageBox::Yes | QMessageBox::No
    );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    try {
        // Use the new method
        bool success = _apiClient.deleteBuilding(_selectedBuildingId);

        if (success) {
            QMessageBox::information(this, "Success", "Building deleted successfully.");
            refreshBuildingList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete building.");
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error deleting building: {}", e.what());
        QMessageBox::warning(this, "Error", "Error deleting building: " + QString(e.what()));
    }
}

void AdminDialog::selectBuilding(int row, int column) {
    Q_UNUSED(column);

    if (row >= 0 && row < _buildingsTable->rowCount()) {
        QTableWidgetItem *idItem = _buildingsTable->item(row, 0);
        QTableWidgetItem *nameItem = _buildingsTable->item(row, 1);
        QTableWidgetItem *addressItem = _buildingsTable->item(row, 2);

        if (idItem && nameItem && addressItem) {
            _selectedBuildingId = idItem->text().toInt();
            _buildingNameEdit->setText(nameItem->text());
            _buildingAddressEdit->setText(addressItem->text());

            _editBuildingButton->setEnabled(true);
            _deleteBuildingButton->setEnabled(true);
        }
    }
}

void AdminDialog::refreshDeskList() {
    _desksTable->clearContents();
    _desksTable->setRowCount(0);

    try {
        // Use the higher-level methods or public executeRequest
        json response = _apiClient.executeRequest("GET", "/api/desks");

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("desks") && response["desks"].is_array()) {
            const auto &desks = response["desks"];
            _desksTable->setRowCount(desks.size());

            int row = 0;
            for (const auto &desk: desks) {
                int id = desk.contains("id") ? desk["id"].get<int>() : 0;
                std::string deskId = desk.contains("deskId") ? desk["deskId"].get<std::string>() : "";

                // Get building ID and name
                std::string buildingId;
                if (desk.contains("buildingId")) {
                    if (desk["buildingId"].is_string()) {
                        buildingId = desk["buildingId"].get<std::string>();
                    } else if (desk["buildingId"].is_number()) {
                        buildingId = std::to_string(desk["buildingId"].get<int>());
                    }
                }

                // Find building name from ID
                QString buildingName = "Unknown";
                for (int i = 0; i < _buildingCombo->count(); i++) {
                    if (_buildingCombo->itemData(i).toString().toStdString() == buildingId) {
                        buildingName = _buildingCombo->itemText(i);
                        break;
                    }
                }

                int floorNumber = desk.contains("floorNumber") ? desk["floorNumber"].get<int>() : 0;
                int locationX = desk.contains("locationX") ? desk["locationX"].get<int>() : 0;
                int locationY = desk.contains("locationY") ? desk["locationY"].get<int>() : 0;

                _desksTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
                _desksTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(deskId)));
                _desksTable->setItem(row, 2, new QTableWidgetItem(buildingName));
                _desksTable->setItem(row, 3, new QTableWidgetItem(QString::number(floorNumber)));
                _desksTable->setItem(row, 4, new QTableWidgetItem(QString::number(locationX)));
                _desksTable->setItem(row, 5, new QTableWidgetItem(QString::number(locationY)));

                row++;
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error refreshing desk list: {}", e.what());
        QMessageBox::warning(this, "Error", "Error refreshing desk list: " + QString(e.what()));
    }

    // Clear selection
    _selectedDeskId = -1;
    _deskIdEdit->clear();
    _floorNumberSpin->setValue(1);
    _locationXSpin->setValue(0);
    _locationYSpin->setValue(0);
    _editDeskButton->setEnabled(false);
    _deleteDeskButton->setEnabled(false);
}

void AdminDialog::addDesk() {
    std::string deskId = _deskIdEdit->text().trimmed().toStdString();

    if (deskId.empty()) {
        QMessageBox::warning(this, "Error", "Desk number cannot be empty.");
        return;
    }

    if (_buildingCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Error", "Please select a building.");
        return;
    }

    int buildingId = _buildingCombo->currentData().toInt();
    int floorNumber = _floorNumberSpin->value();
    int locationX = _locationXSpin->value();
    int locationY = _locationYSpin->value();

    try {
        // Use the new method
        bool success = _apiClient.addDesk(deskId, buildingId, floorNumber, locationX, locationY);

        if (success) {
            QMessageBox::information(this, "Success", "Desk added successfully.");
            refreshDeskList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to add desk.");
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error adding desk: {}", e.what());
        QMessageBox::warning(this, "Error", "Error adding desk: " + QString(e.what()));
    }
}

void AdminDialog::editDesk() {
    if (_selectedDeskId < 0) {
        QMessageBox::warning(this, "Error", "No desk selected.");
        return;
    }

    std::string deskId = _deskIdEdit->text().trimmed().toStdString();

    if (deskId.empty()) {
        QMessageBox::warning(this, "Error", "Desk number cannot be empty.");
        return;
    }

    if (_buildingCombo->currentIndex() < 0) {
        QMessageBox::warning(this, "Error", "Please select a building.");
        return;
    }

    int buildingId = _buildingCombo->currentData().toInt();
    int floorNumber = _floorNumberSpin->value();
    int locationX = _locationXSpin->value();
    int locationY = _locationYSpin->value();

    try {
        // Use the new method
        bool success = _apiClient.updateDesk(_selectedDeskId, deskId, buildingId, floorNumber, locationX, locationY);

        if (success) {
            QMessageBox::information(this, "Success", "Desk updated successfully.");
            refreshDeskList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to update desk.");
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error updating desk: {}", e.what());
        QMessageBox::warning(this, "Error", "Error updating desk: " + QString(e.what()));
    }
}

void AdminDialog::deleteDesk() {
    if (_selectedDeskId < 0) {
        QMessageBox::warning(this, "Error", "No desk selected.");
        return;
    }

    // Confirm deletion
    QMessageBox::StandardButton confirm = QMessageBox::question(
        this, "Confirm Deletion",
        "Are you sure you want to delete this desk?\nThis cannot be undone.",
        QMessageBox::Yes | QMessageBox::No
    );

    if (confirm != QMessageBox::Yes) {
        return;
    }

    try {
        // Use the new method
        bool success = _apiClient.deleteDesk(_selectedDeskId);

        if (success) {
            QMessageBox::information(this, "Success", "Desk deleted successfully.");
            refreshDeskList();
        } else {
            QMessageBox::warning(this, "Error", "Failed to delete desk.");
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error deleting desk: {}", e.what());
        QMessageBox::warning(this, "Error", "Error deleting desk: " + QString(e.what()));
    }
}

void AdminDialog::selectDesk(int row, int column) {
    Q_UNUSED(column);

    if (row >= 0 && row < _desksTable->rowCount()) {
        QTableWidgetItem *idItem = _desksTable->item(row, 0);
        QTableWidgetItem *deskIdItem = _desksTable->item(row, 1);
        QTableWidgetItem *buildingItem = _desksTable->item(row, 2);
        QTableWidgetItem *floorItem = _desksTable->item(row, 3);
        QTableWidgetItem *xItem = _desksTable->item(row, 4);
        QTableWidgetItem *yItem = _desksTable->item(row, 5);

        if (idItem && deskIdItem && buildingItem && floorItem && xItem && yItem) {
            _selectedDeskId = idItem->text().toInt();
            _deskIdEdit->setText(deskIdItem->text());

            // Find building ID from name
            QString buildingName = buildingItem->text();
            for (int i = 0; i < _buildingCombo->count(); i++) {
                if (_buildingCombo->itemText(i) == buildingName) {
                    _buildingCombo->setCurrentIndex(i);
                    break;
                }
            }

            _floorNumberSpin->setValue(floorItem->text().toInt());
            _locationXSpin->setValue(xItem->text().toInt());
            _locationYSpin->setValue(yItem->text().toInt());

            _editDeskButton->setEnabled(true);
            _deleteDeskButton->setEnabled(true);
        }
    }
}

void AdminDialog::populateBuildingCombo() {
    _buildingCombo->clear();

    try {
        // Use the higher-level methods or public executeRequest
        json response = _apiClient.executeRequest("GET", "/api/buildings");

        if (response.contains("status") && response["status"] == "success" &&
            response.contains("buildings") && response["buildings"].is_array()) {
            const auto &buildings = response["buildings"];

            for (const auto &building: buildings) {
                int id = building.contains("id") ? building["id"].get<int>() : 0;
                std::string name = building.contains("name") ? building["name"].get<std::string>() : "";

                _buildingCombo->addItem(QString::fromStdString(name), id);
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("Error populating building combo: {}", e.what());
    }
}
