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
Implementation of the tray icon follows guidance from the Jianshu article
"Qt5 application system tray" by XiaoQ_wang:
https://www.jianshu.com/p/a000044f1f4a

The tutorial explains how to use QSystemTrayIcon to show the application's
icon in the system tray, handle single and double click events to hide or
restore the main window, and provide a context menu.
*/

/*
Initialize the tray icon
*/
void MainWindow::Init_SystemTrayIcon()
{
    // Initialize the icon
    systemTray->setIcon(*MainIcon_QIcon);
    systemTray->setToolTip(tr("Beya_Waifu\nRight-click to show the menu."));
    // Initialize click actions
    connect(systemTray,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)),Qt::UniqueConnection);
    // Initialize menu actions
    minimumAct_SystemTrayIcon->setText(tr("Hide"));
    minimumAct_SystemTrayIcon->setToolTip(tr("Use the middle mouse button to click the\ntaskbar icon to quickly hide the window."));
    minimumAct_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/Minimize.png"));
    //===
    restoreAct_SystemTrayIcon->setText(tr("Show"));
    restoreAct_SystemTrayIcon->setToolTip(tr("Use the left mouse button to click the taskbar icon to quickly\ndisplay the window, double-click to maximize the window."));
    restoreAct_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/ShowWindow.png"));
    //===
    quitAct_SystemTrayIcon->setText(tr("Exit"));
    quitAct_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/Exit.png"));
    //===
    BackgroundModeAct_SystemTrayIcon->setText(tr("Background mode"));
    BackgroundModeAct_SystemTrayIcon->setToolTip(tr("Set the number of threads to \"1\" to reduce background resource usage."));
    BackgroundModeAct_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/BackgroudMode.png"));
    //===
    SendFeedback_SystemTrayIcon->setText(tr("Send feedback"));
    SendFeedback_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/SendFeedback.png"));
    //===
    BecomePatron_SystemTrayIcon->setText(tr("Get Premium version"));
    BecomePatron_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/patreon_sysTray.png"));
    //===
    TopSupportersList_SystemTrayIcon->setText(tr("Top Supporters"));
    TopSupportersList_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/donateTabIcon.png"));
    //===
    About_SystemTrayIcon->setText(tr("About"));
    About_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/About.png"));
    //===
    Donate_SystemTrayIcon->setText(tr("Donate"));
    Donate_SystemTrayIcon->setToolTip(tr("Donate to support this project."));
    Donate_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/donateTabIcon_1.png"));
    //===
    Pause_SystemTrayIcon->setText(tr("Pause"));
    Pause_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/Pause_SysTray.png"));
    //===
    Start_SystemTrayIcon->setText(tr("Start"));
    Start_SystemTrayIcon->setIcon(QIcon(":/new/prefix1/icon/Start_SysTray.png"));
    //===
    connect(minimumAct_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(SystemTray_hide_self()),Qt::UniqueConnection);
    connect(restoreAct_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(SystemTray_showNormal_self()),Qt::UniqueConnection);
    connect(quitAct_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(close()),Qt::UniqueConnection);
    connect(BackgroundModeAct_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(EnableBackgroundMode_SystemTray()),Qt::UniqueConnection);
    connect(SendFeedback_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(on_pushButton_Report_clicked()),Qt::UniqueConnection);
    connect(BecomePatron_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(on_pushButton_Patreon_clicked()),Qt::UniqueConnection);
    connect(About_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(on_pushButton_about_clicked()),Qt::UniqueConnection);
    connect(Donate_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(SystemTray_showDonate()),Qt::UniqueConnection);
    connect(Pause_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(on_pushButton_Stop_clicked()),Qt::UniqueConnection);
    connect(Start_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(on_pushButton_Start_clicked()),Qt::UniqueConnection);
    connect(TopSupportersList_SystemTrayIcon, SIGNAL(triggered()), this, SLOT(on_pushButton_SupportersList_clicked()),Qt::UniqueConnection);
    // Initialize menu items
    pContextMenu->setToolTipsVisible(1);
    pContextMenu->setToolTipDuration(-1);
    pContextMenu->addAction(minimumAct_SystemTrayIcon);
    pContextMenu->addAction(restoreAct_SystemTrayIcon);
    pContextMenu->addSeparator();
    pContextMenu->addAction(Start_SystemTrayIcon);
    pContextMenu->addAction(Pause_SystemTrayIcon);
    pContextMenu->addSeparator();
    pContextMenu->addAction(BackgroundModeAct_SystemTrayIcon);
    pContextMenu->addSeparator();
    pContextMenu->addAction(BecomePatron_SystemTrayIcon);
    pContextMenu->addAction(TopSupportersList_SystemTrayIcon);
    pContextMenu->addSeparator();
    pContextMenu->addAction(Donate_SystemTrayIcon);
    pContextMenu->addAction(SendFeedback_SystemTrayIcon);
    pContextMenu->addAction(About_SystemTrayIcon);
    pContextMenu->addSeparator();
    pContextMenu->addAction(quitAct_SystemTrayIcon);
    systemTray->setContextMenu(pContextMenu);
    // Show the tray icon
    systemTray->show();
}
/*
Tray icon click behavior
*/
void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            // Single click restores the window (if maximized it returns to normal)
            this->showNormal();
            this->activateWindow();
            this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            break;
        case QSystemTrayIcon::DoubleClick:
            // Double click maximizes the window
            this->showMaximized();
            this->activateWindow();
            this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
            break;
        case QSystemTrayIcon::MiddleClick:
            // Middle click hides the window
            this->hide();
            break;
        default:
            break;
    }
}
/*
Enable background mode
(set all thread counts to 1 to reduce resource usage)
*/
void MainWindow::EnableBackgroundMode_SystemTray()
{
    ui->spinBox_ThreadNum_image->setValue(1);
    ui->spinBox_ThreadNum_gif_internal->setValue(1);
    ui->spinBox_ThreadNum_video_internal->setValue(1);
    ui->spinBox_NumOfThreads_VFI->setValue(1);
    pContextMenu->hide();
}
/*
Tray notification
*/
void MainWindow::SystemTray_NewMessage(QString message)
{
    systemTray->showMessage(tr("Notification"),message,*MainIcon_QIcon,5000);
}

void MainWindow::SystemTray_hide_self()
{
    this->hide();
    pContextMenu->hide();
}

void MainWindow::SystemTray_showNormal_self()
{
    this->showNormal();
    this->activateWindow();
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    pContextMenu->hide();
}

void MainWindow::SystemTray_showDonate()
{
    this->showNormal();
    this->activateWindow();
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    pContextMenu->hide();
    ui->tabWidget->setCurrentIndex(0);
}


