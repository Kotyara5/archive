#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QtSql/QtSql>
#include "auth_window.h"
#include "reg_window.h"
#include "profile.h"

#include <QTableView> //Для отображения таблицы
#include <QMessageBox> //Всплывающие окна с ошибками

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void display();
    bool connectDB();

private:
    Ui::MainWindow *ui;

    auth_window *ui_Auth = new auth_window();
    reg_window *ui_Reg = new reg_window();

    QString m_username;
    QString m_userpass;
    QString m_accessLevel;

    QString db_input;
    QSqlDatabase mw_db;

    QSqlTableModel  *model; //Вывод базы данных на экран

    int m_row;

signals:
    void signal_logoff_pushButton_clicked();

private slots:
    void authorizeUser();
    void registerWindowShow();
    void registerUser();

    void slot_UpdateModels();

    void on_logoff_pushButton_clicked();
    void on_myProfile_pushButton_clicked();
    void on_tableView_doubleClicked(const QModelIndex &index);
    void on_newProfile_pushButton_clicked();
};
#endif // MAINWINDOW_H
