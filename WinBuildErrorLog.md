g++ -c -pipe -g -Wall -Wextra -fPIC -D_REENTRANT -DQT_DEPRECATED_WARNINGS -DQT_MULTIMEDIA_LIB -DQT_OPENGLWIDGETS_LIB -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CONCURRENT_LIB -DQT_NETWORK_LIB -DQT_CORE5COMPAT_LIB -DQT_CORE_LIB -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtMultimedia -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGLWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtConcurrent -I/usr/include/x86_64-linux-gnu/qt6/QtNetwork -I/usr/include/x86_64-linux-gnu/qt6/QtCore5Compat -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o files.o files.cpp
g++ -c -pipe -g -Wall -Wextra -fPIC -D_REENTRANT -DQT_DEPRECATED_WARNINGS -DQT_MULTIMEDIA_LIB -DQT_OPENGLWIDGETS_LIB -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CONCURRENT_LIB -DQT_NETWORK_LIB -DQT_CORE5COMPAT_LIB -DQT_CORE_LIB -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtMultimedia -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGLWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtConcurrent -I/usr/include/x86_64-linux-gnu/qt6/QtNetwork -I/usr/include/x86_64-linux-gnu/qt6/QtCore5Compat -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o image.o image.cpp
g++ -c -pipe -g -Wall -Wextra -fPIC -D_REENTRANT -DQT_DEPRECATED_WARNINGS -DQT_MULTIMEDIA_LIB -DQT_OPENGLWIDGETS_LIB -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CONCURRENT_LIB -DQT_NETWORK_LIB -DQT_CORE5COMPAT_LIB -DQT_CORE_LIB -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtMultimedia -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGLWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtConcurrent -I/usr/include/x86_64-linux-gnu/qt6/QtNetwork -I/usr/include/x86_64-linux-gnu/qt6/QtCore5Compat -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o mainwindow.o mainwindow.cpp
g++ -c -pipe -g -Wall -Wextra -fPIC -D_REENTRANT -DQT_DEPRECATED_WARNINGS -DQT_MULTIMEDIA_LIB -DQT_OPENGLWIDGETS_LIB -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CONCURRENT_LIB -DQT_NETWORK_LIB -DQT_CORE5COMPAT_LIB -DQT_CORE_LIB -I. -I/usr/include/x86_64-linux-gnu/qt6 -I/usr/include/x86_64-linux-gnu/qt6/QtMultimedia -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGLWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtOpenGL -I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6/QtGui -I/usr/include/x86_64-linux-gnu/qt6/QtConcurrent -I/usr/include/x86_64-linux-gnu/qt6/QtNetwork -I/usr/include/x86_64-linux-gnu/qt6/QtCore5Compat -I/usr/include/x86_64-linux-gnu/qt6/QtCore -I. -I. -I/usr/lib/x86_64-linux-gnu/qt6/mkspecs/linux-g++ -o progressBar.o progressBar.cpp
progressBar.cpp:34:6: error: no declaration matches ‘void MainWindow::progressbar_SetToMax(int)’
   34 | void MainWindow::progressbar_SetToMax(int maxval)
      |      ^~~~~~~~~~
progressBar.cpp:34:6: note: no functions named ‘void MainWindow::progressbar_SetToMax(int)’
In file included from progressBar.cpp:20:
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
progressBar.cpp: In member function ‘void MainWindow::progressbar_Add()’:
progressBar.cpp:44:5: error: ‘Progressbar_CurrentVal’ was not declared in this scope
   44 |     Progressbar_CurrentVal++;
      |     ^~~~~~~~~~~~~~~~~~~~~~
files.cpp: In member function ‘virtual void MainWindow::dropEvent(QDropEvent*)’:
files.cpp:57:65: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::dropEvent(QDropEvent*)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
   57 |     QtConcurrent::run([this, urls]() { this->Read_urls(urls); });
      |                                                                 ^
In file included from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/QtConcurrent:14,
                 from mainwindow.h:29,
                 from files.cpp:20:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
image.cpp: In member function ‘void MainWindow::ImagesResize_Folder_MultiThread(int, int, QString)’:
image.cpp:47:143: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::ImagesResize_Folder_MultiThread(int, int, QString)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
   47 |         QtConcurrent::run([this, New_width, New_height, OutPut_Path]() { this->ResizeImage_MultiThread(New_width, New_height, OutPut_Path); });
      |                                                                                                                                               ^
In file included from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/QtConcurrent:14,
                 from mainwindow.h:29,
                 from image.cpp:19:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
image.cpp: At global scope:
image.cpp:205:6: error: no declaration matches ‘void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int)’
  205 | void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int index)
      |      ^~~~~~~~~~
image.cpp:205:6: note: no functions named ‘void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
progressBar.cpp:48:107: error: ‘Progressbar_MaxVal’ was not declared in this scope; did you mean ‘progressbar_Add’?
   48 |     ui->label_progressBar_filenum->setText(QString::number(Progressbar_CurrentVal,10)+"/"+QString::number(Progressbar_MaxVal,10));
      |                                                                                                           ^~~~~~~~~~~~~~~~~~
      |                                                                                                           progressbar_Add
progressBar.cpp: At global scope:
progressBar.cpp:53:6: error: no declaration matches ‘void MainWindow::progressbar_clear()’
   53 | void MainWindow::progressbar_clear()
      |      ^~~~~~~~~~
progressBar.cpp:53:6: note: no functions named ‘void MainWindow::progressbar_clear()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
files.cpp: In member function ‘void MainWindow::Read_urls(QList<QUrl>)’:
files.cpp:64:5: error: ‘Progressbar_MaxVal’ was not declared in this scope; did you mean ‘progressbar_Add’?
   64 |     Progressbar_MaxVal = urls.size();
      |     ^~~~~~~~~~~~~~~~~~
      |     progressbar_Add
files.cpp:65:5: error: ‘Progressbar_CurrentVal’ was not declared in this scope
   65 |     Progressbar_CurrentVal = 0;
      |     ^~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp: In constructor ‘MainWindow::MainWindow(int, QWidget*)’:
mainwindow.cpp:262:5: error: ‘on_tabWidget_currentChanged’ was not declared in this scope
  262 |     on_tabWidget_currentChanged(1);
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~
files.cpp: In member function ‘void MainWindow::Read_urls_finfished()’:
files.cpp:104:5: error: ‘Progressbar_MaxVal’ was not declared in this scope; did you mean ‘progressbar_Add’?
  104 |     Progressbar_MaxVal = 0;
      |     ^~~~~~~~~~~~~~~~~~
      |     progressbar_Add
mainwindow.cpp:334:71: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::MainWindow(int, QWidget*)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
  334 |     QtConcurrent::run([this] { this->DeleteErrorLog_Waifu2xCaffe(); });
      |                                                                       ^
In file included from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/QtConcurrent:14,
                 from mainwindow.h:29,
                 from mainwindow.cpp:20:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
mainwindow.cpp:335:59: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::MainWindow(int, QWidget*)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
  335 |     QtConcurrent::run([this] { this->Del_TempBatFile(); });
      |                                                           ^
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
files.cpp:105:5: error: ‘Progressbar_CurrentVal’ was not declared in this scope
  105 |     Progressbar_CurrentVal = 0;
      |     ^~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:339:5: error: ‘TextBrowser_StartMes’ was not declared in this scope
  339 |     TextBrowser_StartMes();
      |     ^~~~~~~~~~~~~~~~~~~~
files.cpp:106:5: error: ‘progressbar_clear’ was not declared in this scope; did you mean ‘progressbar_Add’?
  106 |     progressbar_clear();
      |     ^~~~~~~~~~~~~~~~~
      |     progressbar_Add
mainwindow.cpp: In member function ‘void MainWindow::ProcessDroppedFilesAsync(QList<QUrl>)’:
mainwindow.cpp:466:5: error: ‘Progressbar_MaxVal’ was not declared in this scope; did you mean ‘progressbar_Add’?
  466 |     Progressbar_MaxVal = 0;
      |     ^~~~~~~~~~~~~~~~~~
      |     progressbar_Add
mainwindow.cpp:467:5: error: ‘Progressbar_CurrentVal’ was not declared in this scope
  467 |     Progressbar_CurrentVal = 0;
      |     ^~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘virtual void MainWindow::closeEvent(QCloseEvent*)’:
mainwindow.cpp:612:79: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::closeEvent(QCloseEvent*)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
  612 |         QtConcurrent::run([this] { this->Auto_Save_Settings_Watchdog(true); });
      |                                                                               ^
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
mainwindow.cpp:616:80: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::closeEvent(QCloseEvent*)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
  616 |         QtConcurrent::run([this] { this->Auto_Save_Settings_Watchdog(false); });
      |                                                                                ^
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
mainwindow.cpp: In member function ‘void MainWindow::on_pushButton_ClearList_clicked()’:
mainwindow.cpp:776:5: error: ‘progressbar_clear’ was not declared in this scope; did you mean ‘progressbar_Add’?
  776 |     progressbar_clear();
      |     ^~~~~~~~~~~~~~~~~
      |     progressbar_Add
mainwindow.cpp: At global scope:
mainwindow.cpp:779:6: error: no declaration matches ‘void MainWindow::on_pushButton_Stop_clicked()’
  779 | void MainWindow::on_pushButton_Stop_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:779:6: note: no functions named ‘void MainWindow::on_pushButton_Stop_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
In file included from /usr/include/x86_64-linux-gnu/qt6/QtCore/qfuture.h:13,
                 from /usr/include/x86_64-linux-gnu/qt6/QtCore/QtCore:98,
                 from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/QtConcurrentDepends:3,
                 from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/QtConcurrent:3:
/usr/include/x86_64-linux-gnu/qt6/QtCore/qfuture_impl.h: In instantiation of ‘struct QtPrivate::ArgResolver<MainWindow*>’:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:189:64:   required from ‘struct QtConcurrent::PromiseTaskResolver<MainWindow*, void (MainWindow::*)()>’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:211:8:   required from ‘struct QtConcurrent::TaskResolverHelper<std::integral_constant<bool, false>, MainWindow*, void (MainWindow::*)()>’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:217:8:   required from ‘struct QtConcurrent::TaskResolver<MainWindow*, void (MainWindow::*)()>’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:40:76:   required from ‘auto QtConcurrent::run(QThreadPool*, Function&&, Args&& ...) [with Function = MainWindow*; Args = {void (MainWindow::*)()}]’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:57:15:   required from ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow*; Args = {void (MainWindow::*)()}]’
mainwindow.cpp:804:30:   required from here
/usr/include/x86_64-linux-gnu/qt6/QtCore/qfuture_impl.h:178:43: error: ‘operator()’ is not a member of ‘std::decay<MainWindow*>::type’ {aka ‘MainWindow*’}
  178 | struct ArgResolver : ArgResolver<decltype(&std::decay_t<F>::operator())>
      |                                           ^~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtaskbuilder.h:9,
                 from /usr/include/x86_64-linux-gnu/qt6/QtConcurrent/QtConcurrent:4:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h: In instantiation of ‘struct QtConcurrent::PromiseTaskResolver<MainWindow*, void (MainWindow::*)()>’:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:211:8:   required from ‘struct QtConcurrent::TaskResolverHelper<std::integral_constant<bool, false>, MainWindow*, void (MainWindow::*)()>’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:217:8:   required from ‘struct QtConcurrent::TaskResolver<MainWindow*, void (MainWindow::*)()>’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:40:76:   required from ‘auto QtConcurrent::run(QThreadPool*, Function&&, Args&& ...) [with Function = MainWindow*; Args = {void (MainWindow::*)()}]’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:57:15:   required from ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow*; Args = {void (MainWindow::*)()}]’
mainwindow.cpp:804:30:   required from here
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:189:64: error: no type named ‘IsPromise’ in ‘struct QtPrivate::ArgResolver<MainWindow*>’
  189 |     static_assert(QtPrivate::ArgResolver<Function>::IsPromise::value,
      |                                                                ^~~~~
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h: In instantiation of ‘static auto QtConcurrent::PromiseTaskResolver<Function, Args ...>::run(TaskWithArgs&&, const QtConcurrent::TaskStartParameters&) [with Function = MainWindow*; Args = {void (MainWindow::*)()}; TaskWithArgs = std::tuple<MainWindow*, void (MainWindow::*)()>]’:
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:40:76:   required from ‘auto QtConcurrent::run(QThreadPool*, Function&&, Args&& ...) [with Function = MainWindow*; Args = {void (MainWindow::*)()}]’
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:57:15:   required from ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow*; Args = {void (MainWindow::*)()}]’
mainwindow.cpp:804:30:   required from here
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:195:15: error: no type named ‘PromiseType’ in ‘struct QtPrivate::ArgResolver<MainWindow*>’
  195 |         using PromiseType = typename QtPrivate::ArgResolver<Function>::PromiseType;
      |               ^~~~~~~~~~~
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentstoredfunctioncall.h:196:17: error: no type named ‘PromiseType’ in ‘struct QtPrivate::ArgResolver<MainWindow*>’
  196 |         return (new StoredFunctionCallWithPromise<Function, PromiseType, Args...>(std::move(args)))
      |                ~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:812:5: error: no declaration matches ‘int MainWindow::on_pushButton_RemoveItem_clicked()’
  812 | int MainWindow::on_pushButton_RemoveItem_clicked()
      |     ^~~~~~~~~~
mainwindow.cpp:812:5: note: no functions named ‘int MainWindow::on_pushButton_RemoveItem_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:922:6: error: no declaration matches ‘void MainWindow::on_pushButton_Report_clicked()’
  922 | void MainWindow::on_pushButton_Report_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:922:6: note: no functions named ‘void MainWindow::on_pushButton_Report_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:927:6: error: no declaration matches ‘void MainWindow::on_pushButton_ReadMe_clicked()’
  927 | void MainWindow::on_pushButton_ReadMe_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:927:6: note: no functions named ‘void MainWindow::on_pushButton_ReadMe_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:942:6: error: no declaration matches ‘void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int)’
  942 | void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:942:6: note: no functions named ‘void MainWindow::on_comboBox_Engine_Image_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:963:6: error: no declaration matches ‘void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int)’
  963 | void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:963:6: note: no functions named ‘void MainWindow::on_comboBox_Engine_GIF_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:984:6: error: no declaration matches ‘void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int)’
  984 | void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:984:6: note: no functions named ‘void MainWindow::on_comboBox_Engine_Video_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1005:6: error: no declaration matches ‘void MainWindow::on_pushButton_clear_textbrowser_clicked()’
 1005 | void MainWindow::on_pushButton_clear_textbrowser_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1005:6: note: no functions named ‘void MainWindow::on_pushButton_clear_textbrowser_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1011:6: error: no declaration matches ‘void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int)’
 1011 | void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1011:6: note: no functions named ‘void MainWindow::on_spinBox_textbrowser_fontsize_valueChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1028:6: error: no declaration matches ‘void MainWindow::on_pushButton_HideSettings_clicked()’
 1028 | void MainWindow::on_pushButton_HideSettings_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1028:6: note: no functions named ‘void MainWindow::on_pushButton_HideSettings_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1044:6: error: no declaration matches ‘void MainWindow::on_comboBox_language_currentIndexChanged(int)’
 1044 | void MainWindow::on_comboBox_language_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:1044:6: note: no functions named ‘void MainWindow::on_comboBox_language_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1134:6: error: no declaration matches ‘void MainWindow::on_pushButton_ReadFileList_clicked()’
 1134 | void MainWindow::on_pushButton_ReadFileList_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1134:6: note: no functions named ‘void MainWindow::on_pushButton_ReadFileList_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1165:6: error: no declaration matches ‘void MainWindow::on_Ext_image_editingFinished()’
 1165 | void MainWindow::on_Ext_image_editingFinished()
      |      ^~~~~~~~~~
mainwindow.cpp:1165:6: note: no functions named ‘void MainWindow::on_Ext_image_editingFinished()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1174:6: error: no declaration matches ‘void MainWindow::on_Ext_video_editingFinished()’
 1174 | void MainWindow::on_Ext_video_editingFinished()
      |      ^~~~~~~~~~
mainwindow.cpp:1174:6: note: no functions named ‘void MainWindow::on_Ext_video_editingFinished()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1183:6: error: no declaration matches ‘void MainWindow::on_checkBox_AutoSaveSettings_clicked()’
 1183 | void MainWindow::on_checkBox_AutoSaveSettings_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1183:6: note: no functions named ‘void MainWindow::on_checkBox_AutoSaveSettings_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1193:6: error: no declaration matches ‘void MainWindow::on_pushButton_about_clicked()’
 1193 | void MainWindow::on_pushButton_about_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1193:6: note: no functions named ‘void MainWindow::on_pushButton_about_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1214:6: error: no declaration matches ‘void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int)’
 1214 | void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:1214:6: note: no functions named ‘void MainWindow::on_comboBox_AspectRatio_custRes_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1237:6: error: no declaration matches ‘void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int)’
 1237 | void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1237:6: note: no functions named ‘void MainWindow::on_checkBox_AlwaysHideSettings_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1247:6: error: no declaration matches ‘void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()’
 1247 | void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1247:6: note: no functions named ‘void MainWindow::on_pushButton_Save_GlobalFontSize_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1260:6: error: no declaration matches ‘void MainWindow::on_pushButton_BrowserFile_clicked()’
 1260 | void MainWindow::on_pushButton_BrowserFile_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1260:6: note: no functions named ‘void MainWindow::on_pushButton_BrowserFile_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::Read_Input_paths_BrowserFile(QStringList)’:
mainwindow.cpp:1329:5: error: ‘Progressbar_MaxVal’ was not declared in this scope; did you mean ‘progressbar_Add’?
 1329 |     Progressbar_MaxVal = Input_path_List.size();
      |     ^~~~~~~~~~~~~~~~~~
      |     progressbar_Add
mainwindow.cpp:1330:5: error: ‘Progressbar_CurrentVal’ was not declared in this scope
 1330 |     Progressbar_CurrentVal = 0;
      |     ^~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp: At global scope:
mainwindow.cpp:1362:6: error: no declaration matches ‘void MainWindow::on_pushButton_wiki_clicked()’
 1362 | void MainWindow::on_pushButton_wiki_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1362:6: note: no functions named ‘void MainWindow::on_pushButton_wiki_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1371:6: error: no declaration matches ‘void MainWindow::on_pushButton_HideTextBro_clicked()’
 1371 | void MainWindow::on_pushButton_HideTextBro_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1371:6: note: no functions named ‘void MainWindow::on_pushButton_HideTextBro_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1385:6: error: no declaration matches ‘void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int)’
 1385 | void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1385:6: note: no functions named ‘void MainWindow::on_checkBox_AlwaysHideTextBrowser_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1396:6: error: no declaration matches ‘void MainWindow::on_Ext_image_textChanged(const QString&)’
 1396 | void MainWindow::on_Ext_image_textChanged(const QString &arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1396:6: note: no functions named ‘void MainWindow::on_Ext_image_textChanged(const QString&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1402:6: error: no declaration matches ‘void MainWindow::on_Ext_video_textChanged(const QString&)’
 1402 | void MainWindow::on_Ext_video_textChanged(const QString &arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1402:6: note: no functions named ‘void MainWindow::on_Ext_video_textChanged(const QString&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1408:6: error: no declaration matches ‘void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int)’
 1408 | void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:1408:6: note: no functions named ‘void MainWindow::on_comboBox_model_vulkan_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1432:6: error: no declaration matches ‘void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int)’
 1432 | void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:1432:6: note: no functions named ‘void MainWindow::on_comboBox_ImageStyle_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1444:6: error: no declaration matches ‘void MainWindow::on_pushButton_ResetVideoSettings_clicked()’
 1444 | void MainWindow::on_pushButton_ResetVideoSettings_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1444:6: note: no functions named ‘void MainWindow::on_pushButton_ResetVideoSettings_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1461:6: error: no declaration matches ‘void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString&)’
 1461 | void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString &arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1461:6: note: no functions named ‘void MainWindow::on_lineEdit_encoder_vid_textChanged(const QString&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1467:6: error: no declaration matches ‘void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString&)’
 1467 | void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString &arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1467:6: note: no functions named ‘void MainWindow::on_lineEdit_encoder_audio_textChanged(const QString&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1473:6: error: no declaration matches ‘void MainWindow::on_lineEdit_pixformat_textChanged(const QString&)’
 1473 | void MainWindow::on_lineEdit_pixformat_textChanged(const QString &arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1473:6: note: no functions named ‘void MainWindow::on_lineEdit_pixformat_textChanged(const QString&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1479:6: error: no declaration matches ‘void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int)’
 1479 | void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1479:6: note: no functions named ‘void MainWindow::on_checkBox_vcodec_copy_2mp4_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1491:6: error: no declaration matches ‘void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int)’
 1491 | void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1491:6: note: no functions named ‘void MainWindow::on_checkBox_acodec_copy_2mp4_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1504:6: error: no declaration matches ‘void MainWindow::on_pushButton_encodersList_clicked()’
 1504 | void MainWindow::on_pushButton_encodersList_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1504:6: note: no functions named ‘void MainWindow::on_pushButton_encodersList_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::Tip_FirstTimeStart()’:
mainwindow.cpp:1529:9: error: ‘on_comboBox_language_currentIndexChanged’ was not declared in this scope; did you mean ‘on_comboBox_Engine_VFI_currentIndexChanged’?
 1529 |         on_comboBox_language_currentIndexChanged(0);
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         on_comboBox_Engine_VFI_currentIndexChanged
mainwindow.cpp:1537:9: error: ‘on_pushButton_clear_textbrowser_clicked’ was not declared in this scope; did you mean ‘on_pushButton_ClearList_clicked’?
 1537 |         on_pushButton_clear_textbrowser_clicked();
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |         on_pushButton_ClearList_clicked
mainwindow.cpp: At global scope:
mainwindow.cpp:1542:6: error: no declaration matches ‘void MainWindow::on_checkBox_DelOriginal_stateChanged(int)’
 1542 | void MainWindow::on_checkBox_DelOriginal_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1542:6: note: no functions named ‘void MainWindow::on_checkBox_DelOriginal_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1557:6: error: no declaration matches ‘void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int)’
 1557 | void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1557:6: note: no functions named ‘void MainWindow::on_checkBox_FileList_Interactive_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1573:6: error: no declaration matches ‘void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int)’
 1573 | void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1573:6: note: no functions named ‘void MainWindow::on_checkBox_OutPath_isEnabled_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1596:6: error: no declaration matches ‘void MainWindow::on_pushButton_ForceRetry_clicked()’
 1596 | void MainWindow::on_pushButton_ForceRetry_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1596:6: note: no functions named ‘void MainWindow::on_pushButton_ForceRetry_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1636:6: error: no declaration matches ‘void MainWindow::on_pushButton_PayPal_clicked()’
 1636 | void MainWindow::on_pushButton_PayPal_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1636:6: note: no functions named ‘void MainWindow::on_pushButton_PayPal_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1640:6: error: no declaration matches ‘void MainWindow::on_checkBox_AudioDenoise_stateChanged(int)’
 1640 | void MainWindow::on_checkBox_AudioDenoise_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1640:6: note: no functions named ‘void MainWindow::on_checkBox_AudioDenoise_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1651:6: error: no declaration matches ‘void MainWindow::on_tabWidget_currentChanged(int)’
 1651 | void MainWindow::on_tabWidget_currentChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:1651:6: note: no functions named ‘void MainWindow::on_tabWidget_currentChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1787:6: error: no declaration matches ‘void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int)’
 1787 | void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1787:6: note: no functions named ‘void MainWindow::on_checkBox_ProcessVideoBySegment_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1800:6: error: no declaration matches ‘void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int)’
 1800 | void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:1800:6: note: no functions named ‘void MainWindow::on_comboBox_version_Waifu2xNCNNVulkan_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1828:6: error: no declaration matches ‘void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int)’
 1828 | void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1828:6: note: no functions named ‘void MainWindow::on_checkBox_EnablePreProcessing_Anime4k_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1851:6: error: no declaration matches ‘void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int)’
 1851 | void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1851:6: note: no functions named ‘void MainWindow::on_checkBox_EnablePostProcessing_Anime4k_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1874:6: error: no declaration matches ‘void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int)’
 1874 | void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1874:6: note: no functions named ‘void MainWindow::on_checkBox_SpecifyGPU_Anime4k_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1889:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked()’
 1889 | void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1889:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1893:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked()’
 1893 | void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1893:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_NEW_FP16P_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1897:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked()’
 1897 | void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1897:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_NCNN_Vulkan_OLD_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1901:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked()’
 1901 | void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1901:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_SRMD_NCNN_Vulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1905:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked()’
 1905 | void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1905:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_SRMD_CUDA_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1909:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked()’
 1909 | void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1909:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Converter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1913:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked()’
 1913 | void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1913:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Anime4k_CPU_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1917:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked()’
 1917 | void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1917:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Anime4k_GPU_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1921:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked()’
 1921 | void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1921:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_FFmpeg_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1925:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_FFprobe_clicked()’
 1925 | void MainWindow::on_checkBox_isCompatible_FFprobe_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1925:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_FFprobe_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1929:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked()’
 1929 | void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1929:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_ImageMagick_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1933:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked()’
 1933 | void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1933:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Gifsicle_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1937:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_SoX_clicked()’
 1937 | void MainWindow::on_checkBox_isCompatible_SoX_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1937:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_SoX_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1941:6: error: no declaration matches ‘void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int)’
 1941 | void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1941:6: note: no functions named ‘void MainWindow::on_checkBox_GPUMode_Anime4K_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1957:6: error: no declaration matches ‘void MainWindow::on_checkBox_ShowInterPro_stateChanged(int)’
 1957 | void MainWindow::on_checkBox_ShowInterPro_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1957:6: note: no functions named ‘void MainWindow::on_checkBox_ShowInterPro_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1964:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked()’
 1964 | void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1964:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_CPU_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1968:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked()’
 1968 | void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1968:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_GPU_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1972:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked()’
 1972 | void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1972:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Waifu2x_Caffe_cuDNN_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1976:6: error: no declaration matches ‘void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked()’
 1976 | void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1976:6: note: no functions named ‘void MainWindow::on_pushButton_SplitSize_Add_Waifu2xCaffe_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1984:6: error: no declaration matches ‘void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked()’
 1984 | void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1984:6: note: no functions named ‘void MainWindow::on_pushButton_SplitSize_Minus_Waifu2xCaffe_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1992:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked()’
 1992 | void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:1992:6: note: no functions named ‘void MainWindow::on_checkBox_isCompatible_Realsr_NCNN_Vulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:1996:6: error: no declaration matches ‘void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int)’
 1996 | void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:1996:6: note: no functions named ‘void MainWindow::on_checkBox_ACNet_Anime4K_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2022:6: error: no declaration matches ‘void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int)’
 2022 | void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:2022:6: note: no functions named ‘void MainWindow::on_checkBox_HDNMode_Anime4k_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2066:6: error: no declaration matches ‘void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int)’
 2066 | void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:2066:6: note: no functions named ‘void MainWindow::on_comboBox_UpdateChannel_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2073:6: error: no declaration matches ‘void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int)’
 2073 | void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:2073:6: note: no functions named ‘void MainWindow::on_checkBox_ReplaceOriginalFile_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2119:6: error: no declaration matches ‘void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int)’
 2119 | void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:2119:6: note: no functions named ‘void MainWindow::on_checkBox_isCustFontEnable_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘virtual bool MainWindow::eventFilter(QObject*, QEvent*)’:
mainwindow.cpp:2149:21: error: ‘on_pushButton_RemoveItem_clicked’ was not declared in this scope; did you mean ‘on_pushButton_CheckUpdate_clicked’?
 2149 |                     on_pushButton_RemoveItem_clicked();
      |                     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                     on_pushButton_CheckUpdate_clicked
mainwindow.cpp: At global scope:
mainwindow.cpp:2177:6: error: no declaration matches ‘void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked()’
 2177 | void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:2177:6: note: no functions named ‘void MainWindow::on_pushButton_ResizeFilesListSplitter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2181:6: error: no declaration matches ‘void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int)’
 2181 | void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int index)
      |      ^~~~~~~~~~
mainwindow.cpp:2181:6: note: no functions named ‘void MainWindow::on_comboBox_GPGPUModel_A4k_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2194:6: error: no declaration matches ‘void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int)’
 2194 | void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int arg1)
      |      ^~~~~~~~~~
mainwindow.cpp:2194:6: note: no functions named ‘void MainWindow::on_checkBox_DisableGPU_converter_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2209:6: error: no declaration matches ‘void MainWindow::on_groupBox_video_settings_clicked()’
 2209 | void MainWindow::on_groupBox_video_settings_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:2209:6: note: no functions named ‘void MainWindow::on_groupBox_video_settings_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2241:6: error: no declaration matches ‘void MainWindow::on_pushButton_TurnOffScreen_clicked()’
 2241 | void MainWindow::on_pushButton_TurnOffScreen_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:2241:6: note: no functions named ‘void MainWindow::on_pushButton_TurnOffScreen_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:2270:6: error: no declaration matches ‘void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked()’
 2270 | void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:2270:6: note: no functions named ‘void MainWindow::on_pushButton_MultipleOfFPS_VFI_MIN_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::on_pushButton_DetectGPU_RealCUGAN_clicked()’:
mainwindow.cpp:2309:21: error: ‘class GpuManager’ has no member named ‘isCompatible’
 2309 |     if (!gpuManager.isCompatible("realcugan")) { // Assuming GpuManager has such a check
      |                     ^~~~~~~~~~~~
mainwindow.cpp:2314:44: error: ‘class GpuManager’ has no member named ‘detectGpus’; did you mean ‘detectGPUs’?
 2314 |     Available_GPUID_RealCUGAN = gpuManager.detectGpus("realcugan"); // Or a more specific method
      |                                            ^~~~~~~~~~
      |                                            detectGPUs
mainwindow.cpp: At global scope:
mainwindow.cpp:2395:6: error: no declaration matches ‘void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked()’
 2395 | void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:2395:6: note: no functions named ‘void MainWindow::on_pushButton_MultipleOfFPS_VFI_ADD_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::PreLoad_Engines_Settings()’:
mainwindow.cpp:2430:12: error: ‘class Ui::MainWindow’ has no member named ‘comboBox_Model_VFI_cuda’; did you mean ‘comboBox_Model_VFI’?
 2430 |     if(ui->comboBox_Model_VFI_cuda && ui->comboBox_Model_VFI_cuda->count() <= 0) SRMD_CUDA_PreLoad_Settings_Str += tr("SRMD model list is empty.") + "\n";
      |            ^~~~~~~~~~~~~~~~~~~~~~~
      |            comboBox_Model_VFI
mainwindow.cpp:2430:43: error: ‘class Ui::MainWindow’ has no member named ‘comboBox_Model_VFI_cuda’; did you mean ‘comboBox_Model_VFI’?
 2430 |     if(ui->comboBox_Model_VFI_cuda && ui->comboBox_Model_VFI_cuda->count() <= 0) SRMD_CUDA_PreLoad_Settings_Str += tr("SRMD model list is empty.") + "\n";
      |                                           ^~~~~~~~~~~~~~~~~~~~~~~
      |                                           comboBox_Model_VFI
mainwindow.cpp:2435:89: error: ‘class Ui::MainWindow’ has no member named ‘comboBox_PlatformID_A4k’
 2435 |     if(ui->checkBox_GPUMode_Anime4K && ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_PlatformID_A4k && ui->comboBox_PlatformID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Platform ID list is empty.") + "\n";
      |                                                                                         ^~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:2435:120: error: ‘class Ui::MainWindow’ has no member named ‘comboBox_PlatformID_A4k’
 2435 |     if(ui->checkBox_GPUMode_Anime4K && ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_PlatformID_A4k && ui->comboBox_PlatformID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Platform ID list is empty.") + "\n";
      |                                                                                                                        ^~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:2436:89: error: ‘class Ui::MainWindow’ has no member named ‘comboBox_DeviceID_A4k’
 2436 |     if(ui->checkBox_GPUMode_Anime4K && ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_DeviceID_A4k && ui->comboBox_DeviceID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Device ID list is empty.") + "\n";
      |                                                                                         ^~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:2436:118: error: ‘class Ui::MainWindow’ has no member named ‘comboBox_DeviceID_A4k’
 2436 |     if(ui->checkBox_GPUMode_Anime4K && ui->checkBox_GPUMode_Anime4K->isChecked() && ui->comboBox_DeviceID_A4k && ui->comboBox_DeviceID_A4k->count() <= 0) Anime4KCPP_PreLoad_Settings_Str += tr("Anime4KCPP Device ID list is empty.") + "\n";
      |                                                                                                                      ^~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:2444:5: error: ‘Realsr_NCNN_Vulkan_PreLoad_Settings_Str’ was not declared in this scope; did you mean ‘Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str’?
 2444 |     Realsr_NCNN_Vulkan_PreLoad_Settings_Str = ""; // This is for the older RealSR tab, not RealESRGAN
      |     ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |     Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str
mainwindow.cpp: In member function ‘bool MainWindow::Check_PreLoad_Settings()’:
mainwindow.cpp:2554:12: error: ‘Realsr_NCNN_Vulkan_PreLoad_Settings_Str’ was not declared in this scope; did you mean ‘Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str’?
 2554 |         if(Realsr_NCNN_Vulkan_PreLoad_Settings_Str != "")
      |            ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |            Realesrgan_NCNN_Vulkan_PreLoad_Settings_Str
mainwindow.cpp: In member function ‘void MainWindow::APNG_Main(int, bool)’:
mainwindow.cpp:2637:79: error: ‘sourceFileFullPath’ was not declared in this scope; did you mean ‘sourceFileFullPath_dummy’?
 2637 |         case 0: APNG_Waifu2xNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
      |                                                                               ^~~~~~~~~~~~~~~~~~
      |                                                                               sourceFileFullPath_dummy
mainwindow.cpp:2637:124: error: ‘resultFileFullPath’ was not declared in this scope; did you mean ‘resultFileFullPath_dummy’?
 2637 |         case 0: APNG_Waifu2xNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
      |                                                                                                                            ^~~~~~~~~~~~~~~~~~
      |                                                                                                                            resultFileFullPath_dummy
mainwindow.cpp:2639:17: error: ‘APNG_SRMDNCNNVulkan’ was not declared in this scope; did you mean ‘APNG_SrmdNCNNVulkan’?
 2639 |         case 2: APNG_SRMDNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
      |                 ^~~~~~~~~~~~~~~~~~~
      |                 APNG_SrmdNCNNVulkan
mainwindow.cpp:2640:17: error: ‘APNG_Anime4K’ was not declared in this scope; did you mean ‘APNG_Anime4k’?
 2640 |         case 3: APNG_Anime4K(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
      |                 ^~~~~~~~~~~~
      |                 APNG_Anime4k
mainwindow.cpp:2642:17: error: ‘APNG_RealSRNCNNVulkan’ was not declared in this scope; did you mean ‘APNG_RealsrNCNNVulkan’?
 2642 |         case 5: APNG_RealSRNCNNVulkan(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
      |                 ^~~~~~~~~~~~~~~~~~~~~
      |                 APNG_RealsrNCNNVulkan
mainwindow.cpp:2643:17: error: ‘APNG_SRMDCuda’ was not declared in this scope; did you mean ‘APNG_SrmdCUDA’?
 2643 |         case 6: APNG_SRMDCuda(splitFramesFolder, scaledFramesFolder, sourceFileFullPath, framesFileName_qStrList, resultFileFullPath); break;
      |                 ^~~~~~~~~~~~~
      |                 APNG_SrmdCUDA
mainwindow.cpp:2622:32: warning: unused parameter ‘rowNum’ [-Wunused-parameter]
 2622 | void MainWindow::APNG_Main(int rowNum, bool isFromImageList)
      |                            ~~~~^~~~~~
mainwindow.cpp:2622:45: warning: unused parameter ‘isFromImageList’ [-Wunused-parameter]
 2622 | void MainWindow::APNG_Main(int rowNum, bool isFromImageList)
      |                                        ~~~~~^~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘bool MainWindow::FrameInterpolation(QString, QString)’:
mainwindow.cpp:3245:64: warning: unused parameter ‘OutputPath’ [-Wunused-parameter]
 3245 | bool MainWindow::FrameInterpolation(QString SourcePath,QString OutputPath)
      |                                                        ~~~~~~~~^~~~~~~~~~
mainwindow.cpp: In member function ‘bool MainWindow::Video_AutoSkip_CustRes(int)’:
mainwindow.cpp:3269:9: warning: unused variable ‘original_width’ [-Wunused-variable]
 3269 |     int original_width = metadata.width;
      |         ^~~~~~~~~~~~~~
mainwindow.cpp:3270:9: warning: unused variable ‘original_height’ [-Wunused-variable]
 3270 |     int original_height = metadata.height;
      |         ^~~~~~~~~~~~~~~
mainwindow.cpp: At global scope:
mainwindow.cpp:3286:6: error: no declaration matches ‘void MainWindow::TextBrowser_StartMes()’
 3286 | void MainWindow::TextBrowser_StartMes(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3286:6: note: no functions named ‘void MainWindow::TextBrowser_StartMes()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3305:6: error: no declaration matches ‘void MainWindow::progressbar_clear()’
 3305 | void MainWindow::progressbar_clear(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3305:6: note: no functions named ‘void MainWindow::progressbar_clear()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3307:6: error: no declaration matches ‘void MainWindow::on_pushButton_Start_clicked()’
 3307 | void MainWindow::on_pushButton_Start_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3307:6: note: no functions named ‘void MainWindow::on_pushButton_Start_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3310:6: error: no declaration matches ‘void MainWindow::on_pushButton_DetectGPU_clicked()’
 3310 | void MainWindow::on_pushButton_DetectGPU_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3310:6: note: no functions named ‘void MainWindow::on_pushButton_DetectGPU_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3311:6: error: no declaration matches ‘void MainWindow::on_pushButton_SaveSettings_clicked()’
 3311 | void MainWindow::on_pushButton_SaveSettings_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3311:6: note: no functions named ‘void MainWindow::on_pushButton_SaveSettings_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3312:6: error: no declaration matches ‘void MainWindow::on_pushButton_ResetSettings_clicked()’
 3312 | void MainWindow::on_pushButton_ResetSettings_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3312:6: note: no functions named ‘void MainWindow::on_pushButton_ResetSettings_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()’:
mainwindow.cpp:3315:21: error: ‘class GpuManager’ has no member named ‘isCompatible’
 3315 |     if (!gpuManager.isCompatible("realesrgan")) { // Assuming GpuManager has such a check
      |                     ^~~~~~~~~~~~
mainwindow.cpp:3319:57: error: ‘class GpuManager’ has no member named ‘detectGpus’; did you mean ‘detectGPUs’?
 3319 |     Available_GPUID_RealESRGAN_ncnn_vulkan = gpuManager.detectGpus("realesrgan");
      |                                                         ^~~~~~~~~~
      |                                                         detectGPUs
mainwindow.cpp: In member function ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan_valueChanged(int)’:
mainwindow.cpp:3429:92: warning: unused parameter ‘arg1’ [-Wunused-parameter]
 3429 | void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_RealsrNcnnVulkan_valueChanged(int arg1)
      |                                                                                        ~~~~^~~~
mainwindow.cpp: At global scope:
mainwindow.cpp:3449:6: error: no declaration matches ‘void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int)’
 3449 | void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3449:6: note: no functions named ‘void MainWindow::on_comboBox_TargetProcessor_converter_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3450:6: error: no declaration matches ‘void MainWindow::on_pushButton_DetectGPUID_srmd_clicked()’
 3450 | void MainWindow::on_pushButton_DetectGPUID_srmd_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3450:6: note: no functions named ‘void MainWindow::on_pushButton_DetectGPUID_srmd_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3451:6: error: no declaration matches ‘void MainWindow::on_pushButton_ListGPUs_Anime4k_clicked()’
 3451 | void MainWindow::on_pushButton_ListGPUs_Anime4k_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3451:6: note: no functions named ‘void MainWindow::on_pushButton_ListGPUs_Anime4k_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3452:6: error: redefinition of ‘void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()’
 3452 | void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3313:6: note: ‘void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()’ previously defined here
 3313 | void MainWindow::on_pushButton_DetectGPU_RealsrNCNNVulkan_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:3453:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_clicked()’
 3453 | void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3453:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3454:6: error: no declaration matches ‘void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan_currentIndexChanged(int)’
 3454 | void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan_currentIndexChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3454:6: note: no functions named ‘void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xNCNNVulkan_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3455:6: error: no declaration matches ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_valueChanged(int)’
 3455 | void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_valueChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3455:6: note: no functions named ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_valueChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3456:6: error: no declaration matches ‘void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_clicked()’
 3456 | void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3456:6: note: no functions named ‘void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3457:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_stateChanged(int)’
 3457 | void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_stateChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3457:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xNCNNVulkan_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3458:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_stateChanged(int)’
 3458 | void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_stateChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3458:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3459:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_clicked()’
 3459 | void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3459:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_SrmdNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3460:6: error: no declaration matches ‘void MainWindow::on_comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan_currentIndexChanged(int)’
 3460 | void MainWindow::on_comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan_currentIndexChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3460:6: note: no functions named ‘void MainWindow::on_comboBox_GPUIDs_MultiGPU_SrmdNCNNVulkan_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3461:6: error: no declaration matches ‘void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan_clicked()’
 3461 | void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3461:6: note: no functions named ‘void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_SrmdNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3462:6: error: no declaration matches ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan_valueChanged(int)’
 3462 | void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan_valueChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3462:6: note: no functions named ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_SrmdNCNNVulkan_valueChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3464:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_clicked()’
 3464 | void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:3464:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_RealsrNcnnVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3476:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_clicked()’
 3476 | void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3476:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3477:6: error: no declaration matches ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_stateChanged(int)’
 3477 | void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_stateChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3477:6: note: no functions named ‘void MainWindow::on_checkBox_MultiGPU_Waifu2xConverter_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3478:6: error: no declaration matches ‘void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xConverter_currentIndexChanged(int)’
 3478 | void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xConverter_currentIndexChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3478:6: note: no functions named ‘void MainWindow::on_comboBox_GPUIDs_MultiGPU_Waifu2xConverter_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3479:6: error: no declaration matches ‘void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter_clicked()’
 3479 | void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3479:6: note: no functions named ‘void MainWindow::on_checkBox_isEnable_CurrentGPU_MultiGPU_Waifu2xConverter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3480:6: error: no declaration matches ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter_valueChanged(int)’
 3480 | void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter_valueChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3480:6: note: no functions named ‘void MainWindow::on_spinBox_TileSize_CurrentGPU_MultiGPU_Waifu2xConverter_valueChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3481:6: error: no declaration matches ‘void MainWindow::on_checkBox_EnableMultiGPU_Waifu2xCaffe_stateChanged(int)’
 3481 | void MainWindow::on_checkBox_EnableMultiGPU_Waifu2xCaffe_stateChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3481:6: note: no functions named ‘void MainWindow::on_checkBox_EnableMultiGPU_Waifu2xCaffe_stateChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3482:6: error: no declaration matches ‘void MainWindow::on_comboBox_ProcessMode_Waifu2xCaffe_currentIndexChanged(int)’
 3482 | void MainWindow::on_comboBox_ProcessMode_Waifu2xCaffe_currentIndexChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3482:6: note: no functions named ‘void MainWindow::on_comboBox_ProcessMode_Waifu2xCaffe_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3483:6: error: no declaration matches ‘void MainWindow::on_lineEdit_GPUs_Anime4k_editingFinished()’
 3483 | void MainWindow::on_lineEdit_GPUs_Anime4k_editingFinished(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3483:6: note: no functions named ‘void MainWindow::on_lineEdit_GPUs_Anime4k_editingFinished()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3484:6: error: no declaration matches ‘void MainWindow::on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished()’
 3484 | void MainWindow::on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3484:6: note: no functions named ‘void MainWindow::on_lineEdit_MultiGPUInfo_Waifu2xCaffe_editingFinished()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3485:6: error: no declaration matches ‘void MainWindow::on_pushButton_VerifyGPUsConfig_Anime4k_clicked()’
 3485 | void MainWindow::on_pushButton_VerifyGPUsConfig_Anime4k_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3485:6: note: no functions named ‘void MainWindow::on_pushButton_VerifyGPUsConfig_Anime4k_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3486:6: error: no declaration matches ‘void MainWindow::on_pushButton_VerifyGPUsConfig_Waifu2xCaffe_clicked()’
 3486 | void MainWindow::on_pushButton_VerifyGPUsConfig_Waifu2xCaffe_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3486:6: note: no functions named ‘void MainWindow::on_pushButton_VerifyGPUsConfig_Waifu2xCaffe_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3487:6: error: no declaration matches ‘void MainWindow::on_tableView_image_doubleClicked(const QModelIndex&)’
 3487 | void MainWindow::on_tableView_image_doubleClicked(const QModelIndex&){}
      |      ^~~~~~~~~~
mainwindow.cpp:3487:6: note: no functions named ‘void MainWindow::on_tableView_image_doubleClicked(const QModelIndex&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3488:6: error: no declaration matches ‘void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex&)’
 3488 | void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex&){}
      |      ^~~~~~~~~~
mainwindow.cpp:3488:6: note: no functions named ‘void MainWindow::on_tableView_gif_doubleClicked(const QModelIndex&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3489:6: error: no declaration matches ‘void MainWindow::on_tableView_video_doubleClicked(const QModelIndex&)’
 3489 | void MainWindow::on_tableView_video_doubleClicked(const QModelIndex&){}
      |      ^~~~~~~~~~
mainwindow.cpp:3489:6: note: no functions named ‘void MainWindow::on_tableView_video_doubleClicked(const QModelIndex&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3491:6: error: no declaration matches ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan_clicked()’
 3491 | void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3491:6: note: no functions named ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3492:6: error: no declaration matches ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xConverter_clicked()’
 3492 | void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xConverter_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3492:6: note: no functions named ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_Waifu2xConverter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3493:6: error: no declaration matches ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan_clicked()’
 3493 | void MainWindow::on_pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3493:6: note: no functions named ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_SrmdNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3494:6: error: redefinition of ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan_clicked()’
 3494 | void MainWindow::on_pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3435:6: note: ‘void MainWindow::on_pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan_clicked()’ previously defined here
 3435 | void MainWindow::on_pushButton_ShowMultiGPUSettings_RealsrNcnnVulkan_clicked()
      |      ^~~~~~~~~~
mainwindow.cpp:3495:6: error: no declaration matches ‘void MainWindow::on_tableView_image_pressed(const QModelIndex&)’
 3495 | void MainWindow::on_tableView_image_pressed(const QModelIndex&){}
      |      ^~~~~~~~~~
mainwindow.cpp:3495:6: note: no functions named ‘void MainWindow::on_tableView_image_pressed(const QModelIndex&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3496:6: error: no declaration matches ‘void MainWindow::on_tableView_gif_pressed(const QModelIndex&)’
 3496 | void MainWindow::on_tableView_gif_pressed(const QModelIndex&){}
      |      ^~~~~~~~~~
mainwindow.cpp:3496:6: note: no functions named ‘void MainWindow::on_tableView_gif_pressed(const QModelIndex&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3497:6: error: no declaration matches ‘void MainWindow::on_tableView_video_pressed(const QModelIndex&)’
 3497 | void MainWindow::on_tableView_video_pressed(const QModelIndex&){}
      |      ^~~~~~~~~~
mainwindow.cpp:3497:6: note: no functions named ‘void MainWindow::on_tableView_video_pressed(const QModelIndex&)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3498:6: error: no declaration matches ‘void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int)’
 3498 | void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int){}
      |      ^~~~~~~~~~
mainwindow.cpp:3498:6: note: no functions named ‘void MainWindow::on_comboBox_ImageSaveFormat_currentIndexChanged(int)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3499:6: error: no declaration matches ‘void MainWindow::on_pushButton_TileSize_Add_W2xNCNNVulkan_clicked()’
 3499 | void MainWindow::on_pushButton_TileSize_Add_W2xNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3499:6: note: no functions named ‘void MainWindow::on_pushButton_TileSize_Add_W2xNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3500:6: error: no declaration matches ‘void MainWindow::on_pushButton_TileSize_Minus_W2xNCNNVulkan_clicked()’
 3500 | void MainWindow::on_pushButton_TileSize_Minus_W2xNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3500:6: note: no functions named ‘void MainWindow::on_pushButton_TileSize_Minus_W2xNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3501:6: error: no declaration matches ‘void MainWindow::on_pushButton_BlockSize_Add_W2xConverter_clicked()’
 3501 | void MainWindow::on_pushButton_BlockSize_Add_W2xConverter_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3501:6: note: no functions named ‘void MainWindow::on_pushButton_BlockSize_Add_W2xConverter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3502:6: error: no declaration matches ‘void MainWindow::on_pushButton_BlockSize_Minus_W2xConverter_clicked()’
 3502 | void MainWindow::on_pushButton_BlockSize_Minus_W2xConverter_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3502:6: note: no functions named ‘void MainWindow::on_pushButton_BlockSize_Minus_W2xConverter_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3503:6: error: no declaration matches ‘void MainWindow::on_pushButton_Add_TileSize_SrmdNCNNVulkan_clicked()’
 3503 | void MainWindow::on_pushButton_Add_TileSize_SrmdNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3503:6: note: no functions named ‘void MainWindow::on_pushButton_Add_TileSize_SrmdNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3504:6: error: no declaration matches ‘void MainWindow::on_pushButton_Minus_TileSize_SrmdNCNNVulkan_clicked()’
 3504 | void MainWindow::on_pushButton_Minus_TileSize_SrmdNCNNVulkan_clicked(){}
      |      ^~~~~~~~~~
mainwindow.cpp:3504:6: note: no functions named ‘void MainWindow::on_pushButton_Minus_TileSize_SrmdNCNNVulkan_clicked()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::Read_urls(QList<QUrl>)’:
mainwindow.cpp:3544:78: warning: ignoring return value of ‘auto QtConcurrent::run(Function&&, Args&& ...) [with Function = MainWindow::Read_urls(QList<QUrl>)::<lambda()>; Args = {}]’, declared with attribute ‘nodiscard’ [-Wunused-result]
 3544 |     QtConcurrent::run([this, urls] { this->ProcessDroppedFilesAsync(urls); });
      |                                                                              ^
/usr/include/x86_64-linux-gnu/qt6/QtConcurrent/qtconcurrentrun.h:55:6: note: declared here
   55 | auto run(Function &&f, Args &&...args)
      |      ^~~
mainwindow.cpp: At global scope:
mainwindow.cpp:3581:6: error: no declaration matches ‘void MainWindow::onRealCuganProcessFinished(int, QProcess::ExitStatus)’
 3581 | void MainWindow::onRealCuganProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
      |      ^~~~~~~~~~
mainwindow.cpp:3581:6: note: no functions named ‘void MainWindow::onRealCuganProcessFinished(int, QProcess::ExitStatus)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3592:6: error: no declaration matches ‘void MainWindow::onRealCuganProcessError(QProcess::ProcessError)’
 3592 | void MainWindow::onRealCuganProcessError(QProcess::ProcessError error) {
      |      ^~~~~~~~~~
mainwindow.cpp:3592:6: note: no functions named ‘void MainWindow::onRealCuganProcessError(QProcess::ProcessError)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3600:6: error: no declaration matches ‘void MainWindow::onRealCuganProcessStdOut()’
 3600 | void MainWindow::onRealCuganProcessStdOut() {
      |      ^~~~~~~~~~
mainwindow.cpp:3600:6: note: no functions named ‘void MainWindow::onRealCuganProcessStdOut()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3607:6: error: no declaration matches ‘void MainWindow::onRealCuganProcessStdErr()’
 3607 | void MainWindow::onRealCuganProcessStdErr() {
      |      ^~~~~~~~~~
mainwindow.cpp:3607:6: note: no functions named ‘void MainWindow::onRealCuganProcessStdErr()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3616:6: error: no declaration matches ‘void MainWindow::onRealESRGANProcessFinished(int, QProcess::ExitStatus)’
 3616 | void MainWindow::onRealESRGANProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
      |      ^~~~~~~~~~
mainwindow.cpp:3616:6: note: no functions named ‘void MainWindow::onRealESRGANProcessFinished(int, QProcess::ExitStatus)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3624:6: error: no declaration matches ‘void MainWindow::onRealESRGANProcessError(QProcess::ProcessError)’
 3624 | void MainWindow::onRealESRGANProcessError(QProcess::ProcessError error) {
      |      ^~~~~~~~~~
mainwindow.cpp:3624:6: note: no functions named ‘void MainWindow::onRealESRGANProcessError(QProcess::ProcessError)’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3632:6: error: no declaration matches ‘void MainWindow::onRealESRGANProcessStdOut()’
 3632 | void MainWindow::onRealESRGANProcessStdOut() {
      |      ^~~~~~~~~~
mainwindow.cpp:3632:6: note: no functions named ‘void MainWindow::onRealESRGANProcessStdOut()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp:3639:6: error: no declaration matches ‘void MainWindow::onRealESRGANProcessStdErr()’
 3639 | void MainWindow::onRealESRGANProcessStdErr() {
      |      ^~~~~~~~~~
mainwindow.cpp:3639:6: note: no functions named ‘void MainWindow::onRealESRGANProcessStdErr()’
mainwindow.h:111:7: note: ‘class MainWindow’ defined here
  111 | class MainWindow : public QMainWindow
      |       ^~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::Realcugan_NCNN_Vulkan_Image(int, bool, bool)’:
mainwindow.cpp:3659:46: error: ‘class FileManager’ has no member named ‘generateOutputFilePath’
 3659 |     QString outputFileFullPath = fileManager.generateOutputFilePath(sourceFileFullPath, आउटपुटFolder_main, outputFormat);
      |                                              ^~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3659:89: error: ‘आउटपुटFolder_main’ was not declared in this scope
 3659 |     QString outputFileFullPath = fileManager.generateOutputFilePath(sourceFileFullPath, आउटपुटFolder_main, outputFormat);
      |                                                                                         ^~~~~~~~~~~~~~~~
mainwindow.cpp:3665:17: error: ‘class FileManager’ has no member named ‘ensureOutputDirectoryExists’
 3665 |     fileManager.ensureOutputDirectoryExists(outputFileFullPath);
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3672:13: error: ‘class Ui::MainWindow’ has no member named ‘spinBox_Scale_RealCUGAN’
 3672 |         ui->spinBox_Scale_RealCUGAN ? ui->spinBox_Scale_RealCUGAN->value() : 2, // Default scale if UI not found
      |             ^~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3672:43: error: ‘class Ui::MainWindow’ has no member named ‘spinBox_Scale_RealCUGAN’
 3672 |         ui->spinBox_Scale_RealCUGAN ? ui->spinBox_Scale_RealCUGAN->value() : 2, // Default scale if UI not found
      |                                           ^~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3684:64: error: no matching function for call to ‘RealCuganProcessor::executablePath()’
 3684 |     QString executablePath = realCuganProcessor->executablePath();
      |                              ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~
In file included from mainwindow.cpp:22:
RealCuganProcessor.h:34:13: note: candidate: ‘QString RealCuganProcessor::executablePath(bool) const’
   34 |     QString executablePath(bool experimental) const;
      |             ^~~~~~~~~~~~~~
RealCuganProcessor.h:34:13: note:   candidate expects 1 argument, 0 provided
mainwindow.cpp:3699:104: error: ‘onRealCuganProcessFinished’ is not a member of ‘MainWindow’
 3699 |     connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::onRealCuganProcessFinished);
      |                                                                                                        ^~~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3700:67: error: ‘onRealCuganProcessError’ is not a member of ‘MainWindow’
 3700 |     connect(process, &QProcess::errorOccurred, this, &MainWindow::onRealCuganProcessError);
      |                                                                   ^~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3701:77: error: ‘onRealCuganProcessStdOut’ is not a member of ‘MainWindow’
 3701 |     connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::onRealCuganProcessStdOut);
      |                                                                             ^~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3702:76: error: ‘onRealCuganProcessStdErr’ is not a member of ‘MainWindow’
 3702 |     connect(process, &QProcess::readyReadStandardError, this, &MainWindow::onRealCuganProcessStdErr);
      |                                                                            ^~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3648:82: warning: unused parameter ‘ReProcess_MissingAlphaChannel’ [-Wunused-parameter]
 3648 | void MainWindow::Realcugan_NCNN_Vulkan_Image(int rowNum, bool experimental, bool ReProcess_MissingAlphaChannel)
      |                                                                             ~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings()’:
mainwindow.cpp:3736:25: error: ‘class RealCuganProcessor’ has no member named ‘setUiPointers’
 3736 |     realCuganProcessor->setUiPointers(
      |                         ^~~~~~~~~~~~~
mainwindow.cpp:3748:45: error: ‘class RealCuganProcessor’ has no member named ‘getModel’
 3748 |     m_realcugan_Model = realCuganProcessor->getModel();
      |                                             ^~~~~~~~
mainwindow.cpp:3749:52: error: ‘class RealCuganProcessor’ has no member named ‘getDenoiseLevel’
 3749 |     m_realcugan_DenoiseLevel = realCuganProcessor->getDenoiseLevel();
      |                                                    ^~~~~~~~~~~~~~~
mainwindow.cpp:3750:48: error: ‘class RealCuganProcessor’ has no member named ‘getTileSize’
 3750 |     m_realcugan_TileSize = realCuganProcessor->getTileSize();
      |                                                ^~~~~~~~~~~
mainwindow.cpp:3751:43: error: ‘class RealCuganProcessor’ has no member named ‘getTTA’
 3751 |     m_realcugan_TTA = realCuganProcessor->getTTA();
      |                                           ^~~~~~
mainwindow.cpp:3752:45: error: ‘class RealCuganProcessor’ has no member named ‘getGPUID’
 3752 |     m_realcugan_GPUID = realCuganProcessor->getGPUID();
      |                                             ^~~~~~~~
mainwindow.cpp:3753:57: error: ‘class RealCuganProcessor’ has no member named ‘getGpuJobConfig’
 3753 |     m_realcugan_gpuJobConfig_temp = realCuganProcessor->getGpuJobConfig();
      |                                                         ^~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::Realcugan_NCNN_Vulkan_ReadSettings_Video_GIF(int)’:
mainwindow.cpp:3775:45: error: ‘class RealCuganProcessor’ has no member named ‘getModel’
 3775 |     m_realcugan_Model = realCuganProcessor->getModel();
      |                                             ^~~~~~~~
mainwindow.cpp:3776:52: error: ‘class RealCuganProcessor’ has no member named ‘getDenoiseLevel’
 3776 |     m_realcugan_DenoiseLevel = realCuganProcessor->getDenoiseLevel();
      |                                                    ^~~~~~~~~~~~~~~
mainwindow.cpp:3777:48: error: ‘class RealCuganProcessor’ has no member named ‘getTileSize’
 3777 |     m_realcugan_TileSize = realCuganProcessor->getTileSize();
      |                                                ^~~~~~~~~~~
mainwindow.cpp:3778:43: error: ‘class RealCuganProcessor’ has no member named ‘getTTA’
 3778 |     m_realcugan_TTA = realCuganProcessor->getTTA();
      |                                           ^~~~~~
mainwindow.cpp:3779:45: error: ‘class RealCuganProcessor’ has no member named ‘getGPUID’
 3779 |     m_realcugan_GPUID = realCuganProcessor->getGPUID(); // This might be more complex for video/gif batch
      |                                             ^~~~~~~~
mainwindow.cpp:3780:57: error: ‘class RealCuganProcessor’ has no member named ‘getGpuJobConfig’
 3780 |     m_realcugan_gpuJobConfig_temp = realCuganProcessor->getGpuJobConfig();
      |                                                         ^~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int, bool)’:
mainwindow.cpp:3795:46: error: ‘class FileManager’ has no member named ‘generateOutputFilePath’
 3795 |     QString outputFileFullPath = fileManager.generateOutputFilePath(sourceFileFullPath, OutPutFolder_main, outputFormat);
      |                                              ^~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3801:17: error: ‘class FileManager’ has no member named ‘ensureOutputDirectoryExists’
 3801 |     fileManager.ensureOutputDirectoryExists(outputFileFullPath);
      |                 ^~~~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3837:104: error: ‘onRealESRGANProcessFinished’ is not a member of ‘MainWindow’
 3837 |     connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::onRealESRGANProcessFinished);
      |                                                                                                        ^~~~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3838:67: error: ‘onRealESRGANProcessError’ is not a member of ‘MainWindow’
 3838 |     connect(process, &QProcess::errorOccurred, this, &MainWindow::onRealESRGANProcessError);
      |                                                                   ^~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3839:77: error: ‘onRealESRGANProcessStdOut’ is not a member of ‘MainWindow’
 3839 |     connect(process, &QProcess::readyReadStandardOutput, this, &MainWindow::onRealESRGANProcessStdOut);
      |                                                                             ^~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3840:76: error: ‘onRealESRGANProcessStdErr’ is not a member of ‘MainWindow’
 3840 |     connect(process, &QProcess::readyReadStandardError, this, &MainWindow::onRealESRGANProcessStdErr);
      |                                                                            ^~~~~~~~~~~~~~~~~~~~~~~~~
mainwindow.cpp:3784:64: warning: unused parameter ‘ReProcess_MissingAlphaChannel’ [-Wunused-parameter]
 3784 | void MainWindow::RealESRGAN_NCNN_Vulkan_Image(int rowNum, bool ReProcess_MissingAlphaChannel)
      |                                                           ~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~
make: *** [Makefile:790: progressBar.o] Error 1
make: *** Waiting for unfinished jobs....
mainwindow.cpp: In member function ‘int MainWindow::Waifu2x_NCNN_Vulkan_Image(int, bool)’:
mainwindow.cpp:3964:5: error: ‘QTableWidgetItem’ was not declared in this scope; did you mean ‘QWidgetItem’?
 3964 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |     ^~~~~~~~~~~~~~~~
      |     QWidgetItem
mainwindow.cpp:3964:23: error: ‘itemIn’ was not declared in this scope
 3964 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |                       ^~~~~~
mainwindow.cpp:3964:36: error: ‘class Ui::MainWindow’ has no member named ‘tableWidget_Files’
 3964 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |                                    ^~~~~~~~~~~~~~~~~
mainwindow.cpp:3965:23: error: ‘itemOut’ was not declared in this scope
 3965 |     QTableWidgetItem *itemOut = ui->tableWidget_Files->item(rowNum, 1);
      |                       ^~~~~~~
mainwindow.cpp:3965:37: error: ‘class Ui::MainWindow’ has no member named ‘tableWidget_Files’
 3965 |     QTableWidgetItem *itemOut = ui->tableWidget_Files->item(rowNum, 1);
      |                                     ^~~~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘int MainWindow::Anime4k_Image(int, bool)’:
mainwindow.cpp:4004:5: error: ‘QTableWidgetItem’ was not declared in this scope; did you mean ‘QWidgetItem’?
 4004 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |     ^~~~~~~~~~~~~~~~
      |     QWidgetItem
make: *** [Makefile:760: image.o] Error 1
mainwindow.cpp:4004:23: error: ‘itemIn’ was not declared in this scope
 4004 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |                       ^~~~~~
mainwindow.cpp:4004:36: error: ‘class Ui::MainWindow’ has no member named ‘tableWidget_Files’
 4004 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |                                    ^~~~~~~~~~~~~~~~~
mainwindow.cpp:4005:23: error: ‘itemOut’ was not declared in this scope
 4005 |     QTableWidgetItem *itemOut = ui->tableWidget_Files->item(rowNum, 1);
      |                       ^~~~~~~
mainwindow.cpp:4005:37: error: ‘class Ui::MainWindow’ has no member named ‘tableWidget_Files’
 4005 |     QTableWidgetItem *itemOut = ui->tableWidget_Files->item(rowNum, 1);
      |                                     ^~~~~~~~~~~~~~~~~
mainwindow.cpp: In member function ‘int MainWindow::Get_NumOfGPU_Anime4k()’:
mainwindow.cpp:4050:59: error: ‘QRegExp’ was not declared in this scope
 4050 |     QStringList lines = QString::fromLocal8Bit(out).split(QRegExp("[\r\n]"),
      |                                                           ^~~~~~~
mainwindow.cpp: In member function ‘int MainWindow::Waifu2x_Converter_Image(int, bool)’:
mainwindow.cpp:4063:5: error: ‘QTableWidgetItem’ was not declared in this scope; did you mean ‘QWidgetItem’?
 4063 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |     ^~~~~~~~~~~~~~~~
      |     QWidgetItem
mainwindow.cpp:4063:23: error: ‘itemIn’ was not declared in this scope
 4063 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |                       ^~~~~~
mainwindow.cpp:4063:36: error: ‘class Ui::MainWindow’ has no member named ‘tableWidget_Files’
 4063 |     QTableWidgetItem *itemIn = ui->tableWidget_Files->item(rowNum, 0);
      |                                    ^~~~~~~~~~~~~~~~~
mainwindow.cpp:4064:23: error: ‘itemOut’ was not declared in this scope
 4064 |     QTableWidgetItem *itemOut = ui->tableWidget_Files->item(rowNum, 1);
      |                       ^~~~~~~
mainwindow.cpp:4064:37: error: ‘class Ui::MainWindow’ has no member named ‘tableWidget_Files’
 4064 |     QTableWidgetItem *itemOut = ui->tableWidget_Files->item(rowNum, 1);
      |                                     ^~~~~~~~~~~~~~~~~
make: *** [Makefile:741: files.o] Error 1
make: *** [Makefile:781: mainwindow.o] Error 1
