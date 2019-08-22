/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2019 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided rhs the following conditions are met:
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

#include <inviwo/topologytoolkit/datastructures/triangulationdata.h>
#include <inviwo/topologytoolkit/utils/ttkexception.h>

#include <algorithm>
#include <inviwo/core/util/formats.h>

namespace inviwo {

namespace topology {

TriangulationData::TriangulationData(const size3_t& dims, const vec3& origin, const vec3& extent,
                                     const DataMapper& dataMapper) {
    set(dims, origin, extent, dataMapper);
}

TriangulationData::TriangulationData(std::vector<vec3> points, const std::vector<uint32_t>& indices,
                                     InputTriangulation type) {
    set(std::move(points), indices, type);
}

TriangulationData::TriangulationData(std::vector<vec3> points, const std::vector<uint32_t>& indices,
                                     Mesh::MeshInfo meshInfo) {
    set(std::move(points), indices, meshInfo);
}

TriangulationData::TriangulationData(const TriangulationData& rhs)
    : SpatialEntity<3>(rhs)
    , MetaDataOwner(rhs)
    , cells_(rhs.cells_)
    , points_(rhs.points_)
    , offsets_(rhs.offsets_)
    , scalars_(rhs.scalars_->clone())
    , gridDims_(rhs.gridDims_)
    , gridOrigin_(rhs.gridOrigin_)
    , gridExtent_(rhs.gridExtent_) {
    if (rhs.isUniformGrid()) {
        set(gridDims_, gridOrigin_, gridExtent_, rhs.getDataMapper());
    } else {
        triangulation_.setInputPoints(static_cast<int>(points_.size()), points_.data(), false);
        triangulation_.setInputCells(static_cast<int>(getCellCount()), cells_.data());
    }
}

TriangulationData::TriangulationData(TriangulationData&& rhs)
    : SpatialEntity<3>(rhs)
    , MetaDataOwner(rhs)
    , cells_(std::move(rhs.cells_))
    , points_(std::move(rhs.points_))
    , offsets_(std::move(rhs.offsets_))
    , scalars_(std::move(rhs.scalars_))
    , gridDims_(std::move(rhs.gridDims_))
    , gridOrigin_(std::move(rhs.gridOrigin_))
    , gridExtent_(std::move(rhs.gridExtent_)) {
    if (rhs.isUniformGrid()) {
        set(gridDims_, gridOrigin_, gridExtent_, rhs.getDataMapper());
    } else {
        triangulation_.setInputPoints(static_cast<int>(points_.size()), points_.data(), false);
        triangulation_.setInputCells(static_cast<int>(getCellCount()), cells_.data());
    }
}

TriangulationData& TriangulationData::operator=(const TriangulationData& rhs) {
    if (this != &rhs) {
        SpatialEntity<3>::operator=(rhs);
        MetaDataOwner::operator=(rhs);

        cells_ = rhs.cells_;
        points_ = rhs.points_;
        offsets_ = rhs.offsets_;
        scalars_.reset(rhs.scalars_->clone());
        gridDims_ = rhs.gridDims_;
        gridOrigin_ = rhs.gridOrigin_;
        gridExtent_ = rhs.gridExtent_;

        if (rhs.isUniformGrid()) {
            set(gridDims_, gridOrigin_, gridExtent_, rhs.getDataMapper());
        } else {
            triangulation_.setInputPoints(static_cast<int>(points_.size()), points_.data(), false);
            triangulation_.setInputCells(static_cast<int>(getCellCount()), cells_.data());
        }
    }
    return *this;
}

TriangulationData& TriangulationData::operator=(TriangulationData&& rhs) {
    if (this != &rhs) {
        SpatialEntity<3>::operator=(rhs);
        MetaDataOwner::operator=(rhs);

        cells_ = std::move(rhs.cells_);
        points_ = std::move(rhs.points_);
        offsets_ = std::move(rhs.offsets_);
        scalars_ = std::move(rhs.scalars_);
        gridDims_ = std::move(rhs.gridDims_);
        gridOrigin_ = std::move(rhs.gridOrigin_);
        gridExtent_ = std::move(rhs.gridExtent_);

        if (rhs.isUniformGrid()) {
            set(gridDims_, gridOrigin_, gridExtent_, rhs.getDataMapper());
        } else {
            triangulation_.setInputPoints(static_cast<int>(points_.size()), points_.data(), false);
            triangulation_.setInputCells(static_cast<int>(getCellCount()), cells_.data());
        }
    }
    return *this;
}

TriangulationData* TriangulationData::clone() const { return new TriangulationData(*this); }

bool TriangulationData::isUniformGrid() const {
    // cannot use triangulation_.getGridDimensions(std::vector<int>&) here since it is not const
    return glm::compMul(gridDims_) > 0u;
}

DataMapper TriangulationData::getDataMapper() const {
    return isUniformGrid() ? volumeDataMapper_ : DataMapper();
}

size3_t TriangulationData::getGridDimensions() const {
    return isUniformGrid() ? gridDims_ : size3_t(0u);
}

vec3 TriangulationData::getGridOrigin() const { return isUniformGrid() ? gridOrigin_ : vec3(0.0f); }

vec3 TriangulationData::getGridExtent() const { return isUniformGrid() ? gridExtent_ : vec3(0.0f); }

void TriangulationData::set(const size3_t& dims, const vec3& origin, const vec3& extent,
                            const DataMapper& dataMapper) {
    points_.clear();
    cells_.clear();
    const vec3 spacing(extent / vec3(dims));
    triangulation_.setInputGrid(origin.x, origin.y, origin.z, spacing.x, spacing.y, spacing.z,
                                static_cast<int>(dims.x), static_cast<int>(dims.y),
                                static_cast<int>(dims.z));
    gridDims_ = dims;
    gridOrigin_ = origin;
    gridExtent_ = extent;
    volumeDataMapper_ = dataMapper;
}

void TriangulationData::set(const std::vector<vec3>& points, const std::vector<uint32_t>& indices,
                            InputTriangulation type) {
    set(std::vector<vec3>(points), indices, type);
}

void TriangulationData::set(std::vector<vec3>&& points, const std::vector<uint32_t>& indices,
                            InputTriangulation type) {
    points_ = std::move(points);
    cells_.clear();

    // init ttk::Triangulation
    triangulation_.setInputPoints(static_cast<int>(points_.size()), points_.data(), false);
    addIndices(indices, type);

    unsetGrid();
}

void TriangulationData::set(const std::vector<vec3>& points, const std::vector<uint32_t>& indices,
                            Mesh::MeshInfo meshInfo) {
    set(std::vector<vec3>(points), indices, meshInfo);
}

void TriangulationData::set(std::vector<vec3>&& points, const std::vector<uint32_t>& indices,
                            Mesh::MeshInfo meshInfo) {
    switch (meshInfo.dt) {
        case DrawType::Lines: {
            if (meshInfo.ct == ConnectivityType::None) {
                set(points, indices, InputTriangulation::Edges);
            } else {
                set(points, convertToLines(indices, meshInfo), InputTriangulation::Edges);
            }
            break;
        }
        case DrawType::Triangles: {
            if (meshInfo.ct == ConnectivityType::None) {
                set(points, indices, InputTriangulation::Triangles);
            } else {
                set(points, convertToTriangles(indices, meshInfo), InputTriangulation::Triangles);
            }
            break;
        }
        case DrawType::Points:
        case DrawType::NotSpecified:
        case DrawType::NumberOfDrawTypes:
        default:
            std::stringstream ss;
            ss << "Unsupported primitive type " << meshInfo.dt;
            throw TTKConversionException(ss.str());
    }
}

void TriangulationData::set(const std::vector<vec3>& points,
                            const std::vector<long long int>& cells) {
    set(std::vector<vec3>(points), std::vector<long long int>(cells));
}

void TriangulationData::set(std::vector<vec3>&& points, std::vector<long long int>&& cells) {
    points_ = std::move(points);
    cells_ = std::move(cells);

    // determine number of cells
    int numCells = 0;
    for (size_t i = 0; i < cells.size(); i += cells[i] + 1) {
        ++numCells;
    }

    unsetGrid();

    // init ttk::Triangulation
    int retVal =
        triangulation_.setInputPoints(static_cast<int>(points_.size()), points_.data(), false);
    if (retVal < 0) {
        throw TTKException("Error setting input points of ttk::Triangulation");
    }
    retVal = triangulation_.setInputCells(numCells, cells_.data());
    if (retVal < 0) {
        throw TTKException("Error setting input cells of ttk::Triangulation");
    }
}

void TriangulationData::addIndices(const std::vector<uint32_t>& indices, InputTriangulation type) {
    size_t numCells = 0;
    size_t pointsPerCell = 0;
    // transform indices to VTK triangulation
    switch (type) {
        case InputTriangulation::Edges:
            numCells = indices.size() / 2;
            pointsPerCell = 2;
            break;
        case InputTriangulation::Tetrahedra:
            numCells = indices.size() / 4;
            pointsPerCell = 4;
            break;
        case InputTriangulation::Triangles:
        default:
            numCells = indices.size() / 3;
            pointsPerCell = 3;
            break;
    }

    const int newCellCount = numCells + static_cast<int>(getCellCount());
    cells_.reserve(cells_.size() + numCells * (pointsPerCell + 1));
    for (size_t i = 0; i < numCells * pointsPerCell; ++i) {
        if (i % pointsPerCell == 0) {
            cells_.push_back(pointsPerCell);
        }
        cells_.push_back(indices[i]);
    }

    unsetGrid();

    // update TTK triangulation
    int retVal = triangulation_.setInputCells(newCellCount, cells_.data());
    if (retVal < 0) {
        throw TTKException("Error setting input cells of ttk::Triangulation");
    }
}

void TriangulationData::addIndices(const std::vector<uint32_t>& indices, Mesh::MeshInfo meshInfo) {
    switch (meshInfo.dt) {
        case DrawType::Lines: {
            if (meshInfo.ct == ConnectivityType::None) {
                addIndices(indices, InputTriangulation::Edges);
            } else {
                addIndices(convertToLines(indices, meshInfo), InputTriangulation::Edges);
            }
            break;
        }
        case DrawType::Triangles: {
            if (meshInfo.ct == ConnectivityType::None) {
                addIndices(indices, InputTriangulation::Triangles);
            } else {
                addIndices(convertToTriangles(indices, meshInfo), InputTriangulation::Triangles);
            }
            break;
        }
        case DrawType::Points:
        case DrawType::NotSpecified:
        case DrawType::NumberOfDrawTypes:
        default:
            std::stringstream ss;
            ss << "Unsupported primitive type " << meshInfo.dt;
            throw TTKConversionException(ss.str());
    }
}

void TriangulationData::setScalarValues(std::shared_ptr<BufferBase> buffer) {
    if (buffer->getDataFormat()->getComponents() > 1) {
        throw TTKException("TriangulationData supports only scalar data");
    }
    if (isUniformGrid()) {
        if (buffer->getSize() < glm::compMul(gridDims_)) {
            throw TTKException("Too little data (" + std::to_string(buffer->getSize()) +
                               " values given, but implicit triangulation holds " +
                               std::to_string(glm::compMul(gridDims_)) + " positions");
        }
    } else if (buffer->getSize() < points_.size()) {
        throw TTKException("Too little data (" + std::to_string(buffer->getSize()) +
                           " values given, but triangulation holds " +
                           std::to_string(points_.size()) + " positions");
    }
    scalars_ = buffer;
}

void TriangulationData::setScalarValues(std::shared_ptr<BufferBase> buffer, size_t component) {
    if (isUniformGrid()) {
        if (buffer->getSize() < glm::compMul(gridDims_)) {
            throw TTKException("Too little data (" + std::to_string(buffer->getSize()) +
                               " values given, but implicit triangulation holds " +
                               std::to_string(glm::compMul(gridDims_)) + " positions");
        }
    } else if (buffer->getSize() < points_.size()) {
        throw TTKException("Too little data (" + std::to_string(buffer->getSize()) +
                           " values given, but triangulation holds " +
                           std::to_string(points_.size()) + " positions");
    }

    auto convertBuffer = [](auto buffer, size_t component) {
        using ValueType = util::PrecisionValueType<decltype(buffer)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        std::vector<PrimitiveType> scalarData(buffer->getSize());
        std::transform(buffer->getDataContainer().begin(), buffer->getDataContainer().end(),
                       scalarData.begin(),
                       [component](auto elem) { return util::glmcomp(elem, component); });
        return util::makeBuffer<PrimitiveType>(std::move(scalarData));
    };

    scalars_ = buffer->getRepresentation<BufferRAM>()->dispatch<std::shared_ptr<BufferBase>>(
        convertBuffer, component);
}

void TriangulationData::setScalarValues(const BufferBase& buffer, size_t component) {
    if (isUniformGrid()) {
        if (buffer.getSize() < glm::compMul(gridDims_)) {
            throw TTKException("Too little data (" + std::to_string(buffer.getSize()) +
                               " values given, but implicit triangulation holds " +
                               std::to_string(glm::compMul(gridDims_)) + " positions");
        }
    } else if (buffer.getSize() < points_.size()) {
        throw TTKException("Too little data (" + std::to_string(buffer.getSize()) +
                           " values given, but triangulation holds " +
                           std::to_string(points_.size()) + " positions");
    }

    auto convertBuffer = [](auto bufferpr, size_t component) {
        using ValueType = util::PrecisionValueType<decltype(bufferpr)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        std::vector<PrimitiveType> scalarData(bufferpr->getSize());
        std::transform(bufferpr->getDataContainer().begin(), bufferpr->getDataContainer().end(),
                       scalarData.begin(),
                       [component](auto elem) { return util::glmcomp(elem, component); });
        return util::makeBuffer<PrimitiveType>(std::move(scalarData));
    };

    scalars_ = buffer.getRepresentation<BufferRAM>()->dispatch<std::shared_ptr<BufferBase>>(
        convertBuffer, component);
}

std::shared_ptr<BufferBase> TriangulationData::getScalarValues() const { return scalars_; }

void TriangulationData::setOffsets(const std::vector<int>& offsets) {
    setOffsets(std::vector<int>(offsets));
}

void TriangulationData::setOffsets(std::vector<int>&& offsets) {
    const auto numelems = isUniformGrid() ? glm::compMul(gridDims_) : points_.size();
    if (offsets.size() != numelems) {
        throw TTKException("Mismatch in range (" + std::to_string(offsets.size()) + " offsets, " +
                           std::to_string(numelems) + " vertices)");
    }
    offsets_ = std::move(offsets);
}

std::vector<int>& TriangulationData::getOffsets() {
    const auto numelems = isUniformGrid() ? glm::compMul(gridDims_) : points_.size();
    if (offsets_.size() != numelems) {
        offsets_.resize(numelems);
        std::iota(offsets_.begin(), offsets_.end(), 0);
    }
    return offsets_;
}

const std::vector<int>& TriangulationData::getOffsets() const {
    const auto numelems = isUniformGrid() ? glm::compMul(gridDims_) : points_.size();
    if (offsets_.size() != numelems) {
        offsets_.resize(numelems);
        std::iota(offsets_.begin(), offsets_.end(), 0);
    }
    return offsets_;
}

const std::vector<long long int>& TriangulationData::getCells() const { return cells_; }

const std::vector<vec3>& TriangulationData::getPoints() const {
    if (isUniformGrid() && points_.empty()) {
        for (int index = 0; index < triangulation_.getNumberOfVertices(); index++) {
            points_.push_back(getPoint(index));
        }
    }
    return points_;
}

vec3 TriangulationData::getPoint(const int index) const {
    vec3 point;
    triangulation_.getVertexPoint(index, point.x, point.y, point.z);
    return point;
}

ttk::Triangulation& TriangulationData::getTriangulation() { return triangulation_; }

const ttk::Triangulation& TriangulationData::getTriangulation() const { return triangulation_; }

vec3& TriangulationData::operator[](size_t i) { return points_[i]; }

const vec3& TriangulationData::operator[](size_t i) const { return points_[i]; }

const SpatialCameraCoordinateTransformer<3>& TriangulationData::getCoordinateTransformer(
    const Camera& camera) const {
    return SpatialEntity<3>::getCoordinateTransformer(camera);
}

std::vector<uint32_t> TriangulationData::convertToTriangles(const std::vector<uint32_t>& indices,
                                                            Mesh::MeshInfo meshInfo) {
    std::vector<uint32_t> ind;
    auto triangle = [&ind](size_t i1, size_t i2, size_t i3) {
        ind.push_back(static_cast<uint32_t>(i1));
        ind.push_back(static_cast<uint32_t>(i2));
        ind.push_back(static_cast<uint32_t>(i3));
    };
    switch (meshInfo.ct) {
        case ConnectivityType::None:
            return indices;
            break;
        case ConnectivityType::Strip: {
            const auto numTris = (indices.size() > 2 ? indices.size() - 2 : 0);
            ind.reserve(3 * numTris);
            for (size_t i = 0; i < indices.size() - 3u; i += 2) {
                triangle(indices[i + 0], indices[i + 1], indices[i + 2]);
                triangle(indices[i + 2], indices[i + 1], indices[i + 3]);
            }
            break;
        }
        case ConnectivityType::Fan: {
            const auto numTris = (indices.size() > 2 ? indices.size() - 2 : 0);
            ind.reserve(3 * numTris);
            for (size_t i = 1; i < indices.size() - 1u; i += 1) {
                triangle(indices[0], indices[i], indices[i + 1]);
            }
            break;
        }
        case ConnectivityType::Adjacency: {
            const auto numTris = indices.size() / 6;
            ind.reserve(3 * numTris);
            for (size_t i = 0; i < numTris * 6; i += 6) {
                triangle(indices[i], indices[i + 2], indices[i + 4]);
            }
            break;
        }
        case ConnectivityType::StripAdjacency: {
            const auto numTris = (indices.size() > 4 ? (indices.size() - 4) / 2 : 0);
            ind.reserve(3 * numTris);
            for (size_t i = 0; i < numTris * 2 + 4; i += 4) {
                triangle(indices[i], indices[i + 2], indices[i + 4]);
                triangle(indices[i + 4], indices[i + 2], indices[i + 6]);
            }
            break;
        }
        case ConnectivityType::Loop:
        case ConnectivityType::NumberOfConnectivityTypes:
        default:
            std::stringstream ss;
            ss << "Unsupported connectivity type " << meshInfo.ct;
            throw TTKConversionException(ss.str());
    }
    return ind;
}

std::vector<uint32_t> TriangulationData::convertToLines(const std::vector<uint32_t>& indices,
                                                        Mesh::MeshInfo meshInfo) {
    std::vector<uint32_t> ind;
    auto line = [&ind](size_t i1, size_t i2) {
        ind.push_back(static_cast<uint32_t>(i1));
        ind.push_back(static_cast<uint32_t>(i2));
    };

    switch (meshInfo.ct) {
        case ConnectivityType::None:
            return indices;
        case ConnectivityType::Strip: {
            const auto numLines = (indices.size() > 1 ? indices.size() - 1 : 0);
            ind.reserve(2 * numLines);
            for (size_t i = 0; i < numLines; ++i) {
                line(indices[i], indices[i + 1]);
            }
            break;
        }
        case ConnectivityType::Loop: {
            const auto numLines = indices.size();
            ind.reserve(2 * numLines);
            for (size_t i = 0; i < numLines - 1; ++i) {
                line(indices[i], indices[i + 1]);
            }
            line(indices.back(), indices.front());
            break;
        }
        case ConnectivityType::Adjacency: {
            const auto numLines = indices.size() / 4;
            ind.reserve(2 * numLines);
            for (size_t i = 0; i < indices.size(); i += 4) {
                line(indices[i + 1], indices[i + 2]);
            }
            break;
        }
        case ConnectivityType::StripAdjacency: {
            const auto numLines = (indices.size() > 3 ? indices.size() - 3 : 0);
            ind.reserve(numLines * 2);
            for (size_t i = 1; i <= numLines; ++i) {
                line(indices[i], indices[i + 1]);
            }
            break;
        }
        case ConnectivityType::NumberOfConnectivityTypes:
        default:
            std::stringstream ss;
            ss << "unsupported connectivity type " << meshInfo.ct;
            throw TTKConversionException(ss.str());
    }
    return ind;
}

size_t TriangulationData::getCellCount() const {
    // determine number of cells based on VTK index list
    size_t numCells = 0;
    for (size_t i = 0; i < cells_.size(); i += cells_[i] + 1) {
        ++numCells;
    }
    return numCells;
}

void TriangulationData::unsetGrid() {
    // unset grid information
    gridDims_ = size3_t(0u);
    gridOrigin_ = vec3(0.0f);
    gridExtent_ = vec3(0.0f);
}

}  // namespace topology

}  // namespace inviwo
