//
// Created by 小火锅 on 24-11-25.
//

#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include <QMessageBox>

#include "Account.h"

QT_BEGIN_NAMESPACE
namespace Ui { class signup; }
QT_END_NAMESPACE

class Signup : public QWidget {
Q_OBJECT

public:
    explicit Signup(QWidget *parent = nullptr);
    void allLineClear();
    ~Signup() override;

private slots:
    void on_signupButton_clicked();
    void on_backToLoginButton_clicked();

signals:
    void switchToLogin();

private:
    Ui::signup *ui;
};


#endif //SIGNUP_H
