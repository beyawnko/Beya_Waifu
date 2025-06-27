/********************************************************************************
** Form generated from reading UI file 'topsupporterslist.ui'
**
** Created by: Qt User Interface Compiler version 6.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TOPSUPPORTERSLIST_H
#define UI_TOPSUPPORTERSLIST_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TopSupportersList
{
public:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser_SupportersNameList;
    QPushButton *pushButton_BecomePatron;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *TopSupportersList)
    {
        if (TopSupportersList->objectName().isEmpty())
            TopSupportersList->setObjectName("TopSupportersList");
        TopSupportersList->resize(500, 400);
        TopSupportersList->setMinimumSize(QSize(500, 400));
        gridLayout = new QGridLayout(TopSupportersList);
        gridLayout->setObjectName("gridLayout");
        textBrowser_SupportersNameList = new QTextBrowser(TopSupportersList);
        textBrowser_SupportersNameList->setObjectName("textBrowser_SupportersNameList");
        textBrowser_SupportersNameList->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(textBrowser_SupportersNameList, 0, 0, 1, 3);

        pushButton_BecomePatron = new QPushButton(TopSupportersList);
        pushButton_BecomePatron->setObjectName("pushButton_BecomePatron");
        pushButton_BecomePatron->setMinimumSize(QSize(180, 45));
        pushButton_BecomePatron->setStyleSheet(QString::fromUtf8("QPushButton{\n"
"background-color: rgb(255, 66, 77);\n"
"color: rgb(255, 255, 255);\n"
"border-style:outset;\n"
"border-radius:8px;\n"
"padding:10px;\n"
"}\n"
"QPushButton:hover{\n"
"background-color: rgb(255, 105, 112);\n"
"color: rgb(255, 255, 255);\n"
"border-style:outset;\n"
"border-radius:8px;\n"
"padding:10px;\n"
"}\n"
"QPushButton:pressed{\n"
"background-color: rgb(223, 50, 61);\n"
"color: rgb(255, 255, 255);\n"
"border-style:outset;\n"
"border-radius:8px;\n"
"padding:10px;\n"
"}\n"
"QPushButton:disabled{\n"
"background-color: rgb(166, 166, 166);\n"
"color: rgb(255, 255, 255);\n"
"border-style:outset;\n"
"border-radius:8px;\n"
"padding:10px;\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new/prefix1/icon/patreon.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_BecomePatron->setIcon(icon);

        gridLayout->addWidget(pushButton_BecomePatron, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 2, 1, 1);


        retranslateUi(TopSupportersList);

        QMetaObject::connectSlotsByName(TopSupportersList);
    } // setupUi

    void retranslateUi(QWidget *TopSupportersList)
    {
        TopSupportersList->setWindowTitle(QCoreApplication::translate("TopSupportersList", "Top Supporters @Beya_Waifu", nullptr));
        pushButton_BecomePatron->setText(QCoreApplication::translate("TopSupportersList", " Become a patron", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TopSupportersList: public Ui_TopSupportersList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TOPSUPPORTERSLIST_H
