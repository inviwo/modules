/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2019 Inviwo Foundation
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
 **********************************************************************************
 *
 * Author: Jochen Jankowai
 * Contact: jochen.jankowai@liu.se
 *
 *********************************************************************************/

#ifndef IVW_VGTEXTOVERLAYPROCESSOR_H
#define IVW_VGTEXTOVERLAYPROCESSOR_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/vgfontrendering/vgfontrenderingmoduledefine.h>
#include <inviwo/nanovgutils/nanovgutils.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/opengl/openglutils.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/ports/imageport.h>

namespace inviwo {

class IVW_MODULE_VGFONTRENDERING_API VGTextOverlayProcessor : public Processor {
public:
    VGTextOverlayProcessor();
    virtual ~VGTextOverlayProcessor();

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    virtual void process() override;
    virtual void initializeResources() override;

private:
    ImageInport inport_;
    ImageOutport outport_;

    StringProperty text_;
    FileProperty file_;
    IntVec2Property position_;
    FloatProperty size_;
    FloatProperty textBoxWidth_;
    FloatVec4Property color_;
    int numFontsLoaded_;

    OptionPropertyInt horizontalAlignment_;

    OptionPropertyInt font_;

    void setAbsoluteAnchorPositionBounds(vec4 textBoxBounds);

    NanoVGContext* nvgContext_;
};

}  // namespace inviwo

#endif  // IVW_VGTEXTOVERLAYPROCESSOR_H
