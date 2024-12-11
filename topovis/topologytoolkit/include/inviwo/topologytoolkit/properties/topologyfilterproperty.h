/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2024 Inviwo Foundation
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
#pragma once

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>

#include <inviwo/topologytoolkit/datastructures/morsesmalecomplexdata.h>

namespace inviwo {

/**
 * \ingroup properties
 *  A property providing comonly-used filters in topology visualization.
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TopologyFilterProperty : public CompositeProperty {
public:
    virtual std::string_view getClassIdentifier() const override;
    static constexpr std::string_view classIdentifier{"org.inviwo.topology.TopologyFilterProperty"};

    TopologyFilterProperty(std::string identifier, std::string displayName);

    TopologyFilterProperty(const TopologyFilterProperty& rhs);
    virtual TopologyFilterProperty* clone() const override;
    virtual ~TopologyFilterProperty() = default;

    BoolProperty maxima;
    BoolProperty minima;
    BoolProperty saddle;
    BoolProperty maxSaddle;
    BoolProperty minSaddle;
    BoolProperty saddleSaddle;

    bool showExtrema(int dim, char cellDim) const;
    bool showSeperatrix(int dim, char type) const;
    bool showSeperatrix(topology::CellType type) const;

private:
    auto props() { return std::tie(maxima, minima, saddle, maxSaddle, minSaddle, saddleSaddle); }
    auto props() const {
        return std::tie(maxima, minima, saddle, maxSaddle, minSaddle, saddleSaddle);
    }
};

}  // namespace inviwo
