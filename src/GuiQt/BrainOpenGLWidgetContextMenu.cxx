
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

#include <QActionGroup>

#define __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__
#include "BrainOpenGLWidgetContextMenu.h"
#undef __BRAIN_OPEN_G_L_WIDGET_CONTEXT_MENU_DECLARE__

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "ChartableInterface.h"
#include "ChartingDataManager.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiConnectivityMatrixDataFileManager.h"
#include "CiftiFiberTrajectoryManager.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CursorDisplayScoped.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventUpdateTimeCourseDialog.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "FociPropertiesEditorDialog.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GuiManager.h"
#include "IdentifiedItemNode.h"
#include "IdentificationManager.h"
#include "LabelFile.h"
#include "Overlay.h"
#include "OverlaySet.h"
#include "Model.h"
#include "ProgressReportingDialog.h"
#include "SelectionItemBorderSurface.h"
#include "SelectionItemFocusSurface.h"
#include "SelectionItemFocusVolume.h"
#include "SelectionItemSurfaceNode.h"
#include "SelectionItemSurfaceNodeIdentificationSymbol.h"
#include "SelectionItemVoxel.h"
#include "SelectionManager.h"
#include "Surface.h"
#include "TimeCourseDialog.h"
#include "UserInputModeFociWidget.h"
#include "VolumeFile.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainOpenGLWidgetContextMenu 
 * \brief Context (pop-up) menu for BrainOpenGLWidget
 *
 * Displays a menu in the BrainOpenGLWidget.  Content of menu
 * is dependent upon data under the cursor.
 */
/**
 * Constructor.
 * @param identificationManager
 *    The identification manager, provides data under the cursor.
 * @param parent
 *    Parent on which the menu is displayed.
 */
BrainOpenGLWidgetContextMenu::BrainOpenGLWidgetContextMenu(SelectionManager* identificationManager,
                                                           BrowserTabContent* browserTabContent,
                                                           QWidget* parent)
: QMenu(parent)
{
    this->parentWidget = parent;
    this->identificationManager = identificationManager;
    this->browserTabContent = browserTabContent;
    
    /*
     * Add the identification actions.
     */
    addIdentificationActions();
    
    /*
     * Add the border options.
     */
    addBorderRegionOfInterestActions();
    
    /*
     * Add the foci actions.
     */
    addFociActions();
    
    /*
     * Add the label region of interest operations
     */
    addLabelRegionOfInterestActions();
    
//    /*
//     * Identify Node
//     */
//    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
//    
//    std::vector<QAction*> ciftiConnectivityActions;
//    QActionGroup* ciftiConnectivityActionGroup = new QActionGroup(this);
//    QObject::connect(ciftiConnectivityActionGroup, SIGNAL(triggered(QAction*)),
//                     this, SLOT(parcelCiftiConnectivityActionSelected(QAction*)));
//    
//    std::vector<QAction*> ciftiFiberTrajectoryActions;
//    QActionGroup* ciftiFiberTrajectoryActionGroup = new QActionGroup(this);
//    QObject::connect(ciftiFiberTrajectoryActionGroup, SIGNAL(triggered(QAction*)),
//                     this, SLOT(parcelCiftiFiberTrajectoryActionSelected(QAction*)));
//    
//    std::vector<QAction*> dataSeriesActions;
//    QActionGroup* dataSeriesActionGroup = new QActionGroup(this);
//    QObject::connect(dataSeriesActionGroup, SIGNAL(triggered(QAction*)),
//                     this, SLOT(parcelChartableDataActionSelected(QAction*)));
//    if (surfaceID->isValid()) {
//        /*
//         * Connectivity actions for labels
//         */
//        Brain* brain = surfaceID->getBrain();
//        Surface* surface = surfaceID->getSurface();
//        const int32_t nodeNumber = surfaceID->getNodeNumber();
//        
//        CiftiConnectivityMatrixDataFileManager* connMatrixMan = brain->getCiftiConnectivityMatrixDataFileManager();
//        std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
//        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
//        bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
//        
//        CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager = brain->getCiftiFiberTrajectoryManager();
//        std::vector<CiftiFiberTrajectoryFile*> ciftiFiberTrajectoryFiles;
//        const int32_t numFiberFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
//        for (int32_t i = 0; i < numFiberFiles; i++) {
//            ciftiFiberTrajectoryFiles.push_back(brain->getConnectivityFiberTrajectoryFile(i));
//        }
//        const bool haveCiftiFiberTrajectoryFiles = (ciftiFiberTrajectoryFiles.empty() == false);
//        
//        std::vector<ChartableInterface*> chartableFiles;
//        brain->getAllChartableDataFiles(chartableFiles);
//        const bool haveChartableFiles = (chartableFiles.empty() == false);
//        ChartingDataManager* chartingDataManager = brain->getChartingDataManager();
//    
//        Model* model = this->browserTabContent->getModelControllerForDisplay();
//        if (model != NULL) {
//            std::vector<CaretMappableDataFile*> allMappableLabelFiles;
//            
//            std::vector<CiftiBrainordinateLabelFile*> ciftiLabelFiles;
//            brain->getConnectivityDenseLabelFiles(ciftiLabelFiles);
//            allMappableLabelFiles.insert(allMappableLabelFiles.end(),
//                                 ciftiLabelFiles.begin(),
//                                 ciftiLabelFiles.end());
//            
//                std::vector<LabelFile*> brainStructureLabelFiles;
//                surface->getBrainStructure()->getLabelFiles(brainStructureLabelFiles);
//            allMappableLabelFiles.insert(allMappableLabelFiles.end(),
//                                 brainStructureLabelFiles.begin(),
//                                 brainStructureLabelFiles.end());
//            
//            const int32_t numberOfLabelFiles = static_cast<int32_t>(allMappableLabelFiles.size());
//            for (int32_t ilf = 0; ilf < numberOfLabelFiles; ilf++) {
//                CaretMappableDataFile* mappableLabelFile = allMappableLabelFiles[ilf];
//                const int32_t numMaps = mappableLabelFile->getNumberOfMaps();
//                for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
//                    
//                    int32_t labelKey = -1;
//                    AString labelName;
//                    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
//                    LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
//                    
//                    if (ciftiLabelFile != NULL) {
//                        float nodeValue;
//                        bool nodeValueValid = false;
//                        AString stringValue;
//                        if (ciftiLabelFile->getMapSurfaceNodeValue(mapIndex,
//                                                                   surface->getStructure(),
//                                                                   nodeNumber,
//                                                                   surface->getNumberOfNodes(),
//                                                                   nodeValue,
//                                                                   nodeValueValid,
//                                                                   stringValue)) {
//                            if (nodeValueValid) {
//                                labelKey = static_cast<int32_t>(nodeValue);
//                                const GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
//                                labelName =  labelTable->getLabelName(labelKey);
//                            }
//                        }
//                    }
//                    else if (labelFile != NULL) {
//                        labelKey = labelFile->getLabelKey(nodeNumber,
//                                                          mapIndex);
//                        labelName = labelFile->getLabelName(nodeNumber,
//                                                            mapIndex);
//                    }
//                    else {
//                        CaretAssertMessage(0,
//                                           "Should never get here, new or invalid label file type");
//                    }
//                    
//                    const AString mapName = mappableLabelFile->getMapName(mapIndex);
//                    if (labelName.isEmpty() == false) {
//                        ParcelConnectivity* pc = new ParcelConnectivity(mappableLabelFile,
//                                                                        mapIndex,
//                                                                        labelKey,
//                                                                        labelName,
//                                                                        surface,
//                                                                        nodeNumber,
//                                                                        chartingDataManager,
//                                                                        connMatrixMan,
//                                                                        ciftiFiberTrajectoryManager);
//                        this->parcelConnectivities.push_back(pc);
//                        
//                        if (hasCiftiConnectivity) {
//                            const AString actionName("Show Cifti Connectivity For Parcel "
//                                                     + labelName
//                                                     + " in map "
//                                                     + mapName);
//                            QAction* action = ciftiConnectivityActionGroup->addAction(actionName);
//                            action->setData(qVariantFromValue((void*)pc));
//                            ciftiConnectivityActions.push_back(action);
//                        }
//                        
//                        if (haveCiftiFiberTrajectoryFiles) {
//                            const AString fiberTrajActionName("Show Average Fiber Trajectory for Parcel "
//                                                              + labelName
//                                                              + " in map "
//                                                              + mapName);
//                            QAction* fiberTrajAction = ciftiFiberTrajectoryActionGroup->addAction(fiberTrajActionName);
//                            fiberTrajAction->setData(qVariantFromValue((void*)pc));
//                            ciftiFiberTrajectoryActions.push_back(fiberTrajAction);
//                        }
//                        
//                        if (haveChartableFiles) {
//                            const AString tsActionName("Show Data Series Graph For Parcel "
//                                                       + labelName
//                                                       + " in map "
//                                                       + mapName);
//                            QAction* tsAction = dataSeriesActionGroup->addAction(tsActionName);
//                            tsAction->setData(qVariantFromValue((void*)pc));
//                            dataSeriesActions.push_back(tsAction);
//                        }
//                    }
//                }
//            }
//        }
//    }
    
//    if (idVoxel->isValid()) {
//        Brain* brain = idVoxel->getBrain();
//        std::vector<CaretMappableDataFile*> mappableFiles;
//        brain->getAllMappableDataFiles(mappableFiles);
//        
//        double voxelXYZDouble[3];
//        idVoxel->getModelXYZ(voxelXYZDouble);
//        const float voxelXYZ[3] = {
//            voxelXYZDouble[0],
//            voxelXYZDouble[1],
//            voxelXYZDouble[2]
//        };
//        const int32_t numberOfFiles = static_cast<int32_t>(mappableFiles.size());
//        for (int32_t ilf = 0; ilf < numberOfFiles; ilf++) {
//            CaretMappableDataFile* mappableLabelFile = mappableFiles[ilf];
//            if (mappableLabelFile->isMappedWithLabelTable()
//                && mappableLabelFile->isVolumeMappable()) {
//                const int32_t numMaps = mappableLabelFile->getNumberOfMaps();
//                for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
//                    
//                    int32_t labelKey = -1;
//                    AString labelName;
//                    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
//                    VolumeFile* volumeLabelFile = dynamic_cast<VolumeFile*>(mappableLabelFile);
//                    VolumeMappableInterface* volumeInterface = dynamic_cast<VolumeMappableInterface*>(mappableLabelFile);
//                    if (volumeInterface != NULL) {
//                        float nodeValue;
//                        bool nodeValueValid = false;
//                        AString stringValue;
//                        
//                        int64_t voxelIJK[3];
//                        float voxelValue;
//                        bool voxelValueValid;
//                        AString textValue;
//                        if (ciftiLabelFile->getMapVolumeVoxelValue(mapIndex,
//                                                                   voxelXYZ,
//                                                                   voxelIJK,
//                                                                   voxelValue,
//                                                                   voxelValueValid,
//                                                                   textValue)) {
//                            if (voxelValueValid) {
//                                labelKey = static_cast<int32_t>(voxelValue);
//                                const GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
//                                labelName =  labelTable->getLabelName(labelKey);
//                            }
//                        }
//                    }
//                    else {
//                        CaretAssertMessage(0,
//                                           "Should never get here, new or invalid label file type");
//                    }
//                    
//                    const AString mapName = mappableLabelFile->getMapName(mapIndex);
//                    
//                    
////                if (labelName.isEmpty() == false) {
////                    ParcelConnectivity* pc = new ParcelConnectivity(mappableLabelFile,
////                                                                    mapIndex,
////                                                                    labelKey,
////                                                                    labelName,
////                                                                    surface,
////                                                                    nodeNumber,
////                                                                    chartingDataManager,
////                                                                    connMatrixMan,
////                                                                    ciftiFiberTrajectoryManager);
////                    this->parcelConnectivities.push_back(pc);
////                    
////                    if (hasCiftiConnectivity) {
////                        const AString actionName("Show Cifti Connectivity For Parcel "
////                                                 + labelName
////                                                 + " in map "
////                                                 + mapName);
////                        QAction* action = ciftiConnectivityActionGroup->addAction(actionName);
////                        action->setData(qVariantFromValue((void*)pc));
////                        ciftiConnectivityActions.push_back(action);
////                    }
////                    
////                    if (haveCiftiFiberTrajectoryFiles) {
////                        const AString fiberTrajActionName("Show Average Fiber Trajectory for Parcel "
////                                                          + labelName
////                                                          + " in map "
////                                                          + mapName);
////                        QAction* fiberTrajAction = ciftiFiberTrajectoryActionGroup->addAction(fiberTrajActionName);
////                        fiberTrajAction->setData(qVariantFromValue((void*)pc));
////                        ciftiFiberTrajectoryActions.push_back(fiberTrajAction);
////                    }
////                    
////                    if (haveChartableFiles) {
////                        const AString tsActionName("Show Data Series Graph For Parcel "
////                                                   + labelName
////                                                   + " in map "
////                                                   + mapName);
////                        QAction* tsAction = dataSeriesActionGroup->addAction(tsActionName);
////                        tsAction->setData(qVariantFromValue((void*)pc));
////                        dataSeriesActions.push_back(tsAction);
////                    }
////                }
//                    
//                    
//                }
//            }
//        }
//    }
    
//    if (ciftiConnectivityActions.empty() == false) {
//        this->addSeparator();
//        for (std::vector<QAction*>::iterator ciftiConnIter = ciftiConnectivityActions.begin();
//             ciftiConnIter != ciftiConnectivityActions.end();
//             ciftiConnIter++) {
//            this->addAction(*ciftiConnIter);
//        }
//    }
//    
//    if (ciftiFiberTrajectoryActions.empty() == false) {
//        this->addSeparator();
//        for (std::vector<QAction*>::iterator ciftiFiberIter = ciftiFiberTrajectoryActions.begin();
//             ciftiFiberIter != ciftiFiberTrajectoryActions.end();
//             ciftiFiberIter++) {
//            this->addAction(*ciftiFiberIter);
//        }
//    }
//    if(dataSeriesActions.empty() == false) {
//        this->addSeparator();            
//        for (std::vector<QAction*>::iterator tsIter = dataSeriesActions.begin();
//             tsIter != dataSeriesActions.end();
//             tsIter++) {
//            this->addAction(*tsIter);
//        }
//    }
    
//    std::vector<QAction*> createActions;
//    
    const SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();
//
//    /*
//     * Create focus at surface node or at ID symbol
//     */
//    if (surfaceID->isValid()
//        && (focusID->isValid() == false)) {
//        const int32_t nodeIndex = surfaceID->getNodeNumber();
//        const Surface* surface = surfaceID->getSurface();
//        const QString text = ("Create Focus at Vertex "
//                              + QString::number(nodeIndex)
//                              + " ("
//                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
//                              + ")...");
//        
//        createActions.push_back(WuQtUtilities::createAction(text,
//                                                            "",
//                                                            this,
//                                                            this,
//                                                            SLOT(createSurfaceFocusSelected())));
//    }
//    else if (idSymbol->isValid()
//             && (focusID->isValid() == false)) {
//        const int32_t nodeIndex = idSymbol->getNodeNumber();
//        const Surface* surface = idSymbol->getSurface();
//        const QString text = ("Create Focus at Selected Vertex "
//                              + QString::number(nodeIndex)
//                              + " ("
//                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
//                              + ")...");
//        
//        createActions.push_back(WuQtUtilities::createAction(text,
//                                                            "",
//                                                            this,
//                                                            this,
//                                                            SLOT(createSurfaceIDSymbolFocusSelected())));
//    }
//
//    /*
//     * Create focus at voxel as long as there is no volume focus ID
//     */
//    if (idVoxel->isValid()
//        && (focusVolID->isValid() == false)) {
//        int64_t ijk[3];
//        idVoxel->getVoxelIJK(ijk);
//        float xyz[3];
//        const VolumeMappableInterface* vf = idVoxel->getVolumeFile();
//        vf->indexToSpace(ijk, xyz);
//        
//        const AString text = ("Create Focus at Voxel IJK ("
//                              + AString::fromNumbers(ijk, 3, ",")
//                              + ") XYZ ("
//                              + AString::fromNumbers(xyz, 3, ",")
//                              + ")...");
//        createActions.push_back(WuQtUtilities::createAction(text,
//                                                            "",
//                                                            this,
//                                                            this,
//                                                            SLOT(createVolumeFocusSelected())));
//    }
//    
//    if (createActions.empty() == false) {
//        if (this->actions().count() > 0) {
//            this->addSeparator();
//        }
//        for (std::vector<QAction*>::iterator iter = createActions.begin();
//             iter != createActions.end();
//             iter++) {
//            this->addAction(*iter);
//        }
//    }
//    
//    /*
//     * Actions for editing
//     */
//    std::vector<QAction*> editActions;
//    
//    /*
//     * Edit Surface Focus
//     */
//    if (focusID->isValid()) {
//        const QString text = ("Edit Surface Focus ("
//                              + focusID->getFocus()->getName()
//                              + ")");
//        editActions.push_back(WuQtUtilities::createAction(text,
//                                                          "",
//                                                          this,
//                                                          this,
//                                                          SLOT(editSurfaceFocusSelected())));
//    }
//    
//    /*
//     * Edit volume focus
//     */
//    if (focusVolID->isValid()) {
//        const QString text = ("Edit Volume Focus ("
//                              + focusVolID->getFocus()->getName()
//                              + ")");
//        editActions.push_back(WuQtUtilities::createAction(text,
//                                                          "",
//                                                          this,
//                                                          this,
//                                                          SLOT(editVolumeFocusSelected())));
//    }
//    
//    if (editActions.empty() == false) {
//        if (this->actions().count() > 0) {
//            this->addSeparator();
//        }
//        for (std::vector<QAction*>::iterator iter = editActions.begin();
//             iter != editActions.end();
//             iter++) {
//            this->addAction(*iter);
//        }
//    }
    
    if (this->actions().count() > 0) {
        this->addSeparator();
    }
    this->addAction("Remove All Vertex Identification Symbols",
                    this,
                    SLOT(removeAllNodeIdentificationSymbolsSelected()));
    
    if (idSymbol->isValid()) {
        const AString text = ("Remove Identification of Vertices "
                              + AString::number(idSymbol->getNodeNumber()));
        
        this->addAction(WuQtUtilities::createAction(text,
                                                    "",
                                                    this,
                                                    this,
                                                    SLOT(removeNodeIdentificationSymbolSelected())));
    }
}

/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::~BrainOpenGLWidgetContextMenu()
{
    for (std::vector<ParcelConnectivity*>::iterator parcelIter = this->parcelConnectivities.begin();
         parcelIter != this->parcelConnectivities.end();
         parcelIter++) {
        delete *parcelIter;
    }
}

/**
 * Add the actions to this context menu.
 *
 * @param actionsToAdd
 *     Actions to add to the menum.
 * @param addSeparatorBeforeActions
 *     If true and there are actions presently in the menu, a separator
 *     (horizontal bar) is added prior to adding the given actions.
 */
void
BrainOpenGLWidgetContextMenu::addActionsToMenu(QList<QAction*>& actionsToAdd,
                                               const bool addSeparatorBeforeActions)
{
    if (actionsToAdd.empty() == false) {
        if (addSeparatorBeforeActions) {
            if (actions().isEmpty() == false) {
                addSeparator();
            }
        }

        addActions(actionsToAdd);
    }
}


/**
 * Add the identification actions to the menu.
 */
void
BrainOpenGLWidgetContextMenu::addIdentificationActions()
{
    /*
     * Accumlate identification actions
     */
    QList<QAction*> identificationActions;
    
    /*
     * Identify Border
     */
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    if (borderID->isValid()) {
        const QString text = ("Identify Border ("
                              + borderID->getBorder()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifySurfaceBorderSelected())));
    }
    
    /*
     * Identify Surface Focus
     */
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    if (focusID->isValid()) {
        const QString text = ("Identify Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifySurfaceFocusSelected())));
    }
    
    /*
     * Identify Node
     */
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    if (surfaceID->isValid()) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const Surface* surface = surfaceID->getSurface();
        const QString text = ("Identify Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ") Under Mouse");
        
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifySurfaceNodeSelected())));
    }
    
    /*
     * Identify Voxel
     */
    SelectionItemVoxel* idVoxel = this->identificationManager->getVoxelIdentification();
    if (idVoxel->isValid()) {
        int64_t ijk[3];
        idVoxel->getVoxelIJK(ijk);
        const AString text = ("Identify Voxel ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ")");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifyVoxelSelected())));
    }
    
    /*
     * Identify Volume Focus
     */
    SelectionItemFocusVolume* focusVolID = this->identificationManager->getVolumeFocusIdentification();
    if (focusVolID->isValid()) {
        const QString text = ("Identify Volume Focus ("
                              + focusVolID->getFocus()->getName()
                              + ") Under Mouse");
        identificationActions.push_back(WuQtUtilities::createAction(text,
                                                                    "",
                                                                    this,
                                                                    this,
                                                                    SLOT(identifyVolumeFocusSelected())));
    }
    
    addActionsToMenu(identificationActions,
                     true);
}

/**
 * Add the border region of interest actions to the menu.
 */
void
BrainOpenGLWidgetContextMenu::addBorderRegionOfInterestActions()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    
    QList<QAction*> borderActions;
    
    if (borderID->isValid()) {
        Brain* brain = borderID->getBrain();
        std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
        brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
        bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
        
        /*
         * Connectivity actions for borders
         */
        if (hasCiftiConnectivity) {
            const QString text = ("Show CIFTI Connectivity for Nodes Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderCiftiConnectivitySelected()));
            borderActions.push_back(action);
        }
        
        std::vector<ChartableInterface*> chartableFiles;
        brain->getAllChartableDataFiles(chartableFiles);
        
        if (chartableFiles.empty() == false) {
            const QString text = ("Show Charts for Nodes Inside Border "
                                  + borderID->getBorder()->getName());
            QAction* action = WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(borderDataSeriesSelected()));
            borderActions.push_back(action);
        }
    }

    addActionsToMenu(borderActions,
                     true);
}

/**
 * Add all label region of interest options to the menu
 */
void
BrainOpenGLWidgetContextMenu::addLabelRegionOfInterestActions()
{
    Brain* brain = NULL;
    
    float voxelXYZ[3] = { 0.0, 0.0, 0.0 };
    SelectionItemVoxel* idVoxel = this->identificationManager->getVoxelIdentification();
    if (idVoxel->isValid()) {
        double voxelXYZDouble[3];
        idVoxel->getModelXYZ(voxelXYZDouble);
        
        voxelXYZ[0] = voxelXYZDouble[0];
        voxelXYZ[1] = voxelXYZDouble[1];
        voxelXYZ[2] = voxelXYZDouble[2];
        brain = idVoxel->getBrain();
    }
    
    Surface* surface = NULL;
    int32_t surfaceNodeIndex = -1;
    int32_t surfaceNumberOfNodes = -1;
    StructureEnum::Enum surfaceStructure = StructureEnum::INVALID;
    SelectionItemSurfaceNode* idNode = this->identificationManager->getSurfaceNodeIdentification();
    if (idNode->isValid()) {
        surface = idNode->getSurface();
        surfaceNodeIndex = idNode->getNodeNumber();
        surfaceNumberOfNodes = surface->getNumberOfNodes();
        surfaceStructure = surface->getStructure();
        brain = idNode->getBrain();
    }
    
    /*
     * If Brain is invalid, then there is no identified node or voxel
     */
    if (brain == NULL) {
        return;
    }
    
    /*
     * Manager for connectivity matrix files
     */
    CiftiConnectivityMatrixDataFileManager* ciftiConnectivityMatrixManager = brain->getCiftiConnectivityMatrixDataFileManager();
    std::vector<CiftiMappableConnectivityMatrixDataFile*> ciftiMatrixFiles;
    brain->getAllCiftiConnectivityMatrixFiles(ciftiMatrixFiles);
    bool hasCiftiConnectivity = (ciftiMatrixFiles.empty() == false);
    
    /*
     * Manager for fiber trajectory
     */
    CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager = brain->getCiftiFiberTrajectoryManager();
    std::vector<CiftiFiberTrajectoryFile*> ciftiFiberTrajectoryFiles;
    const int32_t numFiberFiles = brain->getNumberOfConnectivityFiberTrajectoryFiles();
    for (int32_t i = 0; i < numFiberFiles; i++) {
        ciftiFiberTrajectoryFiles.push_back(brain->getConnectivityFiberTrajectoryFile(i));
    }
    const bool haveCiftiFiberTrajectoryFiles = (ciftiFiberTrajectoryFiles.empty() == false);
    
    /*
     * Manager for Chartable files
     */
    std::vector<ChartableInterface*> chartableFiles;
    brain->getAllChartableDataFiles(chartableFiles);
    const bool haveChartableFiles = (chartableFiles.empty() == false);
    ChartingDataManager* chartingDataManager = brain->getChartingDataManager();

    /*
     * Actions for each file type
     */
    QList<QAction*> ciftiConnectivityActions;
    QActionGroup* ciftiConnectivityActionGroup = new QActionGroup(this);
    QObject::connect(ciftiConnectivityActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiConnectivityActionSelected(QAction*)));
    
    QList<QAction*> ciftiFiberTrajectoryActions;
    QActionGroup* ciftiFiberTrajectoryActionGroup = new QActionGroup(this);
    QObject::connect(ciftiFiberTrajectoryActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelCiftiFiberTrajectoryActionSelected(QAction*)));
    
    QList<QAction*> chartableDataActions;
    QActionGroup* chartableDataActionGroup = new QActionGroup(this);
    QObject::connect(chartableDataActionGroup, SIGNAL(triggered(QAction*)),
                     this, SLOT(parcelChartableDataActionSelected(QAction*)));

    /*
     * Get all mappable files and find files mapped with using labels
     */
    std::vector<CaretMappableDataFile*> mappableFiles;
    brain->getAllMappableDataFiles(mappableFiles);

    /*
     * Process each map file
     */
    for (std::vector<CaretMappableDataFile*>::iterator mapFileIterator = mappableFiles.begin();
         mapFileIterator != mappableFiles.end();
         mapFileIterator++) {
        CaretMappableDataFile* mappableLabelFile = *mapFileIterator;
        
        if (mappableLabelFile->isMappedWithLabelTable()) {
            const int32_t numMaps = mappableLabelFile->getNumberOfMaps();
            for (int32_t mapIndex = 0; mapIndex < numMaps; mapIndex++) {
                
                Surface* labelSurface = NULL;
                int32_t labelNodeNumber = -1;
                int32_t labelKey = -1;
                AString labelName;
                
                ParcelConnectivity::ParcelType parcelType = ParcelConnectivity::PARCEL_TYPE_INVALID;
                
                if (mappableLabelFile->isVolumeMappable()) {
                    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
                    VolumeFile* volumeLabelFile = dynamic_cast<VolumeFile*>(mappableLabelFile);
                    VolumeMappableInterface* volumeInterface = dynamic_cast<VolumeMappableInterface*>(mappableLabelFile);
                    if (volumeInterface != NULL) {
                        int64_t voxelIJK[3];
                        float voxelValue;
                        bool voxelValueValid;
                        AString textValue;
                        if (ciftiLabelFile != NULL) {
                            if (ciftiLabelFile->getMapVolumeVoxelValue(mapIndex,
                                                                       voxelXYZ,
                                                                       voxelIJK,
                                                                       voxelValue,
                                                                       voxelValueValid,
                                                                       textValue)) {
                                if (voxelValueValid) {
                                    labelKey = static_cast<int32_t>(voxelValue);
                                    const GiftiLabelTable* labelTable = ciftiLabelFile->getMapLabelTable(mapIndex);
                                    labelName =  labelTable->getLabelName(labelKey);
                                    
                                    if (labelName.isEmpty() == false) {
                                        parcelType = ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS;
                                    }
                                }
                            }
                        }
                        else if (volumeLabelFile != NULL) {
                            int64_t voxelIJK[3];
                            volumeLabelFile->enclosingVoxel(voxelXYZ, voxelIJK);
                            if (volumeLabelFile->indexValid(voxelIJK)) {
                                const float voxelValue = volumeLabelFile->getValue(voxelIJK[0],
                                                                                   voxelIJK[1],
                                                                                   voxelIJK[2],
                                                                                   mapIndex);
                                const GiftiLabelTable* labelTable = volumeLabelFile->getMapLabelTable(mapIndex);
                                labelKey = static_cast<int32_t>(voxelValue);
                                labelName = labelTable->getLabelName(voxelValue);

                                if (labelName.isEmpty() == false) {
                                    parcelType = ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS;
                                }
                            }
                        }
                        else {
                            CaretAssertMessage(0,
                                               "Should never get here, new or invalid label file type");
                        }
                    }
                }
                
                if (mappableLabelFile->isSurfaceMappable()) {
                    if (labelName.isEmpty()) {
                        if (idNode->isValid()) {
                            labelSurface = idNode->getSurface();
                            labelNodeNumber = idNode->getNodeNumber();
                            
                            LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
                            CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
                            
                            if (labelFile != NULL) {
                                labelKey = labelFile->getLabelKey(labelNodeNumber, mapIndex);
                                const GiftiLabelTable* labelTable = labelFile->getMapLabelTable(mapIndex);
                                labelName = labelTable->getLabelName(labelKey);
                                
                                if (labelName.isEmpty() == false) {
                                    parcelType = ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES;
                                }
                            }
                            else if (ciftiLabelFile != NULL) {
                                float nodeValue = 0.0;
                                bool nodeValueValid = false;
                                AString stringValue;
                                if (ciftiLabelFile->getMapSurfaceNodeValue(mapIndex,
                                                                           surfaceStructure,
                                                                           surfaceNodeIndex,
                                                                           surfaceNumberOfNodes,
                                                                           nodeValue,
                                                                           nodeValueValid,
                                                                           stringValue)) {
                                    if (nodeValueValid) {
                                        labelKey = nodeValue;
                                        const GiftiLabelTable* labelTable = labelFile->getMapLabelTable(mapIndex);
                                        labelName = labelTable->getLabelName(labelKey);

                                        if (labelName.isEmpty() == false) {
                                            parcelType = ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                if (parcelType != ParcelConnectivity::PARCEL_TYPE_INVALID) {
                    const AString mapName = mappableLabelFile->getMapName(mapIndex);
                    
                    ParcelConnectivity* parcelConnectivity = new ParcelConnectivity(parcelType,
                                                                                    mappableLabelFile,
                                                                                  mapIndex,
                                                                                  labelKey,
                                                                                  labelName,
                                                                                  labelSurface,
                                                                                  labelNodeNumber,
                                                                                  chartingDataManager,
                                                                                  ciftiConnectivityMatrixManager,
                                                                                  ciftiFiberTrajectoryManager);
                        this->parcelConnectivities.push_back(parcelConnectivity);

                    if (hasCiftiConnectivity) {
                        const AString actionName("Show Cifti Connectivity For Parcel "
                                                 + labelName
                                                 + " in map "
                                                 + mapName);
                        QAction* action = ciftiConnectivityActionGroup->addAction(actionName);
                        action->setData(qVariantFromValue((void*)parcelConnectivity));
                        ciftiConnectivityActions.push_back(action);
                    }
                    
                    if (haveCiftiFiberTrajectoryFiles) {
                        const AString fiberTrajActionName("Show Average Fiber Trajectory for Parcel "
                                                          + labelName
                                                          + " in map "
                                                          + mapName);
                        QAction* fiberTrajAction = ciftiFiberTrajectoryActionGroup->addAction(fiberTrajActionName);
                        fiberTrajAction->setData(qVariantFromValue((void*)parcelConnectivity));
                        ciftiFiberTrajectoryActions.push_back(fiberTrajAction);
                    }
                    
                    if (haveChartableFiles) {
                        const AString tsActionName("Show Data Series Graph For Parcel "
                                                   + labelName
                                                   + " in map "
                                                   + mapName);
                        QAction* tsAction = chartableDataActionGroup->addAction(tsActionName);
                        tsAction->setData(qVariantFromValue((void*)parcelConnectivity));
                        chartableDataActions.push_back(tsAction);
                    }
                }
            }
        }
    }

    addActionsToMenu(ciftiConnectivityActions,
                     true);
    addActionsToMenu(ciftiFiberTrajectoryActions,
                     true);
    addActionsToMenu(chartableDataActions,
                     true);
}

/**
 * Add the foci options to the menu.
 */
void
BrainOpenGLWidgetContextMenu::addFociActions()
{
    QList<QAction*> fociCreateActions;
    
    const SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    SelectionItemVoxel* idVoxel = this->identificationManager->getVoxelIdentification();
    SelectionItemFocusVolume* focusVolID = this->identificationManager->getVolumeFocusIdentification();
    
    
    /*
     * Create focus at surface node or at ID symbol
     */
    if (surfaceID->isValid()
        && (focusID->isValid() == false)) {
        const int32_t nodeIndex = surfaceID->getNodeNumber();
        const Surface* surface = surfaceID->getSurface();
        const QString text = ("Create Focus at Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ")...");
        
        fociCreateActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceFocusSelected())));
    }
    else if (idSymbol->isValid()
             && (focusID->isValid() == false)) {
        const int32_t nodeIndex = idSymbol->getNodeNumber();
        const Surface* surface = idSymbol->getSurface();
        const QString text = ("Create Focus at Selected Vertex "
                              + QString::number(nodeIndex)
                              + " ("
                              + AString::fromNumbers(surface->getCoordinate(nodeIndex), 3, ",")
                              + ")...");
        
        fociCreateActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createSurfaceIDSymbolFocusSelected())));
    }
    
    /*
     * Create focus at voxel as long as there is no volume focus ID
     */
    if (idVoxel->isValid()
        && (focusVolID->isValid() == false)) {
        int64_t ijk[3];
        idVoxel->getVoxelIJK(ijk);
        float xyz[3];
        const VolumeMappableInterface* vf = idVoxel->getVolumeFile();
        vf->indexToSpace(ijk, xyz);
        
        const AString text = ("Create Focus at Voxel IJK ("
                              + AString::fromNumbers(ijk, 3, ",")
                              + ") XYZ ("
                              + AString::fromNumbers(xyz, 3, ",")
                              + ")...");
        fociCreateActions.push_back(WuQtUtilities::createAction(text,
                                                            "",
                                                            this,
                                                            this,
                                                            SLOT(createVolumeFocusSelected())));
    }
    
    addActionsToMenu(fociCreateActions,
                     true);
    
    /*
     * Actions for editing
     */
    QList<QAction*> fociEditActions;
    
    /*
     * Edit Surface Focus
     */
    if (focusID->isValid()) {
        const QString text = ("Edit Surface Focus ("
                              + focusID->getFocus()->getName()
                              + ")");
        fociEditActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editSurfaceFocusSelected())));
    }
    
    /*
     * Edit volume focus
     */
    if (focusVolID->isValid()) {
        const QString text = ("Edit Volume Focus ("
                              + focusVolID->getFocus()->getName()
                              + ")");
        fociEditActions.push_back(WuQtUtilities::createAction(text,
                                                          "",
                                                          this,
                                                          this,
                                                          SLOT(editVolumeFocusSelected())));
    }
    
    addActionsToMenu(fociEditActions,
                     true);
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
BrainOpenGLWidgetContextMenu::parcelCiftiConnectivityActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    switch (pc->parcelType) {
        case ParcelConnectivity::PARCEL_TYPE_INVALID:
            break;
        case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            
            if (this->warnIfNetworkNodeCountIsLarge(pc->ciftiConnectivityManager,
                                                    nodeIndices) == false) {
                return;
            }
            break;
        case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
            break;
    }
    
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    try {
        ProgressReportingDialog progressDialog("Connectivity Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        switch (pc->parcelType) {
            case ParcelConnectivity::PARCEL_TYPE_INVALID:
                break;
            case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
                pc->ciftiConnectivityManager->loadAverageDataForSurfaceNodes(pc->surface,
                                                                             nodeIndices);
                break;
            case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
                break;
        }
    }
    catch (const DataFileException& e) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when a cifti connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void
BrainOpenGLWidgetContextMenu::parcelCiftiFiberTrajectoryActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    
    switch (pc->parcelType) {
        case ParcelConnectivity::PARCEL_TYPE_INVALID:
            break;
        case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            
            if (this->warnIfNetworkNodeCountIsLarge(pc->ciftiConnectivityManager,
                                                    nodeIndices) == false) {
                return;
            }
            break;
        case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
            break;
    }
    
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    try {
        ProgressReportingDialog progressDialog("Trajectory Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        switch (pc->parcelType) {
            case ParcelConnectivity::PARCEL_TYPE_INVALID:
                break;
            case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
                pc->ciftiFiberTrajectoryManager->loadDataAverageForSurfaceNodes(pc->surface,
                                                                                nodeIndices);
                break;
            case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
                break;
        }
    }
    catch (const DataFileException& e) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this, e.whatString());
    }
    
    
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when border cifti connectivity is selected.
 */
void
BrainOpenGLWidgetContextMenu::borderCiftiConnectivitySelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getChartingDataManager(),
                                                nodeIndices) == false) {
            return;
        }
        
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        try {
            ProgressReportingDialog progressDialog("Connectivity Within Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
            CiftiConnectivityMatrixDataFileManager* ciftiConnMann = borderID->getBrain()->getCiftiConnectivityMatrixDataFileManager();
            ciftiConnMann->loadAverageDataForSurfaceNodes(surface,
                                                          nodeIndices);
        }
        catch (const DataFileException& e) {
            cursor.restoreCursor();
            WuQMessageBox::errorOk(this, e.whatString());
        }
        
        
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }
}

///**
// * Called when border connectivity is selected.
// */
//void 
//BrainOpenGLWidgetContextMenu::borderConnectivitySelected()
//{
//    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
//    Border* border = borderID->getBorder();
//    Surface* surface = borderID->getSurface();
//    
//    const int32_t numberOfNodes = surface->getNumberOfNodes();
//    LabelFile labelFile;
//    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
//    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
//    const int32_t mapIndex = 0;
//    
//    try {
//        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
//                                                         surface,
//                                                         border,
//                                                         false,
//                                                         mapIndex,
//                                                         labelKey,
//                                                         &labelFile);
//        std::vector<int32_t> nodeIndices;
//        nodeIndices.reserve(numberOfNodes);
//        for (int32_t i = 0; i < numberOfNodes; i++) {
//            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
//                nodeIndices.push_back(i);
//            }
//        }
//        
//        if (nodeIndices.empty()) {
//            WuQMessageBox::errorOk(this,
//                                   "No vertices found inside border " + border->getName());
//            return;
//        }
//        
//        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getChartingDataManager(),
//                                                nodeIndices) == false) {
//            return;
//        }
//        
//        try {
//            ProgressReportingDialog progressDialog("Connectivity Within Border",
//                                                    "",
//                                                    this);
//            progressDialog.setValue(0);
//            const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
//            
//            QList<TimeLine> timeLines;
//            ChartingDataManager* chartingDataManger = borderID->getBrain()->getChartingDataManager();
//            chartingDataManger->loadAverageChartForSurfaceNodes(surface,
//                                                                 nodeIndices,
//                                                                true,
//                                                                 timeLines);
//            if (showAllGraphs) {
//                displayAllDataSeriesGraphs();
//            }
//            
//            GuiManager::get()->addTimeLines(timeLines);
//            EventUpdateTimeCourseDialog e;
//            EventManager::get()->sendEvent(e.getPointer());
//            EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//        }
//        catch (const DataFileException& e) {
//            WuQMessageBox::errorOk(this, e.whatString());
//        }
//        
//        
//        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());    
//    }
//    catch (const AlgorithmException& e) {
//        WuQMessageBox::errorOk(this, e.whatString());
//    }
//}

/**
 * Called when a connectivity action is selected.
 * @param action
 *    Action that was selected.
 */
void 
BrainOpenGLWidgetContextMenu::parcelChartableDataActionSelected(QAction* action)
{
    void* pointer = action->data().value<void*>();
    ParcelConnectivity* pc = (ParcelConnectivity*)pointer;
    
    std::vector<int32_t> nodeIndices;
    
    switch (pc->parcelType) {
        case ParcelConnectivity::PARCEL_TYPE_INVALID:
            break;
        case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
            pc->getNodeIndices(nodeIndices);
            if (nodeIndices.empty()) {
                WuQMessageBox::errorOk(this,
                                       "No vertices match label " + pc->labelName);
                return;
            }
            
            if (this->warnIfNetworkNodeCountIsLarge(pc->chartingDataManager,
                                                    nodeIndices) == false) {
                return;
            }
            break;
        case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
            break;
    }
    
    
    CursorDisplayScoped cursor;
    cursor.showWaitCursor();
    
    try {
        ProgressReportingDialog progressDialog("Data Series Within Parcel",
                                               "",
                                               this);
        progressDialog.setValue(0);
        
        const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
        
        QList<TimeLine> timeLines;
        switch (pc->parcelType) {
            case ParcelConnectivity::PARCEL_TYPE_INVALID:
                break;
            case ParcelConnectivity::PARCEL_TYPE_SURFACE_NODES:
                pc->chartingDataManager->loadAverageChartForSurfaceNodes(pc->surface,
                                                                         nodeIndices,
                                                                         true,  // only files with charting enabled
                                                                         timeLines);
                break;
            case ParcelConnectivity::PARCEL_TYPE_VOLUME_VOXELS:
                break;
        }

        if(timeLines.size()!=0)
        {
            for (int i = 0; i < timeLines.size(); i++) {
                TimeLine &tl = timeLines[i];
                for(int i=0;i<3;i++) tl.point[i] = 0.0;
                tl.parcelName = pc->labelName;
                tl.structureName = StructureEnum::toGuiName(pc->surface->getStructure());
                tl.label = tl.structureName + ":" + tl.parcelName;
            }
            
            GuiManager::get()->addTimeLines(timeLines);
            
            if (showAllGraphs) {
                displayAllDataSeriesGraphs();
            }
        }
        EventUpdateTimeCourseDialog e;
        EventManager::get()->sendEvent(e.getPointer());
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    }
    catch (const DataFileException& e) {
        cursor.restoreCursor();
        WuQMessageBox::errorOk(this, e.whatString());
    }   
  
}

/**
 * Called when border timeseries is selected.
 */
void 
BrainOpenGLWidgetContextMenu::borderDataSeriesSelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Border* border = borderID->getBorder();
    Surface* surface = borderID->getSurface();
    
    const int32_t numberOfNodes = surface->getNumberOfNodes();
    LabelFile labelFile;
    labelFile.setNumberOfNodesAndColumns(numberOfNodes, 1);
    const int32_t labelKey = labelFile.getLabelTable()->addLabel("TempLabel", 1.0f, 1.0f, 1.0f, 1.0f);
    const int32_t mapIndex = 0;
    
    try {
        AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                         surface,
                                                         border,
                                                         false,
                                                         mapIndex,
                                                         labelKey,
                                                         &labelFile);
        std::vector<int32_t> nodeIndices;
        nodeIndices.reserve(numberOfNodes);
        for (int32_t i = 0; i < numberOfNodes; i++) {
            if (labelFile.getLabelKey(i, mapIndex) == labelKey) {
                nodeIndices.push_back(i);
            }
        }
        
        if (nodeIndices.empty()) {
            WuQMessageBox::errorOk(this,
                                   "No vertices found inside border " + border->getName());
            return;
        }
        
        if (this->warnIfNetworkNodeCountIsLarge(borderID->getBrain()->getChartingDataManager(),
                                                nodeIndices) == false) {
            return;
        }
        
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
        try {
            ProgressReportingDialog progressDialog("Data Series Within Border",
                                                   "",
                                                   this);
            progressDialog.setValue(0);
//            TimeLine tl;
//            for(int i=0;i<3;i++) tl.point[i] = 0.0;
//            tl.borderClassName = border->getClassName();
//            tl.borderName = border->getName();
//            tl.structureName = StructureEnum::toGuiName(border->getStructure());
//            tl.label =  tl.structureName + ":" + tl.borderClassName + ":" + tl.borderName;
            
            const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
            ChartingDataManager* chartingDataManager = borderID->getBrain()->getChartingDataManager();
            QList<TimeLine> timeLines;
            chartingDataManager->loadAverageChartForSurfaceNodes(surface,
                                                                 nodeIndices,
                                                                 true,  // only files with charting enabled
                                                                 timeLines);
            if (timeLines.empty() == false) {
                const int numTimelines = timeLines.size();
                for (int itl = 0; itl < numTimelines; itl++) {
                    for(int i=0;i<3;i++) {
                        timeLines[itl].point[i] = 0.0;
                    }
                    timeLines[itl].borderClassName = border->getClassName();
                    timeLines[itl].borderName = border->getName();
                    timeLines[itl].structureName = StructureEnum::toGuiName(border->getStructure());
                    timeLines[itl].label =  timeLines[itl].structureName + ":" + timeLines[itl].borderClassName + ":" + timeLines[itl].borderName;
                }
                
                if (showAllGraphs) {
                    displayAllDataSeriesGraphs();
                }
                
                GuiManager::get()->addTimeLines(timeLines);
                EventUpdateTimeCourseDialog e;
                EventManager::get()->sendEvent(e.getPointer());
                EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
            }
            
//            ConnectivityLoaderManager* connectivityLoaderManager = borderID->getBrain()->getConnectivityLoaderManager();
//            const bool showAllGraphs = enableDataSeriesGraphsIfNoneEnabled();
//            connectivityLoaderManager->loadAverageTimeSeriesForSurfaceNodes(surface,
//                                                                          nodeIndices,tl);
//            QList <TimeLine> tlV;
//            connectivityLoaderManager->getSurfaceTimeLines(tlV);
//            if(tlV.size()!=0)
//            {
//                if (showAllGraphs) {
//                    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
//                    displayAllDataSeriesGraphs();
//                }
//                GuiManager::get()->addTimeLines(tlV);
//            }
//            EventUpdateTimeCourseDialog e;
//            EventManager::get()->sendEvent(e.getPointer());

        }
        catch (const DataFileException& e) {
            cursor.restoreCursor();
            WuQMessageBox::errorOk(this, e.whatString());
        }   
    }
    catch (const AlgorithmException& e) {
        WuQMessageBox::errorOk(this, e.whatString());
    }

}

/**
 * @return If no data series graphs are enabled, enable all of them and return 
 * true.  Else, return false.
 */
bool
BrainOpenGLWidgetContextMenu::enableDataSeriesGraphsIfNoneEnabled()
{
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<ChartableInterface*> chartFiles;
    brain->getAllChartableDataFiles(chartFiles);
    if (chartFiles.empty()) {
        return false;
    }
    
    /*
     * Exit if any data series graph is enabled.
     */
    for (std::vector<ChartableInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableInterface* chartFile = *iter;
        if (chartFile->isChartingEnabled()) {
            return false;
        }
    }
    
    /*
     * Enable and display all data series graphs.
     */
    for (std::vector<ChartableInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableInterface* chartFile = *iter;
        chartFile->setChartingEnabled(true);
    }

    return true;
}

/**
 * Display all data-series graphs.
 */
void
BrainOpenGLWidgetContextMenu::displayAllDataSeriesGraphs()
{
    Brain* brain = GuiManager::get()->getBrain();
    std::vector<ChartableInterface*> chartFiles;
    brain->getAllChartableDataFiles(chartFiles);
    for (std::vector<ChartableInterface*>::iterator iter = chartFiles.begin();
         iter != chartFiles.end();
         iter++) {
        ChartableInterface* chartFile = *iter;
        chartFile->setChartingEnabled(true);
        TimeCourseDialog* tcd = GuiManager::get()->getTimeCourseDialog(chartFile);
        tcd->setTimeSeriesGraphEnabled(true);
        tcd->show();
    }    
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceBorderSelected()
{
    SelectionItemBorderSurface* borderID = this->identificationManager->getSurfaceBorderIdentification();
    Brain* brain = borderID->getBrain();
    this->identificationManager->clearOtherSelectedItems(borderID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to create a focus at a node location
 */
void
BrainOpenGLWidgetContextMenu::createSurfaceFocusSelected()
{
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    const Surface* surface = surfaceID->getSurface();
    const int32_t nodeIndex = surfaceID->getNodeNumber();
    const float* xyz = surface->getCoordinate(nodeIndex);
    
    const AString focusName = (StructureEnum::toGuiName(surface->getStructure())
                               + " Vertex "
                               + AString::number(nodeIndex));
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}


/**
 * Called to create a focus at a node location
 */
void
BrainOpenGLWidgetContextMenu::createSurfaceIDSymbolFocusSelected()
{
    SelectionItemSurfaceNodeIdentificationSymbol* nodeSymbolID =
        this->identificationManager->getSurfaceNodeIdentificationSymbol();
    
    const Surface* surface = nodeSymbolID->getSurface();
    const int32_t nodeIndex = nodeSymbolID->getNodeNumber();
    const float* xyz = surface->getCoordinate(nodeIndex);
    
    const AString focusName = (StructureEnum::toGuiName(surface->getStructure())
                               + " Vertex "
                               + AString::number(nodeIndex));
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}
/**
 * Called to create a focus at a voxel location
 */
void
BrainOpenGLWidgetContextMenu::createVolumeFocusSelected()
{
    SelectionItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    const VolumeMappableInterface* vf = voxelID->getVolumeFile();
    int64_t ijk[3];
    voxelID->getVoxelIJK(ijk);
    float xyz[3];
    vf->indexToSpace(ijk, xyz);
    
    const CaretMappableDataFile* cmdf = dynamic_cast<const CaretMappableDataFile*>(vf);
    const AString focusName = (cmdf->getFileNameNoPath()
                               + " IJK ("
                               + AString::fromNumbers(ijk, 3, ",")
                               + ")");
    
    const AString comment = ("Created from "
                             + focusName);
    Focus* focus = new Focus();
    focus->setName(focusName);
    focus->getProjection(0)->setStereotaxicXYZ(xyz);
    focus->setComment(comment);
    
    FociPropertiesEditorDialog::createFocus(focus,
                                            this->browserTabContent,
                                            this->parentWidget);
}


/**
 * Called to display identication information on the surface focus.
 */
void
BrainOpenGLWidgetContextMenu::identifySurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    Brain* brain = focusID->getBrain();
    this->identificationManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the volume focus.
 */
void
BrainOpenGLWidgetContextMenu::identifyVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->identificationManager->getVolumeFocusIdentification();
    Brain* brain = focusID->getBrain();
    this->identificationManager->clearOtherSelectedItems(focusID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to edit the surface focus.
 */
void
BrainOpenGLWidgetContextMenu::editSurfaceFocusSelected()
{
    SelectionItemFocusSurface* focusID = this->identificationManager->getSurfaceFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentWidget);
}

/**
 * Called to edit the volume focus.
 */
void
BrainOpenGLWidgetContextMenu::editVolumeFocusSelected()
{
    SelectionItemFocusVolume* focusID = this->identificationManager->getVolumeFocusIdentification();
    Focus* focus = focusID->getFocus();
    FociFile* fociFile = focusID->getFociFile();
    
    FociPropertiesEditorDialog::editFocus(fociFile,
                                          focus,
                                          this->parentWidget);
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifySurfaceNodeSelected()
{    
    SelectionItemSurfaceNode* surfaceID = this->identificationManager->getSurfaceNodeIdentification();
    Brain* brain = surfaceID->getBrain();
    this->identificationManager->clearOtherSelectedItems(surfaceID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    Surface* surface = surfaceID->getSurface();
    const StructureEnum::Enum structure = surface->getStructure();
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItemNode(idMessage,
                                                        structure,
                                                        surface->getNumberOfNodes(),
                                                        surfaceID->getNodeNumber()));
    
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to display identication information on the surface border.
 */
void 
BrainOpenGLWidgetContextMenu::identifyVoxelSelected()
{
    SelectionItemVoxel* voxelID = this->identificationManager->getVoxelIdentification();
    Brain* brain = voxelID->getBrain();
    this->identificationManager->clearOtherSelectedItems(voxelID);
    const AString idMessage = this->identificationManager->getIdentificationText(brain);
    
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->addIdentifiedItem(new IdentifiedItem(idMessage));
    EventManager::get()->sendEvent(EventUpdateInformationWindows().getPointer());
}

/**
 * Called to remove all node identification symbols.
 */
void 
BrainOpenGLWidgetContextMenu::removeAllNodeIdentificationSymbolsSelected()
{
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    idManager->removeAllIdentifiedItems();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called to remove node identification symbol from node.
 */
void
BrainOpenGLWidgetContextMenu::removeNodeIdentificationSymbolSelected()
{
   SelectionItemSurfaceNodeIdentificationSymbol* idSymbol = identificationManager->getSurfaceNodeIdentificationSymbol();
    if (idSymbol->isValid()) {
        Surface* surface = idSymbol->getSurface();
        
        IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
        idManager->removeIdentifiedNodeItem(surface->getStructure(),
                                            surface->getNumberOfNodes(),
                                            idSymbol->getNodeNumber());

        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    }
}
/**
 * If any enabled connectivity files retrieve data from the network
 * and the number of nodes is large, warn the user since this will
 * be a very slow operation.
 *
 * @param clm
 *    The connectivity manager.
 * @param nodeIndices
 *    Indices of nodes that will have connectivity data retrieved.
 * @return
 *    true if process should continue, false if user cancels.
 */
bool
BrainOpenGLWidgetContextMenu::warnIfNetworkNodeCountIsLarge(const CiftiConnectivityMatrixDataFileManager* cmdf,
                                                            const std::vector<int32_t>& nodeIndices)
{
    const int32_t numNodes = static_cast<int32_t>(nodeIndices.size());
    if (numNodes < 200) {
        return true;
    }
    
    if (cmdf->hasNetworkFiles() == false) {
        return true;
    }
    
    const QString msg = ("There are "
                         + QString::number(numNodes)
                         + " vertices in the selected region.  Loading data from the network for "
                         + "this quantity of vertices may take a very long time.");
    const bool result = WuQMessageBox::warningYesNo(this,
                                                    "Do you want to continue?",
                                                    msg);
    return result;
}


/**
 * If any enabled connectivity files retrieve data from the network
 * and the number of nodes is large, warn the user since this will
 * be a very slow operation.
 *
 * @param chartingDataManager
 *    The charting data manager.
 * @param nodeIndices
 *    Indices of nodes that will have connectivity data retrieved.
 * @return 
 *    true if process should continue, false if user cancels.
 */
bool 
BrainOpenGLWidgetContextMenu::warnIfNetworkNodeCountIsLarge(const ChartingDataManager* chartingDataManager,
                                                            const std::vector<int32_t>& nodeIndices)
{
    const int32_t numNodes = static_cast<int32_t>(nodeIndices.size());
    if (numNodes < 200) {
        return true;
    }
    
    if (chartingDataManager->hasNetworkFiles() == false) {
        return true;
    }
    
    const QString msg = ("There are "
                         + QString::number(numNodes)
                         + " vertices in the selected region.  Loading data for "
                         + "this quantity of vertices may take a very long time.");
    const bool result = WuQMessageBox::warningYesNo(this,
                                                    "Do you want to continue?",
                                                    msg);
    return result;
}

/* ------------------------------------------------------------------------- */
/**
 * Constructor.
 */
BrainOpenGLWidgetContextMenu::ParcelConnectivity::ParcelConnectivity(const ParcelType parcelType,
                                                                     CaretMappableDataFile* mappableLabelFile,
                   const int32_t labelFileMapIndex,
                   const int32_t labelKey,
                   const QString& labelName,
                   Surface* surface,
                   const int32_t nodeNumber,
                   ChartingDataManager* chartingDataManager,
                   CiftiConnectivityMatrixDataFileManager* ciftiConnectivityManager,
                   CiftiFiberTrajectoryManager* ciftiFiberTrajectoryManager) {
    this->parcelType = parcelType;
    this->mappableLabelFile = mappableLabelFile;
    this->labelFileMapIndex = labelFileMapIndex;
    this->labelKey = labelKey;
    this->labelName = labelName;
    this->surface = surface;
    this->nodeNumber = nodeNumber;
    this->chartingDataManager = chartingDataManager;
    this->ciftiConnectivityManager = ciftiConnectivityManager;
    this->ciftiFiberTrajectoryManager = ciftiFiberTrajectoryManager;
}

/**
 * Destructor.
 */
BrainOpenGLWidgetContextMenu::ParcelConnectivity::~ParcelConnectivity()
{
    
}

/**
 * Get the indices inside the parcel.
 *
 * @param nodeIndicesOut
 *    Contains node indices upon exit.
 */
void
BrainOpenGLWidgetContextMenu::ParcelConnectivity::getNodeIndices(std::vector<int32_t>& nodeIndicesOut) const
{
    nodeIndicesOut.clear();
    
    if (this->parcelType != PARCEL_TYPE_SURFACE_NODES) {
        return;
    }
    
    CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mappableLabelFile);
    LabelFile* labelFile = dynamic_cast<LabelFile*>(mappableLabelFile);
    VolumeFile* volumeLabelFile = dynamic_cast<VolumeFile*>(mappableLabelFile);
    if (ciftiLabelFile != NULL) {
        ciftiLabelFile->getNodeIndicesWithLabelKey(surface->getStructure(),
                                                   surface->getNumberOfNodes(),
                                                   labelFileMapIndex,
                                                   labelKey,
                                                   nodeIndicesOut);
    }
    else if (labelFile != NULL) {
        labelFile->getNodeIndicesWithLabelKey(labelFileMapIndex,
                                              labelKey,
                                              nodeIndicesOut);
    }
    else if (volumeLabelFile != NULL) {
        /* nothing */
    }
    else {
        CaretAssertMessage(0,
                           "Should never get here, new or invalid label file type");
    }
    
}

