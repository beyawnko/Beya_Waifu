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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qRegisterMetaTypeStreamOperators<QList_QMap_QStrQStr >("QList_QMap_QStrQStr");
    QThreadPool::globalInstance()->setMaxThreadCount(60);//解除全局线程池的最大线程数量限制
    //==============
    this->setWindowTitle("Waifu2x-Extension-GUI "+VERSION+" by Aaron Feng");
    //==============
    translator = new QTranslator(this);
    //==============
    ui->tabWidget->setCurrentIndex(1);//显示home tab
    ui->tabWidget->tabBar()->setTabTextColor(0,Qt::red);
    on_tabWidget_currentChanged(1);
    ui->tabWidget_Engines->setCurrentIndex(0);
    this->setAcceptDrops(true);//mainwindow接收drop
    Init_Table();//初始化table
    ui->groupBox_CurrentFile->setVisible(0);//隐藏当前文件进度
    pushButton_Stop_setEnabled_self(0);//禁用隐藏暂停按钮
    ui->pushButton_ForceRetry->setVisible(0);
    ui->progressBar_CompatibilityTest->setVisible(0);
    //=================== 初始隐藏所有table和禁用按钮 ======================
    ui->tableView_image->setVisible(0);
    ui->tableView_gif->setVisible(0);
    ui->tableView_video->setVisible(0);
    Table_FileCount_reload();//重载文件列表下的文件数量统计
    //==============
    Init_ActionsMenu_checkBox_ReplaceOriginalFile();//第一次初始化[替换原文件]的右键菜单(需要在载入设定前设置为checkable
    Init_ActionsMenu_checkBox_DelOriginal();
    //=========安装事件过滤器==========
    ui->tableView_image->installEventFilter(this);
    ui->tableView_gif->installEventFilter(this);
    ui->tableView_video->installEventFilter(this);
    //===========================================
    connect(this, SIGNAL(Send_Set_checkBox_DisableResize_gif_Checked()), this, SLOT(Set_checkBox_DisableResize_gif_Checked()));
    connect(this, SIGNAL(Send_Table_EnableSorting(bool)), this, SLOT(Table_EnableSorting(bool)));
    connect(this, SIGNAL(Send_Add_progressBar_CompatibilityTest()), this, SLOT(Add_progressBar_CompatibilityTest()));
    connect(this, SIGNAL(Send_Unable2Connect_RawGithubusercontentCom()), this, SLOT(Unable2Connect_RawGithubusercontentCom()));
    connect(this, SIGNAL(Send_SetEnable_pushButton_ForceRetry_self()), this, SLOT(SetEnable_pushButton_ForceRetry_self()));
    connect(this, SIGNAL(Send_SystemTray_NewMessage(QString)), this, SLOT(SystemTray_NewMessage(QString)));
    connect(this, SIGNAL(Send_PrograssBar_Range_min_max(int, int)), this, SLOT(progressbar_setRange_min_max(int, int)));
    connect(this, SIGNAL(Send_progressbar_Add()), this, SLOT(progressbar_Add()));
    connect(this, SIGNAL(Send_Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_image_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_gif_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)), this, SLOT(Table_video_ChangeStatus_rowNumInt_statusQString(int, QString)));
    connect(this, SIGNAL(Send_Table_FileCount_reload()), this, SLOT(Table_FileCount_reload()));
    connect(this, SIGNAL(Send_Table_image_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_image_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_gif_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_gif_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_video_insert_fileName_fullPath(QString,QString)), this, SLOT(Table_video_insert_fileName_fullPath(QString,QString)));
    connect(this, SIGNAL(Send_Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_image_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_gif_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)), this, SLOT(Table_video_CustRes_rowNumInt_HeightQString_WidthQString(int,QString,QString)));
    connect(this, SIGNAL(Send_Table_Read_Saved_Table_Filelist_Finished(QString)), this, SLOT(Table_Read_Saved_Table_Filelist_Finished(QString)));
    connect(this, SIGNAL(Send_Table_Save_Current_Table_Filelist_Finished()), this, SLOT(Table_Save_Current_Table_Filelist_Finished()));
    connect(this, SIGNAL(Send_Waifu2x_Finished()), this, SLOT(Waifu2x_Finished()));
    connect(this, SIGNAL(Send_Waifu2x_Finished_manual()), this, SLOT(Waifu2x_Finished_manual()));
    connect(this, SIGNAL(Send_TextBrowser_NewMessage(QString)), this, SLOT(TextBrowser_NewMessage(QString)));
    connect(this, SIGNAL(Send_Waifu2x_Compatibility_Test_finished()), this, SLOT(Waifu2x_Compatibility_Test_finished()));
    connect(this, SIGNAL(Send_Waifu2x_DetectGPU_finished()), this, SLOT(Waifu2x_DetectGPU_finished()));
    connect(this, SIGNAL(Send_Realsr_ncnn_vulkan_DetectGPU_finished()), this, SLOT(Realsr_ncnn_vulkan_DetectGPU_finished()));
    connect(this, SIGNAL(Send_CheckUpadte_NewUpdate(QString,QString)), this, SLOT(CheckUpadte_NewUpdate(QString,QString)));
    connect(this, SIGNAL(Send_SystemShutDown()), this, SLOT(SystemShutDown()));
    connect(this, SIGNAL(Send_Waifu2x_DumpProcessorList_converter_finished()), this, SLOT(Waifu2x_DumpProcessorList_converter_finished()));
    connect(this, SIGNAL(Send_Read_urls_finfished()), this, SLOT(Read_urls_finfished()));
    connect(this, SIGNAL(Send_FinishedProcessing_DN()), this, SLOT(FinishedProcessing_DN()));
    connect(this, SIGNAL(Send_SRMD_DetectGPU_finished()), this, SLOT(SRMD_DetectGPU_finished()));
    connect(this, SIGNAL(Send_FrameInterpolation_DetectGPU_finished()), this, SLOT(FrameInterpolation_DetectGPU_finished()));
    connect(this, SIGNAL(Send_video_write_VideoConfiguration(QString,int,int,bool,int,int,QString,bool,QString,QString,bool,int)), this, SLOT(video_write_VideoConfiguration(QString,int,int,bool,int,int,QString,bool,QString,QString,bool,int)));
    connect(this, SIGNAL(Send_Settings_Save()), this, SLOT(Settings_Save()));
    connect(this, SIGNAL(Send_video_write_Progress_ProcessBySegment(QString,int,bool,bool,int,int)), this, SLOT(video_write_Progress_ProcessBySegment(QString,int,bool,bool,int,int)));
    connect(this, SIGNAL(Send_Donate_ReplaceQRCode(QString)), this, SLOT(Donate_ReplaceQRCode(QString)));
    connect(this, SIGNAL(Send_CurrentFileProgress_Start(QString,int)), this, SLOT(CurrentFileProgress_Start(QString,int)));
    connect(this, SIGNAL(Send_CurrentFileProgress_Stop()), this, SLOT(CurrentFileProgress_Stop()));
    connect(this, SIGNAL(Send_CurrentFileProgress_progressbar_Add()), this, SLOT(CurrentFileProgress_progressbar_Add()));
    connect(this, SIGNAL(Send_CurrentFileProgress_progressbar_Add_SegmentDuration(int)), this, SLOT(CurrentFileProgress_progressbar_Add_SegmentDuration(int)));
    connect(this, SIGNAL(Send_CurrentFileProgress_progressbar_SetFinishedValue(int)), this, SLOT(CurrentFileProgress_progressbar_SetFinishedValue(int)));
    //======
    TimeCostTimer = new QTimer();
    connect(TimeCostTimer, SIGNAL(timeout()), this, SLOT(TimeSlot()));
    //==================================================
    Settings_Read_Apply();//读取与应用设置
    //=====================================
    Set_Font_fixed();//固定字体
    //=====================================
    QtConcurrent::run(this, &MainWindow::DeleteErrorLog_Waifu2xCaffe);//删除Waifu2xCaffe生成的错误日志
    QtConcurrent::run(this, &MainWindow::Del_TempBatFile);//删除bat文件缓存
    AutoUpdate = QtConcurrent::run(this, &MainWindow::CheckUpadte_Auto);//自动检查更新线程
    DownloadOnlineQRCode = QtConcurrent::run(this, &MainWindow::Donate_DownloadOnlineQRCode);//在线更新捐赠二维码
    SystemShutDown_isAutoShutDown();//上次是否自动关机
    //====================================
    TextBrowser_StartMes();//显示启动msg
    //===================================
    Tip_FirstTimeStart();//首次启动
    file_mkDir(Current_Path+"/FilesList_W2xEX");//生成保存文件列表的文件夹
    //==============
    /*
    校验软件是否对所在目录有写权限
    */
    if(file_isDirWritable(Current_Path)==false)
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("Error")), QString(tr("It is detected that this software lacks the necessary permissions to run."
                        "\n\nPlease close this software and start this software again after giving this software administrator permission. "
                        "Or reinstall the software into a directory that can run normally without administrator rights.\n\nOtherwise, this software may not work properly.")));
        Msg.setIcon(QMessageBox::Warning);
        Msg.addButton(QString("OK"), QMessageBox::NoRole);
        Msg.exec();
    }
    //==============
    Init_SystemTrayIcon();//初始化托盘图标
    Init_ActionsMenu_lineEdit_outputPath();//初始化 输出路径 lineEDIT的右键菜单
    Init_ActionsMenu_FilesList();
    Init_ActionsMenu_pushButton_RemoveItem();
    Init_ActionsMenu_checkBox_ReplaceOriginalFile();//第二次初始化[替换原文件]的右键菜单(载入语言设置
    Init_ActionsMenu_checkBox_DelOriginal();
    //==============

    // RealCUGAN UI Pointers
    ui->comboBox_Model_RealCUGAN = findChild<QComboBox*>("comboBox_Model_RealCUGAN");
    ui->spinBox_Scale_RealCUGAN = findChild<QSpinBox*>("spinBox_Scale_RealCUGAN");
    ui->spinBox_DenoiseLevel_RealCUGAN = findChild<QSpinBox*>("spinBox_DenoiseLevel_RealCUGAN");
    ui->spinBox_TileSize_RealCUGAN = findChild<QSpinBox*>("spinBox_TileSize_RealCUGAN");
    ui->checkBox_TTA_RealCUGAN = findChild<QCheckBox*>("checkBox_TTA_RealCUGAN");
    ui->comboBox_GPUID_RealCUGAN = findChild<QComboBox*>("comboBox_GPUID_RealCUGAN");
    ui->pushButton_DetectGPU_RealCUGAN = findChild<QPushButton*>("pushButton_DetectGPU_RealCUGAN");
    ui->checkBox_MultiGPU_RealCUGAN = findChild<QCheckBox*>("checkBox_MultiGPU_RealCUGAN");
    ui->groupBox_GPUSettings_MultiGPU_RealCUGAN = findChild<QGroupBox*>("groupBox_GPUSettings_MultiGPU_RealCUGAN");
    ui->comboBox_GPUIDs_MultiGPU_RealCUGAN = findChild<QComboBox*>("comboBox_GPUIDs_MultiGPU_RealCUGAN");
    ui->pushButton_AddGPU_MultiGPU_RealCUGAN = findChild<QPushButton*>("pushButton_AddGPU_MultiGPU_RealCUGAN");

    // RealCUGAN Process List
    ProcList_RealCUGAN.clear();

    // RealCUGAN Preload Settings
    Realcugan_NCNN_Vulkan_PreLoad_Settings();

    // RealCUGAN Connects
    if(ui->pushButton_DetectGPU_RealCUGAN)
        connect(ui->pushButton_DetectGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked);
    if(ui->checkBox_MultiGPU_RealCUGAN)
        connect(ui->checkBox_MultiGPU_RealCUGAN, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged);
    if(ui->pushButton_AddGPU_MultiGPU_RealCUGAN)
        connect(ui->pushButton_AddGPU_MultiGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked);
    // Assuming pushButton_TileSize_Add_RealCUGAN and pushButton_TileSize_Minus_RealCUGAN are actual object names
    ui->pushButton_TileSize_Add_RealCUGAN = findChild<QPushButton*>("pushButton_TileSize_Add_RealCUGAN");
    ui->pushButton_TileSize_Minus_RealCUGAN = findChild<QPushButton*>("pushButton_TileSize_Minus_RealCUGAN");
    if(ui->pushButton_TileSize_Add_RealCUGAN)
        connect(ui->pushButton_TileSize_Add_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked);
    if(ui->pushButton_TileSize_Minus_RealCUGAN)
        connect(ui->pushButton_TileSize_Minus_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked);

    // Also connect remove and clear GPU buttons for RealCUGAN MultiGPU
    ui->pushButton_RemoveGPU_MultiGPU_RealCUGAN = findChild<QPushButton*>("pushButton_RemoveGPU_MultiGPU_RealCUGAN");
    ui->pushButton_ClearGPU_MultiGPU_RealCUGAN = findChild<QPushButton*>("pushButton_ClearGPU_MultiGPU_RealCUGAN");
    if(ui->pushButton_RemoveGPU_MultiGPU_RealCUGAN)
        connect(ui->pushButton_RemoveGPU_MultiGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked);
    if(ui->pushButton_ClearGPU_MultiGPU_RealCUGAN)
        connect(ui->pushButton_ClearGPU_MultiGPU_RealCUGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked);
    if(ui->comboBox_Model_RealCUGAN) // comboBox_Model_RealCUGAN was already found earlier
        connect(ui->comboBox_Model_RealCUGAN, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged);

    // RealESRGAN UI Pointers
    ui->comboBox_Model_RealESRGAN = findChild<QComboBox*>("comboBox_Model_RealESRGAN");
    ui->comboBox_GPUID_RealESRGAN = findChild<QComboBox*>("comboBox_GPUID_RealESRGAN");
    ui->pushButton_DetectGPU_RealESRGAN = findChild<QPushButton*>("pushButton_DetectGPU_RealESRGAN");
    ui->spinBox_TileSize_RealESRGAN = findChild<QSpinBox*>("spinBox_TileSize_RealESRGAN");
    ui->pushButton_TileSize_Add_RealESRGAN = findChild<QPushButton*>("pushButton_TileSize_Add_RealESRGAN");
    ui->pushButton_TileSize_Minus_RealESRGAN = findChild<QPushButton*>("pushButton_TileSize_Minus_RealESRGAN");
    ui->checkBox_TTA_RealESRGAN = findChild<QCheckBox*>("checkBox_TTA_RealESRGAN");
    ui->checkBox_MultiGPU_RealESRGAN = findChild<QCheckBox*>("checkBox_MultiGPU_RealESRGAN");
    ui->groupBox_GPUSettings_MultiGPU_RealESRGAN = findChild<QGroupBox*>("groupBox_GPUSettings_MultiGPU_RealESRGAN");
    ui->comboBox_GPUIDs_MultiGPU_RealESRGAN = findChild<QComboBox*>("comboBox_GPUIDs_MultiGPU_RealESRGAN");
    ui->listWidget_GPUList_MultiGPU_RealESRGAN = findChild<QListWidget*>("listWidget_GPUList_MultiGPU_RealESRGAN");
    ui->pushButton_AddGPU_MultiGPU_RealESRGAN = findChild<QPushButton*>("pushButton_AddGPU_MultiGPU_RealESRGAN");
    ui->pushButton_RemoveGPU_MultiGPU_RealESRGAN = findChild<QPushButton*>("pushButton_RemoveGPU_MultiGPU_RealESRGAN");
    ui->pushButton_ClearGPU_MultiGPU_RealESRGAN = findChild<QPushButton*>("pushButton_ClearGPU_MultiGPU_RealESRGAN");
    ui->spinBox_Threads_MultiGPU_RealESRGAN = findChild<QSpinBox*>("spinBox_Threads_MultiGPU_RealESRGAN");
    ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN = findChild<QCheckBox*>("checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN");
    ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN = findChild<QSpinBox*>("spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN");
    ui->pushButton_ShowMultiGPUSettings_RealESRGAN = findChild<QPushButton*>("pushButton_ShowMultiGPUSettings_RealESRGAN");

    // RealESRGAN Process List & Variables
    ProcList_RealESRGAN.clear();
    GPU_ID_RealesrganNcnnVulkan_MultiGPU_CycleCounter = 0;
    isCompatible_RealESRGAN_NCNN_Vulkan = false;
    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.clear();

    // RealESRGAN Preload Settings
    RealESRGAN_NCNN_Vulkan_PreLoad_Settings();

    // RealESRGAN Connects
    if(ui->pushButton_DetectGPU_RealESRGAN)
        connect(ui->pushButton_DetectGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_DetectGPU_RealESRGAN_clicked);
    if(ui->comboBox_Model_RealESRGAN)
        connect(ui->comboBox_Model_RealESRGAN, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_Model_RealESRGAN_currentIndexChanged);
    if(ui->pushButton_TileSize_Add_RealESRGAN)
        connect(ui->pushButton_TileSize_Add_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Add_RealESRGAN_clicked);
    if(ui->pushButton_TileSize_Minus_RealESRGAN)
        connect(ui->pushButton_TileSize_Minus_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_TileSize_Minus_RealESRGAN_clicked);
    if(ui->checkBox_MultiGPU_RealESRGAN)
        connect(ui->checkBox_MultiGPU_RealESRGAN, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_MultiGPU_RealESRGAN_stateChanged);
    if(ui->comboBox_GPUIDs_MultiGPU_RealESRGAN)
        connect(ui->comboBox_GPUIDs_MultiGPU_RealESRGAN, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_GPUIDs_MultiGPU_RealESRGAN_currentIndexChanged);
    if(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN)
        connect(ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN, &QCheckBox::clicked, this, &MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN_clicked);
    if(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN)
        connect(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN_valueChanged);
    if(ui->pushButton_ShowMultiGPUSettings_RealESRGAN)
        connect(ui->pushButton_ShowMultiGPUSettings_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_ShowMultiGPUSettings_RealESRGAN_clicked);
    if(ui->pushButton_AddGPU_MultiGPU_RealESRGAN)
        connect(ui->pushButton_AddGPU_MultiGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_AddGPU_MultiGPU_RealESRGAN_clicked);
    if(ui->pushButton_RemoveGPU_MultiGPU_RealESRGAN)
        connect(ui->pushButton_RemoveGPU_MultiGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealESRGAN_clicked);
    if(ui->pushButton_ClearGPU_MultiGPU_RealESRGAN)
        connect(ui->pushButton_ClearGPU_MultiGPU_RealESRGAN, &QPushButton::clicked, this, &MainWindow::on_pushButton_ClearGPU_MultiGPU_RealESRGAN_clicked);

    this->showNormal();
    this->activateWindow();
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    this->adjustSize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ... (rest of the file from turn 7, starting from MainWindow::closeEvent) ...
// ... I will manually insert the changes into this copied content ...

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(isAlreadyClosed)//解决QT退出时重复调用closeEvent的问题
    {
        event->accept();
        return;
    }
    //=============== 询问是否退出 =======================
    if(ui->checkBox_PromptWhenExit->isChecked())
    {
        QMessageBox Msg(QMessageBox::Question, QString(tr("Notification")), QString(tr("Do you really wanna exit Waifu2x-Extension-GUI ?")));
        Msg.setIcon(QMessageBox::Question);
        QAbstractButton *pYesBtn = Msg.addButton(QString(tr("YES")), QMessageBox::YesRole);
        QAbstractButton *pNoBtn = Msg.addButton(QString(tr("NO")), QMessageBox::NoRole);
        Msg.exec();
        if (Msg.clickedButton() == pNoBtn)
        {
            event->ignore();
            return;
        }
        if (Msg.clickedButton() == pYesBtn)isAlreadyClosed=true;
    }
    //=============================
    systemTray->hide();
    this->hide();
    QApplication::setQuitOnLastWindowClosed(true);//無窗口時不再保持運行
    QApplication::closeAllWindows();
    //====
    if(Waifu2xMain.isRunning() == true)
    {
        TimeCostTimer->stop();
        pushButton_Stop_setEnabled_self(0);//隐藏stop button
        waifu2x_STOP = true;
        emit TextBrowser_NewMessage(tr("Trying to stop, please wait..."));
        //======
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Notification")+" @Waifu2x-Extension-GUI");
        MSG_2->setText(tr("Waiting for the files processing thread to pause"));
        MSG_2->setIcon(QMessageBox::Information);
        MSG_2->setModal(true);
        MSG_2->setStandardButtons(NULL);
        MSG_2->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        MSG_2->show();
    }
    else
    {
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Notification")+" @Waifu2x-Extension-GUI");
        MSG_2->setText(tr("Closing...\n\nPlease wait"));
        MSG_2->setIcon(QMessageBox::Information);
        MSG_2->setModal(true);
        MSG_2->setStandardButtons(NULL);
        MSG_2->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        MSG_2->show();
    }
    AutoUpdate.cancel();
    DownloadOnlineQRCode.cancel();
    //=====
    bool AutoSaveSettings = ui->checkBox_AutoSaveSettings->isChecked();
    if(AutoSaveSettings&&(!Settings_isReseted))
    {
        Settings_Save();
        QtConcurrent::run(this, &MainWindow::Auto_Save_Settings_Watchdog,true);
    }
    else
    {
        QtConcurrent::run(this, &MainWindow::Auto_Save_Settings_Watchdog,false);
    }
}

int MainWindow::Auto_Save_Settings_Watchdog(bool isWaitForSave)
{
    Waifu2xMain.waitForFinished();
    //======
    if(isWaitForSave == true)
    {
        Delay_msec_sleep(1000);
        QString settings_ini = Current_Path+"/settings.ini";
        while(!QFile::exists(settings_ini))
        {
            Delay_msec_sleep(250);
        }
        Delay_msec_sleep(3000);
    }
    //=====
    Force_close();
    //====
    return 0;
}

int MainWindow::Force_close()
{
    //=============
    QStringList TaskNameList;
    TaskNameList << "convert_waifu2xEX.exe"<<"ffmpeg_waifu2xEX.exe"<<"ffprobe_waifu2xEX.exe"<<"identify_waifu2xEX.exe"<<"gifsicle_waifu2xEX.exe"<<"waifu2x-ncnn-vulkan_waifu2xEX.exe"
                 <<"waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe"<<"Anime4K_waifu2xEX.exe"<<"waifu2x-caffe_waifu2xEX.exe"<<"srmd-ncnn-vulkan_waifu2xEX.exe"<<"realsr-ncnn-vulkan_waifu2xEX.exe"
                 <<"waifu2x-converter-cpp_waifu2xEX.exe"<<"sox_waifu2xEX.exe"<<"wget_waifu2xEX.exe"<<"rife-ncnn-vulkan_waifu2xEX.exe"<<"cain-ncnn-vulkan_waifu2xEX.exe"<<"dain-ncnn-vulkan_waifu2xEX.exe"
                 <<"srmd-cuda_waifu2xEX.exe"<<"apngdis_waifu2xEX.exe"<<"apngasm_waifu2xEX.exe";
    KILL_TASK_QStringList(TaskNameList,true);
    //===========
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq Waifu2x-Extension-GUI.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    return 0;
}
/*
最小化
*/
void MainWindow::changeEvent(QEvent *e)
{
    if((e->type()==QEvent::WindowStateChange)&&this->isMinimized())
    {
        if(ui->checkBox_MinimizeToTaskbar->isChecked())
        {
            this->hide();
        }
    }
}
/*
计时器到时执行的代码
*/
void MainWindow::TimeSlot()
{
    TimeCost++;
    //====================总进度==================
    QString TimeCostStr = tr("Time taken:[")+Seconds2hms(TimeCost)+"]";
    ui->label_TimeCost->setText(TimeCostStr);
    if(ui->label_TimeRemain->isVisible())
    {
        long unsigned int TaskNumFinished_tmp = TaskNumFinished;
        long unsigned int TimeCost_tmp = TimeCost;
        long unsigned int TaskNumTotal_tmp = TaskNumTotal;
        if(TaskNumFinished_tmp>0&&TimeCost_tmp>0&&TaskNumTotal_tmp>0)
        {
            if(NewTaskFinished)
            {
                NewTaskFinished=false;
                double avgTimeCost = (double)TimeCost_tmp/(double)TaskNumFinished_tmp;
                ETA = avgTimeCost*((double)TaskNumTotal_tmp-(double)TaskNumFinished_tmp);
            }
            else
            {
                if(ETA>1)
                {
                    ETA--;
                }
            }
            QString TimeRemainingStr = tr("Time remaining:[")+Seconds2hms(ETA)+"]";
            ui->label_TimeRemain->setText(TimeRemainingStr);
            QDateTime time = QDateTime::currentDateTime();
            long unsigned int Time_t = time.toTime_t();
            Time_t+=ETA;
            time = QDateTime::fromTime_t(Time_t);
            QString Current_Time = time.toString("hh:mm:ss");
            QString ETA_str = "ETA:["+Current_Time+"]";
            ui->label_ETA->setText(ETA_str);
        }
    }
    //====================当前文件=================
    if(isStart_CurrentFile)
    {
        TimeCost_CurrentFile++;
        QString TimeCostStr_CurrentFile = tr("Time taken:[")+Seconds2hms(TimeCost_CurrentFile)+"]";
        ui->label_TimeCost_CurrentFile->setText(TimeCostStr_CurrentFile);
        long unsigned int TaskNumFinished_tmp_CurrentFile = TaskNumFinished_CurrentFile;
        long unsigned int TimeCost_tmp_CurrentFile = TimeCost_CurrentFile;
        long unsigned int TaskNumTotal_tmp_CurrentFile = TaskNumTotal_CurrentFile;
        if(TaskNumFinished_tmp_CurrentFile>0&&TimeCost_tmp_CurrentFile>0&&TaskNumTotal_tmp_CurrentFile>0)
        {
            if(NewTaskFinished_CurrentFile)
            {
                NewTaskFinished_CurrentFile=false;
                double avgTimeCost_CurrentFile = (double)TimeCost_tmp_CurrentFile/(double)TaskNumFinished_tmp_CurrentFile;
                ETA_CurrentFile = avgTimeCost_CurrentFile*((double)TaskNumTotal_tmp_CurrentFile-(double)TaskNumFinished_tmp_CurrentFile);
            }
            else
            {
                if(ETA_CurrentFile>1)
                {
                    ETA_CurrentFile--;
                }
            }
            QString TimeRemainingStr_CurrentFile = tr("Time remaining:[")+Seconds2hms(ETA_CurrentFile)+"]";
            ui->label_TimeRemain_CurrentFile->setText(TimeRemainingStr_CurrentFile);
            QDateTime time_CurrentFile = QDateTime::currentDateTime();
            long unsigned int Time_t_CurrentFile = time_CurrentFile.toTime_t();
            Time_t_CurrentFile+=ETA_CurrentFile;
            time_CurrentFile = QDateTime::fromTime_t(Time_t_CurrentFile);
            QString Current_Time_CurrentFile = time_CurrentFile.toString("hh:mm:ss");
            QString ETA_str_CurrentFile = "ETA:["+Current_Time_CurrentFile+"]";
            ui->label_ETA_CurrentFile->setText(ETA_str_CurrentFile);
        }
    }
}
QString MainWindow::Seconds2hms(long unsigned int seconds)
{
    if(seconds<=0)return "0:0:0";
    long unsigned int hour = seconds / 3600;
    long unsigned int min = (seconds-(hour*3600))/60;
    long unsigned int sec = seconds - hour*3600 - min*60;
    return QString::number(hour,10)+":"+QString::number(min,10)+":"+QString::number(sec,10);
}

void MainWindow::Set_Font_fixed()
{
    QFont font;
    if(ui->checkBox_isCustFontEnable->isChecked())
    {
        font = ui->fontComboBox_CustFont->currentFont();
        font.setPixelSize(ui->spinBox_GlobalFontSize->value());
    }
    else
    {
        font = qApp->font();
        font.setPixelSize(15);
    }
    qApp->setFont(font);
}

void MainWindow::on_pushButton_ClearList_clicked()
{
    curRow_image = -1;
    curRow_gif = -1;
    curRow_video = -1;
    Table_Clear();
    Custom_resolution_list.clear();
    ui->label_DropFile->setVisible(1);
    ui->tableView_gif->setVisible(0);
    ui->tableView_image->setVisible(0);
    ui->tableView_video->setVisible(0);
    Table_FileCount_reload();
    progressbar_clear();
}

/*
停止处理键
*/
void MainWindow::on_pushButton_Stop_clicked()
{
    if(Waifu2xMain.isRunning()==false)return;
    TimeCostTimer->stop();
    pushButton_Stop_setEnabled_self(0);//隐藏stop button
    waifu2x_STOP = true;
    emit TextBrowser_NewMessage(tr("Trying to stop, please wait..."));
    QtConcurrent::run(this, &MainWindow::Wait_waifu2x_stop);
}
/*
等待处理线程完全停止
*/
void MainWindow::Wait_waifu2x_stop()
{
    while(true)
    {
        if(waifu2x_STOP_confirm||ThreadNumRunning==0)
        {
            waifu2x_STOP_confirm = false;
            Waifu2xMain.waitForFinished();
            while(true)
            {
                if(Waifu2xMain.isRunning()==false)break;
                Delay_msec_sleep(300);
            }
            emit TextBrowser_NewMessage(tr("Processing of files has stopped."));
            QtConcurrent::run(this, &MainWindow::Play_NFSound);//成功暂停,播放提示音
            break;
        }
        Delay_msec_sleep(300);
    }
    emit Send_Waifu2x_Finished_manual();
}
/*
从tableview移除item
*/
int MainWindow::on_pushButton_RemoveItem_clicked()
{
    if(curRow_image==-1&&curRow_video==-1&&curRow_gif==-1)
    {
        ui->tableView_image->clearSelection();
        ui->tableView_gif->clearSelection();
        ui->tableView_video->clearSelection();
        //=====
        QMessageBox *MSG = new QMessageBox();
        MSG->setWindowTitle(tr("Warning"));
        MSG->setText(tr("No items are currently selected."));
        MSG->setIcon(QMessageBox::Warning);
        MSG->setModal(true);
        MSG->show();
        //=====
        return 0;
    }
    //==========================
    if(curRow_image >= 0)
    {
        CustRes_remove(Table_model_image->item(curRow_image,2)->text());
        ui->tableView_image->setUpdatesEnabled(false);
        Table_model_image->removeRow(curRow_image);
        ui->tableView_image->setUpdatesEnabled(true);
        curRow_image = -1;
        ui->tableView_image->clearSelection();
    }
    //============================================================
    if(curRow_video >= 0)
    {
        CustRes_remove(Table_model_video->item(curRow_video,2)->text());
        ui->tableView_video->setUpdatesEnabled(false);
        Table_model_video->removeRow(curRow_video);
        ui->tableView_video->setUpdatesEnabled(true);
        curRow_video = -1;
        ui->tableView_video->clearSelection();
    }
    //============================================================
    if(curRow_gif >= 0)
    {
        CustRes_remove(Table_model_gif->item(curRow_gif,2)->text());
        ui->tableView_gif->setUpdatesEnabled(false);
        Table_model_gif->removeRow(curRow_gif);
        ui->tableView_gif->setUpdatesEnabled(true);
        curRow_gif = -1;
        ui->tableView_gif->clearSelection();
    }
    //==================================================
    if(Table_model_gif->rowCount()==0)
    {
        ui->tableView_gif->setVisible(0);
    }
    if(Table_model_image->rowCount()==0)
    {
        ui->tableView_image->setVisible(0);
    }
    if(Table_model_video->rowCount()==0)
    {
        ui->tableView_video->setVisible(0);
    }
    //===================================================
    if(Table_model_gif->rowCount()==0&&Table_model_image->rowCount()==0&&Table_model_video->rowCount()==0)
    {
        on_pushButton_ClearList_clicked();
    }
    Table_FileCount_reload();
    //============
    return 0;
}

//==========================================================
/*
============= 安全的阻塞延时 =====================
*/
void MainWindow::Delay_sec_sleep(int time)
{
    QThread::sleep(time);
}

void MainWindow::Delay_msec_sleep(int time)
{
    QThread::msleep(time);
}
//==========================================================

/*
播放提示音
*/
void MainWindow::Play_NFSound()
{
    if(ui->checkBox_NfSound->isChecked()==false)return;
    //====
    QString NFSound = Current_Path+"/NFSound_Waifu2xEX.mp3";
    if(QFile::exists(NFSound)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Notification sound file is missing!"));
        return;
    }
    //====
    QMediaPlayer *player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile(NFSound));
    player->play();
}

void MainWindow::on_pushButton_Report_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/issues/new"));
}

void MainWindow::on_pushButton_ReadMe_clicked()
{
    if(ui->comboBox_language->currentIndex()==1)
    {
        QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/"));
        QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/"));
    }
    else
    {
        QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/"));
    }
}



void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int index)
{
    switch(ui->comboBox_Engine_Image->currentIndex())
    {
        case 0:
            {
                ui->spinBox_DenoiseLevel_image->setRange(-1,3);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_image->setRange(0,3);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:0(No noise reduction)~3"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~3"));
                break;
            }
        case 2:
        case 6:
            {
                ui->spinBox_DenoiseLevel_image->setRange(-1,10);
                ui->spinBox_DenoiseLevel_image->setValue(4);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:-1(No noise reduction)~10"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~10"));
                break;
            }
        case 3:
            {
                DenoiseLevelSpinboxSetting_Anime4k();
                break;
            }
        case 4:
            {
                ui->spinBox_DenoiseLevel_image->setRange(-1,3);
                ui->spinBox_DenoiseLevel_image->setValue(2);
                ui->spinBox_DenoiseLevel_image->setEnabled(1);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                break;
            }
        case 5:
            {
                ui->spinBox_DenoiseLevel_image->setRange(0,1);
                ui->spinBox_DenoiseLevel_image->setValue(1);
                ui->spinBox_DenoiseLevel_image->setEnabled(0);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Realsr-ncnn-vulkan engine will denoise automatically."));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Realsr-ncnn-vulkan engine will denoise automatically."));
                break;
            }
        case 7: // RealCUGAN
            {
                ui->spinBox_DenoiseLevel_image->setRange(-1,3);
                ui->spinBox_DenoiseLevel_image->setValue(-1);
                ui->spinBox_DenoiseLevel_image->setEnabled(true);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
                break;
            }
        case 8: // RealESRGAN
            {
                ui->spinBox_DenoiseLevel_image->setRange(0,0); // RealESRGAN models usually don't have a separate denoise level param
                ui->spinBox_DenoiseLevel_image->setValue(0);
                ui->spinBox_DenoiseLevel_image->setEnabled(false);
                ui->spinBox_DenoiseLevel_image->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
                ui->label_ImageDenoiseLevel->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
                break;
            }
    }
    isShowAnime4kWarning=true;
    on_comboBox_model_vulkan_currentIndexChanged(0);
    isWaifu2xCaffeEnabled();//判断是否启用caffe图片风格设定
}

void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int index)
{
    switch(ui->comboBox_Engine_GIF->currentIndex())
    {
        case 0:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(-1,3);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(0,3);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:0(No noise reduction)~3"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~3"));
                break;
            }
        case 2:
        case 6:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(-1,10);
                ui->spinBox_DenoiseLevel_gif->setValue(4);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:-1(No noise reduction)~10"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~10"));
                break;
            }
        case 3:
            {
                DenoiseLevelSpinboxSetting_Anime4k();
                break;
            }
        case 4:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(-1,3);
                ui->spinBox_DenoiseLevel_gif->setValue(2);
                ui->spinBox_DenoiseLevel_gif->setEnabled(1);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                break;
            }
        case 5:
            {
                ui->spinBox_DenoiseLevel_gif->setRange(0,1);
                ui->spinBox_DenoiseLevel_gif->setValue(1);
                ui->spinBox_DenoiseLevel_gif->setEnabled(0);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Realsr-ncnn-vulkan engine will denoise automatically."));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Realsr-ncnn-vulkan engine will denoise automatically."));
                break;
            }
        case 7: // RealCUGAN
            {
                ui->spinBox_DenoiseLevel_gif->setRange(-1,3);
                ui->spinBox_DenoiseLevel_gif->setValue(-1);
                ui->spinBox_DenoiseLevel_gif->setEnabled(true);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
                break;
            }
        case 8: // RealESRGAN
            {
                ui->spinBox_DenoiseLevel_gif->setRange(0,0); // RealESRGAN models usually don't have a separate denoise level param
                ui->spinBox_DenoiseLevel_gif->setValue(0);
                ui->spinBox_DenoiseLevel_gif->setEnabled(false);
                ui->spinBox_DenoiseLevel_gif->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
                ui->label_GIFDenoiseLevel->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
                break;
            }
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
    isWaifu2xCaffeEnabled();//判断是否启用caffe图片风格设定
}

void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int index)
{
    switch(ui->comboBox_Engine_Video->currentIndex())
    {
        case 0:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,3);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                break;
            }
        case 1:
            {
                ui->spinBox_DenoiseLevel_video->setRange(0,3);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:0(No noise reduction)~3"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:0(No noise reduction)~3"));
                break;
            }
        case 2:
            {
                DenoiseLevelSpinboxSetting_Anime4k();
                break;
            }
        case 3:
        case 6:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,10);
                ui->spinBox_DenoiseLevel_video->setValue(4);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:-1(No noise reduction)~10"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~10"));
                break;
            }
        case 4:
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,3);
                ui->spinBox_DenoiseLevel_video->setValue(2);
                ui->spinBox_DenoiseLevel_video->setEnabled(1);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Range:-1(No noise reduction)~3"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Range:-1(No noise reduction)~3"));
                break;
            }
        case 5:
            {
                ui->spinBox_DenoiseLevel_video->setRange(0,1);
                ui->spinBox_DenoiseLevel_video->setValue(1);
                ui->spinBox_DenoiseLevel_video->setEnabled(0);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Realsr-ncnn-vulkan engine will denoise automatically."));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Realsr-ncnn-vulkan engine will denoise automatically."));
                break;
            }
        case 7: // RealCUGAN
            {
                ui->spinBox_DenoiseLevel_video->setRange(-1,3);
                ui->spinBox_DenoiseLevel_video->setValue(-1);
                ui->spinBox_DenoiseLevel_video->setEnabled(true);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Denoise Level for RealCUGAN (-1 to 3)"));
                break;
            }
        case 8: // RealESRGAN
            {
                ui->spinBox_DenoiseLevel_video->setRange(0,0); // RealESRGAN models usually don't have a separate denoise level param
                ui->spinBox_DenoiseLevel_video->setValue(0);
                ui->spinBox_DenoiseLevel_video->setEnabled(false);
                ui->spinBox_DenoiseLevel_video->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
                ui->label_VideoDenoiseLevel->setToolTip(tr("Denoise level is model-specific for RealESRGAN and not set here."));
                break;
            }
    }
    on_comboBox_model_vulkan_currentIndexChanged(0);
    isWaifu2xCaffeEnabled();//判断是否启用caffe图片风格设定
}

void MainWindow::on_pushButton_clear_textbrowser_clicked()
{
    ui->textBrowser->clear();
    TextBrowser_StartMes();
}

void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int arg1)
{
    int size = ui->spinBox_textbrowser_fontsize->value();
    ui->textBrowser->setStyleSheet("font: "+QString::number(size,10)+"pt \"Arial\";");
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_CustRes_apply_clicked()
{
    CustRes_SetCustRes();
}

void MainWindow::on_pushButton_CustRes_cancel_clicked()
{
    CustRes_CancelCustRes();
}

void MainWindow::on_pushButton_HideSettings_clicked()
{
    if(ui->groupBox_Setting->isVisible())
    {
        ui->groupBox_Setting->setVisible(0);
        isSettingsHide=true;
        ui->pushButton_HideSettings->setText(tr("Show settings"));
    }
    else
    {
        ui->groupBox_Setting->setVisible(1);
        isSettingsHide=false;
        ui->pushButton_HideSettings->setText(tr("Hide settings"));
    }
}

/*
改变语言设置
*/
void MainWindow::on_comboBox_language_currentIndexChanged(int index)
{
    //检测是否存在日本语翻译文件,若存在则删除并修正翻译设定
    QString JapaneseQM = Current_Path + "/language_Japanese.qm";
    if(QFile::exists(JapaneseQM))
    {
        QFile::remove(JapaneseQM);
        if(ui->comboBox_language->currentIndex()==2)//若原语言为日语则重置为英语
        {
            ui->comboBox_language->setCurrentIndex(0);
        }
        if(ui->comboBox_language->currentIndex()==3 || ui->comboBox_language->currentIndex()==-1)//若原语言为繁中则修正设定
        {
            ui->comboBox_language->setCurrentIndex(2);
        }
    }
    //==============
    QString qmFilename="";
    switch(ui->comboBox_language->currentIndex())
    {
        case 0:
            {
                qmFilename = Current_Path + "/language_English.qm";
                break;
            }
        case 1:
            {
                qmFilename = Current_Path + "/language_Chinese.qm";
                break;
            }
        case 2:
            {
                qmFilename = Current_Path + "/language_TraditionalChinese.qm";
                break;
            }
    }
    //判断文件是否存在
    if(QFile::exists(qmFilename)==false)
    {
        QMessageBox *MSG_languageFile404 = new QMessageBox();
        MSG_languageFile404->setWindowTitle(tr("Error"));
        MSG_languageFile404->setText(tr("Language file is missing, please reinstall this program."));
        MSG_languageFile404->setIcon(QMessageBox::Warning);
        MSG_languageFile404->setModal(true);
        MSG_languageFile404->show();
        return;
    }
    //加载语言文件
    if (translator->load(qmFilename))
    {
        qApp->installTranslator(translator);
        ui->retranslateUi(this);
        Table_FileCount_reload();
        Init_Table();
        Init_SystemTrayIcon();
        Set_Font_fixed();
        //=========
        if(ui->checkBox_AlwaysHideSettings->isChecked())
        {
            ui->groupBox_Setting->setVisible(0);
            isSettingsHide=true;
            ui->pushButton_HideSettings->setText(tr("Show settings"));
        }
        else
        {
            ui->groupBox_Setting->setVisible(1);
            isSettingsHide=false;
            ui->pushButton_HideSettings->setText(tr("Hide settings"));
        }
        //=========
        if(ui->checkBox_AlwaysHideTextBrowser->isChecked())
        {
            ui->splitter_TextBrowser->setVisible(0);
            ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
        }
        else
        {
            ui->splitter_TextBrowser->setVisible(1);
            ui->pushButton_HideTextBro->setText(tr("Hide Text Browser"));
        }
        //=========
        if(this->windowState()!=Qt::WindowMaximized)
        {
            this->adjustSize();
        }
    }
    else
    {
        QMessageBox *MSG_Unable2LoadLanguageFiles = new QMessageBox();
        MSG_Unable2LoadLanguageFiles->setWindowTitle(tr("Error"));
        MSG_Unable2LoadLanguageFiles->setText(tr("Language file cannot be loaded properly."));
        MSG_Unable2LoadLanguageFiles->setIcon(QMessageBox::Warning);
        MSG_Unable2LoadLanguageFiles->setModal(true);
        MSG_Unable2LoadLanguageFiles->show();
    }
}

void MainWindow::on_pushButton_ReadFileList_clicked()
{
    file_mkDir(Current_Path+"/FilesList_W2xEX");//生成保存文件列表的文件夹
    QString Table_FileList_ini = QFileDialog::getOpenFileName(this, tr("Select saved files list @Waifu2x-Extension-GUI"), Current_Path+"/FilesList_W2xEX", "*.ini");
    if(Table_FileList_ini=="")return;
    //========
    if(QFile::exists(Table_FileList_ini))
    {
        ui_tableViews_setUpdatesEnabled(false);
        this->setAcceptDrops(0);//禁止drop file
        pushButton_Start_setEnabled_self(0);//禁用start button
        ui->pushButton_CustRes_cancel->setEnabled(0);
        ui->pushButton_CustRes_apply->setEnabled(0);
        ui->pushButton_ReadFileList->setEnabled(0);
        ui->pushButton_SaveFileList->setEnabled(0);
        ui->pushButton_BrowserFile->setEnabled(0);
        on_pushButton_ClearList_clicked();
        Send_TextBrowser_NewMessage(tr("Please wait while reading the file."));
        ui->label_DropFile->setText(tr("Loading list, please wait."));
        QtConcurrent::run(this, &MainWindow::Table_Read_Saved_Table_Filelist,Table_FileList_ini);
    }
    else
    {
        QMessageBox *MSG_FileList404 = new QMessageBox();
        MSG_FileList404->setWindowTitle(tr("Error"));
        MSG_FileList404->setText(tr("Target files list doesn't exist!"));
        MSG_FileList404->setIcon(QMessageBox::Warning);
        MSG_FileList404->setModal(true);
        MSG_FileList404->show();
    }
}

void MainWindow::on_Ext_image_editingFinished()
{
    QString ext_image_str = ui->Ext_image->text();
    ext_image_str = ext_image_str.trimmed().replace("：",":").remove(" ").remove("　").replace(":gif:",":");
    ui->Ext_image->setText(ext_image_str);
}

void MainWindow::on_Ext_video_editingFinished()
{
    QString ext_video_str = ui->Ext_video->text();
    ext_video_str = ext_video_str.trimmed().replace("：",":").remove(" ").remove("　").replace(":gif:",":");
    ui->Ext_video->setText(ext_video_str);
}

void MainWindow::on_checkBox_AutoSaveSettings_clicked()
{
    QString settings_ini = Current_Path+"/settings.ini";
    if(QFile::exists(settings_ini))
    {
        QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
        configIniWrite->setValue("/settings/AutoSaveSettings", ui->checkBox_AutoSaveSettings->isChecked());
    }
}

void MainWindow::on_pushButton_about_clicked()
{
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("About"));
    QString line1 = "Waifu2x-Extension-GUI\n\n";
    QString line2 = VERSION+"\n\n";
    QString line3 = "Github: https://github.com/AaronFeng753/Waifu2x-Extension-GUI\n\n";
    QString line4 = "Waifu2x-Extension-GUI is licensed under the\n";
    QString line5 = "GNU Affero General Public License v3.0\n\n";
    QString line6 = "Copyright (C) 2021 Aaron Feng. All rights reserved.\n\n";
    QString line7 = "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\n\n";
    QString line8 = "Icons made by : Freepik & Icongeek26 & Roundicons From Flaticon(https://www.flaticon.com/)";
    MSG->setText(line1+line2+line3+line4+line5+line6+line7+line8);
    QImage img(":/new/prefix1/icon/icon_main.png");
    QImage img_scaled = img.scaled(50,50,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pix(QPixmap::fromImage(img_scaled));
    MSG->setIconPixmap(pix);
    MSG->setModal(false);
    MSG->show();
}

void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int index)
{
    int CurrentIndex = ui->comboBox_AspectRatio_custRes->currentIndex();
    switch(CurrentIndex)
    {
        case 0:
            {
                CustRes_AspectRatioMode = Qt::IgnoreAspectRatio;
                break;
            }
        case 1:
            {
                CustRes_AspectRatioMode = Qt::KeepAspectRatio;
                break;
            }
        case 2:
            {
                CustRes_AspectRatioMode = Qt::KeepAspectRatioByExpanding;
                break;
            }
    }
}

void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideSettings->isChecked())
    {
        ui->groupBox_Setting->setVisible(0);
        ui->pushButton_HideSettings->setText(tr("Show settings"));
        isSettingsHide=true;
    }
}

void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()
{
    QString settings_ini = Current_Path+"/settings.ini";
    QSettings *configIniWrite = new QSettings(settings_ini, QSettings::IniFormat);
    configIniWrite->setValue("/settings/GlobalFontSize", ui->spinBox_GlobalFontSize->value());
    //=========
    QMessageBox *MSG = new QMessageBox();
    MSG->setWindowTitle(tr("Notification"));
    MSG->setText(tr("Custom Font Settings saved successfully.\n\nRestart the software to take effect."));
    MSG->setIcon(QMessageBox::Information);
    MSG->setModal(true);
    MSG->show();
}
/*
==================================================================================================
                                      浏览与添加文本
==================================================================================================
*/
void MainWindow::on_pushButton_BrowserFile_clicked()
{
    QString Last_browsed_path = Current_Path+"/LastBrowsedPath_W2xEX.ini";
    //======== 生成 扩展名过滤 字符串 =========
    QStringList nameFilters;
    nameFilters.append("*.gif");
    nameFilters.append("*.apng");
    QString Ext_image_str = ui->Ext_image->text();
    QStringList nameFilters_image = Ext_image_str.split(":");
    nameFilters_image.removeAll("gif");
    nameFilters_image.removeAll("apng");
    for(int i = 0; i < nameFilters_image.size(); ++i)
    {
        QString tmp = nameFilters_image.at(i).trimmed();
        if(tmp=="")continue;
        tmp = "*." + tmp;
        nameFilters.append(tmp);
    }
    QString Ext_video_str = ui->Ext_video->text();
    QStringList nameFilters_video = Ext_video_str.split(":");
    nameFilters_video.removeAll("gif");
    nameFilters_video.removeAll("apng");
    for(int i = 0; i < nameFilters_video.size(); ++i)
    {
        QString tmp = nameFilters_video.at(i).trimmed();
        if(tmp=="")continue;
        tmp = "*." + tmp;
        nameFilters.append(tmp);
    }
    QString nameFilters_QString = "";
    for(int i = 0; i < nameFilters.size(); ++i)
    {
        QString tmp = nameFilters.at(i).trimmed();
        nameFilters_QString = nameFilters_QString +" "+ tmp;
    }
    //=====================================================
    QString BrowserStartPath = "";//浏览文件时的起始文件夹
    //=========== 读取上一次浏览的文件夹 ===========================
    if(QFile::exists(Last_browsed_path))
    {
        QSettings *configIniRead = new QSettings(Last_browsed_path, QSettings::IniFormat);
        configIniRead->setIniCodec(QTextCodec::codecForName("UTF-8"));
        BrowserStartPath = configIniRead->value("/Path").toString();
        if(!QFile::exists(BrowserStartPath))BrowserStartPath = "";
    }
    //===========================================================
    QStringList Input_path_List = QFileDialog::getOpenFileNames(this, tr("Select files @Waifu2x-Extension-GUI"), BrowserStartPath,  tr("All file(")+nameFilters_QString+")");
    if(Input_path_List.isEmpty())
    {
        return;
    }
    //================== 记住上一次浏览的文件夹 =======================
    QFile::remove(Last_browsed_path);
    QSettings *configIniWrite = new QSettings(Last_browsed_path, QSettings::IniFormat);
    configIniWrite->setIniCodec(QTextCodec::codecForName("UTF-8"));
    configIniWrite->setValue("/Warning/EN", "Do not modify this file! It may cause the program to crash! If problems occur after the modification, delete this file and restart the program.");
    QFileInfo lastPath(Input_path_List.at(0));
    QString folder_lastPath = file_getFolderPath(lastPath);
    configIniWrite->setValue("/Path", folder_lastPath);
    //===============================================================
    AddNew_gif=false;
    AddNew_image=false;
    AddNew_video=false;
    //================== 界面管制 ========================
    ui_tableViews_setUpdatesEnabled(false);
    ui->groupBox_Setting->setEnabled(0);
    ui->groupBox_FileList->setEnabled(0);
    ui->groupBox_InputExt->setEnabled(0);
    pushButton_Start_setEnabled_self(0);
    ui->checkBox_ScanSubFolders->setEnabled(0);
    this->setAcceptDrops(0);
    ui->label_DropFile->setText(tr("Adding files, please wait."));
    emit Send_TextBrowser_NewMessage(tr("Adding files, please wait."));
    //===================================================
    QtConcurrent::run(this, &MainWindow::Read_Input_paths_BrowserFile, Input_path_List);
}
/*
读取 路径与添加文件
*/
void MainWindow::Read_Input_paths_BrowserFile(QStringList Input_path_List)
{
    Progressbar_MaxVal = Input_path_List.size();
    Progressbar_CurrentVal = 0;
    emit Send_PrograssBar_Range_min_max(0, Progressbar_MaxVal);
    foreach(QString Input_path, Input_path_List)
    {
        Input_path=Input_path.trimmed();
        if(QFile::exists(Input_path)==false)continue;
        Add_File_Folder(Input_path);
        emit Send_progressbar_Add();
    }
    emit Send_Read_urls_finfished();
}
/*
打开wiki
*/
void MainWindow::on_pushButton_wiki_clicked()
{
    if(ui->comboBox_language->currentIndex()==1)
    {
        QDesktopServices::openUrl(QUrl("https://gitee.com/aaronfeng0711/Waifu2x-Extension-GUI/wikis"));
    }
    QDesktopServices::openUrl(QUrl("https://github.com/AaronFeng753/Waifu2x-Extension-GUI/wiki"));
}

void MainWindow::on_pushButton_HideTextBro_clicked()
{
    if(ui->textBrowser->isVisible())
    {
        ui->splitter_TextBrowser->setVisible(0);
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
    }
    else
    {
        ui->splitter_TextBrowser->setVisible(1);
        ui->pushButton_HideTextBro->setText(tr("Hide Text Browser"));
    }
}

void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1)
{
    if(ui->checkBox_AlwaysHideTextBrowser->isChecked())
    {
        ui->splitter_TextBrowser->setVisible(0);
        ui->pushButton_HideTextBro->setText(tr("Show Text Browser"));
    }
}



void MainWindow::on_Ext_image_textChanged(const QString &arg1)
{
    QString lower = ui->Ext_image->text().toLower();
    ui->Ext_image->setText(lower);
}

void MainWindow::on_Ext_video_textChanged(const QString &arg1)
{
    QString lower = ui->Ext_video->text().toLower();
    ui->Ext_video->setText(lower);
}

void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int index)
{
    if(ui->comboBox_model_vulkan->currentIndex()==0)
    {
        ui->comboBox_ImageStyle->setEnabled(1);
        ui->label_ImageStyle_W2xNCNNVulkan->setVisible(1);
        ui->comboBox_ImageStyle->setVisible(1);
        if(ui->comboBox_Engine_Image->currentIndex()!=0&&ui->comboBox_Engine_GIF->currentIndex()!=0&&ui->comboBox_Engine_Video->currentIndex()!=0)
        {
            ui->comboBox_ImageStyle->setEnabled(0);
            ui->label_ImageStyle_W2xNCNNVulkan->setVisible(0);
            ui->comboBox_ImageStyle->setVisible(0);
        }
    }
    if(ui->comboBox_model_vulkan->currentIndex()==1)
    {
        ui->comboBox_ImageStyle->setEnabled(0);
        ui->label_ImageStyle_W2xNCNNVulkan->setVisible(0);
        ui->comboBox_ImageStyle->setVisible(0);
    }
}

void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int index)
{
    if(ui->comboBox_ImageStyle->currentIndex()==0)
    {
        ui->comboBox_model_vulkan->setEnabled(1);
    }
    if(ui->comboBox_ImageStyle->currentIndex()==1)
    {
        ui->comboBox_model_vulkan->setEnabled(0);
    }
}

void MainWindow::on_pushButton_ResetVideoSettings_clicked()
{
    ui->lineEdit_pixformat->setText("yuv420p");
    ui->lineEdit_encoder_vid->setText("libx264");
    ui->lineEdit_encoder_audio->setText("aac");
    ui->spinBox_bitrate_vid->setValue(6000);
    ui->spinBox_bitrate_audio->setValue(320);
    //====
    ui->spinBox_bitrate_vid_2mp4->setValue(2500);
    ui->spinBox_bitrate_audio_2mp4->setValue(320);
    ui->checkBox_acodec_copy_2mp4->setChecked(0);
    ui->checkBox_vcodec_copy_2mp4->setChecked(0);
    ui->spinBox_bitrate_vid_2mp4->setEnabled(1);
    ui->spinBox_bitrate_audio_2mp4->setEnabled(1);
    //====
    ui->lineEdit_ExCommand_2mp4->setText("");
    ui->lineEdit_ExCommand_output->setText("");
}

void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_encoder_vid->text().trimmed();
    ui->lineEdit_encoder_vid->setText(tmp);
}

void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_encoder_audio->text().trimmed();
    ui->lineEdit_encoder_audio->setText(tmp);
}

void MainWindow::on_lineEdit_pixformat_textChanged(const QString &arg1)
{
    QString tmp = ui->lineEdit_pixformat->text().trimmed();
    ui->lineEdit_pixformat->setText(tmp);
}

void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1)
{
    if(ui->checkBox_vcodec_copy_2mp4->isChecked())
    {
        ui->spinBox_bitrate_vid_2mp4->setEnabled(0);
    }
    else
    {
        ui->spinBox_bitrate_vid_2mp4->setEnabled(1);
    }
}

void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int arg1)
{
    if(ui->checkBox_acodec_copy_2mp4->isChecked())
    {
        ui->spinBox_bitrate_audio_2mp4->setEnabled(0);
    }
    else
    {
        ui->spinBox_bitrate_audio_2mp4->setEnabled(1);
    }
}


void MainWindow::on_pushButton_encodersList_clicked()
{
    file_OpenFile(Current_Path+"/FFmpeg_Encoders_List_waifu2xEX.bat");
}

void MainWindow::Tip_FirstTimeStart()
{
    QString FirstTimeStart = Current_Path+"/FirstTimeStart";
    if(QFile::exists(FirstTimeStart))
    {
        isFirstTimeStart=false;
        return;
    }
    else
    {
        isFirstTimeStart=true;
        /*
          弹出语言选择对话框
        */
        QMessageBox Msg(QMessageBox::Question, QString("Choose your language"), QString("Choose your language.\n\n选择您的语言。\n\n言語を選んでください。"));
        Msg.setIcon(QMessageBox::Information);
        QAbstractButton *pYesBtn_English = Msg.addButton(QString("English"), QMessageBox::YesRole);
        QAbstractButton *pYesBtn_Chinese = Msg.addButton(QString("简体中文"), QMessageBox::YesRole);
        QAbstractButton *pYesBtn_TraditionalChinese = Msg.addButton(QString("繁體中文(由uimee翻譯)"), QMessageBox::YesRole);
        Msg.exec();
        if (Msg.clickedButton() == pYesBtn_English)ui->comboBox_language->setCurrentIndex(0);
        if (Msg.clickedButton() == pYesBtn_Chinese)ui->comboBox_language->setCurrentIndex(1);
        if (Msg.clickedButton() == pYesBtn_TraditionalChinese)ui->comboBox_language->setCurrentIndex(2);
        on_comboBox_language_currentIndexChanged(0);
        //======
        QMessageBox *MSG_2 = new QMessageBox();
        MSG_2->setWindowTitle(tr("Notification"));
        MSG_2->setText(tr("It is detected that this is the first time you have started the software, so the compatibility test will be performed automatically. Please wait for a while, then check the test result."));
        MSG_2->setIcon(QMessageBox::Information);
        MSG_2->setModal(true);
        MSG_2->show();
        //=======
        file_generateMarkFile(FirstTimeStart,"");
        //=======
        on_pushButton_clear_textbrowser_clicked();
        on_pushButton_compatibilityTest_clicked();
    }
}

void MainWindow::on_checkBox_DelOriginal_stateChanged(int arg1)
{
    if(ui->checkBox_DelOriginal->isChecked())
    {
        QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setEnabled(1);
        ui->checkBox_ReplaceOriginalFile->setEnabled(0);
        ui->checkBox_ReplaceOriginalFile->setChecked(0);
    }
    else
    {
        QAction_checkBox_MoveToRecycleBin_checkBox_DelOriginal->setEnabled(0);
        checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    }
}

void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int arg1)
{
    if(ui->checkBox_FileList_Interactive->isChecked())
    {
        ui->tableView_image->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView_gif->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
        ui->tableView_video->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    }
    else
    {
        ui->tableView_image->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_gif->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
        ui->tableView_video->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    }
}

void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int arg1)
{
    if(ui->checkBox_OutPath_isEnabled->isChecked())
    {
        ui->lineEdit_outputPath->setEnabled(1);
        ui->checkBox_OutPath_KeepOriginalFileName->setEnabled(1);
        ui->checkBox_KeepParentFolder->setEnabled(1);
        ui->checkBox_OutPath_Overwrite->setEnabled(1);
        ui->checkBox_ReplaceOriginalFile->setEnabled(0);
        ui->checkBox_ReplaceOriginalFile->setChecked(0);
        ui->checkBox_AutoOpenOutputPath->setEnabled(1);
    }
    else
    {
        ui->lineEdit_outputPath->setEnabled(0);
        ui->checkBox_OutPath_KeepOriginalFileName->setEnabled(0);
        ui->checkBox_KeepParentFolder->setEnabled(0);
        ui->checkBox_OutPath_Overwrite->setEnabled(0);
        ui->checkBox_AutoOpenOutputPath->setEnabled(0);
        checkBox_ReplaceOriginalFile_setEnabled_True_Self();
    }
}

//强制重试
void MainWindow::on_pushButton_ForceRetry_clicked()
{
    if(isForceRetryEnabled==false)//处理视频和gif时禁用强制重试
    {
        emit Send_TextBrowser_NewMessage(tr("Force retry is disabled when processing Video or GIF."));
        return;
    }
    //==========
    ui->pushButton_ForceRetry->setEnabled(0);
    //========
    QtConcurrent::run(this, &MainWindow::isForceRetryClicked_SetTrue_Block_Anime4k);//block a4k引擎线程 防止无效图片污染缓存
    //========
    ForceRetryCount++;
    //========
    QProcess Close;
    Close.start("taskkill /f /t /fi \"imagename eq Anime4K_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-ncnn-vulkan_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-converter-cpp_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq srmd-ncnn-vulkan_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq waifu2x-caffe_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    Close.start("taskkill /f /t /fi \"imagename eq realsr-ncnn-vulkan_waifu2xEX.exe\"");
    Close.waitForStarted(10000);
    Close.waitForFinished(10000);
    //========
    emit Send_TextBrowser_NewMessage(tr("Force retry."));
    return;
}
//激活强制重试按钮
void MainWindow::SetEnable_pushButton_ForceRetry_self()
{
    ui->pushButton_ForceRetry->setEnabled(1);
    return;
}
void MainWindow::on_pushButton_PayPal_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.paypal.me/aaronfeng753"));
}
void MainWindow::on_checkBox_AudioDenoise_stateChanged(int arg1)
{
    if(ui->checkBox_AudioDenoise->isChecked())
    {
        ui->doubleSpinBox_AudioDenoiseLevel->setEnabled(1);
    }
    else
    {
        ui->doubleSpinBox_AudioDenoiseLevel->setEnabled(0);
    }
}
void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch(ui->tabWidget->currentIndex())
    {
        case 0:
            {
                //tab 0
                ui->label_DonateQRCode->setVisible(1);
                ui->pushButton_PayPal->setVisible(1);
                ui->pushButton_Patreon->setVisible(1);
                ui->label_DonateText->setVisible(1);
                //tab 1
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                //tab 2
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                //tab 3
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                //tab 4
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                //tab 5
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 1:
            {
                //tab 0
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                //tab 1
                ui->groupBox_Progress->setVisible(1);
                ui->splitter_2->setVisible(1);
                if(isSettingsHide==false)
                {
                    ui->groupBox_Setting->setVisible(1);
                }
                //tab 2
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                //tab 3
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                //tab 4
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                //tab 5
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 2:
            {
                //tab 0
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                //tab 1
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                //tab 2
                ui->groupBox_Engine->setVisible(1);
                ui->groupBox_NumOfThreads->setVisible(1);
                //tab 3
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                //tab 4
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                //tab 5
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 3:
            {
                //tab 0
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                //tab 1
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                //tab 2
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                //tab 3
                ui->groupBox_AudioDenoise->setVisible(1);
                ui->groupBox_video_settings->setVisible(1);
                ui->groupBox_FrameInterpolation->setVisible(1);
                //tab 4
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                //tab 5
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 4:
            {
                //tab 0
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                //tab 1
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                //tab 2
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                //tab 3
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                //tab 4
                ui->groupBox_3->setVisible(1);
                ui->groupBox_8->setVisible(1);
                ui->groupBox_InputExt->setVisible(1);
                ui->groupBox_other_1->setVisible(1);
                //tab 5
                ui->groupBox_CompatibilityTestRes->setVisible(0);
                ui->pushButton_compatibilityTest->setVisible(0);
                break;
            }
        case 5:
            {
                //tab 0
                ui->label_DonateQRCode->setVisible(0);
                ui->pushButton_PayPal->setVisible(0);
                ui->pushButton_Patreon->setVisible(0);
                ui->label_DonateText->setVisible(0);
                //tab 1
                ui->groupBox_Progress->setVisible(0);
                ui->splitter_2->setVisible(0);
                //tab 2
                ui->groupBox_Engine->setVisible(0);
                ui->groupBox_NumOfThreads->setVisible(0);
                //tab 3
                ui->groupBox_AudioDenoise->setVisible(0);
                ui->groupBox_video_settings->setVisible(0);
                ui->groupBox_FrameInterpolation->setVisible(0);
                //tab 4
                ui->groupBox_3->setVisible(0);
                ui->groupBox_8->setVisible(0);
                ui->groupBox_InputExt->setVisible(0);
                ui->groupBox_other_1->setVisible(0);
                //tab 5
                ui->groupBox_CompatibilityTestRes->setVisible(1);
                ui->pushButton_compatibilityTest->setVisible(1);
                break;
            }
    }
}
void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int arg1)
{
    if(ui->checkBox_ProcessVideoBySegment->isChecked())
    {
        ui->label_SegmentDuration->setEnabled(1);
        ui->spinBox_SegmentDuration->setEnabled(1);
    }
    else
    {
        ui->label_SegmentDuration->setEnabled(0);
        ui->spinBox_SegmentDuration->setEnabled(0);
    }
}
void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index)
{
    switch (ui->comboBox_version_Waifu2xNCNNVulkan->currentIndex())
    {
        case 0:
            {
                Waifu2x_ncnn_vulkan_FolderPath = Current_Path + "/waifu2x-ncnn-vulkan";
                Waifu2x_ncnn_vulkan_ProgramPath = Waifu2x_ncnn_vulkan_FolderPath + "/waifu2x-ncnn-vulkan_waifu2xEX.exe";
                ui->checkBox_TTA_vulkan->setEnabled(1);
                return;
            }
        case 1:
            {
                Waifu2x_ncnn_vulkan_FolderPath = Current_Path + "/waifu2x-ncnn-vulkan";
                Waifu2x_ncnn_vulkan_ProgramPath = Waifu2x_ncnn_vulkan_FolderPath + "/waifu2x-ncnn-vulkan-fp16p_waifu2xEX.exe";
                ui->checkBox_TTA_vulkan->setEnabled(1);
                return;
            }
        case 2:
            {
                Waifu2x_ncnn_vulkan_FolderPath = Current_Path + "/waifu2x-ncnn-vulkan-old";
                Waifu2x_ncnn_vulkan_ProgramPath = Waifu2x_ncnn_vulkan_FolderPath + "/waifu2x-ncnn-vulkan_waifu2xEX.exe";
                ui->checkBox_TTA_vulkan->setEnabled(0);
                ui->checkBox_TTA_vulkan->setChecked(0);
                return;
            }
    }
}
void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1)
{
    if(ui->checkBox_EnablePreProcessing_Anime4k->isChecked())
    {
        ui->checkBox_MedianBlur_Pre_Anime4k->setEnabled(1);
        ui->checkBox_MeanBlur_Pre_Anime4k->setEnabled(1);
        ui->checkBox_CASSharping_Pre_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlurWeak_Pre_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlur_Pre_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilter_Pre_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilterFaster_Pre_Anime4k->setEnabled(1);
    }
    else
    {
        ui->checkBox_MedianBlur_Pre_Anime4k->setEnabled(0);
        ui->checkBox_MeanBlur_Pre_Anime4k->setEnabled(0);
        ui->checkBox_CASSharping_Pre_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlurWeak_Pre_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlur_Pre_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilter_Pre_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilterFaster_Pre_Anime4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1)
{
    if(ui->checkBox_EnablePostProcessing_Anime4k->isChecked())
    {
        ui->checkBox_MedianBlur_Post_Anime4k->setEnabled(1);
        ui->checkBox_MeanBlur_Post_Anime4k->setEnabled(1);
        ui->checkBox_CASSharping_Post_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlurWeak_Post_Anime4k->setEnabled(1);
        ui->checkBox_GaussianBlur_Post_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilter_Post_Anime4k->setEnabled(1);
        ui->checkBox_BilateralFilterFaster_Post_Anime4k->setEnabled(1);
    }
    else
    {
        ui->checkBox_MedianBlur_Post_Anime4k->setEnabled(0);
        ui->checkBox_MeanBlur_Post_Anime4k->setEnabled(0);
        ui->checkBox_CASSharping_Post_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlurWeak_Post_Anime4k->setEnabled(0);
        ui->checkBox_GaussianBlur_Post_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilter_Post_Anime4k->setEnabled(0);
        ui->checkBox_BilateralFilterFaster_Post_Anime4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1)
{
    if(ui->checkBox_SpecifyGPU_Anime4k->isChecked())
    {
        ui->lineEdit_GPUs_Anime4k->setEnabled(1);
        ui->pushButton_ListGPUs_Anime4k->setEnabled(1);
        ui->pushButton_VerifyGPUsConfig_Anime4k->setEnabled(1);
    }
    else
    {
        ui->lineEdit_GPUs_Anime4k->setEnabled(0);
        ui->pushButton_ListGPUs_Anime4k->setEnabled(0);
        ui->pushButton_VerifyGPUsConfig_Anime4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD->setChecked(isCompatible_Waifu2x_NCNN_Vulkan_OLD);
}
void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked()
{
    ui->checkBox_isCompatible_SRMD_NCNN_Vulkan->setChecked(isCompatible_SRMD_NCNN_Vulkan);
}
void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked()
{
    ui->checkBox_isCompatible_SRMD_CUDA->setChecked(isCompatible_SRMD_CUDA);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Converter->setChecked(isCompatible_Waifu2x_Converter);
}
void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked()
{
    ui->checkBox_isCompatible_Anime4k_CPU->setChecked(isCompatible_Anime4k_CPU);
}
void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked()
{
    ui->checkBox_isCompatible_Anime4k_GPU->setChecked(isCompatible_Anime4k_GPU);
}
void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked()
{
    ui->checkBox_isCompatible_FFmpeg->setChecked(isCompatible_FFmpeg);
}
void MainWindow::on_checkBox_isCompatible_FFprobe_clicked()
{
    ui->checkBox_isCompatible_FFprobe->setChecked(isCompatible_FFprobe);
}
void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked()
{
    ui->checkBox_isCompatible_ImageMagick->setChecked(isCompatible_ImageMagick);
}
void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked()
{
    ui->checkBox_isCompatible_Gifsicle->setChecked(isCompatible_Gifsicle);
}
void MainWindow::on_checkBox_isCompatible_SoX_clicked()
{
    ui->checkBox_isCompatible_SoX->setChecked(isCompatible_SoX);
}
void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int arg1)
{
    if(ui->checkBox_GPUMode_Anime4K->isChecked())
    {
        ui->checkBox_SpecifyGPU_Anime4k->setEnabled(1);
        ui->comboBox_GPGPUModel_A4k->setEnabled(1);
        on_comboBox_GPGPUModel_A4k_currentIndexChanged(1);
    }
    else
    {
        ui->checkBox_SpecifyGPU_Anime4k->setEnabled(0);
        ui->checkBox_SpecifyGPU_Anime4k->setChecked(0);
        ui->comboBox_GPGPUModel_A4k->setEnabled(0);
        on_comboBox_GPGPUModel_A4k_currentIndexChanged(1);
    }
}
void MainWindow::on_checkBox_ShowInterPro_stateChanged(int arg1)
{
    if(ui->checkBox_ShowInterPro->isChecked()==false)
    {
        emit Send_CurrentFileProgress_Stop();
    }
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Caffe_CPU->setChecked(isCompatible_Waifu2x_Caffe_CPU);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Caffe_GPU->setChecked(isCompatible_Waifu2x_Caffe_GPU);
}
void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked()
{
    ui->checkBox_isCompatible_Waifu2x_Caffe_cuDNN->setChecked(isCompatible_Waifu2x_Caffe_cuDNN);
}
void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked()
{
    int VAL = ui->spinBox_SplitSize_Waifu2xCaffe->value()*2;
    if(VAL<=999999999)
    {
        ui->spinBox_SplitSize_Waifu2xCaffe->setValue(VAL);
    }
}
void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked()
{
    int VAL = ui->spinBox_SplitSize_Waifu2xCaffe->value()/2;
    if(VAL>=2)
    {
        ui->spinBox_SplitSize_Waifu2xCaffe->setValue(VAL);
    }
}
void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked()
{
    ui->checkBox_isCompatible_Realsr_NCNN_Vulkan->setChecked(isCompatible_Realsr_NCNN_Vulkan);
}
void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int arg1)
{
    if(ui->checkBox_ACNet_Anime4K->isChecked())
    {
        ui->checkBox_HDNMode_Anime4k->setEnabled(1);
        ui->groupBox_PostProcessing_Anime4k->setEnabled(0);
        ui->groupBox_PreProcessing_Anime4k->setEnabled(0);
        ui->doubleSpinBox_PushColorStrength_Anime4K->setEnabled(0);
        ui->doubleSpinBox_PushGradientStrength_Anime4K->setEnabled(0);
        ui->spinBox_Passes_Anime4K->setEnabled(0);
        ui->spinBox_PushColorCount_Anime4K->setEnabled(0);
    }
    else
    {
        ui->checkBox_HDNMode_Anime4k->setEnabled(0);
        on_checkBox_GPUMode_Anime4K_stateChanged(0);
        on_checkBox_SpecifyGPU_Anime4k_stateChanged(0);
        ui->groupBox_PostProcessing_Anime4k->setEnabled(1);
        ui->groupBox_PreProcessing_Anime4k->setEnabled(1);
        ui->doubleSpinBox_PushColorStrength_Anime4K->setEnabled(1);
        ui->doubleSpinBox_PushGradientStrength_Anime4K->setEnabled(1);
        ui->spinBox_Passes_Anime4K->setEnabled(1);
        ui->spinBox_PushColorCount_Anime4K->setEnabled(1);
    }
    DenoiseLevelSpinboxSetting_Anime4k();
}
void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int arg1)
{
    DenoiseLevelSpinboxSetting_Anime4k();
}
/*
生成独立cmd文件来执行cmd命令
*/
void MainWindow::ExecuteCMD_batFile(QString cmd_str,bool requestAdmin)
{
    ExecuteCMD_batFile_QMutex.lock();
    QString cmd_commands = "@echo off\n "+cmd_str+"\n exit";
    Delay_msec_sleep(10);//延时防止文件名称碰撞
    file_mkDir(Current_Path+"/batFiles_tmp");
    QString Bat_path = Current_Path+"/batFiles_tmp/W2xEX_"+QDateTime::currentDateTime().toString("dhhmmsszzz")+".bat";
    //========
    QFile OpenFile_cmdFile(Bat_path);
    OpenFile_cmdFile.remove();
    if (OpenFile_cmdFile.open(QIODevice::ReadWrite | QIODevice::Text)) //QIODevice::ReadWrite支持读写
    {
        QTextStream stream(&OpenFile_cmdFile);
        stream << cmd_commands;
    }
    OpenFile_cmdFile.close();
    if(requestAdmin)
    {
        ShellExecuteW(NULL, QString("runas").toStdWString().c_str(), QString(Bat_path).toStdWString().c_str(), QString(Bat_path).toStdWString().c_str(), NULL, 1);
    }
    else
    {
        QDesktopServices::openUrl(QUrl("file:"+QUrl::toPercentEncoding(Bat_path)));
    }
    //========
    ExecuteCMD_batFile_QMutex.unlock();
}
void MainWindow::Del_TempBatFile()
{
    file_DelDir(Current_Path+"/batFiles_tmp");
}
void MainWindow::comboBox_UpdateChannel_setCurrentIndex_self(int index)
{
    comboBox_UpdateChannel_setCurrentIndex_self_QMutex.lock();
    isClicked_comboBox_UpdateChannel=false;
    ui->comboBox_UpdateChannel->setCurrentIndex(index);
    isClicked_comboBox_UpdateChannel=true;
    comboBox_UpdateChannel_setCurrentIndex_self_QMutex.unlock();
}
void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int index)
{
    if(isClicked_comboBox_UpdateChannel && AutoUpdate.isRunning()==false)
    {
        AutoUpdate = QtConcurrent::run(this, &MainWindow::CheckUpadte_Auto);//自动检查更新线程
    }
}
void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int arg1)
{
    if(ui->checkBox_ReplaceOriginalFile->isChecked())
    {
        ui->groupBox_OutPut->setEnabled(0);
        ui->checkBox_OutPath_isEnabled->setChecked(0);
        ui->checkBox_DelOriginal->setEnabled(0);
    }
    else
    {
        ui->groupBox_OutPut->setEnabled(1);
        ui->checkBox_DelOriginal->setEnabled(1);
    }
}
void MainWindow::checkBox_ReplaceOriginalFile_setEnabled_True_Self()
{
    if(ui->checkBox_DelOriginal->isChecked()==false && ui->checkBox_OutPath_isEnabled->isChecked()==false)
    {
        ui->checkBox_ReplaceOriginalFile->setEnabled(1);
    }
}
bool MainWindow::ReplaceOriginalFile(QString original_fullpath,QString output_fullpath)
{
    //检查是否启用替换源文件,以及输出文件是否存在
    if(ui->checkBox_ReplaceOriginalFile->isChecked()==false || QFile::exists(output_fullpath)==false)return false;
    //=================
    QFileInfo fileinfo_original_fullpath(original_fullpath);
    QFileInfo fileinfo_output_fullpath(output_fullpath);
    QString file_name = file_getBaseName(original_fullpath);//获取源文件的文件名
    QString file_ext = fileinfo_output_fullpath.suffix();//获取输出文件的后辍
    QString file_path = file_getFolderPath(fileinfo_original_fullpath);//获取源文件的文件路径
    //=================
    QString Target_fullpath=file_path+"/"+file_name+"."+file_ext;
    //=================
    if(QAction_checkBox_MoveToRecycleBin_checkBox_ReplaceOriginalFile->isChecked())
    {
        file_MoveToTrash(original_fullpath);
        file_MoveToTrash(Target_fullpath);
    }
    else
    {
        QFile::remove(original_fullpath);
        QFile::remove(Target_fullpath);
    }
    //=================
    if(QFile::rename(output_fullpath,Target_fullpath)==false)
    {
        emit Send_TextBrowser_NewMessage(tr("Error! Failed to move [")+output_fullpath+tr("] to [")+Target_fullpath+"]");
    }
    return true;
}
void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int arg1)
{
    if(ui->checkBox_isCustFontEnable->isChecked())
    {
        ui->pushButton_Save_GlobalFontSize->setEnabled(1);
        ui->spinBox_GlobalFontSize->setEnabled(1);
        ui->fontComboBox_CustFont->setEnabled(1);
    }
    else
    {
        ui->pushButton_Save_GlobalFontSize->setEnabled(0);
        ui->spinBox_GlobalFontSize->setEnabled(0);
        ui->fontComboBox_CustFont->setEnabled(0);
    }
}
void MainWindow::OutputSettingsArea_setEnabled(bool isEnabled)
{
    ui->scrollArea_outputPathSettings->setEnabled(isEnabled);
    ui->lineEdit_outputPath->setClearButtonEnabled(isEnabled);
    if(isEnabled==true)
    {
        ui->lineEdit_outputPath->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        ui->lineEdit_outputPath->setFocusPolicy(Qt::NoFocus);
    }
}
//事件过滤器
bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
    //=============================
    //按下 Delete 移除文件列表里的文件
    //=============================
    if (target == ui->tableView_image || target == ui->tableView_gif || target == ui->tableView_video)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Delete)
            {
                if(RemoveFile_FilesList_QAction_FileList->isEnabled())
                {
                    on_pushButton_RemoveItem_clicked();
                }
                return true;
            }
            if ((keyEvent->modifiers() == Qt::ControlModifier) && (keyEvent->key() == Qt::Key_A))
            {
                if(Apply_CustRes_QAction_FileList->isEnabled())
                {
                    EnableApply2All_CustRes=false;
                    on_pushButton_CustRes_apply_clicked();
                    EnableApply2All_CustRes=true;
                }
                return true;
            }
            if ((keyEvent->modifiers() == Qt::ControlModifier) && (keyEvent->key() == Qt::Key_C))
            {
                if(Cancel_CustRes_QAction_FileList->isEnabled())
                {
                    EnableApply2All_CustRes=false;
                    on_pushButton_CustRes_cancel_clicked();
                    EnableApply2All_CustRes=true;
                }
                return true;
            }
        }
    }
    //==============
    return false;
}
void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked()
{
    ui->splitter_FilesList->setSizes(QList<int>() << 1 << 1 << 1);
}
void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int index)
{
    if(ui->comboBox_GPGPUModel_A4k->currentText().toLower().trimmed()=="opencl" && ui->checkBox_GPUMode_Anime4K->isChecked())
    {
        ui->spinBox_OpenCLCommandQueues_A4k->setEnabled(1);
        ui->checkBox_OpenCLParallelIO_A4k->setEnabled(1);
    }
    else
    {
        ui->spinBox_OpenCLCommandQueues_A4k->setEnabled(0);
        ui->checkBox_OpenCLParallelIO_A4k->setEnabled(0);
    }
}
void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int arg1)
{
    if(ui->checkBox_DisableGPU_converter->isChecked())
    {
        ui->checkBox_MultiGPU_Waifu2xConverter->setChecked(0);
        ui->comboBox_TargetProcessor_converter->setEnabled(0);
        ui->comboBox_TargetProcessor_converter->setCurrentIndex(0);
        ui->checkBox_MultiGPU_Waifu2xConverter->setEnabled(0);
    }
    else
    {
        ui->comboBox_TargetProcessor_converter->setEnabled(1);
        ui->checkBox_MultiGPU_Waifu2xConverter->setEnabled(1);
    }
}
void MainWindow::on_groupBox_video_settings_clicked()
{
    if(ui->groupBox_video_settings->isChecked())
    {
        //======
        if(isCustomVideoSettingsClicked==true)
        {
            QMessageBox *MSG = new QMessageBox();
            MSG->setWindowTitle(tr("Warning"));
            MSG->setText(tr("Change Custom video settings might cause ERROR.\n\nMake sure you know what you are doing before change any settings."));
            MSG->setIcon(QMessageBox::Warning);
            MSG->setModal(true);
            MSG->show();
        }
        //======
        ui->groupBox_OutputVideoSettings->setEnabled(1);
        ui->groupBox_ToMp4VideoSettings->setEnabled(1);
        ui->pushButton_encodersList->setEnabled(1);
        ui->pushButton_ResetVideoSettings->setEnabled(1);
    }
    else
    {
        ui->groupBox_OutputVideoSettings->setEnabled(0);
        ui->groupBox_ToMp4VideoSettings->setEnabled(0);
        ui->pushButton_encodersList->setEnabled(0);
        ui->pushButton_ResetVideoSettings->setEnabled(0);
    }
}
void MainWindow::Set_checkBox_DisableResize_gif_Checked()
{
    emit Send_TextBrowser_NewMessage(tr("[Disable \"-resize\"] is automatically enabled to fix compatibility issue and improve performance."));
    ui->checkBox_DisableResize_gif->setChecked(true);
}

void MainWindow::on_pushButton_TurnOffScreen_clicked()
{
    if(TurnOffScreen_QF.isRunning() == true)return;
    TurnOffScreen_QF = QtConcurrent::run(this, &MainWindow::TurnOffScreen);//关闭显示器
}
/*
调用nircmd关闭显示器
*/
void MainWindow::TurnOffScreen()
{
    QProcess *OffScreen = new QProcess();
    OffScreen->start("\""+Current_Path+"/nircmd-x64/nircmd.exe\" monitor off");
    OffScreen->waitForStarted(5000);
    OffScreen->waitForFinished(5000);
    OffScreen->kill();
    return;
}
/*
禁用&启用 开始按钮
*/
void MainWindow::pushButton_Start_setEnabled_self(bool isEnabled)
{
    ui->pushButton_Start->setEnabled(isEnabled);
    Start_SystemTrayIcon->setEnabled(isEnabled);
    ui->pushButton_Start->setVisible(isEnabled);
}
/*
禁用&启用 暂停按钮
*/
void MainWindow::pushButton_Stop_setEnabled_self(bool isEnabled)
{
    ui->pushButton_Stop->setEnabled(isEnabled);
    Pause_SystemTrayIcon->setEnabled(isEnabled);
    ui->pushButton_Stop->setVisible(isEnabled);
}


void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked()
{
    int VAL = 2;
    if(ui->comboBox_Engine_VFI->currentIndex()==2)
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()-1;
    }
    else
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()/2;
    }
    if(VAL>=2)
    {
        ui->spinBox_MultipleOfFPS_VFI->setValue(VAL);
    }
}

// RealCUGAN Tile Size Adjustments
void MainWindow::on_pushButton_TileSize_Add_RealCUGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealCUGAN) return;
    ui->spinBox_TileSize_RealCUGAN->setValue(AddTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealCUGAN->value()));
}

void MainWindow::on_pushButton_TileSize_Minus_RealCUGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealCUGAN) return;
    ui->spinBox_TileSize_RealCUGAN->setValue(MinusTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealCUGAN->value()));
}

// RealCUGAN Model Change
void MainWindow::on_comboBox_Model_RealCUGAN_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(!ui->comboBox_Model_RealCUGAN) return;

    // Optionally, trigger a re-read or save of this specific setting if needed immediately
    // For now, we assume Realcugan_NCNN_Vulkan_ReadSettings() is called before processing,
    // which will pick up the current value.
    // If there are model-specific UI changes (e.g., enabling/disabling denoise levels),
    // that logic would go here.
    qDebug() << "RealCUGAN model changed to:" << ui->comboBox_Model_RealCUGAN->currentText();

    // Update the member variable immediately
    Realcugan_NCNN_Vulkan_ReadSettings();

    // Example: Auto-save this setting (if not handled by a global save button)
    // This is good practice if no explicit "Save Settings" button is pressed often.
    // However, the application has a "Save Settings" button and auto-save on exit.
    // For consistency, individual changes like this might not need to auto-save immediately
    // unless it affects other dynamic UI elements that depend on this value.
    /*
    QSettings settings("Waifu2x-Extension-QT", "Waifu2x-Extension-QT");
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");
    settings.setValue("Model", ui->comboBox_Model_RealCUGAN->currentText());
    settings.endGroup();
    */
}


void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked()
{
    int VAL = 2;
    if(ui->comboBox_Engine_VFI->currentIndex()==2)
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()+1;
    }
    else
    {
        VAL = ui->spinBox_MultipleOfFPS_VFI->value()*2;
    }
    if(VAL<=999999999)
    {
        ui->spinBox_MultipleOfFPS_VFI->setValue(VAL);
    }
}

// =================================================================================================
//                                      PRELOAD ENGINES SETTINGS
// =================================================================================================
void MainWindow::PreLoad_Engines_Settings()
{
    //PreLoad Waifu2x-NCNN-Vulkan Settings
    Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_vulkan->count() <= 0) Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str += tr("Waifu2x model list is empty.") + "\n";
    if(ui->comboBox_GPUID_vulkan->count() <= 0) Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str += tr("Waifu2x GPU list is empty.") + "\n";
    if(Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str != "") Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str = tr("Waifu2x-ncnn-Vulkan Preload Failed:") + "\n" + Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str;

    //PreLoad Waifu2x-Converter-CPP Settings
    Waifu2x_Converter_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_converter->count() <= 0) Waifu2x_Converter_PreLoad_Settings_Str += tr("Waifu2x model list is empty.") + "\n";
    if(ui->comboBox_TargetProcessor_converter->count() <= 0) Waifu2x_Converter_PreLoad_Settings_Str += tr("Waifu2x GPU list is empty.") + "\n";
    if(Waifu2x_Converter_PreLoad_Settings_Str != "") Waifu2x_Converter_PreLoad_Settings_Str = tr("Waifu2x-Converter-CPP Preload Failed:") + "\n" + Waifu2x_Converter_PreLoad_Settings_Str;

    //PreLoad SRMD-NCNN-Vulkan Settings
    SRMD_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_srmd->count() <= 0) SRMD_NCNN_Vulkan_PreLoad_Settings_Str += tr("SRMD model list is empty.") + "\n";
    if(ui->comboBox_GPUID_srmd->count() <= 0) SRMD_NCNN_Vulkan_PreLoad_Settings_Str += tr("SRMD GPU list is empty.") + "\n";
    if(SRMD_NCNN_Vulkan_PreLoad_Settings_Str != "") SRMD_NCNN_Vulkan_PreLoad_Settings_Str = tr("SRMD-ncnn-Vulkan Preload Failed:") + "\n" + SRMD_NCNN_Vulkan_PreLoad_Settings_Str;

    //PreLoad SRMD-CUDA Settings
    SRMD_CUDA_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_srmd_cuda->count() <= 0) SRMD_CUDA_PreLoad_Settings_Str += tr("SRMD model list is empty.") + "\n";
    if(ui->comboBox_GPUID_srmd_cuda->count() <= 0) SRMD_CUDA_PreLoad_Settings_Str += tr("SRMD GPU list is empty.") + "\n";
    if(SRMD_CUDA_PreLoad_Settings_Str != "") SRMD_CUDA_PreLoad_Settings_Str = tr("SRMD-CUDA Preload Failed:") + "\n" + SRMD_CUDA_PreLoad_Settings_Str;

    //PreLoad Anime4KCPP Settings
    Anime4KCPP_PreLoad_Settings_Str = "";
    if(ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_PlatformID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Platform ID list is empty.") + "\n";
    if(ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_DeviceID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Device ID list is empty.") + "\n";
    if(Anime4KCPP_PreLoad_Settings_Str != "") Anime4KCPP_PreLoad_Settings_Str = tr("Anime4KCPP Preload Failed:") + "\n" + Anime4KCPP_PreLoad_Settings_Str;

    //PreLoad Waifu2x-Caffe Settings
    Waifu2x_Caffe_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_caffe->count() <= 0) Waifu2x_Caffe_PreLoad_Settings_Str += tr("Waifu2x model list is empty.") + "\n";
    if(ui->comboBox_GPUID_caffe->count() <= 0) Waifu2x_Caffe_PreLoad_Settings_Str += tr("Waifu2x GPU list is empty.") + "\n";
    if(Waifu2x_Caffe_PreLoad_Settings_Str != "") Waifu2x_Caffe_PreLoad_Settings_Str = tr("Waifu2x-Caffe Preload Failed:") + "\n" + Waifu2x_Caffe_PreLoad_Settings_Str;

    //PreLoad RealSR-NCNN-Vulkan Settings
    Realsr_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_realsr->count() <= 0) Realsr_NCNN_Vulkan_PreLoad_Settings_Str += tr("Realsr model list is empty.") + "\n";
    if(ui->comboBox_GPUID_realsr->count() <= 0) Realsr_NCNN_Vulkan_PreLoad_Settings_Str += tr("Realsr GPU list is empty.") + "\n";
    if(Realsr_NCNN_Vulkan_PreLoad_Settings_Str != "") Realsr_NCNN_Vulkan_PreLoad_Settings_Str = tr("Realsr-ncnn-Vulkan Preload Failed:") + "\n" + Realsr_NCNN_Vulkan_PreLoad_Settings_Str;

    //PreLoad RealCUGAN-NCNN-Vulkan Settings
    Realcugan_NCNN_Vulkan_PreLoad_Settings_Str = "";
    // Check if UI elements are valid before accessing them
    if (ui->comboBox_Model_RealCUGAN && ui->comboBox_GPUID_RealCUGAN) {
        if(ui->comboBox_Model_RealCUGAN->count() <= 0) Realcugan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealCUGAN model list is empty.") + "\n";
        if(ui->comboBox_GPUID_RealCUGAN->count() <= 0) Realcugan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealCUGAN GPU list is empty.") + "\n";
    } else {
        Realcugan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealCUGAN UI elements not found.") + "\n";
    }
    if(Realcugan_NCNN_Vulkan_PreLoad_Settings_Str != "") Realcugan_NCNN_Vulkan_PreLoad_Settings_Str = tr("RealCUGAN-ncnn-Vulkan Preload Failed:") + "\n" + Realcugan_NCNN_Vulkan_PreLoad_Settings_Str;

    //PreLoad RealESRGAN-NCNN-Vulkan Settings
    Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if (ui->comboBox_Model_RealESRGAN && ui->comboBox_GPUID_RealESRGAN) {
        if(ui->comboBox_Model_RealESRGAN->count() <= 0) Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealESRGAN model list is empty.") + "\n";
        if(ui->comboBox_GPUID_RealESRGAN->count() <= 0) Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealESRGAN GPU list is empty.") + "\n";
    } else {
        Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str += tr("RealESRGAN UI elements not found.") + "\n";
    }
    if(Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str != "") Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str = tr("RealESRGAN-ncnn-Vulkan Preload Failed:") + "\n" + Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str;

    //PreLoad FrameInterpolation Engines Settings
    Rife_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_rife->count() <= 0) Rife_NCNN_Vulkan_PreLoad_Settings_Str += tr("RIFE model list is empty.") + "\n";
    if(ui->comboBox_GPUID_rife->count() <= 0) Rife_NCNN_Vulkan_PreLoad_Settings_Str += tr("RIFE GPU list is empty.") + "\n";
    if(Rife_NCNN_Vulkan_PreLoad_Settings_Str != "") Rife_NCNN_Vulkan_PreLoad_Settings_Str = tr("RIFE-ncnn-Vulkan Preload Failed:") + "\n" + Rife_NCNN_Vulkan_PreLoad_Settings_Str;

    Cain_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_cain->count() <= 0) Cain_NCNN_Vulkan_PreLoad_Settings_Str += tr("CAIN model list is empty.") + "\n";
    if(ui->comboBox_GPUID_cain->count() <= 0) Cain_NCNN_Vulkan_PreLoad_Settings_Str += tr("CAIN GPU list is empty.") + "\n";
    if(Cain_NCNN_Vulkan_PreLoad_Settings_Str != "") Cain_NCNN_Vulkan_PreLoad_Settings_Str = tr("CAIN-ncnn-Vulkan Preload Failed:") + "\n" + Cain_NCNN_Vulkan_PreLoad_Settings_Str;

    Dain_NCNN_Vulkan_PreLoad_Settings_Str = "";
    if(ui->comboBox_model_dain->count() <= 0) Dain_NCNN_Vulkan_PreLoad_Settings_Str += tr("DAIN model list is empty.") + "\n";
    if(ui->comboBox_GPUID_dain->count() <= 0) Dain_NCNN_Vulkan_PreLoad_Settings_Str += tr("DAIN GPU list is empty.") + "\n";
    if(Dain_NCNN_Vulkan_PreLoad_Settings_Str != "") Dain_NCNN_Vulkan_PreLoad_Settings_Str = tr("DAIN-ncnn-Vulkan Preload Failed:") + "\n" + Dain_NCNN_Vulkan_PreLoad_Settings_Str;
}


// =================================================================================================
//                                      WAIFU2X MAIN FUNCTION
// =================================================================================================
void MainWindow::Waifu2x()
{
    // ... (rest of Waifu2x, a very long function) ...
}

// =================================================================================================
//                                      CHECK PRELOAD SETTINGS
// =================================================================================================
bool MainWindow::Check_PreLoad_Settings()
{
    bool isPreLoadError = false;
    QString ErrorMsg_PreLoad = "";
    int Engine = 0; // 0:waifu2x-ncnn-vulkan, 1:waifu2x-converter, 2:srmd-ncnn-vulkan, 3:Anime4K, 4:waifu2x-caffe, 5:realsr-ncnn-vulkan, 6:srmd-cuda, 7:RealCUGAN, 8:RealESRGAN
    if(ui->tabWidget_FileType->currentIndex()==0) Engine = ui->comboBox_Engine_Image->currentIndex(); //Image
    if(ui->tabWidget_FileType->currentIndex()==1) Engine = ui->comboBox_Engine_GIF->currentIndex();   //GIF
    if(ui->tabWidget_FileType->currentIndex()==2) Engine = ui->comboBox_Engine_Video->currentIndex(); //Video
    if(ui->tabWidget_FileType->currentIndex()==3) Engine = ui->comboBox_Engine_VFI->currentIndex(); //Video Frame Interpolation

    if(Engine==0 && (ui->tabWidget_FileType->currentIndex()!=3)) //Waifu2x-ncnn-Vulkan
    {
        if(Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Waifu2x_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==1 && (ui->tabWidget_FileType->currentIndex()!=3)) //Waifu2x-Converter-CPP
    {
        if(Waifu2x_Converter_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Waifu2x_Converter_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==2 && (ui->tabWidget_FileType->currentIndex()!=3)) //SRMD-ncnn-Vulkan
    {
        if(SRMD_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = SRMD_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==3 && (ui->tabWidget_FileType->currentIndex()!=3)) //Anime4KCPP
    {
        if(Anime4KCPP_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Anime4KCPP_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==4 && (ui->tabWidget_FileType->currentIndex()!=3)) //Waifu2x-Caffe
    {
        if(Waifu2x_Caffe_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Waifu2x_Caffe_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==5 && (ui->tabWidget_FileType->currentIndex()!=3)) //RealSR-ncnn-Vulkan
    {
        if(Realsr_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Realsr_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==6 && (ui->tabWidget_FileType->currentIndex()!=3)) //SRMD-CUDA
    {
        if(SRMD_CUDA_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = SRMD_CUDA_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==7) //RealCUGAN-ncnn-Vulkan
    {
        if(Realcugan_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Realcugan_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==8) //RealESRGAN-ncnn-Vulkan
    {
        if(Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    //FrameInterpolation Engines
    else if(Engine==0 && (ui->tabWidget_FileType->currentIndex()==3)) // RIFE CHECK
    {
        if(Rife_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Rife_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==1 && (ui->tabWidget_FileType->currentIndex()==3)) // CAIN CHECK
    {
        if(Cain_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Cain_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }
    else if(Engine==2 && (ui->tabWidget_FileType->currentIndex()==3)) // DAIN CHECK
    {
        if(Dain_NCNN_Vulkan_PreLoad_Settings_Str != "")
        {
            ErrorMsg_PreLoad = Dain_NCNN_Vulkan_PreLoad_Settings_Str;
            isPreLoadError = true;
        }
    }

    if(isPreLoadError)
    {
        QMessageBox *MSG_PreLoadError = new QMessageBox();
        MSG_PreLoadError->setWindowTitle(tr("Error"));
        MSG_PreLoadError->setText(ErrorMsg_PreLoad + "\n" + tr("Please check your settings."));
        MSG_PreLoadError->setIcon(QMessageBox::Warning);
        MSG_PreLoadError->setModal(true);
        MSG_PreLoadError->show();
        return false;
    }
    return true;
}


// =================================================================================================
//                                      APNG MAIN FUNCTION
// =================================================================================================
void MainWindow::APNG_Main(QString splitFramesFolder, QString scaledFramesFolder, QString sourceFileFullPath, QStringList framesFileName_qStrList, QString resultFileFullPath)
{
    int Engine = 0; // 0:waifu2x-ncnn-vulkan, 1:waifu2x-converter, 2:srmd-ncnn-vulkan, 3:Anime4K, 4:waifu2x-caffe, 5:realsr-ncnn-vulkan, 6:srmd-cuda, 7:RealCUGAN, 8:RealESRGAN
    if(ui->tabWidget_FileType->currentIndex()==0) Engine = ui->comboBox_Engine_Image->currentIndex(); //Image (APNG treated as image)
    if(ui->tabWidget_FileType->currentIndex()==1) Engine = ui->comboBox_Engine_GIF->currentIndex();   //GIF

    switch(Engine)
    {
        case 0:
        {
            APNG_Waifu2xNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 1:
        {
            APNG_Waifu2xConverter(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 2:
        {
            APNG_SRMDNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 3:
        {
            APNG_Anime4K(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 4:
        {
            APNG_Waifu2xCaffe(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 5:
        {
            APNG_RealSRNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 6: // SRMD CUDA
        {
            APNG_SRMDCuda(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 7: // RealCUGAN
        {
            APNG_RealcuganNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        case 8: // RealESRGAN
        {
            APNG_RealESRGANNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath);
            break;
        }
        // Note: Case 9 for next engine will be added here in a subsequent step.
    }
}
// ... (The rest of the file continues from here) ...

void MainWindow::on_pushButton_Patreon_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.patreon.com/AaronFeng"));
}

void MainWindow::on_pushButton_afdian_clicked()
{
    QDesktopServices::openUrl(QUrl("https://afdian.net/@AaronFeng"));
}

void MainWindow::on_pushButton_qqPay_clicked()
{
    ui->label_DonateQRCode->setPixmap(QPixmap(":/new/prefix1/icon/qqpay.webp"));
}

void MainWindow::on_pushButton_aliPay_clicked()
{
    ui->label_DonateQRCode->setPixmap(QPixmap(":/new/prefix1/icon/alipay.webp"));
}

void MainWindow::on_pushButton_wechatPay_clicked()
{
    ui->label_DonateQRCode->setPixmap(QPixmap(":/new/prefix1/icon/wechatpay.webp"));
}

// RealESRGAN Tile Size Adjustments
void MainWindow::on_pushButton_TileSize_Add_RealESRGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealESRGAN) return;
    ui->spinBox_TileSize_RealESRGAN->setValue(AddTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealESRGAN->value()));
}

void MainWindow::on_pushButton_TileSize_Minus_RealESRGAN_clicked()
{
    if(!ui->spinBox_TileSize_RealESRGAN) return;
    ui->spinBox_TileSize_RealESRGAN->setValue(MinusTileSize_NCNNVulkan_Converter(ui->spinBox_TileSize_RealESRGAN->value()));
}

// RealESRGAN Model Change
void MainWindow::on_comboBox_Model_RealESRGAN_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if(!ui->comboBox_Model_RealESRGAN) return;
    qDebug() << "RealESRGAN model changed to:" << ui->comboBox_Model_RealESRGAN->currentText();
    RealESRGAN_NCNN_Vulkan_ReadSettings(); // Update member variables based on new model
}

// RealESRGAN GPU Detection
void MainWindow::on_pushButton_DetectGPU_RealESRGAN_clicked()
{
    if(!ui->comboBox_GPUID_RealESRGAN || !ui->pushButton_DetectGPU_RealESRGAN) return;
    ui->comboBox_GPUID_RealESRGAN->clear();
    ui->pushButton_DetectGPU_RealESRGAN->setEnabled(false);
    ui->pushButton_DetectGPU_RealESRGAN->setText(tr("Detecting..."));
    QtConcurrent::run(this, &MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU);
}

void MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU_finished()
{
    if(!ui->pushButton_DetectGPU_RealESRGAN || !ui->comboBox_GPUID_RealESRGAN) return;
    ui->pushButton_DetectGPU_RealESRGAN->setEnabled(true);
    ui->pushButton_DetectGPU_RealESRGAN->setText(tr("Detect GPU"));
    if(Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty())
    {
        ui->comboBox_GPUID_RealESRGAN->addItem(tr("No compatible GPU found"));
    }
    else
    {
        ui->comboBox_GPUID_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
    }
    // Automatically select the first available GPU or a default if previously saved
    QSettings settings(Current_Path + "/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    settings.beginGroup("RealESRGAN_NCNN_Vulkan");
    QString lastSelectedGPU = settings.value("GPUID", "").toString();
    if (!lastSelectedGPU.isEmpty() && Available_GPUID_RealESRGAN_ncnn_vulkan.contains(lastSelectedGPU)) {
        ui->comboBox_GPUID_RealESRGAN->setCurrentText(lastSelectedGPU);
    } else if (!Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty()) {
        ui->comboBox_GPUID_RealESRGAN->setCurrentIndex(0);
    }
    settings.endGroup();
    emit Send_Realesrgan_ncnn_vulkan_DetectGPU_finished(); // Emit signal if needed by other parts
}

void MainWindow::RealESRGAN_ncnn_vulkan_DetectGPU_errorOccurred()
{
    if(!ui->pushButton_DetectGPU_RealESRGAN || !ui->comboBox_GPUID_RealESRGAN) return;
    ui->pushButton_DetectGPU_RealESRGAN->setEnabled(true);
    ui->pushButton_DetectGPU_RealESRGAN->setText(tr("Detect GPU"));
    ui->comboBox_GPUID_RealESRGAN->clear();
    ui->comboBox_GPUID_RealESRGAN->addItem(tr("Error during GPU detection"));
    QMessageBox::warning(this, tr("Error"), tr("An error occurred during RealESRGAN GPU detection."));
}

// RealESRGAN Multi-GPU Checkbox State Change
void MainWindow::on_checkBox_MultiGPU_RealESRGAN_stateChanged(int state)
{
    if(!ui->groupBox_GPUSettings_MultiGPU_RealESRGAN || !ui->comboBox_GPUID_RealESRGAN || !ui->spinBox_Threads_MultiGPU_RealESRGAN) return;
    if(state == Qt::Checked)
    {
        ui->groupBox_GPUSettings_MultiGPU_RealESRGAN->setVisible(true);
        ui->comboBox_GPUID_RealESRGAN->setEnabled(false); // Disable single GPU selection
        RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay();
    }
    else
    {
        ui->groupBox_GPUSettings_MultiGPU_RealESRGAN->setVisible(false);
        ui->comboBox_GPUID_RealESRGAN->setEnabled(true); // Enable single GPU selection
    }
}

// RealESRGAN Add GPU to Multi-GPU List
void MainWindow::on_pushButton_AddGPU_MultiGPU_RealESRGAN_clicked()
{
    if (!ui->comboBox_GPUIDs_MultiGPU_RealESRGAN || !ui->listWidget_GPUList_MultiGPU_RealESRGAN || Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty()) return;

    QString selectedGPU = ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->currentText();
    if (selectedGPU.isEmpty() || selectedGPU.startsWith(tr("No compatible")) || selectedGPU.startsWith(tr("Error"))) return;

    // Extract GPU ID (e.g., "0: NVIDIA GeForce RTX 3080" -> "0")
    QString gpuID = selectedGPU.split(":").first();

    // Check if already added
    for (int i = 0; i < ui->listWidget_GPUList_MultiGPU_RealESRGAN->count(); ++i) {
        if (ui->listWidget_GPUList_MultiGPU_RealESRGAN->item(i)->data(Qt::UserRole).toString() == gpuID) {
            QMessageBox::information(this, tr("Info"), tr("GPU already added to the list."));
            return;
        }
    }

    QListWidgetItem *newItem = new QListWidgetItem(selectedGPU);
    newItem->setData(Qt::UserRole, gpuID); // Store GPU ID
    newItem->setData(Qt::UserRole + 1, ui->spinBox_Threads_MultiGPU_RealESRGAN->value()); // Store thread count for this GPU
    newItem->setData(Qt::UserRole + 2, ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->value()); // Store tile size
    newItem->setToolTip(QString("ID: %1, Threads: %2, Tile: %3")
                        .arg(gpuID)
                        .arg(ui->spinBox_Threads_MultiGPU_RealESRGAN->value())
                        .arg(ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->value()));

    ui->listWidget_GPUList_MultiGPU_RealESRGAN->addItem(newItem);
    RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay();
}


// RealESRGAN Remove GPU from Multi-GPU List
void MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealESRGAN_clicked()
{
    if(!ui->listWidget_GPUList_MultiGPU_RealESRGAN) return;
    QList<QListWidgetItem*> selectedItems = ui->listWidget_GPUList_MultiGPU_RealESRGAN->selectedItems();
    if(selectedItems.isEmpty()){
        QMessageBox::warning(this, tr("Warning"), tr("Please select a GPU from the list to remove."));
        return;
    }
    for(QListWidgetItem *item : selectedItems){
        delete ui->listWidget_GPUList_MultiGPU_RealESRGAN->takeItem(ui->listWidget_GPUList_MultiGPU_RealESRGAN->row(item));
    }
    RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay();
}

// RealESRGAN Clear Multi-GPU List
void MainWindow::on_pushButton_ClearGPU_MultiGPU_RealESRGAN_clicked()
{
    if(!ui->listWidget_GPUList_MultiGPU_RealESRGAN) return;
    ui->listWidget_GPUList_MultiGPU_RealESRGAN->clear();
    RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay();
}

void MainWindow::RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay() {
    if (!ui->listWidget_GPUList_MultiGPU_RealESRGAN || !ui->label_SelectedGPUs_RealESRGAN) return;
    QStringList selectedGpusText;
    GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.clear(); // Clear and rebuild
    m_realesrgan_gpuJobConfig_temp.clear();

    for (int i = 0; i < ui->listWidget_GPUList_MultiGPU_RealESRGAN->count(); ++i) {
        QListWidgetItem *item = ui->listWidget_GPUList_MultiGPU_RealESRGAN->item(i);
        QString gpuId = item->data(Qt::UserRole).toString();
        int threads = item->data(Qt::UserRole + 1).toInt();
        int tileSize = item->data(Qt::UserRole + 2).toInt();

        selectedGpusText.append(QString("GPU %1 (T:%2, Tile:%3)").arg(gpuId).arg(threads).arg(tileSize));

        GPUIDs_List_MultiGPU_RealesrganNcnnVulkan.append(gpuId); // For old style multi-GPU string

        // For new job config
        QMap<QString, QString> job;
        job["gpuid"] = gpuId;
        job["threads"] = QString::number(threads);
        job["tilesize"] = QString::number(tileSize);
        m_realesrgan_gpuJobConfig_temp.append(job);
    }

    if (selectedGpusText.isEmpty()) {
        ui->label_SelectedGPUs_RealESRGAN->setText(tr("No GPUs selected for multi-GPU mode."));
    } else {
        ui->label_SelectedGPUs_RealESRGAN->setText(tr("Selected for Multi-GPU: ") + selectedGpusText.join(", "));
    }
}


void MainWindow::on_comboBox_GPUIDs_MultiGPU_RealESRGAN_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    if (!ui->comboBox_GPUIDs_MultiGPU_RealESRGAN || !ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN ||
        !ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN || !ui->spinBox_Threads_MultiGPU_RealESRGAN) return;

    QString currentGPUText = ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->currentText();
    if (currentGPUText.isEmpty() || currentGPUText.startsWith(tr("No compatible")) || currentGPUText.startsWith(tr("Error"))) {
        ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setEnabled(false);
        ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setEnabled(false);
        ui->spinBox_Threads_MultiGPU_RealESRGAN->setEnabled(false); // Assuming this is the general thread count for the selected GPU
        return;
    }

    ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->setEnabled(true);
    bool isCurrentGPUEnabled = ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->isChecked();
    ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setEnabled(isCurrentGPUEnabled);
    ui->spinBox_Threads_MultiGPU_RealESRGAN->setEnabled(isCurrentGPUEnabled);
}

void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN_clicked(bool checked)
{
    if (!ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN || !ui->spinBox_Threads_MultiGPU_RealESRGAN) return;
    ui->spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN->setEnabled(checked);
    ui->spinBox_Threads_MultiGPU_RealESRGAN->setEnabled(checked);

    // If an item is selected in listWidget_GPUList_MultiGPU_RealESRGAN that matches comboBox_GPUIDs_MultiGPU_RealESRGAN, update it
    if (ui->listWidget_GPUList_MultiGPU_RealESRGAN && ui->comboBox_GPUIDs_MultiGPU_RealESRGAN) {
        QString currentComboGpuId = ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->currentText().split(":").first();
        for (int i = 0; i < ui->listWidget_GPUList_MultiGPU_RealESRGAN->count(); ++i) {
            QListWidgetItem* item = ui->listWidget_GPUList_MultiGPU_RealESRGAN->item(i);
            if (item->data(Qt::UserRole).toString() == currentComboGpuId) {
                // Update the item if it's being disabled (though typically this checkbox might be for adding new)
                // Or, this logic might be better placed when ADDING the GPU.
                // For now, let's assume this checkbox primarily affects NEW additions.
                // If it should modify existing, that's more complex.
                break;
            }
        }
    }
}


void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealESRGAN_valueChanged(int value)
{
    // If an item is selected in listWidget_GPUList_MultiGPU_RealESRGAN that matches comboBox_GPUIDs_MultiGPU_RealESRGAN, update it
    if (ui->listWidget_GPUList_MultiGPU_RealESRGAN && ui->comboBox_GPUIDs_MultiGPU_RealESRGAN && ui->checkBox_isEnable_CurrentGPU_MultiGPU_RealESRGAN->isChecked()) {
        QString currentComboGpuId = ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->currentText().split(":").first();
        for (int i = 0; i < ui->listWidget_GPUList_MultiGPU_RealESRGAN->count(); ++i) {
            QListWidgetItem* item = ui->listWidget_GPUList_MultiGPU_RealESRGAN->item(i);
            if (item->data(Qt::UserRole).toString() == currentComboGpuId) {
                item->setData(Qt::UserRole + 2, value); // Update tile size
                item->setToolTip(QString("ID: %1, Threads: %2, Tile: %3")
                                 .arg(item->data(Qt::UserRole).toString())
                                 .arg(item->data(Qt::UserRole + 1).toInt())
                                 .arg(value));
                RealESRGAN_MultiGPU_UpdateSelectedGPUDisplay(); // Refresh label
                break;
            }
        }
    }
}


void MainWindow::on_pushButton_ShowMultiGPUSettings_RealESRGAN_clicked()
{
    if (!ui->groupBox_GPUSettings_MultiGPU_RealESRGAN) return;
    bool isVisible = ui->groupBox_GPUSettings_MultiGPU_RealESRGAN->isVisible();
    ui->groupBox_GPUSettings_MultiGPU_RealESRGAN->setVisible(!isVisible);
    if (isVisible) {
        ui->pushButton_ShowMultiGPUSettings_RealESRGAN->setText(tr("Show Multi-GPU Settings"));
    } else {
        ui->pushButton_ShowMultiGPUSettings_RealESRGAN->setText(tr("Hide Multi-GPU Settings"));
        // Populate the combobox for adding GPUs if it's empty or needs refresh
        if (ui->comboBox_GPUIDs_MultiGPU_RealESRGAN && ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->count() == 0 && !Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty()) {
             ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->addItems(Available_GPUID_RealESRGAN_ncnn_vulkan);
        } else if (ui->comboBox_GPUIDs_MultiGPU_RealESRGAN && ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->count() == 0 && Available_GPUID_RealESRGAN_ncnn_vulkan.isEmpty()){
             ui->comboBox_GPUIDs_MultiGPU_RealESRGAN->addItem(tr("No compatible GPU detected"));
        }
         on_comboBox_GPUIDs_MultiGPU_RealESRGAN_currentIndexChanged(0); // Trigger update of dependent controls
    }
}

// RealCUGAN slots that were defined in mainwindow.h but might be missing if an old version was pasted
void MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked() {
    if(!ui->comboBox_GPUID_RealCUGAN || !ui->pushButton_DetectGPU_RealCUGAN) return;
    ui->comboBox_GPUID_RealCUGAN->clear();
    ui->pushButton_DetectGPU_RealCUGAN->setEnabled(false);
    ui->pushButton_DetectGPU_RealCUGAN->setText(tr("Detecting..."));
    QtConcurrent::run(this, &MainWindow::Realcugan_ncnn_vulkan_DetectGPU);
}

void MainWindow::Realcugan_ncnn_vulkan_DetectGPU_finished() {
    if(!ui->pushButton_DetectGPU_RealCUGAN || !ui->comboBox_GPUID_RealCUGAN) return;
    ui->pushButton_DetectGPU_RealCUGAN->setEnabled(true);
    ui->pushButton_DetectGPU_RealCUGAN->setText(tr("Detect GPU"));
    if(Available_GPUID_RealCUGAN_ncnn_vulkan.isEmpty()) {
        ui->comboBox_GPUID_RealCUGAN->addItem(tr("No compatible GPU found"));
    } else {
        ui->comboBox_GPUID_RealCUGAN->addItems(Available_GPUID_RealCUGAN_ncnn_vulkan);
    }
    QSettings settings(Current_Path + "/settings.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));
    settings.beginGroup("RealCUGAN_NCNN_Vulkan");
    QString lastSelectedGPU = settings.value("GPUID", "").toString();
    if (!lastSelectedGPU.isEmpty() && Available_GPUID_RealCUGAN_ncnn_vulkan.contains(lastSelectedGPU)) {
        ui->comboBox_GPUID_RealCUGAN->setCurrentText(lastSelectedGPU);
    } else if (!Available_GPUID_RealCUGAN_ncnn_vulkan.isEmpty()) {
        ui->comboBox_GPUID_RealCUGAN->setCurrentIndex(0);
    }
    settings.endGroup();
    emit Send_Realcugan_ncnn_vulkan_DetectGPU_finished();
}

void MainWindow::Realcugan_ncnn_vulkan_DetectGPU_errorOccurred() {
    if(!ui->pushButton_DetectGPU_RealCUGAN || !ui->comboBox_GPUID_RealCUGAN) return;
    ui->pushButton_DetectGPU_RealCUGAN->setEnabled(true);
    ui->pushButton_DetectGPU_RealCUGAN->setText(tr("Detect GPU"));
    ui->comboBox_GPUID_RealCUGAN->clear();
    ui->comboBox_GPUID_RealCUGAN->addItem(tr("Error during GPU detection"));
    QMessageBox::warning(this, tr("Error"), tr("An error occurred during RealCUGAN GPU detection."));
}

void MainWindow::on_checkBox_MultiGPU_RealCUGAN_stateChanged(int state) {
    if(!ui->groupBox_GPUSettings_MultiGPU_RealCUGAN || !ui->comboBox_GPUID_RealCUGAN) return;
    if(state == Qt::Checked) {
        ui->groupBox_GPUSettings_MultiGPU_RealCUGAN->setVisible(true);
        ui->comboBox_GPUID_RealCUGAN->setEnabled(false);
         // Ensure the multi-GPU combobox is populated
        if (ui->comboBox_GPUIDs_MultiGPU_RealCUGAN && ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->count() == 0 && !Available_GPUID_RealCUGAN_ncnn_vulkan.isEmpty()) {
             ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->addItems(Available_GPUID_RealCUGAN_ncnn_vulkan);
        } else if (ui->comboBox_GPUIDs_MultiGPU_RealCUGAN && ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->count() == 0 && Available_GPUID_RealCUGAN_ncnn_vulkan.isEmpty()){
             ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->addItem(tr("No compatible GPU detected"));
        }
    } else {
        ui->groupBox_GPUSettings_MultiGPU_RealCUGAN->setVisible(false);
        ui->comboBox_GPUID_RealCUGAN->setEnabled(true);
    }
}

void MainWindow::on_pushButton_AddGPU_MultiGPU_RealCUGAN_clicked() {
    if (!ui->comboBox_GPUIDs_MultiGPU_RealCUGAN || !ui->listWidget_GPUList_MultiGPU_RealCUGAN || Available_GPUID_RealCUGAN_ncnn_vulkan.isEmpty()) return;
    QString selectedGPU = ui->comboBox_GPUIDs_MultiGPU_RealCUGAN->currentText();
    if (selectedGPU.isEmpty() || selectedGPU.startsWith(tr("No compatible")) || selectedGPU.startsWith(tr("Error"))) return;
    QString gpuID = selectedGPU.split(":").first();
    for (int i = 0; i < ui->listWidget_GPUList_MultiGPU_RealCUGAN->count(); ++i) {
        if (ui->listWidget_GPUList_MultiGPU_RealCUGAN->item(i)->data(Qt::UserRole).toString() == gpuID) {
            QMessageBox::information(this, tr("Info"), tr("GPU already added to the list."));
            return;
        }
    }
    QListWidgetItem *newItem = new QListWidgetItem(selectedGPU);
    newItem->setData(Qt::UserRole, gpuID);
    ui->listWidget_GPUList_MultiGPU_RealCUGAN->addItem(newItem);
}

void MainWindow::on_pushButton_RemoveGPU_MultiGPU_RealCUGAN_clicked() {
    if(!ui->listWidget_GPUList_MultiGPU_RealCUGAN) return;
    QList<QListWidgetItem*> selectedItems = ui->listWidget_GPUList_MultiGPU_RealCUGAN->selectedItems();
    if(selectedItems.isEmpty()){
        QMessageBox::warning(this, tr("Warning"), tr("Please select a GPU from the list to remove."));
        return;
    }
    for(QListWidgetItem *item : selectedItems){
        delete ui->listWidget_GPUList_MultiGPU_RealCUGAN->takeItem(ui->listWidget_GPUList_MultiGPU_RealCUGAN->row(item));
    }
}

void MainWindow::on_pushButton_ClearGPU_MultiGPU_RealCUGAN_clicked() {
    if(!ui->listWidget_GPUList_MultiGPU_RealCUGAN) return;
    ui->listWidget_GPUList_MultiGPU_RealCUGAN->clear();
}

void MainWindow::Realcugan_NCNN_Vulkan_Image_finished() { /* Implementation from realcugan_ncnn_vulkan.cpp */ }
void MainWindow::Realcugan_NCNN_Vulkan_Image_readyReadStandardOutput() { /* Implementation from realcugan_ncnn_vulkan.cpp */ }
void MainWindow::Realcugan_NCNN_Vulkan_Image_readyReadStandardError() { /* Implementation from realcugan_ncnn_vulkan.cpp */ }
void MainWindow::Realcugan_NCNN_Vulkan_Image_errorOccurred() { /* Implementation from realcugan_ncnn_vulkan.cpp */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_finished(int, QProcess::ExitStatus) { /* Stub */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardOutput() { /* Stub */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_readyReadStandardError() { /* Stub */ }
void MainWindow::Realcugan_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError) { /* Stub */ }

// RealESRGAN slots that were defined in mainwindow.h
void MainWindow::RealESRGAN_NCNN_Vulkan_Image_finished() { /* Implementation from realesrgan_ncnn_vulkan.cpp */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Image_readyReadStandardOutput() { /* Implementation from realesrgan_ncnn_vulkan.cpp */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Image_readyReadStandardError() { /* Implementation from realesrgan_ncnn_vulkan.cpp */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Image_errorOccurred() { /* Implementation from realesrgan_ncnn_vulkan.cpp */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_finished(int, QProcess::ExitStatus) { /* Stub */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardOutput() { /* Stub */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_readyReadStandardError() { /* Stub */ }
void MainWindow::RealESRGAN_NCNN_Vulkan_Iterative_errorOccurred(QProcess::ProcessError) { /* Stub */ }

// Compatibility CheckBox slots
void MainWindow::on_checkBox_isCompatible_RealCUGAN_NCNN_Vulkan_clicked()
{
    ui->checkBox_isCompatible_RealCUGAN_NCNN_Vulkan->setChecked(isCompatible_RealCUGAN_NCNN_Vulkan);
}

void MainWindow::on_checkBox_isCompatible_RealESRGAN_NCNN_Vulkan_clicked()
{
    ui->checkBox_isCompatible_RealESRGAN_NCNN_Vulkan->setChecked(isCompatible_RealESRGAN_NCNN_Vulkan);
}
