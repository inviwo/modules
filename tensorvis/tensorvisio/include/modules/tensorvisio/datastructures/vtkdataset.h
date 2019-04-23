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

#pragma once
#include <modules/tensorvisio/tensorvisiomoduledefine.h>

#include <inviwo/core/common/inviwo.h>
#include <utility>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkSmartPointer.h>
#include <vtkDataSet.h>
#include <warn/pop>

namespace inviwo {

/**
 * \brief Wrapper class for vtkDataSet pointer
 * Wrapper class for vtkDataSet pointer so we don't have to explicitly take care of memory handling
 * etc.
 */
class IVW_MODULE_TENSORVISIO_API VTKDataSet {
public:
    VTKDataSet() = delete;
    explicit VTKDataSet(vtkSmartPointer<vtkDataSet> dataSet) : dataSet_(dataSet) {}
    virtual ~VTKDataSet() {}

    vtkSmartPointer<vtkDataSet> operator->() const { return dataSet_; }
    vtkSmartPointer<vtkDataSet> operator*() const { return dataSet_; }

    std::string getDataInfo() const;

    /** Attempts to get the dimensions of the data set. This will only work if the data set
     * is of type rectiliniar grid, structured grid or structured points. Otherwise [0,0,0] will be
     * returned. The method is here because the GetDimensions() method is not declared const in VTK
     * so if you have a VTKDataSet on the inport you will not be able to determine its
     * dimensions unless you clone it...
     * */
    size3_t getDimensions() const;

private:
    vtkSmartPointer<vtkDataSet> dataSet_;
};

}  // namespace inviwo
