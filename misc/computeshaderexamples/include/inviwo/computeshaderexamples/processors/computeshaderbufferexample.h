/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2024 Inviwo Foundation
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

#include <inviwo/computeshaderexamples/computeshaderexamplesmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/meshport.h>
#include <modules/opengl/shader/shader.h>

namespace inviwo {

/** \docpage{org.inviwo.ComputeShaderBufferExample, Compute Shader Buffer Example}
 * ![](org.inviwo.ComputeShaderBufferExample.png?classIdentifier=org.inviwo.ComputeShaderBufferExample)
 *
 * An processor to show how compute shaders can be utilized to create vertex buffers. Uses shader
 * spiral.comp to create a simple spiral.
 *
 *
 * ### Outports
 *   * __mesh__ Mesh containing the created vertex buffers.
 *
 * ### Properties
 *   * __Number of points__ Number of points used to represent the spiral.
 *   * __Radius__ The radius of the spiral.
 *   * __Rotations__ How many rotations the spiral will have.
 *   * __Height__ Hight difference between start and end point of the spiral.
 *   * __Color Mapping__ Used to colorize the spiral.
 *
 */

class IVW_MODULE_COMPUTESHADEREXAMPLES_API ComputeShaderBufferExample : public Processor {
public:
    ComputeShaderBufferExample();
    virtual ~ComputeShaderBufferExample() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    MeshOutport mesh_;

    Shader shader_;

    IntProperty numPoints_;
    FloatProperty radius_;
    FloatProperty rotations_;
    FloatProperty height_;

    TransferFunctionProperty tf_;
};

}  // namespace inviwo
