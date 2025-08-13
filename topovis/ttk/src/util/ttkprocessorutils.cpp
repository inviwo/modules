/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024-2025 Inviwo Foundation
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

#include <inviwo/vtk/processors/vtkgenericprocessor.h>

#include <ttk/vtk/ttkAlgorithm.h>
#include <vtkInformation.h>
#include <vtkAlgorithm.h>

namespace inviwo {

namespace ttk {

std::optional<std::string> getOutportDataType(vtkAlgorithm* filter, int portNumber) {
    std::optional<std::string> dataType;
    if (vtkInformation* info = filter->GetOutputPortInformation(portNumber)) {
        if (info->Has(ttkAlgorithm::SAME_DATA_TYPE_AS_INPUT_PORT())) {
            int inportNum = info->Get(ttkAlgorithm::SAME_DATA_TYPE_AS_INPUT_PORT());
            if (vtkInformation* inportInfo = filter->GetInputPortInformation(inportNum)) {
                if (inportInfo->Has(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE())) {
                    dataType = inportInfo->Get(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE());
                } else if (inportInfo->Has(vtkDataObject::DATA_TYPE_NAME())) {
                    dataType = inportInfo->Get(vtkDataObject::DATA_TYPE_NAME());
                }
            }
        } else if (info->Has(vtkDataObject::DATA_TYPE_NAME())) {
            dataType = info->Get(vtkDataObject::DATA_TYPE_NAME());
        }
    }
    return dataType;
}

}  // namespace ttk

}  // namespace inviwo
