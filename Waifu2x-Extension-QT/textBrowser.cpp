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
Output a new message in the text box
*/
void MainWindow::TextBrowser_NewMessage(QString message)
{
    QString Current_Time = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss]");
    QString OutPutStr = Current_Time+" "+message;
    ui->textBrowser->append(OutPutStr);
    ui->textBrowser->moveCursor(QTextCursor::End);
}
/*
Output the welcome message in the text box
Displayed on startup
*/
void MainWindow::TextBrowser_StartMes()
{
    QString CurrentVerState="";
    if(isBetaVer)
    {
        CurrentVerState=tr("[Beta]");
    }
    else
    {
        CurrentVerState=tr("[Stable]");
    }
    //====
    ui->textBrowser->append("Beya_Waifu by beyawnko");
    ui->textBrowser->append(tr("Version:")+" "+VERSION+" "+CurrentVerState);
    ui->textBrowser->append("Github: https://github.com/beyawnko/Beya_Waifu");
    ui->textBrowser->append("Gitee: https://gitee.com/beyawnko/Beya_Waifu");
    ui->textBrowser->append(tr("Please donate to support the developers, so we can bring further updates for this software, thank you! (^_^)/"));
    ui->textBrowser->moveCursor(QTextCursor::End);
}
