//
// Created by 小火锅 on 24-12-5.
//

#include "../inc/checkOut.h"
#include "../inc/Dormitory.h"
#include <memory>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <algorithm>

const QMap<checkOut::Progress, QString> checkOut::progressToString = {
    {UnderReview, "审核中"},
    {Approved, "审核通过"},
    {Rejected, "审核失败"},
    {Canceled, "已取消"}
};

const QMap<QString, checkOut::Progress> checkOut::stringToProgress = {
    {"审核中", UnderReview},
    {"审核通过", Approved},
    {"审核失败", Rejected},
    {"已取消", Canceled}
};

const QMap<checkOut::ReasonType, QString> checkOut::reasonToString = {
    {AcademicChange, "学业变动"},
    {PersonalReason, "个人原因"},
    {FinancialIssue, "财务问题"},
    {HealthConcern, "健康问题"},
    {FamilyEmergency, "家庭紧急情况"},
    {Other, "其他原因"}
};

const QMap<QString, checkOut::ReasonType> checkOut::stringToReason = {
    {"学业变动", AcademicChange},
    {"个人原因", PersonalReason},
    {"财务问题", FinancialIssue},
    {"健康问题", HealthConcern},
    {"家庭紧急情况", FamilyEmergency},
    {"其他原因", Other}
};

bool checkOut::apply(const QString &id, const QString &detail, checkOut::ReasonType reason) {
    std::shared_ptr<Dormitory> dorm = Dormitory::searchDorm(id);
    if(dorm == nullptr) {
        qDebug() << "该状态下，不能退宿！";
        return false;
    }
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "处理退宿时，打开数据库失败";
        return false;
    }
    QSqlQuery query(db);
    if(!query.exec("PRAGMA foreign_keys = ON;")) {
        qDebug() << "注册宿舍信息时，连接外键失败，" << query.lastError().text();
    }
    if (!query.exec("CREATE TABLE IF NOT EXISTS check_out("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "student_id TEXT NOT NULL, "
        "detail TEXT NOT NULL, "
        "apply_date TEXT NOT NULL, "
        "progress TEXT CHECK (progress IN ('审核中', '审核通过', '审核失败', '已取消')), "
        "reason TEXT CHECK (reason IN ('学业变动', '个人原因', '财务问题', '健康问题', '家庭紧急情况', '其他原因')), "
        "FOREIGN KEY (student_id) REFERENCES users(student_id)"
        ");")) {
        qDebug() << "创建表失败：" << query.lastError().text();
        return false;
    }
    query.prepare("INSERT INTO check_out(student_id, detail, reason, progress, apply_date) "
                  "VALUES (:student_id, :detail, :reason, :progress, :apply_date)");
    query.bindValue(":student_id", id);
    query.bindValue(":detail", detail);
    query.bindValue(":apply_date", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    query.bindValue(":reason", reasonToString[reason]);
    query.bindValue(":progress", progressToString[UnderReview]);
    if(!query.exec()) {
        qDebug() << "退宿申请插入失败" << query.lastError().text();
        return false;
    }
    return true;
}

void checkOut::toDo(QVector<checkOut>& todo) {
    todo.clear();
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "处理退宿时，打开数据库失败";
        return ;
    }
    QSqlQuery query(db);
    query.prepare("SELECT * from check_out WHERE progress = :p1");
    query.bindValue(":p1", progressToString[UnderReview]);
    if (!query.exec()) {
        qDebug() << "查询退宿申请失败：" << query.lastError().text();
        return;
    }
    while(query.next()) {
        auto stu_id = query.value("student_id").toString();
        auto reason = stringToReason[query.value("reason").toString()];
        auto date = query.value("apply_date").toDateTime();
        auto detail = query.value("detail").toString();
        int co_id = query.value("id").toInt();
        todo.push_back({stu_id, date, UnderReview, reason, detail, co_id});
    }
    std::sort(todo.begin(), todo.end(), [](const checkOut& c1, const checkOut& c2) {
        return c1.apply_date < c2.apply_date;
    });
    qDebug() << "查询完成，待审核申请数量：" << todo.size();
}

bool checkOut::cancel(const QString &id) {
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "取消申请时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE check_out SET progress = :progress WHERE student_id = :student_id AND progress = :current_progress");
    query.bindValue(":progress", progressToString[Canceled]);
    query.bindValue(":student_id", id);
    query.bindValue(":current_progress", progressToString[UnderReview]);

    if (!query.exec()) {
        qDebug() << "撤销申请时，指令执行错误" << query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0) {
        qDebug() << "没有状态需要撤销";
        return false;
    }

    return true;
}

bool checkOut::check(int id) {
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "检查进度时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT progress FROM check_out WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "检查进度时，查询执行失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        QString progress = query.value("progress").toString();
        return progress == progressToString[UnderReview];
    } else {
        qDebug() << "未找到对应的记录";
        return false;
    }
}

QString checkOut::getStuId(int id) {
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "获取学生ID时，数据库打开失败";
        return {};
    }

    QSqlQuery query(db);
    query.prepare("SELECT student_id FROM check_out WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "获取学生ID时，查询执行失败：" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        return query.value("student_id").toString();
    } else {
        qDebug() << "未找到对应的记录";
        return {};
    }
}

void checkOut::updateProgress(int id, Progress new_progress) {
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "更新进度时，数据库打开失败";
        return;
    }

    QSqlQuery query(db);
    query.prepare("UPDATE check_out SET progress = :progress WHERE id = :id");
    query.bindValue(":progress", progressToString[new_progress]);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "更新进度时，查询执行失败：" << query.lastError().text();
    } else {
        qDebug() << "更新进度成功";
    }
}

bool checkOut::preApply(const QString &id) {
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "预处理申请时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT id FROM check_out WHERE student_id = :id AND progress = :progress");
    query.bindValue(":id", id);
    query.bindValue(":progress", progressToString[UnderReview]);

    if (!query.exec()) {
        qDebug() << "预处理申请时，查询执行失败：" << query.lastError().text();
        return false;
    }
    if(query.next()) return true;
    else return false;
}

void checkOut::record(const QString &stu_id, QVector<QPair<checkOut, Reply>>& record) {
    record.clear();
    QSqlDatabase db = QSqlDatabase::database(OUT_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "获取申请记录时，数据库打开失败";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM check_out WHERE student_id = :student_id");
    query.bindValue(":student_id", stu_id);
    if(!query.exec()) {
        qDebug() << "获取申请记录时，查询失败" << query.lastError().text();
        return;
    }
    while(query.next()) {
        checkOut co(stu_id, query.value("apply_date").toDateTime(),
                    stringToProgress[query.value("progress").toString()],
                    stringToReason[query.value("reason").toString()],
                    query.value("detail").toString(),
                    query.value("id").toInt());
        record.push_back(qMakePair(co, Reply()));
    }
    query.prepare("SELECT * FROM replies WHERE checkout_id = :id");
    for(auto& pcr : record) {
        query.bindValue(":id", pcr.first.getCheckout_id());
        if (query.exec() && query.next()) {
            pcr.second = Reply(query.value("reply").toString(),
                               query.value("admin_id").toString(),
                               query.value("reply_date").toDateTime(),
                               pcr.first.getCheckout_id(),
                               query.value("id").toInt());
        }
    }
    std::sort(record.begin(), record.end(), [](const QPair<checkOut, Reply>& q1, const QPair<checkOut, Reply>& q2) {
        return q1.second.reply_date > q2.second.reply_date;
    });
}

int checkOut::getCheckout_id() const {
    return checkout_id;
}


