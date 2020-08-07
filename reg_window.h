#ifndef REG_WINDOW_H
#define REG_WINDOW_H

#include <QDialog>

namespace Ui {
class reg_window;
}

class reg_window : public QDialog
{
    Q_OBJECT

public:
    explicit reg_window(QWidget *parent = nullptr);
    ~reg_window();

    QString getName();
    QString getPass();
    bool checkPass();

signals:
    void register_button_clicked2();

    void signal_back_pushButton_clicked();

private slots:
    void on_namelineEdit_3_textEdited(const QString &arg1);
    void on_passlineEdit_textEdited(const QString &arg1);
    void on_conlineEdit_2_textEdited(const QString &arg1);
    void on_regPushButton_clicked();
    void on_back_pushButton_clicked();

private:
    Ui::reg_window *ui;

    QString m_userName;
    QString m_userPass;
    QString m_confirmation;
};

#endif // REG_WINDOW_H
