/*
    Copyright (C) 2025  beyawnko

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

    My Github homepage: https://github.com/beyawnko
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
Download file from network to local
*/
bool MainWindow::DownloadTo(QString OnlineLink,QString LocalPath)
{
    QFile::remove(LocalPath);
    QString user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.96 Safari/537.36";
    QString program = Current_Path+"/wget_waifu2xEX.exe";
    QString command = "\""+program+"\" --user-agent=\""+user_agent+"\" -O \""+LocalPath+"\" \""+OnlineLink+"\" --timeout=15";
    QProcess Downlad2;
    runProcess(&Downlad2, command);
    if(QProcess_stop)
        return false;
    QString Downlad2_OutPutStr = Downlad2.readAllStandardError().toLower();
    QFileInfo localInfo(LocalPath);
    if(localInfo.size()<1 || Downlad2_OutPutStr.contains("saved")==false)QFile::remove(LocalPath);
    return QFile::exists(LocalPath);
}
/*
Check the connection to GitHub to inform the user if Gitee can be disabled
*/
void MainWindow::on_checkBox_BanGitee_clicked()
{
    if(ui->checkBox_BanGitee->isChecked() && isConnectivityTest_RawGithubusercontentCom_Running==false)
    {
        QtConcurrent::run(this, &MainWindow::ConnectivityTest_RawGithubusercontentCom);//Run network test in the background to determine if raw.githubusercontent.com can be connected
    }
}
/*
Github Connectivity Test
*/
void MainWindow::ConnectivityTest_RawGithubusercontentCom()
{
    QMutex_ConnectivityTest_RawGithubusercontentCom.lock();
    isConnectivityTest_RawGithubusercontentCom_Running=true;
    //===
    QString OnlineAddress="https://raw.githubusercontent.com/beyawnko/Beya_Waifu/master/.github/ConnectivityTest_githubusercontent.txt";
    QString LocalAddress=Current_Path+"/ConnectivityTest_Waifu2xEX.txt";
    QFile::remove(LocalAddress);
    //===
    emit Send_TextBrowser_NewMessage(tr("Testing if your PC can connect to raw.githubusercontent.com..."));
    if(DownloadTo(OnlineAddress,LocalAddress)==true)
    {
        emit Send_TextBrowser_NewMessage(tr("Detection complete, your PC can connect to raw.githubusercontent.com."));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Detection complete, your PC cannot connect to raw.githubusercontent.com."));
        emit Send_Unable2Connect_RawGithubusercontentCom();
    }
    QFile::remove(LocalAddress);
    //===
    isConnectivityTest_RawGithubusercontentCom_Running=false;
    QMutex_ConnectivityTest_RawGithubusercontentCom.unlock();
}
/*
Github Connectivity Test - Prompt unable to connect
*/
void MainWindow::Unable2Connect_RawGithubusercontentCom()
{
    QMessageBox *MSG_2 = new QMessageBox();
    MSG_2->setWindowTitle(tr("Notification"));
    MSG_2->setText(tr("Detected that you are currently unable to connect to raw.githubusercontent.com, so enabling [Ban Gitee] won't allow the software to automatically check for updates. It is recommended that you disable [Ban Gitee]."));
    MSG_2->setIcon(QMessageBox::Warning);
    MSG_2->setModal(true);
    MSG_2->show();
}
