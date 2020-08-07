#include "profile.h"
#include "ui_profile.h"

Profile::Profile(int row, QString accessLevel, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Profile)
{
    ui->setupUi(this);

    m_accessLevel = accessLevel;

    //Инициализируем модель и делаем выборку из неё
    model = new QSqlTableModel(this);
    model->setTable("userlist");
    model->select();

    // Инициализируем mapper и привязываемполя данных к объектам LineEdit
    mapper = new QDataWidgetMapper();
    mapper->setModel(model);
    mapper->addMapping(ui->lin_lineEdit, 0);
    mapper->addMapping(ui->nickname_lineEdit, 1);
    mapper->addMapping(ui->date_lineEdit, 3);
    mapper->addMapping(ui->status_comboBox, 4);
    mapper->addMapping(ui->accessLevel_comboBox, 5);

    if (row == -1) { //Создать новую запись
        model->insertRow(model->rowCount(QModelIndex()));
        mapper->toLast();
        ui->status_comboBox->setCurrentIndex(3);
        ui->accessLevel_comboBox->setCurrentIndex(3);
    } else {
       mapper->setCurrentModelIndex(model->index(row,0));
    }

    m_lin = ui->lin_lineEdit->text(); //Маппер вложил нужное значение и мы можем его забрать как изначальное

    //Владелец уникален. Его нельзя выдать и нельзя убрать. Первый зарегистрированный станет владельцем
    if (ui->status_comboBox->currentIndex() != 0) {
        ui->status_comboBox->removeItem(0);
        ui->accessLevel_comboBox->removeItem(0);
    } else {
        ui->status_comboBox->setEnabled(false);
        ui->accessLevel_comboBox->setEnabled(false);
        ui->accessLevel_comboBox->setCurrentIndex(0);
        ui->deleteAccount_pushButton->hide();
    }

    if (m_accessLevel == "Пользователь" || m_accessLevel == "Модератор") {
        ui->lin_lineEdit->setEnabled(false);
        ui->date_lineEdit->setEnabled(false);
        ui->status_comboBox->setEnabled(false);
        ui->accessLevel_comboBox->hide();
        ui->accessLevel_label->hide();
        if (m_accessLevel == "Пользователь")
            ui->exitProfile_pushButton->setText("Выйти из аккаунта");
        else {
            ui->nickname_lineEdit->setEnabled(false); //Модератор ничего не может редактировать, только просматривать
            ui->changePassword_pushButton->setEnabled(false);
            ui->saveChange_pushButton->setEnabled(false); //Отключить кнопку сохранения (Можно сделать только это)
        }
        ui->deleteAccount_pushButton->hide();
    }
}

Profile::~Profile()
{
    delete ui;
}
void Profile::on_exitProfile_pushButton_clicked()
{
    this->close();
    if (m_accessLevel == "Пользователь")
        emit signal_exitAccount();
}

void Profile::on_saveChange_pushButton_clicked()
{
    QSqlQuery query;
    //Поиск записей в БД с тем же ЛИН, что вписан в ячейку профиля
    QString str = QString("SELECT COUNT(*) "
                          "FROM userlist "
                          "WHERE lin = '%1'")
                .arg(ui->lin_lineEdit->text());

    if(!query.exec(str))
        qDebug() << "Error: " << query.lastError() << " : " << query.lastQuery();
    query.next();

    int n;
    if (m_lin == ui->lin_lineEdit->text()) { //Если ЛИН не изменился
        n = query.value(0).toInt() - 1; //Снять одно значение повтора
    } else {
        n = query.value(0).toInt();
    }

    // Если ЛИН повторяется, то выдавать ошибку
    if(n != 0){
        QMessageBox::information(this, "Ошибка сохранения данных",
            "Нарушена уникальность ЛИН!");
        ui->lin_lineEdit->setText(m_lin); //Вернуть ЛИН к прошлому значению
    //В противном случае производится вставка новых данных в таблицу
    //и диалог завершается с передачей сигнала для обновления таблицы в главном окне
    } else {
       m_lin = ui->lin_lineEdit->text(); //Записать новое значение, с которым будет сравниваться

       mapper->submit();
       model->submitAll();
       emit signal_updateTable();
    }
}

void Profile::on_changePassword_pushButton_clicked()
{
    changePass *ui_changePass = new changePass(ui->lin_lineEdit->text());

    ui_changePass->setWindowTitle("Смена пароля");
    ui_changePass->exec();
}

void Profile::on_deleteAccount_pushButton_clicked()
{
    QSqlQuery query;
    QString str = QString("DELETE FROM userlist WHERE lin = '%1';")
                .arg(ui->lin_lineEdit->text());

    if(!query.exec(str))
        qDebug() << "Error: " << query.lastError() << " : " << query.lastQuery();

    emit signal_updateTable();
    this->close();
}
