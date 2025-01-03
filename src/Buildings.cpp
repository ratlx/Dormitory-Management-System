//
// Created by 小火锅 on 24-11-27.
//

#include "../inc/Buildings.h"

#include <set>

Buildings::Buildings(const QString& name, const QString& location, Gender gender, int floors, int rooms, int beds, int check_ins)
:LocName(location + '_' + name), location(location), gender(gender), floors(floors), rooms(rooms), beds(beds), check_ins(check_ins) {}

bool Buildings::addBuilding() {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(LocName);
    if(settings.contains("location")) {
        qDebug() << "该楼栋已存在！";
        return false;
    }
    else {
        settings.setValue("location", location);
        settings.setValue("rooms", rooms);
        settings.setValue("gender", gender == Female ? "女" : "男");
        settings.setValue("floors", floors);
        settings.setValue("beds", beds);
        settings.setValue("check_ins", check_ins);
        settings.endGroup();
        qDebug() << "楼栋添加成功！";
        return true;
    }
}

bool Buildings::deleteBuilding(const QString &LocName, QWidget *parent) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(LocName);
    if(!settings.contains("location")) {
        qDebug() << "该楼栋不存在！";
        return false;
    }
    else if(settings.value("check_ins").toInt() != 0){
        qDebug() << "该楼栋仍有人居住，不能删除！";
        QMessageBox::warning(parent, "delete error", "该楼栋仍有人居住，不能删除！");
        return false;
    }
    else {
        if(!dormManage::deleteDorm(LocName))
            return false;
        settings.remove("");
        settings.endGroup();
        qDebug() << "楼栋删除成功！";
        return true;
    }
}

Buildings Buildings::searchBuilding(const QString &LocName) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(LocName);
    if(!settings.contains("location")) {
        qDebug() << "该楼栋不存在！";
        return Buildings("", "", None, 0, 0, 0, 0);
    }
    else {
        QString loc = settings.value("location").toString();
        Gender gender = settings.value("gender").toString() == "女" ? Female : Male;
        int floors = settings.value("floors").toInt();
        int rooms = settings.value("rooms").toInt();
        int beds = settings.value("beds").toInt();
        int check_ins = settings.value("check_ins").toInt();
        settings.endGroup();
        qDebug() << "查找成功！";
        return Buildings(LocName, loc, gender, floors, rooms, beds, check_ins);
    }
}

bool Buildings::modifyCheckIns(const QString &LocName, int delta) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(LocName);
    if(!settings.contains("location")) {
        qDebug() << "该楼栋不存在！";
        return false;
    }
    else {
        int check_ins = settings.value("check_ins").toInt();
        if(check_ins + delta < 0) {
            qDebug() << "入住人数不能为负！";
            return false;
        }
        check_ins += delta;
        settings.setValue("check_ins", check_ins);
        settings.endGroup();
        qDebug() << "修改成功！";
        return true;
    }
}

int Buildings::counts(const QString& type) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    QStringList groups = settings.childGroups();        //获取所有分组

    if(type != "rooms" && type != "beds" && type != "floors" && type != "check_ins") {
        qDebug() << "无效的类型！";
        return -1;
    }

    int count = 0;
    for(const auto& group : groups) {
        settings.beginGroup(group);
        count += settings.value(type).toInt();
        settings.endGroup();
    }

    return count;
}

QPair<QString, QString> Buildings::split(const QString &LocName) {
    auto pos = LocName.indexOf('_');
    return qMakePair(LocName.mid(0, pos), LocName.mid(pos+1));
}

void Buildings::getLoc(Gender gender, std::set<QString>& loc) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    QStringList groups = settings.childGroups(); // Get all groups
    loc.clear();
    for (const auto& group : groups) {
        settings.beginGroup(group);
        QString genderStr = settings.value("gender").toString();
        if (gender == None || (gender == Male && genderStr == "男") || (gender == Female && genderStr == "女")) {
            loc.insert(settings.value("location").toString());
        }
        settings.endGroup();
    }

}

void Buildings::getBuildings(Gender gender, const QString &loc, QVector<QString> &buildings) {
    buildings.clear();
    if(loc.isEmpty()) return;
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    QStringList groups = settings.childGroups(); // Get all groups

    for (const auto& group : groups) {
        settings.beginGroup(group);
        QString genderStr = settings.value("gender").toString();
        if ((gender == None || (gender == Male && genderStr == "男") || (gender == Female && genderStr == "女")) && split(group).first == loc) {
            //qDebug() <<
            buildings.append(split(group).second);
        }
        settings.endGroup();
    }
}

void Buildings::getAllBldgs(QVector<Buildings>& bldgs) {
    bldgs.clear();
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    QStringList groups = settings.childGroups(); // Get all groups

    for (const auto& group : groups) {
        settings.beginGroup(group);
        QString loc = settings.value("location").toString();
        Gender gender = settings.value("gender").toString() == "女" ? Female : Male;
        int floors = settings.value("floors").toInt();
        int rooms = settings.value("rooms").toInt();
        int beds = settings.value("beds").toInt();
        int check_ins = settings.value("check_ins").toInt();
        settings.endGroup();
        bldgs.append(Buildings(split(group).second, loc, gender, floors, rooms, beds, check_ins));
    }
}

bool Buildings::modify(const QString &loc, const QString &bldg, int floors, int rooms, int beds) {
    QString locName = loc + '_' + bldg;
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(locName);
    if (!settings.contains("location")) {
        qDebug() << "该楼栋不存在！";
        return false;
    }
    int cnt = 0;

    bool floorsValid = (floors == 0 || dormManage::checkFloors(locName, floors));
    bool roomsValid = (rooms == 0 || dormManage::checkRooms(locName, rooms));
    bool bedsValid = (beds == 0 || dormManage::checkBeds(locName, beds));

    if (!floorsValid || !roomsValid || !bedsValid) {
        return false;
    }

    if (floors != 0) {
        settings.setValue("floors", floors);
        cnt++;
    }
    if (rooms != 0) {
        settings.setValue("rooms", rooms);
        cnt++;
    }
    if (beds != 0) {
        settings.setValue("beds", beds);
        cnt++;
    }
    settings.endGroup();
    return cnt != 0;
}

int Buildings::getBeds(const QString &LocName) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(LocName);
    if (!settings.contains("location")) {
        qDebug() << "该楼栋不存在！";
        return -1;
    }
    int beds = settings.value("beds").toInt();
    settings.endGroup();
    qDebug() << "楼栋床位数：" << beds;
    return beds;
}

int Buildings::getRooms(const QString &LocName) {
    QSettings settings(BUILDINGS_FILE, QSettings::IniFormat);
    settings.beginGroup(LocName);
    if (!settings.contains("location")) {
        qDebug() << "该楼栋不存在！";
        return -1;
    }
    int rooms = settings.value("rooms").toInt();
    settings.endGroup();
    return rooms;
}

