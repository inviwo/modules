/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2023-2024 Inviwo Foundation
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

#include <inviwo/ttk/ttkmoduledefine.h>

#include <inviwo/core/ports/inport.h>
#include <inviwo/core/ports/porttraits.h>

#include <string_view>
#include <string>

#include <vtkType.h>

class vtkDataObject;

namespace inviwo {

namespace vtk {

class IVW_MODULE_TTK_API VtkInport : public Inport {
public:
    VtkInport(std::string_view identifier, int typeId = VTK_DATA_OBJECT,
              Document help = Document{});
    VtkInport(std::string_view identifier, std::string_view vtkDataClassName,
              Document help = Document{});

    using type = vtkDataObject;

    virtual bool canConnectTo(const Port* port) const override;
    virtual size_t getMaxNumberOfConnections() const override;
    virtual std::string getClassIdentifier() const override;
    virtual glm::uvec3 getColorCode() const override;
    virtual Document getInfo() const override;

    vtkDataObject* getData() const;
    bool hasData() const;
    int getTypeId() const;
    void setTypeId(int typeId);

private:
    int typeId_;
};

}  // namespace vtk

template <>
struct PortTraits<vtk::VtkInport> {
    static const std::string& classIdentifier() {
        static std::string id{"org.inviwo.vtk.inport"};
        return id;
    }
};

inline std::string vtk::VtkInport::getClassIdentifier() const {
    return PortTraits<vtk::VtkInport>::classIdentifier();
}

}  // namespace inviwo
