/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <inviwo/core/common/inviwo.h>
#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <functional>

namespace inviwo {
/**
 * \class DeformableSphere
 * \brief Data structure for a sphere whose vertices can be manipulated by a supplied lambda
 * function
 */
class IVW_MODULE_TENSORVISBASE_API DeformableSphere {
public:
    DeformableSphere() = delete;
    DeformableSphere(const size_t& numTheta, const size_t& numPhi, const vec4& color = vec4(1.f));
    virtual ~DeformableSphere() = default;

    void deform(const std::function<void(vec3& vertex)>& lambda, const bool& normalize = true);
    void deform(const std::function<void(vec3& vertex, vec4& color)>& lambda,
                const bool& normalize = true);
    void transform(const vec3& pos, const vec3& scale);
    std::shared_ptr<BasicMesh> getGeometry();

private:
    std::vector<size3_t> faces_;
    std::shared_ptr<BasicMesh> mesh_;

    void createSphere(const size_t& numTheta, const size_t& numPhi, const vec4& color);
    void calculateNormals();
};

}  // namespace inviwo
