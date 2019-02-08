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

#ifndef IVW_OPENMESHCONVERTERS_H
#define IVW_OPENMESHCONVERTERS_H

#include <modules/openmesh/openmeshmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/core/datastructures/geometry/mesh.h>
#include <modules/openmesh/utils/meshiteratorutils.h>

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

namespace inviwo {

struct CustomMeshTraits : public OpenMesh::DefaultTraits {
    typedef OpenMesh::Vec4f Color;

    VertexAttributes(OpenMesh::Attributes::Normal | OpenMesh::Attributes::TexCoord3D |
                     OpenMesh::Attributes::Color);
};

using TriMesh = OpenMesh::TriMesh_ArrayKernelT<CustomMeshTraits>;

namespace openmeshutil {

template <typename OM_Mesh>
std::shared_ptr<Mesh> toInviwo(const OM_Mesh &mesh) {
    auto newmesh = std::make_shared<Mesh>();

    {
        using T = typename OM_Mesh::Point;
        using Vec = typename inviwo::Vector<T::dim(), typename T::value_type>;
        auto vertices = std::make_shared<Buffer<Vec>>();
        auto &vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        newmesh->addBuffer(BufferType::PositionAttrib, vertices);
        for (const OpenMesh::VertexHandle &v_it : mesh.vertices()) {
            auto point = mesh.point(v_it);
            Vec pos;
            for (int i = 0; i < T::dim(); i++) {
                pos[i] = point[i];
            }
            vec.push_back(pos);
        }
    }

    if (mesh.has_vertex_colors()) {
        using T = typename OM_Mesh::Color;
        using Vec = typename inviwo::Vector<T::dim(), typename T::value_type>;
        auto vertices = std::make_shared<Buffer<Vec>>();
        auto &vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        newmesh->addBuffer(BufferType::ColorAttrib, vertices);
        for (const OpenMesh::VertexHandle &v_it : mesh.vertices()) {
            auto point = mesh.color(v_it);
            Vec pos;
            for (int i = 0; i < T::dim(); i++) {
                pos[i] = point[i];
            }
            vec.push_back(pos);
        }
    }

    if (mesh.has_vertex_normals()) {
        using T = typename OM_Mesh::Normal;
        using Vec = typename inviwo::Vector<T::dim(), typename T::value_type>;
        auto vertices = std::make_shared<Buffer<Vec>>();
        auto &vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        newmesh->addBuffer(BufferType::NormalAttrib, vertices);
        for (const OpenMesh::VertexHandle &v_it : mesh.vertices()) {
            auto point = mesh.normal(v_it);
            Vec pos;
            for (int i = 0; i < T::dim(); i++) {
                pos[i] = point[i];
            }
            vec.push_back(pos);
        }
    }

    if (mesh.has_vertex_texcoords3D()) {
        using T = typename OM_Mesh::TexCoord3D;
        using Vec = typename inviwo::Vector<T::dim(), typename T::value_type>;
        auto vertices = std::make_shared<Buffer<Vec>>();
        auto &vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        newmesh->addBuffer(BufferType::TexcoordAttrib, vertices);
        for (const OpenMesh::VertexHandle &v_it : mesh.vertices()) {
            auto point = mesh.texcoord3D(v_it);
            Vec pos;
            for (int i = 0; i < T::dim(); i++) {
                pos[i] = point[i];
            }
            vec.push_back(pos);
        }
    } else if (mesh.has_vertex_texcoords2D()) {
        using T = typename OM_Mesh::TexCoord2D;
        using Vec = typename inviwo::Vector<T::dim(), typename T::value_type>;
        auto vertices = std::make_shared<Buffer<Vec>>();
        auto &vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        newmesh->addBuffer(BufferType::TexcoordAttrib, vertices);
        for (const OpenMesh::VertexHandle &v_it : mesh.vertices()) {
            auto point = mesh.texcoord2D(v_it);
            Vec pos;
            for (int i = 0; i < T::dim(); i++) {
                pos[i] = point[i];
            }
            vec.push_back(pos);
        }
    } else if (mesh.has_vertex_texcoords1D()) {
        using T = typename OM_Mesh::TexCoord1D;
        auto vertices = std::make_shared<Buffer<T>>();
        auto &vec = vertices->getEditableRAMRepresentation()->getDataContainer();
        newmesh->addBuffer(BufferType::TexcoordAttrib, vertices);
        for (const OpenMesh::VertexHandle &v_it : mesh.vertices()) {
            vec.push_back(mesh.texcoord1D(v_it));
        }
    }

    auto indicesRam = std::make_shared<IndexBufferRAM>();
    auto indices = std::make_shared<IndexBuffer>(indicesRam);
    auto &indVec = indicesRam->getDataContainer();
    newmesh->addIndicies(Mesh::MeshInfo(DrawType::Triangles, ConnectivityType::None), indices);
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

IVW_MODULE_OPENMESH_API TriMesh
fromInviwo(const Mesh &inmesh, TransformCoordinates transform = TransformCoordinates::DataToWorld);

}  // namespace openmeshutil

}  // namespace inviwo

#endif  // IVW_OPENMESHCONVERTERS_H
