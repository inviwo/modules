/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024-2026 Inviwo Foundation
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
#include <modules/opengl/openglcapabilities.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/buffer/buffergl.h>
#include <modules/opengl/buffer/bufferobject.h>
#include <modules/opengl/texture/texture3d.h>
#include <modules/opengl/texture/textureunit.h>

#include <modules/base/algorithm/dataminmax.h>

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

const ProcessorInfo& ComputeShaderMinMax::getProcessorInfo() const { return processorInfo_; }

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
    , shaderLinear_{{{ShaderType::Compute, "minmaxlinear.comp"}}, Shader::Build::No} {

    addPorts(volume_);
    addProperties(minValues_, maxValues_, logErrorOnly_);

    shaderSampleVolume_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
    shaderLinear_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });
}

void ComputeShaderMinMax::initializeResources() {
    shaderSampleVolume_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_basic", ShaderObject::ExtensionBehavior::Require);
    shaderSampleVolume_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_arithmetic", ShaderObject::ExtensionBehavior::Require);
    shaderSampleVolume_.getComputeShaderObject()->addShaderDefine("IMAGE_FORMAT", "r32f");
    shaderSampleVolume_.build();

    shaderLinear_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_basic", ShaderObject::ExtensionBehavior::Require);
    shaderLinear_.getComputeShaderObject()->addShaderExtension(
        "GL_KHR_shader_subgroup_arithmetic", ShaderObject::ExtensionBehavior::Require);
    shaderLinear_.build();
}

namespace detail {

std::string_view getSamplerPrefix(const GLFormat& glFormat) {
    if (glFormat.normalization != utilgl::Normalization::None) {
        return {};
    }
    const auto* format = DataFormatBase::get(GLFormats::get(glFormat));
    using namespace std::string_view_literals;
    switch (format->getNumericType()) {
        case NumericType::UnsignedInteger:
            return "u"sv;
        case NumericType::SignedInteger:
            return "i"sv;
        default:
            return {};
    }
}

std::string_view getImagePrefix(const GLFormat& glFormat) {
    using namespace std::string_view_literals;
    if (glFormat.layoutQualifier.ends_with("ui")) {
        return "u"sv;
    } else if (glFormat.layoutQualifier.ends_with("i")) {
        return "i"sv;
    } else {
        return {};
    }
}

}  // namespace detail

void ComputeShaderMinMax::process() {
    const auto* volumeGL = volume_.getData()->getRepresentation<VolumeGL>();

    const uvec3 groupSize{32, 32, 1};
    const uvec3 dims{volumeGL->getDimensions()};
    // ignore z dimension since the compute shader iterates over all voxels in z direction
    const uvec3 numGroups{(uvec3{dims.x, dims.y, 1} + groupSize - uvec3{1}) / groupSize};

    const auto& glFormat = GLFormats::get(volumeGL->getDataFormat()->getId());
    const bool useImageLoadStore = (glFormat.channels != 3);

    {
        // Note: inefficient to recompile the shader every frame
        auto* computeShader = shaderSampleVolume_.getComputeShaderObject();

        StrBuffer buf;
        if (useImageLoadStore) {
            // use image and imageLoad()
            computeShader->addShaderDefine("IMAGE_FORMAT", glFormat.layoutQualifier);
            computeShader->removeShaderDefine("USE_IMAGE_SAMPLER");
            buf.append("{}image{}D", detail::getImagePrefix(glFormat), 3);
        } else {
            // use regular texture sampler since imageLoad() does not support RGB formats
            computeShader->addShaderDefine("USE_IMAGE_SAMPLER");
            buf.append("{}sampler{}D", detail::getSamplerPrefix(glFormat), 3);
        }
        computeShader->addShaderDefine("IMAGE_SOURCE", buf);

        shaderSampleVolume_.build();
    }

    if (useImageLoadStore) {
        glBindImageTexture(0, volumeGL->getTexture()->getID(), 0, GL_TRUE, 0, GL_READ_ONLY,
                           glFormat.internalFormat);
    }

    // global buffer for min/max values for all work groups
    const size_t bufSize = 2 * glm::compMul(numGroups);
    const BufferObject buf(bufSize * sizeof(ivec4), GLFormats::get(DataFormatId::Vec4Float32),
                           GL_DYNAMIC_READ, GL_SHADER_STORAGE_BUFFER);
    buf.bindBase(3);

    {
        const utilgl::Activate activateShader(&shaderSampleVolume_);

        if (!useImageLoadStore) {
            const TextureUnit texUnit;
            texUnit.activate();
            volumeGL->getTexture()->bind();
            shaderSampleVolume_.setUniform("volume", texUnit.getUnitNumber());
            TextureUnit::setZeroUnit();
        }

        glDispatchCompute(numGroups.x, numGroups.y, numGroups.z);
    }
    const uint32_t arrayLen = glm::compMul(numGroups);
    {
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        const utilgl::Activate activateShader(&shaderLinear_);
        shaderLinear_.setUniform("arrayLength", arrayLen);
        glDispatchCompute(1, 1, 1);
    }

    // download result
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    std::array minmaxGL{vec4{0}, vec4{0}};
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 2 * sizeof(glm::vec4), minmaxGL.data());

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, 0);

    if (volumeGL->getDataFormat()->getComponents() < 4) {
        // reset alpha channel since imageLoad in the shader returns 1.0 for non-rgba formats
        minmaxGL[0].a = 0.0;
        minmaxGL[1].a = 0.0;
    }

    // undo GL format normalization
    if (glFormat.normalization == utilgl::Normalization::Normalized) {
        const DataMapper dataMapper{DataFormatBase::get(GLFormats::get(glFormat))};

        minmaxGL[0] = dataMapper.mapFromNormalizedToData(minmaxGL[0]);
        minmaxGL[1] = dataMapper.mapFromNormalizedToData(minmaxGL[1]);
    } else if (glFormat.normalization == utilgl::Normalization::SignNormalized) {
        using enum DataMapper::SignedNormalization;
        const DataMapper dataMapper{
            DataFormatBase::get(GLFormats::get(glFormat)),
            OpenGLCapabilities::isSignedIntNormalizationSymmetric() ? Symmetric : Asymmetric};

        minmaxGL[0] = dataMapper.mapFromSignNormalizedToData(minmaxGL[0]);
        minmaxGL[1] = dataMapper.mapFromSignNormalizedToData(minmaxGL[1]);
    }

    // reference implementation
    auto [refMin, refMax] = util::volumeMinMax(volume_.getData().get(), IgnoreSpecialValues::No);

    auto checkResult = [&](const dvec4& minTest, const dvec4& maxTest, std::string_view name) {
        const double deltaMin = std::abs(refMin.x - minTest.x);
        const double deltaMax = std::abs(refMax.x - maxTest.x);
        const bool diffDetected = (deltaMin > std::numeric_limits<double>::epsilon() ||
                                   deltaMax > std::numeric_limits<double>::epsilon());
        const auto loglevel = diffDetected ? inviwo::LogLevel::Warn : inviwo::LogLevel::Info;

        if (!logErrorOnly_ || diffDetected) {
            LogSpecial(inviwo::LogCentral::getPtr(), loglevel,
                       fmt::format("{} min/max: {:.8} / {:.8}", name, minTest.x, maxTest.x));
            LogSpecial(inviwo::LogCentral::getPtr(), loglevel,
                       fmt::format("ref min/max:     {:.8} / {:.8}", refMin.x, refMax.x));
            if (diffDetected) {
                LogInfo(fmt::format("delta min/max: {:.8} / {:.8}", deltaMin, deltaMax));
            }
        }
    };

    checkResult(minmaxGL[0], minmaxGL[1], "GL");

    minValues_.set(minmaxGL[0]);
    maxValues_.set(minmaxGL[1]);
}

}  // namespace inviwo
