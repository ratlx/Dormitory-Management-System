//
// Created by 小火锅 on 24-11-27.
//

#ifndef ADMINWIDGET_H
#define ADMINWIDGET_H

#include <QWidget>
#include "Account.h"
#include <memory>
#include <QTimer>
#include <QtWidgets/QTableWidget>
#include "Buildings.h"
#include "Dormitory.h"
#include <QRegularExpression>

const int TABLE_MAX_SIZE = 1000;
static QRegularExpression re(R"(^\d{3}(-\d{3})?$)");

QT_BEGIN_NAMESPACE
namespace Ui { class adminWidget; }
QT_END_NAMESPACE

class administratorWidget : public QWidget {
Q_OBJECT

public:
    explicit administratorWidget(std::shared_ptr<Account> account, QWidget *parent = nullptr);
    ~administratorWidget() override;
    void updateSumTable();
    void buildSumTable();
    static QString buttonStylesheet();
    static void setItem(QTableWidget* table, int row, int col, const QString& text);
    void build_or_update_stuinfoTable(QVector<Account>& stuInfo, QTableWidget* stuInfoTable);
    void build_or_update_dormTable(QVector<Account>& dormInfo, QTableWidget* dormTable);
    static void build_or_update_bldgTable(QVector<Buildings>& bldgInfo, QTableWidget* bldgTable);
    static void build_or_update_dormManageTable(QVector<dormManage>& dormManageInfo, QTableWidget* dmtTable);
    void build_or_update_todoTable(QVector<Account>& list, QVector<checkOut>& todo, QTableWidget* todoTable);
    void build_or_update_historyTable(QVector<QPair<checkOut, Reply>>& record, QVector<Account>& list, QTableWidget* historyTable);
    void updateButtonStyles(QPushButton* activeButton);
    void checkDarkMode();
signals:
    void adminSwitchBack();
private slots:
    void on_stuInfoButton_clicked();
    void on_sumButton_clicked();
    void on_searchButton_clicked();
    void on_dormDistrubuteButton_clicked();
    void on_d_searchButton_clicked();
    void on_dormManageButton_clicked();
    void on_manageTab_currentChanged(int index);
    void on_reviewTab_currentChanged(int index);
    void on_delete_locCombo_currentTextChanged(const QString &loc);
    void on_dd_locCombo_currentTextChanged(const QString& loc);
    void on_dd_addButton_clicked();
    void on_b_deleteButton_clicked();
    void on_b_addButton_clicked();
    void on_b_modifyButton_clicked();
    void on_dd_modifyButton_clicked();
    void on_dd_deleteButton_clicked();
    void on_checkOutButton_clicked();
    void on_r_searchButton_clicked();
    void on_logoutButton_clicked();
private:
    Ui::adminWidget *ui;
    std::shared_ptr<Account> account;
    QTableWidget* sumTable;
    QVector<Account> stuInfo;
    QVector<Account> unchecked;
    QVector<Account> checkOutList;
    QVector<Buildings> bldgInfo;
    QVector<dormManage> dormManageInfo;
    QVector<checkOut> todo;
    QTimer* darkModeTimer;
    bool currentDarkMode;
};


#endif //ADMINWIDGET_H
