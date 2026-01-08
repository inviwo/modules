/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2026 Inviwo Foundation
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
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/layerport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>
#include <modules/opengl/shader/shader.h>
#include <modules/opengl/buffer/framebufferobject.h>

namespace inviwo {

class Layer;

class IVW_MODULE_TENSORVISBASE_API TensorFieldLIC : public Processor {
public:
    TensorFieldLIC();
    virtual ~TensorFieldLIC() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    enum class Sizing { SameAsInput, ScalingFactor, Custom };

    TensorField2DInport inport_;
    LayerInport noiseTexture_;
    LayerOutport outport_;

    CompositeProperty dimensions_;
    OptionProperty<Sizing> sizing_;
    IntSize2Property inputDimensions_;
    IntSize2Property outputDimensions_;
    FloatProperty scaling_;

    IntProperty samples_;
    FloatProperty stepLength_;
    BoolProperty normalizeVectors_;
    BoolProperty useRK4_;
    BoolProperty majorMinor_;
    FloatVec4Property backgroundColor_;

    BoolCompositeProperty postProcessing_;
    BoolProperty intensityMapping_;
    FloatProperty brightness_;
    FloatProperty contrast_;
    FloatProperty gamma_;

    Shader shader_;
    Image tf_texture_;

    float minVal_;
    float maxVal_;
    float eigenValueRange_;

    void updateEigenValues();

    LayerConfig config_;
    std::vector<std::pair<FrameBufferObject, std::shared_ptr<Layer>>> cache_;
};

}  // namespace inviwo
