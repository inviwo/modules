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

#include <inviwo/tensorvisio/tensorvisiomoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/tensorvisbase/ports/tensorfieldport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/tensorvisbase/datastructures/attributes.h>
#include <inviwo/core/util/constexprhash.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorField3DExport, Tensor Field Export}
 * ![](org.inviwo.TensorField3DExport.png?classIdentifier=org.inviwo.TensorField3DExport)
 * Input a tensor field, specify a path where to save the file and hit that button!
 *
 * ### Inports
 *   * __Inport__ Tensor field inport.
 *
 * ### Properties
 *   * __File__ The file which the tensor field will be saved to.
 *   * __Button__ Trigger the saving process.
 *   * __Include Meta data__ Include/exclude meta data in the saved file.
 */

class IVW_MODULE_TENSORVISIO_API TensorField3DExport : public Processor {
public:
    TensorField3DExport();
    virtual ~TensorField3DExport() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    TensorField3DInport inport_;

    CompositeProperty export_;
    FileProperty exportFile_;
    ButtonProperty exportButton_;
    BoolProperty includeMetaData_;

    void exportBinary() const;
};  // namespace inviwo

}  // namespace inviwo
