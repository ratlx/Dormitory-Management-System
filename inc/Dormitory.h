//
// Created by 小火锅 on 24-11-29.
//

#ifndef DORMITORY_H
#define DORMITORY_H

#include <QString>
#include <memory>
#include <QtCore/qdatetime.h>

enum Status {NotConfirmed, CheckedIn, CheckedOut, InvalidStatus};
const QString statusToString[] = {
    "未确认", "已入住", "已退宿", ""
};
const QString DORM_DATABASE_FILE = "DMS.db";
const QString DORM_CONNECTION = "dms_conn";

class Dormitory {
public:
    Dormitory()
        :student_id(""), manage_id(-1), status(InvalidStatus){}
    Dormitory(const QString& student_id, int manage_id)
        :student_id(student_id), manage_id(manage_id), status(NotConfirmed){}
    Dormitory(const QString& student_id, int manage_id, Status status)
        :student_id(student_id), manage_id(manage_id), status(status){}
    Dormitory(const QString& student_id, int manage_id, Status status, const QDate& cid, const QDate& cod)
        :student_id(student_id), manage_id(manage_id), status(status), checkin_date(cid), checkout_date(cod){}
    static std::shared_ptr<Dormitory> searchDorm(const QString& student_id);
    static bool checkIn_and_modify(const QString& stu_id, const QString& loc, const QString& bldg, int dorm_id);
    static bool CheckOut(const QString& student_id);
    static void getRecord(const QString& stu_id, QVector<Dormitory>& record);
    static Status stringToStatus(const QString &str);
    static int getManageId(const QString& stu_id);
    static int countCheckout();
    bool Confirm();
    int getManageId();
    Status getStatus() const;
    QString getCID();
    QString getCOD();
private:
    static bool CheckIn(const QString& student_id, int manage_id);
    QString student_id;           //学生id
    int manage_id;                //宿舍管理id
    Status status;                //宿舍状态：未确认、已入住、已退宿
    QDate checkin_date;            //入住日期
    QDate checkout_date;           //退宿日期
};



#endif //DORMITORY_H
