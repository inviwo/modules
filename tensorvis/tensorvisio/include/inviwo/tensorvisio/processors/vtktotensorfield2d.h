/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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
#include <inviwo/vtk/ports/vtkdatasetport.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/core/processors/activityindicator.h>
#include <inviwo/core/properties/buttonproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.VTKDataSetToTensorField2D, VTK Data Set To Tensor Field 2D}
 * ![](org.inviwo.VTKToTensorField2D.png?classIdentifier=org.inviwo.VTKDataSetToTensorField2D)
 *
 * Converts a VTK data set to a 2D tensor field.
 * This requires that one of the dimensions of the VTK data set is 1.
 * Additionally, the data type must be either float or double.
 * Lastly, unstructured grids are not supported.
 *
 *
 * ### Inports
 *   * __vtkDataSetInport__ VTK data set inport.
 *
 * ### Outports
 *   * __tensorField2DOutport__ Outputs a 2D tensor field.
 *
 */

class IVW_MODULE_TENSORVISIO_API VTKDataSetToTensorField2D : public Processor,
                                                             public ActivityIndicatorOwner {
public:
    VTKDataSetToTensorField2D();
    virtual ~VTKDataSetToTensorField2D() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VTKDataSetInport dataSetInport_;

    TensorField2DOutport tensorField2DOutport_;

    BoolProperty normalizeExtents_;

    OptionPropertyString tensors_;
    // OptionPropertyString scalars_;
    ButtonProperty generate_;

    void generate();

    bool busy_;
};

}  // namespace inviwo
