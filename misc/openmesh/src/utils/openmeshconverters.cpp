/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
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

#include <modules/openmesh/utils/openmeshconverters.h>

#include <inviwo/core/datastructures/geometry/typedmesh.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/datastructures/geometry/simplemesh.h>

namespace inviwo {
namespace openmeshutil {
namespace detail {

void createVertexBuffers(TriMesh &mesh, const BasicMesh &inmesh, TransformCoordinates transform) {
    auto &vertices = inmesh.getVertices()->getRAMRepresentation()->getDataContainer();
    auto &normals = inmesh.getNormals()->getRAMRepresentation()->getDataContainer();
    auto &texCoords = inmesh.getTexCoords()->getRAMRepresentation()->getDataContainer();
    auto &colors = inmesh.getColors()->getRAMRepresentation()->getDataContainer();

    mat4 m;
    if (transform == TransformCoordinates::DataToModel) {
        m = inmesh.getCoordinateTransformer().getDataToModelMatrix();
    } else if (transform == TransformCoordinates::DataToWorld) {
        m = inmesh.getCoordinateTransformer().getDataToWorldMatrix();
    }

    for (auto &&vert : util::zip(vertices, normals, texCoords, colors)) {
        auto &pos = get<0>(vert);
        auto &normal = get<1>(vert);
        auto &texCoord = get<2>(vert);
        auto &color = get<3>(vert);
        auto i = [&]() {
            if (transform == TransformCoordinates::NoTransform) {
                return mesh.add_vertex({pos.x, pos.y, pos.z});
            } else {
                auto pos4 = m * vec4{pos, 1};
                return mesh.add_vertex({pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w});
            }
        }();

        mesh.set_normal(i, {normal.x, normal.y, normal.z});
        mesh.set_texcoord3D(i, {texCoord.x, texCoord.y, texCoord.z});
        mesh.set_color(i, {color.r, color.g, color.b, color.a});
    }
}

void createVertexBuffers(TriMesh &mesh, const SimpleMesh &inmesh, TransformCoordinates transform) {

    auto &vertices = inmesh.getVertexList()->getRAMRepresentation()->getDataContainer();
    auto &texCoords = inmesh.getTexCoordList()->getRAMRepresentation()->getDataContainer();
    auto &colors = inmesh.getColorList()->getRAMRepresentation()->getDataContainer();

    mat4 m;
    if (transform == TransformCoordinates::DataToModel) {
        m = inmesh.getCoordinateTransformer().getDataToModelMatrix();
    } else if (transform == TransformCoordinates::DataToWorld) {
        m = inmesh.getCoordinateTransformer().getDataToWorldMatrix();
    }

    for (auto &&vert : util::zip(vertices, texCoords, colors)) {
        auto &pos = get<0>(vert);
        auto &texCoord = get<1>(vert);
        auto &color = get<2>(vert);
        auto i = [&]() {
            if (transform == TransformCoordinates::NoTransform) {
                return mesh.add_vertex({pos.x, pos.y, pos.z});
            } else {
                auto pos4 = m * vec4{pos, 1};
                return mesh.add_vertex({pos4.x / pos4.w, pos4.y / pos4.w, pos4.z / pos4.w});
            }
        }();
        mesh.set_texcoord3D(i, {texCoord.x, texCoord.y, texCoord.z});
        mesh.set_color(i, {color.r, color.g, color.b, color.a});
    }
}

void createVertexBuffers(TriMesh &mesh, const Mesh &inmesh, TransformCoordinates transform) {

    mat4 m;
    if (transform == TransformCoordinates::DataToModel) {
        m = inmesh.getCoordinateTransformer().getDataToModelMatrix();
    } else if (transform == TransformCoordinates::DataToWorld) {
        m = inmesh.getCoordinateTransformer().getDataToWorldMatrix();
    }

    for (auto &buf : inmesh.getBuffers()) {
        if (buf.first.type == BufferType::PositionAttrib) {
            auto loop = [&](auto buf, auto t) {
                for (auto &v1 : buf->getRAMRepresentation()->getDataContainer()) {
                    auto v = t(v1);
                    mesh.add_vertex({v.x, v.y, v.z});
                }
            };

            if (auto b4 = std::dynamic_pointer_cast<const Buffer<vec4>>(buf.second)) {
                loop(b4, [&m](auto v) { return m * v; });
            } else if (auto b3 = std::dynamic_pointer_cast<const Buffer<vec3>>(buf.second)) {
                loop(b3, [&m](auto v) {
                    auto tmp = m * vec4{v, 1.f};
                    return vec3(tmp) / tmp.w;
                });
            } else {
                throw inviwo::Exception("Unknown position buffer type",
                                        IvwContextCustom("openmeshutil::meshHelper"));
            }
        }
    }
    using VH = OpenMesh::VertexHandle;
    // Make a second loop for other buffers, to make sure the Positions has been added first
    for (auto &buf : inmesh.getBuffers()) {
        if (buf.first.type == BufferType::NormalAttrib) {
            if (auto b3 = std::dynamic_pointer_cast<const Buffer<vec3>>(buf.second)) {
                int i = 0;
                for (auto &n : b3->getRAMRepresentation()->getDataContainer()) {
                    mesh.set_normal(VH(i++), {n.x, n.y, n.z});
                }
            } else {
                throw inviwo::Exception("Unknown normals buffer type",
                                        IvwContextCustom("openmeshutil::meshHelper"));
            }
        } else if (buf.first.type == BufferType::ColorAttrib) {
            auto loop = [&](auto &buf, auto toVec4) {
                int i = 0;
                for (auto &v : buf->getRAMRepresentation()->getDataContainer()) {
                    auto c = toVec4(v);
                    mesh.set_color(VH(i++), {c.x, c.y, c.z, c.a});
                }
            };

            if (auto f3 = std::dynamic_pointer_cast<const Buffer<vec3>>(buf.second)) {
                loop(f3, [](vec3 v) { return vec4(v, 1); });
            } else if (auto f4 = std::dynamic_pointer_cast<const Buffer<vec4>>(buf.second)) {
                loop(f4, [](vec4 v) { return v; });
            } else if (auto u4 = std::dynamic_pointer_cast<const Buffer<glm::u8vec4>>(buf.second)) {
                loop(u4, [](glm::u8vec4 v) { return vec4(v) / 255.0f; });
            } else if (auto u3 = std::dynamic_pointer_cast<const Buffer<glm::u8vec3>>(buf.second)) {
                loop(u3, [](glm::u8vec3 v) { return vec4(v, 255.0f) / 255.0f; });
            } else {
                throw inviwo::Exception("Unknown normals buffer type",
                                        IvwContextCustom("openmeshutil::meshHelper"));
            }
        } else if (buf.first.type == BufferType::TexcoordAttrib) {
            if (auto t1 = std::dynamic_pointer_cast<const Buffer<float>>(buf.second)) {
                int i = 0;
                for (auto &v : t1->getRAMRepresentation()->getDataContainer()) {
                    mesh.set_texcoord3D(VH(i++), {v, 0, 0});
                }
            } else if (auto t2 = std::dynamic_pointer_cast<const Buffer<vec2>>(buf.second)) {
                int i = 0;
                for (auto &v : t2->getRAMRepresentation()->getDataContainer()) {
                    mesh.set_texcoord3D(VH(i++), {v.x, v.y, 0});
                }
            } else if (auto t3 = std::dynamic_pointer_cast<const Buffer<vec3>>(buf.second)) {
                int i = 0;
                for (auto &v : t3->getRAMRepresentation()->getDataContainer()) {
                    mesh.set_texcoord3D(VH(i++), {v.x, v.y, v.z});
                }
            } else if (auto t4 = std::dynamic_pointer_cast<const Buffer<vec4>>(buf.second)) {
                int i = 0;
                for (auto &v : t4->getRAMRepresentation()->getDataContainer()) {
                    mesh.set_texcoord3D(VH(i++), {v.x, v.y, v.z});
                }
            } else {
                throw inviwo::Exception("Unknown normals buffer type",
                                        IvwContextCustom("openmeshutil::meshHelper"));
            }
        }
    }
}

}  // namespace detail

TriMesh fromInviwo(const Mesh &inmesh, TransformCoordinates transform) {
    TriMesh mesh;

    if (auto bm = dynamic_cast<const BasicMesh *>(&inmesh)) {
        detail::createVertexBuffers(mesh, *bm, transform);
    } else if (auto sm = dynamic_cast<const SimpleMesh *>(&inmesh)) {
        detail::createVertexBuffers(mesh, *sm, transform);
    } else {
        detail::createVertexBuffers(mesh, inmesh, transform);
    }

    for (auto &ib : inmesh.getIndexBuffers()) {
        if (ib.first.dt == DrawType::Triangles) {
            meshutil::forEachTriangle(ib.first, *ib.second,
                                      [&mesh](uint32_t i0, uint32_t i1, uint32_t i2) {
                                          using VH = OpenMesh::VertexHandle;
                                          mesh.add_face(VH(i0), VH(i1), VH(i2));
                                      });
        }
    }

    return mesh;
}

}  // namespace openmeshutil
}  // namespace inviwo
