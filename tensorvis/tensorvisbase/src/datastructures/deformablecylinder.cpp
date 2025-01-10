/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#include <inviwo/tensorvisbase/datastructures/deformablecylinder.h>

namespace inviwo {
DeformableCylinder::DeformableCylinder(const size_t& numTheta, const vec4& color) {
    createCylinder(numTheta, color);

    calculateNormals();
}

void DeformableCylinder::createCylinder(const size_t& numTheta, const vec4& color) {
    mesh_ = std::make_shared<BasicMesh>();

    auto rotation_matrix = mat3(glm::rotate(mat4(1), static_cast<float>(M_PI / 2.), vec3(1, 0, 0)));

    auto indexBuffer = mesh_->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);

    // 2 for the centers plus the first one on the ring with 2 normals respectively
    glm::uint32_t num_indices = 0;

    const auto frac = static_cast<float>((2. * M_PI) / static_cast<float>(numTheta));

    const auto top_center = vec3(0.5f) - vec3(0.5f);
    const auto bot_center = vec3(0.5f, -0.5f, 0.5f) - vec3(0.5f);
    const auto top_start = vec3(0.5f, 0.5f, 1.0f) - vec3(0.5f);
    const auto bot_start = vec3(0.5f, -0.5f, 1.0f) - vec3(0.5f);
    const auto rot_vector = vec3(0.0f, 0.0f, 0.5f);

    mesh_->addVertices({{rotation_matrix * top_center,
                         glm::normalize(rotation_matrix * vec3(0, 1, 0)), vec3(), color},
                        {rotation_matrix * bot_center,
                         glm::normalize(rotation_matrix * vec3(0, -1, 0)), vec3(), color},
                        {rotation_matrix * top_start,
                         glm::normalize(rotation_matrix * vec3(0, 1, 0)), vec3(), color},
                        {rotation_matrix * bot_start,
                         glm::normalize(rotation_matrix * vec3(0, -1, 0)), vec3(), color},
                        {rotation_matrix * top_start,
                         glm::normalize(rotation_matrix * vec3(0, 0, 1)), vec3(), color},
                        {rotation_matrix * bot_start,
                         glm::normalize(rotation_matrix * vec3(0, 0, 1)), vec3(), color}});

    num_indices += 6;

    for (glm::uint32_t i = 1; i < numTheta; i++) {
        const auto cur_vec = glm::rotateY(rot_vector, i * frac);

        mesh_->addVertices({
            {rotation_matrix * (top_center + cur_vec),
             glm::normalize(rotation_matrix * vec3(0, 1, 0)), vec3(), color},  // 0  top y dir
            {rotation_matrix * (bot_center + cur_vec),
             glm::normalize(rotation_matrix * vec3(0, -1, 0)), vec3(), color},  // 1  bot y dir
            {rotation_matrix * (top_center + cur_vec), glm::normalize(rotation_matrix * cur_vec),
             vec3(), color},  // 2  top out dir
            {rotation_matrix * (bot_center + cur_vec), glm::normalize(rotation_matrix * cur_vec),
             vec3(), color}  // 3  bot out dir
        });

        num_indices += 4;

        indexBuffer->add({0, num_indices - 4, num_indices - 4 - 4});
        faces_.emplace_back(0, num_indices - 4, num_indices - 4 - 4);

        indexBuffer->add({1, num_indices - 4 - 3, num_indices - 3});
        faces_.emplace_back(1, num_indices - 4 - 3, num_indices - 3);

        auto v1 = num_indices - 2;
        auto v2 = num_indices - 2 - 4;
        auto v3 = num_indices - 1;
        auto v4 = num_indices - 1 - 4;

        indexBuffer->add({v3, v2, v1});
        faces_.emplace_back(v3, v2, v1);

        indexBuffer->add({v3, v4, v2});
        faces_.emplace_back(v3, v4, v2);
    }

    indexBuffer->add({0, 2, num_indices - 4});
    faces_.emplace_back(0, 2, num_indices - 4);

    indexBuffer->add({1, num_indices - 3, 3});
    faces_.emplace_back(1, num_indices - 3, 3);

    indexBuffer->add({5, num_indices - 2, 4});
    faces_.emplace_back(5, num_indices - 2, 4);

    indexBuffer->add({5, num_indices - 1, num_indices - 2});
    faces_.emplace_back(5, num_indices - 1, num_indices - 2);
}

void DeformableCylinder::deform(const std::function<void(vec3& vertex)>& lambda,
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

void DeformableCylinder::deform(const std::function<void(vec3& vertex, vec4& color)>& lambda,
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

void DeformableCylinder::transform(const vec3& pos, const vec3& scale) {
    auto translation = glm::translate(pos);
    auto scaling = glm::scale(vec3(scale));

    mesh_->setWorldMatrix(mat4(1) * translation * scaling);
}

std::shared_ptr<BasicMesh> DeformableCylinder::getGeometry() {
    return std::make_shared<BasicMesh>(*mesh_);
}

void DeformableCylinder::calculateNormals() {
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

    for (auto& normal : normals) normal = -glm::normalize(normal);
}
}  // namespace inviwo