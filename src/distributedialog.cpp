//
// Created by 小火锅 on 24-12-7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_distributeDialog.h" resolved

#include "../inc/distributedialog.h"
#include "ui_distributeDialog.h"
#include "../inc/Buildings.h"
#include "../inc/Dormitory.h"


distributeDialog::distributeDialog(const Account& account, QWidget *parent) :
    account(account), QDialog(parent), ui(new Ui::distributeDialog) {
    ui->setupUi(this);
    std::set<QString> loc;
    this->setWindowTitle("宿舍分配");
    Buildings::getLoc(account.get_gender(), loc);
    for(auto& l : loc)
        ui->locCombo->addItem(l);
    ui->buildingCombo->setEditable(false);
}

distributeDialog::~distributeDialog() {
    delete ui;
}

void distributeDialog::on_locCombo_currentTextChanged(const QString &loc) {
    static QVector<QString> buildings;
    ui->buildingCombo->clear();
    ui->buildingCombo->addItem("");
    qDebug() << loc;
    Buildings::getBuildings(account.get_gender(), loc, buildings);
    for (const auto& building : buildings) {
        ui->buildingCombo->addItem(building);
    }
    ui->buildingCombo->setEditable(false);
}

void distributeDialog::on_confirmButton_clicked() {
    auto loc = ui->locCombo->currentText(), bldg = ui->buildingCombo->currentText();
    int dorm_id = ui->dormEdit->text().toInt();
    if(loc.isEmpty() || bldg.isEmpty() || !dorm_id) {
        QMessageBox::warning(this, "distribute error", "请填写所有信息！");
        return;
    }
    if(Dormitory::checkIn_and_modify(account.get_id(), loc, bldg, dorm_id)) {
        QMessageBox::information(this, "distribute success", "宿舍分配成功！");
        this->close();
        return;
    }
    QMessageBox::information(this, "distribute error", "宿舍分配失败！");
}

