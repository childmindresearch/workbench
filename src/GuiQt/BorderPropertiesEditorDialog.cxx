
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QCheckBox>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QStringListModel>
#include <QToolButton>

#define __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__
#include "BorderPropertiesEditorDialog.h"
#undef __BORDER_PROPERTIES_EDITOR_DIALOG__DECLARE__

#include "Brain.h"
#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "GroupAndNameHierarchyModel.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiLabelTableEditor.h"
#include "GiftiLabelTableSelectionComboBox.h"
#include "GuiManager.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BorderPropertiesEditorDialog 
 * \brief Dialog that queries user to finish drawing of a border.
 *
 * This dialog is displayed when the user finishes drawing a 
 * border.  It allows the user to select the border file, 
 * enter the border name, select the color, enter the class,
 * set the type of border (open/closed), and
 * possibly other attributes of the border.
 */

/**
 * Create a new instance of the border properties editor for finishing
 * a border using a drawing mode.
 *
 * @param border
 *    Border that was drawn.
 * @param parent
 *    Parent on which this dialog is shown.
 * @return
 *    Dialog that will finish the border.
 *    Users MUST DELETE the returned dialog.
 */
BorderPropertiesEditorDialog*
BorderPropertiesEditorDialog::newInstanceFinishBorder(Border* border,
                                                      SurfaceFile* surfaceFile,
                                                      QWidget* parent)
{
    CaretAssert(surfaceFile);
    BorderPropertiesEditorDialog* dialog =
    new BorderPropertiesEditorDialog("Finish Border",
                                     surfaceFile,
                                     BorderPropertiesEditorDialog::MODE_FINISH_DRAWING,
                                     NULL,
                                     border,
                                     parent);
    return dialog;
}

/**
 * Create a new instance of the border properties editor for editing
 * a border properties.
 *
 * @param editModeBorderFile
 *    Border file containing the border that is being edited.
 * @param border
 *    Border that is to be edited.
 * @param parent
 *    Parent on which this dialog is shown.
 * @return
 *    Dialog that will finish the border.
 *    Users MUST DELETE the returned dialog.
 */
BorderPropertiesEditorDialog*
BorderPropertiesEditorDialog::newInstanceEditBorder(BorderFile* editModeBorderFile,
                                                    Border* border,
                                                    QWidget* parent)
{
    BorderPropertiesEditorDialog* dialog =
    new BorderPropertiesEditorDialog("Edit Border Properties",
                                     NULL,
                                     BorderPropertiesEditorDialog::MODE_EDIT,
                                     editModeBorderFile,
                                     border,
                                     parent);
    return dialog;
}

/**
 * Constructor.
 */
BorderPropertiesEditorDialog::BorderPropertiesEditorDialog(const QString& title,
                                                           SurfaceFile* finishBorderSurfaceFile,
                                                           Mode mode,
                                                           BorderFile* editModeBorderFile,
                                                           Border* border,
                                                           QWidget* parent)
: WuQDialogModal(title,
                 parent)
{
    CaretAssert(border);
    this->finishBorderSurfaceFile = finishBorderSurfaceFile;
    this->editModeBorderFile = editModeBorderFile;
    this->mode   = mode;
    this->border = border;
    this->classComboBox = NULL;
    
    QString borderName = border->getName();
    QString className = border->getClassName();
    switch (this->mode) {
        case MODE_EDIT:
            break;
        case MODE_FINISH_DRAWING:
            if (s_previousDataValid) {
                borderName = BorderPropertiesEditorDialog::previousName;
                className = BorderPropertiesEditorDialog::previousClassName;
            }
            break;
    }
    
    /*
     * File selection combo box
     */
    QLabel* borderFileLabel = new QLabel("Border File");
    this->borderFileSelectionComboBox = new QComboBox();
    WuQtUtilities::setToolTipAndStatusTip(this->borderFileSelectionComboBox, 
                                          "Selects an existing border file\n"
                                          "to which new borders are added.");
    QObject::connect(this->borderFileSelectionComboBox, SIGNAL(activated(int)),
                     this, SLOT(borderFileSelected()));
    QAction* newFileAction = WuQtUtilities::createAction("New",
                                                         "Create a new border file", 
                                                         this, 
                                                         this,
                                                         SLOT(newBorderFileButtonClicked()));
    QToolButton* newFileToolButton = new QToolButton();
    newFileToolButton->setDefaultAction(newFileAction);
    
    /*
     * Completer for name
     */
    m_nameCompleterStringListModel = new QStringListModel(this);
    
    /*
     * Name
     */
    QLabel* nameLabel = new QLabel("Name");
    this->nameComboBox = new GiftiLabelTableSelectionComboBox(this);
//    this->nameLineEdit->setText(borderName);
    QAction* displayNameColorEditorAction = WuQtUtilities::createAction("Add/Edit...",
                                                                    "Add and/or edit name colors",
                                                                    this,
                                                                    this,
                                                                    SLOT(displayNameEditor()));
    QToolButton* displayNameColorEditorToolButton = new QToolButton();
    displayNameColorEditorToolButton->setDefaultAction(displayNameColorEditorAction);
    
    /*
     * Class
     */
    QLabel* classLabel = new QLabel("Class");
    this->classComboBox = new GiftiLabelTableSelectionComboBox(this);
    WuQtUtilities::setToolTipAndStatusTip(this->classComboBox->getWidget(),
                                          "The class is used to group borders with similar\n"
                                          "characteristics.  Controls are available to\n"
                                          "display borders by their class attributes.");
    QAction* displayClassEditorAction = WuQtUtilities::createAction("Add/Edit...", 
                                                                    "Add and/or edit classes", 
                                                                    this, 
                                                                    this, 
                                                                    SLOT(displayClassEditor()));
    QToolButton* displayClassEditorToolButton = new QToolButton();
    displayClassEditorToolButton->setDefaultAction(displayClassEditorAction);
    
    /*
     * Closed
     */
    this->closedCheckBox = new QCheckBox("Closed Border");
    WuQtUtilities::setToolTipAndStatusTip(this->closedCheckBox, 
                                          "If checked, additional points will be added\n"
                                          "to the border so that the border forms a loop\n"
                                          "with the last point adjacent to the first point.");
    if (BorderPropertiesEditorDialog::previousClosedSelected) {
        this->closedCheckBox->setChecked(true);
    }
    else {
        this->closedCheckBox->setChecked(false);
    }
    
    /*
     * Reverse point order
     */
    this->reversePointOrderCheckBox = new QCheckBox("Reverse Point Order");
    WuQtUtilities::setToolTipAndStatusTip(this->reversePointOrderCheckBox, 
                                          "If checked, the order of the points in the \n"
                                          "border are reversed when the OK button is pressed.");
    
    /*
     * Layout widgets
     */
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = 0;
    gridLayout->addWidget(borderFileLabel, row, 0);
    gridLayout->addWidget(this->borderFileSelectionComboBox, row, 1);
    gridLayout->addWidget(newFileToolButton, row, 2);
    row++;
    gridLayout->addWidget(nameLabel, row, 0);
    gridLayout->addWidget(this->nameComboBox->getWidget(), row, 1);
    gridLayout->addWidget(displayNameColorEditorToolButton, row, 2);
    row++;
    gridLayout->addWidget(classLabel, row, 0);
    gridLayout->addWidget(this->classComboBox->getWidget(), row, 1);
    gridLayout->addWidget(displayClassEditorToolButton, row, 2);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(this->closedCheckBox, row, 0, 1, 3, Qt::AlignLeft);
    row++;
    gridLayout->addWidget(this->reversePointOrderCheckBox, row, 0, 1, 3, Qt::AlignLeft);
    
    /*
     * Show/Hide options based upon mode
     */
    bool showFileOptionFlag   = false;
    bool showClosedOptionFlag = false;
    bool showReverseOptionFlag = false;
    switch (this->mode) {
        case MODE_EDIT:
            showReverseOptionFlag = true;
            break;
        case MODE_FINISH_DRAWING:
            showFileOptionFlag = true;
            showClosedOptionFlag = true;
            break;
    }
    
    borderFileLabel->setVisible(showFileOptionFlag);
    this->borderFileSelectionComboBox->setVisible(showFileOptionFlag);
    newFileToolButton->setVisible(showFileOptionFlag);
    this->closedCheckBox->setVisible(showClosedOptionFlag);
    this->reversePointOrderCheckBox->setVisible(showReverseOptionFlag);
    
    this->loadBorderFileComboBox();
    this->loadNameComboBox(borderName);
    this->loadClassComboBox(className);
    
    /*
     * Set the widget for the dialog.
     */
    this->setCentralWidget(widget);
}

/**
 * Destructor.
 */
BorderPropertiesEditorDialog::~BorderPropertiesEditorDialog()
{
    
}

/**
 * Get the selected border file.
 * @return BorderFile or NULL if no border file.
 */
BorderFile* 
BorderPropertiesEditorDialog::getSelectedBorderFile()
{
    if (this->editModeBorderFile != NULL) {
        return this->editModeBorderFile;
    }
    
    const int fileComboBoxIndex = this->borderFileSelectionComboBox->currentIndex();
    void* filePointer = this->borderFileSelectionComboBox->itemData(fileComboBoxIndex).value<void*>();
    BorderFile* borderFile = (BorderFile*)filePointer;
    BorderPropertiesEditorDialog::previousBorderFile = borderFile;
    
    return borderFile;
}

/**
 * Load the border files into the border file combo box.
 */
void 
BorderPropertiesEditorDialog::loadBorderFileComboBox()
{
    Brain* brain = GuiManager::get()->getBrain();
    const int32_t numBorderFiles = brain->getNumberOfBorderFiles();
    this->borderFileSelectionComboBox->clear();
    
    int defaultFileComboIndex = 0;
    for (int32_t i = 0; i < numBorderFiles; i++) {
        BorderFile* borderFile = brain->getBorderFile(i);
        const AString name = borderFile->getFileNameNoPath();
        this->borderFileSelectionComboBox->addItem(name,
                                                   qVariantFromValue((void*)borderFile));
        if (BorderPropertiesEditorDialog::previousBorderFile == borderFile) {
            defaultFileComboIndex = this->borderFileSelectionComboBox->count() - 1;
        }
    }
    this->borderFileSelectionComboBox->setCurrentIndex(defaultFileComboIndex);

    const BorderFile* borderFile = getSelectedBorderFile();
    if (borderFile != NULL) {
        m_nameCompleterStringList = borderFile->getAllBorderNamesSorted();
        m_nameCompleterStringListModel->setStringList(m_nameCompleterStringList);
    }
}

/**
 * Called to create a new border file.
 */
void 
BorderPropertiesEditorDialog::newBorderFileButtonClicked()
{
    /*
     * Create a new border file that will have proper path
     */
    Brain* brain = GuiManager::get()->getBrain();
    BorderFile* newBorderFile = brain->addBorderFile();
    
    /*
     * Let user choose a different path/name
     */
    AString newBorderFileName = CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::BORDER,
                                                                      this,
                                                                      "Choose Border File Name",
                                                                      newBorderFile->getFileName());
    /*
     * If user cancels, delete the new border file and return
     */
    if (newBorderFileName.isEmpty()) {
        brain->removeDataFile(newBorderFile);
        return;
    }
    
    /*
     * Set name of new scene file
     */
    newBorderFile->setFileName(newBorderFileName);
    BorderPropertiesEditorDialog::previousBorderFile = newBorderFile;
    this->loadBorderFileComboBox();
    this->borderFileSelected();
    
    
//    const QString fileExtension = DataFileTypeEnum::toFileExtension(DataFileTypeEnum::BORDER);
//    QString newFileName = ("NewFile." 
//                           + fileExtension);
//    
//    WuQDataEntryDialog newFileDialog("New Border File",
//                                        this);
//    QLineEdit* newFileNameLineEdit = newFileDialog.addLineEditWidget("New Border File Name", 
//                                                                        newFileName);
//    
//    if (newFileDialog.exec() == WuQDataEntryDialog::Accepted) {
//        QString borderFileName   = newFileNameLineEdit->text();
//        
//        try {
//            if (borderFileName.endsWith(fileExtension) == false) {
//                borderFileName += ("."
//                                + fileExtension);
//            }
//            
//            BorderFile* borderFile = GuiManager::get()->getBrain()->addBorderFile();
//            borderFile->setFileName(borderFileName);
//            
//            BorderPropertiesEditorDialog::previousBorderFile = borderFile;
//            this->loadBorderFileComboBox();
//            this->borderFileSelected();
//        }
//        catch (const DataFileException& dfe) {
//            WuQMessageBox::errorOk(this, 
//                                   dfe.whatString());
//        }
//    }
}


/**
 * Called when a border file is selected.
 */
void 
BorderPropertiesEditorDialog::borderFileSelected()
{
    if (this->classComboBox != NULL) {
        this->loadNameComboBox();
        this->loadClassComboBox();
    }
}

/**
 * Load the class combo box.
 */
void 
BorderPropertiesEditorDialog::loadClassComboBox(const QString& className)
{
    BorderFile* borderFile = this->getSelectedBorderFile();
    if (borderFile != NULL) {
        this->classComboBox->updateContent(borderFile->getClassColorTable());
    }
    
    if (className.isEmpty() == false) {
        this->classComboBox->setSelectedLabelName(className);
    }
}

/**
 * Load the name combo box.
 */
void
BorderPropertiesEditorDialog::loadNameComboBox(const QString& name)
{
    BorderFile* borderFile = this->getSelectedBorderFile();
    if (borderFile != NULL) {
        this->nameComboBox->updateContent(borderFile->getNameColorTable());
    }
    
    if (name.isEmpty() == false) {
        this->nameComboBox->setSelectedLabelName(name);
    }
}

/**
 * Called when the OK button is pressed.
 */
void 
BorderPropertiesEditorDialog::okButtonClicked()
{
    AString errorMessage;

    /*
     * Get border file.
     */
    BorderFile* borderFile = this->getSelectedBorderFile();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this, 
                               "Border file is not valid, use the New button to create a border file.");
        return;
    }
    
    /*
     * Get data entered by the user.
     */
    const AString name = this->nameComboBox->getSelectedLabelName();
    if (name.isEmpty()) {
        errorMessage += ("Name is invalid.\n");
    }
    const QString className = this->classComboBox->getSelectedLabelName();
    
    /*
     * Error?
     */
    if (errorMessage.isEmpty() == false) {
            WuQMessageBox::errorOk(this, 
                                   errorMessage);
        return;
    }
    
    
    Border* borderBeingEdited = NULL;
    bool finishModeFlag = false;
    switch (this->mode) {
        case MODE_EDIT:
            borderBeingEdited = this->border;
            break;
        case MODE_FINISH_DRAWING:
            borderBeingEdited = new Border(*this->border);
            finishModeFlag = true;
            break;
    }
    
    /*
     * Make a copy of the border being drawn
     */
    borderBeingEdited->setName(name);
    borderBeingEdited->setClassName(className);
    
    if (finishModeFlag) {
        /*
         * Close border
         */
        if (this->closedCheckBox->isChecked()) {
            borderBeingEdited->addPointsToCloseBorderWithGeodesic(this->finishBorderSurfaceFile);    
        }
        
        /*
         * Add border to border file
         */
        CaretAssert(borderFile);
        borderFile->addBorder(borderBeingEdited);
        
        /*
         * Save values entered by the user and
         * use them to initialize the dialog next
         * time it is displayed.
         */
        s_previousDataValid = true;
        BorderPropertiesEditorDialog::previousName = name;
        BorderPropertiesEditorDialog::previousClassName = className;
        BorderPropertiesEditorDialog::previousClosedSelected = this->closedCheckBox->isChecked();
        BorderPropertiesEditorDialog::previousBorderFile = borderFile;
    }
    else {
        if (this->reversePointOrderCheckBox->isChecked()) {
            this->border->reverse();
        }
    }
    
    if (m_nameCompleterStringList.contains(name) == false) {
        m_nameCompleterStringList.append(name);
        m_nameCompleterStringList.sort();
        m_nameCompleterStringListModel->setStringList(m_nameCompleterStringList);
    }
    
    /*
     * continue with OK button processing
     */
    WuQDialogModal::okButtonClicked();
}

/**
 * Display the class editor
 */
void 
BorderPropertiesEditorDialog::displayClassEditor()
{
    BorderFile* borderFile = this->getSelectedBorderFile();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this, 
                               "Border file is not valid, use the New button to create a border file.");
        return;
    }
    
    GiftiLabelTableEditor editor(borderFile,
                                 borderFile->getClassColorTable(),
                                 "Edit Class Attributes",
                                 this);
    const QString className = this->classComboBox->getSelectedLabelName();
    if (className.isEmpty() == false) {
        if (this->mode == MODE_EDIT) {
            editor.selectLabelWithName(className);
        }
    }
    editor.exec();
    
    this->loadClassComboBox();

    const QString selectedClassName = editor.getLastSelectedLabelName();
    if (selectedClassName.isEmpty() == false) {
        this->classComboBox->setSelectedLabelName(selectedClassName);
    }
}

/**
 * Display the name editor
 */
void
BorderPropertiesEditorDialog::displayNameEditor()
{
    BorderFile* borderFile = this->getSelectedBorderFile();
    if (borderFile == NULL) {
        WuQMessageBox::errorOk(this,
                               "Border file is not valid, use the New button to create a border file.");
        return;
    }
    
    GiftiLabelTableEditor editor(borderFile,
                                 borderFile->getNameColorTable(),
                                 "Edit Class Attributes",
                                 this);
    const QString name = this->nameComboBox->getSelectedLabelName();
    if (name.isEmpty() == false) {
        const GiftiLabel* label = borderFile->getNameColorTable()->getLabelBestMatching(name);
        if (label != NULL) {
            editor.selectLabelWithName(label->getName());
        }
    }
    editor.exec();
    
    this->loadNameComboBox();
    
    const QString selectedName = editor.getLastSelectedLabelName();
    if (selectedName.isEmpty() == false) {
        this->nameComboBox->setSelectedLabelName(selectedName);
    }
}




