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


#include "MainWindow.h"
#include "ui_MWindow.h"
#include <iostream>

#include <QFileDialog>

#include <QApplication>

#include "BoundBox.h"
#include "Utilities.h"

#include <boost/lexical_cast.hpp>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent),  m_ui(new Ui::MWindow)
{
    m_ui->setupUi(this);
    initGLWidget();
}

MainWindow::~MainWindow()
{
    // make sure the information table is removed correctly
    delete m_ui;
}

void MainWindow::initGLWidget()
{
    QGLFormat format;
#ifdef GL32
    format.setVersion(3,2);
#endif
#ifdef GL42
    format.setVersion(4,2);
#endif
#ifdef GL43
    format.setVersion(4,3);
#endif
#ifdef GL44
    format.setVersion(4,4);
#endif

#ifdef DARWIN
    format.setProfile( QGLFormat::CoreProfile);
#endif

    format.setSamples(3);

    // create new glWindow
    m_glWindow = new GLWindow(format,this);
    // init the gl window
    m_glWindow->init(this);
    // add the gl window to the ui
    m_ui->glWindow->addWidget(m_glWindow,0,0,12,1);
    // pass this as the main window for the information window
    m_infoWindow.setMainWindow(this);
}

void MainWindow::connectAndInit()
{
    m_readCrop = false;

    m_cullToComboMap[0] = 0;
    m_cullToComboMap[1] = 0;
    m_cullToComboMap[2] = 0;

    m_renderNewCullColour = true;

    // connect all signals from the UI components to their correposnding slots

    connect(m_ui->actionOpen,SIGNAL(triggered(bool)),this,SLOT(openFile()));
    connect(m_ui->actionClear,SIGNAL(triggered(bool)),this,SLOT(clear()));
    connect(m_ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(quitApp()));
    connect(m_ui->actionInformation,SIGNAL(triggered(bool)),this,SLOT(showInformation()));
    connect(m_ui->btn_showFileInformation,SIGNAL(clicked(bool)),this,SLOT(showInformation()));
    connect(m_ui->actionAbout_OpenVDBViewer,SIGNAL(triggered()),this,SLOT(openAbout()));

    connect(m_ui->cbx_showGrid,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleGrid()));

    connect(m_ui->actionRemove_High_Resolution_Data,SIGNAL(triggered(bool)),this,SLOT(removeMesh()));
    connect(m_ui->actionReset_Transform,SIGNAL(triggered(bool)),m_glWindow,SLOT(resetVolumeTransform()));
    connect(m_ui->actionReset_Scene,SIGNAL(triggered(bool)),m_glWindow,SLOT(resetCameraAndVolumeTransform()));
    connect(m_ui->actionReset_Camera,SIGNAL(triggered(bool)),m_glWindow,SLOT(resetCameraVolume()));

    connect(m_ui->cbx_showMesh,SIGNAL(clicked(bool)),this,SLOT(toggleMesh()));

    connect(m_ui->cbx_applyColourMap,SIGNAL(clicked(bool)),this,SLOT(setColourMapApplied()));
    connect(m_ui->cmbo_applyCullToChannelSelect,SIGNAL(currentIndexChanged(int)),this,SLOT(setCullToChannelNumber()));

    connect(m_ui->rdio_applyToFile,SIGNAL(toggled(bool)),this,SLOT(setCullToChannel()));
    connect(m_ui->rdio_applyToCulledPoints,SIGNAL(toggled(bool)),this,SLOT(setCullToChannel()));

    connect(m_ui->rdio_userDefColourMap,SIGNAL(toggled(bool)),this,SLOT(setUserRamp()));
    connect(m_ui->rdio_automaticColourMap,SIGNAL(toggled(bool)),this,SLOT(setUserRamp()));

    connect(m_ui->dsbx_colourRampBottom,SIGNAL(valueChanged(double)),this,SLOT(setRampMin()));
    connect(m_ui->dsbx_colourRampTop,SIGNAL(valueChanged(double)),this,SLOT(setRampMax()));

    connect(m_ui->rdio_pointRamp,SIGNAL(toggled(bool)),this,SLOT(setRampPoints()));
    connect(m_ui->rdio_channelRamp,SIGNAL(toggled(bool)),this,SLOT(setRampPoints()));
    connect(m_ui->sbx_colourRampRed,SIGNAL(valueChanged(int)),this,SLOT(setRampColour()));
    connect(m_ui->sbx_colourRampGreen,SIGNAL(valueChanged(int)),this,SLOT(setRampColour()));
    connect(m_ui->sbx_colourRampBlue,SIGNAL(valueChanged(int)),this,SLOT(setRampColour()));


    connect(m_ui->cmbo_renderChannel,SIGNAL(currentIndexChanged(int)),this,SLOT(setRenderChannelFromRenderTab()));

    connect(m_ui->cmbo_currentPointChannelCulling_1,SIGNAL(currentIndexChanged(int)),this,SLOT(setCull1Type()));
    connect(m_ui->cmbo_cullingChoice1,SIGNAL(currentIndexChanged(int)),this,SLOT(setCull1Boolean()));
    connect(m_ui->dsbx_cull1FromX,SIGNAL(valueChanged(double)),this,SLOT(setCull1Values()));
    connect(m_ui->dsbx_cull1FromY,SIGNAL(valueChanged(double)),this,SLOT(setCull1Values()));
    connect(m_ui->dsbx_cull1FromZ,SIGNAL(valueChanged(double)),this,SLOT(setCull1Values()));
    connect(m_ui->dsbx_cull1ToX,SIGNAL(valueChanged(double)),this,SLOT(setCull1Values()));
    connect(m_ui->dsbx_cull1ToY,SIGNAL(valueChanged(double)),this,SLOT(setCull1Values()));
    connect(m_ui->dsbx_cull1ToZ,SIGNAL(valueChanged(double)),this,SLOT(setCull1Values()));
    connect(m_ui->cbx_useCulling1,SIGNAL(clicked(bool)),this,SLOT(setCullingEnabled()));

    connect(m_ui->cmbo_currentPointChannelCulling_2,SIGNAL(currentIndexChanged(int)),this,SLOT(setCull2Type()));
    connect(m_ui->cmbo_cullingChoice2,SIGNAL(currentIndexChanged(int)),this,SLOT(setCull2Boolean()));
    connect(m_ui->dsbx_cull2FromX,SIGNAL(valueChanged(double)),this,SLOT(setCull2Values()));
    connect(m_ui->dsbx_cull2FromY,SIGNAL(valueChanged(double)),this,SLOT(setCull2Values()));
    connect(m_ui->dsbx_cull2FromZ,SIGNAL(valueChanged(double)),this,SLOT(setCull2Values()));
    connect(m_ui->dsbx_cull2ToX,SIGNAL(valueChanged(double)),this,SLOT(setCull2Values()));
    connect(m_ui->dsbx_cull2ToY,SIGNAL(valueChanged(double)),this,SLOT(setCull2Values()));
    connect(m_ui->dsbx_cull2ToZ,SIGNAL(valueChanged(double)),this,SLOT(setCull2Values()));
    connect(m_ui->cbx_useCulling2,SIGNAL(clicked(bool)),this,SLOT(setCullingEnabled()));

    connect(m_ui->cmbo_currentPointChannelCulling_3,SIGNAL(currentIndexChanged(int)),this,SLOT(setCull3Type()));
    connect(m_ui->cmbo_cullingChoice3,SIGNAL(currentIndexChanged(int)),this,SLOT(setCull3Boolean()));
    connect(m_ui->dsbx_cull3FromX,SIGNAL(valueChanged(double)),this,SLOT(setCull3Values()));
    connect(m_ui->dsbx_cull3FromY,SIGNAL(valueChanged(double)),this,SLOT(setCull3Values()));
    connect(m_ui->dsbx_cull3FromZ,SIGNAL(valueChanged(double)),this,SLOT(setCull3Values()));
    connect(m_ui->dsbx_cull3ToX,SIGNAL(valueChanged(double)),this,SLOT(setCull3Values()));
    connect(m_ui->dsbx_cull3ToY,SIGNAL(valueChanged(double)),this,SLOT(setCull3Values()));
    connect(m_ui->dsbx_cull3ToZ,SIGNAL(valueChanged(double)),this,SLOT(setCull3Values()));
    connect(m_ui->cbx_useCulling3,SIGNAL(clicked(bool)),this,SLOT(setCullingEnabled()));

    connect(m_ui->cbx_showBBox,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleBBox()));

    connect(m_ui->cbx_showVectors,SIGNAL(clicked(bool)),this,SLOT(toggleChannelVectors()));

    connect(m_ui->cmbo_channelSelect,SIGNAL(currentIndexChanged(int)),this,SLOT(setVectorChannel()));

    connect(m_ui->cbx_showVDBTree,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleVDB()));

    connect(m_ui->cbx_treeLevel0,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleLevel0()));
    connect(m_ui->cbx_treeLevel1,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleLevel1()));
    connect(m_ui->cbx_treeLevel2,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleLevel2()));
    connect(m_ui->cbx_treeLevel3,SIGNAL(clicked(bool)),m_glWindow,SLOT(toggleLevel3()));

    connect(m_ui->hsld_vectorLength,SIGNAL(valueChanged(int)),this,SLOT(setVectorLength()));

    connect(m_ui->hsld_LOD,SIGNAL(valueChanged(int)),this,SLOT(setLOD()));
    connect(m_ui->hsld_cropSlide,SIGNAL(sliderPressed()),this,SLOT(startCropSlideTimer()));
    connect(m_ui->hsld_cropSlide,SIGNAL(sliderReleased()),this,SLOT(resetCropSlider()));

    connect(m_ui->cbx_useCrop,SIGNAL(clicked(bool)),this,SLOT(useCrop()));
    connect(m_ui->cmbo_numCrops,SIGNAL(currentIndexChanged(int)),this,SLOT(fillNumSlices()));
    connect(m_ui->cmbo_sliceSelection,SIGNAL(currentIndexChanged(int)),this,SLOT(fillCropDetails()));

    connect(m_ui->cmbo_vectorColourType,SIGNAL(currentIndexChanged(int)),this,SLOT(toggleVectorColourEnabled()));
    connect(m_ui->sbx_red,SIGNAL(valueChanged(int)),this,SLOT(setVectorColour()));
    connect(m_ui->sbx_green,SIGNAL(valueChanged(int)),this,SLOT(setVectorColour()));
    connect(m_ui->sbx_blue,SIGNAL(valueChanged(int)),this,SLOT(setVectorColour()));
    connect(m_ui->cbx_invertVectorColours,SIGNAL(clicked(bool)),this,SLOT(setVectorInvert()));

    connect(m_ui->sbx_pointSize,SIGNAL(valueChanged(double)),this,SLOT(setPointSize()));
    connect(m_ui->sbx_lineWidth,SIGNAL(valueChanged(double)),this,SLOT(setLineWidth()));

    connect(m_ui->dsbx_xScanWidth,SIGNAL(valueChanged(double)),this,SLOT(setScanXWidth()));
    connect(m_ui->dsbx_yScanHeight,SIGNAL(valueChanged(double)),this,SLOT(setScanYHeight()));
    connect(m_ui->dsbx_zScanDepth,SIGNAL(valueChanged(double)),this,SLOT(setScanZDepth()));

    connect(m_ui->dsbx_cropX,SIGNAL(valueChanged(double)),this,SLOT(setCrop()));
    connect(m_ui->dsbx_cropY,SIGNAL(valueChanged(double)),this,SLOT(setCrop()));
    connect(m_ui->dsbx_cropZ,SIGNAL(valueChanged(double)),this,SLOT(setCrop()));
    connect(m_ui->dsbx_cropWidth,SIGNAL(valueChanged(double)),this,SLOT(setCrop()));
    connect(m_ui->dsbx_cropHeight,SIGNAL(valueChanged(double)),this,SLOT(setCrop()));
    connect(m_ui->dsbx_cropDepth,SIGNAL(valueChanged(double)),this,SLOT(setCrop()));

    connect(m_ui->btn_startSingleScan,SIGNAL(clicked(bool)),this,SLOT(startSingleAxisScan()));
    connect(m_ui->btn_PauseScan,SIGNAL(clicked(bool)),this,SLOT(pauseResumeScan()));
    connect(m_ui->btn_stopScan,SIGNAL(clicked(bool)),this,SLOT(stopScan()));
    connect(m_ui->hsld_scanSpeed,SIGNAL(valueChanged(int)),this,SLOT(setScanSpeed()));

    // set up basic information that is file independent
    getHardwareInformation();

    std::cout<<"OpenVDBViewer application set up and ready to go..."<<std::endl;
}

void MainWindow::setStartup()
{
    // set up basic attributes to allow for manipulationm of data
    // set the level of detail
    setLOD();
    setPointSize();
    setLineWidth();
    // fill the number of crop boxes
    fillNumSlices();
    setScanSpeed();
    setScanXWidth();
    setScanYHeight();
    setScanZDepth();
    setVectorColour();
    setCropLimits();
}

void MainWindow::setCropLimits()
{
    BoundBox bbox = m_glWindow->getBBox();
    // set up the crop box size limits to be the size of the bounding box
    m_ui->dsbx_cropX->setMinimum(bbox.minX());
    m_ui->dsbx_cropX->setMaximum(bbox.maxX());

    m_ui->dsbx_cropY->setMinimum(bbox.minY());
    m_ui->dsbx_cropY->setMaximum(bbox.maxY());

    m_ui->dsbx_cropZ->setMinimum(bbox.minZ());
    m_ui->dsbx_cropZ->setMaximum(bbox.maxZ());

    m_ui->dsbx_cropDepth->setMaximum(bbox.depth());
    m_ui->dsbx_cropWidth->setMaximum(bbox.width());
    m_ui->dsbx_cropHeight->setMaximum(bbox.height());
}

void MainWindow::getHardwareInformation()
{
    // retrieve hardware information from the glwindow and pass to the information window
    m_infoWindow.setGPUVendor(m_glWindow->GPUVendor());
    m_infoWindow.setOpenGLVersion(m_glWindow->OpenGLVersion());
    m_infoWindow.setRenderer(m_glWindow->GPURenderer());
    m_infoWindow.setTotalGPUMem(m_glWindow->totalGPUMemMB());
    m_infoWindow.setUsedGPUMem(m_glWindow->currentUsedGPUMemMB());
}

void MainWindow::getFileInformation()
{
    // retrieve all file information from the glwindow and pass to the information window
    clearInfoTable();
    int row = 0;
    m_infoWindow.initInfoTable();
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("File Name"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->fileName().c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("File Path"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->filePath().c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("File Version"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->fileVersion().c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("High Resolution Count"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("file_voxel_count",1).c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("File Bounding Box Min"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("file_bbox_min",1).c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("File Bounding Box Max"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("file_bbox_max",1).c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("File Size Bytes"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("file_mem_bytes",1).c_str()));
    row++;

    if (m_glWindow->metaValueAt("class",1) != "__not_found__") // only put this into the table if it exists
    {
        m_infoWindow.insertTableRow(row);
        m_infoWindow.setTableItem(row,0,tr("Class"));
        m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("class",1).c_str()));
        row++;
    }

    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr(""));
    m_infoWindow.setTableItem(row,1,tr(""));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("Grids in File"));
    m_infoWindow.setTableItem(row,1,tr(""));
    row++;
    m_infoWindow.insertTableRow(row);
    int numGrids = m_glWindow->numGrids();
    m_infoWindow.setTableItem(row,0,tr("Number of Grids"));
    m_infoWindow.setTableItem(row,1,tr(boost::lexical_cast<std::string>(numGrids).c_str()));
    row++;

    // write to the table each grid in the file
    for (int i = 0; i < numGrids; ++i)
    {
        m_infoWindow.insertTableRow(row);
        std::string indexStr = boost::lexical_cast<std::string>(i+1);
        std::string temp = "Grid " + indexStr;
        m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
        m_infoWindow.setTableItem(row,1,tr(""));
        row++;
        m_infoWindow.insertTableRow(row);
        temp = "Name";
        m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
        m_infoWindow.setTableItem(row,1,tr(m_glWindow->gridName(i).c_str()));
        row++;
        m_infoWindow.insertTableRow(row);
        temp = "Type";
        std::string type = m_glWindow->gridType(i);
        m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
        m_infoWindow.setTableItem(row,1,tr(type.c_str()));
        row++;
        m_infoWindow.insertTableRow(row);
        temp = "Dimensions";
        m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
        m_infoWindow.setTableItem(row,1,tr(m_glWindow->gridDimAt(i).c_str()));
        row++;
        m_infoWindow.insertTableRow(row);
        temp = "Vector Type";
        m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
        std::string vecType = "__not_found__";
        if (m_glWindow->metaValueAt("VectorType",i+1) != "__not_found__") // if data exists in file
        {
            vecType = m_glWindow->metaValueAt("VectorType",i+1);
        }
        else if (m_glWindow->metaValueAt("vector_type",i+1) != "__not_found__") // if data exists in file
        {
            vecType = m_glWindow->metaValueAt("vector_type",i+1);
        }
        m_infoWindow.setTableItem(row,1,tr(vecType.c_str()));
        row++;

        if (m_glWindow->metaValueAt("is_local_space",i+1) != "__not_found__") // if data exists in file
        {
            m_infoWindow.insertTableRow(row);
            temp = "Local Space";
            m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
            m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("is_local_space",i+1).c_str()));
            row++;
        }

        if (m_glWindow->metaValueAt("is_saved_as_half_float",i+1) != "__not_found__") // if data exists in file
        {
            m_infoWindow.insertTableRow(row);
            temp = "Half Float";
            m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
            m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("is_saved_as_half_float",i+1).c_str()));
            row++;
        }

        if (m_glWindow->metaValueAt("roiMin",i+1) != "__not_found__") // if data exists in file
        {
            m_infoWindow.insertTableRow(row);
            temp = "ROI Min";
            m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
            m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("roiMin",i+1).c_str()));
            row++;
        }

        if (m_glWindow->metaValueAt("roiMax",i+1) != "__not_found__") // if data exists in file
        {
            m_infoWindow.insertTableRow(row);
            temp = "ROI Max";
            m_infoWindow.setTableItem(row,0,tr(temp.c_str()));
            m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt("roiMax",i+1).c_str()));
            row++;
        }
        m_infoWindow.insertTableRow(row);
        m_infoWindow.setTableItem(row,0,tr(""));
        m_infoWindow.setTableItem(row,1,tr(""));
        row++;
    }

    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("VDB Voxel Tree"));
    m_infoWindow.setTableItem(row,1,tr(""));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("Total Voxels"));
    m_infoWindow.setTableItem(row,1,tr(m_glWindow->voxelCount().c_str()));
    row++;
    m_infoWindow.insertTableRow(row);
    int treeDepth = m_glWindow->treeDepth();
    m_infoWindow.setTableItem(row,0,tr("Tree Depth"));
    m_infoWindow.setTableItem(row,1,tr(boost::lexical_cast<std::string>(treeDepth).c_str()));
    row++;

    for (int j = 0; j < treeDepth; j++) // loop for the depth of the tree to display all levels
    {
        m_infoWindow.insertTableRow(row);
        std::string heading = "Level " + boost::lexical_cast<std::string>(j);
        m_infoWindow.setTableItem(row,0,tr(heading.c_str()));
        m_infoWindow.setTableItem(row,1,tr(m_glWindow->voxelCountAtTreeDepth((treeDepth-1)-j).c_str()));
        row++;
    }

    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr(""));
    m_infoWindow.setTableItem(row,1,tr(""));
    row++;
    m_infoWindow.insertTableRow(row);
    m_infoWindow.setTableItem(row,0,tr("Raw Metadata"));
    m_infoWindow.setTableItem(row,1,tr(""));
    row++;

    int numMeta = m_glWindow->numMeta();

    for (int k = 0; k < numMeta; k++) // display all metadata retrieved from file
    {
        m_infoWindow.insertTableRow(row);
        m_infoWindow.setTableItem(row,0,tr(m_glWindow->metaNameAt(k).c_str()));
        m_infoWindow.setTableItem(row,1,tr(m_glWindow->metaValueAt(k).c_str()));
        row++;
    }
}

void MainWindow::setVectorLength()
{
    m_glWindow->setVectorLength(float(m_ui->hsld_vectorLength->value())/10.0f);
}

void MainWindow::setLOD()
{
    float LOD = float(m_ui->hsld_LOD->value())/10.0f; // divide by 10 to allow more control
    m_ui->sbx_LODValue->setValue(LOD);
    m_glWindow->setLOD(LOD);
    m_glWindow->updateGL();
}

void MainWindow::setPointSize()
{
    if (m_glWindow->glInitialised())
    {
        glPointSize(m_ui->sbx_pointSize->value());
        m_glWindow->updateGL();
    }
}

void MainWindow::setScanSpeed()
{
    m_glWindow->setScanSpeed(float(m_ui->hsld_scanSpeed->value())/100.0f); // divide by 100 to allow more control over a larger scale
    m_glWindow->updateGL();
}

void MainWindow::setLineWidth()
{
    if (m_glWindow->glInitialised())
    {
        glLineWidth(m_ui->sbx_lineWidth->value());
        m_glWindow->updateGL();
    }
}

void MainWindow::startSingleAxisScan()
{
    if (m_glWindow->vdbLoaded()) // only start if a vdb file has been loaded
    {
        // enable relavent ui
        m_ui->btn_PauseScan->setEnabled(true);
        m_ui->cbx_useCrop->setChecked(false);
        m_ui->cbx_useCrop->setEnabled(false);
        toggleCropSettings(false);
        // start scan
        m_glWindow->runScan(m_ui->cmbo_scanType->currentIndex());
    }
}

void MainWindow::stopScan()
{
    // stop scan, restore relavant data and enable ui
    m_ui->hsld_cropSlide->setValue(0);
    m_ui->hsld_cropSlide->setEnabled(false);
    m_ui->btn_PauseScan->setEnabled(false);
    m_ui->btn_PauseScan->setText("Pause");
    m_ui->cbx_useCrop->setEnabled(true);
    toggleScanControls(true);
    m_glWindow->setNumCropsToDraw(m_ui->cmbo_numCrops->currentIndex()+1);
    m_glWindow->stopScan();
}

void MainWindow::fillNumSlices()
{
    m_ui->cmbo_sliceSelection->clear(); // first empty the drop down

    for (int i = 0; i < m_ui->cmbo_numCrops->currentIndex()+1; i++)
    {
        std::string value = boost::lexical_cast<std::string>(i+1); // cast to string to set as text
        m_ui->cmbo_sliceSelection->addItem(QString::fromStdString(value));
    }

    m_glWindow->setNumCropsToDraw(m_ui->cmbo_numCrops->currentIndex()+1); // add one to the selected index as the amount to use
    m_glWindow->updateGL();
}

void MainWindow::pauseResumeScan()
{
    // enable and disable the appropriate controls
    toggleScanControls();
    if (m_ui->btn_PauseScan->text() == "Pause")
    {
        // if pause has been clicked, stop the scan where it is but do not reset any cop boxes
        m_ui->hsld_cropSlide->setValue(0);
        m_ui->hsld_cropSlide->setEnabled(true);
        m_ui->btn_PauseScan->setText("Resume");
        m_glWindow->pauseScan();
    }
    else
    {
        // continue the scan if resume pressed
        m_ui->hsld_cropSlide->setValue(0);
        m_ui->hsld_cropSlide->setEnabled(false);
        m_ui->btn_PauseScan->setText("Pause");
        m_glWindow->resumeScan();
    }
}

void MainWindow::useCrop()
{
    // set crop to be used
    toggleCropSettings();
    m_glWindow->toggleCrop();
}

void MainWindow::setVectorColour()
{
    m_glWindow->setVectorColour(float(m_ui->sbx_red->value()),float(m_ui->sbx_green->value()),float(m_ui->sbx_blue->value()));
    m_glWindow->updateGL();
}

void MainWindow::toggleMesh()
{
    switch(m_glWindow->toggleMesh())
    {
        // if the user decided against loading in the high res data
        case(0):
        {
            m_ui->cbx_showMesh->setChecked(false);
            m_ui->cbx_showVectors->setChecked(false);
            m_ui->cbx_showVectors->setEnabled(false);
            setChannelCombosEnabled(false);
        }break;
        case(1):
        {
            // user chose to load in high res
            if (m_glWindow->vdbLoaded())
            {
                // if loaded file then set enabled
                m_ui->cbx_showVectors->setEnabled(true);
                setPointChannelsEnabled(true);
            }
        }break;
    }
    m_glWindow->updateGL();
}

void MainWindow::enableColourRampUserControls()
{
    bool enabled = m_ui->rdio_userDefColourMap->isChecked();
    m_ui->dsbx_colourRampBottom->setEnabled(enabled);
    m_ui->dsbx_colourRampTop->setEnabled(enabled);
}

void MainWindow::setColourMapApplied()
{
    m_glWindow->setColourMapApplied(m_ui->cbx_applyColourMap->isChecked());
}

void MainWindow::toggleVectorColourEnabled()
{
    bool enabled = true;
    if (m_ui->cmbo_vectorColourType->currentIndex() == 0)
    {
        enabled = true;
        m_glWindow->setVectorColourMode(0);
    }
    else
    {
        enabled = false;
        m_glWindow->setVectorColourMode(1);
    }

    m_ui->sbx_red->setEnabled(enabled);
    m_ui->sbx_green->setEnabled(enabled);
    m_ui->sbx_blue->setEnabled(enabled);
    m_glWindow->updateGL();
}

void MainWindow::setVectorInvert()
{
    if (m_ui->cbx_invertVectorColours->isChecked())
    {
        m_glWindow->setVectorInvert(1);
    }
    else
    {
        m_glWindow->setVectorInvert(0);
    }
    m_glWindow->updateGL();
}

void MainWindow::setRenderChannelFromRenderTab()
{
    int index = m_ui->cmbo_renderChannel->currentIndex();
    setRenderChannel(index);
}

void MainWindow::setRenderChannel(int _index)
{
    m_glWindow->setPointChannel(_index);
    m_glWindow->updateGL();
}

void MainWindow::setVectorChannel()
{
    m_glWindow->setVectorChannel(m_ui->cmbo_channelSelect->currentIndex());
    m_glWindow->updateGL();
}

void MainWindow::setScanXWidth()
{
    m_glWindow->setScanXWidth(m_ui->dsbx_xScanWidth->value(),0);
}

void MainWindow::setScanYHeight()
{
    m_glWindow->setScanYHeight(m_ui->dsbx_yScanHeight->value(),0);
}

void MainWindow::setScanZDepth()
{
    m_glWindow->setScanZDepth(m_ui->dsbx_zScanDepth->value(),0);
}

void MainWindow::setCrop()
{
    if (!m_readCrop)
    {
        // if you are not just trying to read the crop values at that time, set them
        float width = m_ui->dsbx_cropWidth->value()/2.0f;
        float height = m_ui->dsbx_cropHeight->value()/2.0f;
        float depth = m_ui->dsbx_cropDepth->value()/2.0f;
        float x = m_ui->dsbx_cropX->value();
        float y = m_ui->dsbx_cropY->value();
        float z = m_ui->dsbx_cropZ->value();
        // standard calculation of box from centre using width height and depth
        m_glWindow->setCrop(x-width,x+width,y-height,y+height,z-depth,z+depth,m_ui->cmbo_sliceSelection->currentIndex());
        m_glWindow->updateGL();
    }
}

void MainWindow::toggleCropSettings()
{
    bool enabled = m_ui->cmbo_numCrops->isEnabled();
    enabled ^= true;

    m_ui->cmbo_numCrops->setEnabled(enabled);
    m_ui->cmbo_sliceSelection->setEnabled(enabled);
    m_ui->dsbx_cropX->setEnabled(enabled);
    m_ui->dsbx_cropY->setEnabled(enabled);
    m_ui->dsbx_cropZ->setEnabled(enabled);
    m_ui->dsbx_cropWidth->setEnabled(enabled);
    m_ui->dsbx_cropHeight->setEnabled(enabled);
    m_ui->dsbx_cropDepth->setEnabled(enabled);
}

void MainWindow::toggleCropSettings(bool _enabled)
{
    m_ui->cmbo_numCrops->setEnabled(_enabled);
    m_ui->cmbo_sliceSelection->setEnabled(_enabled);
    m_ui->dsbx_cropX->setEnabled(_enabled);
    m_ui->dsbx_cropY->setEnabled(_enabled);
    m_ui->dsbx_cropZ->setEnabled(_enabled);
    m_ui->dsbx_cropWidth->setEnabled(_enabled);
    m_ui->dsbx_cropHeight->setEnabled(_enabled);
    m_ui->dsbx_cropDepth->setEnabled(_enabled);
}

void MainWindow::toggleScanControls()
{
    bool enabled = m_ui->cmbo_scanType->isEnabled();
    enabled ^= true;

    m_ui->cmbo_scanType->setEnabled(enabled);
    m_ui->btn_startSingleScan->setEnabled(enabled);
}

void MainWindow::toggleScanControls(bool _enabled)
{
    m_ui->cmbo_scanType->setEnabled(_enabled);
    m_ui->btn_startSingleScan->setEnabled(_enabled);
}

void MainWindow::fillCropDetails()
{
    m_readCrop = true;
    // get the current index of the drop down box
    int index = m_ui->cmbo_sliceSelection->currentIndex();
    BoundBox temp;
    // return the cop box at this index
    temp = m_glWindow->getCBox(index);

    // set the input value boxes to these values
    m_ui->dsbx_cropWidth->setValue(temp.width());
    m_ui->dsbx_cropHeight->setValue(temp.height());
    m_ui->dsbx_cropDepth->setValue(temp.depth());

    m_ui->dsbx_cropX->setValue(temp.centre().x());
    m_ui->dsbx_cropY->setValue(temp.centre().y());
    m_ui->dsbx_cropZ->setValue(temp.centre().z());
    m_readCrop = false;
}

void MainWindow::updateUsedGPUMem()
{
    m_infoWindow.setUsedGPUMem(m_glWindow->currentUsedGPUMemMB());
}

void MainWindow::openFile()
{
    // open a file dialouge looking for vdb files
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open VDB File"), tr(""),
                                              tr("VDB Files (*.vdb)"));

    if (!fileName.isEmpty())
    {
        // clear any old file
        clear();
        std::string file = fileName.toStdString();
        // load
        m_glWindow->loadFile(file);
        m_ui->FileName->setText(m_glWindow->fileName().c_str());
        setStartup();
        m_ui->cbx_useCrop->setEnabled(true);

        m_ui->cbx_showMesh->setEnabled(true);

        // clear and refill the drop down boxes containing the files channels
        clearChannelCombos();

        fillChannelCombos();

        if (m_glWindow->extLoaded())
        {
            // if the high res data was loaded
            m_ui->cbx_showVectors->setEnabled(true);
            setPointChannelsEnabled(true);
        }

        getFileInformation();
    }
}

void MainWindow::clear()
{
    // clear all ui components and data from the file
    m_ui->cbx_showVectors->setChecked(false);
    m_ui->cbx_showVectors->setEnabled(false);
    m_ui->cbx_showMesh->setChecked(false);
    m_ui->cbx_showMesh->setEnabled(false);

    m_ui->cmbo_applyCullToChannelSelect->clear();

    m_ui->rdio_applyToFile->setChecked(true);
    m_ui->rdio_applyToCulledPoints->setEnabled(false);

    m_ui->cbx_useCulling1->setChecked(false);
    m_ui->cbx_useCulling2->setChecked(false);
    m_ui->cbx_useCulling3->setChecked(false);

    m_glWindow->setCullingEnabled(0);

    setChannelCombosEnabled(false);

    m_glWindow->resetCameraNoVolume(false);
    m_glWindow->clear();
    clearInfoTable();
    m_ui->cbx_useCrop->setEnabled(false);
    clearChannelCombos();

    if (m_glWindow->scanRunning())
    {
        stopScan();
    }
    m_ui->FileName->setText("Use File->Open to open a VDB file");
}

void MainWindow::quitApp()
{
    exit(0);
}

void MainWindow::keyPressEvent(QKeyEvent *_event)
{
  switch (_event->key())
  {
      case Qt::Key_Alt :
      {
          // if the alt button is head, switch the move mdoe so that you can move in the world by holding alt and using the mouse
          if (!_event->isAutoRepeat())
          {
              m_glWindow->moveAroundOrigin(true);
          }
      }break;
      default : break;
  }
  m_glWindow->updateGL();
}

void MainWindow::removeMesh()
{
    // remove only the high resolution mesh
    // disable UI components
    m_glWindow->removeMesh();
    m_ui->cbx_showVectors->setChecked(false);
    m_ui->cbx_showVectors->setEnabled(false);
    m_glWindow->showVectors(false);
    m_ui->cbx_showMesh->setChecked(false);
    setChannelCombosEnabled(false);
    setPointChannelsEnabled(false);
}

void MainWindow::setRampPoints()
{
    if (m_ui->rdio_pointRamp->isChecked())
    {
        m_glWindow->setRampPoints(1);
    }
    else
    {
        m_glWindow->setRampPoints(0);
    }
    m_glWindow->updateGL();
}

void MainWindow::setRampColour()
{
    openvdb::Vec3f colour = openvdb::Vec3f(float(m_ui->sbx_colourRampRed->value())/255.0f,
                             float(m_ui->sbx_colourRampGreen->value())/255.0f,
                             float(m_ui->sbx_colourRampBlue->value())/255.0f);
    m_glWindow->setRampColour(colour);
    m_glWindow->updateGL();
}

void MainWindow::setUserRamp()
{
    if (m_ui->rdio_userDefColourMap->isChecked())
    {
        m_glWindow->setUserDefinedRamp(true);
        setRampMin(false);
        setRampMax();
    }
    else
    {
        m_glWindow->setUserDefinedRamp(false);
    }
    enableColourRampUserControls();
    m_glWindow->updateGL();
}

void MainWindow::setRampMin(bool _updateGL)
{
    m_glWindow->setRampMin(m_ui->dsbx_colourRampBottom->value());
    if (_updateGL)
    {
        m_glWindow->updateGL();
    }
}

void MainWindow::setRampMax(bool _updateGL)
{
    m_glWindow->setRampMax(m_ui->dsbx_colourRampTop->value());
    if (_updateGL)
    {
        m_glWindow->updateGL();
    }
}

void MainWindow::resetCropSlider()
{
    // reset the slider back to 0 when the mouse is released
    m_ui->hsld_cropSlide->setValue(0);
    stopCropSlideTimer();
}

void MainWindow::toggleChannelVectors()
{
    m_ui->cmbo_channelSelect->setEnabled(m_ui->cbx_showVectors->isChecked());
    m_glWindow->toggleVectors();
}

void MainWindow::startCropSlideTimer()
{
    // start the timer for the manual crop move
    m_cropSlideTimer = startTimer(30);
}

void MainWindow::updateCropSlideValue()
{
    float delta = float(m_ui->hsld_cropSlide->value())/500.0f; // divide by 500 for more control over its position
    m_glWindow->manualCropMove(delta);
    m_glWindow->updateGL();
}

// separated the following into three functions otherwise suffer from seg faults whenever one is set - easier than boolean checks

void MainWindow::setCull1Type()
{
    // read the type of the channel selected and display in label
    m_ui->lbl_cull1Type->setText(QString::fromStdString(m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_1->currentIndex())));
    m_glWindow->setCullChannelOffset(m_ui->cmbo_currentPointChannelCulling_1->currentIndex(),0);
    // enable the appropriate range boxes for this cull
    enableCull1RangeBoxes(m_ui->cmbo_currentPointChannelCulling_1->isEnabled());
    if (m_glWindow->vdbLoaded())
    {
        // set the max and min for the value boxes
        setCull1Extremes();
    }
    setCull1Values();
    m_glWindow->updateGL();
}

void MainWindow::setCull2Type()
{
    // read the type of the channel selected and display in label
    m_ui->lbl_cull2Type->setText(QString::fromStdString(m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_2->currentIndex())));
    m_glWindow->setCullChannelOffset(m_ui->cmbo_currentPointChannelCulling_2->currentIndex(),1);
    // enable the appropriate range boxes for this cull
    enableCull2RangeBoxes(m_ui->cmbo_currentPointChannelCulling_2->isEnabled());
    if (m_glWindow->vdbLoaded())
    {
        // set the max and min for the value boxes
        setCull2Extremes();
    }
    setCull2Values();
    m_glWindow->updateGL();
}

void MainWindow::setCull3Type()
{
    // read the type of the channel selected and display in label
    m_ui->lbl_cull3Type->setText(QString::fromStdString(m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_3->currentIndex())));
    m_glWindow->setCullChannelOffset(m_ui->cmbo_currentPointChannelCulling_3->currentIndex(),2);
    // enable the appropriate range boxes for this cull
    enableCull3RangeBoxes(m_ui->cmbo_currentPointChannelCulling_3->isEnabled());
    if (m_glWindow->vdbLoaded())
    {
        // set the max and min for the value boxes
        setCull3Extremes();
    }
    setCull3Values();
    m_glWindow->updateGL();
}

void MainWindow::setCull1Boolean()
{
    // set if cull is less than, between or greater than
    m_glWindow->setCullType(m_ui->cmbo_cullingChoice1->currentIndex(),0);
    enableCull1RangeBoxes(m_ui->cmbo_currentPointChannelCulling_1->isEnabled());
    m_glWindow->updateGL();
}

void MainWindow::setCull2Boolean()
{
    // set if cull is less than, between or greater than
    m_glWindow->setCullType(m_ui->cmbo_cullingChoice2->currentIndex(),1);
    enableCull2RangeBoxes(m_ui->cmbo_currentPointChannelCulling_2->isEnabled());
    m_glWindow->updateGL();
}

void MainWindow::setCull3Boolean()
{
    // set if cull is less than, between or greater than
    m_glWindow->setCullType(m_ui->cmbo_cullingChoice3->currentIndex(),2);
    enableCull3RangeBoxes(m_ui->cmbo_currentPointChannelCulling_3->isEnabled());
    m_glWindow->updateGL();
}

void MainWindow::enableCull1RangeBoxes(bool _enabled)
{
    // clear the group box titles
    m_ui->gbx_cull1From->setTitle(tr(""));
    m_ui->gbx_cull1To->setTitle(tr(""));
    // first set them all to disabled to ensure the correct ones are disabled
    m_ui->dsbx_cull1FromX->setEnabled(false);
    m_ui->dsbx_cull1FromY->setEnabled(false);
    m_ui->dsbx_cull1FromZ->setEnabled(false);
    m_ui->dsbx_cull1ToX->setEnabled(false);
    m_ui->dsbx_cull1ToY->setEnabled(false);
    m_ui->dsbx_cull1ToZ->setEnabled(false);

    if(_enabled)
    {
        if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_1->currentIndex()) == "float")
        {
            m_glWindow->setCullChannelType(0,0);
            if (m_ui->cmbo_cullingChoice1->currentIndex() == 1)
            {
                // set the title to represent the use of floats
                m_ui->gbx_cull1To->setTitle(tr("Top of Cull Range - Float"));
                m_ui->gbx_cull1From->setTitle(tr("Bottom of Cull Range - Float"));
                m_ui->dsbx_cull1ToX->setEnabled(true);
            }
            else
            {
                // set the title to represent the use of floats
                m_ui->gbx_cull1From->setTitle(tr("Cull Value - Float"));
            }

            m_ui->dsbx_cull1FromX->setEnabled(true);
        }
        else if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_1->currentIndex()) == "vec3s")
        {
            m_glWindow->setCullChannelType(1,0);
            if (m_ui->cmbo_cullingChoice1->currentIndex() == 1)
            {
                // set the title to represent the use of vectors
                m_ui->gbx_cull1To->setTitle(tr("Top of Cull Range - Vector [x,y,z]"));
                m_ui->gbx_cull1From->setTitle(tr("Bottom of Cull Range - Vector [x,y,z]"));
                m_ui->dsbx_cull1ToX->setEnabled(true);
                m_ui->dsbx_cull1ToY->setEnabled(true);
                m_ui->dsbx_cull1ToZ->setEnabled(true);
            }
            else
            {
                // set the title to represent the use of vectors
                m_ui->gbx_cull1From->setTitle(tr("Cull Value - Vector [x,y,z]"));
            }

            m_ui->dsbx_cull1FromX->setEnabled(true);
            m_ui->dsbx_cull1FromY->setEnabled(true);
            m_ui->dsbx_cull1FromZ->setEnabled(true);
        }
    }
}

void MainWindow::enableCull2RangeBoxes(bool _enabled)
{
    // clear the group box titles
    m_ui->gbx_cull2From->setTitle(tr(""));
    m_ui->gbx_cull2To->setTitle(tr(""));
    // first set them all to disabled to ensure the correct ones are disabled
    m_ui->dsbx_cull2FromX->setEnabled(false);
    m_ui->dsbx_cull2FromY->setEnabled(false);
    m_ui->dsbx_cull2FromZ->setEnabled(false);
    m_ui->dsbx_cull2ToX->setEnabled(false);
    m_ui->dsbx_cull2ToY->setEnabled(false);
    m_ui->dsbx_cull2ToZ->setEnabled(false);

    if(_enabled)
    {
        if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_2->currentIndex()) == "float")
        {
            m_glWindow->setCullChannelType(0,1);
            if (m_ui->cmbo_cullingChoice2->currentIndex() == 1)
            {
                // set the title to represent the use of floats
                m_ui->gbx_cull2To->setTitle(tr("Top of Cull Range - Float"));
                m_ui->gbx_cull2From->setTitle(tr("Bottom of Cull Range - Float"));
                m_ui->dsbx_cull2ToX->setEnabled(true);
            }
            else
            {
                // set the title to represent the use of floats
                m_ui->gbx_cull2From->setTitle(tr("Cull Value - Float"));
            }

            m_ui->dsbx_cull2FromX->setEnabled(true);
        }
        else if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_2->currentIndex()) == "vec3s")
        {
            m_glWindow->setCullChannelType(1,1);
            if (m_ui->cmbo_cullingChoice2->currentIndex() == 1)
            {
                // set the title to represent the use of vectors
                m_ui->gbx_cull2To->setTitle(tr("Top of Cull Range - Vector [x,y,z]"));
                m_ui->gbx_cull2From->setTitle(tr("Bottom of Cull Range - Vector [x,y,z]"));
                m_ui->dsbx_cull2ToX->setEnabled(true);
                m_ui->dsbx_cull2ToY->setEnabled(true);
                m_ui->dsbx_cull2ToZ->setEnabled(true);
            }
            else
            {
                // set the title to represent the use of vectors
                m_ui->gbx_cull2From->setTitle(tr("Cull Value - Vector [x,y,z]"));
            }

            m_ui->dsbx_cull2FromX->setEnabled(true);
            m_ui->dsbx_cull2FromY->setEnabled(true);
            m_ui->dsbx_cull2FromZ->setEnabled(true);
        }
    }
}

void MainWindow::enableCull3RangeBoxes(bool _enabled)
{
    // clear the group box titles
    m_ui->gbx_cull3From->setTitle(tr(""));
    m_ui->gbx_cull3To->setTitle(tr(""));
    // first set them all to disabled to ensure the correct ones are disabled
    m_ui->dsbx_cull3FromX->setEnabled(false);
    m_ui->dsbx_cull3FromY->setEnabled(false);
    m_ui->dsbx_cull3FromZ->setEnabled(false);
    m_ui->dsbx_cull3ToX->setEnabled(false);
    m_ui->dsbx_cull3ToY->setEnabled(false);
    m_ui->dsbx_cull3ToZ->setEnabled(false);

    if(_enabled)
    {
        if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_3->currentIndex()) == "float")
        {
            m_glWindow->setCullChannelType(0,2);
            if (m_ui->cmbo_cullingChoice3->currentIndex() == 1)
            {
                // set the title to represent the use of floats
                m_ui->gbx_cull3To->setTitle(tr("Top of Cull Range - Float"));
                m_ui->gbx_cull3From->setTitle(tr("Bottom of Cull Range - Float"));
                m_ui->dsbx_cull3ToX->setEnabled(true);
            }
            else
            {
                // set the title to represent the use of floats
                m_ui->gbx_cull3From->setTitle(tr("Cull Value - Float"));
            }

            m_ui->dsbx_cull3FromX->setEnabled(true);
        }
        else if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_3->currentIndex()) == "vec3s")
        {
            m_glWindow->setCullChannelType(1,2);
            if (m_ui->cmbo_cullingChoice3->currentIndex() == 1)
            {
                // set the title to represent the use of vectors
                m_ui->gbx_cull3To->setTitle(tr("Top of Cull Range - Vector [x,y,z]"));
                m_ui->gbx_cull3From->setTitle(tr("Bottom of Cull Range - Vector [x,y,z]"));
                m_ui->dsbx_cull3ToX->setEnabled(true);
                m_ui->dsbx_cull3ToY->setEnabled(true);
                m_ui->dsbx_cull3ToZ->setEnabled(true);
            }
            else
            {
                // set the title to represent the use of vectors
                m_ui->gbx_cull3From->setTitle(tr("Cull Value - Vector [x,y,z]"));
            }

            m_ui->dsbx_cull3FromX->setEnabled(true);
            m_ui->dsbx_cull3FromY->setEnabled(true);
            m_ui->dsbx_cull3FromZ->setEnabled(true);
        }
    }
}

void MainWindow::stopCropSlideTimer()
{
    // kill the timer for the manual crop box move
    killTimer(m_cropSlideTimer);
}

void MainWindow::keyReleaseEvent(QKeyEvent *_event)
{
    switch(_event->key())
    {
        case Qt::Key_Alt:
        {
            if (!_event->isAutoRepeat())
            {
                m_glWindow->moveAroundOrigin(false);
            }
        }
    }
    m_glWindow->updateGL();
}

void MainWindow::closeEvent(QCloseEvent *)
{
    if (m_infoWindow.isVisible())
    {
        m_infoWindow.close();
    }
    if (m_aboutWindow.isVisible())
    {
        m_aboutWindow.close();
    }
    exit(0);
}

void MainWindow::clearChannelCombos()
{
    // clear all drop down boxes dealing with channel data
    m_ui->cmbo_renderChannel->clear();
    m_ui->cmbo_channelSelect->clear();
    m_ui->cmbo_currentPointChannelCulling_1->clear();
    m_ui->cmbo_currentPointChannelCulling_2->clear();
    m_ui->cmbo_currentPointChannelCulling_3->clear();

    // clear all channel type lbels within the cull tab
    m_ui->lbl_cull1Type->clear();
    m_ui->lbl_cull2Type->clear();
    m_ui->lbl_cull3Type->clear();
}

void MainWindow::fillChannelCombos()
{
    int numChannels = m_glWindow->numGrids();
    for (int i = 0; i < numChannels; i++)
    {
        // retrieve the channel names for the loaded file and load them into all the drop down boxes that can select channels
        std::string value = m_glWindow->gridName(i);
        m_ui->cmbo_channelSelect->addItem(QString::fromStdString(value));
        m_ui->cmbo_renderChannel->addItem(QString::fromStdString(value));
        m_ui->cmbo_currentPointChannelCulling_1->addItem(QString::fromStdString(value));
        m_ui->cmbo_currentPointChannelCulling_2->addItem(QString::fromStdString(value));
        m_ui->cmbo_currentPointChannelCulling_3->addItem(QString::fromStdString(value));
    }

    if (numChannels >= 0)
    {
        // if there is at least one channel, set the current index of all to 0
        m_ui->cmbo_currentPointChannelCulling_1->setCurrentIndex(0);
        m_ui->cmbo_currentPointChannelCulling_2->setCurrentIndex(0);
        m_ui->cmbo_currentPointChannelCulling_3->setCurrentIndex(0);
    }
}

void MainWindow::setChannelCombosEnabled(bool _enabled)
{
    m_ui->cmbo_channelSelect->setEnabled(_enabled);
}

void MainWindow::setPointChannelsEnabled(bool _enabled)
{
    m_ui->cmbo_renderChannel->setEnabled(_enabled);

    m_ui->cmbo_currentPointChannelCulling_1->setEnabled(_enabled);
    m_ui->cmbo_cullingChoice1->setEnabled(_enabled);
    enableCull1RangeBoxes(_enabled);
    m_ui->cbx_useCulling1->setEnabled(_enabled);

    m_ui->cmbo_currentPointChannelCulling_2->setEnabled(_enabled);
    m_ui->cmbo_cullingChoice2->setEnabled(_enabled);
    enableCull2RangeBoxes(_enabled);
    m_ui->cbx_useCulling2->setEnabled(_enabled);

    m_ui->cmbo_currentPointChannelCulling_3->setEnabled(_enabled);
    m_ui->cmbo_cullingChoice3->setEnabled(_enabled);
    enableCull3RangeBoxes(_enabled);
    m_ui->cbx_useCulling3->setEnabled(_enabled);
}

void MainWindow::setCull1Extremes()
{
    // set the max and min values of the value selection boxes to the max and min of the extremes of the chosen channel
    BBoxBare extremes = m_glWindow->getChannelExtremes(m_ui->cmbo_currentPointChannelCulling_1->currentIndex());
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull1FromX,extremes.minx,extremes.maxx);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull1ToX,extremes.minx,extremes.maxx);

    setCullSpinBoxesMaxMin(m_ui->dsbx_cull1FromY,extremes.miny,extremes.maxy);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull1ToY,extremes.miny,extremes.maxy);

    setCullSpinBoxesMaxMin(m_ui->dsbx_cull1FromZ,extremes.minz,extremes.maxz);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull1ToZ,extremes.minz,extremes.maxz);
}

void MainWindow::setCull2Extremes()
{
    // set the max and min values of the value selection boxes to the max and min of the extremes of the chosen channel
    BBoxBare extremes = m_glWindow->getChannelExtremes(m_ui->cmbo_currentPointChannelCulling_2->currentIndex());
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull2FromX,extremes.minx,extremes.maxx);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull2ToX,extremes.minx,extremes.maxx);

    setCullSpinBoxesMaxMin(m_ui->dsbx_cull2FromY,extremes.miny,extremes.maxy);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull2ToY,extremes.miny,extremes.maxy);

    setCullSpinBoxesMaxMin(m_ui->dsbx_cull2FromZ,extremes.minz,extremes.maxz);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull2ToZ,extremes.minz,extremes.maxz);
}

void MainWindow::setCull3Extremes()
{
    // set the max and min values of the value selection boxes to the max and min of the extremes of the chosen channel
    BBoxBare extremes = m_glWindow->getChannelExtremes(m_ui->cmbo_currentPointChannelCulling_3->currentIndex());
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull3FromX,extremes.minx,extremes.maxx);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull3ToX,extremes.minx,extremes.maxx);

    setCullSpinBoxesMaxMin(m_ui->dsbx_cull3FromY,extremes.miny,extremes.maxy);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull3ToY,extremes.miny,extremes.maxy);

    setCullSpinBoxesMaxMin(m_ui->dsbx_cull3FromZ,extremes.minz,extremes.maxz);
    setCullSpinBoxesMaxMin(m_ui->dsbx_cull3ToZ,extremes.minz,extremes.maxz);
}

void MainWindow::setCullSpinBoxesMaxMin(QDoubleSpinBox *_box, float _min, float _max)
{
    _box->setMinimum(_min);
    _box->setMaximum(_max);
}

void MainWindow::timerEvent(QTimerEvent *)
{
    updateCropSlideValue();
    m_glWindow->updateGL();
}

void MainWindow::setCullingEnabled()
{
    // first default eveything to false to ensure when a culling is urned off, it is reflected in the drawing
    m_renderNewCullColour = false;
    m_glWindow->setNumActiveCullings(0);
    m_glWindow->setCullingEnabled(0);
    m_ui->cmbo_applyCullToChannelSelect->clear();

    // set all culls to unactive
    for (int i = 0; i < 3; i++)
    {
        m_glWindow->setCullActive(false,i);
    }

    // if at least one cull is checked, set to active
    if (m_ui->cbx_useCulling1->isChecked() || m_ui->cbx_useCulling2->isChecked() || m_ui->cbx_useCulling3->isChecked())
    {
        m_glWindow->setCullingEnabled(1);
        m_ui->rdio_applyToCulledPoints->setEnabled(true);

        int numActive = 0; // count how many are active

        if (m_ui->cbx_useCulling1->isChecked())
        { // if checked, set to active and set values
            m_glWindow->setCullActive(true,0);
            m_cullToComboMap[numActive] = 0;
            setCull1Extremes();
            numActive++;
            m_ui->cmbo_applyCullToChannelSelect->addItem(tr("Cull 1"));
        }
        if (m_ui->cbx_useCulling2->isChecked())
        { // if checked, set to active and set values
            m_glWindow->setCullActive(true,1);
            m_cullToComboMap[numActive] = 1;
            setCull2Extremes();
            numActive++;
            m_ui->cmbo_applyCullToChannelSelect->addItem(tr("Cull 2"));
        }
        if (m_ui->cbx_useCulling3->isChecked())
        { // if checked, set to active and set values
            m_glWindow->setCullActive(true,2);
            m_cullToComboMap[numActive] = 2;
            setCull3Extremes();
            numActive++;
            m_ui->cmbo_applyCullToChannelSelect->addItem(tr("Cull 3"));
        }
        // number of active cullings
        m_glWindow->setNumActiveCullings(numActive);
    }
    else
    {
        m_ui->rdio_applyToFile->setChecked(true);
        m_ui->rdio_applyToCulledPoints->setEnabled(false);
    }
    m_renderNewCullColour = true;

    m_glWindow->updateGL();
}

void MainWindow::setCull1Values()
{
    // switch on the type of the channel being used for this cull
    if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_1->currentIndex()) == "float")
    {
        // if scalar type
        if (m_ui->cmbo_cullingChoice1->currentIndex() == 1) // if between cull type
        {
            m_glWindow->setCullULimit(m_ui->dsbx_cull1ToX->value(),0);
        }
        m_glWindow->setCullLLimit(m_ui->dsbx_cull1FromX->value(),0); // either less than or greater than
    }
    else if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_1->currentIndex()) == "vec3s") // vector type
    {
        if (m_ui->cmbo_cullingChoice1->currentIndex() == 1) // if between cull type
        {
            openvdb::Vec3s upper(m_ui->dsbx_cull1ToX->value(),m_ui->dsbx_cull1ToY->value(),m_ui->dsbx_cull1ToZ->value());
            m_glWindow->setCullULimit(upper,0);
        }
        // either less than or greater than
        openvdb::Vec3s lower(m_ui->dsbx_cull1FromX->value(),m_ui->dsbx_cull1FromY->value(),m_ui->dsbx_cull1FromZ->value());
        m_glWindow->setCullLLimit(lower,0);
    }
    m_glWindow->updateGL();
}

void MainWindow::setCull2Values()
{
    // switch on the type of the channel being used for this cull
    if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_2->currentIndex()) == "float")
    {
        // if scalar type
        if (m_ui->cmbo_cullingChoice2->currentIndex() == 1) // if between cull type
        {
            m_glWindow->setCullULimit(m_ui->dsbx_cull2ToX->value(),1);
        }
        // either less than or greater than
        m_glWindow->setCullLLimit(m_ui->dsbx_cull2FromX->value(),1);
    }
    else if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_2->currentIndex()) == "vec3s") // vector type
    {
        if (m_ui->cmbo_cullingChoice2->currentIndex() == 1) // if between cull type
        {
            openvdb::Vec3s upper(m_ui->dsbx_cull2ToX->value(),m_ui->dsbx_cull2ToY->value(),m_ui->dsbx_cull2ToZ->value());
            m_glWindow->setCullULimit(upper,1);
        }
        // either less than or greater than
        openvdb::Vec3s lower(m_ui->dsbx_cull2FromX->value(),m_ui->dsbx_cull2FromY->value(),m_ui->dsbx_cull2FromZ->value());
        m_glWindow->setCullLLimit(lower,1);
    }
    m_glWindow->updateGL();
}

void MainWindow::setCull3Values()
{
     // switch on the type of the channel being used for this cull
    if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_3->currentIndex()) == "float")
    {
        // if scalar type
        if (m_ui->cmbo_cullingChoice3->currentIndex() == 1) // if between cull type
        {
            m_glWindow->setCullULimit(m_ui->dsbx_cull3ToX->value(),2);
        }
        m_glWindow->setCullLLimit(m_ui->dsbx_cull3FromX->value(),2); // either less than or greater than
    }
    else if (m_glWindow->gridType(m_ui->cmbo_currentPointChannelCulling_3->currentIndex()) == "vec3s") // vector type
    {
        if (m_ui->cmbo_cullingChoice3->currentIndex() == 1) // if between cull type
        {
            openvdb::Vec3s upper(m_ui->dsbx_cull3ToX->value(),m_ui->dsbx_cull3ToY->value(),m_ui->dsbx_cull3ToZ->value());
            m_glWindow->setCullULimit(upper,2);
        }
        // either less than or greater than
        openvdb::Vec3s lower(m_ui->dsbx_cull3FromX->value(),m_ui->dsbx_cull3FromY->value(),m_ui->dsbx_cull3FromZ->value());
        m_glWindow->setCullLLimit(lower,2);
    }
    m_glWindow->updateGL();
}

void MainWindow::setCullToChannel()
{
    if (m_ui->rdio_applyToFile->isChecked())
    {
        m_ui->rdio_pointRamp->setEnabled(true);
        m_glWindow->setColourMapOnCull(false);
        m_ui->cmbo_applyCullToChannelSelect->setEnabled(false);
    }
    else
    {
        m_ui->rdio_channelRamp->setChecked(true);
        m_ui->rdio_pointRamp->setEnabled(false);
        m_glWindow->setColourMapOnCull(true);
        m_ui->cmbo_applyCullToChannelSelect->setEnabled(true);
        setCullToChannelNumber();
    }
    m_glWindow->updateGL();
}

void MainWindow::setCullToChannelNumber()
{
    m_glWindow->setColourMapToCullNumber(m_cullToComboMap[m_ui->cmbo_applyCullToChannelSelect->currentIndex()]);
    if (m_renderNewCullColour)
    {
        m_glWindow->updateGL();
    }
}

void MainWindow::showInformation()
{
    if (m_infoWindow.isVisible())
    {
        m_infoWindow.raise();
    }
    else
    {
        m_infoWindow.show();
    }
}

void MainWindow::openAbout()
{
    if (m_aboutWindow.isVisible())
    {
        m_aboutWindow.raise();
    }
    else
    {
        m_aboutWindow.show();
    }
}

void MainWindow::clearInfoTable()
{
    m_infoWindow.clearInfoTable();
}
