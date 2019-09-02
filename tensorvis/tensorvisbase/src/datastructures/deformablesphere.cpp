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

#include <modules/tensorvisbase/datastructures/deformablesphere.h>

namespace inviwo {
DeformableSphere::DeformableSphere(const size_t& numTheta, const size_t& numPhi,
                                   const vec4& color) {
    createSphere(numTheta, numPhi, color);

    calculateNormals();
}

void DeformableSphere::createSphere(const size_t& numTheta, const size_t& numPhi,
                                    const vec4& color) {
    mesh_ = std::make_shared<BasicMesh>();

    auto nFaces = (numPhi - 3) * (numTheta - 1) * 2 + (numPhi - 3) * 2 + 2 * ((numTheta - 1) + 1);

    auto indexBuffer = mesh_->addIndexBuffer(DrawType::Triangles, ConnectivityType::None);
    indexBuffer->getDataContainer().reserve(nFaces * 3);

    auto nIndices = (numPhi - 1) * numTheta + 2;
    std::vector<uint32_t> addedIndices;
    addedIndices.reserve(nIndices);

    auto& vertices =
        mesh_->getEditableVertices()->getEditableRAMRepresentation()->getDataContainer();
    auto& normals = mesh_->getEditableNormals()->getEditableRAMRepresentation()->getDataContainer();
    auto& texCoords =
        mesh_->getEditableTexCoords()->getEditableRAMRepresentation()->getDataContainer();
    auto& colors = mesh_->getEditableColors()->getEditableRAMRepresentation()->getDataContainer();

    auto totalVertices = (numPhi - 1) * numTheta + 2;  // 2 extreme points
    vertices.reserve(totalVertices);
    normals.reserve(totalVertices);
    texCoords.reserve(totalVertices);
    colors.reserve(totalVertices);

    faces_.clear();
    faces_.reserve(nFaces);

    auto calcVert = [](vec3 & vertex, const float& cosphi, const float& sinphi,
                       const float& costheta, const float& sintheta) -> auto {
        auto sgnsinphi = int(glm::sign(sinphi));
        auto sgncosphi = int(glm::sign(cosphi));
        auto sgnsintheta = int(glm::sign(sintheta));
        auto sgncostheta = int(glm::sign(costheta));

        vertex.x = (sgncostheta * std::abs(costheta)) * (sgnsinphi * std::abs(sinphi));
        vertex.y = (sgnsintheta * std::abs(sintheta)) * (sgnsinphi * std::abs(sinphi));
        vertex.z = sgncosphi * std::abs(cosphi);
    };
    auto addVertex = [&](const vec3& vertex, const vec3& texCoord, const vec3& normal,
                         const vec4& color) -> auto {
        vertices.emplace_back(vertex);
        normals.emplace_back(normal);
        texCoords.emplace_back(texCoord);
        colors.emplace_back(color);
        return static_cast<uint32_t>(vertices.size() - 1);
    };

    // Generate main geometry body
    for (size_t j = 1; j < numPhi - 1; j++) {
        for (size_t i = 0; i < numTheta; i++) {
            auto theta = static_cast<float>(i) * 2.f * static_cast<float>(M_PI) /
                         (static_cast<float>(numTheta));
            auto phi = static_cast<float>(j) * static_cast<float>(M_PI) /
                       (static_cast<float>(numPhi) - 1.f);

            auto sinphi = std::sin(phi);
            auto cosphi = std::cos(phi);
            auto sintheta = std::sin(theta);
            auto costheta = std::cos(theta);

            vec3 vertex;

            calcVert(vertex, cosphi, sinphi, costheta, sintheta);

            addedIndices.emplace_back(addVertex(vertex, vec3(0), vec3(0), color));
        }
    }

    vec3 vertex;

    calcVert(vertex, 1.f, 0.f, 1.f, 0.f);

    addedIndices.emplace_back(addVertex(vertex, vec3(0), vec3(0), color));

    // Generate second extreme point
    auto theta = (static_cast<float>(numTheta) - 1.f) * 2.f * static_cast<float>(M_PI) /
                 (static_cast<float>(numTheta) - 1.f);
    auto phi = (static_cast<float>(numPhi) - 1.f) * static_cast<float>(M_PI) /
               (static_cast<float>(numPhi) - 1);

    auto sinphi = std::sin(phi);
    auto cosphi = std::cos(phi);

    auto sintheta = std::sin(theta);
    auto costheta = std::cos(theta);

    calcVert(vertex, cosphi, sinphi, costheta, sintheta);

    addedIndices.emplace_back(addVertex(vertex, vec3(0), vec3(0), color));

    // Tesselate main geometry
    // First, we create the triangles for the "sides"
    for (size_t j = 0; j < numPhi - 3; j++) {
        for (size_t i = 0; i < numTheta - 1; i++) {
            auto firstItemInFirstRow = numTheta * j;
            auto firstItemInSecondRow = numTheta * (j + 1);

            auto idx1 = firstItemInSecondRow + i;     // 0,0
            auto idx2 = firstItemInFirstRow + 1 + i;  // 1,1
            auto idx3 = firstItemInFirstRow + i;      // 0,1

            auto idx4 = firstItemInFirstRow + 1 + i;
            auto idx5 = firstItemInSecondRow + i;
            auto idx6 = firstItemInSecondRow + 1 + i;

            indexBuffer->add(addedIndices.at(idx1));
            indexBuffer->add(addedIndices.at(idx2));
            indexBuffer->add(addedIndices.at(idx3));
            faces_.emplace_back(addedIndices.at(idx1), addedIndices.at(idx2),
                                addedIndices.at(idx3));
            indexBuffer->add(addedIndices.at(idx4));
            indexBuffer->add(addedIndices.at(idx5));
            indexBuffer->add(addedIndices.at(idx6));
            faces_.emplace_back(addedIndices.at(idx4), addedIndices.at(idx5),
                                addedIndices.at(idx6));
        }
    }

    for (size_t j = 0; j < numPhi - 3; j++) {
        auto firstItemInFirstRow = j * numTheta;
        auto firstItemInSecondRow = (j + 1) * numTheta;
        auto lastItemInFirstRow = (j * numTheta) + (numTheta - 1);
        auto lastItemInSecondRow = ((j + 1) * numTheta) + (numTheta - 1);

        indexBuffer->add(addedIndices.at(lastItemInSecondRow));
        indexBuffer->add(addedIndices.at(firstItemInFirstRow));
        indexBuffer->add(addedIndices.at(lastItemInFirstRow));
        faces_.emplace_back(addedIndices.at(lastItemInSecondRow),
                            addedIndices.at(firstItemInFirstRow),
                            addedIndices.at(lastItemInFirstRow));

        indexBuffer->add(addedIndices.at(firstItemInFirstRow));
        indexBuffer->add(addedIndices.at(lastItemInSecondRow));
        indexBuffer->add(addedIndices.at(firstItemInSecondRow));
        faces_.emplace_back(addedIndices.at(firstItemInFirstRow),
                            addedIndices.at(lastItemInSecondRow),
                            addedIndices.at(firstItemInSecondRow));
    }

    // Tesselate first extreme point
    // Now, we need to create the triangles connected to the extreme points
    auto extremePoint1 = addedIndices.at(addedIndices.size() - 2);

    for (size_t i = 0; i < numTheta - 1; i++) {
        indexBuffer->add(extremePoint1);
        indexBuffer->add(addedIndices.at(i));
        indexBuffer->add(addedIndices.at((i + 1)));
        faces_.emplace_back(extremePoint1, addedIndices.at(i), addedIndices.at((i + 1)));
    }

    indexBuffer->add(extremePoint1);
    indexBuffer->add(addedIndices.at(numTheta - 1));
    indexBuffer->add(0);
    faces_.emplace_back(extremePoint1, addedIndices.at(numTheta - 1), addedIndices.at(0));

    // Tesselate second extreme point
    // Now, we need to create the triangles connected to the extreme points
    auto extremePoint2 = addedIndices.at(addedIndices.size() - 1);

    for (size_t i = 0; i < numTheta - 1; i++) {
        indexBuffer->add(extremePoint2);
        indexBuffer->add(addedIndices.at((i + 1) + numTheta * (numPhi - 3)));
        indexBuffer->add(addedIndices.at(i + numTheta * (numPhi - 3)));
        faces_.emplace_back(extremePoint2, addedIndices.at((i + 1) + numTheta * (numPhi - 3)),
                            addedIndices.at(i + numTheta * (numPhi - 3)));
    }

    indexBuffer->add(extremePoint2);

    indexBuffer->add(addedIndices.at(numTheta * (numPhi - 2) - numTheta));
    indexBuffer->add(addedIndices.at(numTheta * (numPhi - 2) - 1));

    faces_.emplace_back(extremePoint2, addedIndices.at(numTheta * (numPhi - 2) - numTheta),
                        addedIndices.at(numTheta * (numPhi - 2) - 1));
}

void DeformableSphere::deform(const std::function<void(vec3& vertex)>& lambda,
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

void DeformableSphere::deform(const std::function<void(vec3& vertex, vec4& color)>& lambda,
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

void DeformableSphere::transform(const vec3& pos, const vec3& scale) {
    auto translation = glm::translate(pos);
    auto scaling = glm::scale(vec3(scale));

    mesh_->setWorldMatrix(mat4(1) * translation * scaling);
}

std::shared_ptr<BasicMesh> DeformableSphere::getGeometry() {
    return std::make_shared<BasicMesh>(*mesh_);
}

void DeformableSphere::calculateNormals() {
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