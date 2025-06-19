#include "mainwindow.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc,argv);
    QStyle *style = QStyleFactory::create("macos");
    if (!style)
        style = QStyleFactory::create("Fusion");
    if (style)
        a.setStyle(style);
    MainWindow *w = new MainWindow(0);
    w->show();
    return a.exec();
}
