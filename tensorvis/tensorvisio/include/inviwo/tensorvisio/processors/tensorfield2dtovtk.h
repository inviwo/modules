/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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

#include <inviwo/tensorvisio/tensorvisiomoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/vtk/ports/vtkoutport.h>

#include <vtkSmartPointer.h>

class vtkStructuredGrid;

namespace inviwo {

/** \docpage{org.inviwo.TensorField2DToVTK, Tensor Field2DTo VTK}
 * ![](org.inviwo.TensorField2DToVTK.png?classIdentifier=org.inviwo.TensorField2DToVTK)
 * Export a 2D tensor field to VTK structured grid. The z dimension of the grid will always be 1
 * since it's a 2D field.
 *
 * ### Inports
 *   * tensorFieldInport_ 2D tensor field input.
 *
 * ### Outports
 *   * __vtkDataSetOutport___ VTK structured grid output.
 *
 */

/**
 * \brief Export a 2D tensor field to VTK structured grid.
 * This processor creates a VTK structured grid and memcpys the data from the input field to the VTK
 * data set.
 */
class IVW_MODULE_TENSORVISIO_API TensorField2DToVTK : public Processor {
public:
    TensorField2DToVTK();
    virtual ~TensorField2DToVTK() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    TensorField2DInport tensorFieldInport_;
    vtk::VtkOutport vtkDataSetOutport_;

    vtkSmartPointer<vtkStructuredGrid> structuredGrid_;
};

}  // namespace inviwo
