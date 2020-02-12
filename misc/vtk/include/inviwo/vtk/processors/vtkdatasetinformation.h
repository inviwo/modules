/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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

#include <inviwo/vtk/vtkmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/vtk/ports/vtkdatasetport.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkDataSet.h>
#include <warn/pop>

namespace inviwo {

/** \docpage{org.inviwo.VTKDataSetInformation, VTKData Set Information}
 * ![](org.inviwo.VTKDataSetInformation.png?classIdentifier=org.inviwo.VTKDataSetInformation)
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
 * \brief VERY_BRIEFLY_DESCRIBE_THE_PROCESSOR
 * DESCRIBE_THE_PROCESSOR_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_VTK_API VTKDataSetInformation : public Processor {
public:
    VTKDataSetInformation();
    virtual ~VTKDataSetInformation() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    class ArrayInformationProperty : public CompositeProperty {
    public:
        ArrayInformationProperty() = delete;
        ArrayInformationProperty(const std::string &arrayName, const std::string &identifier,
                                 const std::string &dataType, const std::string &numberOfComponents)
            : CompositeProperty(identifier, arrayName)
            , dataType_(identifier + "dataType", "Data type", dataType)
            , numberOfComponents_(identifier + "numberOfComponents", "Components",
                                  numberOfComponents) {
            addProperty(dataType_);
            addProperty(numberOfComponents_);
            dataType_.setReadOnly(true);
            numberOfComponents_.setReadOnly(true);
        }

        const std::string &getDataType() const { return dataType_.get(); }
        const std::string &getNumberOfComponents() const { return numberOfComponents_.get(); }

    private:
        StringProperty dataType_;
        StringProperty numberOfComponents_;
    };

    VTKDataSetInport inport_;

    StringProperty className_;
    CompositeProperty pointDataArrays_;
    CompositeProperty cellDataArrays_;
};

}  // namespace inviwo
