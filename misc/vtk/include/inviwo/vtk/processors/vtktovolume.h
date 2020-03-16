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
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/processors/activityindicator.h>
#include <inviwo/core/properties/compositeproperty.h>
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

/** \docpage{org.inviwo.VTKtoVolume, VTKtoVolume}
 * ![](org.inviwo.VTKtoVolume.png?classIdentifier=org.inviwo.VTKtoVolume)
 * Converts a VTK data set to a base volume, assuming that it is axis aligned and uniform.
 */

/**
 * \brief Convert rectlinear VTK data to base volume
 * Builds a new inviwo volume with the data from the given VTK data set.
 * Checks if the underlying grid is regular.
 */
class IVW_MODULE_VTK_API VTKtoVolume : public Processor {
public:
    VTKtoVolume();
    virtual ~VTKtoVolume() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    static const DataFormatBase* getFormatFromVtkId(int vtkTypeId, int bitSize);
    virtual void deserialize(Deserializer&) override;
    virtual void serialize(Serializer& s) const override;

private:
    void updateFormats();
    void updateArrays(bool keepSettings = false);
    void convertData();
    void updateAvailableArrays();

    VolumeOutport outport_;
    VTKDataSetInport inport_;
    BoolProperty useCellData_;
    std::shared_ptr<Volume> data_;

    struct VTKArrayList : CompositeProperty {
        VTKArrayList(const std::string& identifier, const std::string& displayName)
            : CompositeProperty(identifier, displayName) {}
        void clear();
        void addArray(const std::string& name, int numChannels, void* dataPtr);
        DataFormatId selectedFormat;
        std::vector<size_t> numArrayComponents;
        std::vector<unsigned char*> ptrArrayData;

    protected:
        using CompositeProperty::addProperties;
        using CompositeProperty::addProperty;
    } dataArrays_;
    TemplateOptionProperty<DataFormatId> dataFormats_;
    ButtonProperty convertButton_;
    DoubleVec2Property dataRange_;

    bool reactToChange_;
    std::vector<std::string> formerArraySelection_;
};

}  // namespace inviwo
