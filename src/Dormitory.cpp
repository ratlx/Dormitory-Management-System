//
// Created by 小火锅 on 24-11-29.
//

#include "../inc/Dormitory.h"
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QMessageBox>
#include <algorithm>

#include "../inc/Buildings.h"

Status Dormitory::stringToStatus(const QString &str) {
    for(int i = 0; i < 3; i++) {
        if(str == statusToString[i])
            return static_cast<Status> (i);
    }
    return NotConfirmed;
}

bool Dormitory::CheckIn(const QString& student_id, int manage_id) {
    auto db = QSqlDatabase::database(DORM_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "注册宿舍信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "注册时，数据库打开失败");
        QCoreApplication::exit(-1);
        return false;
    }

    QSqlQuery query(db);
    if(!query.exec("PRAGMA foreign_keys = ON;")) {
        qDebug() << "注册宿舍信息时，连接外键失败，" << query.lastError().text();
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS dormitories("
        "student_id TEXT NOT NULL, "
        "manage_id INTEGER NOT NULL, "
        "status TEXT CHECK (status IN ('未确认', '已入住', '已退宿')), "
        "checkin_date TEXT NOT NULL, "
        "checkout_date TEXT, "
        "FOREIGN KEY (student_id) REFERENCES users(student_id), "
        "FOREIGN KEY (manage_id) REFERENCES dormitories_manage(manage_id)"
        ");")) {
        qDebug() << "创建表失败：" << query.lastError().text();
        QMessageBox::critical(nullptr, "Database Error", "注册宿舍信息时，创建表失败：");
        QCoreApplication::exit(-1);
        return false;
    }
    query.prepare("SELECT status FROM dormitories WHERE student_id = :student_id AND "
                  "(status = :status1 OR status = :status2);");
    query.bindValue(":student_id", student_id);
    query.bindValue(":status1", statusToString[NotConfirmed]);
    query.bindValue(":status2", statusToString[CheckedIn]);
    if(!query.exec()) {
        qDebug() << "安排寝室时，查询失败：" << query.lastError().text();
        return false;
    }
    if(query.next()) {
        qDebug() << "该学生已安排宿舍！";
        return false;
    }
    query.prepare("INSERT INTO dormitories(student_id, manage_id, status, checkin_date) "
                  "VALUES (:student_id, :manage_id, :status, :checkin_date)");
    query.bindValue(":student_id", student_id);
    query.bindValue(":manage_id", manage_id);
    query.bindValue(":status", statusToString[NotConfirmed]);
    query.bindValue(":checkin_date", QDate::currentDate().toString("yyyy-MM-dd"));
    if(!query.exec()) {
        qDebug() << "安排寝室失败：" << query.lastError().text();
        return false;
    }
    qDebug() << "安排寝室成功！";
    return true;
}

Status Dormitory::getStatus() const {
    return status;
}

bool Dormitory::Confirm() {
    auto db = QSqlDatabase::database(DORM_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "确认宿舍信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "注册时，数据库打开失败");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE dormitories SET status = :new_status WHERE student_id = :student_id AND status = :cur_status");
    query.bindValue(":cur_status", statusToString[NotConfirmed]);
    query.bindValue(":student_id", student_id);
    query.bindValue(":new_status", statusToString[CheckedIn]);
    if(!query.exec()) {
        qDebug() << "确认宿舍信息失败：" << query.lastError().text();
        return false;
    }
    qDebug() << "确认宿舍信息成功！";
    status = CheckedIn;
    return true;
}

bool Dormitory::CheckOut(const QString &student_id) {
    auto db = QSqlDatabase::database(DORM_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "确认宿舍信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "注册时，数据库打开失败");
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE dormitories SET status = :new_status, checkout_date = :checkout_date WHERE student_id = :student_id AND status IN (:status1, :status2)");
    query.bindValue(":status1", statusToString[CheckedIn]);
    query.bindValue(":status2", statusToString[NotConfirmed]);
    query.bindValue(":student_id", student_id);
    query.bindValue(":new_status", statusToString[CheckedOut]);
    query.bindValue(":checkout_date", QDate::currentDate().toString("yyyy-MM-dd"));

    if(!query.exec()) {
        qDebug() << "退宿时，修改失败" << query.lastError().text();
        return false;
    }
    qDebug() << "退宿成功";
    return true;
}

std::shared_ptr<Dormitory> Dormitory::searchDorm(const QString &student_id) {
    auto db = QSqlDatabase::database(DORM_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "获取宿舍信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "注册时，数据库打开失败");
        return nullptr;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM dormitories WHERE student_id = :student_id AND "
                  "(status = :status1 OR status = :status2);");
    query.bindValue(":student_id", student_id);
    query.bindValue(":status1", statusToString[NotConfirmed]);
    query.bindValue(":status2", statusToString[CheckedIn]);
    if(!query.exec()) {
        qDebug() << "获取宿舍信息时，查询失败：" << query.lastError().text();
        return nullptr;
    }
    if(!query.next()) {
        qDebug() << "没有该同学的宿舍信息";
        return nullptr;
    }
    return std::make_shared<Dormitory>(student_id,
        query.value("manage_id").toInt(),
        stringToStatus(query.value("status").toString()),
        query.value("checkin_date").toDate(),
        query.value("checkout_date").toDate());
}

int Dormitory::getManageId() {
    return manage_id;
}

void Dormitory::getRecord(const QString &stu_id, QVector<Dormitory>& record) {
    auto db = QSqlDatabase::database(DORM_CONNECTION);

    QSqlQuery query(db);
    query.prepare("SELECT * FROM dormitories WHERE student_id = :student_id;");
    query.bindValue(":student_id", stu_id);
    if(!query.exec()) {
        qDebug() << "获取宿舍记录时，查询失败：" << query.lastError().text();
        return;
    }
    while(query.next()) {
        record.push_back({stu_id, query.value("manage_id").toInt(), stringToStatus(query.value("status").toString()),
        QDate::fromString(query.value("checkin_date").toString(), "yyyy-MM-dd"), QDate::fromString(query.value("checkout_date").toString(), "yyyy-MM-dd")});
    }
    std::sort(record.begin(), record.end(), [](const Dormitory& d1, const Dormitory& d2){return d1.checkin_date > d2.checkin_date;});
    qDebug() << "成功获取宿舍记录";
}

QString Dormitory::getCID() {
    return checkin_date.toString("yyyy-MM-dd");
}

QString Dormitory::getCOD() {
    return checkout_date.toString("yyyy-MM-dd");
}

int Dormitory::getManageId(const QString &stu_id) {
    auto db = QSqlDatabase::database(DORM_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "获取宿舍管理id时，数据库打开失败";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("SELECT manage_id FROM dormitories WHERE student_id = :student_id AND (status = :status1 OR status = :status2)");
    query.bindValue(":student_id", stu_id);
    query.bindValue(":status1", statusToString[NotConfirmed]);
    query.bindValue(":status2", statusToString[CheckedIn]);

    if (!query.exec()) {
        qDebug() << "查询管理id失败：" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        return query.value("manage_id").toInt();
    }
    qDebug() << "未找到匹配的管理id";
    return -1;
}

bool Dormitory::checkIn_and_modify(const QString &stu_id, const QString &loc, const QString &bldg, int dorm_id) {
    QString locName = loc + '_' + bldg;
    qDebug() << locName << dorm_id;
    dormManage dM(locName, dorm_id);
    if(dM.manage_id == -1) return false;
    if(!dM.manageStudent(1))
        return false;
    if(!Buildings::modifyCheckIns(locName, 1))
        return false;
    if(!CheckIn(stu_id, dM.manage_id))
        return false;
    return true;
}

int Dormitory::countCheckout() {
    auto db = QSqlDatabase::database(DORM_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "获取退宿学生数量时，数据库打开失败";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT COUNT(DISTINCT student_id)
        FROM dormitories d1
        WHERE d1.status = '已退宿'
          AND NOT EXISTS (
            SELECT 1
            FROM dormitories d2
            WHERE d2.student_id = d1.student_id
              AND d2.status IN ('未确认', '已入住')
        );
    )");

    if (!query.exec()) {
        qDebug() << "获取退宿学生数量时，查询失败：" << query.lastError().text();
        return -1;
    }

    if (query.next()) {
        qDebug() << query.value(0).toInt();
        return query.value(0).toInt();
    }

    return 0;
}
