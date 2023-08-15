/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023 Inviwo Foundation
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

#include <inviwo/ttk/ports/vtkoutport.h>

#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/safecstr.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/processors/processor.h>

#include <glm/vec3.hpp>

#include <vtkDataObject.h>
#include <vtkDataObjectTypes.h>

namespace inviwo::vtk {

VtkOutport::VtkOutport(std::string_view identifier, std::string_view vtkDataClassName,
                       Document help)
    : Outport(identifier, std::move(help)), typeId_{[&]() {
        auto id = vtkDataObjectTypes::GetTypeIdFromClassName(SafeCStr{vtkDataClassName}.c_str());
        if (id < 0) {
            throw Exception(IVW_CONTEXT, "Invalid port Data Class Name {}", vtkDataClassName);
        }
        return id;
    }()} {

    isReady_.setUpdate(
        [this]() { return invalidationLevel_ == InvalidationLevel::Valid && data_ != nullptr; });
}

VtkOutport::VtkOutport(std::string_view identifier, int typeId, Document help)
    : Outport(identifier, std::move(help)), typeId_{[&]() {
        if (typeId >= VTK_POLY_DATA && typeId <= VTK_IMAGE_STENCIL_DATA) {
            return typeId;
        } else {
            throw Exception(IVW_CONTEXT, "Invalid port Data TypeID {}", typeId);
        }
    }()} {

    isReady_.setUpdate(
        [this]() { return invalidationLevel_ == InvalidationLevel::Valid && data_ != nullptr; });
}

int VtkOutport::getTypeId() const { return typeId_; }

void VtkOutport::setTypeId(int typeId) {
    typeId_ = typeId;
    for (auto inport : connectedInports_) {
        if (!inport->canConnectTo(this)) {
            getProcessor()->getNetwork()->removeConnection(this, inport);
        }
    }
}

vtkDataObject* VtkOutport::getData() const { return data_; }

void VtkOutport::setData(vtkDataObject* data) {
    if (!data) {
        data_ = data;
        isReady_.update();
        return;
    }

    if (data->IsA(vtkDataObjectTypes::GetClassNameFromTypeId(typeId_))) {
        data_ = data;
        isReady_.update();
    } else {
        throw Exception(IVW_CONTEXT, "Invalid VTK data type for port {} expected {} got {}",
                        getPath(), vtkDataObjectTypes::GetClassNameFromTypeId(typeId_),
                        data->GetClassName());
    }
}

bool VtkOutport::hasData() const { return data_ != nullptr; }

void VtkOutport::clear() {
    data_ = nullptr;
    isReady_.update();
}

void VtkOutport::addTable(Document::DocumentHandle& h, std::string_view str) {

    auto r = h.append("table").append("tr");
    size_t curr = 0;
    size_t offset = 0;
    size_t count = 0;

    while (offset != std::string_view::npos) {
        while (offset != std::string_view::npos && count < 35) {
            offset = str.find('\n', offset + 1);
            ++count;
        }
        r.append("td").append("pre", str.substr(curr, offset - curr));
        curr = offset;
        count = 0;
    }
}

glm::uvec3 VtkOutport::getColorCode() const {
    // Todo use a table here or something...
    return glm::uvec3{102, 102, 153 + 5 * typeId_};
};
Document VtkOutport::getInfo() const {
    Document doc;
    using P = Document::PathComponent;
    using H = utildoc::TableBuilder::Header;
    auto b = doc.append("html").append("body");
    auto p = b.append("p");
    p.append("b", fmt::format("{} Outport", vtkDataObjectTypes::GetClassNameFromTypeId(typeId_)),
             {{"style", "color:white;"}});
    utildoc::TableBuilder tb(p, P::end());
    tb(H("Identifier"), getIdentifier());
    tb(H("Class"), getClassIdentifier());
    tb(H("Ready"), isReady());
    tb(H("Connected"), isConnected());
    tb(H("Connections"), connectedInports_.size());

    if (hasData()) {
        std::stringstream ss;
        data_->Print(ss);
        addTable(b, ss.str());
    } else {
        b.append("p", "Port has no data");
    }
    return doc;
}

}  // namespace inviwo::vtk
