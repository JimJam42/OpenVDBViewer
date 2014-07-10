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


#ifndef __GLSCENE_H__
#define __GLSCENE_H__

#include "ShaderLibrary.h"

#include <QtCore/qmath.h>

#ifdef DARWIN
    #include <OpenGL/gl3.h>
    #include <OpenGL/glext.h>
#else
    #include <GL/glew.h>
    #include <GL/gl.h>
#endif

#include "Camera.h"
#include "VDB.h"
#include "Grid.h"
#include "Types.h"

#include <QEvent>
#include <QResizeEvent>
#include <QGLWidget>

/// @file GLWindow.h
/// @brief GL Window using OpenVDB math library to render OpenVDB files through OpenGL
/// @author Callum James
/// @version 1.0
/// @date 12/02/2014
/// Revision History:
/// Initial Version 05/01/2014
/// @class GLWindow
/// @brief GL Window using OpenVDB math library to render OpenVDB files through OpenGL. All drawing elements and
/// context are handled within this class. Also contains a lot of pass through functions to allow access to the VDB
/// class and also control some UI functionality
class GLWindow : public QGLWidget
{
    Q_OBJECT
public:
    /// @brief Constructor of GLWindow class
    GLWindow();
    /// @brief Constructor of GLWindow class
    /// @param [in] _format const QGLFormat - GLFormat to set in this widget
    /// @param [in] _parent QWidget* - parent widget
    GLWindow(const QGLFormat _format,QWidget *_parent);
    /// @brief Destructor of GLWindow class
    ~GLWindow();

    /// @brief Load in file
    /// @param [in] _file std::string - path to file to open
    void loadFile(std::string _file);
    /// @brief Clear the GLWindow
    void clear();
    /// @brief Reset the camera relative to the origin
    /// @param [in] _updateGL bool - whether to update or not
    void resetCameraNoVolume(bool _updateGL=true);
    /// @brief Initialize the class
    /// @param [in] _parent QWidget* - the parent widget
    void init(QWidget *_parent);
    /// @brief Initialise GL
    void initializeGL();
    /// @brief The render function, called everytime updateGL is called
    void paintGL();
    /// @brief Return whether a vdb file has been loaded or not - returns true or false
    inline bool vdbLoaded() {return m_vdbLoaded;}
    /// @brief Return whether a vdb extra information has been loaded or not - returns true or false
    inline bool extLoaded() {return m_extLoaded;}
    /// @brief Returns whether a scan is current running or not - returns true or false
    inline bool scanRunning() {return m_scanRunning;}
    /// @brief Set the crop box at the given index
    /// @param [in] _minx float - minimum x value
    /// @param [in] _maxx float - maximum x value
    /// @param [in] _miny float - minimum y value
    /// @param [in] _maxy float - maximum y value
    /// @param [in] _minz float - minimum z value
    /// @param [in] _maxz float - maximum z value
    /// @param [in] _index int - index to set crop
    void setCrop(float _minx, float _maxx,
                 float _miny, float _maxy,
                 float _minz, float _maxz, int _index);
    /// @brief Return Crop box at the given index - returns Bounding Box
    /// @param [in] _index int - index tp return crop box
    inline BoundBox getCBox(int _index) {return m_vdb->getCBox(_index);}
    /// @brief Set the number of crop boxes to draw
    /// @param [in] _n int - the number to draw
    inline void setNumCropsToDraw(int _n) {m_vdb->setNumCropToDraw(_n);}
    /// @brief Set whether to draw the mesh or not
    /// @param [in] _show bool - whether to show or not
    inline void showMesh(bool _show) {m_drawMesh = _show;}
    /// @brief Set whether to draw the vectors or not
    /// @param [in] _show bool - whether to show or not
    inline void showVectors(bool _show) {m_drawVectors = _show;}
    /// @brief Set Vector colour
    /// @param [in] _r float - red value
    /// @param [in] _g float - green value
    /// @param [in] _b float - blue value
    void setVectorColour(float _r, float _g, float _b);
    /// @brief Set to invert vector colour
    /// @param [in] _i int - invert value
    inline void setVectorInvert(int _i) {m_invertVectorColour = _i;}
    /// @brief Set which mode the vectors colour is taken from
    /// @param [in] _i int - the mode
    inline void setVectorColourMode(int _i) {m_vectorColourMode = _i;}
    /// @brief Set the point render channel
    /// @param [in] _channel int - the channel to set to
    void setPointChannel(int _channel);
    /// @brief Set the vector render channel
    /// @param [in] _channel int - the channel to set to
    void setVectorChannel(int _channel);
    /// @brief Set if move around origin or not
    /// @param [in] _move bool - whether to move around origin or not
    void moveAroundOrigin(bool _move);
    /// @brief Set colour map on cull true or false
    /// @param [in] _on bool - set to cull or not
    inline void setColourMapOnCull(bool _on) {m_colourMapOnCull = _on;}
    /// @brief Set ramp points mode
    /// @param [in] _i int - mode passed in
    inline void setRampPoints(int  _i) {m_rampPoints = _i;}
    /// @brief Set whether to use user defined ramp or not
    /// @param [in] _user bool - set user define ramp or not
    inline void setUserDefinedRamp(bool _user) {m_userDefinedRamp = _user;}
    /// @brief Set ramp minimum
    /// @param [in] _min float - minimum value
    void setRampMin(float _min);
    /// @brief Set ramp maximum
    /// @param [in] _max float - maximum value
    void setRampMax(float _max);
    /// @brief Return if GL has been initialised or not - returns true or false
    inline bool glInitialised () {return m_glInitialized;}
    /// @brief Remove the mesh
    void removeMesh();
    /// @brief Manually move the crop box on a scan
    /// @param [in] _delta float - the amount to move by
    void manualCropMove(float _delta);
    /// @brief Return the bounding box of the VDB file - returns BoundBox
    inline BoundBox getBBox() {return m_vdb->getBBox();}
    /// @brief Set colour map boolean
    /// @param [in] _applied bool - whether it is applied or not
    inline void setColourMapApplied(bool _applied) {m_colourMapApplied = _applied; updateGL();}
    /// @brief Set colour map to specific cull
    /// @param [in] _num int - the cull to apply to
    inline void setColourMapToCullNumber(int _num) {m_applyColourMapToCullNumber = _num;}
    /// @brief Set the ramp colour
    /// @param [in] _colour openvdb::Vec3f - the ramp colour to set
    inline void setRampColour(openvdb::Vec3f _colour) {m_colourMapColour = _colour;}

    // functions to set and get culling attributes
    /// @brief Set culling enabled or not
    /// @param [in] _e int - either a 0 or 1 is enabled or not
    inline void setCullingEnabled(int _e) {m_cullingEnabled = _e;}
    /// @brief Set the number of culls being used
    /// @param [in] _n int - the number of culls to use
    inline void setNumActiveCullings(int _n) {m_numActiveCulls = _n;}
    /// @brief Return if culling is enabled or not - returns int
    inline int getCullingEnabled() {return m_cullingEnabled;}
    /// @brief Get the number of active cullings - returns int
    inline int getNumActiveCullings() {return m_numActiveCulls;}
    /// @brief Get the channel extremes for a specific channel - returns BBoxBare
    /// @param [in] _index int - channel to get extremes from
    BBoxBare getChannelExtremes(int _index);
    /// @brief Set a cull
    /// @param [in] _cull Cull - the cull data
    /// @param [in] _index int - the cull to set
    void setCull(Cull _cull, int _index);
    /// @brief Set a cull active
    /// @param [in] _active bool - active or not
    /// @param [in] _index int - the cull to set
    void setCullActive(bool _active, int _index);
    /// @brief Set a cull channel offset
    /// @param [in] _channel int - the channel offset
    /// @param [in] _index int - the cull to set
    void setCullChannelOffset(int _channel, int _index);
    /// @brief Set a cull channel type
    /// @param [in] _type int - the channel type
    /// @param [in] _index int - the cull to set
    void setCullChannelType(int _type, int _index);
    /// @brief Set a cull type
    /// @param [in] _type int - the cull type
    /// @param [in] _index int - the cull to set
    void setCullType(int _type, int _index);
    /// @brief Set a cull upper float limit
    /// @param [in] _u float - the upper float limit
    /// @param [in] _index int - the cull to set
    void setCullULimit(float _u, int _index);
    /// @brief Set a cull lower float limit
    /// @param [in] _l float - the cull lower float limit
    /// @param [in] _index int - the cull to set
    void setCullLLimit(float _l, int _index);
    /// @brief Set a cull lower vec limit
    /// @param [in] _l openvdb::Vec3s - the cull lower vec limit
    /// @param [in] _index int - the cull to set
    void setCullLLimit(openvdb::Vec3s _l, int _index);
    /// @brief Set a cull upper vec limit
    /// @param [in] _u openvdb::Vec3s - the cull upper vec limit
    /// @param [in] _index int - the cull to set
    void setCullULimit(openvdb::Vec3s _u, int _index);
    /// @brief Get cull at index - returns cull
    /// @param [in] _index int - index to return from
    Cull getCull(int _index);
    /// @brief Get cull active at index - returns true or false
    /// @param [in] _index int - index to return from
    bool getCullActive(int _index);
    /// @brief Get cull channel offset at index - returns int
    /// @param [in] _index int - index to return from
    int getCullChannelOffset(int _index);
    /// @brief Get cull channel type at index - returns int
    /// @param [in] _index int - index to return from
    int getCullChannelType(int _index);
    /// @brief Get cull type at index - returns int
    /// @param [in] _index int - index to return from
    int getCullType(int _index);
    /// @brief Get cull lower float limit at index - returns float
    /// @param [in] _index int - index to return from
    float getCullFloatLLimit(int _index);
    /// @brief Get cull upper float limit at index - returns float
    /// @param [in] _index int - index to return from
    float getCullFloatULimit(int _index);
    /// @brief Get cull lower vec limit at index - returns openvdb::Vec3s
    /// @param [in] _index int - index to return from
    openvdb::Vec3s getCullVecLLimit(int _index);
    /// @brief Get cull upper vec limit at index - returns openvdb::Vec3s
    /// @param [in] _index int - index to return from
    openvdb::Vec3s getCullVecULimit(int _index);

    //---------------------------------------------------------------

    // fuctions to retrive file information for information tab on UI
    /// @brief Return GPU vendor - returns std::string
    std::string GPUVendor();
    /// @brief Return OpenGL Version - returns std::string
    std::string OpenGLVersion();
    /// @brief Return GPU renderer - returns std::string
    std::string GPURenderer();
    /// @brief Return total GPU memory in MB - returns std::string
    /// @param [in] _ext bool - if true, the extension of MB will be added
    std::string totalGPUMemMB(bool _ext=true);
    /// @brief Return current GPU memory in MB - returns std::string
    /// @param [in] _ext bool - if true, the extension of MB will be added
    std::string currentUsedGPUMemMB(bool _ext=true);
    /// @brief Return file name - returns std::string
    std::string fileName();
    /// @brief Return file path - returns std::string
    std::string filePath();
    /// @brief Return file version - returns std::string
    std::string fileVersion();
    /// @brief Return number of channels - returns int
    int numChannels();
    /// @brief Return number of grids - returns int
    int numGrids();
    /// @brief Return grid name at - returns std::string
    /// @param [in] _channel int - channel to return name from
    std::string gridName(int _channel);
    /// @brief Return grid type at - returns std::string
    /// @param [in] _channel int - channel to return type from
    std::string gridType(int _channel);
    /// @brief Return grid dimension at - returns std::string
    /// @param [in] _grid int - grid to return dimension from from
    std::string gridDimAt(int _grid);
    /// @brief Return voxel count - returns std::string
    std::string voxelCount();
    /// @brief Return tree depth - returns int
    int treeDepth();
    /// @brief Return voxel count at tree depth specified - returns std::string
    /// @param [in] _depth int - level to return count from
    std::string voxelCountAtTreeDepth(int _depth);
    /// @brief Return num of metadata - returns int
    int numMeta();
    /// @brief Return metadata name at - returns std::string
    /// @param [in] _index int - channel to return from
    std::string metaNameAt(int _index);
    /// @brief Return the index of the meta name specified - returns int
    /// @param [in] _name std::string - name to find
    /// @param [in] _skip int - number of duplicates to skip until returning
    int metaNameIndex(std::string _name, int _skip);
    /// @brief Return the meta value at - returns std::string
    /// @param [in] _name std::string - name to find
    /// @param [in] _skip int - number of duplicates to skip until returning
    std::string metaValueAt(std::string _name, int _skip);
    /// @brief Return the meta value at - returns std::string
    /// @param [in] _index int - index to return from
    std::string metaValueAt(int _index);

public slots:
    /// @brief Toggle showing the mesh - returns int
    int toggleMesh();
    /// @brief Toggle showing the bounding box
    void toggleBBox();
    /// @brief Toggle showing the vectors
    void toggleVectors();
    /// @brief Toggle showing the VDB tree
    void toggleVDB();
    /// @brief Toggle showing the grid
    void toggleGrid();
    /// @brief Toggle showing crop boxes
    void toggleCrop();
    /// @brief Set the length of vectors
    /// @param [in] _delta float - the length of vectors
    void setVectorLength(float _delta);
    /// @brief Set level of detail
    /// @param [in] _lod float - level of detail percentage to show
    void setLOD(float _lod);
    /// @brief Set the scan speed
    /// @param [in] _speed float - the speed for the scan
    void setScanSpeed(float _speed);
    /// @brief Set the width of the X axis scan
    /// @param [in] _w float - width
    /// @param [in] _index int - crop box to apply to
    void setScanXWidth(float _w, int _index);
    /// @brief Set the height of the Y axis scan
    /// @param [in] _h float - height
    /// @param [in] _index int - crop box to apply to
    void setScanYHeight(float _h, int _index);
    /// @brief Set the depth of the Z axis scan
    /// @param [in] _d float - depth
    /// @param [in] _index int - crop box to apply to
    void setScanZDepth(float _d, int _index);

    // functions to toggle tree levels to be drawn
    /// @brief Toggle drawing VDB tree level passed in
    /// @param [in] _level int - level to toggle
    void toggleLevel(int _level);
    /// @brief Toggle VDB tree level 0
    void toggleLevel0();
    /// @brief Toggle VDB tree level 1
    void toggleLevel1();
    /// @brief Toggle VDB tree level 2
    void toggleLevel2();
    /// @brief Toggle VDB tree level 3
    void toggleLevel3();
    /// @brief Run scan on specified axis
    /// @param [in] _axis int - axis for scan
    void runScan(int _axis);
    /// @brief Pause scan
    void pauseScan();
    /// @brief Resume scan
    void resumeScan();
    /// @brief Stop scan
    void stopScan();
    /// @brief Reset the camera relative to the VDB file loaded
    void resetCameraVolume();
    /// @brief Reset the VDB files transform
    void resetVolumeTransform();
    /// @brief Reset both Camera and VDB Volumne transform
    void resetCameraAndVolumeTransform();

signals:
    /// @brief Signal to update the current used GPU mem on the UI
    void updateUsedGPUMem();

private:
    /// @brief Width of the x scan
    float m_xScanWidth;
    /// @brief Height of the y scan
    float m_yScanHeight;
    /// @brief Depth of the z scan
    float m_zScanDepth;
    /// @brief Boolean of whether to move the camera around the origin or move the object
    bool m_moveAroundOrigin;
    /// @brief Total difference in x
    float m_totalDiffX;
    /// @brief Total difference in y
    float m_totalDiffY;
    /// @brief Integer representing if the vector colours should be inverted or not
    int m_invertVectorColour;
    /// @brief Integer representing the current vector colour mode
    int m_vectorColourMode;
    /// @brief Boolean on if the VDB file is loaded or not
    bool m_vdbLoaded;
    /// @brief Vector holding the mouses translation
    openvdb::Vec3f m_mouseTranslate;
    /// @brief Value moved in X by mouse
    float m_spinX;
    /// @brief Value moved in Y by mouse
    float m_spinY;
    /// @brief Boolean if rotation is enabled
    bool m_rotate;
    /// @brief Boolean if translation is enabled
    bool m_translate;
    /// @brief Original and previous X value
    float m_origX;
    /// @brief Original and previous Y value
    float m_origY;
    /// @brief Difference in X value
    float m_diffX;
    /// @brief Difference in Y value
    float m_diffY;
    /// @brief The original X position
    int m_origXPos;
    /// @brief The original Y position
    int m_origYPos;
    /// @brief Camera in the scene
    Camera *m_camera;
    /// @brief ShaderLibrary used to handle this GL context
    ShaderLibrary *m_shaderLib;
    /// @brief Resize window event
    /// @param [in] _w int - new width
    /// @param [in] _h int - new height
    void resizeGL(int _w, int _h);
    /// @brief Event handler for a mouse movement
    /// @param [in] _event QKeyEvent* - the triggered event
    void mouseMoveEvent (QMouseEvent * _event );
    /// @brief Event handler for a mouse click
    /// @param [in] _event QKeyEvent* - the triggered event
    void mousePressEvent ( QMouseEvent *_event);
    /// @brief Event handler for a mouse button release
    /// @param [in] _event QKeyEvent* - the triggered event
    void mouseReleaseEvent ( QMouseEvent *_event );
    /// @brief Event handler for a mouse wheel movement
    /// @param [in] _event QKeyEvent* - the triggered event
    void wheelEvent( QWheelEvent *_event);
    /// @brief Integer holding if a ramp is on points or channel
    int m_rampPoints;
    /// @brief Member to store the extremes on one channel - set when needed
    BBoxBare m_channelRampExtremes;
    /// @brief Boolean to specify if the colour ramp is on an automatic range or user defined
    bool m_userDefinedRamp;
    /// @brief VDB class to handle all things VDB
    VDB *m_vdb;
    /// @brief Grid used as reference in the scene
    Grid *m_grid;
    /// @brief Boolean of whether to draw the mesh or not
    bool m_drawMesh;
    /// @brief Boolean of whether to draw the bounding box or not
    bool m_drawBBox;
    /// @brief Boolean of whether to draw the grid or not
    bool m_drawGrid;
    /// @brief Boolean of whether to draw the VDB tree or not
    bool m_drawVDBTree;
    /// @brief Boolean of whether to draw the vectors or not
    bool m_drawVectors;
    /// @brief Boolean of whether to use crop boxes or not
    bool m_useCrop;
    /// @brief value used to determine which points should be hidden for LOD - uploaded to shader
    float m_s;

    // the timer used for any 'scans' of the model
    /// @brief Timer used for any scans of the model
    int m_timer;
    /// @brief Boolean true if x scan active
    bool m_xScan;
    /// @brief Boolean true if y scan active
    bool m_yScan;
    /// @brief Boolean true if z scan active
    bool m_zScan;
    /// @brief Boolean true if scan running
    bool m_scanRunning;

    // here numbers will relate to the axis
    // 0 - x
    // 1 - y
    // 2 - z
    /// @brief Speed of any scans
    float m_scanSpeed;
    /// @brief Scan direction - positive or negative
    int m_scanDirection;
    /// @brief Event triggered within the execution of a timer
    /// @param [in] QTimerEvent*
    void timerEvent(QTimerEvent *);
    /// @brief Caclulate scan direction at the current time
    void scanDirection();

    // information on the attached graphics card
    /// @brief The GPU Vendor
    const GLubyte *m_GPUVendor;
    /// @brief The GPU Renderer
    const GLubyte *m_GPURenderer;
    /// @brief The GPU OpenGL Version
    const GLubyte *m_GPUVersion;
    /// @brief The total GPU memory in KB
    GLint m_total_GPU_mem_kb;
    /// @brief Boolean of whether GL has been initialised or not
    bool m_glInitialized;

    /// @brief The delta used for mouse wheel movement dependant on the distance from the model
    float m_wheelZoomDelta;
    /// @brief Set the zoom delta
    void setWheelZoomSpeed();
    /// @brief Boolean of whether the high res mesh has been loaded or not
    bool m_extLoaded;
    /// @brief Boolean of whether a colour map has been applied or not
    bool m_colourMapApplied;
    /// @brief Boolean of whether a colour map is applied to a cull
    bool m_colourMapOnCull;
    /// @brief Cull number map applied to
    int m_applyColourMapToCullNumber;
    /// @brief Colour of colour map
    openvdb::Vec3f m_colourMapColour;
    /// @brief Set to true when a reset of the Volume transform is being conducted
    bool m_resetTransform;

    /// @brief Vector storing where the original eye is for each file
    openvdb::Vec4f m_currentBaseCameraFrom;
    /// @brief Vector storing where the original look is for each file
    openvdb::Vec4f m_currentBaseCameraTo;

    // following are used for culling of data
    /// @brief Boolean of whether culling is enabled and used or not
    int m_cullingEnabled;
    /// @brief The number of active culls being used
    int m_numActiveCulls;
    // maximum of three active culls at one time so fix this as a size
    /// @brief All culls possible - fixed number of 3 due to performance limitations
    Cull m_culls[3];

    /// @brief Set Colour map to a cull
    void setMapOnCullValues();
    /// @brief Upload a single crop box data to the GPU
    /// @param [in] _uniformName std::string - uniform name to upload to
    /// @param [in] _index int - Crop box number number to upload
    void uploadSingleCropBoxToShader(std::string _uniformName, int _index);
    /// @brief Upload all crop boxes to the shader
    void uploadCropBoxesToShader();

    /// @brief Upload a single cull data to the GPU
    /// @param [in] _uniformName std::string - uniform name to upload to
    /// @param [in] _index int - Cull number number to upload
    void uploadSingleCullToShader(std::string _uniformName, int _index);
    /// @brief Upload all culls to the shader
    void uploadCullsToShader();
};

#endif /* __GLSCENE_H__ */
