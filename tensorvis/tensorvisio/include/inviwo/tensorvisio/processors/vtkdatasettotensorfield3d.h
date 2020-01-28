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

#include <inviwo/tensorvisio/tensorvisiomoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/processors/activityindicator.h>

#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/vtk/ports/vtkdatasetport.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/buttonproperty.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataArrayAccessor.h>
#include <vtkAssume.h>
#include <warn/pop>

namespace inviwo {

/** \docpage{org.inviwo.VTKDataSetToTensorField3D, VTK Data Set To Tensor Field3D}
 * ![](org.inviwo.VTKDataSetToTensorField3D.png?classIdentifier=org.inviwo.VTKDataSetToTensorField3D)
 * Explanation of how to use the processor.
 *
 * ### Inports
 *   * __<Inport1>__ <description>.
 *
 * ### Outports
 *   * __<Outport1>__ <description>.
 *
 * ### Properties
 *   * __<Prop1>__ <description>.
 *   * __<Prop2>__ <description>
 */

/**
 * \brief VERY_BRIEFLY_DESCRIBE_THE_PROCESSOR
 * DESCRIBE_THE_PROCESSOR_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_TENSORVISIO_API VTKDataSetToTensorField3D : public Processor,
                                                             public ActivityIndicatorOwner {
public:
    VTKDataSetToTensorField3D();
    virtual ~VTKDataSetToTensorField3D() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    VTKDataSetInport dataSetInport_;

    TensorField3DOutport tensorField3DOutport_;

    BoolProperty normalizeExtents_;

    OptionPropertyString tensors_;
    OptionPropertyString scalars_;
    ButtonProperty generate_;

    bool busy_;

    void generate();

    struct VTKToVector {
        std::shared_ptr<std::vector<mat3>> vec;

        VTKToVector() : vec(std::make_shared<std::vector<mat3>>()) {}

        template <typename TensorArray>
        void operator()(TensorArray* tensors) {
            vec->clear();
            // This allows the compiler to optimize for the AOS array stride.
            const auto numComponents = tensors->GetNumberOfComponents();
            VTK_ASSUME(numComponents == 9);

            vtkDataArrayAccessor<TensorArray> t(tensors);

            const vtkIdType numTensors = tensors->GetNumberOfTuples();

            vec->reserve(numTensors * numComponents);

            for (vtkIdType tupleIdx = 0; tupleIdx < numTensors; ++tupleIdx) {
                // Get compiles to inlined optimizable raw memory accesses for
                // vtkGenericDataArray subclasses.
                vec->emplace_back(mat3(t.Get(tupleIdx, 0), t.Get(tupleIdx, 1), t.Get(tupleIdx, 2),
                                       t.Get(tupleIdx, 3), t.Get(tupleIdx, 4), t.Get(tupleIdx, 5),
                                       t.Get(tupleIdx, 6), t.Get(tupleIdx, 7), t.Get(tupleIdx, 8)));
            }
        }
    };
};

}  // namespace inviwo
