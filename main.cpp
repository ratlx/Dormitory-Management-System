#include "inc/mainwindow.h"
#include "inc/checkOut.h"
#include "inc/studentwidget.h"
#include "inc/Reply.h"
#include <QApplication>
#include "inc/administratorWidget.h"
#include <QDir>
#include <Qt>
#include <fstream>
#include "inc/Account.h"
#include <QSqlDatabase>
#include "inc/dormManage.h"
#include "inc/Dormitory.h"
#include <QSqlQuery>
#include <QTime>
#include "inc/dormdialog.h"
#include "inc/Buildings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow DMS;
    DMS.show();
    //批量导入数据
    /*auto db = QSqlDatabase::addDatabase("QSQLITE", "dms_conn");
    db.setDatabaseName("DMS.db");
    std::fstream ipf("users.txt");
    int bd = 101, gd = 101;
    std::string name, id, tel, gender;*/
   /*while(ipf >> name >> id >> gender >> tel) {
       QString qName = QString::fromStdString(name);
       QString qId = QString::fromStdString(id);
       QString qTel = QString::fromStdString(tel);
       QString qGender = QString::fromStdString(gender);
       Account account(qId, qName, qGender == "男" ? Male : Female, "123456", qTel, Student);
       //account.signUp();
        if(qGender == "男") {
            if(!Dormitory::checkIn_and_modify(account.get_id(), "德智园区", "5栋", bd)) {
                if(++bd % 100 > 30) bd = bd / 100 * 100 + 101;
                Dormitory::checkIn_and_modify(account.get_id(), "德智园区", "5栋", bd);
            }
        }
        else if(qGender == "女") {
            if(!Dormitory::checkIn_and_modify(account.get_id(), "望麓桥学生公寓", "2栋北", gd)) {
                if(++gd % 100 > 30) gd = gd / 100 * 100 + 101;
                Dormitory::checkIn_and_modify(account.get_id(), "望麓桥学生公寓", "2栋北", gd);
            }
        }
    }
    qDebug() << "Done!";*/
    return a.exec();
}
