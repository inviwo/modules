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

#include <inviwo/vtk/processors/vtktovolume.h>
#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/common/inviwoapplication.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/util/clock.h>
#include <inviwo/core/util/formats.h>
// #include <modules/base/algorithm/dataminmax.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkCallbackCommand.h>
#include <warn/pop>

#include <vtkInformation.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkImageData.h>
#include <vtkMatrix4x4.h>
#include <vtkType.h>

#include <fstream>
#include <inviwo/vtk/util/vtkutil.h>
#include <fmt/format.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKtoVolume::processorInfo_{
    "org.inviwo.VTKtoVolume",     // Class identifier
    "VTK to Volume",              // Display name
    "VTK",                        // Category
    CodeState::Experimental,      // Code state
    "CPU, Data Conversion, VTK",  // Tags
};
const ProcessorInfo VTKtoVolume::getProcessorInfo() const { return processorInfo_; }

VTKtoVolume::VTKtoVolume()
    : Processor()
    , outport_("VolumeOutport")
    , inport_("VTKDataObjectOutport")
    , useCellData_("useCellData", "Use cell data", false)
    , dataArrays_("availableArrays", "Data Arrays")
    , dataFormats_("existingDataFormats", "Data formats")
    , convertButton_("triggerConvert", "Convert")
    , dataRange_("dataRange", "Data range", vec2(0, 1))
    , reactToChange_(true)
    , formerArraySelection_() {

    addPort(outport_);
    addPort(inport_);
    addProperty(useCellData_);
    addProperty(dataFormats_);
    addProperty(dataArrays_);
    addProperty(convertButton_);
    addProperty(dataRange_);

    inport_.onChange([this]() {
        if (reactToChange_) updateFormats();
    });
    useCellData_.onChange([this]() { updateFormats(); });
    dataFormats_.onChange([this]() {
        if (!reactToChange_) return;
        dataArrays_.clear();
        updateArrays();
    });
    convertButton_.onChange([this]() { convertData(); });
    dataArrays_.onChange([this]() {
        if (reactToChange_) updateAvailableArrays();
    });
}

void VTKtoVolume::process() {}

void VTKtoVolume::deserialize(Deserializer &d) {
    reactToChange_ = false;
    Processor::deserialize(d);

    dataArrays_.clear();
    std::string allSelected;
    d.deserialize("selectedArrays", allSelected, SerializationTarget::Attribute);
    std::stringstream sstream(allSelected);
    std::string token;
    while (std::getline(sstream, token, ';')) formerArraySelection_.push_back(token);
    reactToChange_ = true;
}

void VTKtoVolume::serialize(Serializer &s) const {
    Processor::serialize(s);

    std::stringstream allSelected;
    for (size_t a = 0; a < dataArrays_.size(); ++a) {
        for (auto *prop : dataArrays_.getProperties()) {
            BoolProperty *boolProp = dynamic_cast<BoolProperty *>(prop);
            if (boolProp && boolProp->get()) allSelected << boolProp->getDisplayName() << ';';
        }
    }
    s.serialize("selectedArrays", allSelected.str(), SerializationTarget::Attribute);
}

void VTKtoVolume::VTKArrayList::addArray(const std::string &name, int numChannels, void *dataPtr) {
    auto boolProp = new BoolProperty(name, name + " (" + std::to_string(numChannels) + ')', false);
    addProperty(boolProp);
    numArrayComponents.push_back(numChannels);
    ptrArrayData.push_back(reinterpret_cast<unsigned char *>(dataPtr));
}

void VTKtoVolume::VTKArrayList::clear() {
    std::vector<std::string> identifiers{};
    for (const auto &property : getProperties()) {
        identifiers.emplace_back(property->getIdentifier());
    }
    for (const auto &identifier : identifiers) {
        Property *prop = removeProperty(identifier);
        delete prop;
    }
    numArrayComponents.clear();
    ptrArrayData.clear();
}

void VTKtoVolume::updateFormats() {
    NetworkLock lock;
    reactToChange_ = false;

    DataFormatId formerFormat = DataFormatId::NotSpecialized;
    if (dataFormats_.size()) formerFormat = dataFormats_.getSelectedValue();

    dataFormats_.clearOptions();
    if (!inport_.hasData()) {
        dataArrays_.clear();
        return;
    }

    // Get dimensions. If none are given, it's not a rectilinear grid.
    const VTKDataSet &vtkData = *inport_.getData();
    std::optional<size3_t> dimensions = inport_.getData()->getDimensions();
    if (!dimensions) {
        LogWarn("Input vtk data set is not rectilinear.");
        return;
    }

    // Get data attributes.
    auto cellData = vtkData->GetCellData();
    auto pointData = vtkData->GetPointData();
    if ((useCellData_.get() && !cellData) || (!useCellData_.get() && !pointData)) return;
    int numArrays =
        useCellData_.get() ? cellData->GetNumberOfArrays() : pointData->GetNumberOfArrays();

    // Find first non-internal data array and take that value type.
    std::vector<int> insertedValues;
    for (int i = 0; i < numArrays; ++i) {
        auto array = useCellData_.get() ? cellData->GetArray(i) : pointData->GetArray(i);
        int vtkTypeId = array->GetDataType();

        if (std::find(insertedValues.begin(), insertedValues.end(), vtkTypeId) !=
            insertedValues.end())
            continue;

        const DataFormatBase *format =
            getFormatFromVtkId(vtkTypeId, array->GetElementComponentSize() * 8);
        if (!format) continue;

        insertedValues.push_back(vtkTypeId);
        dataFormats_.addOption(format->getString(), format->getString(), format->getId());
    }

    // Try to keep arrays if the new input has arrays of the same type.
    bool keep = dataFormats_.setSelectedValue(formerFormat);

    if (dataFormats_.size())
        updateArrays(keep);
    else
        dataArrays_.clear();

    reactToChange_ = true;
}

void VTKtoVolume::updateArrays(bool keepSettings) {
    NetworkLock lock;
    reactToChange_ = false;

    // Remember selected channels
    std::unordered_set<std::string> formerChannels;
    if (keepSettings) {
        if (!formerArraySelection_.empty()) {
            for (auto &name : formerArraySelection_) formerChannels.insert(name);
            formerArraySelection_.clear();
        } else {
            for (auto *prop : dataArrays_.getProperties())
                if (dynamic_cast<BoolProperty *>(prop)->get())
                    formerChannels.insert(prop->getDisplayName());
        }
    }

    dataArrays_.clear();
    if (!dataFormats_.size() || !inport_.hasData()) {
        reactToChange_ = true;
        return;
    }
    const VTKDataSet &vtkData = *inport_.getData();

    auto selectedFormat = dataFormats_.getSelectedValue();
    auto cellData = vtkData->GetCellData();
    auto pointData = vtkData->GetPointData();
    if ((useCellData_.get() && !cellData) || (!useCellData_.get() && !pointData)) return;
    int numArrays =
        useCellData_.get() ? cellData->GetNumberOfArrays() : pointData->GetNumberOfArrays();

    int numTotalComps = 0;
    for (int i = 0; i < numArrays; ++i) {
        auto array = useCellData_.get() ? cellData->GetArray(i) : pointData->GetArray(i);
        std::string arrayName(array->GetName());

        int vtkTypeId = array->GetDataType();
        const DataFormatBase *format =
            getFormatFromVtkId(vtkTypeId, array->GetElementComponentSize() * 8);
        if (format->getId() != selectedFormat) continue;
        int numComps = array->GetNumberOfComponents();

        numTotalComps += numComps;
        dataArrays_.addArray(arrayName, numComps, array->GetVoidPointer(0));
        if (keepSettings) {
            BoolProperty *prop = dynamic_cast<BoolProperty *>(dataArrays_[dataArrays_.size() - 1]);
            if (prop && formerChannels.find(prop->getDisplayName()) != formerChannels.end())
                prop->set(true);
        }
    }
    if (!keepSettings && numTotalComps <= 4)
        for (auto *prop : dataArrays_.getProperties())
            if (BoolProperty *boolProp = dynamic_cast<BoolProperty *>(prop)) boolProp->set(true);

    dataArrays_.selectedFormat = dataFormats_.get();

    if (keepSettings) convertData();

    reactToChange_ = true;
}

void VTKtoVolume::updateAvailableArrays() {
    NetworkLock lock;
    reactToChange_ = false;

    int numTotalComps = 0;
    auto properties = dataArrays_.getProperties();
    for (size_t p = 0; p < properties.size(); ++p) {
        if (dynamic_cast<BoolProperty *>(properties[p])->get())
            numTotalComps += dataArrays_.numArrayComponents[p];
    }

    for (size_t p = 0; p < properties.size(); ++p) {
        if (dynamic_cast<BoolProperty *>(properties[p])->get()) continue;
        bool tooMuch = numTotalComps + dataArrays_.numArrayComponents[p] > 4;
        properties[p]->setReadOnly(tooMuch);
    }

    reactToChange_ = true;
}

void VTKtoVolume::convertData() {
    outport_.setData(nullptr);
    if (!inport_.hasData()) return;

    auto &vtkData = *inport_.getData();

    // Get dimensions. If none are given, it's not a rectilinear grid.
    std::optional<size3_t> dimensionsOpt = vtkData.getDimensions();
    if (!dimensionsOpt) {
        LogWarn("Input vtk data set is not rectilinear.");
        return;
    }
    size3_t dimensions = *dimensionsOpt;
    if (useCellData_.get())
        for (size_t d = 0; d < 3; ++d) dimensions[d]--;

    struct RawArrayData {
        unsigned char *ptr;
        size_t byteSize;
    };
    std::vector<RawArrayData> arrayDataPointers;

    // How many components in total?
    const DataFormatBase *format = DataFormatBase::get(dataFormats_.getSelectedValue());
    size_t sizeElement = format->getSize();
    size_t numTotalComps = 0;
    for (size_t p = 0; p < dataArrays_.size(); ++p) {
        BoolProperty *prop = dynamic_cast<BoolProperty *>(dataArrays_[p]);
        if (prop && prop->get()) {
            arrayDataPointers.push_back(
                {dataArrays_.ptrArrayData[p], dataArrays_.numArrayComponents[p] * sizeElement});
            numTotalComps += dataArrays_.numArrayComponents[p];
        }
    }
    if (!numTotalComps) {
        LogWarn("No components selected.");
        return;
    }

    const DataFormatBase *multichannelFormat =
        DataFormatBase::get(format->getNumericType(), numTotalComps, format->getSize() * 8);

    // Use point bounds
    Matrix<4, float> worldMatrix(0);
    double bounds[6];
    vtkData->GetBounds(bounds);
    for (size_t x = 0; x < 3; ++x) {
        worldMatrix[x][x] = (bounds[x * 2 + 1] - bounds[x * 2]) / dimensions[x];
        worldMatrix[3][x] = bounds[x * 2];
    }
    worldMatrix[3][3] = 1;

    // Create new volume
    auto volume = std::make_shared<Volume>(dimensions, multichannelFormat);
    volume->setModelMatrix(worldMatrix);

    // Create RAM volume
    auto volRAM = createVolumeRAM(dimensions, multichannelFormat);
    unsigned char *dataPtr = static_cast<unsigned char *>(volRAM->getData());

    // Copy the data from the other volumes to the new multichannel volume
    size_t numElements = dimensions.x * dimensions.y * dimensions.z;
    for (size_t e = 0; e < numElements; e++) {
        for (auto &arrayData : arrayDataPointers) {
            memcpy(dataPtr, arrayData.ptr, arrayData.byteSize);
            dataPtr += arrayData.byteSize;
            arrayData.ptr += arrayData.byteSize;
        }
    }

    volume->addRepresentation(volRAM);
    double minVal(std::numeric_limits<double>::max()), maxVal(std::numeric_limits<double>::min());
    for (size_t z = 0; z < dimensions.z; ++z)
        for (size_t y = 0; y < dimensions.y; ++y)
            for (size_t x = 0; x < dimensions.x; ++x) {
                auto data = volRAM->getAsDVec4(size3_t(x, y, z));
                for (size_t d = 0; d < numTotalComps; ++d) {
                    minVal = std::min(data[d], minVal);
                    maxVal = std::max(data[d], maxVal);
                }
            }

    volume->dataMap_.valueRange = {minVal, maxVal};
    volume->dataMap_.dataRange = {dataRange_.get().x, dataRange_.get().y};
    outport_.setData(volume);
}

const DataFormatBase *VTKtoVolume::getFormatFromVtkId(int vtkTypeId, int bitSize) {
    switch (vtkTypeId) {
        case VTK_CHAR:
        case VTK_SIGNED_CHAR:
        case VTK_SHORT:
        case VTK_INT:
        case VTK_LONG:
        case VTK_LONG_LONG:
        case VTK___INT64:
            return DataFormatBase::get(NumericType::SignedInteger, 1, bitSize);

        case VTK_UNSIGNED_CHAR:
        case VTK_UNSIGNED_SHORT:
        case VTK_UNSIGNED_INT:
        case VTK_UNSIGNED_LONG:
        case VTK_UNSIGNED_LONG_LONG:
        case VTK_UNSIGNED___INT64:
        case VTK_ID_TYPE:
            return DataFormatBase::get(NumericType::UnsignedInteger, 1, bitSize);

        case VTK_FLOAT:
        case VTK_DOUBLE:
            return DataFormatBase::get(NumericType::Float, 1, bitSize);
        default:
            return nullptr;
    }
}

}  // namespace inviwo
