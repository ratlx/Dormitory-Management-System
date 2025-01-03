#include "../inc/login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->pwdLine->setEchoMode(QLineEdit::Password);

    this->setWindowTitle("宿舍管理系统-登录");
}

Login::~Login()
{
    delete ui;
}

void Login::on_logInButton_clicked()
{
    auto id = ui->idLine->text();
    auto pwd = ui->pwdLine->text();

    std::shared_ptr<Account> account = Account::login(id, pwd);

    if(account != nullptr) {
        QMessageBox::information(this, "Login Successful", "登录成功！");
        emit accountAuthenticatd(account);
        allLineClear();
    }
    else {
        QMessageBox::warning(this, "Login Failed", "用户名或密码错误，请重新输入！");
    }
}


void Login::on_SignUpButton_clicked()
{
    emit switchToSignup();
    allLineClear();
}

void Login::allLineClear() {
    ui->idLine->clear();
    ui->pwdLine->clear();
}




