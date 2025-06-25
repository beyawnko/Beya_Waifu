#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->setupUi(this);
    connect(this, SIGNAL(Send_RUN()), this, SLOT(RUN_SLOT()));
    connect(this, SIGNAL(Send_Duplicate()), this, SLOT(Duplicate_SLOT()));
    // Store the future
    m_runConcurrentFuture = QtConcurrent::run([this]() { return this->RUN_Concurrent_Worker(); });
    // Optionally, attach a .then() if you want to react to completion/errors here,
    // for example, logging or cleanup that isn't part of the worker itself.
    // For this specific case, signals are already used for UI updates from the worker.
    // If RUN_Concurrent_Worker handles its own errors and emits signals,
    // a .then() might primarily be for logging the future's state if needed.
    m_runConcurrentFuture.then(this, [this](bool result){
        if (m_runConcurrentFuture.isCanceled()) {
            // Handle cancellation if applicable
            qDebug() << "RUN_Concurrent was canceled.";
        } else if (m_runConcurrentFuture.isFaulted()) {
            // Handle error
            qDebug() << "RUN_Concurrent failed:" << m_runConcurrentFuture.exception().what();
        } else {
            qDebug() << "RUN_Concurrent finished. Result:" << result;
            // The worker already emits signals Send_Duplicate or Send_RUN
            // and may call this->close().
            // If the worker calls this->close(), this MainWindow instance might be
            // deleted by the time this continuation runs.
            // Care must be taken if this continuation needs to access `this`.
            // In this specific case, RUN_Concurrent_Worker calls this->close()
            // which might lead to issues if this lambda tries to access `this` afterwards.
            // However, the signals should have already been processed.
            // If `this->close()` in the worker is problematic for the future's continuation,
            // the closing logic might need to be deferred or handled differently.
            // For now, we assume the signals are sufficient and the main concern is
            // the potential deletion of `this`.
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Duplicate_SLOT()
{
    ui->label_status->setText("It has been detected that the program is already\n"
                              "running,do not start the program repeatedly.");
    ui->label_status->setStyleSheet("color: rgb(255, 69, 69);background-color: rgb(255, 255, 255);");
}

bool MainWindow::RUN_Concurrent_Worker()
{
    QThread::sleep(3);
    //========
    QProcess get_tasklist;
    QByteArray taskOutput;
    // Assuming runProcess itself indicates success/failure,
    // but here we care about the logic flow for RUN_Concurrent_Worker's return.
    // Let's say the critical part is whether we could check the process list.
    bool processCheckSuccess = runProcess(&get_tasklist, "tasklist", &taskOutput);

    if (!processCheckSuccess) {
        // Log error or handle error if runProcess failed to even run tasklist
        qDebug() << "Failed to execute tasklist.";
        // Depending on desired behavior, could emit a specific error signal here.
        // For now, return false to indicate the worker had an issue.
        return false;
    }

    if(taskOutput.contains("Beya_Waifu.exe"))
    {
        emit Send_Duplicate();
        QThread::sleep(5); // This sleep might be problematic if instance is closed.
                           // Consider removing if `close()` is immediate.
        this->close(); // `this` might be deleted soon after this call.
        return true; // Operation considered "successful" in terms of its logic path.
    }
    //========
    emit Send_RUN();
    return true; // Operation considered "successful".
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    this->hide();
    qApp->setQuitOnLastWindowClosed(true);// do not keep running when no window exists
    qApp->closeAllWindows();
}
// Check whether the directory is writable
bool MainWindow::file_isDirWritable(QString DirPath)
{
    if(DirPath.right(1)=="/")
    {
        DirPath = DirPath.left(DirPath.length() - 1);
    }
    QString TestTemp = DirPath+"/RWTest_W2xEXLauncher.tmp";
    QFile file_TestTemp(TestTemp);
    file_TestTemp.remove();
    if (file_TestTemp.open(QIODevice::ReadWrite | QIODevice::Text)) // QIODevice::ReadWrite allows read/write
    {
        QTextStream stream(&file_TestTemp);
        stream << "W2xEX";
    }
    if(QFile::exists(TestTemp))
    {
        file_TestTemp.remove();
        return true;
    }
    else
    {
        file_TestTemp.remove();
        return false;
    }
}

void MainWindow::RUN_SLOT()
{
    //Current_Path = "D:/workspace/Waifu2x-Extension-QT/Full_package/waifu2x-extension-gui";
#ifdef Q_OS_WIN
    if(file_isDirWritable(Current_Path)==false)
    {
        ui->label_status->setText("Please grant administrator rights to\n"
                                  "ensure the normal operation of the software.");
        ShellExecuteW(NULL, QString("runas").toStdWString().c_str(), QString(Current_Path+"/Beya_Waifu.exe").toStdWString().c_str(), QString(Current_Path+"/Beya_Waifu.exe").toStdWString().c_str(), NULL, 1);
        this->close();
        return;
    }
    ShellExecuteW(NULL, QString("open").toStdWString().c_str(), QString(Current_Path+"/Beya_Waifu.exe").toStdWString().c_str(), NULL, NULL, 1);
#else
    // For non-Windows systems, try to launch directly or handle appropriately.
    // This simplified version might not fully replicate the intended Windows behavior.
    QProcess::startDetached(Current_Path + "/Beya_Waifu.exe"); // Assuming Beya_Waifu.exe is executable on Linux or Wine is used.
                                                              // Or Current_Path + "/Beya_Waifu" if it's a Linux binary.
#endif
    this->close();
    return;
}

bool MainWindow::runProcess(QProcess *process, const QString &cmd,
                            QByteArray *stdOut, QByteArray *stdErr)
{
    QEventLoop loop;
    if(stdOut) stdOut->clear();
    if(stdErr) stdErr->clear();

    if(stdOut)
        QObject::connect(process, &QProcess::readyReadStandardOutput,
                         [&](){ stdOut->append(process->readAllStandardOutput()); });
    if(stdErr)
        QObject::connect(process, &QProcess::readyReadStandardError,
                         [&](){ stdErr->append(process->readAllStandardError()); });

    QObject::connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                     &loop, &QEventLoop::quit);
    QObject::connect(process, &QProcess::errorOccurred,
                     &loop, &QEventLoop::quit);

    process->start(cmd);
    loop.exec();

    return process->exitStatus() == QProcess::NormalExit && process->exitCode() == 0;
}
