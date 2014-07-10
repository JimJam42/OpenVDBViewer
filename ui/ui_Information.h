/********************************************************************************
** Form generated from reading UI file 'Information.ui'
**
** Created by: Qt User Interface Compiler version 5.2.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INFORMATION_H
#define UI_INFORMATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Information
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *gbox_hardwareInfo;
    QGridLayout *gridLayout_12;
    QLabel *lbl_OpenGLVersion;
    QLabel *lbl_RendererValue;
    QLabel *lbl_UsedGPUMemValue;
    QLabel *lbl_Renderer;
    QLabel *lbl_GPUVendorValue;
    QLabel *lbl_TotalGPUMem;
    QLabel *lbl_OpenGLVersionValue;
    QLabel *lbl_UsedGPUMem;
    QLabel *lbl_GPUVendor;
    QLabel *lbl_TotalGPUMemValue;
    QPushButton *btn_updateUsedMemory;
    QFrame *line;
    QGroupBox *gbox_fileInfo;
    QGridLayout *gridLayout_13;
    QTableWidget *tble_FileInfo;

    void setupUi(QWidget *Information)
    {
        if (Information->objectName().isEmpty())
            Information->setObjectName(QStringLiteral("Information"));
        Information->resize(354, 628);
        verticalLayout = new QVBoxLayout(Information);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gbox_hardwareInfo = new QGroupBox(Information);
        gbox_hardwareInfo->setObjectName(QStringLiteral("gbox_hardwareInfo"));
        gridLayout_12 = new QGridLayout(gbox_hardwareInfo);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        lbl_OpenGLVersion = new QLabel(gbox_hardwareInfo);
        lbl_OpenGLVersion->setObjectName(QStringLiteral("lbl_OpenGLVersion"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        lbl_OpenGLVersion->setFont(font);

        gridLayout_12->addWidget(lbl_OpenGLVersion, 1, 0, 1, 1);

        lbl_RendererValue = new QLabel(gbox_hardwareInfo);
        lbl_RendererValue->setObjectName(QStringLiteral("lbl_RendererValue"));

        gridLayout_12->addWidget(lbl_RendererValue, 2, 1, 1, 1);

        lbl_UsedGPUMemValue = new QLabel(gbox_hardwareInfo);
        lbl_UsedGPUMemValue->setObjectName(QStringLiteral("lbl_UsedGPUMemValue"));

        gridLayout_12->addWidget(lbl_UsedGPUMemValue, 4, 1, 1, 1);

        lbl_Renderer = new QLabel(gbox_hardwareInfo);
        lbl_Renderer->setObjectName(QStringLiteral("lbl_Renderer"));
        lbl_Renderer->setFont(font);

        gridLayout_12->addWidget(lbl_Renderer, 2, 0, 1, 1);

        lbl_GPUVendorValue = new QLabel(gbox_hardwareInfo);
        lbl_GPUVendorValue->setObjectName(QStringLiteral("lbl_GPUVendorValue"));

        gridLayout_12->addWidget(lbl_GPUVendorValue, 0, 1, 1, 1);

        lbl_TotalGPUMem = new QLabel(gbox_hardwareInfo);
        lbl_TotalGPUMem->setObjectName(QStringLiteral("lbl_TotalGPUMem"));
        lbl_TotalGPUMem->setFont(font);

        gridLayout_12->addWidget(lbl_TotalGPUMem, 3, 0, 1, 1);

        lbl_OpenGLVersionValue = new QLabel(gbox_hardwareInfo);
        lbl_OpenGLVersionValue->setObjectName(QStringLiteral("lbl_OpenGLVersionValue"));

        gridLayout_12->addWidget(lbl_OpenGLVersionValue, 1, 1, 1, 1);

        lbl_UsedGPUMem = new QLabel(gbox_hardwareInfo);
        lbl_UsedGPUMem->setObjectName(QStringLiteral("lbl_UsedGPUMem"));
        lbl_UsedGPUMem->setFont(font);

        gridLayout_12->addWidget(lbl_UsedGPUMem, 4, 0, 1, 1);

        lbl_GPUVendor = new QLabel(gbox_hardwareInfo);
        lbl_GPUVendor->setObjectName(QStringLiteral("lbl_GPUVendor"));
        lbl_GPUVendor->setFont(font);

        gridLayout_12->addWidget(lbl_GPUVendor, 0, 0, 1, 1);

        lbl_TotalGPUMemValue = new QLabel(gbox_hardwareInfo);
        lbl_TotalGPUMemValue->setObjectName(QStringLiteral("lbl_TotalGPUMemValue"));

        gridLayout_12->addWidget(lbl_TotalGPUMemValue, 3, 1, 1, 1);

        btn_updateUsedMemory = new QPushButton(gbox_hardwareInfo);
        btn_updateUsedMemory->setObjectName(QStringLiteral("btn_updateUsedMemory"));

        gridLayout_12->addWidget(btn_updateUsedMemory, 5, 1, 1, 1);


        verticalLayout->addWidget(gbox_hardwareInfo);

        line = new QFrame(Information);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        gbox_fileInfo = new QGroupBox(Information);
        gbox_fileInfo->setObjectName(QStringLiteral("gbox_fileInfo"));
        gridLayout_13 = new QGridLayout(gbox_fileInfo);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        tble_FileInfo = new QTableWidget(gbox_fileInfo);
        if (tble_FileInfo->columnCount() < 2)
            tble_FileInfo->setColumnCount(2);
        tble_FileInfo->setObjectName(QStringLiteral("tble_FileInfo"));
        tble_FileInfo->setFrameShape(QFrame::StyledPanel);
        tble_FileInfo->setFrameShadow(QFrame::Sunken);
        tble_FileInfo->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tble_FileInfo->setDragDropOverwriteMode(false);
        tble_FileInfo->setAlternatingRowColors(false);
        tble_FileInfo->setSelectionBehavior(QAbstractItemView::SelectItems);
        tble_FileInfo->setShowGrid(false);
        tble_FileInfo->setCornerButtonEnabled(true);
        tble_FileInfo->setRowCount(0);
        tble_FileInfo->setColumnCount(2);
        tble_FileInfo->horizontalHeader()->setVisible(false);
        tble_FileInfo->verticalHeader()->setVisible(false);
        tble_FileInfo->verticalHeader()->setHighlightSections(true);

        gridLayout_13->addWidget(tble_FileInfo, 0, 0, 1, 1);


        verticalLayout->addWidget(gbox_fileInfo);


        retranslateUi(Information);

        QMetaObject::connectSlotsByName(Information);
    } // setupUi

    void retranslateUi(QWidget *Information)
    {
        Information->setWindowTitle(QApplication::translate("Information", "Information", 0));
        gbox_hardwareInfo->setTitle(QApplication::translate("Information", "GPU Hardware Information", 0));
        lbl_OpenGLVersion->setText(QApplication::translate("Information", "OpenGL Version", 0));
        lbl_RendererValue->setText(QString());
        lbl_UsedGPUMemValue->setText(QString());
        lbl_Renderer->setText(QApplication::translate("Information", "Renderer", 0));
        lbl_GPUVendorValue->setText(QString());
        lbl_TotalGPUMem->setText(QApplication::translate("Information", "Total GPU Memory", 0));
        lbl_OpenGLVersionValue->setText(QString());
        lbl_UsedGPUMem->setText(QApplication::translate("Information", "Used GPU Memory", 0));
        lbl_GPUVendor->setText(QApplication::translate("Information", "GPU Vendor", 0));
        lbl_TotalGPUMemValue->setText(QString());
        btn_updateUsedMemory->setText(QApplication::translate("Information", "Update Used Memory", 0));
        gbox_fileInfo->setTitle(QApplication::translate("Information", "File Information", 0));
    } // retranslateUi

};

namespace Ui {
    class Information: public Ui_Information {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFORMATION_H
