//
// Created by 小火锅 on 24-11-27.
//

// You may need to build the project (run Qt uic code generator) to get "ui_adminWidget.h" resolved



#include <QtWidgets/QVBoxLayout>
#include <QVector>
#include <QPair>
#include "ui_adminWidget.h"
#include "../inc/Buildings.h"
#include "../inc/administratorWidget.h"
#include "../inc/dark.h"
#include "../inc/Dormitory.h"
#include "../inc/dormdialog.h"
#include "../inc/Account.h"
#include "../inc/distributedialog.h"
#include <QRegularExpression>

#include "../inc/detaildialog.h"
#include "../inc/replydialog.h"
class detailDialog;

administratorWidget::administratorWidget(std::shared_ptr<Account> account, QWidget *parent) :
    QWidget(parent), ui(new Ui::adminWidget), account(account), sumTable(new QTableWidget(5, 2, this)) {
    ui->setupUi(this);
    this->setWindowTitle("宿舍管理系统-管理员端");
    ui->stuInfoButton->setStyleSheet(buttonStylesheet());
    ui->checkOutButton->setStyleSheet(buttonStylesheet());
    ui->dormDistrubuteButton->setStyleSheet(buttonStylesheet());
    ui->dormManageButton->setStyleSheet(buttonStylesheet());
    ui->sumButton->setStyleSheet("background-color: #D8BFD8;");
    ui->logoutButton->setStyleSheet(buttonStylesheet());
    ui->stuTab->setCurrentIndex(0);
    buildSumTable();
    darkModeTimer = new QTimer(this);
    connect(darkModeTimer, &QTimer::timeout, this, &administratorWidget::checkDarkMode);
    darkModeTimer->start(1000);
}

administratorWidget::~administratorWidget() {
    delete ui;
}

void administratorWidget::updateSumTable() {
    int beds = Buildings::counts("beds");
    int check_ins = Buildings::counts("check_ins");
    int students = Account::countNum(Student);
    int check_outs = Dormitory::countCheckout();
    sumTable->item(0, 1)->setText(QString::number(students));
    sumTable->item(1, 1)->setText(QString::number(beds));
    sumTable->item(2, 1)->setText(QString::number(100 * check_ins / beds) + "%");
    sumTable->item(3, 1)->setText(QString::number(100 * check_ins / students) + "%");
    sumTable->item(4, 1)->setText(QString::number(100 * check_outs / students) + "%");
}

void administratorWidget::buildSumTable() {
    sumTable->setWindowTitle("汇总信息");
    sumTable->setHorizontalHeaderLabels(QStringList() << "类别" << "统计值");

    QStringList rowLabels = {"学生总数", "床数", "宿舍使用率", "入住率", "退宿率"};
    for (int row = 0; row < sumTable->rowCount(); ++row) {
        sumTable->setItem(row, 0, new QTableWidgetItem(rowLabels[row]));
        sumTable->setItem(row, 1, new QTableWidgetItem());
    }

    updateSumTable();

    sumTable->setColumnWidth(0, 150);
    sumTable->setColumnWidth(1, 100);
    sumTable->setFixedSize(300, 200);
    sumTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* sumlayout = new QVBoxLayout(ui->sumPage);
    sumlayout->addWidget(sumTable, 0, Qt::AlignTop | Qt::AlignHCenter);
    ui->sumPage->setLayout(sumlayout);
    ui->stackedWidget->setCurrentWidget(ui->sumPage);

    /*auto* stuLayout = new QVBoxLayout(ui->stuPage);
    stuLayout->addWidget(stuInfoTable);
    stuLayout->setAlignment(Qt::AlignCenter);
    ui->stuPage->setLayout(stuLayout);*/
}

QString administratorWidget::buttonStylesheet() {
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

void administratorWidget::setItem(QTableWidget* table, int row, int col, const QString& text) {
    if(table->item(row, col))
        table->item(row, col)->setText(text);
    else
        table->setItem(row, col, new QTableWidgetItem(text));
}

void administratorWidget::on_stuInfoButton_clicked() {
    Account::StuInfo(stuInfo);
    build_or_update_stuinfoTable(stuInfo, ui->stuInfoTable);
    ui->stackedWidget->setCurrentWidget(ui->stuPage);
    updateButtonStyles(ui->stuInfoButton);
}

void administratorWidget::on_sumButton_clicked() {
    updateSumTable();
    ui->stackedWidget->setCurrentWidget(ui->sumPage);
    updateButtonStyles(ui->sumButton);
}

void administratorWidget::build_or_update_stuinfoTable(QVector<Account>& stuInfo, QTableWidget* stuInfoTable) {
    stuInfoTable->setRowCount(std::min(static_cast<int>(stuInfo.size()), TABLE_MAX_SIZE));
    stuInfoTable->setColumnCount(6);
    stuInfoTable->setWindowTitle("学生信息");
    stuInfoTable->setHorizontalHeaderLabels(QStringList() << "学号" << "姓名" << "性别" << "联系电话" << "宿舍信息" << "");
    for (int i = 0; i < stuInfo.size() && i < TABLE_MAX_SIZE; ++i) {
        setItem(stuInfoTable, i, 0, stuInfo[i].get_id());
        setItem(stuInfoTable, i, 1, stuInfo[i].get_name());
        setItem(stuInfoTable, i, 2, stuInfo[i].get_gender() == Male ? "男" : "女");
        setItem(stuInfoTable, i, 3, stuInfo[i].get_telephone());
        auto* dormInfoButton = new QPushButton("宿舍信息", this);
        stuInfoTable->setCellWidget(i, 4, dormInfoButton);
        connect(dormInfoButton, &QPushButton::clicked, this, [=]() {
            std::shared_ptr<Dormitory> dorm = Dormitory::searchDorm(stuInfo[i].get_id());
            std::shared_ptr<dormManage> dM;
            if(dorm == nullptr) {
                dorm = std::make_shared<Dormitory>();
                dM = std::make_shared<dormManage>();
            }
            else
                dM = dormManage::dormInfo(dorm->getManageId());
            if(dorm != nullptr && dM != nullptr) {
                dormDialog* dD = new dormDialog(this, stuInfo[i], dorm, dM, false);
                dD->exec();
            }
        });
    }
    stuInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}



void administratorWidget::on_searchButton_clicked() {
    auto name = ui->nameEdit->text(), id = ui->idEdit->text();
    qDebug() << name << id;
    if(name.isEmpty() && id.isEmpty()) {
        QMessageBox::information(this, "invalid input", "请输入有效信息");
        return;
    }
    static QVector<Account> result;
    result.clear();
    for(auto& acnt : stuInfo) {
        if(acnt.get_id().startsWith(id) && acnt.get_name().startsWith(name))
            result.emplace_back(acnt);
    }
    build_or_update_stuinfoTable(result, ui->resultTable);
}

void administratorWidget::on_dormDistrubuteButton_clicked() {
    Account::unChecked(unchecked);
    build_or_update_dormTable(unchecked, ui->dinfoTable);
    ui->stackedWidget->setCurrentWidget(ui->dormPage);
    ui->dormTab->setCurrentIndex(0);
    updateButtonStyles(ui->dormDistrubuteButton);
}

void administratorWidget::build_or_update_dormTable(QVector<Account> &dormInfo, QTableWidget *dormTable) {
    dormTable->setRowCount(std::min(static_cast<int>(dormInfo.size()), TABLE_MAX_SIZE));
    dormTable->setColumnCount(6);
    dormTable->setWindowTitle("学生信息");
    dormTable->setHorizontalHeaderLabels(QStringList() << "学号" << "姓名" << "性别" << "联系电话" << "宿舍分配" << "");
    for (int i = 0; i < dormInfo.size() && i < TABLE_MAX_SIZE; ++i) {
        setItem(dormTable, i, 0, dormInfo[i].get_id());
        setItem(dormTable, i, 1, dormInfo[i].get_name());
        setItem(dormTable, i, 2, dormInfo[i].get_gender() == Male ? "男" : "女");
        setItem(dormTable, i, 3, dormInfo[i].get_telephone());
        auto* dormInfoButton = new QPushButton("分配", this);
        dormTable->setCellWidget(i, 4, dormInfoButton);
        connect(dormInfoButton, &QPushButton::clicked, this, [=]() {
            auto* dbd = new distributeDialog(dormInfo[i], this);
            dbd->exec();
            on_dormDistrubuteButton_clicked();
        });
    }
    dormTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void administratorWidget::on_d_searchButton_clicked() {
    auto name = ui->d_nameEdit->text(), id = ui->d_idEdit->text();
    qDebug() << name << id;
    if(name.isEmpty() && id.isEmpty()) {
        QMessageBox::information(this, "invalid input", "请输入有效信息");
        return;
    }
    static QVector<Account> d_result;
    d_result.clear();
    for(auto& acnt : unchecked) {
        if(acnt.get_id().startsWith(id) && acnt.get_name().startsWith(name))
            d_result.emplace_back(acnt);
    }
    build_or_update_dormTable(d_result, ui->d_resultTable);
}

void administratorWidget::on_dormManageButton_clicked() {
    ui->stackedWidget->setCurrentWidget(ui->dormMPage);
    ui->manageTab->setCurrentIndex(0);
    Buildings::getAllBldgs(bldgInfo);
    build_or_update_bldgTable(bldgInfo, ui->bldgTable);
    updateButtonStyles(ui->dormManageButton);
}

void administratorWidget::build_or_update_bldgTable(QVector<Buildings>& bldgInfo, QTableWidget* bldgTable) {
    bldgTable->setRowCount(std::min(static_cast<int>(bldgInfo.size()), TABLE_MAX_SIZE));
    bldgTable->setColumnCount(7);
    bldgTable->setWindowTitle("楼栋信息");
    bldgTable->setHorizontalHeaderLabels(QStringList() << "园区" << "楼栋号" << "类型" << "层数" << "房间数" << "床位数" << "入住人数");
    for (int i = 0; i < bldgInfo.size() && i < TABLE_MAX_SIZE; ++i) {
        setItem(bldgTable, i, 0, bldgInfo[i].location);
        setItem(bldgTable, i, 1, Buildings::split(bldgInfo[i].LocName).second);
        setItem(bldgTable, i, 2, bldgInfo[i].gender == Male ? "男生宿舍" : "女生宿舍");
        setItem(bldgTable, i, 3, QString::number(bldgInfo[i].floors));
        setItem(bldgTable, i, 4, QString::number(bldgInfo[i].rooms));
        setItem(bldgTable, i, 5, QString::number(bldgInfo[i].beds));
        setItem(bldgTable, i ,6, QString::number(bldgInfo[i].check_ins));
    }

    bldgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void administratorWidget::on_manageTab_currentChanged(int index) {
    static std::set<QString> loc;
    if(index == 0) {
        Buildings::getAllBldgs(bldgInfo);
        build_or_update_bldgTable(bldgInfo, ui->bldgTable);
    }
    else if(index == 1) {
        ui->delete_locCombo->clear();
        ui->delete_locCombo->addItem("");
        Buildings::getLoc(None, loc);
        for(auto& l : loc)
            ui->delete_locCombo->addItem(l);
        ui->delete_locCombo->setEditable(false);
    }
    else if(index == 2) {
        dormManage::getAll(dormManageInfo);
        build_or_update_dormManageTable(dormManageInfo, ui->dmtTable);
    }
    else if(index == 3) {
        ui->dd_locCombo->clear();
        ui->dd_locCombo->addItem("");
        Buildings::getLoc(None, loc);
        for(auto& l : loc)
            ui->dd_locCombo->addItem(l);
        ui->dd_locCombo->setEditable(false);
    }
}

void administratorWidget::on_delete_locCombo_currentTextChanged(const QString &loc) {
    static QVector<QString> buildings;
    ui->delete_bldgCombo->clear();
    ui->delete_bldgCombo->addItem("");
    qDebug() << loc;
    Buildings::getBuildings(None, loc, buildings);
    for (const auto& building : buildings) {
        ui->delete_bldgCombo->addItem(building);
    }
    ui->delete_bldgCombo->setEditable(false);
}

void administratorWidget::on_b_deleteButton_clicked() {
    auto locName = ui->delete_locCombo->currentText() + '_' + ui->delete_bldgCombo->currentText();
    if(!Buildings::deleteBuilding(locName, this))
        QMessageBox::warning(this, "bldg delete error", "楼栋删除失败");
    else
        QMessageBox::information(this, "bldg delete success", "楼栋删除成功");
}

void administratorWidget::on_b_addButton_clicked() {
    if(ui->add_locLine->text().isEmpty() || ui->add_bldg_line->text().isEmpty() || ui->add_genderCombo->currentIndex() == 0
        || ui->floorEdit->text().isEmpty() || ui->roomEdit->text().isEmpty() || ui->capacityEdit->text().isEmpty()) {
        QMessageBox::warning(this, "add error", "请填写完所有信息！");
        return;
    }
    auto loc = ui->add_locLine->text();
    auto bldg =  ui->add_bldg_line->text();
    Gender gender = ui->add_genderCombo->currentIndex() == 1 ? Male : Female;
    int floors = ui->floorEdit->text().toInt();
    int rooms = ui->roomEdit->text().toInt();
    int capacity = ui->capacityEdit->text().toInt();
    Buildings building(bldg, loc, gender, floors, rooms, capacity, 0);
    if(building.addBuilding()) {
        QMessageBox::information(this, "add success", "楼栋增加成功！");
        ui->add_locLine->clear();
        ui->add_bldg_line->clear();
        ui->add_genderCombo->setCurrentIndex(0);
        ui->floorEdit->clear();
        ui->roomEdit->clear();
        ui->capacityEdit->clear();
    }
    else {
        QMessageBox::warning(this, "add error", "楼栋增加失败！");
    }
}

void administratorWidget::on_b_modifyButton_clicked() {
    if(ui->add_locLine->text().isEmpty() || ui->add_bldg_line->text().isEmpty() ||
        (ui->floorEdit->text().isEmpty() && ui->roomEdit->text().isEmpty() && ui->capacityEdit->text().isEmpty())) {
        QMessageBox::warning(this, "modify error", "请填写完必要信息！");
        return;
    }
    auto loc = ui->add_locLine->text();
    auto bldg =  ui->add_bldg_line->text();
    int floors = ui->floorEdit->text().toInt();
    int rooms = ui->roomEdit->text().toInt();
    int capacity = ui->capacityEdit->text().toInt();
    if(Buildings::modify(loc, bldg, floors, rooms, capacity)) {
        QMessageBox::information(this, "modify success", "楼栋修改成功！");
        ui->add_locLine->clear();
        ui->add_bldg_line->clear();
        ui->add_genderCombo->setCurrentIndex(0);
        ui->floorEdit->clear();
        ui->roomEdit->clear();
        ui->capacityEdit->clear();
    }
    else {
        QMessageBox::warning(this, "modify error", "楼栋修改失败！");
    }
}

void administratorWidget::build_or_update_dormManageTable(QVector<dormManage> &dormManageInfo, QTableWidget *dmtTable) {
    dmtTable->setRowCount(std::min(TABLE_MAX_SIZE, static_cast<int>(dormManageInfo.size())));
    dmtTable->setColumnCount(5);

    QStringList headers = {"园区", "楼栋", "宿舍号", "宿舍类型", "剩余容量"};
    dmtTable->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < dormManageInfo.size() && row < TABLE_MAX_SIZE; ++row) {
        const dormManage &dorm = dormManageInfo[row];
        auto locName = dorm.building;
        auto splitLocName = Buildings::split(locName);

        QString park = splitLocName.first;
        QString building = splitLocName.second;
        QString dormitoryNumber = QString::number(dorm.dorm_id);
        QString dormitoryType = dormManage::capacityToName(dorm.capacity);
        QString remainingCapacity = QString::number(dorm.capacity - dorm.curCheckins);

        setItem(dmtTable, row, 0, park);
        setItem(dmtTable, row, 1, building);
        setItem(dmtTable, row, 2, dormitoryNumber);
        setItem(dmtTable, row, 3, dormitoryType);
        setItem(dmtTable, row, 4, remainingCapacity);
    }
    dmtTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void administratorWidget::on_dd_locCombo_currentTextChanged(const QString &loc) {
    static QVector<QString> buildings;
    ui->dd_bldgCombo->clear();
    ui->dd_bldgCombo->addItem("");
    qDebug() << loc;
    Buildings::getBuildings(None, loc, buildings);
    for (const auto& building : buildings) {
        ui->dd_bldgCombo->addItem(building);
    }
    ui->dd_bldgCombo->setEditable(false);
}


void administratorWidget::on_dd_addButton_clicked() {
    if(ui->dd_locCombo->currentIndex() == 0 || ui->dd_bldgCombo->currentIndex() == 0 || ui->dd_dormEdit->text().isEmpty() || ui->dd_dormCombo->currentIndex() == 0) {
        QMessageBox::warning(this, "add error", "请填写完全部信息！");
        return;
    }
    auto dorm = ui->dd_dormEdit->text();
    if(!re.match(dorm).hasMatch()) {
        QMessageBox::warning(this, "add error", "格式错误！");
        return;
    }
    auto locName = ui->dd_locCombo->currentText() + '_' + ui->dd_bldgCombo->currentText();
    int low = dorm.mid(0, 3).toInt();
    int up = dorm.size() > 3 ? dorm.mid(4, 3).toInt() : low;
    int sum = up - low + 1;
    if(low / 100 != up / 100) {
        QMessageBox::warning(this, "add error", "格式错误！");
        return;
    }
    int capacity = dormManage::NameToCapacity(ui->dd_dormCombo->currentText());
    if(!dormManage::checkBeds(locName, Buildings::getBeds(locName) - capacity * sum)) {
        QMessageBox::warning(this, "add error", "床位数已达上限！");
        return;
    }
    if(!dormManage::checkRooms(locName, Buildings::getRooms(locName) - sum)) {
        QMessageBox::warning(this, "add error", "房间数量已达上限！");
        return;
    }
    int cnt = dormManage::addDorms(locName, low, up, capacity);
    QMessageBox::information(this, "Add Result", QString("成功: %1\n失败: %2").arg(cnt).arg(sum - cnt));
}

void administratorWidget::on_dd_modifyButton_clicked() {
    if(ui->dd_locCombo->currentIndex() == 0 || ui->dd_bldgCombo->currentIndex() == 0 || ui->dd_dormEdit->text().isEmpty() || ui->dd_dormCombo->currentIndex() == 0) {
        QMessageBox::warning(this, "add error", "请填写完全部信息！");
        return;
    }
    auto dorm = ui->dd_dormEdit->text();
    if(!re.match(dorm).hasMatch()) {
        QMessageBox::warning(this, "add error", "格式错误！");
        return;
    }
    auto locName = ui->dd_locCombo->currentText() + '_' + ui->dd_bldgCombo->currentText();
    int low = dorm.mid(0, 3).toInt();
    int up = dorm.size() > 3 ? dorm.mid(4, 3).toInt() : low;
    int sum = up - low + 1;
    if(low / 100 != up / 100) {
        QMessageBox::warning(this, "add error", "格式错误！");
        return;
    }
    int capacity = dormManage::NameToCapacity(ui->dd_dormCombo->currentText());
    auto res = dormManage::check_rooms_and_beds(locName, low, up, capacity);
    if(!res.second) {
        QMessageBox::warning(this, "add error", "宿舍不存在//修改后，存在宿舍入住人数大于宿舍容量，修改失败！");
        return;
    }
    if(!dormManage::checkBeds(locName, Buildings::getBeds(locName) - capacity * sum + res.first)) {
        QMessageBox::warning(this, "add error", "床位数已达上限！");
        return;
    }
    int cnt = dormManage::changeCapacity(locName, low, up, capacity);
    QMessageBox::information(this, "Add Result", QString("成功: %1\n失败: %2").arg(cnt).arg(sum - cnt));
}

void administratorWidget::on_dd_deleteButton_clicked() {
    if(ui->dd_locCombo->currentIndex() == 0 || ui->dd_bldgCombo->currentIndex() == 0 || ui->dd_dormEdit->text().isEmpty()) {
        QMessageBox::warning(this, "add error", "请填写完全部信息！");
        return;
    }
    auto dorm = ui->dd_dormEdit->text();
    if(!re.match(dorm).hasMatch()) {
        QMessageBox::warning(this, "add error", "格式错误！");
        return;
    }
    auto locName = ui->dd_locCombo->currentText() + '_' + ui->dd_bldgCombo->currentText();
    int low = dorm.mid(0, 3).toInt();
    int up = dorm.size() > 3 ? dorm.mid(4, 3).toInt() : low;
    int sum = up - low + 1;
    if(low / 100 != up / 100) {
        QMessageBox::warning(this, "add error", "格式错误！");
        return;
    }
    int capacity = 0;
    auto res = dormManage::check_rooms_and_beds(locName, low, up, capacity);
    if(!res.second) {
        QMessageBox::warning(this, "add error", "宿舍不存在//宿舍仍有学生入住");
        return;
    }
    int cnt = dormManage::deleteDorms(locName, low, up);
    QMessageBox::information(this, "Add Result", QString("成功: %1\n失败: %2").arg(cnt).arg(sum - cnt));
}

void administratorWidget::on_checkOutButton_clicked() {
    ui->reviewTab->setCurrentIndex(0);
    ui->stackedWidget->setCurrentWidget(ui->reviewPage);
    on_reviewTab_currentChanged(0);
    updateButtonStyles(ui->checkOutButton);
}

void administratorWidget::on_reviewTab_currentChanged(int index) {
    if(index == 0) {
        checkOut::toDo(todo);
        Account::checkOutList(checkOutList, todo);
        build_or_update_todoTable(checkOutList, todo, ui->todoTable);
    }
}

void administratorWidget::build_or_update_todoTable(QVector<Account> &list, QVector<checkOut> &todo, QTableWidget *todoTable) {
    todoTable->setRowCount(std::min(TABLE_MAX_SIZE, static_cast<int>(list.size())));
    todoTable->setColumnCount(6);

    QStringList headers = {"姓名", "学号", "申请理由", "申请时间", "详细信息", "回复"};
    todoTable->setHorizontalHeaderLabels(headers);
    todoTable->setColumnWidth(3, 150);
    for (int i = 0; i < list.size() && i < TABLE_MAX_SIZE; ++i) {
        setItem(todoTable, i, 0, list[i].get_name());
        setItem(todoTable, i, 1, list[i].get_id());
        setItem(todoTable, i, 2, checkOut::reasonToString[todo[i].reason]);
        setItem(todoTable, i, 3, todo[i].apply_date.toString("yyyy-MM-dd HH:mm:ss"));

        QPushButton *detailButton = new QPushButton("详细信息", this);
        todoTable->setCellWidget(i, 4, detailButton);

        connect(detailButton, &QPushButton::clicked, this, [=]() {
            QString details = QString("姓名: %1\t学号: %2\n性别: %3\t联系电话: %4\n申请理由：%5\n详细理由: %6")
                .arg(list[i].get_name())
                .arg(list[i].get_id())
                .arg(list[i].get_gender() == Male ? "男" : "女")
                .arg(list[i].get_telephone())
                .arg(checkOut::reasonToString[todo[i].reason])
                .arg(todo[i].detail);

            QMessageBox::information(this, "详细信息", details);
        });

        QPushButton* replyButton = new QPushButton("审核回复", this);
        todoTable->setCellWidget(i, 5, replyButton);

        connect(replyButton, &QPushButton::clicked, this, [=] {
            auto* reply = new replyDialog(account, todo[i], this);
            reply->exec();
            on_reviewTab_currentChanged(0);
        });
    }

    todoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void administratorWidget::on_r_searchButton_clicked() {
    auto name = ui->r_nameEdit->text(), id = ui->r_idLineEdit->text();
    qDebug() << name << id;
    if(name.isEmpty() && id.isEmpty()) {
        QMessageBox::information(this, "invalid input", "请输入有效信息");
        return;
    }
    static QVector<QPair<checkOut, Reply>> record;
    static QVector<Account> r_result;
    QVector<QPair<checkOut, Reply>> tmp;
    r_result.clear();
    record.clear();
    if(stuInfo.isEmpty())
        Account::StuInfo(stuInfo);
    for(auto& acnt : stuInfo) {
        if(acnt.get_id().startsWith(id) && acnt.get_name().startsWith(name)) {
            r_result.append(acnt);
            checkOut::record(acnt.get_id(), tmp);
            record.append(tmp.begin(), tmp.end());
        }
    }
    build_or_update_historyTable(record, r_result, ui->historyTable);
}

void administratorWidget::build_or_update_historyTable(QVector<QPair<checkOut, Reply> > &record, QVector<Account> &list, QTableWidget *historyTable) {
    historyTable->setRowCount(std::min(TABLE_MAX_SIZE, static_cast<int>(record.size())));
    historyTable->setColumnCount(7);
    auto it = list.begin();
    QStringList headers = {"姓名", "学号", "申请理由", "审核结果", "申请时间", "审核时间","详细信息"};
    historyTable->setHorizontalHeaderLabels(headers);
    historyTable->setColumnWidth(4, 150);
    historyTable->setColumnWidth(5, 150);
    for(int i = 0; i < record.size() && i < TABLE_MAX_SIZE; i++) {
        while(it->get_id() != record[i].first.stu_id) it++;
        setItem(historyTable, i, 0, it->get_name());
        setItem(historyTable, i, 1, it->get_id());
        setItem(historyTable, i, 2, checkOut::reasonToString[record[i].first.reason]);
        setItem(historyTable, i, 3, checkOut::progressToString[record[i].first.progress]);
        setItem(historyTable, i, 4, record[i].first.apply_date.toString("yyyy-MM-dd HH:mm:ss"));
        setItem(historyTable, i , 5, record[i].second.reply_date.toString("yyyy-MM-dd HH:mm:ss"));
        auto* detailsButton = new QPushButton("查看", this);
        historyTable->setCellWidget(i, 6, detailsButton);
        connect(detailsButton, &QPushButton::clicked, historyTable, [=]() {
            auto* dW = new detailDialog(this, record[i].first, record[i].second);
            dW->setWindowTitle("详细信息");
            dW->exec();
        });
    }
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void administratorWidget::on_logoutButton_clicked() {
    emit adminSwitchBack();
}

void administratorWidget::updateButtonStyles(QPushButton* activeButton) {
    QList<QPushButton*> buttons = {ui->stuInfoButton, ui->sumButton, ui->dormDistrubuteButton, ui->dormManageButton, ui->checkOutButton};

    for (auto button : buttons) {
        if (button == activeButton) {
            button->setStyleSheet("background-color: #D8BFD8;");
        } else {
            button->setStyleSheet(buttonStylesheet());
        }
    }
}

void administratorWidget::checkDarkMode() {
    bool newDarkMode = isDarkMode();
    if (newDarkMode != currentDarkMode) {
        currentDarkMode = newDarkMode;
        ui->stuInfoButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->stuPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->checkOutButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->reviewPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->dormDistrubuteButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->dormPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->dormManageButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->dormMPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->sumButton->setStyleSheet(ui->stackedWidget->currentWidget() != ui->sumPage ? buttonStylesheet() : "background-color: #D8BFD8;");
        ui->logoutButton->setStyleSheet(buttonStylesheet());
    }
}