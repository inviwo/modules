/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/dataoutport.h>

#include <inviwo/tensorvisbase/ports/tensorfieldport.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorFieldSource, TensorFieldSource}
 * ![](org.inviwo.<name>.png?classIdentifier=org.inviwo.TensorFieldSource)
 */

class IVW_MODULE_TENSORVISBASE_API TensorFieldSlice : public Processor {
public:
    TensorFieldSlice();
    virtual ~TensorFieldSlice() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    TensorField3DInport inport_;
    TensorField2DOutport outport2D_;
    TensorField3DOutport outport3D_;

    MeshOutport sliceOutport_;
    MeshOutport planeOutport_;
    DataOutport<unsigned int> offsetOutport_;

    OptionProperty<CartesianCoordinateAxis> sliceAlongAxis_;
    OrdinalProperty<size_t> sliceNr_;
    FloatVec4Property sliceColor_;
    FloatVec4Property planeColor_;
};

}  // namespace inviwo
