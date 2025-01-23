/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2025 Inviwo Foundation
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

#include <inviwo/molvisgl/molvisglmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/interaction/cameratrackball.h>
#include <inviwo/core/properties/cameraproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/simplelightingproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/meshport.h>

#include <modules/basegl/datastructures/meshshadercache.h>

namespace inviwo {

/** \docpage{org.inviwo.MolecularMeshRenderer, Molecular Mesh Renderer}
 * ![](org.inviwo.MolecularMeshRenderer.png?classIdentifier=org.inviwo.MolecularMeshRenderer)
 * Renders the input mesh in a molecular representation. The input mesh is expected to contain
 * molecular data, that is points representing individual atoms (including their radius) and lines
 * indicating bonds.
 *
 * The result is depending on the chosen molecular representation.
 *    - VDW (van der Waals): considers only points
 *    - Licorice:            considers both points and lines
 *    - Ball & Stick:        considers both points and lines
 * If the mesh does not contain points or lines, the resulting rendering will lack either
 * atoms/spheres or bonds.
 *
 * ### Inports
 *   * __geometry__ input meshes containing molecular data
 *   * __imageInport__ Optional background image
 *
 * ### Outports
 *   * __outport__ output image containing the moleculare rendering of the input
 */
class IVW_MODULE_MOLVISGL_API MolecularMeshRenderer : public Processor {
public:
    MolecularMeshRenderer();
    virtual ~MolecularMeshRenderer() = default;

    virtual void process() override;

    virtual void initializeResources() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    enum class Representation { VDW, Licorice, BallAndStick, Ribbon, Cartoon };

    const float BallAndStickVDWScale = 0.3f;
    const float BallAndStickLicoriceScale = 0.5f;

    void configureVdWShader(Shader& shader);
    void configureLicoriceShader(Shader& shader);

    MeshFlatMultiInport inport_;
    ImageInport imageInport_;
    ImageOutport outport_;

    OptionProperty<Representation> representation_;

    FloatProperty radiusScaling_;
    BoolProperty forceRadius_;
    FloatProperty defaultRadius_;

    CameraProperty camera_;
    SimpleLightingProperty lighting_;
    CameraTrackball trackball_;

    MeshShaderCache vdwShaders_;
    MeshShaderCache licoriceShaders_;
};

}  // namespace inviwo
