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


#ifndef __MAINWINDOW_H__
#define __MAINWINDOW_H__

#include <QMainWindow>
#include <QDoubleSpinBox>
#include "InformationWindow.h"
#include "AboutWindow.h"
#include "GLWindow.h"

/**
 @mainpage OpenVDBViewer for Linux and Mac
 @section intro_sec Introduction
 This is the standalone OpenVDBViewer for Linux and Mac built with the Qt libraries and using OpenGL.
 The purpose of this applications is to open, view and scrunitise the data stored within a VDB file.
 It has been designed to be an easy to use, and visually pleasing and centred application that gives
 you quick access to a large amount of the data stored within a VDB file.
 The OpenVDBViewer presents you with a selection of tools can be used to visualise different
 aspects of the file in different ways. This tools is used to inspect and analyse the
 data within the file to help find problem areas or just find out how the data is stored.
 All VDB files are supported, although larger files can run into problems with hardware
 limitations dependant on the hardware you are using.

 OpenVDBViewer has been built on OpenGL and GLSL and is compatible on OpenGL versions
 4.0 and up - the same with GLSL.

 Currently, the OpenVDBViewer builds and runs on MacOSX 10.9 Mavericks and Linux.
 Windows support has not been implemented but is one of the future development tracks.

 @section requirements_sec Requirements
 There are a few dependencies needed to build and run the OpenVDB Viewer. These are
 listed below:

        - OpenVDB (http://www.openvdb.org/download/)
        - Qt 5.0 or later (for the UI) (http://qt-project.org/downloads)
        - TBB (https://www.threadingbuildingblocks.org/download)
        - OpenEXR/ ILMBase (http://www.openexr.com/downloads.html)
        - Boost (http://sourceforge.net/projects/boost/files/boost/1.55.0/)

 Once installed, the locations of these can be specified in the .pro file (explained
 below).

 On MacOSX you will also need to ensure you have the OpenGL framework installed.

 @section build_sec Building OpenVDBViewer
 To build the application, you first need to tell it where headers and libraries of its
 dependancies are. There are two ways in which you can do this within the .pro file.

 At the top of the .pro file there are a few options you can set. The first one is
 AUTOFIND. This needs the value of either 0 or 1. IF set to 1, when building it will
 attempt to find the headers and libraries itself by looking in the most common default
 locations. In this case, the paths it will search are:

     - Linux:
         - Headers:
             - /usr/include
             - /usr/local/include
             - /usr/local/include/openvdb
         - Libraries:
             - /usr/local/lib
     - MacOSX:
         - Headers:
             - /usr/local/include
         - Libraries:
             - /usr/local/lib

 If your distributions of any of the dependencies in 2 are not in these locations, you
 will need to specify where they are. To do this, at the top of the .pro file, set the
 following variables:

     - OPENVDB_INC_PATH - root directory for the OpenVDB headers
     - OPENVDB_LIB_PATH - directory where libopenvdb is located

     - TBB_INC_PATH - root directory for the tbb headers
     - TBB_LIB_PATH - directory where the libtbb library is located

     - HALF_INC_PATH - root directory for Half.h
     - HALF_LIB_PATH - directory where libHalf is located

     - BOOST_INC_DIR - root directory for the Boost headers

 The DEBUG option is default to 0. However if set to 1, the applicatio  will print out
 more information about its processing as it goes along if you are interested. To
 enable this, set this option to 1, else leave it as 0.

 The other option you can set is the USE_EXT_GPU. Once set to 1, the application will
 attempt to query an external device on its memory. Currently this only works on NVIDIA
 cards. Therefore only set this to 1 if you have an external NVIDIA card attached and
 in use. If you do have this, set this to 0. The only functionality it will lose is the
 ability to query the total and currently used GPU memory and display in the UI.

 Once these have all been set, run in the directory:

     - qmake
     - make

 Once built, dependant on your platform, run the appropriate application (either
 executable on Linux or .app on MacOSX)

 @section manual_sec User Manual
 The user manual for how to use the OpenVDBViewer can be found within the manual folder of the
 root directory for the application.
 **/


namespace Ui
{
    class MWindow;
}

/// @file MainWindow.h
/// @brief Class which controls all the UI functionality and Widget handling of the main window
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014
/// @class MainWindow
/// @brief Class to control window context for the main window of the application. Handles all UI interaction and interactions
/// with the GLWindow class
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /// @brief Constructor for the MainWindow class
    /// @param [in] parent QWidget* - parent of this class
    explicit MainWindow(QWidget *parent = 0);
    /// @brief Destructor of the MainWindow class
    ~MainWindow();
    /// @brief Initialise the GL Widget
    void initGLWidget();
    /// @brief Method to connect up all signals and slots
    void connectAndInit();
    /// @brief Set startup values
    void setStartup();
    /// @brief Set the crop box limits
    void setCropLimits();
    /// @brief Retrieve and display information about the hardware
    void getHardwareInformation();
    /// @brief Retrieve and display information about the file
    void getFileInformation();

public slots:
    /// @brief Method to set the vector length
    void setVectorLength();
    /// @brief Method to set the Level of Detail
    void setLOD();
    /// @brief Set the point size glPointSize
    void setPointSize();
    /// @brief Set the speed of the scan
    void setScanSpeed();
    /// @brief Set the line width glLineWidth
    void setLineWidth();
    /// @brief start a scan (currently only works for one axis, although expansion has been written in)
    void startSingleAxisScan();
    /// @brief Stop any running scans
    void stopScan();
    /// @brief Fill the combo box with the available crop boxes
    void fillNumSlices();
    /// @brief Pause and Resume scans dependant on their current state
    void pauseResumeScan();
    /// @brief Use crop boxes
    void useCrop();
    /// @brief Set the colour of vectors
    void setVectorColour();
    /// @brief Toggle the drawing of the mesh
    void toggleMesh();
    /// @brief Toggle whether or not a colour map is applied
    void setColourMapApplied();
    /// @brief Toggle whether vector colour selection boxes are enabled
    void toggleVectorColourEnabled();
    /// @brief Set inversion of colour boolean
    void setVectorInvert();
    /// @brief Set the channel to render from the front screen
    void setRenderChannelFromRenderTab();
    /// @brief Set the channel to use for the vectors
    void setVectorChannel();
    /// @brief Set the width of the X scan
    void setScanXWidth();
    /// @brief Set the height of the Y scan
    void setScanYHeight();
    /// @brief Set the depth of the Z scan
    void setScanZDepth();
    /// @brief Read crop details and set to current crop
    void setCrop();
    /// @brief Retrieve details of current crop and fill in value boxes
    void fillCropDetails();
    /// @brief Update the current used GPU memory in the information window
    void updateUsedGPUMem();
    /// @brief Open a VDB file from a file dialog
    void openFile();
    /// @brief Clear data
    void clear();
    /// @brief Quit the application
    void quitApp();
    /// @brief Remove only the high resolution mesh
    void removeMesh();
    /// @brief Set colour ramp onto points or channel
    void setRampPoints();
    /// @brief Set Colour of colour ramp
    void setRampColour();
    /// @brief Set the colour ramp to be user defined
    void setUserRamp();
    /// @brief Set the minimum of the colour ramp
    /// @param [in] _updateGL bool - whether to update GL when setting values or not
    void setRampMin(bool _updateGL=true);
    /// @brief Set the maximum of the colour ramp
    /// @param [in] _updateGL bool - whether to update GL when setting values or not
    void setRampMax(bool _updateGL=true);

private slots:
    /// @brief Reset the crop slider to 0 if released
    void resetCropSlider();
    /// @brief Set channel vectors enabled
    void toggleChannelVectors();
    /// @brief Start the timer for manual crop move
    void startCropSlideTimer();
    /// @brief Stop the timer for manual crop move
    void stopCropSlideTimer();
    /// @brief Set manual crop delta
    void updateCropSlideValue();
    /// @brief Set details for Cull 1
    void setCull1Type();
    /// @brief Set details for Cull 2
    void setCull2Type();
    /// @brief Set details for Cull 3
    void setCull3Type();
    /// @brief Set cull calculation type for Cull 1
    void setCull1Boolean();
    /// @brief Set cull calculation type for Cull 2
    void setCull2Boolean();
    /// @brief Set cull calculation type for Cull 3
    void setCull3Boolean();
    /// @brief Enable range selection boxes for cull 1
    /// @param [in] _enabled bool - set enabled or not
    void enableCull1RangeBoxes(bool _enabled);
    /// @brief Enable range selection boxes for cull 2
    ////// @param [in] _enabled bool - set enabled or not
    void enableCull2RangeBoxes(bool _enabled);
    /// @brief Enable range selection boxes for cull 3
    ////// @param [in] _enabled bool - set enabled or not
    void enableCull3RangeBoxes(bool _enabled);
    /// @brief Set culling enabled if at least 1 cull is active
    void setCullingEnabled();
    /// @brief Send details for Cull 1 to GLWindow
    void setCull1Values();
    /// @brief Send details for Cull 2 to GLWindow
    void setCull2Values();
    /// @brief Send details for Cull 3 to GLWindow
    void setCull3Values();
    /// @brief Set the ramp to a cull
    void setCullToChannel();
    /// @brief Specify which cull to apply the ramp tp
    void setCullToChannelNumber();
    /// @brief Show the information window
    void showInformation();
    /// @brief open the about dialog
    void openAbout();

private:
    /// @brief The ui for the main window
    Ui::MWindow *m_ui;
    /// @brief The information window
    InformationWindow m_infoWindow;
    /// @brief the About window
    AboutWindow m_aboutWindow;
    /// @brief GLWidget where all GL rendering happens
    GLWindow *m_glWindow;

private:
    /// @brief boolean to specify if the crop should be read or not
    bool m_readCrop;
    /// @brief Automatic toggle Crop settings
    void toggleCropSettings();
    /// @brief Manual toggle Crop settings
    /// @param [in] _enabled bool - set whether they are enabled or not
    void toggleCropSettings(bool _enabled);
    /// @brief Automatic toggle Scan controls
    void toggleScanControls();
    /// @brief Manual toggle Scan controls
    /// @param [in] _enabled bool - set whether they are enabled or not
    void toggleScanControls(bool _enabled);
    /// @brief Set which channel to render
    void setRenderChannel(int _index);
    /// @brief Enable the controls for the colour ramp
    void enableColourRampUserControls();
    /// @brief Event handler for a key press
    /// @param [in] _event QKeyEvent* - the triggered event
    void keyPressEvent(QKeyEvent *_event);
    /// @brief Event handler for a key release
    /// @param [in] _event QKeyEvent* - the triggered event
    void keyReleaseEvent(QKeyEvent *_event);
    /// @brief Event handler for a close event
    void closeEvent (QCloseEvent *);
    /// @brief Clear all channel combo boxes
    void clearChannelCombos();
    /// @brief Fill all channel combo boxes
    void fillChannelCombos();
    /// @brief Enable Channel selection combo boxes
    /// @param [in] _enabled bool - whether enabled or not
    void setChannelCombosEnabled(bool _enabled);
    /// @brief Enable Channel selection combo boxes
    /// @param [in] _enabled bool - whether enabled or not
    void setPointChannelsEnabled(bool _enabled);
    /// @brief Data storing which index from colour ramp to cull
    int m_cullToComboMap[3];
    /// @brief Boolean of whether to update GL when setting culls data
    bool m_renderNewCullColour;
    /// @brief Set extremes for Cull 1
    void setCull1Extremes();
    /// @brief Set extremes for Cull 2
    void setCull2Extremes();
    /// @brief Set extremes for Cull 3
    void setCull3Extremes();
    /// @brief Set the max and min values for the passed in spin box
    /// @param [in] _box QDoubleSpinBox* - the box to set the max and min values to
    /// @param [in] _min float - the minimum value to set
    /// @param [in] _max float - the maximum value to set
    void setCullSpinBoxesMaxMin(QDoubleSpinBox *_box, float _min, float _max);
    /// @brief Timer used for the crop slider manual move
    int m_cropSlideTimer;
    /// @brief Event triggered in every execution of the timer
    /// @param [in] QTimerEvent*
    void timerEvent(QTimerEvent *);
    /// @brief Clear the information table of data
    void clearInfoTable();
};

#endif /* __MAINWINDOW_H__ */
