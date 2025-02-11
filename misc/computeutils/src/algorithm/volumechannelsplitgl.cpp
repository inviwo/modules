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

#include <inviwo/computeutils/algorithm/volumechannelsplitgl.h>

#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumeutils.h>

#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>

namespace inviwo {

VolumeChannelSplitGL::VolumeChannelSplitGL()
    : shader_({{ShaderType::Compute, utilgl::findShaderResource("volumesplit.comp")}},
              Shader::Build::No) {}

std::vector<std::shared_ptr<Volume>> VolumeChannelSplitGL::split(
    std::shared_ptr<const Volume> volume) {
    std::vector<std::shared_ptr<Volume>> outVolumes;

    const auto oldDataFormat = volume->getDataFormat();

    const auto numericType = oldDataFormat->getNumericType();
    const auto numberOfComponents = 1;
    const auto precision = oldDataFormat->getPrecision();

    auto dataFormat = DataFormatBase::get(numericType, numberOfComponents, precision);

    const auto dimensions = volume->getDimensions();

    for (auto i{0}; i < 4; ++i) {
        shader_.getShaderObject(ShaderType::Compute)
            ->removeShaderDefine(StrBuffer("CHANNEL {}", i));
    }

    for (size_t i{0}; i < volume->getDataFormat()->getComponents(); ++i) {
        auto outVolume = std::make_shared<Volume>(
            volume->getDimensions(), dataFormat, swizzlemasks::rgba, volume->getInterpolation(),
            Wrapping3D{Wrapping::Clamp, Wrapping::Clamp, Wrapping::Clamp});

        outVolume->setModelMatrix(volume->getModelMatrix());
        outVolume->setWorldMatrix(volume->getWorldMatrix());
        outVolume->copyMetaDataFrom(*volume);

        outVolumes.push_back(outVolume);

        shader_.getShaderObject(ShaderType::Compute)
            ->removeShaderDefine(StrBuffer("CHANNEL {}", i - 1));
        shader_.getShaderObject(ShaderType::Compute)->addShaderDefine(StrBuffer("CHANNEL {}", i));
        shader_.build();

        shader_.activate();

        TextureUnitContainer cont;
        utilgl::bindAndSetUniforms(shader_, cont, *volume, "inputTexture");

        shader_.setUniform("outputTexture", 0);

        auto outVolumeGL = outVolume->getEditableRepresentation<VolumeGL>();

        glActiveTexture(GL_TEXTURE0);

        const auto texture = outVolumeGL->getTexture();
        const auto texHandle = texture->getID();
        glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                           texture->getInternalFormat());

        outVolumeGL->getTexture()->bind();

        glDispatchCompute(static_cast<GLuint>(dimensions.x), static_cast<GLuint>(dimensions.y),
                          static_cast<GLuint>(dimensions.z));

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        shader_.deactivate();
    }

    for (auto outVolume : outVolumes) {
        outVolume->dataMap_.dataRange = outVolume->dataMap_.valueRange =
            dvec2{volumeReductionGl_.reduce_v(outVolume, ReductionOperator::Min),
                  volumeReductionGl_.reduce_v(outVolume, ReductionOperator::Max)};
    }

    return outVolumes;
}

}  // namespace inviwo
