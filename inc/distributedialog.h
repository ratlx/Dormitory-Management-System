//
// Created by 小火锅 on 24-12-7.
//

#ifndef DISTRIBUTEDIALOG_H
#define DISTRIBUTEDIALOG_H

#include <QDialog>

#include "Account.h"


QT_BEGIN_NAMESPACE
namespace Ui { class distributeDialog; }
QT_END_NAMESPACE

class distributeDialog : public QDialog {
Q_OBJECT

public:
    explicit distributeDialog(const Account& account, QWidget *parent = nullptr);
    ~distributeDialog() override;
private slots:
    void on_locCombo_currentTextChanged(const QString& loc);
    void on_confirmButton_clicked();
private:
    Ui::distributeDialog *ui;
    Account account;
};


#endif //DISTRIBUTEDIALOG_H
