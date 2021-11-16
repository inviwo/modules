/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
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

#include <warn/push>
#include <warn/ignore/all>
#include <vtkSmartPointer.h>
#include <vtkXMLGenericDataObjectReader.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkDataSet.h>
#include <vtkDataObject.h>
#include <warn/pop>

namespace inviwo {

/** \docpage{org.inviwo.VTKReader, VTKReader}
 * ![](org.inviwo.VTKReader.png?classIdentifier=org.inviwo.VTKReader)
 * Browse for a VTK file to load it.
 *
 * ### Outports
 *   * __VTKDataObjectOutport__ Outputs the VTK data object.
 *
 * ### Properties
 *   * __VTK file__ VTK file to load.
 *   * __Reload Data__ If you have updated the data outside Inviwo, you can use this button to
 * manually reload the data.
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
    enum class VTKFileType { XML_Serial, XML_Parallel, Legacy, Unknown };

    FileProperty file_;
    ButtonProperty reloadButton_;
    VTKDataSetOutport outport_;

    vtkSmartPointer<vtkXMLGenericDataObjectReader> xmlreader_;
    vtkSmartPointer<vtkGenericDataObjectReader> legacyreader_;

    std::shared_ptr<VTKDataSet> data_;
    vtkSmartPointer<vtkDataSet> dataSet_;

    VTKFileType determineFileType(const std::string& fileName) const;

    template <VTKFileType T>
    vtkSmartPointer<vtkDataSet> read(const std::string path) {
        using V = std::conditional_t<T == VTKFileType::Legacy, vtkGenericDataObjectReader,
                                     vtkXMLGenericDataObjectReader>;

        auto reader = vtkSmartPointer<V>::New();

        reader->SetFileName(path.c_str());
        reader->Update();

        return vtkDataSet::SafeDownCast(V::SafeDownCast(reader.GetPointer())->GetOutput());
    }
};

}  // namespace inviwo
