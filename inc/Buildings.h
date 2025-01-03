//
// Created by 小火锅 on 24-11-27.
//

#ifndef BUILDINGS_H
#define BUILDINGS_H

#include <QString>
#include <QSettings>
#include "dormManage.h"
#include <set>
#include "Account.h"

const QString BUILDINGS_FILE = "buildings.ini";

class Buildings {
public:
    explicit Buildings(const QString& name, const QString& location, Gender gender, int floors, int rooms, int beds, int check_ins);
    bool addBuilding();
    static bool deleteBuilding(const QString& LocName, QWidget *parent = nullptr);
    static Buildings searchBuilding(const QString& LocName);
    static bool modifyCheckIns(const QString& LocName, int delta);
    static int counts(const QString& type);
    static QPair<QString, QString> split(const QString& LocName);
    static void getLoc(Gender gender, std::set<QString>& loc);
    static void getBuildings(Gender gender, const QString& loc, QVector<QString>& buildings);
    static int getBeds(const QString& LocName);
    static int getRooms(const QString& LocName);
    static void getAllBldgs(QVector<Buildings>& Bldg);
    static bool modify(const QString& loc, const QString& bldg, int floors, int rooms, int beds);
    QString LocName;
    QString location;
    Gender gender;
    int floors;
    int rooms;
    int beds;
    int check_ins;
};



#endif //BUILDINGS_H
