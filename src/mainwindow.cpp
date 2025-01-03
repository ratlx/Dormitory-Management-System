//
// Created by 小火锅 on 24-11-25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "../inc/mainwindow.h"

#include <QtWidgets/QVBoxLayout>

#include "ui_MainWindow.h"
#include "../inc/dormManage.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), authenticated_account(nullptr), stuWidget(nullptr), adminWidget(nullptr) {
    ui->setupUi(this);
    this->setWindowTitle("宿舍管理系统");

    if(!openDatabase(ACCOUNT_DATABASE_FILE, ACCOUNT_CONNECTION)) {
        QMessageBox::critical(this, "Database Error", "用户数据库连接失败");
        QCoreApplication::exit(-1);
    }

    loginWidget = new Login(this);
    signupWidget = new Signup(this);
    auto* loginLayout = new QVBoxLayout(ui->loginPage);
    loginLayout->addWidget(loginWidget);
    loginLayout->setAlignment(Qt::AlignCenter);
    auto* signupLayout = new QVBoxLayout(ui->signupPage);
    signupLayout->addWidget(signupWidget);
    signupLayout->setAlignment(Qt::AlignCenter);
    connect(loginWidget, &Login::switchToSignup, this, &MainWindow::showSignup);
    connect(signupWidget, &Signup::switchToLogin, this, &MainWindow::showLogin);
    connect(loginWidget, &Login::accountAuthenticatd, this, &MainWindow::signupSuccess);

    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

MainWindow::~MainWindow() {
    delete ui;
    QSqlDatabase::database().close();
}

void MainWindow::showLogin() {
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
}

void MainWindow::showSignup() {
    ui->stackedWidget->setCurrentWidget(ui->signupPage);
}

bool MainWindow::openDatabase(const QString& path_to_database, const QString& connection_name) {
    if(QSqlDatabase::contains(connection_name)) {
        QSqlDatabase db = QSqlDatabase::database(connection_name);
        if(db.isOpen()) {
            qDebug() << "用户数据库已经打开: " << path_to_database;
            return true;
        }
    }
    auto db = QSqlDatabase::addDatabase("QSQLITE", connection_name);
    db.setDatabaseName(path_to_database);

    if(!db.open()) {
        qDebug() << "打开用户数据库失败:" << path_to_database;
        return false;
    }
    qDebug() << "成功打开用户数据库:" << path_to_database;
    return true;
}

void MainWindow::signupSuccess(std::shared_ptr<Account> authenticated_account) {
    loginWidget->allLineClear();
    signupWidget->allLineClear();
    this->authenticated_account = authenticated_account;
    openDatabase(DORM_MANAGE_DATABASE_FILE, DORM_MANAGE_CONNECTION);
    if(authenticated_account->get_role() == Student) {
        loadStudentWidget();
        connect(stuWidget, &studentWidget::stuSwitchBack, this, &MainWindow::stuLogout);
    }
    else if(authenticated_account->get_role() == Admin) {
        loadAdminWidget();
        connect(adminWidget, &administratorWidget::adminSwitchBack, this, &MainWindow::adminLogout);
    }
}

void MainWindow::loadStudentWidget() {
    stuWidget = new studentWidget(authenticated_account, this);
    ui->stackedWidget->addWidget(stuWidget);
    setCentralWidget(ui->stackedWidget);
    ui->stackedWidget->setCurrentWidget(stuWidget);
}

void MainWindow::loadAdminWidget() {
    adminWidget = new administratorWidget(authenticated_account, this);
    ui->stackedWidget->addWidget(adminWidget);
    setCentralWidget(ui->stackedWidget);
    ui->stackedWidget->setCurrentWidget(adminWidget);
}

void MainWindow::stuLogout() {
    showLogin();
}

void MainWindow::adminLogout() {
    showLogin();
}
