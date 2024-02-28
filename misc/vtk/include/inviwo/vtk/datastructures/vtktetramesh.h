/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023-2024 Inviwo Foundation
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

#include <inviwo/vtk/vtkmoduledefine.h>
#include <inviwo/tetramesh/datastructures/tetramesh.h>

#include <inviwo/vtk/util/vtkbufferutils.h>

#include <vector>

class vtkUnstructuredGrid;

namespace inviwo {

/**
 * \ingroup datastructures
 * \brief Data required for rendering a vtkUnstructuredGrid as tetrahedral mesh
 *
 * Provides an interface between a vtkUnstructuredGrid and the data structures required for
 * rendering a tetrahedral mesh.
 *
 * Data structures for tetrahedra indexing and face enumeration based on
 *    M. Lage, T. Lewiner, H. Lopes, and L. Velho.
 *    CHF: A scalable topological data structure for tetrahedral meshes.
 *    In Brazilian Symposium on Computer Graphics and Image Processing
 *    (SIBGRAPI'05), pp. 349-356, 2005, doi: 10.1109/SIBGRAPI.2005.18
 */
class IVW_MODULE_VTK_API VTKTetraMesh : public TetraMesh {
public:
    VTKTetraMesh(utilvtk::ArrayBufferMapper& bufferMapper, vtkUnstructuredGrid* vtkData = nullptr);
    virtual VTKTetraMesh* clone() const override;
    virtual ~VTKTetraMesh() = default;

    void setData(vtkUnstructuredGrid* vtkData);

    virtual int getNumberOfCells() const override;
    virtual int getNumberOfPoints() const override;

    /**
     * @copydoc TetraMesh::get
     */
    virtual void get(std::vector<vec4>& nodes, std::vector<ivec4>& nodeIds) const override;

    /**
     * @copydoc TetraMesh::getBoundingBox
     */
    virtual mat4 getBoundingBox() const override;

    /**
     * @copydoc TetraMesh::getDataRange
     */
    virtual dvec2 getDataRange() const override;

    /**
     * @copydoc SpatialEntity::getAxis
     */
    virtual const Axis* getAxis(size_t index) const override;

    std::array<Axis, 3> axes;

private:
    utilvtk::ArrayBufferMapper& bufferMapper_;
    vtkUnstructuredGrid* vtkData_;
};

}  // namespace inviwo
