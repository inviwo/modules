/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2020 Inviwo Foundation
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

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/rendering/meshdrawer.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/core/properties/cameraproperty.h>
#include <inviwo/core/interaction/cameratrackball.h>
#include <inviwo/core/properties/simplelightingproperty.h>

#include <modules/basegl/processors/meshrenderprocessorgl.h>

#include <modules/opengl/inviwoopengl.h>
#include <modules/opengl/image/imagecompositor.h>
#include <modules/opengl/shader/shader.h>

#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/tensorvisbase/properties/tensorglyphproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorGlyphRenderer, Tensor Glyph Renderer}
 * ![](org.inviwo.TensorGlyphRenderer.png?classIdentifier=org.inviwo.TensorGlyphRenderer)
 * Explanation of how to use the processor.
 *
 * ### Inports
 *   * __<Inport1>__ <description>.
 *
 * ### Outports
 *   * __<Outport1>__ <description>.
 *
 * ### Properties
 *   * __<Prop1>__ <description>.
 *   * __<Prop2>__ <description>
 */

/**
 * \class TensorGlyphRenderer
 * \brief <brief description>
 * <Detailed description from a developer prespective>
 */
class IVW_MODULE_TENSORVISBASE_API TensorGlyphRenderer : public Processor {
public:
    TensorGlyphRenderer();
    virtual ~TensorGlyphRenderer() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    void handlePickingEvent(PickingEvent*);

private:
    DataInport<std::vector<std::shared_ptr<Mesh>>> meshInport_;
    TensorField3DInport tensorFieldInport_;
    DataInport<size_t> offsetInport_;
    ImageInport imageInport_;
    MeshOutport selectedMeshOutport_;
    DataOutport<size_t> indexOutport_;

    ImageOutport outport_;

    std::vector<std::unique_ptr<MeshDrawer>> meshDrawers_;

    CameraProperty camera_;
    CameraTrackball trackball_;
    OptionPropertyInt cullFace_;
    SimpleLightingProperty lighting_;

    PickingMapper picking_;

    OrdinalProperty<size_t> selectedIDProperty_;

    Shader shader_;

    ImageCompositor compositor_;

    int selectedID_;
    int previouslySelectedID_;
    std::shared_ptr<BasicMesh> selectedMesh_;

    bool triggerSelect_ = false;

    void addCommonShaderDefines(Shader& shader);

    BoolProperty selectMode_;
    TensorGlyphProperty glyphType_;
};

}  // namespace inviwo
