#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.display(); //Новый метод, чтобы первом делом показывать окно авторизации
    return a.exec();
}
