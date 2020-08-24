/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <inviwo/vortexfeatures/ports/vortexport.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/volumeport.h>
#include <modules/opengl/shader/shader.h>
#include <modules/opengl/shader/shaderresource.h>
#include <modules/opengl/buffer/framebufferobject.h>
#include <modules/opengl/texture/textureunit.h>

#include <inviwo/core/ports/imageport.h>

namespace inviwo {

/** \docpage{org.inviwo.VortexSetToVolume, VortexSet To Volume}
 * ![](org.inviwo.VortexSetToVolume.png?classIdentifier=org.inviwo.VortexSetToVolume)
 * Takes a VortexSet and samples it into a volume.
 * Makes use of the rasterization capabilities of the GPU.
 */
class IVW_MODULE_VORTEXFEATURES_API VortexSetToVolumes : public Processor {
public:
    VortexSetToVolumes();
    virtual ~VortexSetToVolumes() = default;

    virtual void process() override;
    void createMesh();

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    using VortexMesh = TypedMesh<buffertraits::PositionsBuffer, buffertraits::PickingBuffer>;

private:
    VortexSetInport inVortices_;
    VolumeSequenceInport inVolumes_;
    VolumeSequenceOutport outVolumes_;

    IntSize4Property volumeSize_;
    BoolProperty floatVolume_;

    Shader shader_;
    FrameBufferObject fbo_;

    /** One mesh per time slice. **/
    std::vector<VortexMesh> meshes_;
    std::vector<std::vector<GLint>> fanStarts_;
    std::vector<std::vector<GLsizei>> fanCounts_;
    std::shared_ptr<VolumeSequence> volumes_;

    // static const GLuint PRIMITIVE_RESTART = std::numeric_limits<GLuint>::max();  // Magic value.
};

}  // namespace inviwo
