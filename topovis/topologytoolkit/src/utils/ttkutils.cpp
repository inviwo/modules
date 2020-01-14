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

#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/util/interpolation.h>

#include <algorithm>
#include <inviwo/core/util/formats.h>


#pragma optimize("", off)


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
        std::transform(data.begin(), data.end(), result.begin(),
                       [modelMatrix](auto& elem) { 
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


std::shared_ptr<Mesh> applyColorMapToMesh(const TransferFunction& tf, const Mesh& mesh,
                                           const std::vector<int>& segments,
                                           const TriangulationData& triangulationData,
                                           MeshColorOption colorOption) {

    auto newMesh = std::make_shared<Mesh>(mesh);

    auto buffers = newMesh->getBuffers();
    auto isColorBuffer =
        [](const std::pair<Mesh::BufferInfo, std::shared_ptr<BufferBase>>& buffer) -> bool {
        return buffer.first.type == BufferType::ColorAttrib;
    };

    auto bufferIt = std::find_if(buffers.begin(), buffers.end(), isColorBuffer);
    if (bufferIt == buffers.end()) {
        throw TTKConversionException("Mesh does not contain a color buffer");
    } else if (std::find_if(bufferIt + 1, buffers.end(), isColorBuffer) != buffers.end()) {
        LogWarnCustom("topology::mapMeshToContourTree",
                      "Mesh features multiple color buffer, using only first one.");
    }

    auto colorBuffer = std::dynamic_pointer_cast<Buffer<vec4>>(bufferIt->second);
    std::vector<vec4>& colors = colorBuffer->getEditableRAMRepresentation()->getDataContainer();
    // auto minmax = std::minmax_element(colors.begin(), colors.end());

    if (colorOption == MeshColorOption::SEGMENT_COLORMAP) {
        // auto& segments = triangulationData.getSegments();

        auto minmax = std::minmax_element(segments.begin(), segments.end());
        auto div = (float)(*minmax.second - *minmax.first);

        for (size_t i = 0; i < std::min(segments.size(), colors.size()); ++i) {
            colors[i] = tf.sample(static_cast<float>((segments[i] - *minmax.first) / div));
        }
    } else {

        triangulationData.getScalarValues()
            ->getRepresentation<BufferRAM>()
            ->dispatch<void, dispatching::filter::Scalars>([&](auto bufferpr) {
                auto& scalars = bufferpr->getDataContainer();

                auto minmax = std::minmax_element(scalars.begin(), scalars.end());
                auto div = (float)(*minmax.second - *minmax.first);

                for (size_t i = 0; i < std::min(bufferpr->getSize(), colors.size()); ++i) {
                    colors[i] = tf.sample(static_cast<float>((scalars[i] - *minmax.first) / div));
                }
            });
    }

    return newMesh;
}

std::vector<float> ttkExtractScalarValuesFromTriangulation(const TriangulationData& tdata) {

    if (tdata.getScalarValues()) {

		std::vector<float> vertices(tdata.getPoints().size());  // make a copy

        // overwrite vertex[component] with matching scalar value
        tdata.getScalarValues()
            ->getRepresentation<BufferRAM>()
            ->dispatch<void, dispatching::filter::Scalars>([&vertices](auto bufferpr) { 
				auto& scalars = bufferpr->getDataContainer();
                for (size_t i = 0; i < std::min(bufferpr->getSize(), vertices.size()); ++i) {
                    vertices[i] = scalars[i];
                }
            });

		return vertices;
    }

	return std::vector<float>();
}

//Each segment as triangulation data
std::vector<std::shared_ptr<TriangulationData>> ttkSegmentExtraction(std::shared_ptr<const ContourTreeData> contourTreeData) {

	const TriangulationData& tdata = *contourTreeData->triangulation.get();

	std::vector<std::shared_ptr<TriangulationData>> segTriangulations;

	 const std::vector<int>& segments = contourTreeData->getSegments();

	std::set<int> s(segments.begin(), segments.end());
	std::vector<int> usegments(s.begin(), s.end());

	std::map<int, std::vector<uint32_t>> segmentedTriangleIndices;

    const auto& cells = tdata.getCells();
    const auto& positions = tdata.getPoints();


	for (auto& us : usegments) {

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
		for (size_t i = 0; i < cells.size(); i += cells[i] + 1) {

			if (segments[cells[i + 1]] != us && segments[cells[i + 2]] != us && segments[cells[i + 3]] != us)
				continue;

			//if (!(segments[cells[i + 1]] == us || segments[cells[i + 2]] == us || segments[cells[i + 3]] == us))
			//	continue;

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
			continue;
		}

		std::shared_ptr<TriangulationData> segTri = std::make_shared<TriangulationData>();
		if (!indicesLines.empty()) {
			segTri->set(std::move(positions), indicesLines,
                 Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None));

			auto scalars = ttkExtractScalarValuesFromTriangulation(tdata);
			segTri->setScalarValues(util::makeBuffer(std::move(scalars)));
		}

		
		if (!indicesTriangles.empty()) {
			segmentedTriangleIndices[us] = indicesTriangles;
			segTri->set(std::move(positions), indicesTriangles,
                 Mesh::MeshInfo(DrawType::Triangles, ConnectivityType::None));

			auto scalars = ttkExtractScalarValuesFromTriangulation(tdata);
			segTri->setScalarValues(util::makeBuffer(std::move(scalars)));
		}

		segTriangulations.push_back(segTri);
	}

	return segTriangulations;
}

std::shared_ptr<Mesh> ttkIsoContour(std::shared_ptr<const ContourTreeData> treeData, 
									float isoF, vec4 col,
									std::vector<std::shared_ptr<TriangulationData>> segmentTriangulations)
{
		std::set<int> s(treeData->getSegments().begin(),treeData->getSegments().end());
		std::vector<int> usegments(s.begin(), s.end());

		auto triangulation = treeData->triangulation.get();
		std::vector<float> scalars = topology::ttkExtractScalarValuesFromTriangulation(*triangulation);

		//This is expensive and memory consuming
		if (!segmentTriangulations.size())
			segmentTriangulations = topology::ttkSegmentExtraction(treeData); 

		/*auto minmax = std::minmax_element(scalars.begin(), scalars.end());
        auto div = (*minmax.second - *minmax.first);
		auto avg = (*minmax.first + *minmax.second) / 2.0f;*/

		std::shared_ptr<inviwo::Mesh> contour_mesh = nullptr;

		auto tree = treeData->getTree();
		auto numArcs = tree->getNumberOfSuperArcs();

		std::vector<ttk::ftm::idNode> leafNodes;
		for (size_t i = 0; i < tree->getLeaves().size(); i++)
			leafNodes.push_back( tree->getLeave(i));
			

		for (ttk::ftm::idSuperArc i = 0; i <usegments.size(); ++i) {
				
			//if (usegments[i] >= numArcs) continue;

			auto arc = tree->getSuperArc(usegments[i]);

			if (!arc) {
				LogWarnCustom("topology::ttkIsoContour",
				"Segment Id does not have corresponding Super Arc. SegId = " + std::to_string(usegments[i]));
				continue;
			}

			if (std::find(leafNodes.begin(), leafNodes.end(), arc->getDownNodeId()) == leafNodes.end() &&
				std::find(leafNodes.begin(), leafNodes.end(), arc->getUpNodeId()) == leafNodes.end())
					continue;

			auto upscalar = scalars[tree->getNode(arc->getUpNodeId())->getVertexId()];
			auto downscalar =  scalars[tree->getNode(arc->getDownNodeId())->getVertexId()];

			auto iso = (upscalar * isoF) + downscalar *(1.0f - isoF);
			auto avg = (upscalar + downscalar) * 0.5f;

			//auto col = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			//col = glm::clamp(col, vec4(0.0f), vec4(1.0f));

			auto iso_contours = marchingTriangles_from_Triangulation(*segmentTriangulations[i], iso, col);

			if (!contour_mesh) {
				contour_mesh = iso_contours;
				continue;
			}

			contour_mesh->append(*iso_contours);
		}

		return contour_mesh;
}

std::shared_ptr<Mesh> marchingTriangles_from_Triangulation( 
							    const TriangulationData& tdata,
								double isoValue,
								vec4 color) 
{
    // loop over VTK index list
    // <#indices in cell 1>, <v1_1>, ..., v1_n>, <#indices in cell 2>, <v2_1> ...
    size_t invalid = 0;
    const auto& cells = tdata.getCells();
    const auto& positions = tdata.getPoints();
	
	static const std::vector<std::vector<int>> caseTable = {
        std::vector<int>(),            // case 0
        std::vector<int>({0, 1, 0, 2}),// case 1 (6)
		std::vector<int>({0, 1, 2, 1}),// case 2 (5)
        std::vector<int>({2, 0, 2, 1}),// case 3 (4)
        std::vector<int>({0, 2, 1, 2}),// case 4 (3)
        std::vector<int>({1 ,0, 1, 2}),// case 5 (2)
        std::vector<int>({1, 0, 2, 0}),// case 6 (1)
        std::vector<int>()             // case 7
	};
   
	auto mesh = std::make_shared<BasicMesh>();	
    auto indices = mesh->addIndexBuffer(DrawType::Lines, ConnectivityType::None);

	auto debug_mesh = std::make_shared<Mesh>();	

    double vals[4];
    vec3 outPos[4];
	vec3 outPosScale = vec3(1.0f);

	/*std::vector<vec3> vpositions;
	std::vector<vec4> vcolors;
	std::vector<float> vradius;*/

	std::vector<uint32_t> indicesTriangles;
	auto triangle = [&indicesTriangles](auto i1, auto i2, auto i3) {
		indicesTriangles.push_back(static_cast<uint32_t>(i1));
		indicesTriangles.push_back(static_cast<uint32_t>(i2));
		indicesTriangles.push_back(static_cast<uint32_t>(i3));
	};

	for (size_t i = 0; i < cells.size(); i += cells[i] + 1) {

		//if (segments[cells[i + 1]] != us && segments[cells[i + 2]] != us && segments[cells[i + 3]] != us)
		//	continue;

		const auto numIndices = cells[i];
		switch (numIndices) {
		case 2:  // edge
			break;
		case 3:  // triangle
			triangle(cells[i + 1], cells[i + 2], cells[i + 3]);
			break;
		case 4:  // tetrahedron
			break;
		default:
			++invalid;
		}
	}

	if (invalid > 0) {
		LogWarnCustom("topology::marchingTriangles_from_Triangulation",
			"Triangulation contains " + std::to_string(invalid) + " invalid cells.");
		//continue;
	}

	std::vector<float> vertices = ttkExtractScalarValuesFromTriangulation(tdata);
		
	for (size_t i = 0; i < indicesTriangles.size(); i += 3) {
		size_t idx_1 = indicesTriangles[i + 0];
		size_t idx_2 = indicesTriangles[i + 1];
		size_t idx_3 = indicesTriangles[i + 2];

		vals[0] = util::glm_convert<double>(vertices[idx_1]);
		vals[1] = util::glm_convert<double>(vertices[idx_2]);
		vals[2] = util::glm_convert<double>(vertices[idx_3]);

		int theCase = 0;
		theCase += vals[0] < isoValue ? 0 : 1;
		theCase += vals[1] < isoValue ? 0 : 2;
		theCase += vals[2] < isoValue ? 0 : 4;

		if (theCase == 0 || theCase == 7) {
			continue;
		}

		/*auto p = (positions[idx_1] + positions[idx_2] + positions[idx_3]) / 3.0f;
		vpositions.push_back(p);
		vcolors.push_back(color);
		vradius.push_back(1.0f);*/

		outPos[0] = positions[idx_1] * outPosScale;
		outPos[1] = positions[idx_2] * outPosScale;
		outPos[2] = positions[idx_3] * outPosScale;

		auto& edges = caseTable[theCase];
		for (size_t i = 0; i < edges.size(); i += 2) {
			auto t = (isoValue - vals[edges[i]]) / (vals[edges[i + 1]] - vals[edges[i]]);
			auto p = Interpolation<vec3, float>::linear(outPos[edges[i]], outPos[edges[i + 1]],
				static_cast<float>(t));
			indices->add(mesh->addVertex(p, p, p, color));
		}
	}

	// critical points
    /*std::vector<uint32_t> vindices(vpositions.size());
    std::iota(vindices.begin(), vindices.end(), 0);
    debug_mesh->addIndicies(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                      util::makeIndexBuffer(std::move(vindices)));

	debug_mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(vpositions)));
	debug_mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(vcolors)));
	debug_mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(vradius)));  */

    return mesh;
}

}  // namespace topology

}  // namespace inviwo
