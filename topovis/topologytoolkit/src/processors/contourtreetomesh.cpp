/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/topologytoolkit/processors/contourtreetomesh.h>

//#include <inviwo/topologytoolkit/utils/ttkutils.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/meshram.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo ContourTreeToMesh::processorInfo_{
    "org.inviwo.ContourTreeToMesh",      // Class identifier
    "Contour Tree To Mesh",              // Display name
    "Topology",                          // Category
    CodeState::Experimental,             // Code state
    "CPU, Topology, TTK, Contour Tree",  // Tags
};
const ProcessorInfo ContourTreeToMesh::getProcessorInfo() const { return processorInfo_; }

ContourTreeToMesh::ContourTreeToMesh()
    : treeType_("treeType", "Tree Type", "", InvalidationLevel::Valid)
    , showSaddlepoints_("showSaddlepoints", "Show Saddle Points", true)
    , sphereRadius_("sphereRadius", "Sphere Radius", 0.01f, 0.0f, 10.0f, 0.001f)
    , localMaximaColor_("localMaximaColor", "Local Maxima", vec4(1.0f, 0.2f, 0.15f, 1.0f),
                        vec4(0.0f), vec4(1.0f))
    , localMinimaColor_("localMinimaColor", "Local Minima", vec4(0.3f, 1.0f, 0.0f, 1.0f),
                        vec4(0.0f), vec4(1.0f))
    , saddleColor_("saddleColor", "Saddle Point", vec4(1.0f, 1.0f, 0.5f, 1.0f), vec4(0.0f),
                   vec4(1.0f))
    , arcColor_("arcColor", "Arc Color", vec4(0.2f, 0.2f, 0.2f, 1.0f), vec4(0.0f), vec4(1.0f)) {

    addPort(inport_);
    addPort(outport_);

    addProperties(treeType_, showSaddlepoints_, sphereRadius_, localMaximaColor_, localMinimaColor_,
                  saddleColor_, arcColor_);

    treeType_.setReadOnly(true);

    inport_.onChange([this]() {
        treeType_.set([this]() -> std::string {
            if (inport_.hasData()) {
                /*
                // FIXME: does not work right now due to bug in TTK (see topology::ContourTreeData)
                const bool jointree = inport_.getData()->tree->isJT();
                const bool splittree = inport_.getData()->tree->isST();
                if (jointree && splittree) {
                    return "Join-Split Tree";
                } else if (jointree) {
                    return "Join Tree";
                } else if (splittree) {
                    return "Split Tree";
                } else {
                    return "Contour Tree";
                }
                */
                return toString(inport_.getData()->type);
            } else {
                return "";
            }
        }());
    });

    localMaximaColor_.setSemantics(PropertySemantics::Color);
    localMinimaColor_.setSemantics(PropertySemantics::Color);
    saddleColor_.setSemantics(PropertySemantics::Color);
    arcColor_.setSemantics(PropertySemantics::Color);
}

void ContourTreeToMesh::process() {
    auto tree = inport_.getData()->getTree();

    const size_t numPoints = tree->getNumberOfNodes() + tree->getNumberOfSuperArcs() * 2;

    std::vector<vec3> positions;
    std::vector<vec4> colors;
    std::vector<float> radius(numPoints, 0.0f);
    std::fill(radius.begin(), radius.begin() + tree->getNumberOfNodes(), sphereRadius_.get());
    positions.reserve(numPoints);
    colors.reserve(numPoints);

    // create a mesh with critical points and arcs
    auto triangulation = inport_.getData()->triangulation;
    for (ttk::ftm::idNode i = 0; i < tree->getNumberOfNodes(); ++i) {
        auto node = tree->getNode(i);

        const bool up = node->getNumberOfUpSuperArcs() > 0;
        const bool down = node->getNumberOfDownSuperArcs() > 0;

        positions.push_back(triangulation->getPoint(node->getVertexId()));
        colors.push_back(up && down ? saddleColor_ : (up ? localMaximaColor_ : localMinimaColor_));
    }

    // arcs
    for (ttk::ftm::idSuperArc i = 0; i < tree->getNumberOfSuperArcs(); ++i) {
        auto arc = tree->getSuperArc(i);

        positions.push_back(
            triangulation->getPoint(tree->getNode(arc->getUpNodeId())->getVertexId()));
        positions.push_back(
            triangulation->getPoint(tree->getNode(arc->getDownNodeId())->getVertexId()));

        colors.push_back(arcColor_);
        colors.push_back(arcColor_);
    }

    auto mesh = std::make_shared<Mesh>(DrawType::Points, ConnectivityType::None);
    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));
    mesh->addBuffer(BufferType::RadiiAttrib, util::makeBuffer(std::move(radius)));

    // critical points
    std::vector<uint32_t> indices(tree->getNumberOfNodes());
    std::iota(indices.begin(), indices.end(), 0);
    mesh->addIndicies(Mesh::MeshInfo(DrawType::Points, ConnectivityType::None),
                      util::makeIndexBuffer(std::move(indices)));
    // arcs
    std::vector<uint32_t> arcIndices(tree->getNumberOfSuperArcs() * 2);
    std::iota(arcIndices.begin(), arcIndices.end(), tree->getNumberOfNodes());
    mesh->addIndicies(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::None),
                      util::makeIndexBuffer(std::move(arcIndices)));

    outport_.setData(mesh);
}

}  // namespace inviwo
