//
// Created by 小火锅 on 24-12-6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_dormDialog.h" resolved

#include "../inc/dormdialog.h"
#include "ui_dormDialog.h"


dormDialog::dormDialog(QWidget *parent, const Account& account1,
        std::shared_ptr<Dormitory> dormitory1, std::shared_ptr<dormManage> dorm_manage, bool modify)
        : QDialog(parent), ui(new Ui::dormDialog), account(account1),
        dormitory(dormitory1), dorm_manage(dorm_manage), modify(modify) {
    ui->setupUi(this);
    this->setWindowTitle("宿舍信息");
    QString name = "姓名：" + account.get_name();
    QString id = "学号：" + account.get_id();
    QString tel = "电话：" + account.get_telephone();
    QString gender = "性别：";
    gender += (account.get_gender() == Female ? "女" : "男");

    ui->name->setText(name);
    ui->id->setText(id);
    ui->tel->setText(tel);
    ui->gender->setText(gender);

    if (dormitory && dorm_manage && dormitory->getManageId() != -1) {
        QString loc = "园区：" + Buildings::split(dorm_manage->building).first;
        QString building = "楼栋：" + Buildings::split(dorm_manage->building).second;
        QString dorm_id = "宿舍号：" + QString::number(dorm_manage->dorm_id);
        ui->loc->setText(loc);
        ui->building->setText(building);
        ui->dorm_id->setText(dorm_id);
    }
    else {
        ui->loc->setText("该学生未入住");
        ui->building->clear();
        ui->dorm_id->clear();
    }
    QVector<Dormitory> record;
    Dormitory::getRecord(account.get_id(), record);
    ui->recordTable->setRowCount(record.size());
    ui->recordTable->setColumnCount(6);
    ui->recordTable->setWindowTitle("入住记录");
    ui->recordTable->setHorizontalHeaderLabels(QStringList() << "园区" << "楼栋" << "宿舍号" << "入住状态" << "入住时间" << "退宿时间");

    for (int i = 0; i < record.size(); i++) {
        std::shared_ptr<dormManage> dMInfo = dormManage::dormInfo(record[i].getManageId());
        if (dMInfo) {
            auto bInfo = Buildings::split(dMInfo->building);
            ui->recordTable->setItem(i, 0, new QTableWidgetItem(bInfo.first));
            ui->recordTable->setItem(i, 1, new QTableWidgetItem(bInfo.second));
            ui->recordTable->setItem(i, 2, new QTableWidgetItem(QString::number(dMInfo->dorm_id)));
        }
        else {
            ui->recordTable->setItem(i, 0, new QTableWidgetItem("宿舍已删除"));
        }
        ui->recordTable->setItem(i, 3, new QTableWidgetItem(statusToString[record[i].getStatus()]));
        if (record[i].getStatus() == CheckedIn)
            ui->recordTable->item(i, 3)->setForeground(QBrush(Qt::green));
        else if (record[i].getStatus() == CheckedOut)
            ui->recordTable->item(i, 3)->setForeground(QBrush(Qt::red));
        else
            ui->recordTable->item(i, 3)->setForeground(QBrush(Qt::yellow));
        ui->recordTable->setItem(i, 4, new QTableWidgetItem(record[i].getCID()));
        ui->recordTable->setItem(i, 5, new QTableWidgetItem(record[i].getCOD()));
    }
    ui->recordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

dormDialog::~dormDialog() {
    delete ui;
}
