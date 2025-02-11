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

#include <inviwo/computeutils/algorithm/volumereductiongl.h>
#include <inviwo/core/util/stringconversion.h>

#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumeutils.h>
#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/texture/texture3d.h>

#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

#include <fmt/base.h>

namespace inviwo {

VolumeReductionGL::VolumeReductionGL()
    : shader_({{ShaderType::Compute, utilgl::findShaderResource("volumereduction.comp")}},
              Shader::Build::No)
    , activeReductionOperator_(ReductionOperator::None)
    , activeDisregardingStatus_(DisregardingStatus::Unset) {}

std::shared_ptr<Volume> VolumeReductionGL::reduce(std::shared_ptr<const Volume> volume,
                                                  const ReductionOperator op,
                                                  DisregardingStatus disregardingStatus,
                                                  const vec2& range) {
    setReductionOperator(op);
    setDisregarding(disregardingStatus);
    setSamplerType(volume);

    shader_.build();

    shader_.activate();

    const auto dimensions = volume->getDimensions();

    auto input = std::shared_ptr<Volume>(volume->clone());
    auto dataFormat = volume->getDataFormat();
    std::shared_ptr<Volume> output;

    auto m = static_cast<GLuint>(dimensions.x) / GLuint{2};
    auto n = static_cast<GLuint>(dimensions.y) / GLuint{2};
    auto o = static_cast<GLuint>(dimensions.z) / GLuint{2};

    while (true) {
        /*
         * Bind textures and set uniforms.
         */
        TextureUnitContainer cont;
        utilgl::bindAndSetUniforms(shader_, cont, *input, "inputTexture");
        shader_.setUniform("outputTexture", 0);

        /*
         * Set value disregarding parameters
         */
        shader_.setUniform("disregardingRange", range);

        /*
         * Update output texture.
         */
        output = std::make_shared<Volume>(size3_t{m, n, o}, dataFormat);
        auto outputGL = output->getEditableRepresentation<VolumeGL>();
        outputGL->setWrapping({Wrapping::Clamp, Wrapping::Clamp, Wrapping::Clamp});

        glActiveTexture(GL_TEXTURE0);

        auto texture = outputGL->getTexture();
        auto texHandle = texture->getID();
        glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                           texture->getInternalFormat());

        outputGL->setSwizzleMask(swizzlemasks::rgba);

        outputGL->getTexture()->bind();

        /*
         * Run reduction.
         */
        gpuDispatch(m, n, o);

        /*
         * When we have reached an output texture size of 1, the reduction is complete.
         */
        if (m == 1 && n == 1 && o == 1) break;

        /*
         * Swap textures.
         */
        std::swap(input, output);

        /*
         * Update output texture size
         */
        m = std::max(m / 2, GLuint{1});
        n = std::max(n / 2, GLuint{1});
        o = std::max(o / 2, GLuint{1});
    }

    shader_.deactivate();

    return output;
}

double VolumeReductionGL::reduce_v(std::shared_ptr<const Volume> volume, const ReductionOperator op,
                                   DisregardingStatus disregardingStatus, const vec2& range) {
    auto res = reduce(volume, op, disregardingStatus, range);

    return res->getRepresentation<VolumeRAM>()->getAsDouble(size3_t{0, 0, 0});
}

void VolumeReductionGL::setReductionOperator(ReductionOperator op) {
    if (op == activeReductionOperator_) return;

    auto computeShader = shader_.getComputeShaderObject();
    computeShader->removeShaderDefine(StrBuffer{"OPERATOR {}", activeReductionOperator_});

    activeReductionOperator_ = op;

    computeShader->addShaderDefine(StrBuffer{"OPERATOR {}", activeReductionOperator_});
}

void VolumeReductionGL::setDisregarding(DisregardingStatus disregardingStatus) {
    if (disregardingStatus == activeDisregardingStatus_) return;

    auto computeShader = shader_.getComputeShaderObject();
    computeShader->removeShaderDefine(StrBuffer{"DISREGARD {}", activeDisregardingStatus_});

    activeDisregardingStatus_ = disregardingStatus;

    computeShader->addShaderDefine(StrBuffer{"DISREGARD {}", activeDisregardingStatus_});
}

void VolumeReductionGL::setSamplerType(std::shared_ptr<const Volume> volume) {
    auto computeShader = shader_.getComputeShaderObject();

    /*
     * Reset just to be sure.
     */
    computeShader->removeShaderDefine("REDUCTION_SAMPLER_TYPE_FLOAT");
    computeShader->removeShaderDefine("REDUCTION_SAMPLER_TYPE_INTEGER");
    computeShader->removeShaderDefine("REDUCTION_SAMPLER_TYPE_UNSIGNED");

    volume->getRepresentation<VolumeRAM>()->dispatch<void, dispatching::filter::All>(
        [&computeShader](auto vrprecision) {
            using VectorType = util::PrecisionValueType<decltype(vrprecision)>;
            using ValueType = util::value_type_t<VectorType>;

            if constexpr (std::is_floating_point_v<ValueType>) {
                computeShader->addShaderDefine("REDUCTION_SAMPLER_TYPE_FLOAT");
            }
            if constexpr (std::is_integral_v<ValueType>) {
                computeShader->addShaderDefine("REDUCTION_SAMPLER_TYPE_SIGNED");
            }
            if constexpr (std::is_unsigned_v<ValueType>) {
                computeShader->addShaderDefine("REDUCTION_SAMPLER_TYPE_UNSIGNED");
            }
        });
}

void VolumeReductionGL::gpuDispatch(const GLuint x, const GLuint y, const GLuint z) {
    glDispatchCompute(x, y, z);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
}  // namespace inviwo
