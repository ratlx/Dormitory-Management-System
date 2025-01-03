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

    return a.exec();
}
