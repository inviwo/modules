/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/topologytoolkit/properties/topologyfilterproperty.h>
#include <inviwo/core/util/foreacharg.h>

namespace inviwo {

const std::string TopologyFilterProperty::classIdentifier =
    "org.inviwo.topology.TopologyFilterProperty";
std::string TopologyFilterProperty::getClassIdentifier() const { return classIdentifier; }

TopologyFilterProperty::TopologyFilterProperty(std::string identifier, std::string displayName)
    : CompositeProperty(identifier, displayName)
    , maxima{"maxima", "Show Maxima", true}
    , minima{"minima", "Show Minima", true}
    , saddle{"saddle", "Show Saddles", true}
    , maxSaddle{"maxSaddle", "Show Maxima Saddle Separatrices", true}
    , minSaddle{"minSaddle", "Show Minima Saddle Separatrices", true}
    , saddleSaddle{"saddleSaddle", "Show Saddle Saddle Separatrices", true} {

    util::for_each_in_tuple([&](auto& e) { addProperty(e); }, props());
}

TopologyFilterProperty::TopologyFilterProperty(const TopologyFilterProperty& rhs)
    : CompositeProperty(rhs)
    , maxima{rhs.maxima}
    , minima{rhs.minima}
    , saddle{rhs.saddle}
    , maxSaddle{rhs.maxSaddle}
    , minSaddle{rhs.minSaddle}
    , saddleSaddle{rhs.saddleSaddle} {

    util::for_each_in_tuple([&](auto& e) { addProperty(e); }, props());
}

TopologyFilterProperty* TopologyFilterProperty::clone() const {
    return new TopologyFilterProperty(*this);
}

bool TopologyFilterProperty::showExtrema(int dimensionality, char cellDim) const {
    switch (topology::extremaDimToType(dimensionality, cellDim)) {
        case topology::CellType::unkown:
            return false;
        case topology::CellType::minimum:
            return *minima;
        case topology::CellType::saddle:
            return *saddle;
        case topology::CellType::maximum:
            return *maxima;
        case topology::CellType::minSaddle:
            return false;
        case topology::CellType::maxSaddle:
            return false;
        case topology::CellType::saddleSaddle:
            return false;
        default:
            return false;
    }
}

bool TopologyFilterProperty::showSeperatrix(int dimensionality, char type) const {
    return showSeperatrix(topology::seperatrixTypeToType(dimensionality, type));
}

bool TopologyFilterProperty::showSeperatrix(topology::CellType type) const {
    switch (type) {
        case topology::CellType::unkown:
            return false;
        case topology::CellType::minimum:
            return *minSaddle;
        case topology::CellType::saddle:
            return *minSaddle || *maxSaddle || *saddleSaddle;
        case topology::CellType::maximum:
            return *maxSaddle;
        case topology::CellType::minSaddle:
            return *minSaddle;
        case topology::CellType::maxSaddle:
            return *maxSaddle;
        case topology::CellType::saddleSaddle:
            return *saddleSaddle;
        default:
            return false;
    }
}

}  // namespace inviwo
