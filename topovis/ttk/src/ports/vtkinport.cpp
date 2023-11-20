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

#include <inviwo/ttk/ports/vtkinport.h>

#include <inviwo/core/util/exception.h>
#include <inviwo/core/util/safecstr.h>
#include <inviwo/core/util/stdextensions.h>
#include <inviwo/core/network/processornetwork.h>
#include <inviwo/core/processors/processor.h>

#include <inviwo/ttk/ports/vtkoutport.h>

#include <string_view>

#include <glm/vec3.hpp>

#include <vtkDataObject.h>
#include <vtkDataObjectTypes.h>

namespace inviwo::vtk {

VtkInport::VtkInport(std::string_view identifier, std::string_view vtkDataClassName, Document help)
    : Inport(identifier, std::move(help)), typeId_{[&]() {
        auto id = vtkDataObjectTypes::GetTypeIdFromClassName(SafeCStr{vtkDataClassName}.c_str());
        if (id < 0) {
            throw Exception(IVW_CONTEXT, "Invalid port Data Class Name {}", vtkDataClassName);
        }
        return id;
    }()} {}

VtkInport::VtkInport(std::string_view identifier, int typeId, Document help)
    : Inport(identifier, std::move(help)), typeId_{[&]() {
        if (typeId >= VTK_POLY_DATA && typeId <= VTK_IMAGE_STENCIL_DATA) {
            return typeId;
        } else {
            throw Exception(IVW_CONTEXT, "Invalid port Data TypeID {}", typeId);
        }
    }()} {}

bool VtkInport::canConnectTo(const Port* port) const {
    if (!port || port->getProcessor() == getProcessor() || circularConnection(port)) return false;

    if (auto base = dynamic_cast<const VtkOutport*>(port)) {
        return vtkDataObjectTypes::TypeIdIsA(base->getTypeId(), typeId_);
    }
    return false;
}
size_t VtkInport::getMaxNumberOfConnections() const { return 1; }

vtkDataObject* VtkInport::getData() const {
    if (isConnected()) {
        return static_cast<VtkOutport*>(connectedOutports_[0])->getData();
    }
    return nullptr;
}

bool VtkInport::hasData() const {
    if (isConnected()) {
        return static_cast<VtkOutport*>(connectedOutports_[0])->hasData();
    }
    return false;
}

glm::uvec3 VtkInport::getColorCode() const {
    // Todo use a table here or something...
    return glm::uvec3{102, 102, 153 + 5 * typeId_};
}

Document VtkInport::getInfo() const {
    Document doc;
    using P = Document::PathComponent;
    using H = utildoc::TableBuilder::Header;
    auto b = doc.append("html").append("body");
    auto p = b.append("p");
    p.append("b", fmt::format("{} Inport", vtkDataObjectTypes::GetClassNameFromTypeId(typeId_)),
             {{"style", "color:white;"}});

    if (!help_.empty()) {
        p.append("div", "", {{"class", "help"}}).append(help_);
    }

    utildoc::TableBuilder tb(p, P::end());
    tb(H("Identifier"), getIdentifier());
    tb(H("Class"), getClassIdentifier());
    tb(H("Ready"), isReady());
    tb(H("Connected"), isConnected());

    tb(H("Connections"),
       fmt::format("{} ({})", getNumberOfConnections(), getMaxNumberOfConnections()));
    tb(H("Optional"), isOptional());

    if (hasData()) {
        std::stringstream ss;
        getData()->Print(ss);
        VtkOutport::addTable(b, ss.str());
    } else {
        b.append("p", "Port has no data");
    }
    return doc;
}

int VtkInport::getTypeId() const { return typeId_; }

void VtkInport::setTypeId(int typeId) {
    typeId_ = typeId;
    std::vector<Outport*> portsToRemove =
        util::copy_if(connectedOutports_, [&](auto port) { return !canConnectTo(port); });
    for (auto outport : portsToRemove) {
        getProcessor()->getNetwork()->removeConnection(outport, this);
    }
}

}  // namespace inviwo::vtk
