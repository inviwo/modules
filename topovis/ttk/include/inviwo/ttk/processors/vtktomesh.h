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

#pragma once

#include <inviwo/ttk/ttkmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

#include <inviwo/ttk/ports/vtkinport.h>

#include <inviwo/core/ports/meshport.h>

#include <variant>
#include <array>

class vtkDataSet;

namespace inviwo {

class IVW_MODULE_TTK_API VTKToMesh : public Processor {
public:
    VTKToMesh();

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    enum class SourceType { Default, Point, CellData, PointData, Attribute, None };
    struct IVW_MODULE_TTK_API Source {
        SourceType type = SourceType::Default;
        int index = 0;

        void serialize(Serializer& s) const {
            s.serialize("type", type);
            s.serialize("index", index);
        }

        void deserialize(Deserializer& d) {
            d.deserialize("type", type);
            d.deserialize("index", index);
        }
        friend bool operator==(const Source& a, const Source& b) {
            return std::tie(a.type, a.index) == std::tie(b.type, b.index);
        }
        friend bool operator!=(const Source& a, const Source& b) {
            return std::tie(a.type, a.index) != std::tie(b.type, b.index);
        }
    };

    struct ScaleAndOffset {
        ScaleAndOffset();
        DoubleProperty scale;
        DoubleProperty offset;
    };

    struct OffsetAndPicking {
        OffsetAndPicking(Processor* p, size_t size, std::function<void(PickingEvent*)> callback);
        IntProperty offset;
        PickingMapper pickingMapper;
    };

    struct BufferInfo {
        using Transform = std::variant<OrdinalProperty<dmat4>, ScaleAndOffset, OffsetAndPicking,
                                       TransferFunctionProperty>;
        BufferInfo(BufferType type, Source defaultSource, Transform transform);
        BufferType type;
        BoolCompositeProperty comp;
        OptionProperty<Source> source;
        DoubleMinMaxProperty range;
        BoolCompositeProperty doTransform;
        Transform transform;
        Source defaultSource;
    };

private:
    void updateSources(vtkDataSet* array);

    void picking(PickingEvent* event);

    vtk::VtkInport inport;
    BrushingAndLinkingInport brushLinkPort;
    MeshOutport outport;

    std::array<BufferInfo, 9> infos;
};

template <>
struct OptionPropertyTraits<VTKToMesh::Source> {
    static const std::string& classIdentifier() {
        static const std::string identifier = "org.inviwo.OptionProperty.VTKtoMesh.Source";
        return identifier;
    }
};

}  // namespace inviwo
