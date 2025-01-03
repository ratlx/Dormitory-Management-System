//
// Created by 小火锅 on 24-12-5.
//

#ifndef CHECKOUT_H
#define CHECKOUT_H
#include <QString>
#include <QMap>
#include <QDateTime>
#include <QVector>
#include "Reply.h"
#include <QPair>

const QString OUT_DATABASE_FILE = "DMS.db";
const QString OUT_CONNECTION = "dms_conn";

class checkOut {
public:
    enum Progress{
        UnderReview,    //审核中
        Approved,       //审核通过
        Rejected,        //审核失败
        Canceled
    };
    enum ReasonType {
        AcademicChange,  // 学业变动
        PersonalReason,  // 个人原因
        FinancialIssue,  // 财务问题
        HealthConcern,   // 健康问题
        FamilyEmergency, // 家庭紧急情况
        Other,            // 其他原因
    };
    checkOut(const QString &stu_id, const QDateTime &apply_date, Progress progress, ReasonType reason, const QString &detail, int co_id)
    : stu_id(stu_id), apply_date(apply_date), progress(progress), reason(reason), detail(detail), checkout_id(co_id) {}
    int getCheckout_id() const;
    static const QMap<Progress, QString> progressToString;
    static const QMap<QString, Progress> stringToProgress;
    static const QMap<ReasonType, QString> reasonToString;
    static const QMap<QString, ReasonType> stringToReason;
    static bool apply(const QString& id, const QString& detail, ReasonType reason);
    static void toDo(QVector<checkOut>& todo);
    static bool cancel(const QString& id);
    static bool check(int id);
    static bool preApply(const QString& id);
    static QString getStuId(int id);
    static void updateProgress(int id, Progress new_progress);
    static void record(const QString& stu_id, QVector<QPair<checkOut, Reply>>& record);
    int checkout_id;
    QString stu_id;
    QDateTime apply_date;
    Progress progress;
    ReasonType reason;
    QString detail;
};



#endif //CHECKOUT_H
