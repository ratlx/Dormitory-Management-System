//
// Created by 小火锅 on 24-11-27.
//

#ifndef STUDENTWIDGET_H
#define STUDENTWIDGET_H

#include <QWidget>
#include <memory>
#include <QtWidgets/QTableWidget>
#include "Dormitory.h"
#include "dormManage.h"
#include "Account.h"
#include "dark.h"
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class studentWidget; }
QT_END_NAMESPACE

class studentWidget : public QWidget {
Q_OBJECT

public:
    explicit studentWidget(std::shared_ptr<Account> account, QWidget *parent = nullptr);
    ~studentWidget() override;
    void PerInfoTable();
    QString buttonStylesheet();
private slots:
    void on_dormitoryButton_clicked();
    void on_confirmButton_clicked();
    void on_recordButton_clicked();
    void on_perButton_clicked();
    void on_checkOutButton_clicked();
    void on_sendButton_clicked();
    void on_checkoutTab_currentChanged(int index);
    void on_logOutButton_clicked();
    void checkDarkMode();
signals:
    void stuSwitchBack();
private:
    bool sendMessageBox();
    Ui::studentWidget* ui;
    std::shared_ptr<Account> account;
    std::shared_ptr<Dormitory> dormInfo;
    std::shared_ptr<dormManage> manageInfo;
    QTableWidget* perInfotable;
    QTableWidget* dormInfotable;
    QTableWidget* recordInfotable;
    Mode mode;
    QTimer* darkModeTimer;
    bool currentDarkMode;
};


#endif //STUDENTWIDGET_H
