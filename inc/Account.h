//
// Created by 小火锅 on 24-11-26.
//

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QString>
#include <QMessageBox>
#include <memory>
#include "checkOut.h"

enum Gender{None, Male, Female};
enum Role{Admin, Student};

const QString ACCOUNT_DATABASE_FILE = "DMS.db";
const QString ACCOUNT_CONNECTION = "dms_conn";

class Account {
public:
    Account(const QString& id, const QString& name, Gender gender, const QString& pwd, const QString& tel, Role role)
        : student_id(id), name(name), gender(gender), password(pwd), telephone_number(tel), role(role) {}
    bool signUp() const;                              // 注册新账户
    static std::shared_ptr<Account> login(const QString& id, const QString& pwd); // 登录账户
    static int countNum(Role role);                   // 统计特定角色用户数
    static QString getName(const QString& stu_id);
    static void StuInfo(QVector<Account>& stuInfo);
    static void unChecked(QVector<Account>& unchecked);
    static void checkOutList(QVector<Account>& list, QVector<checkOut>& todo);
    QString get_id() const;
    QString get_name() const;
    QString get_telephone() const;
    Gender get_gender() const;
    Role get_role() const;
private:
    QString student_id;
    QString name;
    Gender gender;
    QString password;
    QString telephone_number;
    Role role;
    static QString processPwd(const QString& pwd);    // 处理密码加密
};



#endif //ACCOUNT_H
