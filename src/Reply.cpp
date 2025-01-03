//
// Created by 小火锅 on 24-12-5.
//

#include "../inc/Reply.h"
#include "../inc/Buildings.h"
#include "../inc/dormManage.h"
#include "../inc/Dormitory.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <memory>
#include "../inc/checkOut.h"

const QMap<Reply::Decision, QString> Reply::decisionToString={
    {agreed, "审核通过"},
    {disagreed, "审核不通过"}
};

bool Reply::review(const QString &admin_id, const QString &reply, int co_id) {
    QSqlDatabase db = QSqlDatabase::database(REPLY_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "审核时，数据库打开失败";
        return false;
    }
    QSqlQuery query(db);
    if(!query.exec("PRAGMA foreign_keys = ON;")) {
        qDebug() << "注册宿舍信息时，连接外键失败，" << query.lastError().text();
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS replies("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "admin_id TEXT NOT NULL, "
        "reply TEXT NOT NULL, "
        "reply_date TEXT NOT NULL, "
        "checkout_id INTEGER NOT NULL, "
        "FOREIGN KEY (checkout_id) REFERENCES check_out(id),"
        "FOREIGN KEY (admin_id) REFERENCES users(student_id)"
        ");")) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }
    query.prepare("INSERT INTO replies(admin_id, reply, reply_date, checkout_id)"
    "VALUES (:admin_id, :reply, :reply_date, :checkout_id)");

    query.bindValue(":admin_id", admin_id);
    query.bindValue(":reply", reply);
    query.bindValue(":reply_date", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    query.bindValue(":checkout_id", co_id);

    if(!query.exec()) {
        qDebug() << "审核退宿时，插入请求失败" << query.lastError().text();
        return false;
    }
    qDebug() << "审核退宿时，插入请求成功";
    return true;
}

bool Reply::modify(int co_id) {
    auto stu_id = checkOut::getStuId(co_id);

    int manage_id = Dormitory::getManageId(stu_id);
    std::shared_ptr<dormManage> manage = dormManage::dormInfo(manage_id);
    if(manage == nullptr) {
        qDebug() << "获取宿舍管理信息失败";
        return false;
    }
    if(!manage->manageStudent(-1)) {
        qDebug() << "退宿时，dormManage模块处理错误";
        return false;
    }
    if(!Buildings::modifyCheckIns(manage->building, -1)) {
        qDebug() << "退宿时，buildings模块处理错误";
        return false;
    }

    if(!Dormitory::CheckOut(stu_id)) {
        qDebug() << "退宿时，dormitories模块处理错误";
        return false;
    }
    return true;
}

bool Reply::review_and_modify(const QString &admin_id, const QString &reply, int co_id, Decision decision) {
    if(!checkOut::check(co_id)) {
        qDebug() << "该申请已经完结，不需要处理";
        return false;
    }
    if(!review(admin_id, reply, co_id)) {
        qDebug() << "审核失败";
        return false;
    }
    if(decision == agreed) {
        if(!modify(co_id)) {
            qDebug() << "退宿失败";
            return false;
        }
        checkOut::updateProgress(co_id, checkOut::Approved);
    }
    else
        checkOut::updateProgress(co_id, checkOut::Rejected);
    return true;
}

