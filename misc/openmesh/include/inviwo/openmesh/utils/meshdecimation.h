/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2021 Inviwo Foundation
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

#ifndef IVW_MESHDECIMATION_H
#define IVW_MESHDECIMATION_H

#include <inviwo/openmesh/openmeshmoduledefine.h>
#include <inviwo/core/common/inviwo.h>

#include <inviwo/core/util/clock.h>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES_WAS_DEFINED
#define _USE_MATH_DEFINES
#endif

#include <warn/push>
#include <warn/ignore/all>
#include <OpenMesh/Core/IO/MeshIO.hh>  // this needs to be included before TriMesh_ArrayKernelT
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <warn/pop>

#ifdef _USE_MATH_DEFINES_WAS_DEFINED
#undef _USE_MATH_DEFINES
#undef _USE_MATH_DEFINES_WAS_DEFINED
#endif

namespace inviwo {

namespace openmeshutil {

//! Utility struct to differentiate between Face and Vertex Fraction in mesh decimation
struct FaceFraction {
    FaceFraction(float fraction = 0.5f) : fraction(fraction) {}
    float fraction;
};

//! Utility struct to differentiate between Face and Vertex Fraction in openmeshutil::decimate
struct VertexFraction {
    VertexFraction(float fraction = 0.5f) : fraction(fraction) {}
    float fraction;
};

/**
 * Utility function to reduce the number of triangles in a mesh.
 * Stops when either the Vertex- or the Face decimation ratio is reach.
 *
 * Example:
 * \snippet modules/misc/openmesh/src/processors/meshdecimationprocessor.cpp OpenMesh Decimation
 *
 * @param mesh An OpenMesh mesh (see fromInviwo(...) and ::toInviwo(...))
 * @param vertexFraction percentage of vertices to keep
 * @param faceFraction percentage of faces to keep
 */
template <typename OMesh>
void decimate(OMesh &mesh, VertexFraction vertexFraction, FaceFraction faceFraction) {
    using Decimater = typename OpenMesh::Decimater::DecimaterT<OMesh>;
    using HModQuadric = typename OpenMesh::Decimater::ModQuadricT<OMesh>::Handle;

    Decimater decimater(mesh);
    HModQuadric hModQuadric;

    decimater.add(hModQuadric);
    decimater.module(hModQuadric).unset_max_err();
    decimater.initialize();
    decimater.decimate_to_faces(static_cast<size_t>(mesh.n_vertices() * vertexFraction.fraction),
                                static_cast<size_t>(mesh.n_faces() * faceFraction.fraction));
    mesh.garbage_collection();
}

/**
 * Utility function to reduce the number of triangles in a mesh
 * Stops when the Face decimation ratio is reach.
 *
 * @param mesh   (@see inviwo::openmeshutil::fromInviwo)
 * @param faceFraction percentage of faces to keep
 */
template <typename Mesh>
void decimate(Mesh &mesh, FaceFraction faceFraction) {
    return decimate(mesh, 0, faceFraction);
}

/**
 * Utility function to reduce the number of triangles in a mesh
 * Stops when the Vertex decimation ratio is reach.
 *
 * @param mesh   (@see inviwo::openmeshutil::fromInviwo)
 * @param vertexFraction percentage of vertices to keep
 */
template <typename Mesh>
void decimate(Mesh &mesh, VertexFraction vertexFraction) {
    return decimate(mesh, vertexFraction, 0);
}

}  // namespace openmeshutil

}  // namespace inviwo

#endif  // IVW_MESHDECIMATION_H
