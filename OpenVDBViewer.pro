######################################################################################################
# AUTOFIND will attempt to automatically locate the correct headers and libraries needed
# by looking at the default most common folder for the required headers and libraries.
# Set to 1 if you wish to use this or set to 0 to use the paths declared below
AUTOFIND = 0
######################################################################################################
# DEBUG, if set to 1, will print out some more information about the workings of the application
# and file as it runs. If set to 0, the application will be much quieter when running
DEBUG = 0
######################################################################################################
# USE_EXT_GPU will tell the application if it is using an external device or onboard chipset.
# Whilst it will make no differnce to the working of the application, it will change defines
# used and enable/disable device query functions used within the application.
# Currently the only supported external device is NVidia (automatically defined by this). This will
# need to be changed in the future to be able to detect the hardware itself. Even though query support
# is only available for NVidia devices, this application will work fine on all other device hardware
USE_EXT_GPU = 1
######################################################################################################

######################################################################################################
# The following path defines are the locations of the required headers and libraries for this
# application. These paths will be used if AUTOFIND is set to 0.
######################################################################################################
# Set the root include path for the OpenVDB header files
#   normally {__path__}/openvdb
OPENVDB_INC_PATH = /usr/local/OpenVDB/include
# Set the library path where the openvdb library can be found
OPENVDB_LIB_PATH = /usr/local/OpenVDB/lib
######################################################################################################
# Set the root include path for the tbb header files
# Make sure you set the path one above the root folder as this application will look for headers
# (through the OpenVDB library) in the following fashion
#   #include <tbb/...>
# So if your tbb headers are found in /usr/local/include/tbb, set the below value as /usr/local/include
TBB_INC_PATH = /usr/local/include
# Set the library path for the tbb library (required by the OpenVDB library)
TBB_LIB_PATH = /usr/local/lib
######################################################################################################
# Set the include path for the Half header files
HALF_INC_PATH = /usr/local/include
#Set the path to where the libHalf library is located
HALF_LIB_PATH = /usr/local/lib
######################################################################################################
# Set the root include path for the boost header files
# Make sure you set the path one above the root folder as this application will look for headers
# in the following fashion
#   #include <boost/...>
# So if your boost headers are found in /usr/local/include/boost, set the below value as /usr/local/include
BOOST_INC_DIR = /usr/local/include
######################################################################################################

TARGET=OpenVDBViewer

MOC_DIR=moc
OBJECTS_DIR = obj

CONFIG+=x86_64
UI_HEADERS_DIR=ui
UI_DIR=ui
QT+= gui opengl core

INC_DIR = include
SRC_DIR = src
SHADER_DIR = shaders
FORM_DIR = ui
RESOURCE_DIR = res

# this define must be changed to AMD if not using NVidia cards or drivers
# I am setting a define on Linux and Windows here as for me it is only Linux machines I use that
# have NVIDIA drivers, my Mac platform does not
isEqual(USE_EXT_GPU,1){
    DEFINES += EXT_GPU
    DEFINES += NVIDIA
}

isEqual(DEBUG,0){
    DEFINES += RELEASE
}
isEqual(DEBUG,1){
    DEFINES += DEBUG
}

# NOW REQUIRES BOOST TO BE INSTALLED AND INCLUDED IN THE INCLUDE PATH

INCLUDEPATH +=. include include/coregl ui

# set include paths dependant on the values set at the top of this .pro
isEqual(AUTOFIND, 0){
    INCLUDEPATH += $$OPENVDB_INC_PATH $$TBB_INC_PATH $$HALF_INC_PATH $$BOOST_INC_DIR
}
isEqual(AUTOFIND,1){
    unix:!macx{
        # include for boost - i think this is right but may have to change
        INCLUDEPATH += /usr/include
        INCLUDEPATH += /usr/local/include
        INCLUDEPATH += /usr/local/include/openvdb
    }
    macx:{
        INCLUDEPATH += /usr/local/include
    }
}

macx:{
    DEFINES += DARWIN
    LIBS += -framework OpenGL
}
unix:!macx{
    DEFINES += LINUX
    LIBS += -lGL -lGLEW
}

QMAKE_CXXFLAGS_WARN_ON +=  "-Wno-unused-parameter"
QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64

USEGL=42

isEqual(USEGL, 44){
        DEFINES+=GL44
}
isEqual(USEGL, 43){
        DEFINES+=GL43
}
isEqual(USEGL, 42){
        DEFINES+=GL42
}
isEqual(USEGL, 32){
        DEFINES+=GL32
}

isEqual(AUTOFIND,0){
    # set library paths dependant on values set above
    LIBS += -L$$OPENVDB_LIB_PATH -lopenvdb
    LIBS += -L$$HALF_LIB_PATH -lHalf
    LIBS += -L$$TBB_LIB_PATH -ltbb
}
isEqual(AUTOFIND,1){
    unix:!macx{
        LIBS += -L/usr/local/lib -lopenvdb -ltbb -lHalf
    }
    macx:{
        LIBS += -L/usr/local/lib -lopenvdb
        LIBS += -L/usr/local/lib -lHalf -ltbb
    }
}

macx:{
    # add frameworks required
    LIBS += -framework Cocoa
}

HEADERS+= $$INC_DIR/Shader.h \
          $$INC_DIR/ShaderFamily.h \
          $$INC_DIR/ShaderLibrary.h \
          $$INC_DIR/GLWindow.h \
          $$INC_DIR/Camera.h \
          $$INC_DIR/Plane.h \
          $$INC_DIR/Utilities.h \
          $$INC_DIR/VAO.h \
          $$INC_DIR/BoundBox.h \
          $$INC_DIR/Types.h \
          $$INC_DIR/Grid.h \
          $$INC_DIR/VDB.h \
          $$INC_DIR/NVidiaDefines.h \
          $$INC_DIR/AMDDefines.h \
          $$INC_DIR/MainWindow.h \
          $$INC_DIR/InformationWindow.h \
          $$INC_DIR/AboutWindow.h \
          $$FORM_DIR/MWindow.h \
          $$FORM_DIR/Information.h \
          $$FORM_DIR/ui_MWindow.h \
          $$FORM_DIR/ui_Information.h

SOURCES += $$SRC_DIR/main.cpp \
           $$SRC_DIR/Shader.cpp \
           $$SRC_DIR/ShaderFamily.cpp \
           $$SRC_DIR/ShaderLibrary.cpp \
           $$SRC_DIR/GLWindow.cpp \
           $$SRC_DIR/Camera.cpp \
           $$SRC_DIR/Plane.cpp \
           $$SRC_DIR/Utilities.cpp \
           $$SRC_DIR/VAO.cpp \
           $$SRC_DIR/BoundBox.cpp \
           $$SRC_DIR/Grid.cpp \
           $$SRC_DIR/VDB.cpp \
           $$SRC_DIR/NVidiaDefines.cpp \
           $$SRC_DIR/AMDDefines.cpp \
           $$SRC_DIR/MainWindow.cpp \
           $$SRC_DIR/InformationWindow.cpp \
           $$SRC_DIR/AboutWindow.cpp \
           $$FORM_DIR/MWindow.cpp \
           $$FORM_DIR/Information.cpp

OTHER_FILES += $$SHADER_DIR/normalVertex.glsl \
               $$SHADER_DIR/normalFragment.glsl \
               $$SHADER_DIR/normalCropVertex.glsl \
               $$SHADER_DIR/colourVertex.glsl \
               $$SHADER_DIR/colourFragment.glsl \
               $$SHADER_DIR/colourMapVertex.glsl \
               $$SHADER_DIR/colourMapCropVertex.glsl \
               $$SHADER_DIR/colourMapFragment.glsl \
               $$SHADER_DIR/VectorColourVertex.glsl \
               $$SHADER_DIR/VectorColourGeometry.glsl \
               $$SHADER_DIR/VectorColourCropVertex.glsl \
               $$SHADER_DIR/VectorColourCropGeometry.glsl \
               $$SHADER_DIR/VectorColourFragment.glsl \
               $$SHADER_DIR/vdbTreeVertex.glsl \
               $$SHADER_DIR/vdbTreeFragment.glsl
FORMS += $$FORM_DIR/MWindow.ui \
    	 $$FORM_DIR/Information.ui \
         $$FORM_DIR/AboutWindow.ui

macx:{
    ICON_DIR = res/ovdbv.iconset

    ICONS +=    $$ICON_DIR/icon_16x16.png \
                $$ICON_DIR/icon_16x16@2x.png \
                $$ICON_DIR/icon_32x32.png \
                $$ICON_DIR/icon_32x32@2x.png \
                $$ICON_DIR/icon_128x128.png \
                $$ICON_DIR/icon_128x128@2x.png \
                $$ICON_DIR/icon_256x256.png \
                $$ICON_DIR/icon_256x256@2x.png \
                $$ICON_DIR/icon_512x512.png \
                $$ICON_DIR/icon_512x512@2x.png

    APP_ICONS.path = Contents/$$ICON_DIR
    APP_ICONS.files += $$ICONS

    APP_SHADERS.path = Contents/Resources/shaders
    APP_SHADERS.files += $$OTHER_FILES

    QMAKE_BUNDLE_DATA +=    APP_SHADERS \
                            APP_ICONS
}

RESOURCES += $$RESOURCE_DIR/ovdbv_resources.qrc

macx:ICON = res/ovdbv.icns

