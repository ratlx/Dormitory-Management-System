//
// Created by 小火锅 on 24-12-6.
//

// You may need to build the project (run Qt uic code generator) to get "ui_detailDialog.h" resolved

#include "../inc/detaildialog.h"
#include "ui_detailDialog.h"
#include "../inc/Account.h"

detailDialog::detailDialog(QWidget *parent, const checkOut& check_out, const Reply& reply) :
    QDialog(parent), ui(new Ui::detailDialog), check_out(check_out), reply(reply)  {
    ui->setupUi(this);
    ui->applyBrowser->setText(check_out.detail);
    ui->replyBrowser->setText(reply.reply);
    QString time = "时间：" + reply.reply_date.toString("yyyy-MM-dd hh:mm");
    qDebug() << time;
    ui->time->setText(time);
    QString decision = "审核进度：" + checkOut::progressToString[check_out.progress];
    qDebug() << decision;
    ui->advice->setText(decision);
    QString name = "审核人：" + Account::getName(reply.admin_id);
    qDebug() << name;
    ui->name->setText(name);
}

detailDialog::~detailDialog() {
    delete ui;
}
