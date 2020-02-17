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
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/processors/progressbarowner.h>
#include <inviwo/core/processors/activityindicator.h>
#include <inviwo/vtk/ports/vtkdatasetport.h>
#include <inviwo/vtk/util/vtkutil.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkSmartPointer.h>
#include <vtkXMLGenericDataObjectReader.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkDataSet.h>
#include <vtkDataObject.h>
#include <vtkCallbackCommand.h>
#include <warn/pop>

namespace inviwo {

/** \docpage{org.inviwo.VTKReader, VTKReader}
 * ![](org.inviwo.VTKReader.png?classIdentifier=org.inviwo.VTKReader)
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
class IVW_MODULE_VTK_API VTKReader : public Processor,
                                     public ProgressBarOwner,
                                     public ActivityIndicatorOwner {
public:
    VTKReader();
    virtual ~VTKReader() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    enum class VTKFileType { XML, Legacy, Unknown };

    FileProperty file_;
    ButtonProperty reloadButton_;
    VTKDataSetOutport outport_;

    std::shared_ptr<VTKDataSet> data_;
    vtkSmartPointer<vtkDataSet> dataSet_;

    VTKFileType determineFileType(const std::string& fileName) const;

    template <VTKFileType T>
    void read();
};

template <VTKReader::VTKFileType T>
inline void VTKReader::read() {
    if constexpr (T != VTKFileType::Unknown) {
        using ReaderType = std::conditional_t<T == VTKFileType::Legacy, vtkGenericDataObjectReader,
                                              vtkXMLGenericDataObjectReader>;

        dispatchPool([this]() {
            dispatchFront([this]() { getActivityIndicator().setActive(true); });

            auto progressCallback = vtkSmartPointer<vtkCallbackCommand>::New();
            progressCallback->SetCallback(vtkProgressBarCallback);
            progressCallback->SetClientData(&progressBar_);

            auto reader = vtkSmartPointer<ReaderType>::New();

            reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);
            reader->SetFileName(file_.get().c_str());
            reader->Update();

            dataSet_ = vtkDataSet::SafeDownCast(reader->GetOutput());

            dispatchFront([this]() {
                data_ = std::make_shared<VTKDataSet>(dataSet_);
                getActivityIndicator().setActive(false);
                outport_.setData(data_);
                invalidate(InvalidationLevel::InvalidOutput);
            });
        });
    }
}

}  // namespace inviwo
