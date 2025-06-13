#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);// enable high DPI scaling
    QApplication a(argc,argv);
    MainWindow *w = new MainWindow(0);
    w->show();
    return a.exec();
}
