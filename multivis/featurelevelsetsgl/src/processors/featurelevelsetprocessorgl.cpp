/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021-2025 Inviwo Foundation
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

#include <inviwo/featurelevelsetsgl/processors/featurelevelsetprocessorgl.h>
#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/stringconversion.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/network/networklock.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/texture/textureunit.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/texture/texture3d.h>
#include <modules/base/algorithm/dataminmax.h>
#include <inviwo/featurelevelsetsgl/util/util.h>
#include <inviwo/featurelevelsetsgl/properties/implicitfunctiontraitproperty.h>
#include <inviwo/featurelevelsetsgl/properties/pointtraitproperty.h>
#include <inviwo/featurelevelsetsgl/properties/rangetraitproperty.h>

#include <algorithm>
#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo FeatureLevelSetProcessorGL::processorInfo_{
    "org.inviwo.FeatureLevelSetProcessorGL",  // Class identifier
    "Feature Level Sets",                     // Display name
    "Volume Operation",                       // Category
    CodeState::Experimental,                  // Code state
    Tags::GL,                                 // Tags
};

const ProcessorInfo& FeatureLevelSetProcessorGL::getProcessorInfo() const { return processorInfo_; }

void FeatureLevelSetProcessorGL::serialize(Serializer& s) const {
    Processor::serialize(s);
    s.serialize("volumeNameCache", volumeNameCache_);
}

void FeatureLevelSetProcessorGL::deserialize(Deserializer& s) {
    Processor::deserialize(s);
    s.deserialize("volumeNameCache", volumeNameCache_);
}

FeatureLevelSetProcessorGL::FeatureLevelSetProcessorGL()
    : Processor()
    , PropertyOwnerObserver()
    , volumes_("volume")
    , distanceVolumeOutport_("distanceVolumeOutport")
    , squaredDistance_("squaredDistance", "Use squared distance", false)
    , useVolumesDataMap_("useVolumesDataMap", "Use volume's data map", true)
    , useNormalizedValues_("useNormalizedValues", "Use normalized values", false)
    , capMaxDistance_("capMaxDistance", "Cap max distance", true)
    , traitPropertiesContainer_("traitProperties", "Traits")
    , injectButton_("injectButton", "Inject")
    , prevNumberOfVolumes_(1)
    , dataRangesCache_(std::array<vec2, 4>{vec2(0), vec2(0), vec2(0), vec2(0)})
    , shader_({{
                  ShaderType::Compute,
                  "featurelevelsets.comp",
              }},
              Shader::Build::No) {
    traitPropertiesContainer_.addPrefab(
        std::make_unique<PointTraitProperty>("pointTrait", "Point Trait"));
    traitPropertiesContainer_.addPrefab(
        std::make_unique<RangeTraitProperty>("rangeTrait", "Range Trait"));
    traitPropertiesContainer_.addPrefab(
        std::make_unique<ImplicitFunctionTraitProperty>("implicitFunction", "Implicit function"));

    addPorts(volumes_, distanceVolumeOutport_);
    addProperties(squaredDistance_, useVolumesDataMap_, useNormalizedValues_, capMaxDistance_,
                  traitPropertiesContainer_, injectButton_);

    traitPropertiesContainer_.PropertyOwnerObservable::addObserver(this);

    shader_.onReload([this]() { invalidate(InvalidationLevel::InvalidOutput); });
    {
        auto computeShader = shader_.getShaderObject(ShaderType::Compute);

        computeShader->addShaderDefine("NUM_VOLUMES", StrBuffer{"{}", prevNumberOfVolumes_});
        computeShader->addShaderDefine("TRAIT_ALLOCATION", StrBuffer{"{}", traitAllocation_});
    }
    shader_.build();

    squaredDistance_.onChange([this]() {
        if (auto computeShader = shader_.getShaderObject(ShaderType::Compute);
            squaredDistance_.get()) {
            computeShader->addShaderDefine("SQUARED_DISTANCE");
        } else {
            computeShader->removeShaderDefine("SQUARED_DISTANCE");
        }

        shader_.build();
    });

    volumes_.onConnect([this]() {
        if (compareInputsToCache()) {
            return;
        }

        auto sourceVectorData = volumes_.getSourceVectorData();

        /*
         * At this point we know that we are not deserializing and that one volume has been
         * connected. If more that 4 volumes are connected now, just ignore the new volume.
         * Otherwise, update the attribute properties of all trait properties.
         */
        const auto numVolumes = sourceVectorData.size();
        if (numVolumes > maxVolumes_) {
            LogWarn(fmt::format("More than {} volumes connected ({}). Ignoring remaining ones.",
                                maxVolumes_, numVolumes));
            return;
        }

        /*
         * There is a possibility that there are more than 1 newly connected volumes. Therefore,
         * check if there is only one new volume and then update properties.
         */

        /*
         * Return difference sets.
         */
        auto [namesToAdd, namesToRemove] = [&inputVolumes = sourceVectorData,
                                            &volumeNameCache = volumeNameCache_]() {
            auto sortedCache = std::vector(volumeNameCache);
            auto inputNames = std::vector<std::string>{};
            auto namesToAdd = std::vector<std::string>{};
            auto namesToRemove = std::vector<std::string>{};

            // auto inputVolumes = volumes_.getSourceVectorData();
            for (const auto& [outport, volume] : inputVolumes) {
                inputNames.push_back(outport->getProcessor()->getDisplayName());
            }

            std::sort(std::begin(sortedCache), std::end(sortedCache));
            std::sort(std::begin(inputNames), std::end(inputNames));

            std::set_difference(std::begin(inputNames), std::end(inputNames),
                                std::begin(sortedCache), std::end(sortedCache),
                                std::back_inserter(namesToAdd));

            std::set_difference(std::begin(sortedCache), std::end(sortedCache),
                                std::begin(inputNames), std::end(inputNames),
                                std::back_inserter(namesToRemove));

            return std::pair(namesToAdd, namesToRemove);
        }();

        /*
         * This is not production code, for now just handle case of +1, otherwise reset everything.
         */
        if (!namesToRemove.empty() || namesToAdd.size() != 1) {
            initializeAllProperties();
            return;
        }

        /*
         * Find volume with corresponding name
         */
        const auto pair =
            std::find_if(std::begin(sourceVectorData), std::end(sourceVectorData),
                         [&name = namesToAdd.front()](const auto& pair) {
                             return pair.first->getProcessor()->getDisplayName() == name;
                         });

        const auto& [outport, volume] = *pair;

        for (const auto property : traitPropertiesContainer_.getProperties()) {
            if (auto traitProperty = dynamic_cast<TraitProperty*>(property)) {

                traitProperty->addAttribute(outport->getProcessor()->getDisplayName(), volume,
                                            useVolumesDataMap_.get());

                traitProperty->setSerializationMode(PropertySerializationMode::All);
            }
        }

        updateNormalizedVolumesCache();
    });

    volumes_.onChange([this]() {
        updateVolumeNamesCache();

        auto computeShader = shader_.getShaderObject(ShaderType::Compute);

        computeShader->removeShaderDefine(StrBuffer{"NUM_VOLUMES {}", prevNumberOfVolumes_});

        prevNumberOfVolumes_ = volumes_.getVectorData().size();

        computeShader->addShaderDefine("NUM_VOLUMES", StrBuffer{"{}", prevNumberOfVolumes_});

        checkInput();

        shader_.build();

        updateDataRangesCache();

        updateNormalizedVolumesCache();

        updateMaximumDistance();
    });

    volumes_.onDisconnect([this]() {
        const auto sourceVectorData = volumes_.getSourceVectorData();

        if (const auto numVolumes = sourceVectorData.size(); numVolumes > maxVolumes_) {
            LogWarn(fmt::format("More than {} volumes connected ({}). Ignoring remaining ones.",
                                maxVolumes_, numVolumes));
            return;
        }
        /*
         * Return difference sets.
         */
        auto [namesToAdd, namesToRemove] = [inputVolumes = volumes_.getSourceVectorData(),
                                            &volumeNameCache = volumeNameCache_]() {
            auto sortedCache = std::vector(volumeNameCache);
            auto inputNames = std::vector<std::string>{};
            auto namesToAdd = std::vector<std::string>{};
            auto namesToRemove = std::vector<std::string>{};

            // auto inputVolumes = volumes_.getSourceVectorData();
            for (const auto& [outport, volume] : inputVolumes) {
                inputNames.push_back(outport->getProcessor()->getDisplayName());
            }

            std::sort(std::begin(sortedCache), std::end(sortedCache));
            std::sort(std::begin(inputNames), std::end(inputNames));

            std::set_difference(std::begin(inputNames), std::end(inputNames),
                                std::begin(sortedCache), std::end(sortedCache),
                                std::back_inserter(namesToAdd));

            std::set_difference(std::begin(sortedCache), std::end(sortedCache),
                                std::begin(inputNames), std::end(inputNames),
                                std::back_inserter(namesToRemove));

            return std::pair(namesToAdd, namesToRemove);
        }();

        /*
         * For now, reset everything, research must go on.
         */
        initializeAllProperties();

        updateNormalizedVolumesCache();
    });

    injectButton_.onChange([this]() {
        for (auto property : traitPropertiesContainer_.getProperties()) {
            if (auto implicitFunctionTraitProperty =
                    dynamic_cast<ImplicitFunctionTraitProperty*>(property)) {
                implicitFunctionTraitProperty->inject(shader_);
            }
        }
    });

    capMaxDistance_.onChange([this]() {
        if (capMaxDistance_.get()) {
            LogInfoCustom("Feature Level Sets",
                          fmt::format("Capping max distance at {}.", maxDist_));
        } else {
            LogInfoCustom("Feature Level Sets", "Capping disabled.");
        }
    });
}

void FeatureLevelSetProcessorGL::process() {
    if (!volumes_.hasData() || volumes_.getVectorData().empty()) return;
    if (traitPropertiesContainer_.getProperties().empty()) return;

    checkInput();

    shader_.activate();

    setUniforms();

    const auto outputTexture = bindOutputTexture();

    gpuDispatch();

    shader_.deactivate();

    auto min = reduction_.reduce_v(outputTexture, ReductionOperator::Min);
    const auto max = reduction_.reduce_v(outputTexture, ReductionOperator::Max);

    /*if (std::abs(max - min) < std::numeric_limits<double>::epsilon()) {
        min = min - max;
    }*/

    outputTexture->dataMap.valueRange = outputTexture->dataMap.dataRange =
        dvec2(min, capMaxDistance_.get() ? std::min(max, maxDist_) : max);

    distanceVolumeOutport_.setData(outputTexture);
}

void FeatureLevelSetProcessorGL::checkInput() const {
    const size3_t dims = volumes_.getData()->getDimensions();
    for (const auto vol : volumes_) {
        if (dims != vol->getDimensions()) {
            throw Exception(fmt::format("Different volume dimensions: {}, expected {}",
                                        glm::to_string(vol->getDimensions()), glm::to_string(dims)),
                            IVW_CONTEXT);
        }

        if (vol->getDataFormat()->getNumericType() != NumericType::Float) {
            LogWarn("Sampling non-float volume, shader will normalize values.");
        }
    }
}

std::vector<vec4> FeatureLevelSetProcessorGL::gatherPointTraits() const {
    auto properties = traitPropertiesContainer_.getProperties();
    std::vector<vec4> pointTraits;

    for (auto property : properties) {
        if (const auto pointTraitProperty = dynamic_cast<PointTraitProperty*>(property)) {
            pointTraits.push_back(pointTraitProperty->getAsVec4());
        }
    }

    return pointTraits;
}

void FeatureLevelSetProcessorGL::normalizePointTraits(std::vector<vec4>& pointTraits) const {
    for (auto& pointTrait : pointTraits) {
        for (size_t i{0}; i < 4; i++) {
            pointTrait[i] =
                util::normalizeValue(pointTrait[i], dataRangesCache_[i].x, dataRangesCache_[i].y);
        }
    }
}

std::vector<mat4> FeatureLevelSetProcessorGL::gatherRangeTraits() const {
    auto properties = traitPropertiesContainer_.getProperties();
    std::vector<mat4> rangeTraits{};

    for (auto property : properties) {
        if (const auto rangeTraitProperty = dynamic_cast<RangeTraitProperty*>(property)) {
            rangeTraits.push_back(rangeTraitProperty->getAsMat4());
        }
    }

    return rangeTraits;
}

void FeatureLevelSetProcessorGL::normalizeRangeTraits(std::vector<mat4>& rangeTraits) const {
    for (auto& rangeTrait : rangeTraits) {
        for (size_t i{0}; i < 4; i++) {
            auto minVal = rangeTrait[i][0];
            auto maxVal = rangeTrait[i][1];

            rangeTrait[i][0] =
                util::normalizeValue(minVal, dataRangesCache_[i].x, dataRangesCache_[i].y);
            rangeTrait[i][1] =
                util::normalizeValue(maxVal, dataRangesCache_[i].x, dataRangesCache_[i].y);
        }
    }
}

std::vector<vec2> FeatureLevelSetProcessorGL::gatherVolumeRanges() const {
    std::vector<vec2> ranges{};

    for (const auto volume : volumes_.getVectorData()) {
        ranges.emplace_back(volume->dataMap.dataRange);
    }

    return ranges;
}

void FeatureLevelSetProcessorGL::normalizeVolumeRanges(std::vector<vec2>& volumeRanges) const {
    std::transform(std::begin(volumeRanges), std::end(volumeRanges), std::begin(volumeRanges),
                   [](const auto&) {
                       return vec2{0.f, 1.f};
                   });
}

void FeatureLevelSetProcessorGL::setUniforms() {
    const size3_t dims = volumes_.getData()->getDimensions();

    TextureUnitContainer cont;

    if (!useNormalizedValues_.get()) {
        size_t idx{0};

        for (auto& [outport, volume] : volumes_.getSourceVectorData()) {
            if (idx >= maxVolumes_) break;
            LogInfo("Binding " + outport->getProcessor()->getDisplayName() + " at index " +
                    std::to_string(idx));
            TextureUnit unit;
            utilgl::bindTexture(*volume, unit);
            shader_.setUniform(StrBuffer{"volume[{}]", idx}, unit.getUnitNumber());
            utilgl::setShaderUniforms(shader_, *volume, StrBuffer{"volumeParameters[{}]", idx++});
            cont.push_back(std::move(unit));
        }
    } else {
        for (auto&& [index, volume] : util::enumerate(normalizedVolumesCache_)) {
            if (index >= maxVolumes_) break;
            LogInfo("Binding " + volume.first + " at index " + std::to_string(index));
            auto vol = volume.second;
            TextureUnit unit;
            utilgl::bindTexture(*vol, unit);
            shader_.setUniform(StrBuffer{"volume[{}]", index}, unit.getUnitNumber());
            utilgl::setShaderUniforms(shader_, *vol, StrBuffer{"volumeParameters[{}]", index});
            cont.push_back(std::move(unit));
        }
    }

    LogInfo("--------------------------------------");

    shader_.setUniform("dest", 0);

    auto pointTraits = gatherPointTraits();
    auto rangeTraits = gatherRangeTraits();
    auto volumeRanges = gatherVolumeRanges();

    if (useNormalizedValues_.get()) {
        normalizePointTraits(pointTraits);
        normalizeRangeTraits(rangeTraits);
        normalizeVolumeRanges(volumeRanges);
    }

    shader_.setUniform("pointTraits", pointTraits);
    shader_.setUniform("rangeTraits", rangeTraits);
    shader_.setUniform("volumeRanges", volumeRanges);

    shader_.setUniform("numPointTraits", static_cast<int>(pointTraits.size()));
    shader_.setUniform("numRangeTraits", static_cast<int>(rangeTraits.size()));
}

std::shared_ptr<Volume> FeatureLevelSetProcessorGL::bindOutputTexture() {
    auto inputVolumes = volumes_.getVectorData();
    const size3_t dims = inputVolumes.front()->getDimensions();
    auto referenceBasis = inputVolumes.front()->getBasis();
    auto referenceOffset = inputVolumes.front()->getOffset();

    for (auto volume : inputVolumes) {
        if (glm::any(glm::notEqual(referenceBasis, volume->getBasis()))) {
            LogWarn(
                "Input volumes do not have the same bases. Setting basis of input volume 1 for the "
                "output volume.");
        }
        if (glm::any(glm::notEqual(referenceOffset, volume->getOffset()))) {
            LogWarn(
                "Input volumes do not have the same offsets. Setting offset of input volume 1 for "
                "the output volume.");
        }
    }

    auto distanceVolume = std::make_shared<Volume>(dims, DataFloat32::get());
    distanceVolume->setBasis(inputVolumes.front()->getBasis());
    distanceVolume->setOffset(inputVolumes.front()->getOffset());
    auto distanceVolumeGL = distanceVolume->getEditableRepresentation<VolumeGL>();

    glActiveTexture(GL_TEXTURE0);

    auto texHandle = distanceVolumeGL->getTexture()->getID();
    glBindImageTexture(0, texHandle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

    distanceVolumeGL->setSwizzleMask(swizzlemasks::rgba);

    distanceVolumeGL->getTexture()->bind();

    return distanceVolume;
}

void FeatureLevelSetProcessorGL::gpuDispatch() {
    const size3_t dims = volumes_.getData()->getDimensions();

    const auto x = static_cast<GLuint>(dims.x);
    const auto y = static_cast<GLuint>(dims.y);
    const auto z = static_cast<GLuint>(dims.z);

    glDispatchCompute(x, y, z);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void FeatureLevelSetProcessorGL::updateDataRangesCache() {
    auto volumes = volumes_.getVectorData();

    for (auto&& [index, volume] : util::enumerate(volumes)) {
        if (auto& range = dataRangesCache_[index]; useVolumesDataMap_.get()) {
            range = volume->dataMap.valueRange;
        } else {
            auto [mins, maxs] = util::volumeMinMax(volume.get());
            range = dvec2(mins.x, maxs.x);
        }
    }
}

void FeatureLevelSetProcessorGL::updateMaximumDistance() {
    auto volumes = volumes_.getVectorData();

    if (useNormalizedValues_.get()) {
        maxDist_ = std::sqrt(volumes.size());
        return;
    }

    std::vector<double> minVec{};
    std::vector<double> maxVec{};

    for (auto range : dataRangesCache_) {
        minVec.push_back(range.x);
        maxVec.push_back(range.y);
    }

    maxDist_ = util::euclideanDistance(minVec, maxVec);
}

void FeatureLevelSetProcessorGL::updateNormalizedVolumesCache() {
    normalizedVolumesCache_.clear();
    for (auto& [outport, volume] : volumes_.getSourceVectorData()) {
        normalizedVolumesCache_.emplace_back(outport->getProcessor()->getDisplayName(),
                                             normalization_.normalize(*volume));
    }
}

bool FeatureLevelSetProcessorGL::compareInputsToCache() const {
    std::vector<std::string> names{};
    std::vector<std::string> intersection{};
    std::vector<std::string> nameCache(volumeNameCache_);

    for (auto [outport, volume] : volumes_.getSourceVectorData()) {
        names.push_back(outport->getProcessor()->getDisplayName());
    }

    std::sort(std::begin(names), std::end(names));
    std::sort(std::begin(nameCache), std::end(nameCache));

    std::set_intersection(std::begin(names), std::end(names), std::begin(nameCache),
                          std::end(nameCache), std::back_inserter(intersection));

    const auto matchesAll = nameCache.size() == intersection.size();
    const auto sameAmount = names.size() == nameCache.size();

    return matchesAll && sameAmount;
}

void FeatureLevelSetProcessorGL::updateVolumeNamesCache() {
    auto volumes = volumes_.getSourceVectorData();

    std::vector<std::string> names{};

    names.reserve(volumes.size());

    for (const auto& [outport, volume] : volumes) {
        names.push_back(outport->getProcessor()->getDisplayName());
    }

    volumeNameCache_ = names;
}

void FeatureLevelSetProcessorGL::resizeTraitAllocation() {
    auto computeShader = shader_.getShaderObject(ShaderType::Compute);

    computeShader->removeShaderDefine(StrBuffer{"TRAIT_ALLOCATION {}", traitAllocation_});

    traitAllocation_ += 8;

    computeShader->addShaderDefine("TRAIT_ALLOCATION", StrBuffer{"{}", traitAllocation_});

    shader_.build();
}

void FeatureLevelSetProcessorGL::initializeAllProperties() {
    for (auto property : traitPropertiesContainer_.getProperties()) {
        auto traitProperty = dynamic_cast<TraitProperty*>(property);
        traitProperty->reset();
        for (const auto [outport, volume] : volumes_.getSourceVectorData()) {
            traitProperty->addAttribute(outport->getProcessor()->getDisplayName(), volume,
                                        useVolumesDataMap_.get());
        }
    }
}

void FeatureLevelSetProcessorGL::onWillAddProperty(Property* property, size_t) {
    if (!volumes_.isConnected()) {
        return;
    }

    const auto volumes = volumes_.getSourceVectorData();

    if (auto traitProperty = dynamic_cast<TraitProperty*>(property)) {

        for (const auto& [outport, volume] : volumes) {
            traitProperty->addAttribute(outport->getProcessor()->getDisplayName(), volume,
                                        useVolumesDataMap_.get());
        }

        traitProperty->setSerializationMode(PropertySerializationMode::All);

        if (traitPropertiesContainer_.getProperties().size() > traitAllocation_) {
            resizeTraitAllocation();
        }

        // traitProperty->onChange([&, this]() { generateTraitMesh(traitProperty->getIdentifier());
        // });
    }
}

}  // namespace inviwo
