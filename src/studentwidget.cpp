//
// Created by 小火锅 on 24-11-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_studentWidget.h" resolved

#include "../inc/studentwidget.h"
#include "../inc/dormManage.h"
#include "../inc/Dormitory.h"

#include <QtWidgets/QVBoxLayout>
#include <memory>
#include <QPalette>
#include "ui_studentWidget.h"
#include "../inc/Buildings.h"
#include "../inc/checkOut.h"
#include "../inc/detaildialog.h"


studentWidget::studentWidget(std::shared_ptr<Account> account, QWidget *parent) :
    QWidget(parent), ui(new Ui::studentWidget), account(account), perInfotable(new QTableWidget(this)), dormInfotable(new QTableWidget(this)),
    recordInfotable(new QTableWidget(this)) {
    if(account == nullptr) {
        QMessageBox::warning(this, "studentWidger error", "无法获取学生信息");
        QCoreApplication::exit(-1);
    }
    ui->setupUi(this);
    ui->dormitoryButton->setStyleSheet(buttonStylesheet());
    ui->recordButton->setStyleSheet(buttonStylesheet());
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    ui->logOutButton->setStyleSheet(buttonStylesheet());
    ui->perButton->setStyleSheet("background-color: #D8BFD8;");
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    this->setWindowTitle("宿舍管理系统-学生界面");
    PerInfoTable();
    auto* perLayout = new QVBoxLayout(ui->perInfoPage);
    perLayout->addWidget(perInfotable);
    perLayout->addWidget(perInfotable, 0, Qt::AlignTop | Qt::AlignHCenter);
    ui->perInfoPage->setLayout(perLayout);
    ui->stackedWidget->setCurrentWidget(ui->perInfoPage);

    dormInfotable->setFixedSize(300, 250);
    auto* dormLayout = new QVBoxLayout(ui->dormInfoPage);
    dormLayout->addWidget(dormInfotable, 0, Qt::AlignTop | Qt::AlignHCenter);
    ui->dormInfoPage->setLayout(dormLayout);

    //recordInfotable->setFixedSize(1000, 400);
    auto* recordLayout = new QVBoxLayout(ui->recordInfoPage);
    recordLayout->addWidget(recordInfotable);
    recordLayout->setAlignment(Qt::AlignCenter);
    ui->recordInfoPage->setLayout(recordLayout);

    darkModeTimer = new QTimer(this);
    connect(darkModeTimer, &QTimer::timeout, this, &studentWidget::checkDarkMode);
    darkModeTimer->start(1000);
}

studentWidget::~studentWidget() {
    delete ui;
}

void studentWidget::PerInfoTable() {
    perInfotable->setRowCount(4);
    perInfotable->setColumnCount(2);
    perInfotable->setWindowTitle("个人信息");
    perInfotable->setHorizontalHeaderLabels(QStringList() << "名称" << "值");
    if (perInfotable->item(0, 0))
        perInfotable->item(0, 0)->setText("姓名");
    else
        perInfotable->setItem(0, 0, new QTableWidgetItem("姓名"));

    if (perInfotable->item(0, 1))
        perInfotable->item(0, 1)->setText(account->get_name());
    else
        perInfotable->setItem(0, 1, new QTableWidgetItem(account->get_name()));

    if (perInfotable->item(1, 0))
        perInfotable->item(1, 0)->setText("学号");
    else
        perInfotable->setItem(1, 0, new QTableWidgetItem("学号"));

    if (perInfotable->item(1, 1))
        perInfotable->item(1, 1)->setText(account->get_id());
    else
        perInfotable->setItem(1, 1, new QTableWidgetItem(account->get_id()));

    if (perInfotable->item(2, 0))
        perInfotable->item(2, 0)->setText("性别");
    else
        perInfotable->setItem(2, 0, new QTableWidgetItem("性别"));

    if (perInfotable->item(2, 1))
        perInfotable->item(2, 1)->setText((account->get_gender() == Male) ? "男" : "女");
    else
        perInfotable->setItem(2, 1, new QTableWidgetItem((account->get_gender() == Male) ? "男" : "女"));

    if (perInfotable->item(3, 0))
        perInfotable->item(3, 0)->setText("电话号码");
    else
        perInfotable->setItem(3, 0, new QTableWidgetItem("电话号码"));

    if (perInfotable->item(3, 1))
        perInfotable->item(3, 1)->setText(account->get_telephone());
    else
        perInfotable->setItem(3, 1, new QTableWidgetItem(account->get_telephone()));

    perInfotable->setColumnWidth(0, 150);
    perInfotable->setColumnWidth(1, 100);

    perInfotable->setFixedSize(300, 200);
    perInfotable->setEditTriggers(QAbstractItemView::NoEditTriggers);

}

QString studentWidget::buttonStylesheet() {
    if (isDarkMode()) {
        return R"(
            QPushButton {
                background-color: black;
                border: none;
            }
            QPushButton:hover {
                background-color: gray;
            }
        )";
    }
    else {
        return R"(
            QPushButton {
                background-color: white;
                border: none;
            }
            QPushButton:hover {
                background-color: lightgray;
            }
        )";
    }
}

void studentWidget::on_dormitoryButton_clicked() {
    if(ui->stackedWidget->currentWidget() == ui->dormInfoPage)
        return;
    dormInfo = Dormitory::searchDorm(account->get_id());
    if(dormInfo == nullptr) {
        QMessageBox::information(this, "dorm info not found", "宿舍未分配");
        return;
    }
    manageInfo = dormManage::dormInfo(dormInfo->getManageId());
    auto buildInfo = Buildings::split(manageInfo->building);
    dormInfotable->setRowCount(6);
    dormInfotable->setColumnCount(2);
    dormInfotable->setWindowTitle("宿舍信息");
    dormInfotable->setHorizontalHeaderLabels(QStringList() << "名称" << "值");
    if (dormInfotable->item(0, 0))
        dormInfotable->item(0, 0)->setText("园区");
    else
        dormInfotable->setItem(0, 0, new QTableWidgetItem("园区"));

    if (dormInfotable->item(0, 1))
        dormInfotable->item(0, 1)->setText(buildInfo.first);
    else
    dormInfotable->setItem(0, 1, new QTableWidgetItem(buildInfo.first));

    if (dormInfotable->item(1, 0))
        dormInfotable->item(1, 0)->setText("楼栋");
    else
        dormInfotable->setItem(1, 0, new QTableWidgetItem("楼栋"));

    if (dormInfotable->item(1, 1))
        dormInfotable->item(1, 1)->setText(buildInfo.second);
    else
        dormInfotable->setItem(1, 1, new QTableWidgetItem(buildInfo.second));

    if (dormInfotable->item(2, 0))
        dormInfotable->item(2, 0)->setText("宿舍号");
    else
        dormInfotable->setItem(2, 0, new QTableWidgetItem("宿舍号"));

    if (dormInfotable->item(2, 1))
        dormInfotable->item(2, 1)->setText(QString::number(manageInfo->dorm_id));
    else
        dormInfotable->setItem(2, 1, new QTableWidgetItem(QString::number(manageInfo->dorm_id)));

    if (dormInfotable->item(3, 0))
        dormInfotable->item(3, 0)->setText("宿舍类型");
    else
        dormInfotable->setItem(3, 0, new QTableWidgetItem("宿舍类型"));

    if (dormInfotable->item(3, 1))
        dormInfotable->item(3, 1)->setText(dormManage::capacityToName(manageInfo->capacity));
    else
        dormInfotable->setItem(3, 1, new QTableWidgetItem(dormManage::capacityToName(manageInfo->capacity)));

    if (dormInfotable->item(4, 0))
        dormInfotable->item(4, 0)->setText("入住状态");
    else
        dormInfotable->setItem(4, 0, new QTableWidgetItem("入住状态"));

    if (dormInfotable->item(4, 1))
        dormInfotable->item(4, 1)->setText(statusToString[dormInfo->getStatus()]);
    else
        dormInfotable->setItem(4, 1, new QTableWidgetItem(statusToString[dormInfo->getStatus()]));
    if(dormInfo->getStatus() == CheckedIn) dormInfotable->item(4, 1)->setForeground(QBrush(Qt::green));
    else dormInfotable->item(4, 1)->setForeground(QBrush(Qt::yellow));

    if (dormInfotable->item(5, 0))
        dormInfotable->item(5, 0)->setText("入住人数");
    else
        dormInfotable->setItem(5, 0, new QTableWidgetItem("入住人数"));

    if (dormInfotable->item(5, 1))
        dormInfotable->item(5, 1)->setText(QString::number(manageInfo->curCheckins));
    else
        dormInfotable->setItem(5, 1, new QTableWidgetItem(QString::number(manageInfo->curCheckins)));

    dormInfotable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->recordButton->setStyleSheet(buttonStylesheet());
    ui->perButton->setStyleSheet(buttonStylesheet());
    ui->stackedWidget->setCurrentWidget(ui->dormInfoPage);
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    ui->dormitoryButton->setStyleSheet("background-color: #D8BFD8;");
}


void studentWidget::on_confirmButton_clicked() {
    if(dormInfo->getStatus() == CheckedIn) {
        QMessageBox::information(this, "already checked in", "已经确认入住");
        return;
    }
    if(dormInfo->Confirm()) {
        QMessageBox::information(this, "checked in successful", "确认入住成功");
        dormInfotable->item(4, 1)->setForeground(QBrush(Qt::green));
        dormInfotable->item(4, 1)->setText("已入住");
    }
    else {
        QMessageBox::critical(this, "checked in error", "确认入住失败");
    }
}

void studentWidget::on_recordButton_clicked() {
    QVector<Dormitory> record;
    Dormitory::getRecord(account->get_id(), record);
    if(record.empty()) {
        QMessageBox::information(this, "search getRecord end", "您没有入住记录");
        return;
    }
    recordInfotable->setRowCount(record.size());
    recordInfotable->setColumnCount(6);
    recordInfotable->setWindowTitle("入住记录");
    recordInfotable->setHorizontalHeaderLabels(QStringList() << "园区" << "楼栋" << "宿舍号" << "入住状态" << "入住时间" << "退宿时间");
    for(int i = 0; i < record.size(); i++) {
        std::shared_ptr<dormManage> dMInfo = dormManage::dormInfo(record[i].getManageId());
        if (dMInfo) {
            auto bInfo = Buildings::split(dMInfo->building);
            if(recordInfotable->item(i, 0))
                recordInfotable->item(i, 0)->setText(bInfo.first);
            else
                recordInfotable->setItem(i, 0, new QTableWidgetItem(bInfo.first));
            if(recordInfotable->item(i, 1))
                recordInfotable->item(i, 1)->setText(bInfo.second);
            else
                recordInfotable->setItem(i, 1, new QTableWidgetItem(bInfo.second));
            if(recordInfotable->item(i, 2))
                recordInfotable->item(i, 2)->setText(QString::number(dMInfo->dorm_id));
            else
                recordInfotable->setItem(i, 2, new QTableWidgetItem(QString::number(dMInfo->dorm_id)));
        }
        else {
            recordInfotable->setItem(i, 0, new QTableWidgetItem("宿舍已删除"));
        }
        if(recordInfotable->item(i, 3))
            recordInfotable->item(i, 3)->setText(statusToString[record[i].getStatus()]);
        else
            recordInfotable->setItem(i, 3, new QTableWidgetItem(statusToString[record[i].getStatus()]));
        if(record[i].getStatus() == CheckedIn)
            recordInfotable->item(i, 3)->setForeground(QBrush(Qt::green));
        else if(record[i].getStatus() == CheckedOut)
            recordInfotable->item(i, 3)->setForeground(QBrush(Qt::red));
        else
            recordInfotable->item(i, 3)->setForeground(QBrush(Qt::yellow));
        if(recordInfotable->item(i, 4))
            recordInfotable->item(i, 4)->setText(record[i].getCID());
        else
            recordInfotable->setItem(i, 4, new QTableWidgetItem(record[i].getCID()));
        if(recordInfotable->item(i, 5))
            recordInfotable->item(i, 5)->setText(record[i].getCOD());
        else
            recordInfotable->setItem(i, 5, new QTableWidgetItem(record[i].getCOD()));
    }
    ui->recordButton->setStyleSheet("background-color: #D8BFD8;");
    ui->dormitoryButton->setStyleSheet(buttonStylesheet());
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    ui->perButton->setStyleSheet(buttonStylesheet());
    ui->stackedWidget->setCurrentWidget(ui->recordInfoPage);
    recordInfotable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void studentWidget::on_perButton_clicked() {
    PerInfoTable();
    ui->stackedWidget->setCurrentWidget(ui->perInfoPage);
    ui->perButton->setStyleSheet("background-color: #D8BFD8;");
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    ui->recordButton->setStyleSheet(buttonStylesheet());
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    ui->dormitoryButton->setStyleSheet(buttonStylesheet());
}

void studentWidget::on_checkOutButton_clicked() {
    ui->perButton->setStyleSheet(buttonStylesheet());
    ui->recordButton->setStyleSheet(buttonStylesheet());
    ui->dormitoryButton->setStyleSheet(buttonStylesheet());
    ui->checkOutButton->setStyleSheet("background-color: #D8BFD8;");
    ui->stackedWidget->setCurrentWidget(ui->checkoutPage);
    ui->checkoutTab->setCurrentIndex(0);
    auto* tableLayout = new QVBoxLayout(ui->historyTab);
    tableLayout->addWidget(ui->arRecordtable);
    tableLayout->setAlignment(Qt::AlignCenter);
    ui->historyTab->setLayout(tableLayout);
}

void studentWidget::on_sendButton_clicked() {
    if(ui->detailEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "empty detail", "请填写退宿原因");
        return;
    }
    if(Dormitory::getManageId(account->get_id()) == -1) {
        QMessageBox::warning(this, "unable to check out", "当前状态下，不能申请退宿！");
        return;
    }
    if(checkOut::preApply(account->get_id())) {
        if(!sendMessageBox() || !checkOut::cancel(account->get_id())) {
            QMessageBox::warning(this, "send error", "退宿申请失败");
            return;
        }
    }
    if(!checkOut::apply(account->get_id(), ui->detailEdit->toPlainText(), checkOut::stringToReason[ui->reasonCombo->currentText()])) {
        QMessageBox::warning(this, "send fail", "退宿请求发送失败");
        return;
    }
    QMessageBox::information(this, "checkout apply success", "退宿请求发送成功！");
    ui->detailEdit->clear();
    ui->reasonCombo->setCurrentIndex(0);
}

bool studentWidget::sendMessageBox() {
    QMessageBox messageBox(this);
    messageBox.setWindowTitle("提示");
    messageBox.setText("您已经有退宿申请正在审核，是否需要取消申请并重新推送？");
    QPushButton *yesButton = messageBox.addButton("确认", QMessageBox::YesRole);
    QPushButton *noButton = messageBox.addButton("取消", QMessageBox::NoRole);
    messageBox.exec();
    if (messageBox.clickedButton() == yesButton) {
        return true;
    }
    else if (messageBox.clickedButton() == noButton) {
        return false;
    }
    return false;
}

void setItem(QTableWidget* table, int row, int col, const QString& text) {
    if(table->item(row, col))
        table->item(row, col)->setText(text);
    else
        table->setItem(row, col, new QTableWidgetItem(text));
}

void studentWidget::on_checkoutTab_currentChanged(int index) {
    if(index == 1) {
        qDebug() << "查询历史记录界面";
        QVector<QPair<checkOut, Reply>> record;
        checkOut::record(account->get_id(), record);
        ui->arRecordtable->setRowCount(record.size());
        ui->arRecordtable->setColumnCount(4);
        ui->arRecordtable->setWindowTitle("申请退宿记录");
        ui->arRecordtable->setColumnWidth(0, 150);
        ui->arRecordtable->setHorizontalHeaderLabels(QStringList() << "申请时间" << "理由" << "处理进度" << "详细信息");
        for(int i = 0; i < record.size(); i++) {
            setItem(ui->arRecordtable, i, 0, record[i].first.apply_date.toString("yyyy-MM-dd hh:mm"));
            setItem(ui->arRecordtable, i, 1, checkOut::reasonToString[record[i].first.reason]);
            setItem(ui->arRecordtable, i, 2, checkOut::progressToString[record[i].first.progress]);
            QPushButton *detailsButton = new QPushButton("查看", this);
            ui->arRecordtable->setCellWidget(i, 3, detailsButton);
            connect(detailsButton, &QPushButton::clicked, this, [=]() {
                detailDialog* dW = new detailDialog(ui->arRecordtable, record[i].first, record[i].second);
                dW->setWindowTitle("详细信息");
                dW->exec();
            });
        }
        ui->arRecordtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }
}

void studentWidget::on_logOutButton_clicked() {
    emit stuSwitchBack();
}

void studentWidget::checkDarkMode() {
    bool newDarkMode = isDarkMode();
    if (newDarkMode != currentDarkMode) {
        currentDarkMode = newDarkMode;
        ui->perButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->perInfoPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->checkOutButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->checkoutPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->dormitoryButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->dormInfoPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->recordButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->recordInfoPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->logOutButton->setStyleSheet(buttonStylesheet());
    }
}