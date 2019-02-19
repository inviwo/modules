/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2018 Inviwo Foundation
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

#include <modules/tensorvisbase/datastructures/deformablecube.h>

namespace inviwo {
DeformableCube::DeformableCube(const vec4& color) {
    createCube(color);

    calculateNormals();
}

void DeformableCube::createCube(const vec4& color) {
    mesh_ = std::make_shared<BasicMesh>();

    auto indexBuffer = mesh_->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);
    indexBuffer->getDataContainer().reserve(36);

    const auto p000 = vec3(-0.5f, -0.5f, -0.5f);
    const auto p001 = vec3(-0.5f, -0.5f, +0.5f);
    const auto p010 = vec3(-0.5f, +0.5f, -0.5f);
    const auto p011 = vec3(-0.5f, +0.5f, +0.5f);
    const auto p100 = vec3(+0.5f, -0.5f, -0.5f);
    const auto p101 = vec3(+0.5f, -0.5f, +0.5f);
    const auto p110 = vec3(+0.5f, +0.5f, -0.5f);
    const auto p111 = vec3(+0.5f, +0.5f, +0.5f);

    const auto xNorm = vec3(1, 0, 0);
    const auto yNorm = vec3(0, 1, 0);
    const auto zNorm = vec3(0, 0, 1);

    const auto pos00 = vec3(0, 0, 0);
    const auto pos10 = vec3(1, 0, 0);
    const auto pos11 = vec3(1, 1, 0);
    // pos normal tex color

    glm::uint32_t offset = 0;

    auto addFace = [&](const vec3& v1, const vec3& v2, const vec3& v3, const vec3& v4,
                       const vec3& normal) {
        mesh_->addVertices({
            {v1, normal, pos00, color},
            {v2, normal, pos10, color},
            {v3, normal, pos11, color},
            {v4, normal, pos00, color},
        });
        indexBuffer->add({offset + 0, offset + 1, offset + 2, offset + 0, offset + 2, offset + 3});

        faces_.emplace_back(offset + 0, offset + 1, offset + 2);
        faces_.emplace_back(offset + 0, offset + 2, offset + 3);

        offset += 4;
    };

    addFace(p000, p100, p110, p010, zNorm);
    addFace(p100, p101, p111, p110, xNorm);
    addFace(p010, p110, p111, p011, yNorm);
    addFace(p001, p000, p010, p011, -xNorm);
    addFace(p011, p111, p101, p001, -zNorm);
    addFace(p001, p101, p100, p000, -yNorm);
}

void DeformableCube::deform(const std::function<void(vec3& vertex)>& lambda,
                            const bool& normalize) {
    auto& vertices =
        mesh_->getEditableVertices()->getEditableRAMRepresentation()->getDataContainer();

    if (normalize) {
        auto maxDist = std::numeric_limits<float>::lowest();
        for (auto& v : vertices) {
            lambda(v);
            maxDist = glm::max(glm::length(v), maxDist);
        }
        for (auto& v : vertices) {
            v /= maxDist;
        }
    } else {
        for (auto& v : vertices) {
            lambda(v);
        }
    }

    calculateNormals();
}

void DeformableCube::deform(const std::function<void(vec3& vertex, vec4& color)>& lambda,
                            const bool& normalize) {
    auto& vertices =
        mesh_->getEditableVertices()->getEditableRAMRepresentation()->getDataContainer();
    auto& colors = mesh_->getEditableColors()->getEditableRAMRepresentation()->getDataContainer();

    if (normalize) {
        auto maxDist = std::numeric_limits<float>::lowest();
        size_t i = 0;
        for (auto& v : vertices) {
            lambda(v, colors[i]);
            maxDist = glm::max(glm::length(v), maxDist);
            i++;
        }
        for (auto& v : vertices) {
            v /= maxDist;
        }
    } else {
        size_t i = 0;
        for (auto& v : vertices) {
            lambda(v, colors[i]);
            i++;
        }
    }

    calculateNormals();
}

void DeformableCube::transform(const vec3& pos, const vec3& scale) {
    auto translation = glm::translate(pos);
    auto scaling = glm::scale(vec3(scale));

    mesh_->setWorldMatrix(mat4(1) * translation * scaling);
}

std::shared_ptr<BasicMesh> DeformableCube::getGeometry() {
    return std::make_shared<BasicMesh>(*mesh_);
}

void DeformableCube::calculateNormals() {
    auto& vertices =
        mesh_->getEditableVertices()->getEditableRAMRepresentation()->getDataContainer();
    auto& normals = mesh_->getEditableNormals()->getEditableRAMRepresentation()->getDataContainer();

    for (auto& normal : normals) {
        normal = vec3(0.f);
    }

    for (const auto& face : faces_) {
        auto v1 = vertices[face.x];
        auto v2 = vertices[face.y];
        auto v3 = vertices[face.z];

        auto edge1 = v2 - v1;
        auto edge2 = v3 - v1;

        auto faceNormal = glm::cross(edge1, edge2);

        normals[face.x] += faceNormal;
        normals[face.y] += faceNormal;
        normals[face.z] += faceNormal;
    }

    size_t i = 0;
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
        auto cosAlpha = glm::dot(normal, glm::normalize(vertices[i]));

        if (cosAlpha < 0.f) {
            normal = -normal;
        }
        i++;
    }
}
}  // namespace inviwo