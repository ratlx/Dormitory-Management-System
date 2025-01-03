//
// Created by 小火锅 on 24-11-27.
//

#include "../inc/dormManage.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QCoreApplication>
#include <QSqlError>

#include "../inc/Dormitory.h"

dormManage::dormManage(int id, const QString &building, int capacity)
    :dorm_id(id), building(building), curCheckins(0), capacity(capacity) {
    manage_id = getManageID(building, dorm_id);
}

dormManage::dormManage()
    :manage_id(0), dorm_id(-1), building(""), curCheckins(0), capacity(0){}

dormManage::dormManage(int id, const QString &building, int capacity, int cur, int m_id)
    :manage_id(m_id), dorm_id(id), building(building), capacity(capacity), curCheckins(cur){}

dormManage::dormManage(const QString &building, int dorm_id)
    : manage_id(-1), building(building), dorm_id(dorm_id), capacity(0), curCheckins(0) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "获取宿舍管理账号时，数据库打开失败";
    }

    QSqlQuery query(db);
    if(!query.prepare("SELECT * FROM dormitories_manage WHERE building = :building AND dorm_id = :dorm_id")) {
        qDebug() << "预处理 INSERT 语句失败：" << query.lastError().text();
        return;
    }
    query.bindValue(":building", building);
    query.bindValue(":dorm_id", dorm_id);
    if(!query.exec()) {
        qDebug() << "查询管理账号失败：" << query.lastError().text();
        return;
    }
    if(!query.next()) {
        qDebug() << "未找到相关管理账号";
        return;
    }
    manage_id = query.value("manage_id").toInt();
    capacity = query.value("capacity").toInt();
    curCheckins = query.value("cur_check_ins").toInt();
}


QString dormManage::capacityToName(int capacity) {
    switch (capacity) {
        case 1:
            return "单人间";
        case 2:
            return "双人间";
        case 4:
            return "四人间";
        case 6:
            return "六人间";
        case 8:
            return "八人间";
        case 10:
            return "十人间";
        default:
            qDebug() << "没有这种规格的宿舍！";
            return "";
    }
}

bool dormManage::manageStudent(int delta) {
    int newCheckins = curCheckins + delta;
    if(newCheckins > capacity || newCheckins < 0) {
        qDebug() << "添加学生失败：容量问题";
        return false;
    }

    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "宿舍学生管理失败：数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    if(!query.prepare("UPDATE dormitories_manage SET cur_check_ins = :cur_check_ins "
        "WHERE manage_id = :manage_id")) {
        qDebug() << "管理学生时，预处理 UPDATE 语句失败：" << query.lastError().text();
        return false;
    }

    query.bindValue(":cur_check_ins", newCheckins);
    query.bindValue(":manage_id", manage_id);

    if(!query.exec()) {
        qDebug() << "管理学生时，执行 UPDATE 语句失败：" << query.lastError().text();
        return false;
    }
    curCheckins = newCheckins;
    qDebug() << "管理学生操作成功";
    return true;
}

bool dormManage::addDorm(const QString& locName, int dorm_id, int capacity) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "添加宿舍失败：数据库打开失败";
        QMessageBox::critical(nullptr, "database error", "查看寝室信息时，数据库打开失败");
        return false;
    }

    QSqlQuery query(db);
    if(!query.exec(
        "CREATE TABLE IF NOT EXISTS dormitories_manage("
        "manage_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "dorm_id INTEGER NOT NULL,"
        "building TEXT NOT NULL,"
        "capacity INTEGER NOT NULL, "
        "cur_check_ins INTEGER DEFAULT 0, "
        "UNIQUE(dorm_id, building)"
        ");")) {
        qDebug() << "创建 dormitories_manage 表失败：" << query.lastError().text();
        QMessageBox::critical(nullptr, "Database Error", "创建 dormitories 表失败：" + query.lastError().text());
        QCoreApplication::exit(-1);
        return false;
    }

    if (!query.prepare(
    "INSERT INTO dormitories_manage (dorm_id, building, capacity, cur_check_ins) "
    "VALUES (:dorm_id, :building, :capacity, :cur_check_ins)")) {
        qDebug() << "预处理 INSERT 语句失败：" << query.lastError().text();
        return false;
    }

    query.bindValue(":dorm_id", dorm_id);
    query.bindValue(":building", locName);
    query.bindValue(":capacity", capacity);
    query.bindValue(":cur_check_ins", 0);

    if(!query.exec()) {
        qDebug() << "添加宿舍失败：" << query.lastError().text();
        return false;
    }
    qDebug() << "添加宿舍成功";
    return true;
}

int dormManage::getManageID(const QString &building, int dorm_id) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "获取宿舍管理id时，数据库打开失败";
        return -1;
    }

    QSqlQuery query(db);
    if(!query.prepare("SELECT manage_id FROM dormitories_manage WHERE building = :building AND dorm_id = :dorm_id")) {
        qDebug() << "预处理 INSERT 语句失败：" << query.lastError().text();
        return -1;
    }
    query.bindValue(":building", building);
    query.bindValue(":dorm_id", dorm_id);
    if(!query.exec()) {
        qDebug() << "查询管理id失败：" << query.lastError().text();
        return -1;
    }
    if(query.next()) {
        return query.value("manage_id").toInt();
    }
    else {
        qDebug() << "未找到匹配的管理id";
        return -1;
    }
}

std::shared_ptr<dormManage> dormManage::dormInfo(int manage_id) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if(!db.isOpen()) {
        qDebug() << "获取宿舍管理id时，数据库打开失败";
        return nullptr;
    }

    QSqlQuery query(db);
    if(!query.prepare("SELECT * from dormitories_manage where manage_id = :manage_id")) {
        qDebug() << "预处理 INSERT 语句失败：" << query.lastError().text();
        return nullptr;
    }
    query.bindValue(":manage_id", manage_id);
    if(!query.exec()) {
        qDebug() << "查询管理id失败：" << query.lastError().text();
        return nullptr;
    }
    if(!query.next()) {
        qDebug() << "找不到该学生的宿舍";
        return nullptr;
    }
    return std::make_shared<dormManage>(query.value("dorm_id").toInt(),
        query.value("building").toString(),
        query.value("capacity").toInt(),
        query.value("cur_check_ins").toInt(),
        manage_id);
}

bool dormManage::checkFloor(const QString& building, int floor) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "检查楼层时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    QString floorPrefix = QString::number(floor);
    if (!query.prepare("SELECT dorm_id FROM dormitories_manage WHERE building = :building AND dorm_id LIKE :floorPrefix || '%'")) {
        qDebug() << "预处理 SELECT 语句失败：" << query.lastError().text();
        return false;
    }

    query.bindValue(":building", building);
    query.bindValue(":floorPrefix", floorPrefix);

    if (!query.exec()) {
        qDebug() << "执行 SELECT 语句失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return false;
    }

    return true;
}

bool dormManage::checkFloors(const QString &building, int floors) {
    while(++floors < 10) {
        if(!checkFloor(building, floors))
            return false;
    }
    return true;
}

bool dormManage::checkRooms(const QString &building, int rooms) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "检查房间时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    if (!query.prepare("SELECT COUNT(*) FROM dormitories_manage WHERE building = :building")) {
        qDebug() << "预处理 SELECT 语句失败：" << query.lastError().text();
        return false;
    }

    query.bindValue(":building", building);

    if (!query.exec()) {
        qDebug() << "执行 SELECT 语句失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int roomCount = query.value(0).toInt();
        if (roomCount > rooms) {
            return false;
        }
    }

    return true;
}

bool dormManage::checkBeds(const QString &building, int beds) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "检查床位时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    if (!query.prepare("SELECT SUM(capacity) FROM dormitories_manage WHERE building = :building")) {
        qDebug() << "预处理 SELECT 语句失败：" << query.lastError().text();
        return false;
    }

    query.bindValue(":building", building);

    if (!query.exec()) {
        qDebug() << "执行 SELECT 语句失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        int totalCapacity = query.value(0).toInt();
        if (totalCapacity > beds) {
            return false;
        }
    }
    return true;
}

bool dormManage::operator<(const dormManage &d) const {
    if(building != d.building)
        return building < d.building;
    return dorm_id < d.dorm_id;
}

void dormManage::getAll(QVector<dormManage> &dormInfo) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "获取所有宿舍信息时，数据库打开失败";
        return;
    }
    dormInfo.clear();
    QSqlQuery query(db);
    if (!query.exec("SELECT * FROM dormitories_manage")) {
        qDebug() << "执行 SELECT 语句失败：" << query.lastError().text();
        return;
    }

    while (query.next()) {
        dormManage dorm(
            query.value("dorm_id").toInt(),
            query.value("building").toString(),
            query.value("capacity").toInt(),
            query.value("cur_check_ins").toInt(),
            query.value("manage_id").toInt()
        );
        dormInfo.append(dorm);
    }
    std::sort(dormInfo.begin(), dormInfo.end());
}

bool dormManage::deleteDorm(const QString &building) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "删除宿舍时，数据库打开失败";
        return false;
    }

    QSqlQuery query(db);
    query.exec("PRAGMA foreign_keys = OFF;");
    query.prepare("DELETE FROM dormitories_manage WHERE building = :building");
    query.bindValue(":building", building);

    if (!query.exec()) {
        qDebug() << "删除宿舍失败：" << query.lastError().text();
        return false;
    }

    qDebug() << "删除宿舍成功";
    return true;
}

int dormManage::addDorms(const QString &locName, int low, int up, int capacity) {
    int successCount = 0;
    for (int dorm_id = low; dorm_id <= up; ++dorm_id) {
        if (addDorm(locName, dorm_id, capacity)) {
            ++successCount;
        }
    }
    return successCount;
}

int dormManage::NameToCapacity(const QString &name) {
    if (name == "单人间") {
        return 1;
    } else if (name == "双人间") {
        return 2;
    } else if (name == "四人间") {
        return 4;
    } else if (name == "六人间") {
        return 6;
    } else if (name == "八人间") {
        return 8;
    } else if (name == "十人间") {
        return 10;
    } else {
        qDebug() << "没有这种规格的宿舍！";
        return -1;
    }
}

QPair<int, bool> dormManage::check_rooms_and_beds(const QString &locName, int low, int up, int capacity) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "检查房间和床位时，数据库打开失败";
        return qMakePair(0, false);
    }

    QSqlQuery query(db);
    int totalCapacity = 0;

    for (int dorm_id = low; dorm_id <= up; ++dorm_id) {
        if (!query.prepare("SELECT capacity, cur_check_ins FROM dormitories_manage WHERE building = :building AND dorm_id = :dorm_id")) {
            qDebug() << "预处理 SELECT 语句失败：" << query.lastError().text();
            return qMakePair(0, false);
        }

        query.bindValue(":building", locName);
        query.bindValue(":dorm_id", dorm_id);

        if (!query.exec()) {
            qDebug() << "执行 SELECT 语句失败：" << query.lastError().text();
            return qMakePair(0, false);
        }

        if (!query.next()) {
            qDebug() << "未找到宿舍 ID：" << dorm_id;
            return qMakePair(0, false);
        }

        int dormCapacity = query.value("capacity").toInt();
        int curCheckins = query.value("cur_check_ins").toInt();

        if (curCheckins > capacity) {
            qDebug() << "宿舍 ID：" << dorm_id << "的当前入住人数超过容量";
            return qMakePair(0, false);
        }

        totalCapacity += dormCapacity;
    }

    return qMakePair(totalCapacity, true);
}

int dormManage::changeCapacity(const QString &locName, int low, int up, int capacity) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "修改宿舍容量时，数据库打开失败";
        return 0;
    }

    QSqlQuery query(db);
    int successCount = 0;

    for (int dorm_id = low; dorm_id <= up; ++dorm_id) {
        if (!query.prepare("UPDATE dormitories_manage SET capacity = :capacity WHERE building = :building AND dorm_id = :dorm_id")) {
            qDebug() << "预处理 UPDATE 语句失败：" << query.lastError().text();
            return successCount;
        }

        query.bindValue(":capacity", capacity);
        query.bindValue(":building", locName);
        query.bindValue(":dorm_id", dorm_id);

        if (!query.exec()) {
            qDebug() << "执行 UPDATE 语句失败：" << query.lastError().text();
            continue;
        }

        ++successCount;
    }

    return successCount;
}

int dormManage::deleteDorms(const QString &locName, int low, int up) {
    auto db = QSqlDatabase::database(DORM_MANAGE_CONNECTION);
    if (!db.isOpen()) {
        qDebug() << "删除宿舍时，数据库打开失败";
        return 0;
    }

    QSqlQuery query(db);
    int successCount = 0;

    query.exec("PRAGMA foreign_keys = OFF;");

    for (int dorm_id = low; dorm_id <= up; ++dorm_id) {
        if (!query.prepare("DELETE FROM dormitories_manage WHERE building = :building AND dorm_id = :dorm_id")) {
            qDebug() << "预处理 DELETE 语句失败：" << query.lastError().text();
            return successCount;
        }

        query.bindValue(":building", locName);
        query.bindValue(":dorm_id", dorm_id);

        if (!query.exec()) {
            qDebug() << "执行 DELETE 语句失败：" << query.lastError().text();
            continue;
        }

        ++successCount;
    }

    return successCount;
}