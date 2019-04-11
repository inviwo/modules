/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/tensorvisio/datastructures/vtkdataset.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkStructuredPoints.h>
#include <warn/pop>

namespace inviwo {

size3_t VTKDataSet::getDimensions() const {
    int* dimsptr{nullptr};

    switch (dataSet_->GetDataObjectType()) {
        case VTK_RECTILINEAR_GRID:
            dimsptr = dynamic_cast<vtkRectilinearGrid*>(dataSet_.GetPointer())->GetDimensions();
            break;
        case VTK_STRUCTURED_GRID:
            dimsptr = dynamic_cast<vtkStructuredGrid*>(dataSet_.GetPointer())->GetDimensions();
            break;
        case VTK_STRUCTURED_POINTS:
            dimsptr = dynamic_cast<vtkStructuredPoints*>(dataSet_.GetPointer())->GetDimensions();
            break;
        default:
            break;
    }

    if (!dimsptr)
        return size3_t{0};
    else
        return size3_t{dimsptr[0], dimsptr[1], dimsptr[2]};
}
}  // namespace inviwo
