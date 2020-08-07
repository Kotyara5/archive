#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "auth_window.h"
#include "reg_window.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    connect(ui_Auth, SIGNAL(login_button_clicked()),
          this, SLOT(authorizeUser()));
    connect(ui_Auth,SIGNAL(register_button_clicked()),
          this,SLOT(registerWindowShow()));

    connect(ui_Reg,SIGNAL(register_button_clicked2()),
          this,SLOT(registerUser()));
     connect(ui_Reg,SIGNAL(signal_back_pushButton_clicked()),
          ui_Auth, SLOT(show()));

    if(!connectDB())
    {
      qDebug() << "Failed to connect DB";
      QMessageBox::information(this, "Ошибка подключения",
          "Не удалось подключиться к базе данных!");
    }

    QSqlQuery query;
    db_input = "CREATE TABLE userlist ( "
            "lin VARCHAR(21), "
            "name VARCHAR(21), "
            "password VARCHAR(21), "
            "date VARCHAR(12),"
            "status VARCHAR(18), "
            "accesslevel VARCHAR(18) "
            ");";
    if(!query.exec(db_input))
    {
          qDebug() << "Unable to create a table" << query.lastError();
    }

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    mw_db.removeDatabase("archive");
    delete ui;
    exit(0);
}

void MainWindow::authorizeUser()
{
    m_username = ui_Auth->getLogin();
    m_userpass = ui_Auth->getPass();

    QString username    = "";
    QString userpass    = "";

    db_input = QString(" SELECT * "
                       " FROM userlist "
                       " WHERE name = '%1'")
                       .arg(m_username);

    QSqlQuery query;
    QSqlRecord rec;

    if(!query.exec(db_input)) //Выполняет SQL запрос
    {
        qDebug() << "Unable to execute query - exiting" << query.lastError() << " : " << query.lastQuery();
    }
    rec = query.record();
    query.next(); //Извлечение данных (Указывает изначально перед полем, поэтому смещаем вперёд)

    username = query.value(rec.indexOf("name")).toString(); //По полученному индексу получаем переменную
    userpass = query.value(rec.indexOf("password")).toString();
    if(m_username != username || m_userpass != userpass)
    {
        qDebug() << "Password missmatch" << username << " " << userpass;
        QMessageBox::information(this, "Ошибка входа",
            "Неверно указано имя или пароль!");
    }
    else
    {
        ui_Auth->close();
        ui_Reg->close();

        m_accessLevel = query.value(rec.indexOf("accesslevel")).toString();

        //Тут сделать вывод всех зарегистрированных
        model = new QSqlTableModel(this);
        model->setTable("userlist");
        if(!model->select())
             qDebug() << model->lastError();

        //Поиск строки в таблице, которая соответствует авторизованному аккаунту
        int row_count = model->rowCount();
        QString search_lin = query.value(rec.indexOf("lin")).toString();
        for( int i = 0; i < row_count; ++i )
        {
            QModelIndex index = model->index( i, 0);
            if( model->data(index).toString() == search_lin )
            {
               m_row = index.row();
               break;
            }
        }
        if (m_accessLevel == "Пользователь") {
            Profile *ui_Profile = new Profile(m_row);
            connect(ui_Profile, SIGNAL(signal_updateTable()),
                      this, SLOT(slot_UpdateModels()));
            //Сигнал, который будет открыть окно авторизации при выходе из профиля пользователя
            connect(ui_Profile, SIGNAL(signal_exitAccount()),
                      this, SLOT(on_logoff_pushButton_clicked()));

            ui_Profile->setWindowTitle("Мой Профиль");
            ui_Profile->exec();
        } else {
            this->show();

            ui->tableView->setModel(model);
            //Разрешаем выделение строк
            ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
            //Устанавливаем режим выделения лишь одной строки в таблице
            ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
            if (m_accessLevel == "Модератор") {
                ui->newProfile_pushButton->setEnabled(false);
                ui->tableView->hideColumn(2); //Скрыть столбец с паролями
                ui->tableView->hideColumn(5); //Скрыть столбец с УД
            } else {
                ui->newProfile_pushButton->setEnabled(true);
                ui->tableView->showColumn(2); //Показать столбец с паролями
                ui->tableView->showColumn(5); //Показать столбец с УД
            }
            //Задать названия столбцов
            QString head[] = {"ЛИН", "Имя", "Пароль", "Дата", "Статус", "УД к Архиву"};
            for(int i = 0; i < model->columnCount(); ++i)
                model->setHeaderData(i, Qt::Horizontal, head[i]);
            //Устанавливаем размер колонок по содержимому
            ui->tableView->resizeColumnsToContents();
        }
    }
}

void MainWindow::registerWindowShow()
{
    ui_Auth->hide();
    ui_Reg->show();
}

void MainWindow::registerUser()
{
    if(ui_Reg->checkPass())
    {
        QSqlQuery query;
        QSqlRecord rec;
        QString lin;
        bool first = false;

        db_input = "SELECT COUNT(*) "
                   "FROM userlist;";
        if(!query.exec(db_input))
        {
            qDebug() << "Unable to get number " << query.lastError() << " : " << query.lastQuery();
            return;
        }
        else
        {
            query.next();
            rec = query.record();
            lin = rec.value(0).toString(); //Вставить в ЛИН количество записей пользователей
            if (lin == "0") first = true;
        }

        m_username = ui_Reg->getName();
        m_userpass = ui_Reg->getPass();

        //Модифицировать ЛИН добавлением номера года в начало
        QDate today = QDate::currentDate();
        QDate bday(2013, 06, 05);
        //Если месяц больше или такой же, но больше день
        if(today.month() > bday.month() ||
                (today.month() == bday.month() && today.day() >= bday.day())) {
            //lin.remove(0,3);
            lin = QString::number(today.year() - bday.year() + 1).rightJustified(3, '0')
                    + QString::number(lin.toInt() + 1);
        } else {
            //lin = QString::number(lin.toInt() + 1);
            lin = QString::number(today.year() - bday.year()).rightJustified(3, '0')
                    + QString::number(lin.toInt() + 1);
        }
        //Добавить нового пользователя
        QString str_t = "INSERT INTO userlist(lin, name, password, date, status, accesslevel)"
                        "VALUES('%1', '%2', %3, '%4', '%5', '%6');";
        if (!first)
            db_input = str_t.arg(lin)
                            .arg(m_username)
                            .arg(m_userpass)
                            .arg(QDate::currentDate().toString(Qt::ISODate))
                            .arg("Стажёр")
                            .arg("Пользователь");
        else
            db_input = str_t.arg(lin)
                            .arg(m_username)
                            .arg(m_userpass)
                            .arg(QDate::currentDate().toString(Qt::ISODate))
                            .arg("Владелец")
                            .arg("Владелец");

        if(!query.exec(db_input))
        {
            qDebug() << "Unable to insert data"  << query.lastError() << " : " << query.lastQuery();
        }
        else
        {
            ui_Reg->hide();
            ui_Auth->show();
        }
    }
    else
    {
        qDebug() << "Confirm password coorectly";
        QMessageBox::information(this, "Ошибка регистрации",
            "Пароли не совпадают!");
    }

}

void MainWindow::display()
{
    ui_Auth->show();
}

bool MainWindow::connectDB()
{
    //Локальная
    //mw_db = QSqlDatabase::addDatabase("QSQLITE");
    //mw_db.setDatabaseName("archive");

    //Сервер
    mw_db = QSqlDatabase::addDatabase("QODBC");
    mw_db.setDatabaseName("DRIVER={SQL Server};Server=///;Database=archive;");
    mw_db.setUserName("///");
    mw_db.setPassword("///");

    if(!mw_db.open())
    {
        qDebug() << "Cannot open database: " << mw_db.lastError();
        return false;
    }
    return true;
}

void MainWindow::slot_UpdateModels()
{
    if(!model->select())
        qDebug() << model->lastError();
}

void MainWindow::on_logoff_pushButton_clicked()
{
    this->hide(); //Скрыть главное окно при выходе из аккаунта
    ui_Auth->show();
}

void MainWindow::on_myProfile_pushButton_clicked()
{
    Profile *ui_Profile;
    if (m_accessLevel == "Модератор") //Модератор открывает свой профиль, как пользователь
        ui_Profile = new Profile(m_row);
    else
        ui_Profile = new Profile(m_row, m_accessLevel);
    ui_Profile->setWindowTitle("Мой Профиль");
    connect(ui_Profile, SIGNAL(signal_updateTable()),
              this, SLOT(slot_UpdateModels()));
    ui_Profile->exec();
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    Profile *ui_Profile;

    if (m_row != index.row()) {
        ui_Profile = new Profile(index.row(), m_accessLevel);
        ui_Profile->setWindowTitle("Профиль");
    } else { //Если номера строк совпали и это профиль активного аккаунта, то открыть как его профиль
        if (m_accessLevel == "Модератор") //Модератор открывает свой профиль, как пользователь
            ui_Profile = new Profile(m_row);
        else
            ui_Profile = new Profile(m_row, m_accessLevel);
        ui_Profile->setWindowTitle("Мой Профиль");
    }

    connect(ui_Profile, SIGNAL(signal_updateTable()),
              this, SLOT(slot_UpdateModels()));
    ui_Profile->exec();
}

void MainWindow::on_newProfile_pushButton_clicked()
{
    Profile *ui_Profile = new Profile(-1, m_accessLevel);
    ui_Profile->setWindowTitle("Профиль");
    connect(ui_Profile, SIGNAL(signal_updateTable()),
              this, SLOT(slot_UpdateModels()));
    ui_Profile->exec();
}
