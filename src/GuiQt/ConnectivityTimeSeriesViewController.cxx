/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <cmath>
#include <limits>

#define __CONNECTIVITY_TIME_SERIES_VIEW_CONTROLLER_DECLARE__
#include "ConnectivityTimeSeriesViewController.h"
#undef __CONNECTIVITY_TIME_SERIES_VIEW_CONTROLLER_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QSpinBox>

#include "Brain.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretPreferences.h"
#include "ChartableInterface.h"
//#include "ConnectivityLoaderFile.h"
#include "ElapsedTimer.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUpdateAnimationStartTime.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SessionManager.h"
#include "TimeCourseDialog.h"
#include "TimeSeriesManagerForViewController.h"
#include "WuQFactory.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ConnectivityTimeSeriesViewController 
 * \brief View-Controller for one connectivity loader
 * \ingroup GuiQt
 */
/**
 * Constructor.
 */
ConnectivityTimeSeriesViewController::ConnectivityTimeSeriesViewController(const Qt::Orientation orientation,
                                                                   QGridLayout* gridLayout,
                                                                   QObject* parent)
: QObject(parent)
{
    this->connectivityLoaderFile = NULL;
    this->previousConnectivityLoaderFile = NULL;
    
    
    
    this->fileNameLineEdit = new QLineEdit("                 ");
    this->fileNameLineEdit->setReadOnly(true);
    
    
    QIcon graphIcon;
    const bool graphIconValid = WuQtUtilities::loadIcon(":/time_series_graph.png",
                                                           graphIcon);
    this->graphDisplayAction = WuQtUtilities::createAction("Graph",
                                                           "Display time-series graph for selected brainordinate", 
                                                           this, 
                                                           this, 
                                                           SLOT(graphDisplayActionTriggered(bool)));
    if (graphIconValid) {
        this->graphDisplayAction->setIcon(graphIcon);
    }
    this->graphDisplayAction->setCheckable(true);
    this->graphToolButton = new QToolButton();
    this->graphToolButton->setDefaultAction(this->graphDisplayAction); 

    
    
    this->gridLayoutGroup = new WuQGridLayoutGroup(gridLayout,
                                                   this);
    if (orientation == Qt::Horizontal) {
        int row = this->gridLayoutGroup->rowCount();        
        this->gridLayoutGroup->addWidget(this->graphToolButton, row, 0);
        this->gridLayoutGroup->addWidget(this->fileNameLineEdit, row, 1);
    }
    else {
        QFrame* bottomHorizontalLineWidget = new QFrame();
        bottomHorizontalLineWidget->setLineWidth(0);
        bottomHorizontalLineWidget->setMidLineWidth(1);
        bottomHorizontalLineWidget->setFrameStyle(QFrame::HLine | QFrame::Raised);
        
        int row = this->gridLayoutGroup->rowCount();
        this->gridLayoutGroup->addWidget(this->graphToolButton, row, 0);
        this->gridLayoutGroup->addWidget(this->fileNameLineEdit, row, 1);        
        row++;
        this->gridLayoutGroup->addWidget(bottomHorizontalLineWidget, row, 0, 1, -1);
    }    
    
    allConnectivityTimeSeriesViewControllers.insert(this);
    
    
}

/**
 * Destructor.
 */
ConnectivityTimeSeriesViewController::~ConnectivityTimeSeriesViewController()
{   
    allConnectivityTimeSeriesViewControllers.erase(this);    
}

/**
 * @return The connectivity loader file in this view controller.
 * NULL if not valid.
 */
ChartableInterface* 
ConnectivityTimeSeriesViewController::getConnectivityLoaderFile()
{
    return this->connectivityLoaderFile;
}

/**
 * Create the grid layout for this view controller using the given orientation.
 * @param orientation
 *    Orientation in toolbox.
 * @return
 *    GridLayout setup for this view controller.
 */
QGridLayout* 
ConnectivityTimeSeriesViewController::createGridLayout(const Qt::Orientation orientation)
{
    QGridLayout* gridLayout = new QGridLayout();
    WuQtUtilities::setLayoutMargins(gridLayout, 2, 2);
    if (orientation == Qt::Horizontal) {

        gridLayout->setColumnStretch(0, 0);
        gridLayout->setColumnStretch(1, 100);        
        
        QLabel* graphLabel = new QLabel("Graph");
        QLabel* fileLabel = new QLabel("File");
        
        const int row = gridLayout->rowCount();        
        gridLayout->addWidget(graphLabel, row, 0, Qt::AlignHCenter);        
        gridLayout->addWidget(fileLabel, row, 1, Qt::AlignHCenter);
    }
    else {
        gridLayout->setColumnStretch(0, 0);        
        gridLayout->setColumnStretch(1, 100);
    }
    return gridLayout;
}

/**
 * Update this view controller.
 * @param connectivityLoaderFile
 *    Connectivity loader file in this view controller.
 */
void 
ConnectivityTimeSeriesViewController::updateViewController(ChartableInterface* connectivityLoaderFile)
{
    this->connectivityLoaderFile = connectivityLoaderFile;
    if (this->connectivityLoaderFile != NULL) {        

        Qt::CheckState enabledState = Qt::Unchecked;
        
        CaretMappableDataFile* mappableDataFile = dynamic_cast<CaretMappableDataFile*>(this->connectivityLoaderFile);
        CaretAssert(mappableDataFile);
        this->fileNameLineEdit->setText(mappableDataFile->getFileName());

        enabledState = Qt::Unchecked;
        if(this->connectivityLoaderFile->isChartingEnabled()) {
            enabledState = Qt::Checked;
        }
        this->graphToolButton->blockSignals(true);
        this->graphToolButton->setChecked(enabledState);
        this->graphToolButton->blockSignals(false);       
    }
     
    this->previousConnectivityLoaderFile = this->connectivityLoaderFile;
}

/**
 * Update the view controller.
 */
void 
ConnectivityTimeSeriesViewController::updateViewController()
{
    this->updateViewController(this->connectivityLoaderFile);    
}



/**
 * Set the visiblity of this overlay view controller.
 */
void 
ConnectivityTimeSeriesViewController::setVisible(bool visible)
{
    this->gridLayoutGroup->setVisible(visible);
    
//    this->updateUserInterfaceAndGraphicsWindow();
}

/**
 * Called when graph display tool button is triggered.
 * @param status
 *    New status.
 */
void 
ConnectivityTimeSeriesViewController::graphDisplayActionTriggered(bool status)
{
    if (this->connectivityLoaderFile != NULL) {
            this->connectivityLoaderFile->setChartingEnabled(status);
            GuiManager::get()->getTimeCourseDialog((void *)this->connectivityLoaderFile)->setTimeSeriesGraphEnabled(status);
            this->updateOtherConnectivityTimeSeriesViewControllers();
        }
}

/**
 * Update other connectivity view controllers other than 'this' instance
 * that contain the same connectivity file.
 */
void 
ConnectivityTimeSeriesViewController::updateOtherConnectivityTimeSeriesViewControllers()
{
    if (this->connectivityLoaderFile != NULL) {
        for (std::set<ConnectivityTimeSeriesViewController*>::iterator iter = allConnectivityTimeSeriesViewControllers.begin();
             iter != allConnectivityTimeSeriesViewControllers.end();
             iter++) {
            ConnectivityTimeSeriesViewController* clvc = *iter;
            if (clvc != this) {
                clvc->updateViewController();
            }
        }
    }
}















