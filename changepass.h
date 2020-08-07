#ifndef CHANGEPASS_H
#define CHANGEPASS_H

#include <QDialog>

#include <QMessageBox> //Всплывающие окна с ошибками
#include <QtSql/QtSql>

namespace Ui {
class changePass;
}

class changePass : public QDialog
{
    Q_OBJECT

public:
    explicit changePass(QString lin, QWidget *parent = nullptr);
    ~changePass();

private slots:
    void on_changePass_pushButton_clicked();

private:
    Ui::changePass *ui;

    QString m_lin;
};

#endif // CHANGEPASS_H
