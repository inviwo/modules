/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
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

#ifndef IVW_MESHDECIMATIONPROCESSOR_H
#define IVW_MESHDECIMATIONPROCESSOR_H

#include <modules/openmesh/openmeshmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/meshport.h>

namespace inviwo {

/** \docpage{org.inviwo.MeshDecimationProcessor, Mesh Decimation}
 * ![](org.inviwo.MeshDecimationProcessor.png?classIdentifier=org.inviwo.MeshDecimationProcessor)
 *
 * Reduces the number of triangles in the input mesh.
 * Stops when either the Vertex- or the Face decimation ratio is reach.
 *
 *
 * ### Inports
 *   * __inmesh__ Input mesh.
 *
 * ### Outports
 *   * __outmesh__ Decimated mesh.
 *
 * ### Properties
 *   * __Vertex Decimation ratio__ Percentage of vertices to keep.
 *   * __Face Decimation ratio__ Percentage of faces to keep.
 */

class IVW_MODULE_OPENMESH_API MeshDecimationProcessor : public Processor {
public:
    MeshDecimationProcessor();
    virtual ~MeshDecimationProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    MeshInport inmesh_{"inmesh"};
    MeshOutport outmesh_{"outmesh"};

    FloatProperty vertDecimation_{
        "vertDecimation", "Vertex Decimation ratio", 0.5f, 0.f, 1.f, 0.01f};
    FloatProperty faceDecimation_{"faceDecimation", "Face Decimation ratio", 0.5f, 0.f, 1.f, 0.01f};
};

}  // namespace inviwo

#endif  // IVW_MESHDECIMATIONPROCESSOR_H
