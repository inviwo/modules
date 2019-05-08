/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2018 Inviwo Foundation
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

#ifndef IVW_EIGENVALUEFIELDTOIMAGE_H
#define IVW_EIGENVALUEFIELDTOIMAGE_H

#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/imageport.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <modules/opengl/shader/shader.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/boolproperty.h>

namespace inviwo {

/** \docpage{org.inviwo.EigenvalueFieldToImage, Eigenvalue Field To Image}
 * ![](org.inviwo.EigenvalueFieldToImage.png?classIdentifier=org.inviwo.EigenvalueFieldToImage)
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
 * \class EigenvalueFieldToImage
 * \brief <brief description>
 * <Detailed description from a developer prespective>
 */
class IVW_MODULE_TENSORVISBASE_API EigenvalueFieldToImage : public Processor {
public:
    EigenvalueFieldToImage();
    virtual ~EigenvalueFieldToImage() = default;

    virtual void initializeResources() override;
    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    TensorField2DInport inport_;
    ImageOutport outport_;

    OptionPropertyInt valueType_;

    BoolProperty grayscale_;

    IntVec2Property dimensions_;

    TransferFunctionProperty tf_;
    Shader shader_;
    Image tf_texture_;
    BoolProperty majorMinor_;

    float minVal_;
    float maxVal_;
    float eigenValueRange_;

    float anisotropyMinVal_;
    float anisotropyMaxVal_;
    float anisotropyValueRange_;

    void updateEigenValues();
    void updateAnisotropy();
};

}  // namespace

#endif  // IVW_EIGENVALUEFIELDTOIMAGE_H
