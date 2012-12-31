#ifndef __CONNECTIVITY_DENSE_VIEW_CONTROLLER__H_
#define __CONNECTIVITY_DENSE_VIEW_CONTROLLER__H_

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

#include <set>

#include <QObject>

class QAction;
class QCheckBox;
class QGridLayout;
class QLineEdit;
class QSpinBox;

namespace caret {

    class CiftiConnectivityMatrixDataFile;
    class WuQGridLayoutGroup;
    
    class ConnectivityDenseViewController : public QObject {
        
        Q_OBJECT

    public:
        ConnectivityDenseViewController(const Qt::Orientation orientation,
                                         QGridLayout* gridLayout,
                                         QObject* parent);
        
        virtual ~ConnectivityDenseViewController();
        
        void updateViewController(CiftiConnectivityMatrixDataFile* ciftiConnectivityMatrixFile);
        
        void setVisible(bool visible);
        
        static QGridLayout* createGridLayout(const Qt::Orientation orientation);
        
    private slots:
        void enabledCheckBoxStateChanged(int);

    private:
        ConnectivityDenseViewController(const ConnectivityDenseViewController&);

        ConnectivityDenseViewController& operator=(const ConnectivityDenseViewController&);
        
        void updateUserInterfaceAndGraphicsWindow();
        
        void updateOtherConnectivityDenseViewControllers();
        
        void updateViewController();
        
        CiftiConnectivityMatrixDataFile* m_ciftiConnectivityMatrixDataFile;
        
        QCheckBox* m_enabledCheckBox;
        
        QLineEdit* m_fileNameLineEdit;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        static std::set<ConnectivityDenseViewController*> s_allConnectivityDenseViewControllers;
    };
    
#ifdef __CONNECTIVITY_DENSE_VIEW_CONTROLLER_DECLARE__
    std::set<ConnectivityDenseViewController*> ConnectivityDenseViewController::s_allConnectivityDenseViewControllers;
#endif // __CONNECTIVITY_DENSE_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_DENSE_VIEW_CONTROLLER__H_
