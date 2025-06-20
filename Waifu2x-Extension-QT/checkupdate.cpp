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
Manually check for updates: directly open the release page
*/
void MainWindow::on_pushButton_CheckUpdate_clicked()
{
    // comboBox_language removed, default to GitHub link
    QDesktopServices::openUrl(QUrl("https://github.com/beyawnko/Beya_Waifu/releases"));
}
/*
Automatic update check:
When the software starts, it runs in a separate thread to detect updates. If there is an update, a pop-up window will be displayed.
*/
int MainWindow::CheckUpdate_Auto()
{
    bool isGiteeBanned = ui->checkBox_BanGitee->isChecked();
    //============
    QString Latest_Ver="";
    QString Current_Ver="";
    QString Github_UpdateInfo_online = "";
    QString Gitee_UpdateInfo_online = "";
    // comboBox_UpdateChannel removed, default to Stable channel (index 0)
    QString UpdateType=tr("Stable"); // Defaulting to "Stable"
    // switch(ui->comboBox_UpdateChannel->currentIndex()) // comboBox_UpdateChannel removed
    // {
    //     case 0:
            Github_UpdateInfo_online = "https://raw.githubusercontent.com/beyawnko/Beya_Waifu/master/.github/Update_Info.ini";
            Gitee_UpdateInfo_online = "https://gitee.com/beyawnko/Beya_Waifu/raw/master/.github/Update_Info.ini";
    //        break; // Part of removed switch
    //    case 1: // Beta channel - logic removed, defaults to stable
    //        Current_Ver=LastBetaVer;
    //        Github_UpdateInfo_online = "https://raw.githubusercontent.com/beyawnko/Beya_Waifu/master/.github/Update_Info_Beta.ini";
    //        Gitee_UpdateInfo_online = "https://gitee.com/beyawnko/Beya_Waifu/raw/master/.github/Update_Info_Beta.ini";
    //        break;
    //    default:
    //        break;
    //}
    //============================
    QString Github_UpdateInfo_local = Current_Path+"/Update_Info_Github.ini";
    QString Gitee_UpdateInfo_local = Current_Path+"/Update_Info_Gitee.ini";
    //============= Download update information from Github ==============
    emit Send_TextBrowser_NewMessage(tr("Starting to download update information(for auto-check update) from Github."));
    DownloadTo(Github_UpdateInfo_online,Github_UpdateInfo_local);
    //========= Check if the github file was downloaded successfully =================
    if(QFile::exists(Github_UpdateInfo_local))
    {
        emit Send_TextBrowser_NewMessage(tr("Successfully downloaded update information from Github."));
        //==
        QSettings *configIniRead = new QSettings(Github_UpdateInfo_local, QSettings::IniFormat);
        //configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8")); // Removed for Qt6
        //=====
        if(configIniRead->value("/Latest_Version/Ver") == QVariant() || configIniRead->value("/Change_log/log") == QVariant())
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
            QFile::remove(Github_UpdateInfo_local);
            QFile::remove(Gitee_UpdateInfo_local);
            return 0;
        }
        //=====
        Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString();
        QString Change_log = configIniRead->value("/Change_log/log").toString();
        Latest_Ver = Latest_Ver.trimmed();
        if(Latest_Ver!=Current_Ver&&Latest_Ver!="")
        {
            emit Send_CheckUpdate_NewUpdate(Latest_Ver, Change_log);
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("No update found, you are using the latest ")+UpdateType+tr(" version."));
        }
        QFile::remove(Github_UpdateInfo_local);
        QFile::remove(Gitee_UpdateInfo_local);
        return 0;
    }
    else
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to download update information from Github."));
    }
    //============= Download update information from Gitee (Code Cloud) ==============
    if(isGiteeBanned==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Starting to download update information(for auto-check update) from Gitee."));
        DownloadTo(Gitee_UpdateInfo_online,Gitee_UpdateInfo_local);
        //========= Check if the gitee file was downloaded successfully =================
        if(QFile::exists(Gitee_UpdateInfo_local))
        {
            emit Send_TextBrowser_NewMessage(tr("Successfully downloaded update information from Gitee."));
            //==
            QSettings *configIniRead = new QSettings(Gitee_UpdateInfo_local, QSettings::IniFormat);
            //configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8")); // Removed for Qt6
            //=====
            if(configIniRead->value("/Latest_Version/Ver") == QVariant() || configIniRead->value("/Change_log/log") == QVariant())
            {
                emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
                QFile::remove(Github_UpdateInfo_local);
                QFile::remove(Gitee_UpdateInfo_local);
                return 0;
            }
            //=====
            Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString();
            QString Change_log = configIniRead->value("/Change_log/log").toString();
            Latest_Ver = Latest_Ver.trimmed();
            if(Latest_Ver!=Current_Ver&&Latest_Ver!="")
            {
                emit Send_CheckUpdate_NewUpdate(Latest_Ver, Change_log);
            }
            else
            {
                emit Send_TextBrowser_NewMessage(tr("No update found, you are using the latest ")+UpdateType+tr(" version."));
            }
            QFile::remove(Github_UpdateInfo_local);
            QFile::remove(Gitee_UpdateInfo_local);
            return 0;
        }
        else
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to download update information from Gitee."));
        }
    }
    if(Latest_Ver=="")
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
    }
    QFile::remove(Github_UpdateInfo_local);
    QFile::remove(Gitee_UpdateInfo_local);
    return 0;
}
/*
Automatic update pop-up window
*/
int MainWindow::CheckUpdate_NewUpdate(QString update_str, QString Change_log)
{
    // comboBox_UpdateChannel removed, default to Stable
    QString UpdateType= tr("Stable");
    //======
    if(ui->checkBox_UpdatePopup->isChecked())
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("New ")+UpdateType+tr(" update available!")), QString(tr("New version: %1\n\nBrief change log:\n%2\n\nDo you wanna update now???")).arg(update_str).arg(Change_log));
        Msg.setIcon(QMessageBox::Information);
        // comboBox_language removed, default to English behavior (single "YES" button for GitHub)
        // if(ui->comboBox_language->currentIndex()==1) // Assuming index 1 is Chinese
        // {
        //     QAbstractButton *pYesBtn_Github = Msg.addButton(tr("Download from Github"), QMessageBox::YesRole); // Go to Github to download
        //     QAbstractButton *pYesBtn_Gitee = Msg.addButton(tr("Download from Gitee"), QMessageBox::YesRole); // Go to Gitee to download
        //     Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
        //     Msg.exec();
        //     if (Msg.clickedButton() == pYesBtn_Github)QDesktopServices::openUrl(QUrl("https://github.com/beyawnko/Beya_Waifu/releases/"+update_str.trimmed()));
        //     if (Msg.clickedButton() == pYesBtn_Gitee)QDesktopServices::openUrl(QUrl("https://gitee.com/beyawnko/Beya_Waifu/releases/"+update_str.trimmed()));
        //     return 0;
        // }
        // else
        // {
            QAbstractButton *pYesBtn = Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
            Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
            Msg.exec();
            if (Msg.clickedButton() == pYesBtn)QDesktopServices::openUrl(QUrl("https://github.com/beyawnko/Beya_Waifu/releases/"+update_str.trimmed()));
            return 0;
        // }
    }
    else
    {
        QString update_msg_str = QString(tr("New ")+UpdateType+tr(" update: %1 is available! Click [Check update] button to download the latest version!")).arg(update_str);
        emit Send_SystemTray_NewMessage(update_msg_str);
        emit Send_TextBrowser_NewMessage(update_msg_str);
    }
    return 0;
}

