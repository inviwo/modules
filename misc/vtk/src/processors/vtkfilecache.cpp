/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2025 Inviwo Foundation
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

#include <inviwo/vtk/processors/vtkfilecache.h>

#include <modules/base/processors/filecache.h>

#include <vtkSmartPointer.h>
#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkXMLDataSetWriter.h>
#include <vtkXMLGenericDataObjectReader.h>
#include <vtkType.h>
#include <vtkDataObjectTypes.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VTKFileCache::processorInfo_{
    "org.inviwo.VTKFileCache",  // Class identifier
    "VTK File Cache",           // Display name
    "Undefined",                // Category
    CodeState::Stable,          // Code state
    Tags::CPU,                  // Tags
    R"(Provides a file cache for vtk data)"_unindentHelp,
};

const ProcessorInfo& VTKFileCache::getProcessorInfo() const { return processorInfo_; }

VTKFileCache::VTKFileCache(InviwoApplication* app)
    : CacheBase{app}
    , outportType_{"outportType", "Derived Type",
                   []() {
                       OptionPropertyState<int> state;
                       for (int typeId = VTK_POLY_DATA; typeId <= VTK_IMAGE_STENCIL_DATA;
                            ++typeId) {

                           if (vtkDataObjectTypes::TypeIdIsA(typeId, VTK_DATA_OBJECT)) {
                               if (typeId == VTK_IMAGE_DATA) {
                                   state.selectedIndex = state.options.size();
                               }

                               auto name = vtkDataObjectTypes::GetClassNameFromTypeId(typeId);
                               state.options.emplace_back(name, name, typeId);
                           }
                       }
                       return state;
                   }()

      }
    , inport_{"inport", VTK_DATA_OBJECT, "data to cache"_help}
    , outport_{"outport", outportType_.getSelectedValue(), "cached data"_help}
    , ram_{} {

    addPorts(inport_, outport_);
    addProperties(enabled_, cacheDir_, refDir_, currentKey_, outportType_, ram_.capacity);

    outportType_.onChange([this]() { outport_.setTypeId(outportType_.getSelectedValue()); });
}

std::optional<std::filesystem::path> VTKFileCache::pathForKey(std::string_view key) {
    if (cacheDir_.get().empty()) return std::nullopt;
    return cacheDir_.get() / fmt::format("{}.vtu", key);
}

bool VTKFileCache::hasCache(std::string_view key) {
    return ram_.has(key) || pathForKey(key)
                                .transform([](const std::filesystem::path& path) -> bool {
                                    return std::filesystem::exists(path);
                                })
                                .value_or(false);
}

void VTKFileCache::castData(vtkDataObject* data) {
    if (data) {
        if (!data->IsA(vtkDataObjectTypes::GetClassNameFromTypeId(outport_.getTypeId()))) {
            outport_.setData(nullptr);
            throw Exception(IVW_CONTEXT, "Data is not of type '{}' got '{}'",
                            vtkDataObjectTypes::GetClassNameFromTypeId(outport_.getTypeId()),
                            data->GetClassName());
        }
        outport_.setData(data);
    }
}

void VTKFileCache::process() {
    if (loadedKey_ == key_) return;

    if (isCached_) {
        if (auto ramData = ram_.get(key_)) {
            castData(ramData->Get());
            loadedKey_ = key_;
        } else if (auto maybePath = pathForKey(key_)) {
            auto reader = vtkSmartPointer<vtkXMLGenericDataObjectReader>::New();
            reader->SetFileName(maybePath->generic_string().c_str());
            reader->Update();  // Process the file
            auto diskData = reader->GetOutput();
            castData(diskData);  // Return as vtkDataSet
            ram_.add(key_, vtkSmartPointer<vtkDataObject>(diskData));
            loadedKey_ = key_;
        } else {
            throw Exception("No file found");
        }
    } else if (auto data = inport_.getData()) {
        if (auto maybePath = pathForKey(key_)) {
            auto writer = vtkSmartPointer<vtkXMLDataSetWriter>::New();
            writer->SetFileName(maybePath->generic_string().c_str());
            writer->SetInputData(data);
            if (writer->Write() != 1) {
                throw Exception(SourceContext{}, "Could not write to vtk file: {:?g}", *maybePath);
            }
        }
        writeXML();

        // We need to clone the data here, since the filter above might change the data later.
        vtkSmartPointer<vtkDataObject> clonedData = vtkSmartPointer<vtkDataObject>::New();
        clonedData->DeepCopy(data);
        ram_.add(key_, clonedData);
        castData(data);
        loadedKey_ = key_;
    } else {
        throw Exception("Port had no data");
    }
}

}  // namespace inviwo
