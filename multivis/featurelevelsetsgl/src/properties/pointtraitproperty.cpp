/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2022-2025 Inviwo Foundation
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

#include <inviwo/featurelevelsetsgl/properties/pointtraitproperty.h>
#include <modules/base/algorithm/dataminmax.h>

namespace inviwo {

std::string_view PointTraitProperty::getClassIdentifier() const { return classIdentifier; }

vec4 PointTraitProperty::getAsVec4() const {
    return vec4(attribute1_.get(), attribute2_.get(), attribute3_.get(), attribute4_.get());
}

void PointTraitProperty::addAttribute(const std::string& name, std::shared_ptr<const Volume> volume,
                                      bool useVolumeDataMap) {
    const auto minVal = useVolumeDataMap
                            ? vec2(volume->dataMap.valueRange).x
                            : static_cast<float>(util::volumeMinMax(volume.get()).first.x);
    const auto maxVal = useVolumeDataMap
                            ? vec2(volume->dataMap.valueRange).y
                            : static_cast<float>(util::volumeMinMax(volume.get()).second.x);

    attributes_[numInitializedAttributes()]->setVisible(true);
    attributes_[numInitializedAttributes()]->set(minVal);
    attributes_[numInitializedAttributes()]->setMinValue(minVal);
    attributes_[numInitializedAttributes()]->setMaxValue(maxVal);
    attributes_[numInitializedAttributes()]->setDisplayName(name);

    inc();
}

}  // namespace inviwo
