OpenVDB Viewer - OpenGL
version 1.0.0 
Callum James 2014

########################################################################################
1. About OpenVDB Viewer - OpenGL

This application is used to open and view VDB files associated with the OpenVDB
library. Once open, a selection of tools can be used to visualise different
aspects of the file in different ways. This tools is used to inspect and analyse the
data within the file to help find problem areas or just find out how the data is stored.
All VDB files are supported, although larger files can run into problems with hardware
limitations dependant on the hardware you are using.

OpenVDB Viewer has been built on OpenGL and GLSL and is compatible on OpenGL versions 
4.0 and up - the same with GLSL.

Currently, the OpenVDB Viewer builds and runs on MacOSX 10.9 Mavericks and Linux.
Windows support has not been implemented but is one of the future development tracks.

########################################################################################
2. Application Requirements

There are a few dependencies needed to build and run the OpenVDB Viewer. These are 
listed below:

	-OpenVDB (http://www.openvdb.org/download/)
	-Qt 5.0 or later (for the UI) (http://qt-project.org/downloads)
	-TBB (https://www.threadingbuildingblocks.org/download)
	-OpenEXR/ ILMBase (http://www.openexr.com/downloads.html)
	-Boost (http://sourceforge.net/projects/boost/files/boost/1.55.0/)

Once installed, the locations of these can be specified in the .pro file (explained
below).

On MacOSX you will also need to ensure you have the OpenGL framework installed.

########################################################################################
3. Making the Application

To make the application, you first need to tell it where headers and libraries of its
dependancies are. There are two ways in which you can do this within the .pro file.

At the top of the .pro file there are a few options you can set. The first one is
AUTOFIND. This needs the value of either 0 or 1. IF set to 1, when building it will
attempt to find the headers and libraries itself by looking in the most common default
locations. In this case, the paths it will search are:

	Linux:
		Headers:
			/usr/include
			/usr/local/include
			/usr/local/include/openvdb
		Libraries:
			/usr/local/lib
	MacOSX:
		Headers:
			/usr/local/include
		Libraries:
			/usr/local/lib

If your distributions of any of the dependencies in 2 are not in these locations, you
will need to specify where they are. To do this, at the top of the .pro file, set the
following variables:

	OPENVDB_INC_PATH - root directory for the OpenVDB headers
	OPENVDB_LIB_PATH - directory where libopenvdb is located

	TBB_INC_PATH - root directory for the tbb headers
	TBB_LIB_PATH - directory where the libtbb library is located

	HALF_INC_PATH - root directory for Half.h
	HALF_LIB_PATH - directory where libHalf is located

	BOOST_INC_DIR - root directory for the Boost headers

The DEBUG option is default to 0. However if set to 1, the applicatio  will print out
more information about its processing as it goes along if you are interested. To
enable this, set this option to 1, else leave it as 0.

The other option you can set is the USE_EXT_GPU. Once set to 1, the application will
attempt to query an external device on its memory. Currently this only works on NVIDIA
cards. Therefore only set this to 1 if you have an external NVIDIA card attached and 
in use. If you do have this, set this to 0. The only functionality it will lose is the
ability to query the total and currently used GPU memory and display in the UI.

Once these have all been set, run in the directory:

	qmake
	make

Once built, dependant on your platform, run the appropriate application (either
executable on Linux or .app on MacOSX)

########################################################################################
4. Quick Use Guide

This is a very brief description of how to use the application. If you want a more
detailed manual with diagrams (pretty pictures!), please refer to the Manual included
within the Docs folder.

Use the File->Open action to select and open a VDB file. Once selected, a dialog will
ask you if you wish to load high resolution or not. If No, only the VDB tree and
information will be loaded and loading times will be quicker with less memory
consumption. If Yes is selected, the high resolution data will be loaded as well as
the VDB tree and information. This will take slightly longer (dependant on the number of
channels in the file) and will take more GPU and CPU memory.

Once open, you can use the options on the right to toggle attributes and manipulate
the data. For example to toggle the high resolution volume, use the 'Show High Resolution'
check box. To select the channel to render use the drop down box below this.

To rotate the volume, on the render window, left click and drag. To rotate the camera,
on the render window, hold Alt and left click and drag. To translate the volume,
on the render window, right click and drag. To zoom in and out, on the render window,
scroll the mouse wheel up and down.

To change the level of detail, use the slider at the bottom of the render tab.

Rendering the channels as a vector can be achieved from the channels tab at the bottom.

Colour ramps can be applied through the channels tab.

Culling can be applied through the Cull tab. Up to three culls can be applied at any
one time and the type of these culls selected with their corresponding drop down
selections.

The crop tab can be used to apply up to three crop boxes to the volume.

To display information about the file loaded and the hardware being used, either use
the Information menu item or click the Show Information button on the render tab.

########################################################################################
5. Some Useful Quick Shortcuts

There are a few useful shortcuts implemented into the application to make use easier.
In the following list, the shortcuts specified are for Linux and MacOSX respectively:

	Mouse click and drag - rotate the volume
	Alt+Mouse click and drag - rotate the camera
	Ctrl+O/cmd+O - Open a file
	Ctrl+Shift+C/cmd+Shift+C - Clear the scene
	Esc/cmd+Q - Quit application
	Del/Del - Remove high resolution mesh if loaded
	Ctrl+R/cmd+R - Reset Volume transform
	R/R - Reset scene (volume transform and camera)
	Shift+R/Shift+R - Reset Camera
		
########################################################################################
