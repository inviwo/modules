/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <inviwo/computeshaderexamples/processors/computeshaderminmax.h>

#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <modules/opengl/glformats.h>
#include <modules/opengl/openglutils.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/buffer/buffergl.h>
#include <modules/opengl/buffer/bufferobject.h>
#include <modules/opengl/texture/texture3d.h>
#include <modules/opengl/texture/textureunit.h>

#include <modules/base/algorithm/dataminmax.h>

#pragma optimize("", off)

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ComputeShaderMinMax::processorInfo_{
    "org.inviwo.ComputeShaderMinMax",  // Class identifier
    "Compute Shader Min Max",          // Display name
    "Example",                         // Category
    CodeState::Experimental,           // Code state
    Tags::GL | Tag("Compute Shader"),  // Tags
    R"(Compute Shader example using map-reduce to determine the min/max values of the 
    input Volume.
    )"_unindentHelp,
};

const ProcessorInfo ComputeShaderMinMax::getProcessorInfo() const { return processorInfo_; }

ComputeShaderMinMax::ComputeShaderMinMax()
    : Processor{}
    , volume_{"volume", "Input volume"_help}
    , minValues_{"minValues", "Minima",
                 OrdinalPropertyState<vec4>{.value = vec4{0.0f},
                                            .min = vec4{-1.0f},
                                            .minConstraint = ConstraintBehavior::Ignore,
                                            .max = vec4{1.0f},
                                            .maxConstraint = ConstraintBehavior::Ignore,
                                            .invalidationLevel = InvalidationLevel::Valid,
                                            .semantics = PropertySemantics::Text,
                                            .help = "Minimum values of the input Volume"_help,
                                            .readOnly = ReadOnly::Yes}}
    , maxValues_{"maxValues", "Maxima",
                 OrdinalPropertyState<vec4>{.value = vec4{0.0f},
                                            .min = vec4{-1.0f},
                                            .minConstraint = ConstraintBehavior::Ignore,
                                            .max = vec4{1.0f},
                                            .maxConstraint = ConstraintBehavior::Ignore,
                                            .invalidationLevel = InvalidationLevel::Valid,
                                            .semantics = PropertySemantics::Text,
                                            .help = "Maximum values of the input Volume"_help,
                                            .readOnly = ReadOnly::Yes}}
    , logErrorOnly_{"logErrorOnly", "Log Only Errors", true}
    , shaderSampleVolume_{{{ShaderType::Compute, "minmaxvolume.comp"}}, Shader::Build::No}
    , shaderRecursive_{{{ShaderType::Compute, "minmaxlinear.comp"}}, Shader::Build::No} {

    addPorts(volume_);
    addProperties(minValues_, maxValues_, logErrorOnly_);

    shaderSampleVolume_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
    shaderRecursive_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
}

void ComputeShaderMinMax::initializeResources() {
    shaderSampleVolume_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_basic", ShaderObject::ExtensionBehavior::Require);
    shaderSampleVolume_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_arithmetic", ShaderObject::ExtensionBehavior::Require);
    shaderSampleVolume_.build();

    shaderRecursive_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_basic", ShaderObject::ExtensionBehavior::Require);
    shaderRecursive_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_arithmetic", ShaderObject::ExtensionBehavior::Require);
    shaderRecursive_.build();
}

void ComputeShaderMinMax::process() {
    const auto* volumeGL = volume_.getData()->getRepresentation<VolumeGL>();

    GLint invocations = 0;
    glGetIntegerv(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, &invocations);
    LogInfo("max invocations " << invocations);

    const uvec3 groupSize{32, 32, 1};
    const uvec3 dims{volumeGL->getDimensions()};
    // ignore z dimension since the compute shader iterates over all voxels in z direction
    const uvec3 numGroups{(uvec3{dims.x, dims.y, 1} + groupSize - uvec3{1}) / groupSize};

    // TODO: set image format specifier in shader to match internal format of the volume
    glBindImageTexture(0, volumeGL->getTexture()->getID(), 0, GL_TRUE, 0, GL_READ_ONLY,
                       GLFormats::get(volumeGL->getDataFormat()->getId()).internalFormat);

    // global buffer for min/max values for all work groups
    const size_t bufSize = 2 * glm::compMul(numGroups);
    BufferObject buf(bufSize * sizeof(ivec4), GLFormats::get(DataFormatId::Vec4Float32),
                     GL_DYNAMIC_READ, GL_SHADER_STORAGE_BUFFER);
    buf.bindBase(3);

    {
        utilgl::Activate activateShader(&shaderSampleVolume_);
        glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
    }
    const uint32_t arrayLen = glm::compMul(numGroups);
    ;
    {
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        utilgl::Activate activateShader(&shaderRecursive_);
        shaderRecursive_.setUniform("arrayLength", arrayLen);
        glDispatchCompute(1, 1, 1);
    }

    // download result
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    std::array minmaxGL{vec4{0}, vec4{0}};
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 2 * sizeof(glm::vec4), minmaxGL.data());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);

    // reference implementation
    auto [ramMin, ramMax] = util::volumeMinMax(volume_.getData().get(), IgnoreSpecialValues::No);

    const float deltaMin = std::abs(minmaxGL[0].x - static_cast<float>(ramMin.x));
    const float deltaMax = std::abs(minmaxGL[1].x - static_cast<float>(ramMax.x));
    const bool diffDetected = (deltaMin > std::numeric_limits<float>::epsilon() ||
                               deltaMax > std::numeric_limits<float>::epsilon());
    const auto loglevel = diffDetected ? inviwo::LogLevel::Warn : inviwo::LogLevel::Info;

    if (!logErrorOnly_ || diffDetected) {
        LogInfo(fmt::format("dispatch num groups: {} / dims: {}", numGroups, dims));
        LogInfo(fmt::format("group size: {}  / buf size: {}", groupSize, bufSize));

        LogSpecial(inviwo::LogCentral::getPtr(), loglevel,
                   fmt::format("GL  min/max: {} / {}", minmaxGL[0].x, minmaxGL[1].x));
        LogSpecial(inviwo::LogCentral::getPtr(), loglevel,
                   fmt::format("RAM min/max: {:f} / {:f}", ramMin.x, ramMax.x));
        if (diffDetected) {
            LogWarn(fmt::format("delta min/max: {:.8} / {:.8}", deltaMin, deltaMax));
        }
    }
    minValues_.set(minmaxGL[0]);
    maxValues_.set(minmaxGL[1]);
}

}  // namespace inviwo
