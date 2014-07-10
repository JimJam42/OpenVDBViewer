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


#ifndef __INFORMATION_H__
#define __INFORMATION_H__

#include <QKeyEvent>
#include <QWidget>
#include <QTableWidget>

namespace Ui
{
    class Information;
}

/// @file InformationWindow.h
/// @brief Simple class to control and display file and hardware information in a separate window
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014
/// @class InformationWindow
/// @brief Class to control window context for the simple information window. Handles the display and control of data
/// both for the hardware and for the file
class InformationWindow : public QWidget
{
    Q_OBJECT

public:
    /// @brief Constructor of the InformationWindow class
    /// @param [in] parent QWidget* - parent of this class
    explicit InformationWindow(QWidget *parent = 0);
    /// @brief Destructor of the InformationWindow class
    ~InformationWindow();

    /// @brief Method to set a pointer to the main window to allow communication
    /// @param _mainWindow QWidget* - QWidget pointer to set as main window
    void setMainWindow(QWidget *_mainWindow);

    /// @brief Set the GPU Vendor label field
    /// @param [in] _vendor std::string - value to write to UI
    void setGPUVendor(std::string _vendor);
    /// @brief Set the OpenGL Version label field
    /// @param [in] _version std::string - value to write to UI
    void setOpenGLVersion(std::string _version);
    /// @brief Set the Renderer label field
    /// @param [in] _renderer std::string - value to write to UI
    void setRenderer(std::string _renderer);
    /// @brief Set the Total GPU Memory label field
    /// @param [in] _total std::string - value to write to UI
    void setTotalGPUMem(std::string _total);
    /// @brief Set the Used GPU Memory label field
    /// @param [in] _used std::string - value to write to UI
    void setUsedGPUMem(std::string _used);

    /// @brief Method to clear the information table
    void clearInfoTable();
    /// @brief Remove all items within the information table
    void removeInfoTable();
    /// @brief Method to initialise the information table
    void initInfoTable();
    /// @brief Insert a row into the information table at the given index
    /// @param [in] _row int - row to add new row at
    void insertTableRow(int _row);
    /// @brief Method to set the item within the table
    /// @param [in] _row int - row to add item at
    /// @param [in] _column int - column to add item at
    /// @param [in] _text QString - text to add to item
    void setTableItem(int _row, int _column, QString _text);

    /// @brief Quit the application
    /// @param [in] _status int - return exit status
    void quit(int _status);

private:
    /// @brief Method to connect up the UI signals and slots
    void connectUI();

    /// @brief Event handler for key presses
    /// @param _event QKeyEvent* - event that has occured
    void keyPressEvent(QKeyEvent *_event);

    /// @brief The ui in this window
    Ui::Information *m_ui;

    /// @brief Pointer to the main window widget
    QWidget *m_mainWindow;

    /// @brief Boolen of whether or not the main window has been set and connected
    bool m_mainWindowConnected;
};

#endif /* __INFORMATION_H__ */
