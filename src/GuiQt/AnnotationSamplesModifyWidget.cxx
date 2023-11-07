
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#define __ANNOTATION_SAMPLES_MODIFY_WIDGET_DECLARE__
#include "AnnotationSamplesModifyWidget.h"
#undef __ANNOTATION_SAMPLES_MODIFY_WIDGET_DECLARE__

#include <QAction>
#include <QGridLayout>
#include <QLabel>
#include <QMenu>
#include <QToolButton>

#include "AnnotationManager.h"
#include "AnnotationPolyhedron.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "EventAnnotationGetBeingDrawnInWindow.h"
#include "EventBrowserTabGetAtWindowXY.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationSamplesModifyWidget
 * \brief Widget for redo and undo buttons
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *    The user input mode
 * @param browserWindowIndex
 *    The browser window index
 * @param parent
 *    The parent widget.
 */
AnnotationSamplesModifyWidget::AnnotationSamplesModifyWidget(const Qt::Orientation orientation,
                                                   const UserInputModeEnum::Enum userInputMode,
                                                   const int32_t browserWindowIndex,
                                                   QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* titleLabel = new QLabel("Modify");

    const AString lockToolTip("<html>"
                              "Lock the selected polyhedron.  Lock is enabled when there "
                              "is one and only one polyhedron selected.  "
                              "Once locked, no other "
                              "polyhedrons can be selected.  "
                              "This feature may be useful when polyhedrons overlap or are "
                              "very close to each other.  "
                              "Lock must be turned off "
                              "to deselect the locked polyhedron or select a different polyhedron.<br><br>"
                              "Lock is turned off by:"
                              "<ul>"
                              "<li> Clicking the <b>Lock</b> button"
                              "<li> Changing to a different Mode"
                              "<li> Cutting or Deleting the polyhedron"
                              "<li> Deselect All or Select All is selected from the Edit or Right-Click Menu"
                              "</ul>"
                              "</html>");
    m_lockAction = new QAction(this);
    m_lockAction->setCheckable(true);
    m_lockAction->setText("Lock");
    m_lockAction->setToolTip(lockToolTip);
    QObject::connect(m_lockAction, &QAction::toggled,
                     this, &AnnotationSamplesModifyWidget::lockActionToggled);
    
    QToolButton* lockToolButton = new QToolButton();
    lockToolButton->setDefaultAction(m_lockAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(lockToolButton);

    m_moreAction = new QAction(this);
    m_moreAction->setText("More...");
    m_moreAction->setToolTip("<html>Click to display menu containing more options</html>");
    QObject::connect(m_moreAction, &QAction::triggered,
                     this, &AnnotationSamplesModifyWidget::moreActionTriggered);
    m_moreToolButton = new QToolButton();
    m_moreToolButton->setDefaultAction(m_moreAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(m_moreToolButton);
    
    QGridLayout* gridLayout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(gridLayout, 2, 0);
    switch (orientation) {
        case Qt::Horizontal:
            gridLayout->addWidget(titleLabel,
                                  0, 0, 1, 2, Qt::AlignHCenter);
            gridLayout->addWidget(m_moreToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(lockToolButton,
                                  1, 1, Qt::AlignHCenter);
            break;
        case Qt::Vertical:
            gridLayout->addWidget(titleLabel,
                                  0, 0, Qt::AlignHCenter);
            gridLayout->addWidget(m_moreToolButton,
                                  1, 0, Qt::AlignHCenter);
            gridLayout->addWidget(lockToolButton,
                                  2, 0, Qt::AlignHCenter);
            break;
    }
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
}

/**
 * Destructor.
 */
AnnotationSamplesModifyWidget::~AnnotationSamplesModifyWidget()
{
}

/**
 * Update with the selected annotations.
 *
 * @param annotations
 *     The selected annotations
 */
void
AnnotationSamplesModifyWidget::updateContent(const std::vector<Annotation*>& annotations)
{
    m_polyhedronSelected  = NULL;
    
    EventAnnotationGetBeingDrawnInWindow annDrawEvent(m_userInputMode,
                                                      m_browserWindowIndex);
    EventManager::get()->sendEvent(annDrawEvent.getPointer());
    const bool drawingAnnotationFlag(annDrawEvent.isAnnotationDrawingInProgress());
    
    if ( ! drawingAnnotationFlag) {
        if (annotations.size() == 1) {
            CaretAssertVectorIndex(annotations, 0);
            CaretAssert(annotations[0]);
            m_polyhedronSelected = annotations[0]->castToPolyhedron();
        }
    }
    
    m_moreAction->setEnabled(false);
    m_lockAction->setEnabled(false);
    
    bool lockCheckedFlag(false);
    
    if (m_polyhedronSelected != NULL) {
        m_lockAction->setEnabled(true);
        m_moreAction->setEnabled(true);
            
        lockCheckedFlag = (m_polyhedronSelected
                           == Annotation::getSelectionLockedPolyhedronInWindow(m_browserWindowIndex));
    }
    QSignalBlocker blocker(m_lockAction);
    m_lockAction->setChecked(lockCheckedFlag);

    setEnabled(m_lockAction->isEnabled()
               || m_moreAction->isEnabled());
}

/**
 * Gets called when the redo action is triggered
 */
void
AnnotationSamplesModifyWidget::moreActionTriggered()
{
    EventBrowserTabGetAtWindowXY tabEvent(m_browserWindowIndex,
                                          Vector3D());
    EventManager::get()->sendEvent(tabEvent.getPointer());
    
    std::vector<std::shared_ptr<DrawingViewportContent>> drawingSlices(tabEvent.getSamplesResetExtentViewportContents());
    QMenu menu(m_moreToolButton);
    QAction* infoAction(menu.addAction("Info..."));
    QAction* resetSliceRangeAction(menu.addAction("Reset Slice Range..."));
    resetSliceRangeAction->setEnabled(drawingSlices.size() == 2);
    
    QAction* actionSelected(menu.exec(m_moreToolButton->mapToGlobal(QPoint(0, 0))));
    
    if (actionSelected == infoAction) {
        
    }
    else if (actionSelected == resetSliceRangeAction) {
        const AString msg("This operation will update the range of the selected sample "
                          "to match the selected volume slices.  Do you want to continue?");
        if (WuQMessageBox::warningOkCancel(m_moreToolButton,
                                           msg)) {
            CaretAssertVectorIndex(drawingSlices, 1);
            /*
             * The two slices are:
             * [0] First Slice Drawn in Montage that allows drawing
             * [1] Last Slice drawn in Montage that allows drawing
             */
            auto firstViewportContent(drawingSlices[0]);
            auto lastViewportContent(drawingSlices[1]);
            
            const DrawingViewportContentVolumeSlice& firstSlice(firstViewportContent->getVolumeSlice());
            const DrawingViewportContentVolumeSlice& lastSlice(lastViewportContent->getVolumeSlice());
            
            const Plane firstPlane(firstSlice.getPlane());
            const Plane lastPlane(lastSlice.getPlane());
            
            AnnotationManager* annotationManager(GuiManager::get()->getBrain()->getAnnotationManager(m_userInputMode));

            AString errorMessage;
            AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
            if (undoCommand->setModePolyhedronResetRangeToPlane(firstPlane,
                                                                lastPlane,
                                                                m_polyhedronSelected,
                                                                errorMessage)) {
                if ( ! annotationManager->applyCommand(undoCommand,
                                                       errorMessage)) {
                    WuQMessageBox::errorOk(m_moreToolButton,
                                           errorMessage);
                }
            }
            else {
                WuQMessageBox::errorOk(m_moreToolButton,
                                       errorMessage);
            }
        }
    }

    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the lock action is triggered
 */
void
AnnotationSamplesModifyWidget::lockActionToggled(bool checked)
{
    if (checked) {
        switch (m_userInputMode) {
            case UserInputModeEnum::Enum::ANNOTATIONS:
            case UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING:
            case UserInputModeEnum::Enum::BORDERS:
            case UserInputModeEnum::Enum::FOCI:
            case UserInputModeEnum::Enum::IMAGE:
            case UserInputModeEnum::Enum::INVALID:
            case UserInputModeEnum::Enum::VIEW:
            case UserInputModeEnum::Enum::VOLUME_EDIT:
                CaretAssert(0);
                break;
            case UserInputModeEnum::Enum::SAMPLES_EDITING:
            {
                if (m_polyhedronSelected != NULL) {
                    if (m_polyhedronSelected == Annotation::getSelectionLockedPolyhedronInWindow(m_browserWindowIndex)) {
                        Annotation::unlockPolyhedronInWindow(m_browserWindowIndex);
                    }
                    else {
                        Annotation::setSelectionLockedPolyhedronInWindow(m_browserWindowIndex,
                                                                         m_polyhedronSelected);
                    }
                }
            }
                break;
        }
    }
    else {
        Annotation::setSelectionLockedPolyhedronInWindow(m_browserWindowIndex, NULL);
    }
    EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}
