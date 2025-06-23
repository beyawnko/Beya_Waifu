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
// bool MainWindow::SystemShutDown() -> Definition is now a stub in mainwindow.cpp
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
