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

#ifndef IVW_TRIANGULATIONTOMESH_H
#define IVW_TRIANGULATIONTOMESH_H

#include <inviwo/topologytoolkit/topologytoolkitmoduledefine.h>
#include <inviwo/topologytoolkit/ports/triangulationdataport.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/ports/meshport.h>

namespace inviwo {

/** \docpage{org.inviwo.ttk.TriangulationToMesh, Triangulation To Mesh}
 * ![](org.inviwo.ttk.TriangulationToMesh.png?classIdentifier=org.inviwo.ttk.TriangulationToMesh)
 * converts a TTK triangulation to a regular Mesh with individual triangles.
 *
 * ### Inports
 *   * __triangulation__ TTK triangulation
 *
 * ### Outports
 *   * __outport__  mesh created from triangulation
 *
 * ### Properties
 *   * __Mesh Color__       color of mesh vertices
 *   * __Map to Component__ if mapping to position is enabled, scalar values of the
 *                          triangulation overwrite this component
 */

/**
 * \class TriangulationToMesh
 * \brief converts topology::TriangulationData into a Mesh
 */
class IVW_MODULE_TOPOLOGYTOOLKIT_API TriangulationToMesh : public Processor {
public:
    TriangulationToMesh();
    virtual ~TriangulationToMesh() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    topology::TriangulationInport inport_;
    MeshOutport outport_;

    FloatVec4Property color_;
    BoolProperty mapScalars_;
    OptionPropertyInt component_;
};

}  // namespace inviwo

#endif  // IVW_TRIANGULATIONTOMESH_H
