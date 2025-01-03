//
// Created by 小火锅 on 24-11-25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <memory>

#include "login.h"
#include "Signup.h"
#include "Account.h"
#include "studentwidget.h"
#include "administratorWidget.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void showLogin();
    void showSignup();
    void stuLogout();
    void signupSuccess(std::shared_ptr<Account> authenticated_account);
    void adminLogout();
private:
    Ui::MainWindow *ui;
    Login* loginWidget;
    Signup* signupWidget;
    std::shared_ptr<Account> authenticated_account;
    studentWidget* stuWidget;
    administratorWidget* adminWidget;
    bool openDatabase(const QString& path_to_database, const QString& connection_name);
    void loadStudentWidget();
    void loadAdminWidget();
};


#endif //MAINWINDOW_H
