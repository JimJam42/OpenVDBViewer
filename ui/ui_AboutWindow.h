/********************************************************************************
** Form generated from reading UI file 'AboutWindow.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTWINDOW_H
#define UI_ABOUTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_AboutWindow
{
public:
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_close;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QLabel *lbl_logo;
    QSpacerItem *horizontalSpacer_4;
    QLabel *lbl_ovdbv_3;
    QLabel *lbl_version_3;
    QLabel *lbl_author_3;
    QTextBrowser *textBrowser_3;

    void setupUi(QDialog *AboutWindow)
    {
        if (AboutWindow->objectName().isEmpty())
            AboutWindow->setObjectName(QStringLiteral("AboutWindow"));
        AboutWindow->resize(279, 363);
        gridLayout = new QGridLayout(AboutWindow);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 1, 0, 1, 1);

        btn_close = new QPushButton(AboutWindow);
        btn_close->setObjectName(QStringLiteral("btn_close"));

        gridLayout->addWidget(btn_close, 1, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 2, 1, 1);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_3, 1, 2, 1, 1);

        lbl_logo = new QLabel(AboutWindow);
        lbl_logo->setObjectName(QStringLiteral("lbl_logo"));
        lbl_logo->setPixmap(QPixmap(QString::fromUtf8(":/res/icon_128")));

        gridLayout_4->addWidget(lbl_logo, 1, 1, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_4, 1, 0, 1, 1);

        lbl_ovdbv_3 = new QLabel(AboutWindow);
        lbl_ovdbv_3->setObjectName(QStringLiteral("lbl_ovdbv_3"));
        QFont font;
        font.setPointSize(20);
        font.setBold(true);
        font.setWeight(75);
        lbl_ovdbv_3->setFont(font);
        lbl_ovdbv_3->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(lbl_ovdbv_3, 0, 0, 1, 3);

        lbl_version_3 = new QLabel(AboutWindow);
        lbl_version_3->setObjectName(QStringLiteral("lbl_version_3"));
        lbl_version_3->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(lbl_version_3, 2, 0, 1, 3);

        lbl_author_3 = new QLabel(AboutWindow);
        lbl_author_3->setObjectName(QStringLiteral("lbl_author_3"));
        lbl_author_3->setAlignment(Qt::AlignCenter);

        gridLayout_4->addWidget(lbl_author_3, 3, 0, 1, 3);

        textBrowser_3 = new QTextBrowser(AboutWindow);
        textBrowser_3->setObjectName(QStringLiteral("textBrowser_3"));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 0));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(237, 237, 237, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        textBrowser_3->setPalette(palette);
        textBrowser_3->setFrameShape(QFrame::NoFrame);
        textBrowser_3->setFrameShadow(QFrame::Plain);
        textBrowser_3->setTextInteractionFlags(Qt::NoTextInteraction);

        gridLayout_4->addWidget(textBrowser_3, 4, 0, 1, 3);


        gridLayout->addLayout(gridLayout_4, 0, 0, 1, 3);


        retranslateUi(AboutWindow);

        QMetaObject::connectSlotsByName(AboutWindow);
    } // setupUi

    void retranslateUi(QDialog *AboutWindow)
    {
        AboutWindow->setWindowTitle(QApplication::translate("AboutWindow", "About OpenVDBViewer", 0));
        btn_close->setText(QApplication::translate("AboutWindow", "Close", 0));
        lbl_logo->setText(QString());
        lbl_ovdbv_3->setText(QApplication::translate("AboutWindow", "OpenVDBViewer", 0));
        lbl_version_3->setText(QApplication::translate("AboutWindow", "version 1.0.0", 0));
        lbl_author_3->setText(QApplication::translate("AboutWindow", "Author: Callum James James", 0));
        textBrowser_3->setHtml(QApplication::translate("AboutWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'.Lucida Grande UI'; font-size:13pt; font-weight:400; font-style:normal;\">\n"
"<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">For any bug reports, improvements, fixes or anything else, email calj.james@btinternet.com</p></body></html>", 0));
    } // retranslateUi

};

namespace Ui {
    class AboutWindow: public Ui_AboutWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTWINDOW_H
