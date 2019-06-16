/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/properties/topologycolorsproperty.h>
#include <inviwo/core/util/foreacharg.h>

namespace inviwo {

const std::string TopologyColorsProperty::classIdentifier =
    "org.inviwo.topology.TopologyColorsProperty";
std::string TopologyColorsProperty::getClassIdentifier() const { return classIdentifier; }

TopologyColorsProperty::TopologyColorsProperty(std::string identifier, std::string displayName)
    : CompositeProperty(identifier, displayName)
    , localMaxima_("localMaximaColor", "Local Maxima", vec4(1.0f, 0.2f, 0.15f, 1.0f), vec4(0.0f),
                   vec4(1.0f))
    , localMinima_("localMinimaColor", "Local Minima", vec4(0.3f, 1.0f, 0.0f, 1.0f), vec4(0.0f),
                   vec4(1.0f))
    , saddle_("saddleColor", "Saddle Point", vec4(1.0f, 1.0f, 0.5f, 1.0f), vec4(0.0f), vec4(1.0f))
    , arc_("arcColor", "Arc", vec4(0.2f, 0.2f, 0.2f, 1.0f), vec4(0.0f), vec4(1.0f)) {
    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());
}

TopologyColorsProperty::TopologyColorsProperty(const TopologyColorsProperty& rhs)
    : CompositeProperty(rhs)
    , localMaxima_(rhs.localMaxima_)
    , localMinima_(rhs.localMinima_)
    , saddle_(rhs.saddle_)
    , arc_(rhs.arc_) {
    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());
}

TopologyColorsProperty& TopologyColorsProperty::operator=(const TopologyColorsProperty& that) {
    if (this != &that) {
        CompositeProperty::operator=(that);
        util::for_each_in_tuple([](auto& dst, auto& src) { dst = src; }, props(), that.props());
    }
    return *this;
}

TopologyColorsProperty* TopologyColorsProperty::clone() const {
    return new TopologyColorsProperty(*this);
}

}  // namespace inviwo
