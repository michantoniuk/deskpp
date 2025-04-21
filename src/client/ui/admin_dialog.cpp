#include "admin_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QHeaderView>

AdminDialog::AdminDialog(ApiClient &apiClient, QWidget *parent)
    : QDialog(parent), apiClient(apiClient) {
    setWindowTitle("Admin Management");
    resize(700, 500);

    tabWidget = new QTabWidget(this);

    // Buildings tab
    auto buildingsTab = new QWidget();
    auto buildingsLayout = new QVBoxLayout(buildingsTab);

    buildingsTable = new QTableWidget(0, 3);
    buildingsTable->setHorizontalHeaderLabels({"ID", "Name", "Address"});
    buildingsTable->horizontalHeader()->setStretchLastSection(true);
    buildingsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(buildingsTable, &QTableWidget::cellClicked, this, &AdminDialog::selectBuilding);
    buildingsLayout->addWidget(buildingsTable);

    // Building form
    auto buildingForm = new QFormLayout();
    buildingNameEdit = new QLineEdit();
    buildingAddressEdit = new QLineEdit();
    buildingForm->addRow("Name:", buildingNameEdit);
    buildingForm->addRow("Address:", buildingAddressEdit);
    buildingsLayout->addLayout(buildingForm);

    // Building buttons
    auto buildingBtnLayout = new QHBoxLayout();

    auto addBuildingBtn = new QPushButton("Add", this);
    connect(addBuildingBtn, &QPushButton::clicked, this, &AdminDialog::addBuilding);
    buildingBtnLayout->addWidget(addBuildingBtn);

    auto updateBuildingBtn = new QPushButton("Update", this);
    connect(updateBuildingBtn, &QPushButton::clicked, this, &AdminDialog::updateBuilding);
    buildingBtnLayout->addWidget(updateBuildingBtn);

    auto deleteBuildingBtn = new QPushButton("Delete", this);
    connect(deleteBuildingBtn, &QPushButton::clicked, this, &AdminDialog::deleteBuilding);
    buildingBtnLayout->addWidget(deleteBuildingBtn);

    auto refreshBuildingsBtn = new QPushButton("Refresh", this);
    connect(refreshBuildingsBtn, &QPushButton::clicked, this, &AdminDialog::refreshBuildings);
    buildingBtnLayout->addWidget(refreshBuildingsBtn);

    buildingsLayout->addLayout(buildingBtnLayout);

    // Desks tab
    auto desksTab = new QWidget();
    auto desksLayout = new QVBoxLayout(desksTab);

    desksTable = new QTableWidget(0, 6);
    desksTable->setHorizontalHeaderLabels({"ID", "Desk #", "Building", "Floor", "X", "Y"});
    desksTable->horizontalHeader()->setStretchLastSection(true);
    desksTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(desksTable, &QTableWidget::cellClicked, this, &AdminDialog::selectDesk);
    desksLayout->addWidget(desksTable);

    // Desk form
    auto deskForm = new QFormLayout();
    deskIdEdit = new QLineEdit();
    buildingCombo = new QComboBox();
    buildingCombo->addItem("Krakow A", 1);
    buildingCombo->addItem("Warsaw B", 2);

    floorSpin = new QSpinBox();
    floorSpin->setMinimum(1);
    floorSpin->setMaximum(100);

    xSpin = new QSpinBox();
    xSpin->setMinimum(0);
    xSpin->setMaximum(1000);

    ySpin = new QSpinBox();
    ySpin->setMinimum(0);
    ySpin->setMaximum(1000);

    deskForm->addRow("Desk #:", deskIdEdit);
    deskForm->addRow("Building:", buildingCombo);
    deskForm->addRow("Floor:", floorSpin);
    deskForm->addRow("X Position:", xSpin);
    deskForm->addRow("Y Position:", ySpin);
    desksLayout->addLayout(deskForm);

    // Desk buttons
    auto deskBtnLayout = new QHBoxLayout();

    auto addDeskBtn = new QPushButton("Add", this);
    connect(addDeskBtn, &QPushButton::clicked, this, &AdminDialog::addDesk);
    deskBtnLayout->addWidget(addDeskBtn);

    auto updateDeskBtn = new QPushButton("Update", this);
    connect(updateDeskBtn, &QPushButton::clicked, this, &AdminDialog::updateDesk);
    deskBtnLayout->addWidget(updateDeskBtn);

    auto deleteDeskBtn = new QPushButton("Delete", this);
    connect(deleteDeskBtn, &QPushButton::clicked, this, &AdminDialog::deleteDesk);
    deskBtnLayout->addWidget(deleteDeskBtn);

    auto refreshDesksBtn = new QPushButton("Refresh", this);
    connect(refreshDesksBtn, &QPushButton::clicked, this, &AdminDialog::refreshDesks);
    deskBtnLayout->addWidget(refreshDesksBtn);

    desksLayout->addLayout(deskBtnLayout);

    // Add tabs
    tabWidget->addTab(buildingsTab, "Buildings");
    tabWidget->addTab(desksTab, "Desks");

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);

    // Load initial data
    refreshBuildings();
    refreshDesks();
}

void AdminDialog::refreshBuildings() {
    buildingsTable->clearContents();
    buildingsTable->setRowCount(0);

    json response = apiClient.executeRequest("GET", "/api/buildings");

    if (response.contains("buildings") && response["buildings"].is_array()) {
        const auto &buildings = response["buildings"];
        buildingsTable->setRowCount(buildings.size());

        for (size_t i = 0; i < buildings.size(); i++) {
            const auto &building = buildings[i];
            int id = building.contains("id") ? building["id"].get<int>() : 0;
            std::string name = building.contains("name") ? building["name"].get<std::string>() : "";
            std::string address = building.contains("address") ? building["address"].get<std::string>() : "";

            buildingsTable->setItem(i, 0, new QTableWidgetItem(QString::number(id)));
            buildingsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(name)));
            buildingsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(address)));
        }
    }
}

void AdminDialog::addBuilding() {
    std::string name = buildingNameEdit->text().toStdString();
    std::string address = buildingAddressEdit->text().toStdString();

    if (apiClient.addBuilding(name, address)) {
        QMessageBox::information(this, "Success", "Building added");
        refreshBuildings();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add building");
    }
}

void AdminDialog::updateBuilding() {
    if (selectedBuildingId < 0) return;

    std::string name = buildingNameEdit->text().toStdString();
    std::string address = buildingAddressEdit->text().toStdString();

    if (apiClient.updateBuilding(selectedBuildingId, name, address)) {
        QMessageBox::information(this, "Success", "Building updated");
        refreshBuildings();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update building");
    }
}

void AdminDialog::deleteBuilding() {
    if (selectedBuildingId < 0) return;

    if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this building?")
        != QMessageBox::Yes) {
        return;
    }

    if (apiClient.deleteBuilding(selectedBuildingId)) {
        QMessageBox::information(this, "Success", "Building deleted");
        refreshBuildings();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete building");
    }
}

void AdminDialog::selectBuilding(int row, int column) {
    Q_UNUSED(column);

    if (row >= 0 && row < buildingsTable->rowCount()) {
        QTableWidgetItem *idItem = buildingsTable->item(row, 0);
        QTableWidgetItem *nameItem = buildingsTable->item(row, 1);
        QTableWidgetItem *addressItem = buildingsTable->item(row, 2);

        if (idItem && nameItem && addressItem) {
            selectedBuildingId = idItem->text().toInt();
            buildingNameEdit->setText(nameItem->text());
            buildingAddressEdit->setText(addressItem->text());
        }
    }
}

void AdminDialog::refreshDesks() {
    desksTable->clearContents();
    desksTable->setRowCount(0);

    json response = apiClient.executeRequest("GET", "/api/desks");

    if (response.contains("desks") && response["desks"].is_array()) {
        const auto &desks = response["desks"];
        desksTable->setRowCount(desks.size());

        for (size_t i = 0; i < desks.size(); i++) {
            const auto &desk = desks[i];

            int id = desk.contains("id") ? desk["id"].get<int>() : 0;
            std::string deskId = desk.contains("deskId") ? desk["deskId"].get<std::string>() : "";
            std::string buildingId = "1";
            if (desk.contains("buildingId")) {
                if (desk["buildingId"].is_string())
                    buildingId = desk["buildingId"].get<std::string>();
                else if (desk["buildingId"].is_number())
                    buildingId = std::to_string(desk["buildingId"].get<int>());
            }

            // Simple mapping for building name
            QString buildingName = buildingId == "1" ? "Krakow A" : "Warsaw B";

            int floor = desk.contains("floorNumber") ? desk["floorNumber"].get<int>() : 1;
            int x = desk.contains("locationX") ? desk["locationX"].get<int>() : 0;
            int y = desk.contains("locationY") ? desk["locationY"].get<int>() : 0;

            desksTable->setItem(i, 0, new QTableWidgetItem(QString::number(id)));
            desksTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(deskId)));
            desksTable->setItem(i, 2, new QTableWidgetItem(buildingName));
            desksTable->setItem(i, 3, new QTableWidgetItem(QString::number(floor)));
            desksTable->setItem(i, 4, new QTableWidgetItem(QString::number(x)));
            desksTable->setItem(i, 5, new QTableWidgetItem(QString::number(y)));
        }
    }
}

void AdminDialog::addDesk() {
    std::string deskId = deskIdEdit->text().toStdString();
    int buildingId = buildingCombo->currentData().toInt();
    int floor = floorSpin->value();
    int x = xSpin->value();
    int y = ySpin->value();

    if (apiClient.addDesk(deskId, buildingId, floor, x, y)) {
        QMessageBox::information(this, "Success", "Desk added");
        refreshDesks();
    }
}

void AdminDialog::selectDesk(int row, int column) {
    Q_UNUSED(column);

    if (row >= 0 && row < desksTable->rowCount()) {
        QTableWidgetItem *idItem = desksTable->item(row, 0);
        QTableWidgetItem *deskIdItem = desksTable->item(row, 1);
        QTableWidgetItem *buildingItem = desksTable->item(row, 2);
        QTableWidgetItem *floorItem = desksTable->item(row, 3);
        QTableWidgetItem *xItem = desksTable->item(row, 4);
        QTableWidgetItem *yItem = desksTable->item(row, 5);

        if (idItem && deskIdItem) {
            selectedDeskId = idItem->text().toInt();
            deskIdEdit->setText(deskIdItem->text());
            buildingCombo->setCurrentIndex(buildingItem->text() == "Krakow A" ? 0 : 1);
            floorSpin->setValue(floorItem->text().toInt());
            xSpin->setValue(xItem->text().toInt());
            ySpin->setValue(yItem->text().toInt());
        }
    }
}

void AdminDialog::updateDesk() {
    if (selectedDeskId < 0) return;

    std::string deskId = deskIdEdit->text().toStdString();
    int buildingId = buildingCombo->currentData().toInt();
    int floor = floorSpin->value();
    int x = xSpin->value();
    int y = ySpin->value();

    if (apiClient.updateDesk(selectedDeskId, deskId, buildingId, floor, x, y)) {
        QMessageBox::information(this, "Success", "Desk updated");
        refreshDesks();
    } else {
        QMessageBox::warning(this, "Error", "Failed to update desk");
    }
}

void AdminDialog::deleteDesk() {
    if (selectedDeskId < 0) return;

    if (QMessageBox::question(this, "Confirm Delete", "Are you sure you want to delete this desk?")
        != QMessageBox::Yes) {
        return;
    }

    if (apiClient.deleteDesk(selectedDeskId)) {
        QMessageBox::information(this, "Success", "Desk deleted");
        refreshDesks();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete desk");
    }
}
