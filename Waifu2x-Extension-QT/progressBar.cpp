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
/*
Set the minimum and maximum value of the progress bar
*/
void MainWindow::progressbar_setRange_min_max(int min, int max)
{
    // This slot is deprecated. File loading progress uses progressbar_setRange_min_max_slots.
    qDebug() << "Deprecated progressbar_setRange_min_max() called. Min:" << min << "Max:" << max;
    // ui->progressBar->setRange(min, max);
    // TaskNumTotal=max;
    // ui->label_progressBar_filenum->setText("0/"+QString::number(max,10));
}
/*
Fill the progress bar directly
*/
void MainWindow::progressbar_SetToMax(int maxval)
{
    // This function sets the progress to 100% for a given total (maxval).
    // It implies that all items up to maxval are considered "finished".
    m_TotalNumProc = maxval;
    m_FinishedProc = maxval; // All items are finished
    m_ErrorProc = 0;         // No errors in this context

    // TaskNumFinished and TaskNumTotal might also need alignment if this function is used for batch completion.
    // For now, focusing on m_...Proc variables for ui->progressBar.
    // TaskNumFinished = maxval; // Align if this is for overall batch
    // TaskNumTotal = maxval;    // Align if this is for overall batch

    UpdateProgressBar(); // This will set value to 100% and update format string.

    // ui->label_progressBar_filenum is updated by UpdateProgressBar's format string.
    // If a separate label update is still desired:
    // ui->label_progressBar_filenum->setText(QString::number(m_FinishedProc + m_ErrorProc,10)+"/"+QString::number(m_TotalNumProc,10));
}
/*
Progress bar progress +1
*/
void MainWindow::progressbar_Add()
{
    // This is the OLD progressbar_Add slot.
    // The new file loading progress is handled by progressbar_Add_slots in mainwindow.cpp
    // This function should ideally be deprecated or its callsites updated.
    // For now, let's make it a no-op regarding m_...Proc to avoid interference.
    // It originally modified Progressbar_CurrentVal, TaskNumFinished, and ui->progressBar.
    // TaskNumFinished++; // TaskNumFinished is still used by TimeSlot, but should not be incremented by this deprecated function.
    // NewTaskFinished=true;
    // ui->label_progressBar_filenum->setText(QString::number(Progressbar_CurrentVal,10)+"/"+QString::number(Progressbar_MaxVal,10)); // Uses obsolete vars
    qDebug() << "Deprecated progressbar_Add() called. Check call sites.";
}
/*
Clear progress bar
*/
void MainWindow::progressbar_clear()
{
    // Resets all primary progress counters and updates the UI.
    m_TotalNumProc = 0;
    m_FinishedProc = 0;
    m_ErrorProc = 0;

    TaskNumFinished = 0; // Also reset TaskNumFinished as it's related to overall progress for ETA
    TaskNumTotal = 0;    // Reset TaskNumTotal as well
    ETA = 0;             // Reset ETA
    TimeCost = 0;        // Reset TimeCost

    UpdateProgressBar(); // This will set value to 0 and format to "Waiting for tasks..."

    // ui->label_progressBar_filenum is handled by UpdateProgressBar's format string.
    // If it needs to be cleared independently:
    if (ui->label_progressBar_filenum) ui->label_progressBar_filenum->setText("0/0");
    if (ui->label_ETA) ui->label_ETA->setText(tr("ETA:N/A"));
    if (ui->label_TimeCost) ui->label_TimeCost->setText(tr("Time taken:N/A"));
}

