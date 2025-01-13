/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2025 Inviwo Foundation
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

#ifndef IVW_OPENMESHCONVERTERS_H
#define IVW_OPENMESHCONVERTERS_H

#include <inviwo/openmesh/openmeshmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <modules/base/algorithm/meshutils.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES_WAS_DEFINED
#define _USE_MATH_DEFINES
#endif

#include <warn/push>
#include <warn/ignore/all>
#include <OpenMesh/Core/IO/MeshIO.hh>  // this needs to be included before TriMesh_ArrayKernelT
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <warn/pop>

#ifdef _USE_MATH_DEFINES_WAS_DEFINED
#undef _USE_MATH_DEFINES
#endif

#include <functional>

namespace inviwo {

struct CustomMeshTraits : public OpenMesh::DefaultTraits {
    using Color = OpenMesh::Vec4f;

    VertexAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::TexCoord3D |
                     OpenMesh::Attributes::Color);
};

using TriMesh = OpenMesh::TriMesh_ArrayKernelT<CustomMeshTraits>;

namespace openmeshutil {

namespace detail {

template <typename T, typename OM_Mesh, typename Func>
void convertOMtoInviwoBuffer(inviwo::Mesh& ivwMesh, const OM_Mesh& omMesh, BufferType bufferType,
                             Func callback) {
    if constexpr (std::is_arithmetic_v<T>) {
        auto vertices = std::make_shared<Buffer<T>>();
        auto& vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        ivwMesh.addBuffer(bufferType, vertices);
        for (const OpenMesh::VertexHandle& v_it : omMesh.vertices()) {
            vec.push_back(callback(v_it));
        }
    } else {
        using value_type = glm::vec<T::dim(), typename T::value_type, glm::defaultp>;
        auto convertValue = [](const T& v) {
            value_type res{0};
            for (int i = 0; i < T::dim(); i++) {
                res[i] = v[i];
            }
            return res;
        };

        auto vertices = std::make_shared<Buffer<value_type>>();
        auto& vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        ivwMesh.addBuffer(bufferType, vertices);
        for (const OpenMesh::VertexHandle& v_it : omMesh.vertices()) {
            vec.push_back(convertValue(callback(v_it)));
        }
    }
};

}  // namespace detail

/**
 * Convert a mesh from OpenMesh- to Inviwo format
 */
template <typename OM_Mesh>
std::shared_ptr<Mesh> toInviwo(const OM_Mesh& mesh) {
    using namespace std::placeholders;
    auto newmesh = std::make_shared<Mesh>();

    detail::convertOMtoInviwoBuffer<typename OM_Mesh::Point>(
        *newmesh, mesh, BufferType::PositionAttrib, [m = &mesh](auto it) { return m->point(it); });

    if (mesh.has_vertex_colors()) {
        detail::convertOMtoInviwoBuffer<typename OM_Mesh::Color>(
            *newmesh, mesh, BufferType::ColorAttrib, [m = &mesh](auto it) { return m->color(it); });
    }

    if (mesh.has_vertex_normals()) {
        detail::convertOMtoInviwoBuffer<typename OM_Mesh::Normal>(
            *newmesh, mesh, BufferType::NormalAttrib,
            [m = &mesh](auto it) { return m->normal(it); });
    }

    if (mesh.has_vertex_texcoords3D()) {
        detail::convertOMtoInviwoBuffer<typename OM_Mesh::TexCoord3D>(
            *newmesh, mesh, BufferType::TexCoordAttrib,
            [m = &mesh](auto it) { return m->texcoord3D(it); });
    } else if (mesh.has_vertex_texcoords2D()) {
        detail::convertOMtoInviwoBuffer<typename OM_Mesh::TexCoord2D>(
            *newmesh, mesh, BufferType::TexCoordAttrib,
            [m = &mesh](auto it) { return m->texcoord2D(it); });
    } else if (mesh.has_vertex_texcoords1D()) {
        detail::convertOMtoInviwoBuffer<typename OM_Mesh::TexCoord1D>(
            *newmesh, mesh, BufferType::TexCoordAttrib,
            [m = &mesh](auto it) { return m->texcoord1D(it); });
    }

    auto indicesRam = std::make_shared<IndexBufferRAM>();
    auto indices = std::make_shared<IndexBuffer>(indicesRam);
    auto& indVec = indicesRam->getDataContainer();
    newmesh->addIndices(Mesh::MeshInfo(DrawType::Triangles, ConnectivityType::None), indices);
    size_t skipped = 0;
    for (auto f_it : mesh.faces()) {
        if (mesh.valence(f_it) != 3) {
            skipped++;
            continue;
        }

        for (auto i = mesh.cfv_begin(f_it); i != mesh.cfv_end(f_it); i++) {
            indVec.push_back(i->idx());
        }
    }
    if (skipped) {
        LogWarnCustom("openmeshutil::toInviwo",
                      "Skipped " << skipped << " faces since they weren't triangles");
    }
    return newmesh;
}

enum class TransformCoordinates { NoTransform, DataToModel, DataToWorld };

/**
 * Convert a mesh from Inviwo- to OpenMesh format
 */
IVW_MODULE_OPENMESH_API TriMesh
fromInviwo(const Mesh& inmesh, TransformCoordinates transform = TransformCoordinates::DataToWorld);

}  // namespace openmeshutil

}  // namespace inviwo

#endif  // IVW_OPENMESHCONVERTERS_H
