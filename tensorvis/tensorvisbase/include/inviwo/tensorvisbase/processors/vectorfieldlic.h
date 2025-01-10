/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#ifndef IVW_VECTORFIELDLIC_H
#define IVW_VECTORFIELDLIC_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/processors/processor.h>
#include <modules/tensorvisualization/tensorvisualizationmoduledefine.h>
#include <modules/tensorvisualization/datastructures/tensorfield2d.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/tensorvisualization/util/tensorfieldutil.h>

namespace inviwo {

/** \docpage{org.inviwo.VectorFieldLIC, VectorFieldLIC}
 * ![](org.inviwo.<name>.png?classIdentifier=org.inviwo.VectorFieldLIC)
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
 * \class VectorFieldLIC
 * \brief VERY_BRIEFLY_DESCRIBE_THE_CLASS
 * DESCRIBE_THE_CLASS_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_TENSORVISUALIZATION_API VectorFieldLIC : public Processor {
public:
    VectorFieldLIC();
    virtual ~VectorFieldLIC() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    ImageInport vectorField_;
    ImageInport noiseTexture_;
    ImageInport imageInport_;
    ImageOutport outport_;

    IntProperty samples_;
    FloatProperty stepLength_;
    BoolProperty normalizeVectors_;
    BoolProperty intensityMapping_;
    BoolProperty useRK4_;

    Shader shader_;
    Image tf_texture_;
};

}  // namespace inviwo

#endif  // IVW_VECTORFIELDLIC_H
