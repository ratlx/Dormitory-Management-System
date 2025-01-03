#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>
#include <memory>

#include "Account.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    void allLineClear();
    ~Login();

private slots:
    void on_logInButton_clicked();

    void on_SignUpButton_clicked();

signals:
    void switchToSignup();

    void accountAuthenticatd(std::shared_ptr<Account> account);

private:
    Ui::Widget *ui;
};
#endif // LOGIN_H
