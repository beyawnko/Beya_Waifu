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
online update for QR Code at donate tab
*/
int MainWindow::Donate_DownloadOnlineQRCode()
{
    bool isGiteeBanned = ui->checkBox_BanGitee->isChecked();
    //============================
    //     Update supporter list
    //============================
    QString Github_TopSupporterList_online = "https://raw.githubusercontent.com/beyawnko/Beya_Waifu/master/.github/TopSupportersList_W2xEX.ini";
    QString Gitee_TopSupporterList_online = "https://gitee.com/beyawnko/Beya_Waifu/raw/master/.github/TopSupportersList_W2xEX.ini";
    QString TopSupporterList_local = Current_Path+"/TopSupportersList_W2xEX.ini";
    QFile::remove(TopSupporterList_local);
    //===
    emit Send_TextBrowser_NewMessage(tr("Starting to update Top Supporters List."));
    DownloadTo(Github_TopSupporterList_online,TopSupporterList_local);
    if(QFile::exists(TopSupporterList_local)==false && isGiteeBanned==false)
    {
        DownloadTo(Gitee_TopSupporterList_online,TopSupporterList_local);
    }
    if(QFile::exists(TopSupporterList_local)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to update Top Supporters List."));
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully updated Top Supporters List."));
    }
    //============================
    //     Update QR code image
    //============================
    QString Github_OnlineQRCode_online = "https://raw.githubusercontent.com/beyawnko/Beya_Waifu/master/.github/Online_Donate_QRCode.jpg";
    QString Gitee_OnlineQRCode_online = "https://gitee.com/beyawnko/Beya_Waifu/raw/master/.github/Online_Donate_QRCode.jpg";
    //=
    QString Github_OnlineQRCode_local = Current_Path+"/Online_Donate_QRCode_Github.jpg";
    QString Gitee_OnlineQRCode_local = Current_Path+"/Online_Donate_QRCode_Gitee.jpg";
    //==================== Download files from Github ========================
    emit Send_TextBrowser_NewMessage(tr("Starting to download QR Code image(for [Donate] tab) from Github."));
    DownloadTo(Github_OnlineQRCode_online,Github_OnlineQRCode_local);
    //========= Check if github files were downloaded successfully =================
    QFileInfo Github_OnlineQRCode_QFileInfo(Github_OnlineQRCode_local);
    if(QFile::exists(Github_OnlineQRCode_local)&&(Github_OnlineQRCode_QFileInfo.size()>100000))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully downloaded QR Code image from Github."));
        emit Send_Donate_ReplaceQRCode(Github_OnlineQRCode_local);
        return 1;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to download QR Code image from Github."));
    }
    //==================== Download files from Gitee (Code Cloud) ========================
    if(isGiteeBanned==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Starting to download QR Code image(for [Donate] tab) from Gitee."));
        DownloadTo(Gitee_OnlineQRCode_online,Gitee_OnlineQRCode_local);
        //========= Check if gitee files were downloaded successfully =================
        QFileInfo Gitee_OnlineQRCode_QFileInfo(Gitee_OnlineQRCode_local);
        if(QFile::exists(Gitee_OnlineQRCode_local)&&(Gitee_OnlineQRCode_QFileInfo.size()>100000))
        {
            emit Send_TextBrowser_NewMessage(tr("Successfully downloaded QR Code image from Gitee."));
            emit Send_Donate_ReplaceQRCode(Gitee_OnlineQRCode_local);
            return 1;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to download QR Code image from Gitee."));
        }
    }
    emit Send_Donate_ReplaceQRCode("");//Download failed, jump directly
    return 0;
}

// Definitions for functions below are now stubs in mainwindow.cpp
// void MainWindow::Donate_ReplaceQRCode(QString QRCodePath)
// void MainWindow::FinishedProcessing_DN()
// void MainWindow::on_pushButton_Patreon_clicked()
// void MainWindow::on_pushButton_SupportersList_clicked()
