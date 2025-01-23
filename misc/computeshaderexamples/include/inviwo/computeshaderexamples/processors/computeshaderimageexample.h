/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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

#include <modules/opengl/shader/shader.h>

namespace inviwo {

/** \docpage{org.inviwo.ComputeShaderImageExample, Compute Shader Image Example}
 * ![](org.inviwo.ComputeShaderImageExample.png?classIdentifier=org.inviwo.ComputeShaderImageExample)
 *
 * An processor to show how compute shaders can be utilized to create images. Uses shader
 * roll.comp to create a simple procedural image.
 * C++ and glsl source code is havily inspired by http://wili.cc/blog/opengl-cs.html
 *
 *
 * ### Outports
 *   * __outport__ The produced image.
 *
 * ### Properties
 *   * __Roll__ Used as offset in to the sin function in the shader to create an rolling effect.
 *
 */

class IVW_MODULE_COMPUTESHADEREXAMPLES_API ComputeShaderImageExample : public Processor {
public:
    ComputeShaderImageExample();
    virtual ~ComputeShaderImageExample() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    ImageOutport outport_;
    Shader shader_;

    FloatProperty roll_;
};

}  // namespace inviwo
