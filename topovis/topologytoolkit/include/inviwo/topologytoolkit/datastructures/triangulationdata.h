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

#ifndef IVW_TRIANGULATIONDATA_H
#define IVW_TRIANGULATIONDATA_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/utils/ttkexception.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/datatraits.h>
#include <inviwo/core/datastructures/datamapper.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/core/util/document.h>
#include <inviwo/core/metadata/metadataowner.h>
#include <inviwo/core/datastructures/spatialdata.h>

#include <warn/push>
#include <warn/ignore/all>
#include <ttk/core/base/triangulation/Triangulation.h>
#include <warn/pop>

#include <memory>
#include <vector>
#include <sstream>

namespace inviwo {

namespace topology {

/**
 * \group datastructures
 * \class TriangulationData
 * \brief datastructure for regular triangulation used by TTK.
 *
 * At the moment, TTK internally only supports float positions even though ttk::Triangulation might
 * hold doubles. When accessing the point data, it is converted to float. See
 * ttk::ExplicitTriangulation::getVertexPoint().
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TriangulationData : public SpatialEntity<3>,
                                                         public MetaDataOwner {
public:
    enum class InputTriangulation { Edges, Triangles, Tetrahedra };

    TriangulationData() = default;
    /**
     * create an implicit triangulation data from a uniform grid
     *
     * @param dims    dimensions of uniform grid
     * @param origin  origin of the grid
     * @param extent  extent of the entire grid
     */
    TriangulationData(const size3_t& dims, const vec3& origin, const vec3& extent,
                      const DataMapper& dataMapper);
    /**
     * create a triangulation from a list of edges, triangles, or tetrahedra
     */
    TriangulationData(std::vector<vec3> points, const std::vector<uint32_t>& indices,
                      InputTriangulation type = InputTriangulation::Triangles);
    TriangulationData(std::vector<vec3> points, const std::vector<uint32_t>& indices,
                      Mesh::MeshInfo meshInfo);
    TriangulationData(const TriangulationData& rhs);
    TriangulationData(TriangulationData&& rhs);
    ~TriangulationData() = default;

    TriangulationData& operator=(const TriangulationData& rhs);
    TriangulationData& operator=(TriangulationData&& rhs);

    virtual TriangulationData* clone() const;

    /**
     * query state of triangulation whether it is implicit, i.e. a uniform grid, or explicit
     *
     * \see set(const size3_t&, const vec3&, const vec3&)
     *
     * @return true if the triangulation is implicit was created for a uniform grid
     */
    bool isUniformGrid() const;
    /**
     * query data mapper used when creating implicit triangulation
     *
     * @return DataMapper used during creation and triangulation is implicit, DataMapper() otherwise
     */
    DataMapper getDataMapper() const;

    /**
     * query grid dimensions of implicit triangulation
     *
     * @return grid dimensions if triangulation is implicit, size3_t(0) otherwise
     */
    size3_t getGridDimensions() const;
    /**
     * query grid origin of implicit triangulation
     *
     * @return grid origin if triangulation is implicit, vec3(0) otherwise
     */
    vec3 getGridOrigin() const;
    /**
     * query grid extent of implicit triangulation
     *
     * @return grid extent if triangulation is implicit, vec3(0) otherwise
     */
    vec3 getGridExtent() const;

    /**
     * create an implicit triangulation data from a uniform grid
     *
     * @param dims    dimensions of uniform grid
     * @param origin  origin of the grid
     * @param extent  extent of the entire grid
     * @param dataMapper Maps scalar values
     */
    void set(const size3_t& dims, const vec3& origin, const vec3& extent,
             const DataMapper& dataMapper);
    /**
     * create a triangulation from a list of primitives
     */
    void set(const std::vector<vec3>& points, const std::vector<uint32_t>& indices,
             InputTriangulation type = InputTriangulation::Triangles);
    void set(std::vector<vec3>&& points, const std::vector<uint32_t>& indices,
             InputTriangulation type = InputTriangulation::Triangles);
    /**
     * create a triangulation from a list of primitives with draw type and connectivity provided by
     * the mesh info
     *
     * @throw TTKException in case of unsupported draw type or connectivity
     */
    void set(const std::vector<vec3>& points, const std::vector<uint32_t>& indices,
             Mesh::MeshInfo meshInfo);
    void set(std::vector<vec3>&& points, const std::vector<uint32_t>& indices,
             Mesh::MeshInfo meshInfo);

    /**
     * create triangulation data using a VTK triangle representation, i.e. each cells is given by
     * the number of points and the respective indices of the points
     */
    void set(const std::vector<vec3>& points, const std::vector<long long int>& cells);
    void set(std::vector<vec3>&& points, std::vector<long long int>&& cells);

    void addIndices(const std::vector<uint32_t>& indices, InputTriangulation type);
    void addIndices(const std::vector<uint32_t>& indices, Mesh::MeshInfo meshInfo);

    /**
     * \brief set scalar values associated with vertex positions of the triangulation
     *
     * @param values    vector of scalar values, will be converted into a Buffer
     * @throw TTKException if buffer size is less than number of vertices in triangulation
     */
    template <typename T, typename std::enable_if<util::rank<T>::value == 0>::type>
    void setScalarValues(const std::vector<T>& values);
    template <typename T, typename std::enable_if<util::rank<T>::value == 0>::type>
    void setScalarValues(std::vector<T>&& values);

    /**
     * \brief set scalar values associated with vertex positions of the triangulation
     *
     * @param buffer    buffer of scalar values
     * @throw TTKException if buffer elements are not scalar, i.e. have more than one component
     * @throw TTKException if buffer size is less than number of vertices in triangulation
     */
    void setScalarValues(std::shared_ptr<BufferBase> buffer);
    /**
     * \brief set scalar values associated with vertex positions of the triangulation
     *
     * @param buffer    buffer of scalar values
     * @param component   selects buffer component to be used as scalars
     * @throw TTKException if buffer size is less than number of vertices in triangulation
     */
    void setScalarValues(std::shared_ptr<BufferBase> buffer, size_t component);
    void setScalarValues(const BufferBase& buffer, size_t component);

    std::shared_ptr<BufferBase> getScalarValues() const;

    /**
     * \brief set position/scalar value offsets used in connection with ttk triangulation data
     *
     * @throw TTKException if number of offsets is different from number of vertices
     */
    void setOffsets(const std::vector<int>& offsets);
    void setOffsets(std::vector<int>&& offsets);
    /**
     * \brief return position/scalar value offsets used in connection with ttk triangulation data
     *
     * If unset, a sequence from 0 to n-1 is returned with n = number of vertices
     *
     * @return offsets used by TTK functions
     */
    std::vector<int>& getOffsets();
    const std::vector<int>& getOffsets() const;

    /**
     * returns the cell information as VTK triangle index representation
     */
    const std::vector<long long int>& getCells() const;
    const std::vector<vec3>& getPoints() const;
    vec3 getPoint(const int index) const;
    ttk::Triangulation& getTriangulation();
    const ttk::Triangulation& getTriangulation() const;

    vec3& operator[](size_t i);
    const vec3& operator[](size_t i) const;

    /**
     * return number of cells currently stored in the triangulation
     *
     * @return number of cells
     */
    size_t getCellCount() const;

    virtual const SpatialCameraCoordinateTransformer<3>& getCoordinateTransformer(
        const Camera& camera) const;
    using SpatialEntity<3>::getCoordinateTransformer;

private:
    std::vector<uint32_t> convertToTriangles(const std::vector<uint32_t>& indices,
                                             Mesh::MeshInfo meshInfo);
    std::vector<uint32_t> convertToLines(const std::vector<uint32_t>& indices,
                                         Mesh::MeshInfo meshInfo);

    void unsetGrid();

    /**
     *  input cells of the triangulation, corresponds to VTK triangle representation
     * Format: <#vertices in cell 1>, <v0_1>, <v1_1>, ..., <#vertices in cell 2>, <v0_2>, <v1_2>,
     * ...
     */
    std::vector<long long int> cells_;
    mutable std::vector<vec3>
        points_;  //!< triangle vertices, mutable due to possible change in getPoints
    mutable std::vector<int> offsets_;  //!< matching offsets

    ttk::Triangulation triangulation_;

    std::shared_ptr<BufferBase> scalars_;  //!< scalars associated with vertices of triangulation
    DataMapper volumeDataMapper_;  //!< Data mapper associated with volume scalar values, only used
                                   //!< for implicit grids

    size3_t gridDims_;
    vec3 gridOrigin_;
    vec3 gridExtent_;
};

template <typename T, typename std::enable_if<util::rank<T>::value == 0>::type>
void TriangulationData::setScalarValues(const std::vector<T>& values) {
    if (values.size() < points_.size()) {
        throw TTKException("Too little data (" + std::to_string(values.size()) +
                           " values given, but triangulation holds " +
                           std::to_string(points_.size()) + " positions");
    }
    scalars_ = util::makeBuffer<T>(std::move(std::vector<T>(values)));
}

template <typename T, typename std::enable_if<util::rank<T>::value == 0>::type>
void TriangulationData::setScalarValues(std::vector<T>&& values) {
    if (values.size() < points_.size()) {
        throw TTKException("Too little data (" + std::to_string(values.size()) +
                           " values given, but triangulation holds " +
                           std::to_string(points_.size()) + " positions");
    }
    scalars_ = util::makeBuffer<T>(std::move(values));
}

}  // namespace topology

template <>
struct DataTraits<topology::TriangulationData> {
    static std::string classIdentifier() { return "TriangulationData"; }
    static std::string dataName() { return "TriangulationData"; }
    static uvec3 colorCode() { return uvec3(34, 96, 150); }
    static Document info(const topology::TriangulationData& data) {
        using H = utildoc::TableBuilder::Header;
        using P = Document::PathComponent;
        Document doc;
        doc.append("b", dataName(), {{"style", "color:white;"}});
        utildoc::TableBuilder tb(doc.handle(), P::end());
        auto triangulation = data.getTriangulation();
        tb(H("Implicit Triangulation"), data.isUniformGrid());
        tb(H("Dimensionality"), triangulation.getDimensionality());
        tb(H("Number of Cells"), triangulation.getNumberOfCells());
        tb(H("Number of Edges"), triangulation.getNumberOfEdges());
        tb(H("Number of Triangles"), triangulation.getNumberOfTriangles());
        tb(H("Number of Vertices"), triangulation.getNumberOfVertices());
        if (auto scalars = data.getScalarValues()) {
            tb(H("Type of Scalars"), scalars->getDataFormat()->getString());
        } else {
            tb(H("Type of Scalars"), "<none>");
        }
        return doc;
    }
};

}  // namespace inviwo

#endif  // IVW_TRIANGULATIONDATA_H
