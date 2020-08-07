#include "changepass.h"
#include "ui_changepass.h"

changePass::changePass(QString lin, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changePass)
{
    ui->setupUi(this);

    m_lin = lin;
}

changePass::~changePass()
{
    delete ui;
}

void changePass::on_changePass_pushButton_clicked()
{
    QSqlQuery query;
    QString str = QString("SELECT * "
                          "FROM userlist "
                          "WHERE lin = '%1'")
                          .arg(m_lin);

    if(!query.exec(str))
        qDebug() << "Error: " << query.lastError() << " : " << query.lastQuery();
    query.next();

    if (query.value(2).toString() == ui->pass_lineEdit->text()) {
        if (ui->newPass_lineEdit->text() == ui->conPass_lineEdit->text()){
            str = QString("UPDATE userlist SET password = '%1' WHERE lin = '%2';")
                    .arg(ui->newPass_lineEdit->text())
                    .arg(m_lin);
            if(!query.exec(str))
                qDebug() << "Error: " << query.lastError() << " : " << query.lastQuery();

            this->close();
        } else {
            QMessageBox::information(this, "Ошибка смены пароля",
                "Пароли не совпадают!");
        }
    } else {
        QMessageBox::information(this, "Ошибка смены пароля",
            "Неверно указан текущий пароль!");
    }
}
