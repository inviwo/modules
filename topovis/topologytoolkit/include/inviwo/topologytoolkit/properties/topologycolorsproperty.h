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
#pragma once

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

namespace inviwo {

/**
 * \ingroup properties
 *  A property providing comonly-used colors in topology visualization.
 *
 * @see CompositeProperty
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TopologyColorsProperty : public CompositeProperty {
public:
    virtual std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

    TopologyColorsProperty(std::string identifier, std::string displayName);

    TopologyColorsProperty(const TopologyColorsProperty& rhs);
    virtual TopologyColorsProperty* clone() const override;
    virtual ~TopologyColorsProperty() = default;

    /**
     * \brief return the color for a critical 2D point based on the cell dimension \p cellDim
     *
     * @param cellDim   cell dimensions of the critical point
     * @return color matching the given cell dimension
     */
    vec4 getColor2D(char cellDim) const;
    /**
     * \brief return the color for a critical 3D point based on the cell dimension \p cellDim
     *
     * @param cellDim   cell dimensions of the critical point
     * @return color matching the given cell dimension
     */
    vec4 getColor3D(char cellDim) const;

    FloatVec4Property localMaxima_;
    FloatVec4Property localMinima_;
    FloatVec4Property saddle_;
    FloatVec4Property arc_;

private:
    auto props() { return std::tie(localMaxima_, localMinima_, saddle_, arc_); }
    auto props() const { return std::tie(localMaxima_, localMinima_, saddle_, arc_); }
};

}  // namespace inviwo
