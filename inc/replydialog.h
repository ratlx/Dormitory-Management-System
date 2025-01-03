//
// Created by 小火锅 on 24-12-7.
//

#ifndef REPLYDIALOG_H
#define REPLYDIALOG_H

#include <QDialog>

#include "Account.h"


QT_BEGIN_NAMESPACE
namespace Ui { class replyDialog; }
QT_END_NAMESPACE

class replyDialog : public QDialog {
Q_OBJECT

public:
    explicit replyDialog(std::shared_ptr<Account> account, const checkOut& check_out, QWidget *parent = nullptr);
    ~replyDialog() override;
private slots:
    void on_sendButton_clicked();
private:
    Ui::replyDialog *ui;
    std::shared_ptr<Account> account;
    checkOut check_out;
};


#endif //REPLYDIALOG_H
