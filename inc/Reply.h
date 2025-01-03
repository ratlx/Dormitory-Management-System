//
// Created by 小火锅 on 24-12-5.
//

#ifndef REPLY_H
#define REPLY_H
#include <QDateTime>
#include <QString>

const QString REPLY_DATABASE_FILE = "DMS.db";
const QString REPLY_CONNECTION = "dms_conn";

class Reply {
public:
    enum Decision{agreed, disagreed};
    static const QMap<Decision, QString> decisionToString;
    static bool review(const QString& admin_id, const QString& reply, int co_id);
    static bool modify(int co_id);
    static bool review_and_modify(const QString& admin_id, const QString& reply, int co_id, Decision decision);
    Reply(const QString &reply, const QString &admin_id, const QDateTime &reply_date, const int checkout_id,
        const int reply_id)
        : reply(reply),
          admin_id(admin_id),
          reply_date(reply_date),
          checkout_id(checkout_id),
          reply_id(reply_id) {
    }
    Reply() {};
    QString reply;
    QString admin_id;
    QDateTime reply_date;
    int checkout_id;
    int reply_id;
};



#endif //REPLY_H
