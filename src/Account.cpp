//
// Created by 小火锅 on 24-11-26.
//

#include "../inc/Account.h"
#include "../inc/dormManage.h"

#include <QCoreApplication>
#include <QSqlError>
#include <QSqlQuery>
#include <QCryptographicHash>

std::shared_ptr<Account> Account::login(const QString &id, const QString &pwd) {
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);

    if(!db.isOpen()) {
        qDebug() << "登录时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "登录时，数据库打开失败");
        QCoreApplication::exit(-1);
        return nullptr;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE student_id = :student_id AND password = :password");
    query.bindValue(":student_id", id);
    query.bindValue(":password", Account::processPwd(pwd));

    if(!query.exec()) {
        qDebug() << "查询执行失败：登录";
        QMessageBox::critical(nullptr, "Database Error", "查询执行失败：登录");
        QCoreApplication::exit(-1);
        return nullptr;
    }

    if(query.next()) {
        qDebug() << "登录成功";
        std::shared_ptr<Account> account = std::make_shared<Account>(
            query.value("student_id").toString(),
            query.value("name").toString(),
            query.value("gender").toString() == "男" ? Male : Female,
            query.value("password").toString(),
            query.value("telephone_number").toString(),
            query.value("role").toString() == "管理员" ? Admin : Student
        );
        return account;
    }
    else {
        qDebug() << "用户名或密码错误";
        return nullptr;
    }
}


bool Account::signUp() const {
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);

    if(!db.isOpen()) {
        qDebug() << "注册时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "注册时，数据库打开失败");
        QCoreApplication::exit(-1);
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
        "student_id TEXT PRIMARY KEY, "
        "name TEXT NOT NULL, "
        "gender TEXT CHECK (gender IN ('男', '女')), "
        "role TEXT CHECK (role IN ('学生', '管理员')), "
        "password TEXT NOT NULL, "
        "telephone_number TEXT NOT NULL "
        ");")) {
        qDebug() << "创建表失败：" << query.lastError().text();
        QMessageBox::critical(nullptr, "Database Error", "注册时，创建表失败：");
        QCoreApplication::exit(-1);
        return false;
    }

    query.prepare("INSERT INTO users (student_id, name, gender, role, password, telephone_number) "
                  "VALUES (:student_id, :name, :gender, :role, :password, :telephone_number)");

    query.bindValue(":student_id", student_id);
    query.bindValue(":name", name);
    query.bindValue(":gender", (gender == Male) ? "男" : "女");
    query.bindValue(":role", (role == Admin) ? "管理员" : "学生");
    query.bindValue(":password", processPwd(password));
    query.bindValue(":telephone_number", telephone_number);
    if (!query.exec()) {
        qDebug() << "插入用户失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "注册成功！";
    return true;
}

QString Account::get_id() const{
    return student_id;
}

QString Account::get_name() const{
    return name;
}

QString Account::get_telephone() const{
    return telephone_number;
}

Gender Account::get_gender() const{
    return gender;
}

Role Account::get_role() const{
    return role;
}

int Account::countNum(Role role) {
    QString r = (role == Student) ? "学生" : "管理员";
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);

    if(!db.isOpen()) {
        qDebug() << "统计总人数时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "统计总人数时，数据库打开失败");
        QCoreApplication::exit(-1);
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE role = :role");
    query.bindValue(":role", r);

    if(!query.exec()) {
        qDebug() << "查询执行失败：统计总人数";
        QMessageBox::critical(nullptr, "Database Error", "统计总人数时，查询执行失败");
        QCoreApplication::exit(-1);
        return -1;
    }
    if(query.next())
        return query.value(0).toInt();
    else
        return 0;
}

QString Account::processPwd(const QString &pwd) {
    QByteArray hashed = QCryptographicHash::hash(pwd.toUtf8(), QCryptographicHash::Sha256);
    return hashed.toHex();
}

QString Account::getName(const QString &stu_id) {
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);

    if (!db.isOpen()) {
        qDebug() << "获取姓名时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "获取姓名时，数据库打开失败");
        return {};
    }

    QSqlQuery query(db);
    query.prepare("SELECT name FROM users WHERE student_id = :student_id");
    query.bindValue(":student_id", stu_id);

    if (!query.exec()) {
        qDebug() << "获取姓名时，查询执行失败：" << query.lastError().text();
        return {};
    }

    if (query.next()) {
        return query.value("name").toString();
    } else {
        qDebug() << "未找到对应的记录";
        return {};
    }
}

void Account::StuInfo(QVector<Account> &stuInfo) {
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);
    stuInfo.clear();

    if (!db.isOpen()) {
        qDebug() << "获取学生信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "获取学生信息时，数据库打开失败");
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT student_id, name, gender, password, telephone_number, role FROM users WHERE role = '学生'");

    if (!query.exec()) {
        qDebug() << "获取学生信息时，查询执行失败：" << query.lastError().text();
        return;
    }

    while (query.next()) {
        Account student(
            query.value("student_id").toString(),
            query.value("name").toString(),
            query.value("gender").toString() == "男" ? Male : Female,
            "",
            query.value("telephone_number").toString(),
            Student
        );
        stuInfo.append(student);
    }
}

void Account::unChecked(QVector<Account> &unchecked) {
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);
    unchecked.clear();

    if (!db.isOpen()) {
        qDebug() << "获取未入住学生信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "获取未入住学生信息时，数据库打开失败");
        return;
    }

    QSqlQuery query(db);
    query.prepare(R"(
        SELECT u.student_id, u.name, u.gender, u.password, u.telephone_number
        FROM users u
        LEFT JOIN dormitories d ON u.student_id = d.student_id
        WHERE d.student_id IS NULL
        AND u.role = '学生'
    )");

    if (!query.exec()) {
        qDebug() << "获取未入住学生信息时，查询执行失败：" << query.lastError().text();
        return;
    }

    while (query.next()) {
        Account student(
            query.value("student_id").toString(),
            query.value("name").toString(),
            query.value("gender").toString() == "男" ? Male : Female,
            query.value("password").toString(),
            query.value("telephone_number").toString(),
            Student
        );
        unchecked.append(student);
    }

    query.prepare(R"(
        SELECT DISTINCT u.student_id, u.name, u.gender, u.password, u.telephone_number
        FROM users u
        LEFT JOIN dormitories d ON u.student_id = d.student_id
        WHERE u.role = '学生' AND d.student_id IS NOT NULL
        AND NOT EXISTS (
            SELECT 1
            FROM dormitories d2
            WHERE d2.student_id = u.student_id
            AND d2.status IN ('未确认', '已入住')
        );
    )");
    if (!query.exec()) {
        qDebug() << "获取未入住学生信息时，查询执行失败：" << query.lastError().text();
        return;
    }

    while (query.next()) {
        Account student(
            query.value("student_id").toString(),
            query.value("name").toString(),
            query.value("gender").toString() == "男" ? Male : Female,
            query.value("password").toString(),
            query.value("telephone_number").toString(),
            Student
        );
        unchecked.append(student);
    }
}

void Account::checkOutList(QVector<Account> &list, QVector<checkOut> &todo) {
    QSqlDatabase db = QSqlDatabase::database(ACCOUNT_CONNECTION);
    list.clear();

    if (!db.isOpen()) {
        qDebug() << "获取未入住学生信息时，数据库打开失败";
        QMessageBox::critical(nullptr, "Database Error", "获取未入住学生信息时，数据库打开失败");
        return;
    }

    QSqlQuery query(db);

    for (const auto &item : todo) {
        query.prepare("SELECT student_id, name, gender, password, telephone_number FROM users WHERE student_id = :student_id");
        query.bindValue(":student_id", item.stu_id);

        if (!query.exec()) {
            qDebug() << "获取未入住学生信息时，查询执行失败：" << query.lastError().text();
            continue;
        }

        if (query.next()) {
            Account student(
                query.value("student_id").toString(),
                query.value("name").toString(),
                query.value("gender").toString() == "男" ? Male : Female,
                query.value("password").toString(),
                query.value("telephone_number").toString(),
                Student
            );
            list.append(student);
        }
    }
}
