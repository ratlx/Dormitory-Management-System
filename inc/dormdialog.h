//
// Created by 小火锅 on 24-12-6.
//

#ifndef DORMDIALOG_H
#define DORMDIALOG_H

#include <QDialog>
#include "Dormitory.h"
#include "Account.h"
#include "dormManage.h"
#include "Buildings.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class dormDialog; }
QT_END_NAMESPACE

class dormDialog : public QDialog {
Q_OBJECT

public:
    dormDialog(QWidget *parent, const Account& account,
        std::shared_ptr<Dormitory> dormitory, std::shared_ptr<dormManage> dorm_manage, bool modify);
    ~dormDialog() override;


private:
    Ui::dormDialog *ui;
    Account account;
    std::shared_ptr<Dormitory> dormitory;
    std::shared_ptr<dormManage> dorm_manage;
    bool modify;
};


#endif //DORMDIALOG_H
