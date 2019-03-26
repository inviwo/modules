/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2018 Inviwo Foundation
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

#include <modules/tensorvisio/processors/vtkxmlrectilineargridreader.h>

#include <inviwo/core/util/filesystem.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/util/clock.h>
#include <inviwo/core/util/exception.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkPointData.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLReader.h>
#include <vtkRectilinearGrid.h>
#include <vtkDataArray.h>
#include <vtkDataArrayAccessor.h>
#include <vtkArrayDispatch.h>
#include <warn/pop>

namespace inviwo {

// The Class Identifier has to be globally shared. Use a reverse DNS naming scheme
const ProcessorInfo VTKXMLRectilinearGridReader::processorInfo_{
    "org.inviwo.VTKXMLRectilinearGridReader",  // Class identifier
    "VTK XML Rectilinear Grid Reader",         // Display name
    "Data Input",                              // Category
    CodeState::Experimental,                   // Code state
    "VTK",                                     // Tags
};
const ProcessorInfo VTKXMLRectilinearGridReader::getProcessorInfo() const { return processorInfo_; }

VTKXMLRectilinearGridReader::VTKXMLRectilinearGridReader()
    : Processor()
    , file_("caseFile", "Case file", "", "ensight")
    , outport_("outport")
    , dataArrays_("arrays", "Arrays")
    , transposeTensor_("transposeTensor", "Transpose Tensor", false)
    , reloadButton_("reload", "Reload Data") {

    addPort(outport_);

    addProperty(file_);
    addProperty(dataArrays_);
    addProperty(transposeTensor_);
    addProperty(reloadButton_);

    file_.addNameFilter("VTK Rectilinear Grid File (*.vtr)");

    reloadButton_.onChange([this]() { reader_ = nullptr; });
}

void VTKXMLRectilinearGridReader::process() {
    if (!reader_ || file_.isModified()) {
        if (!filesystem::fileExists(file_)) {
            return;
        }

        getActivityIndicator().setActive(true);
        dispatchPool([this, filename = file_.get()]() {
            LogInfo("Loading VTK Rectilinear Grid: " << filename);

            IVW_CPU_PROFILING_IF(200, "Create Grid Reader");
            auto reader = vtkSmartPointer<vtkXMLRectilinearGridReader>::New();
            reader->SetFileName(filename.c_str());
            reader->Update();
            dispatchFront([this, reader]() {
                reader_ = reader;

                auto points = reader_->GetOutput()->GetPointData();
                std::vector<OptionPropertyIntOption> options;
                for (int i = 0; i < points->GetNumberOfArrays(); ++i) {
                    const auto name = std::string{points->GetArrayName(i)};
                    options.push_back({toLower(name), name, i});
                }
                NetworkLock lock(&dataArrays_);
                dataArrays_.replaceOptions(options);
                getActivityIndicator().setActive(false);
                invalidate(InvalidationLevel::InvalidOutput);
            });
        });
    }

    if (!reader_) return;

    if (!tensorfield_ || dataArrays_.isModified() || transposeTensor_.isModified()) {
        createTensorField();
    }

    if (!tensorfield_) return;

    outport_.setData(tensorfield_);
}

namespace detail {

// implementation of worker based on VTK example for vtkDataArrayAccessor
// \see https://vtk.org/Wiki/VTK/Tutorials/DataArrays
struct dmat3Convert {

    template <typename T>
    void operator()(T *array) {
        vtkDataArrayAccessor<T> access(array);

        tensors.reserve(static_cast<size_t>(array->GetNumberOfTuples()));
        const int numComps = std::max(array->GetNumberOfComponents(), 9);
        util::IndexMapper<2, int> indexMapper(ivec2{3});
        for (vtkIdType i = 0; i < array->GetNumberOfTuples(); ++i) {
            dmat3 tensor;
            for (int comp = 0; comp < numComps; ++comp) {
                const auto index = indexMapper(comp);
                const auto value = access.Get(i, comp);
                if (transpose) {
                    tensor[index.y][index.x] = static_cast<double>(value);
                } else {
                    tensor[index.x][index.y] = static_cast<double>(value);
                }
            }
            tensors.emplace_back(std::move(tensor));
        }
    }

    bool transpose = false;
    std::vector<dmat3> tensors;
};

}  // namespace detail

void VTKXMLRectilinearGridReader::createTensorField() {
    getActivityIndicator().setActive(true);
    dispatchPool([this, reader = reader_, arrayName = dataArrays_.getSelectedIdentifier()]() {
        IVW_CPU_PROFILING_IF(200, "Create Tensor Field");
        auto grid = reader->GetOutput();
        auto points = grid->GetPointData();

        const auto dims = glm::make_vec3(grid->GetDimensions());

        std::array<double, 6> bounds;
        grid->GetBounds(bounds.data());
        const dvec3 extent{bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]};

        bool found = false;
        for (int i = 0; i < points->GetNumberOfArrays() && !found; ++i) {
            found = arrayName == std::string{points->GetArrayName(i)};
        }
        if (!found) {
            throw Exception("VTK data does not contain an array '" + arrayName + "'", IVW_CONTEXT);
        }

        auto dataArray = points->GetArray(arrayName.c_str());
        const int numComponents = dataArray->GetNumberOfComponents();
        if (numComponents > 9) {
            LogWarn("Data array '" << arrayName << "' holds " << numComponents
                                   << " per tuple. Using only the first 9.");
        }

        detail::dmat3Convert converter;
        converter.transpose = transposeTensor_.get();
        if (!vtkArrayDispatch::Dispatch::Execute(dataArray, converter)) {
            converter(dataArray);  // dispatch failed, call fallback
        }

        auto tensorfield =
            std::make_shared<TensorField3D>(size3_t{dims}, std::move(converter.tensors), extent);
        tensorfield->setOffset({bounds[0], bounds[2], bounds[4]});

        dispatchFront([this, tensorfield]() {
            tensorfield_ = tensorfield;
            getActivityIndicator().setActive(false);
            invalidate(InvalidationLevel::InvalidOutput);
        });
    });
}

}  // namespace inviwo
