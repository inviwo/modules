/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <inviwo/c3d/c3dmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/interaction/pickingmapper.h>

#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

#include <inviwo/c3d/ports/c3dport.h>

#include <string>
#include <vector>

namespace inviwo {

class PickingEvent;

/**
 * @brief Creates a point cloud mesh from C3D marker positions over a range of frames.
 *
 * Reads 3D point data from a C3D file and creates a mesh with point rendering
 * suitable for visualizing marker positions. Each marker is represented as a
 * vertex in the mesh. Multiple frames can be extracted simultaneously using
 * the frame range property. Supports picking for tooltips showing point name,
 * frame index, time, and position.
 */
class IVW_MODULE_C3D_API C3DToMesh : public Processor {
public:
    C3DToMesh();

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    void handlePicking(PickingEvent* e);

    C3DDataInport inport_;
    BrushingAndLinkingInport bnl_;
    MeshOutport outport_;

    IntSizeTMinMaxProperty frame_;
    FloatProperty markerRadius_;
    BoolProperty skipEmpty_;
    BoolProperty enableTooltips_;
    PickingMapper picking_;

    std::shared_ptr<const ezc3d::c3d> data_;
};

}  // namespace inviwo
