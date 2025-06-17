/*
    Copyright (C) 2021  Aaron Feng

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

    My Github homepage: https://github.com/AaronFeng753
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>

//====== Auto shutdown =========================================================
/*
60 second countdown
*/
int MainWindow::SystemShutDown_Countdown()
{
    Delay_sec_sleep(60);
    switch(ui->comboBox_FinishAction->currentIndex())
    {
        case 1:
        case 4:
            {
                emit Send_SystemShutDown();
                break;
            }
        case 2:
            {
                QProcess::execute(Current_Path+"/nircmd-x64/nircmd.exe standby");
                break;
            }
        case 3:
            {
                QProcess::execute(Current_Path+"/nircmd-x64/nircmd.exe hibernate");
                break;
            }
    }
    return 0;
}
/*
Shutdown
Save the list, create a shutdown marker, then power off.
*/
bool MainWindow::SystemShutDown()
{
    Table_Save_Current_Table_Filelist(Current_Path+"/FilesList_W2xEX/FilesList_W2xEX_AutoSave.ini");
    //================
    QString AutoShutDown = Current_Path+"/AutoShutDown_W2xEX";
    QFile file(AutoShutDown);
    file.remove();
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) // QIODevice::ReadWrite allows read/write
    {
        QTextStream stream(&file);
        stream << "Don't delete this file!!";
    }
    //================
#ifdef Q_OS_WIN
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    // Get process token
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    // Get shutdown privilege LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,    &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    // Enable shutdown privilege for this process
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) return false;
    //===
    switch(ui->comboBox_FinishAction->currentIndex())
    {
        case 1:// Shutdown
            {
                // Force shut down the computer
                if ( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
                    return false;
                return true;
            }
        case 4:// Reboot
            {
                // Force reboot the computer
                if ( !ExitWindowsEx(EWX_REBOOT | EWX_FORCE, 0))
                    return false;
                return true;
            }
    }
    return false;
#else
    // System shutdown/reboot is not supported on non-Windows platforms with this method.
    if (ui->comboBox_FinishAction->currentIndex() == 1 || ui->comboBox_FinishAction->currentIndex() == 4)
    {
        qWarning("System shutdown/reboot requested on a non-Windows platform. Action skipped.");
    }
    return false; // Or indicate not supported
#endif
}
/*
Check whether the previous run triggered auto shutdown
*
*/
int MainWindow::SystemShutDown_isAutoShutDown()
{
    QString AutoShutDown = Current_Path+"/AutoShutDown_W2xEX";
    QString Table_FileList_ini = Current_Path+"/FilesList_W2xEX/FilesList_W2xEX_AutoSave.ini";
    if(QFile::exists(AutoShutDown)&&QFile::exists(Table_FileList_ini))
    {
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Notification"));
        MSG->setModal(true);
        // It might be better to show the message only if an action was taken or expected.
        // For now, keeping original logic but ensuring return.
    }
    QFile::remove(AutoShutDown); // Delete previously generated auto shutdown marker
    return 0; // Added to satisfy return type
}

void MainWindow::AutoFinishAction_Message()
{
    QString ActionName = ui->comboBox_FinishAction->currentText();
    //===
    QMessageBox *AutoShutDown = new QMessageBox();
    AutoShutDown->setWindowTitle(tr("Warning!"));
    AutoShutDown->setText(tr("The computer will automatically ")+ActionName+tr(" in 60 seconds!"));
    AutoShutDown->setIcon(QMessageBox::Warning);
    AutoShutDown->setModal(false);
    AutoShutDown->show();
    //===
    emit Send_TextBrowser_NewMessage(tr("The computer will automatically ")+ActionName+tr(" in 60 seconds!"));
    emit Send_SystemTray_NewMessage(tr("The computer will automatically ")+ActionName+tr(" in 60 seconds!"));
}
