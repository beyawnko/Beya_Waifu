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
// void MainWindow::on_pushButton_CheckUpdate_clicked() -> Definition is now a stub in mainwindow.cpp
/*
Automatic update check:
When the software starts, it runs in a separate thread to detect updates. If there is an update, a pop-up window will be displayed.
*/
// int MainWindow::CheckUpdate_Auto() -> This is a helper for auto-update logic, which is likely triggered by a timer or on startup.
// Since its callers would be part of MainWindow initialization or timers, and we are stubbing UI-triggered slots,
// this function's definition can also be removed if it's not directly called by other non-stubbed MainWindow methods.
// For now, let's assume it's part of an internal mechanism that might still be active or refactored later.
// KEEPING CheckUpdate_Auto() for now, as it's not a direct slot.

/*
Automatic update pop-up window
*/
// int MainWindow::CheckUpdate_NewUpdate(QString update_str, QString Change_log) -> Definition is now a stub in mainwindow.cpp
// Note: CheckUpdate_Auto calls Send_CheckUpdate_NewUpdate, which in turn calls the stubbed CheckUpdate_NewUpdate.
// This is acceptable as the stub will handle it.
int MainWindow::CheckUpdate_Auto()
{
    bool isGiteeBanned = ui->checkBox_BanGitee->isChecked();
    //============
    QString Latest_Ver="";
    QString Current_Ver=""; // This should be initialized with the actual current version of the application.
    // Current_Ver = APP_VERSION; // Example, if APP_VERSION is a defined macro or const
    QString Github_UpdateInfo_online = "";
    QString Gitee_UpdateInfo_online = "";
    // comboBox_UpdateChannel removed, default to Stable channel (index 0)
    QString UpdateType=tr("Stable"); // Defaulting to "Stable"

    Github_UpdateInfo_online = "https://raw.githubusercontent.com/beyawnko/Beya_Waifu/master/.github/Update_Info.ini";
    Gitee_UpdateInfo_online = "https://gitee.com/beyawnko/Beya_Waifu/raw/master/.github/Update_Info.ini";
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
        QSettings *configIniRead = new QSettings(Github_UpdateInfo_local, QSettings::IniFormat);
        if(configIniRead->value("/Latest_Version/Ver") == QVariant() || configIniRead->value("/Change_log/log") == QVariant())
        {
            emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
            QFile::remove(Github_UpdateInfo_local);
            QFile::remove(Gitee_UpdateInfo_local);
            return 0;
        }
        Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString().trimmed();
        QString Change_log = configIniRead->value("/Change_log/log").toString();
        if(Latest_Ver!=Current_Ver && !Latest_Ver.isEmpty()) // Check against current version
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
        if(QFile::exists(Gitee_UpdateInfo_local))
        {
            emit Send_TextBrowser_NewMessage(tr("Successfully downloaded update information from Gitee."));
            QSettings *configIniRead = new QSettings(Gitee_UpdateInfo_local, QSettings::IniFormat);
            if(configIniRead->value("/Latest_Version/Ver") == QVariant() || configIniRead->value("/Change_log/log") == QVariant())
            {
                emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
                QFile::remove(Github_UpdateInfo_local);
                QFile::remove(Gitee_UpdateInfo_local);
                return 0;
            }
            Latest_Ver = configIniRead->value("/Latest_Version/Ver").toString().trimmed();
            QString Change_log = configIniRead->value("/Change_log/log").toString();
            if(Latest_Ver!=Current_Ver && !Latest_Ver.isEmpty()) // Check against current version
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
    if(Latest_Ver.isEmpty()) // Check if Latest_Ver was ever successfully populated
    {
        emit Send_TextBrowser_NewMessage(tr("Unable to check for updates! Please check your network or check for updates manually."));
    }
    QFile::remove(Github_UpdateInfo_local);
    QFile::remove(Gitee_UpdateInfo_local);
    return 0;
}

