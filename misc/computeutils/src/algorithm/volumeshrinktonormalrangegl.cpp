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

#include <inviwo/computeutils/algorithm/volumeshrinktonormalrangegl.h>

#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumeutils.h>
#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/texture/texture3d.h>

#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

namespace inviwo {

VolumeShrinkToNormalRangeGL::VolumeShrinkToNormalRangeGL()
    : shader_({{ShaderType::Compute, utilgl::findShaderResource("volumeshrinktonormalrange.comp")}},
              Shader::Build::No)
    , needsCompilation_(false) {
    shader_.getShaderObject(ShaderType::Compute)->addShaderDefine("SHRINK_CHANNEL_0");
}

void VolumeShrinkToNormalRangeGL::setShrinkChannel(const size_t channel, const bool shrink) {
    needsCompilation_ = true;

    if (shrink) {
        shader_.getShaderObject(ShaderType::Compute)
            ->addShaderDefine(StrBuffer{"SHRINK_CHANNEL_{}", channel});
    } else {
        shader_.getShaderObject(ShaderType::Compute)
            ->removeShaderDefine(StrBuffer{"SHRINK_CHANNEL_{}", channel});
    }
}

void VolumeShrinkToNormalRangeGL::setShrinkChannels(bvec4 normalize) {
    setShrinkChannel(0, normalize[0]);
    setShrinkChannel(1, normalize[1]);
    setShrinkChannel(2, normalize[2]);
    setShrinkChannel(3, normalize[3]);
}

void VolumeShrinkToNormalRangeGL::reset() { setShrinkChannels({true, false, false, false}); }

std::shared_ptr<Volume> VolumeShrinkToNormalRangeGL::shrink(const Volume& volume) {
    std::shared_ptr<Volume> outVolume;

    const auto offset =
        (volume.dataMap.dataRange.x < 0.0 && volume.dataMap.dataRange.y > 0.0)
            ? volume.dataMap.dataRange.x / (volume.dataMap.dataRange.y - volume.dataMap.dataRange.x)
            : 0.0;

    // Don't dispatch if we don't have to
    if (volume.getDataFormat()->getNumericType() == NumericType::Float) {
        outVolume = std::make_shared<Volume>(volume.getDimensions(), volume.getDataFormat(),
                                             volume.getSwizzleMask(), volume.getInterpolation(),
                                             volume.getWrapping());
    } else {
        outVolume = volume.getRepresentation<VolumeRAM>()
                        ->dispatch<std::shared_ptr<Volume>, dispatching::filter::Integers>(
                            [](auto vrprecision) {
                                using ValueType = util::PrecisionValueType<decltype(vrprecision)>;

                                using P = typename util::same_extent<ValueType, float>::type;

                                return std::make_shared<Volume>(
                                    vrprecision->getDimensions(), DataFormat<P>::get(),
                                    vrprecision->getSwizzleMask(), vrprecision->getInterpolation(),
                                    vrprecision->getWrapping());
                            });
    }

    outVolume->setModelMatrix(volume.getModelMatrix());
    outVolume->setWorldMatrix(volume.getWorldMatrix());
    outVolume->copyMetaDataFrom(volume);

    if (needsCompilation_ || !shader_.isReady()) {
        needsCompilation_ = false;
        shader_.build();
    }

    const auto dimensions = volume.getDimensions();

    shader_.activate();

    TextureUnitContainer cont;
    utilgl::bindAndSetUniforms(shader_, cont, volume, "inputTexture");
    shader_.setUniform("outputTexture", 0);
    shader_.setUniform("offset", static_cast<float>(offset));

    auto outVolumeGL = outVolume->getEditableRepresentation<VolumeGL>();
    outVolumeGL->setWrapping({Wrapping::Clamp, Wrapping::Clamp, Wrapping::Clamp});

    glActiveTexture(GL_TEXTURE0);

    const auto texture = outVolumeGL->getTexture();
    const auto texHandle = texture->getID();
    glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, texture->getInternalFormat());

    outVolumeGL->setSwizzleMask(swizzlemasks::rgba);

    outVolumeGL->getTexture()->bind();

    /*
     * Run normalization.
     */
    glDispatchCompute(static_cast<GLuint>(dimensions.x), static_cast<GLuint>(dimensions.y),
                      static_cast<GLuint>(dimensions.z));

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    shader_.deactivate();

    outVolume->dataMap.dataRange = outVolume->dataMap.valueRange = dvec2{0 + offset, 1 + offset};

    return outVolume;
}

}  // namespace inviwo
