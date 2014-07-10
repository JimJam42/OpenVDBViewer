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


#include "GLWindow.h"
#include "Utilities.h"

#include <boost/lexical_cast.hpp>

#include <QMessageBox>

#ifdef NVIDIA
    #include "NVidiaDefines.h"
    using namespace NVidiaDef;
#endif
#ifdef AMD
    #include "AMDDefines.h"
    using namespace AMDDef;
#endif

#include <iostream>

const static float INCREMENT=0.01;
#define TIMER_EXEC_INTERVAL 34

GLWindow::GLWindow()
{
    // empty constructor to do nothing
    // initialisation of this class is done later in a different method
}

GLWindow::GLWindow(const QGLFormat _format, QWidget *_parent) : QGLWidget( _format, _parent )
{
    // Another empty constructor that sets the parent widget and the format for QtOpenGL
}

GLWindow::~GLWindow()
{
    if (m_vdbLoaded)
    {
        // only attempt to delete the VDB pointer if it has been loaded and initialised
        delete m_vdb;
    }
}

void GLWindow::loadFile(std::string _file)
{
    m_mouseTranslate = openvdb::Vec3f(0.0,0.0,0.0);
    m_spinX = 0.0f;
    m_spinY = 0.0f;
    if (m_vdbLoaded)
    {
        // remove an old file if it has been loaded
        delete m_vdb;
        m_vdbLoaded = false;
    }

    // give the user the option to load High resolution when first loading or wait unitl later
    QMessageBox::StandardButton replyLoad, replyCont;
    QMessageBox *loadHigh = new QMessageBox(this);
    loadHigh->setWindowFlags(Qt::WindowStaysOnTopHint);
    loadHigh->setWindowModality(Qt::WindowModal);
    replyLoad = loadHigh->question(this,"Loading VDB File", "Load High Resolution",QMessageBox::Yes|QMessageBox::No);
    // bring to the top
    loadHigh->raise();

    if (replyLoad == QMessageBox::Yes)
    {
        // if the user has chosen to load the high resolution data, show them the performance warning
        // and check to make sure they are happy with their decision
        QMessageBox confirm(this);
        confirm.setWindowModality(Qt::WindowModal);
        confirm.raise();
        replyCont = confirm.question(NULL,"Loading High Resolution", "Loading High Resolution volumes can be memory intensive and time consuming. Are you sure you wish to continue?",QMessageBox::Yes|QMessageBox::No);
        confirm.close();
    }

    m_vdb = new VDB(_file);
    m_vdb->setTotalGPUMemKB(m_total_GPU_mem_kb);

    // load the basic iformation from the file
    if(m_vdb->loadBasic())
    {
        #ifdef DEBUG
            std::cout<<"Load complete!!"<<std::endl;
        #endif
    }
    else
    {
        std::cerr<<"Error whilst loading file"<<std::endl;
    }

    m_extLoaded = false;
    if (replyLoad == QMessageBox::Yes && replyCont == QMessageBox::Yes)
    {
        // if it has been chosen to load high resolution, load
        if(m_vdb->loadExt())
        {
            #ifdef DEBUG
                std::cout<<"High resolution load complete!!"<<std::endl;
            #endif
            m_extLoaded = true;
        }
        else
        {
            std::cerr<<"Error whilst loading high resolution volume"<<std::endl;
        }
    }
    // get the s value of the current channel
    m_s = m_vdb->getS();

    m_vdbLoaded = true;
#ifdef EXT_GPU
    #ifdef NVIDIA
        #ifdef DEBUG
            std::cout<<"Current available GPU memory: "<<currentAvailableGPUMemKB()/1024<<"MB"<<std::endl;
        #endif
        emit(updateUsedGPUMem());
    #else
        #ifdef DEBUG
            printAMDNotSupported();
        #endif
    #endif
#else
    #ifdef DEBUG
        printNoExtGPU();
    #endif
#endif

// Initialise all crop boxes
//=====================================================
    m_vdb->setCrop(-1.0f,1.0f,0.0f,20.0f,-5.0f,5.0f,0);
    m_vdb->setCrop(-1.0f,1.0f,0.0f,20.0f,-5.0f,5.0f,1);
    m_vdb->setCrop(-1.0f,1.0f,0.0f,20.0f,-5.0f,5.0f,2);
    m_vdb->setCrop(-1.0f,1.0f,0.0f,20.0f,-5.0f,5.0f,3);
    m_vdb->setCrop(-1.0f,1.0f,0.0f,20.0f,-5.0f,5.0f,4);

    m_vdb->setCropColour(openvdb::Vec3f(0.0f,1.0f,1.0f),0);
    m_vdb->setCropColour(openvdb::Vec3f(0.5f,1.0f,0.5f),1);
    m_vdb->setCropColour(openvdb::Vec3f(1.0f,0.5f,1.0f),2);
    m_vdb->setCropColour(openvdb::Vec3f(1.0f,0.5f,0.5f),3);
    m_vdb->setCropColour(openvdb::Vec3f(0.5f,1.0f,0.0f),4);
//=====================================================

    // I want to frame the entire file in shot when loaded
    // to do this first need to calculate if the width or height of the BBox is greatest

    float edgeLength = m_vdb->getBBox().width();
    if (m_vdb->getBBox().width() < m_vdb->getBBox().height())
    {
        edgeLength = m_vdb->getBBox().height();
    }

    // find the half of the length
    float halfEdgeLength = edgeLength/2.0f;

    // now find the half way point dependent on width or height
    openvdb::Vec3f halfWay = m_vdb->getBBox().centre();

    // to find length of z distance, with a viewing angle of 45 degrees it is a tan theta
    float zLength = halfEdgeLength*tan(Utilities::u_radians(67.5));

    openvdb::Vec3f from;
    from[0] = halfWay[0];
    // move up by a quarter of the bounding box height to give some kind of angle on the view
    from[1] = halfWay[1] + m_vdb->getBBox().height()*0.25f;
    // multiple zLength by 1.5 to ensure the model will fit onto screen
    from[2] = zLength*1.5f;

    openvdb::Vec3f to(halfWay);

    openvdb::Vec4f currentEye = m_camera->getEye();
    openvdb::Vec4f currentLook = m_camera->getLook();

    // find the difference between the new position an the current positions
    m_currentBaseCameraFrom = openvdb::Vec4f(from.x(),from.y(),from.z(),1.0) - currentEye;
    m_currentBaseCameraTo = openvdb::Vec4f(to.x(),to.y(),to.z(),1.0) - currentLook;

    m_camera->moveEye(m_currentBaseCameraFrom.x(),m_currentBaseCameraFrom.y(),m_currentBaseCameraFrom.z());
    m_camera->moveLook(m_currentBaseCameraTo.x(),m_currentBaseCameraTo.y(),m_currentBaseCameraTo.z());

    // set base vectors so that the camera can be returned to these values on a reset once a
    // transformation has taken place
    m_currentBaseCameraFrom = m_camera->getEye();
    m_currentBaseCameraTo = m_camera->getLook();

    updateGL();
}

void GLWindow::clear()
{
    // if a vdb file has been loaded, clear all the data
    if (m_vdbLoaded)
    {
        m_vdbLoaded = false;
        m_extLoaded = false;
        m_drawMesh = false;
        m_drawVectors = false;
        delete m_vdb;
    }
}

void GLWindow::resetCameraNoVolume(bool _updateGL)
{
    // this function will reset the camera without consideration for the vdb volume loaded if loaded
    openvdb::Vec4f eyeDiff = m_camera->getEye()-openvdb::Vec4f(0.0f,5.0f,20.0f,1.0f);
    openvdb::Vec4f lookDiff = m_camera->getLook();

    m_currentBaseCameraFrom = openvdb::Vec4f(0.0,5.0,20.0,1.0);
    m_currentBaseCameraTo = openvdb::Vec4f(0.0f,0.0f,0.0f,1.0f);

    m_camera->moveLook(-lookDiff.x(),-lookDiff.y(),-lookDiff.z());
    m_camera->moveEye(-eyeDiff.x(),-eyeDiff.y(),-eyeDiff.z());

    m_wheelZoomDelta = 0.4f;

    if (_updateGL)
    {
        updateGL();
    }
}

void GLWindow::resetCameraVolume()
{
    // this function will reset the camera relative to a loaded vdb file so it looks correctly at the vdb file
    openvdb::Vec4f eyeDiff = m_camera->getEye()-m_currentBaseCameraFrom;
    openvdb::Vec4f lookDiff = m_camera->getLook()-m_currentBaseCameraTo;

    m_camera->moveLook(-lookDiff.x(),-lookDiff.y(),-lookDiff.z());
    m_camera->moveEye(-eyeDiff.x(),-eyeDiff.y(),-eyeDiff.z());

    m_wheelZoomDelta = 0.4f;
    updateGL();
}

void GLWindow::resetVolumeTransform()
{
    if (m_vdbLoaded)
    {
        // only performm if the vdb file has been loaded, else do nothing
        openvdb::Mat4s  reverseTransform;
        reverseTransform.setIdentity();
        // simply set the transform matrix to the identity matrix
        m_vdb->setTransform(reverseTransform);
        // reset all values to 0 to prevent any transform back
        m_spinX = m_spinY = 0.0f;
        m_mouseTranslate[0] = m_mouseTranslate[1] = m_mouseTranslate[2] = 0.0f;
        // set to true so the next render call knows a reset is being performed
        m_resetTransform = true;
        updateGL();
    }
}

void GLWindow::resetCameraAndVolumeTransform()
{
    resetCameraVolume();
    resetVolumeTransform();
}

void GLWindow::init(QWidget *_parent)
{
    m_glInitialized = false;
    m_resetTransform = false;
    m_vdbLoaded = false;
    m_drawMesh = false;
    m_rotate=false;
    m_drawBBox = false;
    m_drawVDBTree = true;
    m_drawGrid = true;
    m_drawVectors = false;
    m_useCrop = false;
    m_spinX=0.0f;
    m_spinY=0.0f;
    m_diffX = 0.0f;
    m_diffY = 0.0f;
    m_xScan = false;
    m_yScan = false;
    m_zScan = false;
    m_scanRunning = false;
    m_scanSpeed = 0.4f;
    m_scanDirection = 0;
    m_s = 0.0f;
    m_invertVectorColour = 0;
    m_vectorColourMode = 0;
    m_extLoaded = false;
    m_colourMapApplied = false;
    m_colourMapColour = openvdb::Vec3f(1.0,0.0,0.0);

    // initialize cull attributes
    m_cullingEnabled = 0;
    m_colourMapOnCull = false;
    m_numActiveCulls = 0;
    m_applyColourMapToCullNumber = 0;

    // initialise all culls
    for (int i =0; i < 3; i++)
    {
        m_culls[i]._active = false;
        m_culls[i]._cullType = 0;
        m_culls[i]._channelOffset = 0;
        m_culls[i]._channelType = 0;
        m_culls[i]._floatULimit = 1.0;
        m_culls[i]._floatLLimit = 0.0;
        m_culls[i]._vecULimit = openvdb::Vec3s(1.0,1.0,1.0);
        m_culls[i]._vecLLimit = openvdb::Vec3s(0.0,0.0,0.0);
    }

    m_rampPoints = 1;
    m_channelRampExtremes.minx = m_channelRampExtremes.miny = m_channelRampExtremes.minz = 0.0f;
    m_channelRampExtremes.maxx = m_channelRampExtremes.maxy = m_channelRampExtremes.maxz = 0.0f;
    m_userDefinedRamp = false;

    m_wheelZoomDelta = 0.4f;

    m_xScanWidth = m_yScanHeight = m_zScanDepth = 0.0f;
    m_totalDiffX = m_totalDiffY = 0.0f;

    m_currentBaseCameraTo = openvdb::Vec4f(0.0f,0.0f,0.0f,1.0f);
    m_currentBaseCameraFrom = openvdb::Vec4f(0.0f,0.0f,0.0f,1.0f);
    // connect up signal to allow update of read out of available GPU mem on the UI
    connect(this,SIGNAL(updateUsedGPUMem()),_parent,SLOT(updateUsedGPUMem()));
}

void GLWindow::resizeGL(int _w , int _h)
{
    // set the viewport for openGL
    glViewport(0,0,_w,_h);
    // now set the camera size values as the screen size has changed
    m_camera->setShape(45,(float)_w/_h,0.05,350);
}

void GLWindow::initializeGL()
{
#ifndef DARWIN
    glewExperimental = true;
    glewInit();
    glEnableClientState(GL_VERTEX_ARRAY);
#endif
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE_ARB);
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

    #ifdef DEBUG
        QGLFormat format = this->format();
        std::cout<<"Format version: ("<<format.majorVersion()<<"."<<format.minorVersion()<<")"<<std::endl;
        std::cout<<"Format samples: "<<format.samples()<<std::endl;
    #endif

    m_total_GPU_mem_kb = 0;

    // get hardware information
    m_GPUVendor = glGetString(GL_VENDOR);
    m_GPURenderer = glGetString(GL_RENDERER);
    m_GPUVersion = glGetString(GL_VERSION);

#ifdef EXT_GPU
    #ifdef NVIDIA
        // this will only work for NVidia cards
        // would need to do a find on the card vendor to then determine what to do
        m_total_GPU_mem_kb = totalGPUMemKB();
    #else
        #ifdef DEBUG
            printAMDNotSupported();
        #endif
    #endif
#else
    #ifdef DEBUG
        printNoExtGPU();
    #endif
#endif

#ifdef DEBUG
    std::cout<<"Graphics Card Vendor: "<<m_GPUVendor<<std::endl;
    std::cout<<"Current active renderering device: "<<m_GPURenderer<<std::endl;
    std::cout<<"Current OpenGL Version: "<<m_GPUVersion<<std::endl;
    std::cout<<"Total GPU memory available is: "<<m_total_GPU_mem_kb/1024<<"MB"<<std::endl;
#endif

    // get and create basic camera at default position
    openvdb::Vec3f from(0.0,5.0,20.0);
    openvdb::Vec3f to(0.0,0.0,0.0);
    openvdb::Vec3f up(0.0,1.0,0.0);

    m_currentBaseCameraFrom = openvdb::Vec4f(from.x(),from.y(),from.z(),1.0);
    m_currentBaseCameraTo = openvdb::Vec4f(to.x(),to.y(),to.z(),1.0);

    m_camera = new Camera(from,to,up);

    m_camera->setShape(45,(float)1280/720,0.0,2000.0);

    // set up a simple normal shader to setActive
    m_shaderLib = new ShaderLibrary;

    // Because I do not want to have any switches within a shader to keep it as simple as possible for the GPU
    // any shader that ha the possibility of being used in a crop box will have a sister shader that
    // does exactly the same thing but will have a crop calculation in it

    m_shaderLib->createShaderFamily("Normal");
    m_shaderLib->addShader("Normal",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/normalVertex.glsl"));
    m_shaderLib->addShader("Normal",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/normalFragment.glsl"));

    // now create the crop counterpart using the original fragment
    m_shaderLib->createShaderFamily("NormalCrop");
    m_shaderLib->addShader("NormalCrop",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/normalCropVertex.glsl"));
    m_shaderLib->addShader("NormalCrop",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/normalFragment.glsl"));

    // create shaders for VDB tree
    m_shaderLib->createShaderFamily("VDBTree");
    m_shaderLib->addShader("VDBTree",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/vdbTreeVertex.glsl"));
    m_shaderLib->addShader("VDBTree",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/vdbTreeFragment.glsl"));

    m_shaderLib->createShaderFamily("ColourMap");
    m_shaderLib->addShader("ColourMap",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/colourMapVertex.glsl"));
    m_shaderLib->addShader("ColourMap",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/colourMapFragment.glsl"));

    m_shaderLib->createShaderFamily("Colour");
    m_shaderLib->addShader("Colour",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/colourVertex.glsl"));
    m_shaderLib->addShader("Colour",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/colourFragment.glsl"));

    m_shaderLib->createShaderFamily("ColourMapCrop");
    m_shaderLib->addShader("ColourMapCrop",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/colourMapCropVertex.glsl"));
    m_shaderLib->addShader("ColourMapCrop",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/colourMapFragment.glsl"));

    m_shaderLib->createShaderFamily("Vector");
    m_shaderLib->addShader("Vector",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/VectorColourVertex.glsl"));
    m_shaderLib->addShader("Vector",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/VectorColourFragment.glsl"));
    m_shaderLib->addShader("Vector",GEOMETRY,Utilities::PLATFORM_FILE_PATH("shaders/VectorColourGeometry.glsl"));

    // now create the crop counterpart - as the fragment has no knowledge of crop will use the same shader for both
    m_shaderLib->createShaderFamily("VectorCrop");
    m_shaderLib->addShader("VectorCrop",VERTEX,Utilities::PLATFORM_FILE_PATH("shaders/VectorColourCropVertex.glsl"));
    m_shaderLib->addShader("VectorCrop",FRAGMENT,Utilities::PLATFORM_FILE_PATH("shaders/VectorColourFragment.glsl"));
    m_shaderLib->addShader("VectorCrop",GEOMETRY,Utilities::PLATFORM_FILE_PATH("shaders/VectorColourCropGeometry.glsl"));

    m_shaderLib->linkAll();

    m_shaderLib->setActive("Colour");

    m_grid = new Grid(15.0f,15.0f,15);
    m_grid->create();

    m_moveAroundOrigin = false;

    glViewport(0,0,width(),height());

    m_glInitialized = true;
    // #############

}

void GLWindow::paintGL()
{
  // update the wheel move amount depending on the distance from the centre of the bounding box
  // the closer the smaller the amount to move, the further the greater
  setWheelZoomSpeed();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  std::string shaderToUse;

  // choose which shader needs to be used for the currently chosen settings
  if (!m_useCrop)
  {
      if (m_colourMapApplied)
      {
          shaderToUse = "ColourMap";
      }
      else
      {
        shaderToUse = "Normal";
      }
  }
  else
  {
      if (m_colourMapApplied)
      {
          shaderToUse = "ColourMapCrop";
      }
      else
      {
        shaderToUse = "NormalCrop";
      }
  }

  // default all matrices used for transformations
  openvdb::Mat4s transform;
  transform.setIdentity();
  openvdb::Mat4s rotX, rotY;
  rotX.setIdentity();
  rotY.setIdentity();

  if (!m_moveAroundOrigin)
  {
      // move the object itself
      if (m_vdbLoaded && !m_resetTransform)
      {
          // if not resetting the transform, get new transform and set top VDB
          rotX.setToRotation(openvdb::Vec3f(1.0,0.0,0.0),-m_spinX/50.0f);
          rotY.setToRotation(openvdb::Vec3f(0.0,1.0,0.0), -m_spinY/50.0f);

          transform = (rotX*rotY);
          transform.setTranslation(m_mouseTranslate);

          m_vdb->setTransform(transform);
      }
      // if resetting do not set a new transform, instead use the current
  }
  else
  {
      // move the camera around the scene
      openvdb::Mat4s t, it, r;
      t.setIdentity();
      it.setIdentity();
      r.setIdentity();
      openvdb::Vec4f fp;

      openvdb::Vec4f diff = m_camera->getEye();
      t.setTranslation(diff.getVec3());
      it = t.inverse();
      rotY.setToRotation(openvdb::Vec3f(0.0f,1.0f,0.0f),m_diffY/50.0f);
      rotX.setToRotation(openvdb::Vec3f(1.0f,0.0f,0.0f),m_diffX/50.0f);
        // combine rotations
      r = rotX*rotY;

      transform = it*r*t;

      fp = transform*diff;

      openvdb::Vec4f now = fp-diff;
      // move the placement of the camera
      m_camera->moveEye(now.x(),now.y(),now.z());
  }

  if (m_vdbLoaded)
  {
      openvdb::Mat4s MVP;
      openvdb::Mat4s M;
      // get the transform matrix of the vdb file
      M=m_vdb->transform();
      MVP = M*m_camera->getViewProjectionMatrix();

      // if using crop boxes, draw the crop boxes
      if (m_useCrop)
      {
          // here I will set the limits required to the correct values
          // these will then be used outside this if statement to actually crop the drawing
          openvdb::Mat4s localM;
          localM.setIdentity();
          openvdb::Mat4s localMVP = localM*m_camera->getViewProjectionMatrix();
          m_shaderLib->setActive("Colour");
          m_shaderLib->setShaderUniformFromMat4("MVP",localMVP);
          // craw the crop boxes
          m_vdb->drawCrop(m_shaderLib);
      }

      // now move to actual shader to use and continue
      m_shaderLib->setActive(shaderToUse);

      if (m_useCrop)
      {
          m_shaderLib->setShaderUniformFromMat4("u_transform",m_vdb->transform());          
          uploadCropBoxesToShader();
      }

      m_shaderLib->setShaderUniformFromMat4("MVP",MVP);
      m_shaderLib->setShaderParam1f("u_s",m_s);

      if (m_cullingEnabled == 1)
      {
          // set culling to true
          m_shaderLib->setShaderParam1i("u_cull",1);
      }
      else
      {
          m_shaderLib->setShaderParam1i("u_cull",0);
      }

      if (m_drawMesh)
      {
          if (m_colourMapApplied)
          {
              m_shaderLib->setShaderParam4f("u_colour",m_colourMapColour.x(),m_colourMapColour.y(),m_colourMapColour.z(),1.0);
              m_shaderLib->setShaderParam1i("u_rampPoints",m_rampPoints);
              if (m_rampPoints)
              {
                  if (!m_userDefinedRamp)
                  {
                      // automatic ramping so get extremes of the bounding box of the file
                      m_channelRampExtremes = m_vdb->getBBox().asBBoxBare();
                  }
                  m_shaderLib->setShaderParam3f("u_Min",m_channelRampExtremes.minx,m_channelRampExtremes.miny,m_channelRampExtremes.minz);
                  m_shaderLib->setShaderParam3f("u_Max",m_channelRampExtremes.maxx,m_channelRampExtremes.maxy,m_channelRampExtremes.maxz);
              }
              else
              {
                  if (!m_userDefinedRamp)
                  {
                      // automatic ramping
                      m_channelRampExtremes = m_vdb->getCurrentChannelExtremes();
                      if (m_colourMapOnCull)
                      {
                          // if ramping onto a cull, upload the correct data to the correct location
                          setMapOnCullValues();
                      }
                  }
                  // upload the extremes of the channel chosen
                  m_shaderLib->setShaderParam3f("u_Min",m_channelRampExtremes.minx,m_channelRampExtremes.miny,m_channelRampExtremes.minz);
                  m_shaderLib->setShaderParam3f("u_Max",m_channelRampExtremes.maxx,m_channelRampExtremes.maxy,m_channelRampExtremes.maxz);
              }
          }
          if (m_cullingEnabled)
          {
              // if culling is being used, upload all the culls to the shader
            uploadCullsToShader();
          }
          // draw the vdb file
          m_vdb->drawVDB();
      }

      if (m_drawBBox)
      {
          m_shaderLib->setActive("Colour");
          m_shaderLib->setShaderUniformFromMat4("MVP",MVP);

          openvdb::Vec3f colour = m_vdb->getBBox().colour();

          float r = colour.x();
          float g = colour.y();
          float b = colour.z();

          m_shaderLib->setShaderParam4f("u_colour",r,g,b,1.0f);
          // draw the bounding box
          m_vdb->drawBBox();
      }

      if (m_drawVDBTree)
      {
          m_shaderLib->setActive("VDBTree");
          m_shaderLib->setShaderUniformFromMat4("MVP",MVP);
          // darw the VDB tree
          m_vdb->drawTree(m_shaderLib);
      }

      if (m_drawVectors)
      {
          // ame format as drawing the mesh although this time it will draw the vectors
          if (!m_useCrop)
          {
              m_shaderLib->setActive("Vector");
          }
          else
          {
              m_shaderLib->setActive("VectorCrop");
              m_shaderLib->setShaderUniformFromMat4("u_transform",m_vdb->transform());
              // upload crop boxes to the shader
              uploadCropBoxesToShader();
          }
          m_shaderLib->setShaderUniformFromMat4("MVP",MVP);
          m_shaderLib->setShaderParam1f("u_s",m_s);
          m_shaderLib->setShaderParam1i("u_invert",m_invertVectorColour);
          m_shaderLib->setShaderParam1i("u_renderMode",m_vectorColourMode);
          m_shaderLib->setShaderParam1f("u_vectorSize",m_vdb->vectorSize());

          if (m_cullingEnabled)
          {
              // upload culling information
              m_shaderLib->setShaderParam1i("u_cull",1);
              uploadCullsToShader();
          }
          else
          {
              m_shaderLib->setShaderParam1i("u_cull",0);
          }

          m_vdb->drawVectors(m_shaderLib);
      }
      m_resetTransform = false;
  }

  if (m_drawGrid)
  {
      // use a local transform so the grid will not rotate along with the VDB volume
      openvdb::Mat4s localMVP = m_grid->transform()*m_camera->getViewProjectionMatrix();
      m_shaderLib->setActive("Colour");
      m_shaderLib->setShaderUniformFromMat4("MVP",localMVP);
      m_shaderLib->setShaderParam4f("u_colour",0.6f,0.6f,0.6f,1.0f);
      // draw the grid
      m_grid->draw();
  }

  // update the UI output of the current used GPU memory
  emit(updateUsedGPUMem());
}

void GLWindow::setCrop(float _minx, float _maxx, float _miny, float _maxy, float _minz, float _maxz, int _index)
{
    m_vdb->setCrop(_minx, _maxx, _miny, _maxy, _minz, _maxz,_index);
}

void GLWindow::setVectorColour(float _r, float _g, float _b)
{
    if (m_vdbLoaded)
    {
        m_vdb->setVectorColour(openvdb::Vec3f(_r/255.0f,_g/255.0f,_b/255.0f));
    }
}

void GLWindow::setPointChannel(int _channel)
{
    if (m_vdbLoaded)
    {
        m_vdb->setPointChannel(_channel);
    }
}

void GLWindow::setVectorChannel(int _channel)
{
    if (m_vdbLoaded)
    {
        m_vdb->setVectorChannel(_channel);
    }
}

void GLWindow::moveAroundOrigin(bool _move)
{
    m_moveAroundOrigin = _move;
    if (!_move)
    {
        m_diffX = 0.0f;
        m_diffY = 0.0f;
    }
}

int GLWindow::toggleMesh()
{
    if (m_vdbLoaded)
    {
        if (!m_extLoaded && m_drawMesh == false)
        {
            // if the high res has not alreayd been loaded and it is not currently trying to draw the high resolution volume
            // pose warning and question of wanting to continue
            QMessageBox::StandardButton reply;
            QMessageBox loadHigh;
            loadHigh.raise();
            reply = loadHigh.question(NULL,"Loading High Resolution", "Loading High Resolution volumes can be memory intensive and time consuming. Are you sure you wish to continue?",QMessageBox::Yes|QMessageBox::No);
            loadHigh.close();
            if (reply == QMessageBox::Yes)
            {
                // if positive response, continue with load
                if (m_vdb->loadExt())
                {
                    m_extLoaded = true;
                }
                else
                {
                    // else do nothing and ensure the boolean declaring if it has been loaded or not is set to false
                    m_extLoaded = false;
                }
            }
            else
            {
                return 0;
            }
        }
        m_drawMesh ^= true;
    }
    return 1;
    updateGL();
}

void GLWindow::toggleBBox()
{
    m_drawBBox ^= true;
    updateGL();
}

void GLWindow::toggleVectors()
{
    m_drawVectors ^= true;
    updateGL();
}

void GLWindow::toggleVDB()
{
    m_drawVDBTree ^= true;
    updateGL();
}

void GLWindow::toggleGrid()
{
    m_drawGrid ^= true;
    updateGL();
}

void GLWindow::toggleCrop()
{
    m_useCrop ^= true;
    updateGL();
}

void GLWindow::setVectorLength(float _delta)
{
    if (m_vdbLoaded)
    {
        m_vdb->setVectorSize(_delta);
        updateGL();
    }
}

void GLWindow::setLOD(float _lod)
{
    if (m_vdbLoaded)
    {
        // change LOD in VDB class
        m_vdb->changeLoadPercentFactor(_lod);
        // retrieve newly calculated s value
        m_s = m_vdb->getS();
        if (m_s < 1.0f) // ensure s does not creep below 1
        {
            m_s = 1.0f;
        }
    }
}

void GLWindow::setScanSpeed(float _speed)
{
    m_scanSpeed = _speed;
}

void GLWindow::setScanXWidth(float _w, int _index)
{
    m_xScanWidth = _w;
    if (m_xScan)
    {
        m_vdb->setCropW(m_xScanWidth,_index);
        updateGL();
    }
}

void GLWindow::setScanYHeight(float _h, int _index)
{
    m_yScanHeight = _h;
    if (m_yScan)
    {
        m_vdb->setCropH(m_yScanHeight,_index);
        updateGL();
    }
}

void GLWindow::setScanZDepth(float _d, int _index)
{
    m_zScanDepth = _d;
    if (m_zScan)
    {
        m_vdb->setCropD(m_zScanDepth,_index);
        updateGL();
    }
}

void GLWindow::toggleLevel(int _level)
{
    if (m_vdb->m_drawTreeLevels[_level] == 0)
    {
        m_vdb->m_drawTreeLevels[_level] = 1;
    }
    else
    {
        m_vdb->m_drawTreeLevels[_level] = 0;
    }
}

void GLWindow::toggleLevel0()
{
    if (m_vdbLoaded)
    {
        toggleLevel(3);
        updateGL();
    }
}

void GLWindow::toggleLevel1()
{
    if (m_vdbLoaded)
    {
        toggleLevel(2);
        updateGL();
    }
}

void GLWindow::toggleLevel2()
{
    if (m_vdbLoaded)
    {
        toggleLevel(1);
        updateGL();
    }
}

void GLWindow::toggleLevel3()
{
    if (m_vdbLoaded)
    {
        toggleLevel(0);
        updateGL();
    }
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::mouseMoveEvent (QMouseEvent * _event)
{
    // rotation from the left mouse button
    if(m_rotate && _event->buttons() == Qt::LeftButton)
    {
      int diffx=_event->x()-(int)m_origX;
      int diffy=_event->y()-(int)m_origY;
      m_totalDiffX -= 0.5f * (float)diffy;
      m_totalDiffY -= 0.5f * (float)diffx;

      if (!m_moveAroundOrigin)
      {
          // rotate the model
        m_spinX -= 0.5f * (float)diffy;
        m_spinY -= 0.5f * (float)diffx;
      }
      else
      {
          // rotate the camera
        m_diffY = 0.5f *(float)diffx;
        m_diffX = 0.5f *(float)diffy;
      }
      m_origX = _event->x();
      m_origY = _event->y();
      updateGL();

    }// right mouse translate code
    else if(m_translate && _event->buttons() == Qt::RightButton && !m_moveAroundOrigin)
    {
      int diffX = (int)(_event->x() - m_origXPos);
      int diffY = (int)(_event->y() - m_origYPos);
      m_origXPos=_event->x();
      m_origYPos=_event->y();
      // set mouse translate to be applied on the next render pass
      m_mouseTranslate[0] = m_mouseTranslate.x() + (INCREMENT * diffX);
      m_mouseTranslate[1] = m_mouseTranslate.y() - (INCREMENT * diffY);
      updateGL();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mousePressEvent ( QMouseEvent * _event)
{
    if(_event->button() == Qt::LeftButton)
    {
      m_origX = _event->x();
      m_origY = _event->y();
      // set rotation to true
      m_rotate =true;
    }
    // right mouse translate mode
    else if(_event->button() == Qt::RightButton)
    {
      m_origXPos = _event->x();
      m_origYPos = _event->y();
      // translation mode so set translate to true
      m_translate=true;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::mouseReleaseEvent ( QMouseEvent * _event )
{
    if (_event->button() == Qt::LeftButton)
    {
        // rotation no longer active
      m_rotate=false;
    }
          // right mouse translate mode
    if (_event->button() == Qt::RightButton)
    {
        // translation no longer active
      m_translate=false;
    }
}

//----------------------------------------------------------------------------------------------------------------------
void GLWindow::wheelEvent(QWheelEvent *_event)
{
    // zooming in and out
    openvdb::Vec4f now(0.0f,0.0f,0.0f,0.0f);
    if(_event->delta() > 0)
    {
        // if wheel has been move up - zoom in
        // get vector between camera and eye
        now = (m_camera->getLook()-m_camera->getEye());
        // create length 1 of this vector
        now.normalize();
        // create a sclae of this vector
        now *= m_wheelZoomDelta;
        if ((m_camera->getLook()-m_camera->getEye()).length() >= 0.4)
        {
            // move the camera along this scaled normal vector
            m_camera->moveEye(now.x(),now.y(),now.z());
        }
    }
    else if(_event->delta() <0 )
    {
        // do the same as above but wheel has been scrolled down so zoom out
        now = -(m_camera->getLook()-m_camera->getEye());
        now.normalize();
        now *= m_wheelZoomDelta;
        m_camera->moveEye(now.x(),now.y(),now.z());
    }
    updateGL();
}
//----------------------------------------------------------------------------------------------------------------------

void GLWindow::runScan(int _axis)
{
    //***********************************************************************************
    // currently this assumes only crop box at index 0 will be used (only one crop box)
    // future development will allow any of the crop boxes to be used to allow more than
    // one scan at a time
    //***********************************************************************************

    if (m_vdbLoaded) // only run scan if vdb file has been loaded
    {
        if (m_scanRunning)
        {
            // if there is a previous scan running, kill the old scan
            killTimer(m_timer);
        }
        m_xScan = false;
        m_yScan = false;
        m_zScan = false;

        if (!m_scanRunning)
        {
            // put all current crop boxes into storage
            m_vdb->setAllCropStorage();
            // only draw one bound box as this is all a scan has
            m_vdb->setNumCropToDraw(1);
        }
        m_scanRunning = true;

        openvdb::Vec4f min(m_vdb->getBBox().minX(), m_vdb->getBBox().minY(), m_vdb->getBBox().minZ(),1.0f);
        openvdb::Vec4f max(m_vdb->getBBox().maxX(), m_vdb->getBBox().maxY(), m_vdb->getBBox().maxZ(),1.0f);

        switch(_axis)
        {
        // dependant on the axis, size up the crop box for the scan from the bounding box
            case(0):
            {
                m_xScan = true;
                max[0] = min.x()+m_xScanWidth;
            }break;
            case(1):
            {
                m_yScan = true;
                max[1] = min.y()+m_yScanHeight;
            }break;
            case(2):
            {
                m_zScan = true;
                max[2] = min.z()+m_zScanDepth;
            }break;
        }

        m_vdb->setCrop(min.x(),max.x(),min.y(),max.y(),min.z(),max.z(),0);

        m_useCrop = true;
        // start timer for the scan
        m_timer = startTimer(TIMER_EXEC_INTERVAL);
    }
}

void GLWindow::pauseScan()
{
    killTimer(m_timer);
}

void GLWindow::resumeScan()
{
    m_timer = startTimer(TIMER_EXEC_INTERVAL);
}

void GLWindow::stopScan()
{
    if (m_scanRunning)
    {
        // only attempt to kill the timer if a scan is running
        killTimer(m_timer);
        m_scanRunning = false;
        // get all previous crop boxes back from storage
        m_vdb->returnAllFromStorage();
        m_useCrop = false;

        m_scanDirection = 0;
        // set all scans to inactive
        m_xScan = false;
        m_yScan = false;
        m_zScan = false;
        updateGL();
    }
}

void GLWindow::setRampMin(float _min)
{
    // set min values of channel extremes for ramp to the same value
    m_channelRampExtremes.minx = m_channelRampExtremes.miny = m_channelRampExtremes.minz = _min;
}

void GLWindow::setRampMax(float _max)
{
    // set max values of channel extremes for ramp to the same value
    m_channelRampExtremes.maxx = m_channelRampExtremes.maxy = m_channelRampExtremes.maxz = _max;
}

void GLWindow::removeMesh()
{
    if (m_extLoaded)
    {
        // remove the high resolution VDB volume
        m_extLoaded = false;
        m_drawMesh = false;
        // removes the VAO ready to be built again
        m_vdb->removeMeshVAO();
        updateGL();
    }
}

void GLWindow::manualCropMove(float _delta)
{
    //***********************************************************************************
    // currently this assumes only crop box at index 0 will be used (only one crop box)
    // future development will allow any of the crop boxes to be used to allow more than
    // one scan at a time
    //***********************************************************************************

    // manually move the scan crop box
    openvdb::Vec3f translate(0.0f,0.0f,0.0f);
    if (m_xScan) // if scan is currently in the x
    {
        // default the change to the value of _delta
        translate[0] = _delta;
        // if the crop box has reached one side of the bounding box, calculate its change of direction
        if (m_vdb->getCBox(0).minX() + _delta <= m_vdb->getBBox().minX() ||
                m_vdb->getCBox(0).maxX() + _delta >= m_vdb->getBBox().maxX())
        {
            // default to one direction
            translate[0] = m_vdb->getBBox().maxX() - m_vdb->getCBox(0).maxX();
            // change if the opposite is true
            if (m_vdb->getCBox(0).minX() + _delta <= m_vdb->getBBox().minX())
            {
                translate[0] = m_vdb->getBBox().minX() - m_vdb->getCBox(0).minX();
            }
        }
    }
    else if (m_yScan) // if scan is currently in the y
    {
        // same logic as in the x axis but this time in the y axis
        translate[1] = _delta;
        if (m_vdb->getCBox(0).minY() + _delta <= m_vdb->getBBox().minY() ||
                m_vdb->getCBox(0).maxY() + _delta >= m_vdb->getBBox().maxY())
        {
            translate[1] = m_vdb->getBBox().maxY() - m_vdb->getCBox(0).maxY();
            if (m_vdb->getCBox(0).minY() + _delta <= m_vdb->getBBox().minY())
            {
                translate[1] = m_vdb->getBBox().minY() - m_vdb->getCBox(0).minY();
            }
        }
    }
    else // if scan is currently in the z
    {
        // same logic as in the x axis but this time in the z axis
        translate[2] = _delta;
        if (m_vdb->getCBox(0).minZ() + _delta <= m_vdb->getBBox().minZ() ||
                m_vdb->getCBox(0).maxZ() + _delta >= m_vdb->getBBox().maxZ())
        {
            translate[2] = m_vdb->getBBox().maxZ() - m_vdb->getCBox(0).maxZ();
            if (m_vdb->getCBox(0).minZ() + _delta <= m_vdb->getBBox().minZ())
            {
                translate[2] = m_vdb->getBBox().minZ() - m_vdb->getCBox(0).minZ();
            }
        }
    }
    m_vdb->translateCrop(translate,0);
}

BBoxBare GLWindow::getChannelExtremes(int _index)
{
    if (m_vdbLoaded && m_extLoaded)
    {
        // if vdb is loaded, return the channel extremes at the given channel
        return m_vdb->getChannelExtremesAt(_index);
    }
    else
    {
        // else return a null 0 value BBoxBare
        BBoxBare nullReturn;
        nullReturn.minx = nullReturn.miny = nullReturn.minz = 0.0f;
        nullReturn.maxx = nullReturn.maxy = nullReturn.maxz = 0.0f;
        return nullReturn;
    }
}

void GLWindow::setCull(Cull _cull, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index] = _cull;
    }
}

void GLWindow::setCullActive(bool _active, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index]._active = _active;
    }
}

void GLWindow::setCullChannelOffset(int _channel, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        // calculate the total offset for the passed in channel and save it to the cull
        // who corresponds to that channel - use for texture buffer look up
        int count = 0;
        for (int i =0; i < _channel; i++)
        {
            count += m_vdb->getNumPointsAtGrid(i);
        }
        m_culls[_index]._channelOffset = count;
    }
}

void GLWindow::setCullChannelType(int _type, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index]._channelType = _type;
    }
}

void GLWindow::setCullType(int _type, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        if (_type >=0 && _type < 3)
        {
            m_culls[_index]._cullType = _type;
        }
    }
}

void GLWindow::setCullULimit(float _u, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index]._floatULimit = _u;
    }
}

void GLWindow::setCullLLimit(float _l, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index]._floatLLimit = _l;
    }
}

void GLWindow::setCullLLimit(openvdb::Vec3s _l, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index]._vecLLimit = _l;
    }
}

void GLWindow::setCullULimit(openvdb::Vec3s _u, int _index)
{
    if (_index >= 0 && _index < 3)
    {
        m_culls[_index]._vecULimit = _u;
    }
}

Cull GLWindow::getCull(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index];
    }

    Cull nullCull;
    return nullCull;
}

bool GLWindow::getCullActive(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._active;
    }
    return false;
}

int GLWindow::getCullChannelOffset(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._channelOffset;
    }
    return -1;
}

int GLWindow::getCullChannelType(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._channelType;
    }
    return -1;
}

int GLWindow::getCullType(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._cullType;
    }
    return -1;
}

float GLWindow::getCullFloatLLimit(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._floatLLimit;
    }
    return 0.0f;
}

float GLWindow::getCullFloatULimit(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._floatULimit;
    }
    return 1.0f;
}

openvdb::Vec3s GLWindow::getCullVecLLimit(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._vecLLimit;
    }
    openvdb::Vec3s nullVec;
    return nullVec;
}

openvdb::Vec3s GLWindow::getCullVecULimit(int _index)
{
    if (_index >= 0 && _index < 3)
    {
        return m_culls[_index]._vecULimit;
    }
    openvdb::Vec3s nullVec;
    return nullVec;
}

std::string GLWindow::GPUVendor()
{
    return Utilities::glubyteToStdString(m_GPUVendor);
}

std::string GLWindow::OpenGLVersion()
{
    return Utilities::glubyteToStdString(m_GPUVersion);
}

std::string GLWindow::GPURenderer()
{
    return Utilities::glubyteToStdString(m_GPURenderer);
}

std::string GLWindow::totalGPUMemMB(bool _ext)
{
#ifdef EXT_GPU
    #ifdef NVIDIA
        int mb = m_total_GPU_mem_kb/1024;
        std::string ret = boost::lexical_cast<std::string>(mb);
        if (_ext)
        {
            ret += "MB";
        }
        return ret;
    #else
        return "AMD not supported";
    #endif
#else
    return "Not available";
#endif
}

std::string GLWindow::currentUsedGPUMemMB(bool _ext)
{
#ifdef EXT_GPU
    #ifdef NVIDIA
        int mb = (m_total_GPU_mem_kb - currentAvailableGPUMemKB())/1024;
        std::string ret = boost::lexical_cast<std::string>(mb);
        if (_ext)
        {
            ret += "MB";
        }
        return ret;
    #else
        return "AMD not supported";
    #endif
#else
    return "Not available";
#endif
}

std::string GLWindow::fileName()
{
    // this function is a pretty standard way of extracting the file name
    // it will scan the file path from the end to the front until it find the first /
    // once found, the character it has stored it will then reverse and this becomes the file name
    if (m_vdbLoaded)
    {
        std::string path = m_vdb->filename();
        std::string temp, name;
        temp = "";
        name = "";
        int index = path.length()-1;

        bool stop = false;

        while (!stop && index > -1)
        {
            if (path[index] == '/')
            {
                stop = true;
            }
            else
            {
                temp += path[index];
                index--;
            }
        }

        if (stop)
        {
            for (int i = temp.length()-1; i > -1; i--)
            {
                name += temp[i];
            }
            return name;
        }
        else
        {
            return "Error getting VDB file name";
        }
    }
    else
    {
        return "No VDB file loaded";
    }
}

std::string GLWindow::filePath()
{
    if (m_vdbLoaded)
    {
        return m_vdb->filename();
    }
    return "No VDB file loaded";
}

std::string GLWindow::fileVersion()
{
    if (m_vdbLoaded)
    {
        return m_vdb->fileVersion();
    }
    return "No VDB file loaded";
}

int GLWindow::numChannels()
{
    if (m_vdbLoaded)
    {
        return m_vdb->numChannels();
    }
    return -1;
}

int GLWindow::numGrids()
{
    if (m_vdbLoaded)
    {
        return m_vdb->numChannels();
    }
    return -1;
}

std::string GLWindow::gridName(int _channel)
{
    if (m_vdbLoaded)
    {
        return m_vdb->channelName(_channel);
    }
    return "No VDB file loaded";
}

std::string GLWindow::gridType(int _channel)
{
    if (m_vdbLoaded)
    {
        return m_vdb->channelType(_channel);
    }
    return "No VDB file loaded";
}

std::string GLWindow::gridDimAt(int _grid)
{
    if (m_vdbLoaded)
    {
        return Utilities::vdbCoordToStdString(m_vdb->gridDimAt(_grid));
    }
    return "No VDB file loaded";
}

std::string GLWindow::voxelCount()
{
    if (m_vdbLoaded)
    {
        return boost::lexical_cast<std::string>(m_vdb->voxelCount());
    }
    return "No VDB file loaded";
}

int GLWindow::treeDepth()
{
    if (m_vdbLoaded)
    {
        return m_vdb->treeDepth();
    }
    return -1;
}

std::string GLWindow::voxelCountAtTreeDepth(int _depth)
{
    if (m_vdbLoaded)
    {
        return boost::lexical_cast<std::string>(m_vdb->voxelCountAtTreeDepth(_depth));
    }
    return "No VDB file loaded";
}

int GLWindow::numMeta()
{
    if (m_vdbLoaded)
    {
        return m_vdb->numMeta();
    }
    return -1;
}

std::string GLWindow::metaNameAt(int _index)
{
    if (m_vdbLoaded)
    {
        return m_vdb->metaNameAt(_index);
    }
    return "No VDB file loaded";
}

int GLWindow::metaNameIndex(std::string _name, int _skip)
{
    // this function will serach the stored list of meta data values and find the index of the one desired
    // as there possibilities of multiple channels with the same name (multiple channels) there is a skip
    // parameter which determines how many found items to ignore before returning the value
    bool found = false;
    int count = 0;
    int index = -1;
    int numFound = 0;

    while (!found && count < m_vdb->numMeta())
    {
        if (m_vdb->metaNameAt(count) == _name)
        {
            index = count;
            numFound++;
            if (!(numFound < _skip))
            {
                found = true;
            }
        }
        count++;
    }

    return index;
}

std::string GLWindow::metaValueAt(std::string _name, int _skip)
{
    if (m_vdbLoaded)
    {
        int ind = metaNameIndex(_name,_skip);
        if (ind == -1)
        {
            return "__not_found__";
        }
        else
        {
            return m_vdb->metaValueAt(ind);
        }
    }
    return "No VDB file loaded";
}

std::string GLWindow::metaValueAt(int _index)
{
    if (m_vdbLoaded)
    {
        return m_vdb->metaValueAt(_index);
    }
    return "No VDB file loaded";
}

void GLWindow::timerEvent(QTimerEvent *)
{
    openvdb::Vec3f translate(0.0f,0.0f,0.0f);
    if (m_xScan)
    {
        // calculate the direction the box needs to be translated
        scanDirection();
        // set direction as a default and then invert its direction if needed
        translate[0] = -m_scanSpeed;
        if (m_scanDirection == 0)
        {
            translate[0] *= -1;
        }
    }
    if (m_yScan)
    {
        // calculate the direction the box needs to be translated
        scanDirection();
        // set direction as a default and then invert its direction if needed
        translate[1] = -m_scanSpeed;
        if (m_scanDirection == 0)
        {
            translate[1] *= -1;
        }
    }
    if (m_zScan)
    {
        // calculate the direction the box needs to be translated
        scanDirection();
        // set direction as a default and then invert its direction if needed
        translate[2] = -m_scanSpeed;
        if (m_scanDirection == 0)
        {
            translate[2] *= -1;
        }
    }
    m_vdb->translateCrop(translate,0);
    updateGL();
}

void GLWindow::scanDirection()
{
    if (m_xScan)
    {
        if (m_vdb->getCBox(0).minX() <= m_vdb->getBBox().minX())
        {
            m_scanDirection = 0;
        }
        if (m_vdb->getCBox(0).maxX() >= m_vdb->getBBox().maxX())
        {
            m_scanDirection = 1;
        }
    }
    else if (m_yScan)
    {
        if (m_vdb->getCBox(0).minY() <= m_vdb->getBBox().minY())
        {
            m_scanDirection = 0;
        }
        if (m_vdb->getCBox(0).maxY() >= m_vdb->getBBox().maxY())
        {
            m_scanDirection = 1;
        }
    }
    else
    {
        if (m_vdb->getCBox(0).minZ() <= m_vdb->getBBox().minZ())
        {
            m_scanDirection = 0;
        }
        if (m_vdb->getCBox(0).maxZ() >= m_vdb->getBBox().maxZ())
        {
            m_scanDirection = 1;
        }
    }
}

void GLWindow::setWheelZoomSpeed()
{
    if (m_vdbLoaded)
    {
        // if the vdb file is loaded, calculate the length between the camera and the centre of the bounding box
        openvdb::Vec4f vec = m_camera->getEye() - openvdb::Vec4f(m_vdb->getBBox().centre().x(),m_vdb->getBBox().centre().y(),m_vdb->getBBox().centre().z(),1.0f);
        float distanceFromBBoxCentre = vec.length();
        // set to value dependant on its distance
        m_wheelZoomDelta = (distanceFromBBoxCentre *0.01f) + 0.4f;
        if (m_vdb->getBBox().pointInside(openvdb::Vec3f(m_camera->getEye().x(),m_camera->getEye().y(),m_camera->getEye().z())))
        {
            // if then calculated that you are within the bounding box, set to a constant
            m_wheelZoomDelta = 0.4f;
        }
    }
}

void GLWindow::setMapOnCullValues()
{
    // dependant on which cull has been chosen and of which type, a different upload method of values must takle place
    if (m_culls[m_applyColourMapToCullNumber]._active)
    {
        switch(m_culls[m_applyColourMapToCullNumber]._cullType)
        {
            case(0): // less than < cull
            {
                switch(m_culls[m_applyColourMapToCullNumber]._channelType)
                {
                // lower limit variables are used for single value uploads
                    case(0): // type is float
                    {
                        m_channelRampExtremes.minx = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.miny = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.minz = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                    }break;
                    case(1): // type is vector
                    {
                        m_channelRampExtremes.minx = m_culls[m_applyColourMapToCullNumber]._vecLLimit.x();
                        m_channelRampExtremes.miny = m_culls[m_applyColourMapToCullNumber]._vecLLimit.y();
                        m_channelRampExtremes.minz = m_culls[m_applyColourMapToCullNumber]._vecLLimit.z();
                    }break;
                }
            }break;
            case(1): // between cull
            {
                switch(m_culls[m_applyColourMapToCullNumber]._channelType)
                {
                    case(0): // type is float
                    {
                        m_channelRampExtremes.minx = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.miny = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.minz = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.maxx = m_culls[m_applyColourMapToCullNumber]._floatULimit;
                        m_channelRampExtremes.maxy = m_culls[m_applyColourMapToCullNumber]._floatULimit;
                        m_channelRampExtremes.maxz = m_culls[m_applyColourMapToCullNumber]._floatULimit;
                    }break;
                    case(1): // type is vector
                    {
                        m_channelRampExtremes.minx = m_culls[m_applyColourMapToCullNumber]._vecLLimit.x();
                        m_channelRampExtremes.miny = m_culls[m_applyColourMapToCullNumber]._vecLLimit.y();
                        m_channelRampExtremes.minz = m_culls[m_applyColourMapToCullNumber]._vecLLimit.z();
                        m_channelRampExtremes.maxx = m_culls[m_applyColourMapToCullNumber]._vecULimit.x();
                        m_channelRampExtremes.maxy = m_culls[m_applyColourMapToCullNumber]._vecULimit.y();
                        m_channelRampExtremes.maxz = m_culls[m_applyColourMapToCullNumber]._vecULimit.z();
                    }break;
                }
            }break;
            case(2): // greater than > cull
            {
                switch(m_culls[m_applyColourMapToCullNumber]._channelType)
                {
                // lower limit variables are used for single value uploads
                    case(0): // type is float
                    {
                        m_channelRampExtremes.maxx = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.maxy = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                        m_channelRampExtremes.maxz = m_culls[m_applyColourMapToCullNumber]._floatLLimit;
                    }break;
                    case(1): // type is vector
                    {
                        m_channelRampExtremes.maxx = m_culls[m_applyColourMapToCullNumber]._vecLLimit.x();
                        m_channelRampExtremes.maxy = m_culls[m_applyColourMapToCullNumber]._vecLLimit.y();
                        m_channelRampExtremes.maxz = m_culls[m_applyColourMapToCullNumber]._vecLLimit.z();
                    }break;
                }
            }break;
        }
    }
}

void GLWindow::uploadSingleCropBoxToShader(std::string _uniformName, int _index)
{
    // upload to the crop box structure on the shader
    BoundBox temp = m_vdb->getCBox(_index);
    m_shaderLib->setShaderParam3f(_uniformName+"._min",temp.minX(),temp.minY(),temp.minZ());
    m_shaderLib->setShaderParam3f(_uniformName+"._max",temp.maxX(),temp.maxY(),temp.maxZ());
}

void GLWindow::uploadCropBoxesToShader()
{
    // detect which crop boxes are active and upload only these to the shaders
    int numCrops = m_vdb->getNumCropsDrawn();
    m_shaderLib->setShaderParam1i("u_numCrops",numCrops);
    if (numCrops >= 0)
    {
        uploadSingleCropBoxToShader("u_crop1",0);
    }
    if (numCrops >= 1)
    {
        uploadSingleCropBoxToShader("u_crop2",1);
    }
    if (numCrops >= 2)
    {
        uploadSingleCropBoxToShader("u_crop3",2);
    }
}

void GLWindow::uploadSingleCullToShader(std::string _uniformName, int _index)
{
    // upload data into the cull structure on the shader
    m_shaderLib->setShaderParam1i(_uniformName+"._active",m_culls[_index]._active);
    m_shaderLib->setShaderParam1i(_uniformName+"._channelOffset",m_culls[_index]._channelOffset);
    m_shaderLib->setShaderParam1i(_uniformName+"._channelType",m_culls[_index]._channelType);
    m_shaderLib->setShaderParam1i(_uniformName+"._cullType",m_culls[_index]._cullType);
    m_shaderLib->setShaderParam1f(_uniformName+"._floatULimit",m_culls[_index]._floatULimit);
    m_shaderLib->setShaderParam1f(_uniformName+"._floatLLimit",m_culls[_index]._floatLLimit);
    m_shaderLib->setShaderParam3f(_uniformName+"._vecULimit",m_culls[_index]._vecULimit.x(),m_culls[_index]._vecULimit.y(),m_culls[_index]._vecULimit.z());
    m_shaderLib->setShaderParam3f(_uniformName+"._vecLLimit",m_culls[_index]._vecLLimit.x(),m_culls[_index]._vecLLimit.y(),m_culls[_index]._vecLLimit.z());
}

void GLWindow::uploadCullsToShader()
{
    // upload all culls to shader
        uploadSingleCullToShader("u_cull1",0);

        uploadSingleCullToShader("u_cull2",1);

        uploadSingleCullToShader("u_cull3",2);
}
