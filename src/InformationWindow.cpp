/*
  Copyright (C) 2014 Callum James

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "InformationWindow.h"
#include "ui_Information.h"

#include <iostream>

// Most of the functions in this class are setters for attributes and UI labels

InformationWindow::InformationWindow(QWidget *parent) :QWidget(parent),  m_ui(new Ui::Information)
{
    m_ui->setupUi(this);
    m_mainWindowConnected = false;
}

InformationWindow::~InformationWindow()
{
    // ensure table is removed before deletion
    removeInfoTable();
    clearInfoTable();
    delete m_ui;
}

void InformationWindow::setMainWindow(QWidget *_mainWindow)
{
    // set main window parent
    m_mainWindow = _mainWindow;
    m_mainWindowConnected = true;
    // connect up signals and slots
    connectUI();
}

void InformationWindow::setGPUVendor(std::string _vendor)
{
    m_ui->lbl_GPUVendorValue->setText(_vendor.c_str());
}

void InformationWindow::setOpenGLVersion(std::string _version)
{
    m_ui->lbl_OpenGLVersionValue->setText(_version.c_str());
}

void InformationWindow::setRenderer(std::string _renderer)
{
    m_ui->lbl_RendererValue->setText(_renderer.c_str());
}

void InformationWindow::setTotalGPUMem(std::string _total)
{
    m_ui->lbl_TotalGPUMemValue->setText(_total.c_str());
}

void InformationWindow::setUsedGPUMem(std::string _used)
{
    m_ui->lbl_UsedGPUMemValue->setText(_used.c_str());
}

void InformationWindow::clearInfoTable()
{
    // clear the data
    m_ui->tble_FileInfo->clear();
    // as you are removing a row, the count will keep changing so keep looping whilst there are still rows
    while (m_ui->tble_FileInfo->rowCount() > 0)
    {
        // remove a row
        m_ui->tble_FileInfo->removeRow(0);
    }
}

void InformationWindow::removeInfoTable()
{
    for (int i = 0; i < m_ui->tble_FileInfo->rowCount(); ++i)
    {
        // delete each item in the table
        delete m_ui->tble_FileInfo->item(i,0);
        delete m_ui->tble_FileInfo->item(i,1);
    }
}

void InformationWindow::initInfoTable()
{
    // set up auto resizing in the table
    m_ui->tble_FileInfo->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void InformationWindow::insertTableRow(int _row)
{
    // insert row at the given index
    m_ui->tble_FileInfo->insertRow(_row);
}

void InformationWindow::setTableItem(int _row, int _column, QString _text)
{
    // create pointer from the item to set
    QTableWidgetItem *item = m_ui->tble_FileInfo->item(_row,_column);
    if (!item) // if the item doesnt already exist, create a new one
    {
        item = new QTableWidgetItem;
        m_ui->tble_FileInfo->setItem(_row,_column,item);
    }
    item->setText(_text); // set text
    if (_column == 0)
    {
        // if title column, set to bold
        QFont bold;
        bold.setBold(true);
        item->setFont(bold);
    }
}

void InformationWindow::quit(int _status)
{
    close();
    exit(_status);
}

void InformationWindow::connectUI()
{
    if (m_mainWindowConnected)
    {
        // allows update button to get right information
        connect(m_ui->btn_updateUsedMemory,SIGNAL(clicked(bool)),m_mainWindow,SLOT(updateUsedGPUMem()));
    }
    else
    {
        std::cerr<<"Attempting to connect update button on information window when no main window specified - aborting!!"<<std::endl;
        quit(EXIT_FAILURE);
    }
}

void InformationWindow::keyPressEvent(QKeyEvent *_event)
{
    switch (_event->key())
    {
        case Qt::Key_Escape : {close();break;}
    }
}
