#include "reg_window.h"
#include "ui_reg_window.h"

reg_window::reg_window(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::reg_window)
{
    ui->setupUi(this);
}

reg_window::~reg_window()
{
    delete ui;
}

void reg_window::on_namelineEdit_3_textEdited(const QString &arg1)
{
    reg_window::m_userName = arg1;
}

void reg_window::on_passlineEdit_textEdited(const QString &arg1)
{
    reg_window::m_userPass = arg1;
}

void reg_window::on_conlineEdit_2_textEdited(const QString &arg1)
{
    reg_window::m_confirmation = arg1;
}

void reg_window::on_regPushButton_clicked()
{
    emit register_button_clicked2();
}

QString reg_window::getName()
{
    return m_userName;
}

QString reg_window::getPass()
{
    return m_userPass;
}

bool reg_window::checkPass()
{
    return (m_confirmation == m_userPass);
}

void reg_window::on_back_pushButton_clicked()
{
    this->close();
    emit signal_back_pushButton_clicked();
}
