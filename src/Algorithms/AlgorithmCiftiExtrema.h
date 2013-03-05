#ifndef __ALGORITHM_CIFTI_EXTREMA_H__
#define __ALGORITHM_CIFTI_EXTREMA_H__

/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
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

#include "AbstractAlgorithm.h"
#include "CiftiInterface.h"

namespace caret {
    
    class AlgorithmCiftiExtrema : public AbstractAlgorithm
    {
        AlgorithmCiftiExtrema();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiExtrema(ProgressObject* myProgObj, const CiftiFile* myCifti, const float& surfDist, const float& volDist, const CiftiInterface::CiftiDirection& myDir, CiftiFile* myCiftiOut,
                              const SurfaceFile* myLeftSurf = NULL, const SurfaceFile* myRightSurf = NULL, const SurfaceFile* myCerebSurf = NULL, const float& surfPresmooth = -1.0f,
                              const float& volPresmooth = -1.0f, const bool& thresholdMode = false, const float& lowThresh = 0.0f, const float& highThresh = 0.0f,
                              const bool& sumMaps = false, const bool& consolidateMode = false, const bool& ignoreMinima = false, const bool& ignoreMaxima = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiExtrema> AutoAlgorithmCiftiExtrema;

}

#endif //__ALGORITHM_CIFTI_EXTREMA_H__
