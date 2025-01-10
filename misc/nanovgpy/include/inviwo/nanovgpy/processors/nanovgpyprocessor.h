/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2025 Inviwo Foundation
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

#ifndef IVW_NANOVGPYPROCESSOR_H
#define IVW_NANOVGPYPROCESSOR_H

#include <inviwo/nanovgpy/nanovgpymoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/properties/stringproperty.h>
#include <modules/python3/pythonscript.h>

namespace inviwo {

/** \docpage{org.inviwo.NanoVGPyProcessor, Nano VGPy Processor}
 * ![](org.inviwo.NanoVGPyProcessor.png?classIdentifier=org.inviwo.NanoVGPyProcessor)
 *
 * Processor that allows a python script to render to an Image using NanoVG.
 *
 * ### Inports
 *   * __background__ Optional image to use as background.
 *   * __inVec2__ Optional multi inport, will be exposed as vec2s[i] in the script.
 *   * __inVec3__ Optional multi inport, will be exposed as vec3s[i] in the script
 *   * __inVec4__ Optional multi inport, will be exposed as vec4s[i] in the script
 *   * __inVols__ Optional multi inport, will be exposed as vol[i] in the script
 *   * __inMeshes__ Optional multi inport, will be exposed as mesh[i] in the script
 *   * __inImgs__ Optional multi inport, will be exposed as img[i] in the script
 *
 * ### Outports
 *   * __outport__ Resulting image.
 *
 * ### Properties
 *   * __Source__ The script to be run on each call to process.
 *
 */
class IVW_MODULE_NANOVGPY_API NanoVGPyProcessor : public Processor {
public:
    NanoVGPyProcessor();
    virtual ~NanoVGPyProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo& getProcessorInfo() const override;

    static const ProcessorInfo processorInfo_;

private:
    ImageInport background_;

    DataInport<std::vector<vec2>, 0> inVec2_;
    DataInport<std::vector<vec3>, 0> inVec3_;
    DataInport<std::vector<vec4>, 0> inVec4_;
    VolumeSequenceInport inVols_;
    MeshMultiInport inMeshes_;
    ImageMultiInport inImgs_;

    ImageOutport outport_;

    StringProperty src_;

    PythonScript script_;
};

}  // namespace inviwo

#endif  // IVW_NANOVGPYPROCESSOR_H
