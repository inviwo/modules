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

#include <inviwo/computeshaderexamples/processors/computeshaderimageexample.h>
#include <modules/opengl/openglmodule.h>
#include <modules/opengl/shader/shadermanager.h>

#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/datastructures/image/layer.h>
#include <modules/opengl/texture/texture2d.h>
#include <modules/opengl/image/layergl.h>

#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/shader/shaderutils.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ComputeShaderImageExample::processorInfo_{
    "org.inviwo.ComputeShaderImageExample",  // Class identifier
    "Compute Shader Image Example",          // Display name
    "Example",                               // Category
    CodeState::Experimental,                 // Code state
    Tags::GL,                                // Tags
    R"(A processor to show how compute shaders can be utilized to create images. 
    Uses shader `roll.comp` to create a simple procedural image.
 
    C++ and GSLS source code is heavily inspired by http://wili.cc/blog/opengl-cs.html)"_help,
};
const ProcessorInfo& ComputeShaderImageExample::getProcessorInfo() const { return processorInfo_; }

ComputeShaderImageExample::ComputeShaderImageExample()
    : Processor{}
    , outport_{"outport", "Generated output image"_help}
    , shader_{{{ShaderType::Compute, "roll.comp"}}, Shader::Build::No}
    , roll_{"roll", "Roll",
            util::ordinalLength(0.0f, 10.0f)
                .set("Used as offset in the sin function used in the shader to "
                     "create a rolling effect"_help)} {

    addPort(outport_);
    addProperty(roll_);

    shader_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
}

void ComputeShaderImageExample::initializeResources() { shader_.build(); }

void ComputeShaderImageExample::process() {
    // inspired by http://wili.cc/blog/opengl-cs.html

    glActiveTexture(GL_TEXTURE0);

    auto img = std::make_shared<Image>(size2_t{512, 512}, DataFormat<float>::get());

    auto layerGL = img->getColorLayer()->getEditableRepresentation<LayerGL>();
    auto texHandle = layerGL->getTexture()->getID();
    glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    layerGL->setSwizzleMask(swizzlemasks::luminance);

    shader_.activate();
    utilgl::setUniforms(shader_, roll_);

    layerGL->getTexture()->bind();

    shader_.setUniform("dest", 0);

    glDispatchCompute(512 / 16, 512 / 16, 1);  // 512^2 threads in blocks of 16^2

    shader_.deactivate();

    outport_.setData(img);
}
}  // namespace inviwo
