#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <windows.h>
#include <shellapi.h>
#include <QThread>
#include <QCloseEvent>
#include <QApplication>
#include <QtConcurrent>
#include <QEventLoop>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void RUN_Concurrent();
    void closeEvent( QCloseEvent * event );
    QString Current_Path = qApp->applicationDirPath();// current application path
    bool file_isDirWritable(QString DirPath);
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void Send_RUN();
    void Send_Duplicate();
public slots:
    void RUN_SLOT();
    void Duplicate_SLOT();

private:
    Ui::MainWindow *ui;
    bool runProcess(QProcess *process, const QString &cmd,
                    QByteArray *stdOut = nullptr,
                    QByteArray *stdErr = nullptr);
};
#endif // MAINWINDOW_H
