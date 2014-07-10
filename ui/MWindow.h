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


#ifndef __MWINDOW_H__
#define __MWINDOW_H__

#include <QMainWindow>

namespace Ui {
class MWindow;
}

/// @file MWindow.h
/// @brief MWindow class is the class for the Main WIndow
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014
/// @class MWindow
/// @brief MWindow class contains the ui and ui access for the main window of the application
class MWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MWindow(QWidget *parent = 0);
    ~MWindow();

private slots:

private:
    Ui::MWindow *ui;
};

#endif /* __MWINDOW_H__ */
