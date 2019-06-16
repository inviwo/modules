/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
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

#ifndef IVW_TTK_UTILS_H
#define IVW_TTK_UTILS_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/datastructures/triangulationdata.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/volume/volume.h>

#include <vector>

namespace inviwo {

namespace topology {

/**
 * \brief convert a Mesh to TriangulationData
 *
 * Convert the position buffer of \p mesh to TriangulationData which provides a
 * ttk::Triangulation. Supports lines and triangles (strips, loops, adjacency).
 * The scalar values can be set using TriangulationData::setScalarValues().
 *
 * \see TriangulationData::setScalarValues
 *
 * @param mesh   input Mesh
 * @return TriangulationData with ttk::Triangulation
 * @throw TTKConversionException if conversion fails, e.g. the mesh does not have a position buffer
 */
IVW_MODULE_TOPOLOGYTOOLKIT_API TriangulationData meshToTTKTriangulation(const Mesh& mesh);

/**
 * \brief convert TriangulationData into a Mesh
 *
 * Convert TriangulationData \p data into a Mesh, scalars can optionally overwrite one
 * position component
 *
 * TODO: mesh normals based on neighborhood information
 *
 * @param data    triangulation data
 * @param color   used for coloring all vertices
 * @param applyScalars  if true, scalar values will overwrite one position component
 * @param component  scalar values overwrite this component of the vertex positions, i.e. x, y, or z
 * @return Mesh of given triangulation and color
 */
IVW_MODULE_TOPOLOGYTOOLKIT_API
std::shared_ptr<Mesh> ttkTriangulationToMesh(const TriangulationData& data,
                                             const vec4& color = vec4(1.0f),
                                             bool applyScalars = false, size_t component = 0);

/**
 * \brief convert a Volume to TriangulationData
 *
 * Convert \p volume into TriangulationData which provides a ttk::Triangulation. Channel \p
 * channel of the volume is used as input for the scalar values associated with the triangulation.
 *
 * \see TriangulationData::setScalarValues
 *
 * @param volume   input volume
 * @param channel  channel of input volume used as scalar data for the triangulation, if valid
 * @return TriangulationData with ttk::Triangulation
 * @throw TTKConversionException if conversion fails
 */
IVW_MODULE_TOPOLOGYTOOLKIT_API TriangulationData volumeToTTKTriangulation(const Volume& volume,
                                                                          size_t channel);

/**
 * \brief convert TriangulationData into a Volume
 *
 * @param data    triangulation data
 * @return Volume corresponding to the given triangulation
 * @throw TTKConversionException if the triangulation data is not implicit, i.e. not a uniform grid
 */
IVW_MODULE_TOPOLOGYTOOLKIT_API
std::shared_ptr<Volume> ttkTriangulationToVolume(const TriangulationData& data);

}  // namespace topology

}  // namespace inviwo

#endif  // IVW_TTK_UTILS_H
