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
    : Processor{}
    , cacheDir_{"cacheDir", "Cache Dir",
                "Directory to save cached dataset too. "
                "You might want to manually clear it regularly to save space"_help,
                filesystem::getPath(PathType::Cache)}
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
    , outport_{"outport", outportType_.getSelectedValue(), "cached data"_help} {

    addPorts(inport_, outport_);
    addProperties(cacheDir_, outportType_);

    isReady_.setUpdate([this]() {
        return isCached_ ||
               (inport_.isConnected() && util::all_of(inport_.getConnectedOutports(),
                                                      [](Outport* p) { return p->isReady(); }));
    });

    outportType_.onChange([this]() { outport_.setTypeId(outportType_.getSelectedValue()); });

    app->getProcessorNetworkEvaluator()->addObserver(this);
}

void VTKFileCache::onProcessorNetworkEvaluationBegin() {
    if (isValid()) return;

    const auto key = detail::cacheState(this, *getNetwork(), xml_);

    const auto cf = cacheDir_.get() / fmt::format("{}.vtu", key);
    const auto isCached = std::filesystem::exists(cf);

    if (isCached_ != isCached) {
        isCached_ = isCached;
        isReady_.update();
        notifyObserversActiveConnectionsChange(this);
    }
    cache_ = Cache{std::move(key), std::move(cf)};
}

bool VTKFileCache::isConnectionActive(Inport* inport, Outport*) const {
    if (inport == &inport_) {
        return !isCached_;
    } else {
        return true;
    }
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
    if (cacheDir_.get().empty()) {
        throw Exception("No cache dir set");
    }

    if (cache_ && loadedKey_ == cache_->key) {
        return;
    } else if (cache_ && isCached_) {
        auto reader = vtkSmartPointer<vtkXMLGenericDataObjectReader>::New();
        reader->SetFileName(cache_->file.generic_string().c_str());
        reader->Update();               // Process the file
        castData(reader->GetOutput());  // Return as vtkDataSet
        loadedKey_ = cache_->key;
    } else if (cache_ && !isCached_) {
        if (auto data = inport_.getData()) {
            auto writer = vtkSmartPointer<vtkXMLDataSetWriter>::New();
            writer->SetFileName(cache_->file.generic_string().c_str());
            writer->SetInputData(data);
            if (writer->Write() != 1) {
                throw Exception(SourceContext{}, "Could not write to vtk file: {}/{}.vtu",
                                cacheDir_.get(), cache_->key);
            }

            if (auto f = std::ofstream(cacheDir_.get() / fmt::format("{}.inv", cache_->key))) {
                f << xml_;
            } else {
                throw Exception(SourceContext{}, "Could not write to xml file: {}/{}.inv",
                                cacheDir_.get(), cache_->key);
            }

            castData(data);
            loadedKey_ = cache_->key;
        } else {
            throw Exception("Port had no data");
        }
    } else {
        castData(inport_.getData());
    }
}

}  // namespace inviwo
