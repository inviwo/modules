/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2020 Inviwo Foundation
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

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/datastructures/datatraits.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield2d.h>
#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>

namespace inviwo {

/**
 * \ingroup ports
 */
using TensorField2DInport = DataInport<TensorField2D>;

/**
 * \ingroup ports
 */
using TensorField2DOutport = DataOutport<TensorField2D>;

template <>
struct DataTraits<TensorField2D> {
    static std::string classIdentifier() { return "org.inviwo.TensorField2D"; }
    static std::string dataName() { return "TensorField2D"; }
    static uvec3 colorCode() { return uvec3(46, 232, 167); }
    static Document info(const TensorField2D& data) {
        std::ostringstream oss;
        oss << data.getDataInfo();
        Document doc;
        doc.append("p", oss.str());
        return doc;
    }
};

/**
 * \ingroup ports
 */
using TensorField3DInport = DataInport<TensorField3D>;

/**
 * \ingroup ports
 */
using TensorField3DOutport = DataOutport<TensorField3D>;

template <>
struct DataTraits<TensorField3D> {
    static std::string classIdentifier() { return "org.inviwo.TensorField3D"; }
    static std::string dataName() { return "TensorField3D"; }
    static uvec3 colorCode() { return uvec3(46, 179, 232); }
    static Document info(const TensorField3D& data) {
        std::ostringstream oss;
        oss << data.getDataInfo();
        Document doc;
        doc.append("p", oss.str());
        return doc;
    }
};

}  // namespace inviwo
