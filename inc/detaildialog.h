//
// Created by 小火锅 on 24-12-6.
//

#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>
#include <QDateTime>
#include "Reply.h"
#include "checkOut.h"

QT_BEGIN_NAMESPACE
namespace Ui { class detailDialog; }
QT_END_NAMESPACE

class detailDialog : public QDialog {
Q_OBJECT

public:
    explicit detailDialog(QWidget *parent, const checkOut& check_out, const Reply& reply);
    ~detailDialog() override;

private:
    Ui::detailDialog *ui;
    checkOut check_out;
    Reply reply;
};


#endif //DETAILDIALOG_H
