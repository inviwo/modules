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

#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/topologytoolkit/utils/ttkexception.h>

#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/util/glm.h>

#include <algorithm>
#include <inviwo/core/util/formats.h>

namespace inviwo {

namespace topology {

TriangulationData meshToTTKTriangulation(const Mesh& mesh) {
    auto buffers = mesh.getBuffers();
    auto isPositionBuffer =
        [](const std::pair<Mesh::BufferInfo, std::shared_ptr<BufferBase>>& buffer) -> bool {
        return buffer.first.type == BufferType::PositionAttrib;
    };

    auto bufferIt = std::find_if(buffers.begin(), buffers.end(), isPositionBuffer);
    if (bufferIt == buffers.end()) {
        throw TTKConversionException("Mesh does not contain a position buffer");
    } else if (std::find_if(bufferIt + 1, buffers.end(), isPositionBuffer) != buffers.end()) {
        LogWarnCustom("topology::meshToTTKTriangulation",
                      "Mesh features multiple position buffer, using only first one.");
    }

    const auto modelMatrix = mesh.getModelMatrix();

    // convert position buffer of input mesh to vec3
    auto bufferRAM = bufferIt->second->getRepresentation<BufferRAM>();
    auto positions = bufferRAM->dispatch<std::vector<vec3>>([modelMatrix](auto posBuffer) {
        auto data = posBuffer->getDataContainer();
        std::vector<vec3> result(data.size());
        std::transform(data.begin(), data.end(), result.begin(), [modelMatrix](auto& elem) {
            auto v = util::glm_convert<vec3>(elem);
            // apply model transform
            return vec3(modelMatrix * vec4(v, 1.0f));
        });
        return result;
    });

    TriangulationData data;

    const size_t numIndexBuffers = mesh.getNumberOfIndicies();
    if (numIndexBuffers == 0) {
        LogWarnCustom("topology::meshToTTKTriangulation",
                      "Mesh has no index buffers. Assuming triangles.");

        std::vector<uint32_t> indices(positions.size());
        std::iota(indices.begin(), indices.end(), 0);

        data.set(std::move(positions), indices,
                 Mesh::MeshInfo(DrawType::Triangles, ConnectivityType::None));
    } else {
        data.set(std::move(positions), {},
                 Mesh::MeshInfo(DrawType::Triangles, ConnectivityType::None));
        // add each index buffer
        for (auto& indexElem : mesh.getIndexBuffers()) {
            data.addIndices(indexElem.second->getRAMRepresentation()->getDataContainer(),
                            indexElem.first);
        }
    }

    data.copyMetaDataFrom(mesh);
    data.setModelMatrix(mat4(1.0f));
    data.setWorldMatrix(mesh.getWorldMatrix());

    return data;
}

std::shared_ptr<Mesh> ttkTriangulationToMesh(const TriangulationData& data, const vec4& color,
                                             bool applyScalars, size_t component) {
    auto mesh = std::make_shared<Mesh>();

    if (data.getPoints().empty()) {
        return mesh;
    }

    std::vector<vec3> vertices(data.getPoints());  // make a copy
    if (applyScalars && component < 3 && data.getScalarValues()) {
        // overwrite vertex[component] with matching scalar value
        data.getScalarValues()
            ->getRepresentation<BufferRAM>()
            ->dispatch<void, dispatching::filter::Scalars>([&vertices, component](auto bufferpr) {
                auto& scalars = bufferpr->getDataContainer();
                for (size_t i = 0; i < std::min(bufferpr->getSize(), vertices.size()); ++i) {
                    vertices[i][component] = static_cast<float>(scalars[i]);
                }
            });
    }

    auto vertexRAM = std::make_shared<BufferRAMPrecision<vec3>>(std::move(vertices));
    auto colorRAM = std::make_shared<BufferRAMPrecision<vec4>>();
    colorRAM->getDataContainer().resize(data.getPoints().size(), color);

    mesh->addBuffer(Mesh::BufferInfo(BufferType::PositionAttrib),
                    std::make_shared<Buffer<vec3>>(vertexRAM));
    mesh->addBuffer(Mesh::BufferInfo(BufferType::ColorAttrib),
                    std::make_shared<Buffer<vec4>>(colorRAM));

    // create index buffer(s)
    std::vector<uint32_t> indicesLines;
    std::vector<uint32_t> indicesTriangles;

    auto line = [&indicesLines](auto i1, auto i2) {
        indicesLines.push_back(static_cast<uint32_t>(i1));
        indicesLines.push_back(static_cast<uint32_t>(i2));
    };
    auto triangle = [&indicesTriangles](auto i1, auto i2, auto i3) {
        indicesTriangles.push_back(static_cast<uint32_t>(i1));
        indicesTriangles.push_back(static_cast<uint32_t>(i2));
        indicesTriangles.push_back(static_cast<uint32_t>(i3));
    };

    // loop over VTK index list
    // <#indices in cell 1>, <v1_1>, ..., v1_n>, <#indices in cell 2>, <v2_1> ...
    size_t invalid = 0;
    const auto& cells = data.getCells();
    for (size_t i = 0; i < cells.size(); i += cells[i] + 1) {
        const auto numIndices = cells[i];

        switch (numIndices) {
            case 2:  // edge
                line(cells[i + 1], cells[i + 2]);
                break;
            case 3:  // triangle
                triangle(cells[i + 1], cells[i + 2], cells[i + 3]);
                break;
            case 4:  // tetrahedron
                triangle(cells[i + 1], cells[i + 2], cells[i + 3]);
                triangle(cells[i + 1], cells[i + 3], cells[i + 4]);
                triangle(cells[i + 2], cells[i + 1], cells[i + 4]);
                triangle(cells[i + 2], cells[i + 4], cells[i + 3]);
                break;
            default:
                ++invalid;
        }
    }
    if (invalid > 0) {
        LogWarnCustom("topology::ttkTriangulationToMesh",
                      "Triangulation contains " + std::to_string(invalid) + " invalid cells.");
    }

    if (!indicesLines.empty()) {
        mesh->addIndicies(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                          util::makeIndexBuffer(std::move(indicesLines)));
    }
    if (!indicesTriangles.empty()) {
        mesh->addIndicies(Mesh::MeshInfo(DrawType::Triangles, ConnectivityType::None),
                          util::makeIndexBuffer(std::move(indicesTriangles)));
    }

    mesh->setModelMatrix(data.getModelMatrix());
    mesh->setWorldMatrix(data.getWorldMatrix());
    mesh->copyMetaDataFrom(data);

    return mesh;
}

TriangulationData volumeToTTKTriangulation(const Volume& volume, size_t channel) {
    auto dataToWorld = volume.getCoordinateTransformer().getDataToWorldMatrix();
    auto offset = vec3(dataToWorld[3]);
    const vec3 volExtent(glm::length(dataToWorld[0]), glm::length(dataToWorld[1]),
                         glm::length(dataToWorld[2]));

    TriangulationData data(volume.getDimensions(), offset, volExtent, volume.dataMap_);

    auto convertVolumeToBuffer = [&data](auto vrprecision, size_t channel) {
        using ValueType = util::PrecisionValueType<decltype(vrprecision)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        auto volData = vrprecision->getDataTyped();
        const size_t volSize = glm::compMul(vrprecision->getDimensions());

        std::vector<PrimitiveType> scalarData(volSize);
        std::transform(volData, volData + volSize, scalarData.begin(),
                       [channel](auto elem) { return util::glmcomp(elem, channel); });
        data.setScalarValues(util::makeBuffer<PrimitiveType>(std::move(scalarData)));
    };

    volume.getRepresentation<VolumeRAM>()->dispatch<void>(convertVolumeToBuffer, channel);

    data.copyMetaDataFrom(volume);
    data.setModelMatrix(volume.getModelMatrix());
    data.setWorldMatrix(volume.getWorldMatrix());

    return data;
}

std::shared_ptr<Volume> ttkTriangulationToVolume(const TriangulationData& data) {
    if (!data.isUniformGrid()) {
        throw TTKConversionException(
            "Triangulation is not implicit, i.e. does not represent a uniform grid.");
    }
    if (!data.getScalarValues()) {
        LogWarnCustom("topology::ttkTriangulationToVolume",
                      "Triangulation contains no scalar values. Creating empty volume.");

        // create volume and set basis and offset
        auto volumeRAM = createVolumeRAM(data.getGridDimensions(), DataFloat32::get());
        auto volume = std::make_shared<Volume>(volumeRAM);
        volume->setModelMatrix(data.getModelMatrix());
        volume->setWorldMatrix(data.getWorldMatrix());
        volume->copyMetaDataFrom(data);
        return volume;
    }

    auto createVolume = [&data](auto bufferpr) {
        using ValueType = util::PrecisionValueType<decltype(bufferpr)>;
        using PrimitiveType = typename DataFormat<ValueType>::primitive;

        // create matching volume representation
        auto volumeRep =
            std::make_shared<VolumeRAMPrecision<PrimitiveType>>(data.getGridDimensions());
        // fill volume with the scalar data of the triangulation
        std::copy(bufferpr->getDataContainer().begin(), bufferpr->getDataContainer().end(),
                  volumeRep->getDataTyped());

        // create volume and set basis and offset
        auto volume = std::make_shared<Volume>(volumeRep);
        volume->setModelMatrix(data.getModelMatrix());
        volume->setWorldMatrix(data.getWorldMatrix());
        volume->copyMetaDataFrom(data);
        volume->dataMap_ = data.getDataMapper();
        return volume;
    };

    return data.getScalarValues()
        ->getRepresentation<BufferRAM>()
        ->dispatch<std::shared_ptr<Volume>, dispatching::filter::Scalars>(createVolume);
}

}  // namespace topology

}  // namespace inviwo
