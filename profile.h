#ifndef PROFILE_H
#define PROFILE_H

#include <QDialog>

#include <QString>
#include <QSqlTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox> //Всплывающие окна с ошибками
#include <QtSql/QtSql>
#include <QtDebug>

#include "changepass.h"

namespace Ui {
class Profile;
}

class Profile : public QDialog
{
    Q_OBJECT

public:
    explicit Profile(int row = -1, QString accessLevel = "Пользователь", QWidget *parent = nullptr);
    ~Profile();

signals:
    void signal_updateTable();
    void signal_exitAccount();

private slots:
    void on_exitProfile_pushButton_clicked();
    void on_saveChange_pushButton_clicked();
    void on_changePassword_pushButton_clicked();

    void on_deleteAccount_pushButton_clicked();

private:
    Ui::Profile *ui;

    QSqlTableModel *model;
    QDataWidgetMapper *mapper;

    QString m_lin; //Для проверки при редактировании, был ли редактирован ЛИН
    QString m_accessLevel;
    QString m_pass;
};

#endif // PROFILE_H
