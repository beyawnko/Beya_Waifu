#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    a.setStyle(QStyleFactory::create("macos"));
    MainWindow *w = new MainWindow(0);
    w->show();
    return a.exec();
}
