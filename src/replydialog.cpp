//
// Created by 小火锅 on 24-12-7.
//

// You may need to build the project (run Qt uic code generator) to get "ui_replyDialog.h" resolved

#include "../inc/replydialog.h"
#include "ui_replyDialog.h"
#include "../inc/Reply.h"

replyDialog::replyDialog(std::shared_ptr<Account> account, const checkOut& check_out, QWidget *parent) :
    check_out(check_out), account(account), QDialog(parent), ui(new Ui::replyDialog) {
    ui->setupUi(this);
    this->setWindowTitle("审核回复");

}

replyDialog::~replyDialog() {
    delete ui;
}

void replyDialog::on_sendButton_clicked() {
    if(ui->adviceCombo->currentIndex() == 0 || ui->replyEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "send error", "请填写完所有信息！");
        return;
    }
    auto reply = ui->replyEdit->toPlainText();
    auto decision = ui->adviceCombo->currentIndex() == 1 ? Reply::agreed : Reply::disagreed;
    if(Reply::review_and_modify(account->get_id(), reply, check_out.checkout_id, decision)) {
        QMessageBox::warning(this, "send success", "发送成功！");
        this->close();
        return;
    }
    QMessageBox::warning(this, "send error", "发送失败！");
}
