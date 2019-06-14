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

#pragma once

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/ports/meshport.h>

#include <inviwo/topologytoolkit/ports/contourtreeport.h>

namespace inviwo {

/** \docpage{org.inviwo.ContourTreeToMesh, Contour Tree To Mesh}
 * ![](org.inviwo.ContourTreeToMesh.png?classIdentifier=org.inviwo.ContourTreeToMesh)
 * Converts a TTK contour tree to a Mesh
 *
 * ### Inports
 *   * __contourTree__  Contour tree input
 *
 * ### Outports
 *   * __outport__      Mesh of the contour tree containing the critical points and associated
 *                      scalar values as spheres and the connecting arcs as lines
 */

/**
 * \brief converts a TTK contour tree to a Mesh
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API ContourTreeToMesh : public Processor {
public:
    ContourTreeToMesh();
    virtual ~ContourTreeToMesh() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    topology::ContourTreeInport inport_{"contourTree"};
    MeshOutport outport_{"outport"};

    StringProperty treeType_;

    BoolProperty showSaddlepoints_;
    FloatProperty sphereRadius_;
    FloatVec4Property localMaximaColor_;
    FloatVec4Property localMinimaColor_;
    FloatVec4Property saddleColor_;
    FloatVec4Property arcColor_;
};

}  // namespace inviwo
