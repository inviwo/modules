/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2025 Inviwo Foundation
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
#include <inviwo/core/util/volumeramutils.h>
#include <modules/base/algorithm/dataminmax.h>

#include <glm/gtx/component_wise.hpp>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo TensorField3DAnisotropy::processorInfo_{
    "org.inviwo.TensorField3DAnisotropy",  // Class identifier
    "Tensor Field 3D Anisotropy",          // Display name
    "Tensor Visualization",                // Category
    CodeState::Experimental,               // Code state
    Tags::CPU | Tag{"Tensor"},             // Tags
    R"(Computes the anisotropy of a 3D tensor field.)"_unindentHelp,
};
const ProcessorInfo& TensorField3DAnisotropy::getProcessorInfo() const { return processorInfo_; }

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
    , interpolationScheme_("interpolationScheme", "Interpolation scheme",
                           {{"nearest", "Nearest neighbour", InterpolationType::Nearest},
                            {"linear", "Linear", InterpolationType::Linear}},
                           1) {
    addPort(tensorFieldInport_);
    addPort(volumeOutport_);

    addProperty(anisotropy_);
    addProperty(interpolationScheme_);
}

void TensorField3DAnisotropy::process() {
    auto tensorField = tensorFieldInport_.getData();

    auto volumeRam = [&]() -> std::shared_ptr<VolumeRAM> {
        const size3_t dims{tensorField->getDimensions()};

        util::IndexMapper3D indexMapper(dims);
        const auto& majorEigenValues = tensorField->majorEigenValues();
        const auto& intermediateEigenValues = tensorField->middleEigenValues();
        const auto& minorEigenValues = tensorField->minorEigenValues();

        switch (anisotropy_.get()) {
            case tensorutil::Anisotropy::abs_lamda1_minus_lamda2: {
                auto ram = std::make_shared<VolumeRAMPrecision<float>>(dims);
                auto data = ram->getView();
                util::forEachVoxelParallel(dims, [&](const size3_t& pos) -> void {
                    const auto index = indexMapper(pos);
                    const auto val =
                        glm::abs(majorEigenValues[index] - intermediateEigenValues[index]);

                    data[index] = static_cast<float>(val);
                });
                return ram;
            }
            case tensorutil::Anisotropy::abs_lamda1_minus_lamda3: {
                auto ram = std::make_shared<VolumeRAMPrecision<float>>(dims);
                auto data = ram->getView();
                util::forEachVoxelParallel(dims, [&](const size3_t& pos) -> void {
                    const auto index = indexMapper(pos);
                    const auto val = glm::abs(majorEigenValues[index] - minorEigenValues[index]);

                    data[index] = static_cast<float>(val);
                });
                return ram;
            }
            case tensorutil::Anisotropy::barycentric: {
                auto ram = std::make_shared<VolumeRAMPrecision<vec3>>(dims);
                auto data = ram->getView();
                util::forEachVoxelParallel(dims, [&](const size3_t& pos) -> void {
                    const auto index = indexMapper(pos);
                    std::array<double, 3> eigenValues{majorEigenValues[index],
                                                      intermediateEigenValues[index],
                                                      minorEigenValues[index]};

                    std::transform(eigenValues.begin(), eigenValues.end(), eigenValues.begin(),
                                   [](const double& val) { return glm::abs(val); });
                    std::sort(eigenValues.begin(), eigenValues.end(),
                              [](const double& valA, const double& valB) { return valA > valB; });

                    auto denominator = eigenValues[0] + eigenValues[1] + eigenValues[2];
                    denominator = std::max(denominator, std::numeric_limits<double>::epsilon());

                    data[index] =
                        vec3(eigenValues[0] - eigenValues[1],
                             2. * (eigenValues[1] - eigenValues[2]), 3. * eigenValues[2]) /
                        static_cast<float>(denominator);
                });
                break;
            }
            case tensorutil::Anisotropy::abs_lamda1_minus_abs_lamda2: {
                auto ram = std::make_shared<VolumeRAMPrecision<float>>(dims);
                auto data = ram->getView();
                util::forEachVoxelParallel(dims, [&](const size3_t& pos) -> void {
                    const auto index = indexMapper(pos);
                    const auto val =
                        glm::abs(majorEigenValues[index]) - glm::abs(minorEigenValues[index]);

                    data[index] = static_cast<float>(val);
                });
                return ram;
            }
            default:
                throw Exception(IVW_CONTEXT, "Unsupported tensorutil::Anisotropy");
        }
        return nullptr;
    }();

    auto [min, max] = util::volumeMinMax(volumeRam.get(), IgnoreSpecialValues::No);
    dvec2 range{glm::compMin(min), glm::compMax(max)};

    std::shared_ptr<Volume> volume = std::make_shared<Volume>(volumeRam);
    volume->dataMap.dataRange = range;
    volume->dataMap.valueRange = range;

    volume->setModelMatrix(tensorField->getModelMatrix());
    volume->setWorldMatrix(tensorField->getWorldMatrix());
    volume->setInterpolation(interpolationScheme_);
    volume->setSwizzleMask(swizzlemasks::defaultData(volumeRam->getDataFormat()->getComponents()));

    volumeOutport_.setData(volume);
}

}  // namespace inviwo
