/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
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

#include <inviwo/tensorvisbase/processors/tensorfield3danisotropy.h>
#include <inviwo/tensorvisbase/util/tensorfieldutil.h>
#include <inviwo/tensorvisbase/util/tensorutil.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/util/indexmapper.h>
#include <modules/opengl/volume/volumegl.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DAnisotropy::processorInfo_{
    "org.inviwo.TensorField3DAnisotropy",  // Class identifier
    "Tensor Field 3D Anisotropy",          // Display name
    "Tensor Visualization",                // Category
    CodeState::Experimental,               // Code state
    Tags::None,                            // Tags
};
const ProcessorInfo TensorField3DAnisotropy::getProcessorInfo() const { return processorInfo_; }

TensorField3DAnisotropy::TensorField3DAnisotropy()
    : Processor()
    , tensorFieldInport_("tensorFieldInport")
    , volumeOutport_("volumeOutport")
    , anisotropy_("anisotropy", "Anisotropy",
                  {{"1", "|" + tensorutil::lamda1_str + " - " + tensorutil::lamda2_str + "|",
                    tensorutil::Anisotropy::abs_lamda1_minus_lamda2},
                   {"2", "|" + tensorutil::lamda1_str + " - " + tensorutil::lamda3_str + "|",
                    tensorutil::Anisotropy::abs_lamda1_minus_lamda3},
                   {"3", "Barycentric", tensorutil::Anisotropy::barycentric},
                   {"4", "|" + tensorutil::lamda1_str + "| - |" + tensorutil::lamda2_str + "|",
                    tensorutil::Anisotropy::abs_lamda1_minus_abs_lamda2}},
                  0)
    , interpolationScheme_(
          "interpolationScheme", "Interpolation scheme",
          {{"nearest", "Nearest neighbour", GL_NEAREST}, {"linear", "Linear", GL_LINEAR}}, 1) {
    addPort(tensorFieldInport_);
    addPort(volumeOutport_);

    addProperty(anisotropy_);
    addProperty(interpolationScheme_);
}

void TensorField3DAnisotropy::process() {
    auto tensorField = tensorFieldInport_.getData();
    std::shared_ptr<Volume> outputVolume;
    auto dimensions = tensorField->getDimensions();
    std::function<void(const size3_t&)> func;
    util::IndexMapper3D indexMapper(dimensions);
    const auto& majorEigenValues = tensorField->majorEigenValues();
    const auto& intermediateEigenValues = tensorField->intermediateEigenValues();
    const auto& minorEigenValues = tensorField->minorEigenValues();
    auto min = std::numeric_limits<float>::max();
    auto max = std::numeric_limits<float>::lowest();

    switch (anisotropy_.get()) {
        case tensorutil::Anisotropy::abs_lamda1_minus_lamda2:
            outputVolume = std::make_shared<Volume>(dimensions, DataFloat32::get());
            {
                auto data = static_cast<glm::f32*>(
                    outputVolume->getEditableRepresentation<VolumeRAM>()->getData());

                tensorutil::forEachVoxelParallel(
                    *(tensorField.get()), [&](const size3_t& pos) -> void {
                        const auto index = indexMapper(pos);
                        const auto val =
                            glm::abs(majorEigenValues[index] - intermediateEigenValues[index]);
                        max = std::max(max, val);
                        min = std::min(min, val);
                        data[index] = static_cast<glm::f32>(val);
                    });
            }
            break;
        case tensorutil::Anisotropy::abs_lamda1_minus_lamda3:
            outputVolume = std::make_shared<Volume>(dimensions, DataFloat32::get());
            {
                auto data = static_cast<glm::f32*>(
                    outputVolume->getEditableRepresentation<VolumeRAM>()->getData());

                tensorutil::forEachVoxelParallel(
                    *(tensorField.get()), [&](const size3_t& pos) -> void {
                        const auto index = indexMapper(pos);
                        const auto val =
                            glm::abs(majorEigenValues[index] - minorEigenValues[index]);

                        max = std::max(max, val);
                        min = std::min(min, val);
                        data[index] = static_cast<glm::f32>(val);
                    });
            }
            break;
        case tensorutil::Anisotropy::barycentric:
            outputVolume = std::make_shared<Volume>(dimensions, DataVec3Float32::get());
            {
                auto data = static_cast<glm::vec3*>(
                    outputVolume->getEditableRepresentation<VolumeRAM>()->getData());

                tensorutil::forEachVoxelParallel(
                    *(tensorField.get()), [&](const size3_t& pos) -> void {
                        const auto index = indexMapper(pos);
                        auto eigenValues = std::array<float, 3>{majorEigenValues[index],
                                                                 intermediateEigenValues[index],
                                                                 minorEigenValues[index]};

                        std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                                       [](const float& val) { return glm::abs(val); });

                        std::sort(
                            eigenValues.begin(), eigenValues.end(),
                            [](const float& valA, const float& valB) { return valA > valB; });

                        auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
                        if (denominator < std::numeric_limits<float>::epsilon())
                            denominator = std::numeric_limits<float>::epsilon();
                        const auto c_l_ = (eigenValues[0] - eigenValues[1]) / denominator;
                        const auto c_p_ = (2. * (eigenValues[1] - eigenValues[2])) / denominator;
                        const auto c_s_ = (3. * eigenValues[2]) / denominator;

                        const auto barycentricCoordinates = vec3(c_l_, c_p_, c_s_);

                        max = std::max(max, barycentricCoordinates.x);
                        max = std::max(max, barycentricCoordinates.y);
                        max = std::max(max, barycentricCoordinates.z);
                        min = std::min(min, barycentricCoordinates.x);
                        min = std::min(min, barycentricCoordinates.y);
                        min = std::min(min, barycentricCoordinates.z);

                        data[index] = vec3(barycentricCoordinates);
                    });
            }
            break;
        case tensorutil::Anisotropy::abs_lamda1_minus_abs_lamda2:
            outputVolume = std::make_shared<Volume>(dimensions, DataFloat32::get());
            {
                auto data = static_cast<glm::f32*>(
                    outputVolume->getEditableRepresentation<VolumeRAM>()->getData());

                tensorutil::forEachVoxelParallel(
                    *(tensorField.get()), [&](const size3_t& pos) -> void {
                        const auto index = indexMapper(pos);
                        const auto val =
                            glm::abs(majorEigenValues[index]) - glm::abs(minorEigenValues[index]);

                        max = std::max(max, val);
                        min = std::min(min, val);
                        data[index] = static_cast<glm::f32>(val);
                    });
            }
            break;
        default:
            break;
    }
    outputVolume->dataMap_.dataRange = vec2(min, max);
    outputVolume->dataMap_.valueRange = vec2(min, max);

    const auto extends = tensorField->getExtents();

    outputVolume->setBasis(
        mat3(vec3(extends.x, 0., 0.), vec3(0., extends.y, 0.), vec3(0., 0., extends.z)));

    outputVolume->setOffset(tensorField->getOffset());

    // HOTFIX: enable nearest neighbor sampling for volume
    TextureUnit unit;
    outputVolume->getRepresentation<VolumeGL>()->bindTexture(unit.getEnum());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, interpolationScheme_.get());
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, interpolationScheme_.get());
    outputVolume->getRepresentation<VolumeGL>()->unbindTexture();

    volumeOutport_.setData(outputVolume);
}

}  // namespace inviwo
