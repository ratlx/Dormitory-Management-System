//
// Created by 小火锅 on 24-11-25.
//

// You may need to build the project (run Qt uic code generator) to get "ui_signup.h" resolved

#include "../inc/Signup.h"



#include "ui_signup.h"


Signup::Signup(QWidget *parent) :
    QWidget(parent), ui(new Ui::signup) {
    ui->setupUi(this);
    ui->pwdLine->setEchoMode(QLineEdit::Password);
    this->setWindowTitle("宿舍管理系统-注册");
}

Signup::~Signup() {
    delete ui;
}

void Signup::on_signupButton_clicked()
{
    if(ui->idLine->text().isEmpty() || ui->nameLine->text().isEmpty() || ui->pwdLine->text().isEmpty() || ui->telLine->text().isEmpty() || ui->genderBox->currentIndex() == 0) {
        QMessageBox::warning(this, "Signup Failed", "要乖乖填好全部信息哦。");
        return;
    }
    auto student_id = ui->idLine->text();
    auto name = ui->nameLine->text();
    auto gender = ui->genderBox->currentText() == "男" ? Male : Female;
    auto password = ui->pwdLine->text();
    auto telephone_number = ui->telLine->text();
    Account account(student_id, name, gender,password, telephone_number, Student);
    if(account.signUp()) {
        QMessageBox::information(this, "Signup Success", "注册成功");
        emit switchToLogin();
        allLineClear();
    }
    else {
        QMessageBox::warning(this, "Signup Failed", "用户已存在，请重新输入！");
        ui->idLine->clear();
    }
}

void Signup::on_backToLoginButton_clicked()
{
    emit switchToLogin();
    allLineClear();
}

void Signup::allLineClear() {
    ui->genderBox->setCurrentIndex(0);
    ui->idLine->clear();
    ui->nameLine->clear();
    ui->telLine->clear();
    ui->pwdLine->clear();
}

