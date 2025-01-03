//
// Created by 小火锅 on 24-11-27.
//

#ifndef ROOM_H
#define ROOM_H

#include <QString>
#include <QVector>
#include <memory>

#include "Account.h"

const QString DORM_MANAGE_DATABASE_FILE = "DMS.db";
const QString DORM_MANAGE_CONNECTION = "dms_conn";
class dormManage {
public:
    dormManage();
    explicit dormManage(int id, const QString& building, int capacity, int cur, int m_id);
    explicit dormManage(int id, const QString& building, int capacity);
    explicit dormManage(const QString& building, int dorm_id);
    static QString capacityToName(int capacity);
    static int NameToCapacity(const QString& name);
    static int getManageID(const QString& building, int dorm_id);
    static std::shared_ptr<dormManage> dormInfo(int manage_id);
    static bool checkFloor(const QString& building, int floor);
    static bool checkFloors(const QString& building, int floors);
    static bool checkRooms(const QString& building, int rooms);
    static bool checkBeds(const QString& building, int beds);
    static bool deleteDorm(const QString& building);
    bool operator<(const dormManage& d) const;
    static void getAll(QVector<dormManage>& dormInfo);
    bool manageStudent(int delta);
    static bool addDorm(const QString& locName, int dorm_id, int capacity);
    static int addDorms(const QString& locName, int low, int up, int capacity);
    static int changeCapacity(const QString& locName, int low, int up, int capacity);
    static QPair<int, bool> check_rooms_and_beds(const QString& locName, int low, int up, int capacity);
    static int deleteDorms(const QString& locName, int low, int up);
    int manage_id;           //唯一标识，方便数据库操作
    QString building;        //楼栋号
    int dorm_id;             //宿舍号
    int capacity;            //容量
    int curCheckins;         //入住人数
};



#endif //ROOM_H
