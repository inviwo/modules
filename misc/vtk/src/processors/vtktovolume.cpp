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

#include <inviwo/vtk/processors/vtktovolume.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/util/formats.h>
#include <modules/base/algorithm/dataminmax.h>

#include <warn/push>
#include <warn/ignore/all>
#include <warn/pop>

#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGrid.h>
#include <vtkImageData.h>
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
    , inport_("VTKDataObjectInport")
    , useCellData_("useCellData", "Use cell data", false)
    , dataArrays_("availableArrays", "Data Arrays")
    , dataFormats_("existingDataFormats", "Data formats")
    , convertButton_("triggerConvert", "Convert", [this]() { convertData_ = true; })
    , autoConvert_("autoConvert", "Auto-update", false)
    , dataRange_("dataRange", "Data range", vec2(0, 1))
    , isDirty_(false)
    , convertData_(false)
    , prevArraySelection_() {

    addPort(outport_);
    addPort(inport_);
    addProperty(autoConvert_);
    addProperty(convertButton_);
    addProperty(useCellData_);
    addProperty(dataFormats_);
    addProperty(dataArrays_);
    addProperty(dataRange_);

    inport_.onChange([this]() {
        if (!isDirty_) updateFormats();
    });
    useCellData_.onChange([this]() { updateFormats(); });
    dataFormats_.onChange([this]() {
        if (isDirty_) return;
        dataArrays_.clear();
        updateArrays();
    });
    dataArrays_.onChange([this]() {
        if (!isDirty_) updateAvailableArrays();
    });
}

void VTKtoVolume::process() {
    if (convertData_ || autoConvert_.get()) {
        convertData();
        convertData_ = false;
    }
}

void VTKtoVolume::deserialize(Deserializer& d) {
    isDirty_ = true;
    Processor::deserialize(d);
    dataArrays_.clear();
    isDirty_ = false;

    d.deserialize("selectedArrays", prevArraySelection_);
}

void VTKtoVolume::serialize(Serializer& s) const {
    Processor::serialize(s);
    std::vector<std::string> arraySelection;
    auto boolProps = dataArrays_.getPropertiesByType<BoolProperty>();
    for (auto* prop : boolProps) {
        if (prop->get()) arraySelection.push_back(prop->getDisplayName());
    }

    s.serialize("selectedArrays", arraySelection);
}

void VTKtoVolume::VTKArrayList::addArray(const std::string& name, int numChannels, void* dataPtr) {
    auto boolProp = new BoolProperty(name, name + " (" + std::to_string(numChannels) + ')', false);
    addProperty(boolProp);
    numArrayComponents.push_back(numChannels);
    ptrArrayData.push_back(reinterpret_cast<unsigned char*>(dataPtr));
}

void VTKtoVolume::VTKArrayList::clear() {
    while (size() > 0) removeProperty(size_t(0));
    numArrayComponents.clear();
    ptrArrayData.clear();
}

void VTKtoVolume::updateFormats() {
    NetworkLock lock;
    isDirty_ = true;

    DataFormatId prevFormat = DataFormatId::NotSpecialized;
    if (dataFormats_.size()) prevFormat = dataFormats_.getSelectedValue();

    dataFormats_.clearOptions();
    if (!inport_.hasData()) {
        dataArrays_.clear();
        return;
    }

    // Get dimensions. If none are given, it's not a rectilinear grid.
    const VTKDataSet& vtkData = *inport_.getData();
    std::optional<size3_t> dimensions = inport_.getData()->getDimensions();
    if (!dimensions) {
        LogWarn("Input vtk data set is not regular.");
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

        const DataFormatBase* format =
            vtkutil::getFormatFromVtkId(vtkTypeId, array->GetElementComponentSize() * 8);
        if (!format) continue;

        insertedValues.push_back(vtkTypeId);
        dataFormats_.addOption(format->getString(), format->getString(), format->getId());
    }

    dataFormats_.setSelectedValue(prevFormat);

    if (dataFormats_.size())
        updateArrays();
    else
        dataArrays_.clear();

    isDirty_ = false;
}

void VTKtoVolume::updateArrays() {
    NetworkLock lock;
    isDirty_ = true;

    // Remember selected channels
    std::vector<std::string> prevChannels;
    if (!prevArraySelection_.empty()) {
        prevChannels = prevArraySelection_;
        prevArraySelection_.clear();
    } else {
        for (auto* prop : dataArrays_.getPropertiesByType<BoolProperty>()) {
            if (prop->get()) {
                prevChannels.push_back(prop->getDisplayName());
            }
        }
    }
    dataArrays_.clear();
    if (!dataFormats_.size() || !inport_.hasData()) {
        isDirty_ = false;
        return;
    }
    const VTKDataSet& vtkData = *inport_.getData();

    auto selectedFormat = dataFormats_.getSelectedValue();
    auto cellData = vtkData->GetCellData();
    auto pointData = vtkData->GetPointData();
    if ((useCellData_.get() && !cellData) || (!useCellData_.get() && !pointData)) return;
    int numArrays =
        useCellData_.get() ? cellData->GetNumberOfArrays() : pointData->GetNumberOfArrays();

    int numTotalComps = 0;
    size_t numSelectedComps = 0;
    for (int i = 0; i < numArrays; ++i) {
        auto array = useCellData_.get() ? cellData->GetArray(i) : pointData->GetArray(i);
        std::string arrayName(array->GetName());

        int vtkTypeId = array->GetDataType();
        const DataFormatBase* format =
            vtkutil::getFormatFromVtkId(vtkTypeId, array->GetElementComponentSize() * 8);
        if (format->getId() != selectedFormat) continue;
        int numComps = array->GetNumberOfComponents();

        numTotalComps += numComps;
        dataArrays_.addArray(arrayName, numComps, array->GetVoidPointer(0));

        // Set previous arrays to true if existing.
        BoolProperty* prop = dynamic_cast<BoolProperty*>(dataArrays_[dataArrays_.size() - 1]);
        if (prop && std::find(prevChannels.begin(), prevChannels.end(), prop->getDisplayName()) !=
                        prevChannels.end()) {
            prop->set(true);
            numSelectedComps++;
        }
    }
    bool containsAllFormerArrays =
        (numSelectedComps == prevChannels.size()) && prevChannels.size() > 0;
    if (!containsAllFormerArrays && numTotalComps <= 4) {
        for (auto* prop : dataArrays_.getProperties()) {
            if (BoolProperty* boolProp = dynamic_cast<BoolProperty*>(prop)) boolProp->set(true);
        }
    }
    dataArrays_.selectedFormat = dataFormats_.get();

    convertButton_.setDisplayName(numSelectedComps > 0 ? "Convert" : "Please Select Arrays");
    if (containsAllFormerArrays) convertData_ = true;

    isDirty_ = false;
}

void VTKtoVolume::updateAvailableArrays() {
    NetworkLock lock;
    isDirty_ = true;

    size_t numTotalComps = 0;
    auto properties = dataArrays_.getProperties();
    for (size_t p = 0; p < properties.size(); ++p) {
        if (dynamic_cast<BoolProperty*>(properties[p])->get())
            numTotalComps += dataArrays_.numArrayComponents[p];
    }

    for (size_t p = 0; p < properties.size(); ++p) {
        if (dynamic_cast<BoolProperty*>(properties[p])->get()) continue;
        bool tooMuch = numTotalComps + dataArrays_.numArrayComponents[p] > 4;
        properties[p]->setReadOnly(tooMuch);
    }
    convertButton_.setDisplayName(numTotalComps > 0 ? "Convert" : "Please Select Arrays");

    isDirty_ = false;
}

void VTKtoVolume::convertData() {
    outport_.setData(nullptr);
    if (!inport_.hasData()) return;

    auto vtkDataPtr = inport_.getData();

    // Get dimensions. If none are given, it's not a regular grid.
    std::optional<size3_t> dimensionsOpt = vtkDataPtr->getDimensions();
    if (!dimensionsOpt) {
        LogWarn("Input vtk data set is not regular.");
        return;
    }
    size3_t dimensions = *dimensionsOpt;
    if (useCellData_.get())
        for (size_t d = 0; d < 3; ++d) dimensions[d]--;

    struct RawArrayData {
        unsigned char* ptr;
        size_t byteSize;
    };
    std::vector<RawArrayData> arrayDataPointers;

    // How many components in total?
    const DataFormatBase* format = DataFormatBase::get(dataFormats_.getSelectedValue());
    size_t sizeElement = format->getSize();
    size_t numTotalComps = 0;
    for (size_t p = 0; p < dataArrays_.size(); ++p) {
        BoolProperty* prop = dynamic_cast<BoolProperty*>(dataArrays_[p]);
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

    const DataFormatBase* multichannelFormat =
        DataFormatBase::get(format->getNumericType(), numTotalComps, format->getSize() * 8);

    // Extrapolate bounds from first cell.
    std::function<void(int, int, int, double[3])> getPointFromIndex;

    switch ((*vtkDataPtr)->GetDataObjectType()) {
        case VTK_RECTILINEAR_GRID:
            getPointFromIndex = [vtkDataPtr](int i, int j, int k, double pointOut[3]) {
                vtkRectilinearGrid::SafeDownCast(**vtkDataPtr)->GetPoint(i, j, k, pointOut);
            };
            break;
        case VTK_STRUCTURED_GRID:
            getPointFromIndex = [vtkDataPtr](int i, int j, int k, double pointOut[3]) {
                vtkStructuredGrid::SafeDownCast(**vtkDataPtr)->GetPoint(i, j, k, pointOut);
            };
            break;
        case VTK_IMAGE_DATA:
        case VTK_UNIFORM_GRID:
        case VTK_STRUCTURED_POINTS:
            getPointFromIndex = [vtkDataPtr](int i, int j, int k, double pointOut[3]) {
                vtkImageData* grid = vtkImageData::SafeDownCast(**vtkDataPtr);
                int idxs[3] = {i, j, k};
                grid->GetPoint(grid->ComputePointId(idxs), pointOut);
            };
            break;
        default:
            LogError("VTK data not regular.");
            return;
    }

    dvec3 p0, px, py, pz;
    getPointFromIndex(0, 0, 0, glm::value_ptr(p0));
    getPointFromIndex(1, 0, 0, glm::value_ptr(px));
    getPointFromIndex(0, 1, 0, glm::value_ptr(py));
    getPointFromIndex(0, 0, 1, glm::value_ptr(pz));

    // world offset vector on the grids x, y, z-axis
    const auto dx = px - p0;
    const auto dy = py - p0;
    const auto dz = pz - p0;

    dmat3 basis = {dx * static_cast<double>(dimensions.x), dy * static_cast<double>(dimensions.y),
                   dz * static_cast<double>(dimensions.z)};

    // p0 is the center of the voxel, offset needs another half a voxel.
    dvec3 offset = p0 - (dx + dy + dz) * 0.5;

    // Create new volume
    auto volume = std::make_shared<Volume>(dimensions, multichannelFormat);
    volume->setBasis(basis);
    volume->setOffset(offset);

    // Create RAM volume
    auto volRAM = createVolumeRAM(dimensions, multichannelFormat);
    unsigned char* dataPtr = static_cast<unsigned char*>(volRAM->getData());

    // Copy the data from the other volumes to the new multichannel volume
    size_t numElements = dimensions.x * dimensions.y * dimensions.z;
    for (size_t e = 0; e < numElements; e++) {
        for (auto& arrayData : arrayDataPointers) {
            memcpy(dataPtr, arrayData.ptr, arrayData.byteSize);
            dataPtr += arrayData.byteSize;
            arrayData.ptr += arrayData.byteSize;
        }
    }

    volume->addRepresentation(volRAM);

    auto extent = util::volumeMinMax(volume.get());
    double minVal = extent.first[0];
    double maxVal = extent.second[0];
    for (size_t d = 1; d < numTotalComps; ++d) {
        minVal = std::min(extent.first[d], minVal);
        maxVal = std::max(extent.second[d], minVal);
    }

    volume->dataMap_.valueRange = {minVal, maxVal};
    volume->dataMap_.dataRange = {dataRange_.get().x, dataRange_.get().y};
    dataRange_.setMinValue(dvec2(std::min(minVal, dataRange_.getMinValue()[0])));
    dataRange_.setMaxValue(dvec2(std::max(maxVal, dataRange_.getMaxValue()[0])));
    outport_.setData(volume);
}

}  // namespace inviwo
