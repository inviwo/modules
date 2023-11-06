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

#include <inviwo/core/io/serialization/serializer.h>
#include <inviwo/core/io/serialization/deserializer.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <inviwo/core/datastructures/geometry/geometrytype.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/core/datastructures/geometry/mesh.h>

#include <variant>
#include <functional>

#include <flags/allow_flags.h>
#include <flags/flags.h>

class vtkDataSet;

namespace inviwo {

class Processor;
class PickingEvent;
class Mesh;

namespace utilvtk {

enum class ArrayUsage {
    Position = 1 << 0,
    Normal = 1 << 1,
    Color = 1 << 2,
    TexCoord = 1 << 3,
    Curvature = 1 << 4,
    Index = 1 << 5,
    Radii = 1 << 6,
    Picking = 1 << 7,
    Scalar = 1 << 8,
    Unknown = 1 << 9
};

}

ALLOW_FLAGS_FOR_ENUM(utilvtk::ArrayUsage);

namespace utilvtk {

using ArrayUsageSelection = flags::flags<ArrayUsage>;

/**
 * This class manages the properties required for creating Buffers and IndexBuffers from a VTK
 * dataset. In particular, it allows to select specific data arrays to be used for vertex data,
 * normals, colors, etc. Data values can also be transformed.
 */
class IVW_MODULE_TTK_API ArrayBufferMapper {
public:
    struct BufferInfo;

    ArrayBufferMapper(
        Processor& processor, ArrayUsageSelection types = ArrayUsageSelection(flags::any),
        PickingAction::Callback callback = [](PickingEvent*) {});

    /**
     * Adjust the visibility of the composite properties associated with the given buffer types.
     */
    void setVisible(ArrayUsageSelection types, bool visible);

    /**
     * Query the property's visibility for buffer type \p type.
     */
    bool getVisible(BufferType type) const;

    /**
     * Query the isModified() status of all nested composite properties
     *
     * @return true if any of the composite info properties is modified
     */
    bool isModified() const;

    /**
     * Update the internal state to match the VTK data arrays of \p array and their names.
     *
     * @param array   VTK array used as source for generating mesh buffers and updating the internal
     * properties.
     * @see getBuffers
     */
    void updateSources(vtkDataSet* array);

    /**
     * Generate mesh buffers for the given VTK data set \p array based on the internal properties.
     *
     * @param array   VTK array used as data source
     * @see updateSources
     */
    Mesh::BufferVector getBuffers(vtkDataSet* array);

    /**
     * Return the buffer info for the given \p type.
     *
     * @return buffer info matching \p type, nullptr otherwise
     */
    BufferInfo* getBufferInfo(ArrayUsage type);
    const BufferInfo* getBufferInfo(ArrayUsage type) const;

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
        auto operator<=>(const Source&) const = default;
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

    struct IVW_MODULE_TTK_API BufferInfo {
        using Transform = std::variant<OrdinalProperty<dmat4>, ScaleAndOffset, OffsetAndPicking,
                                       TransferFunctionProperty>;
        BufferInfo(BufferType type, Source defaultSource, Transform transform);

        dvec2 getDataRange() const;

        BufferType type;
        BoolCompositeProperty comp;
        OptionProperty<Source> source;
        DoubleMinMaxProperty range;
        BoolCompositeProperty doTransform;
        Transform transform;
        Source defaultSource;
    };

private:
    std::array<BufferInfo, 9> infos;
};

}  // namespace utilvtk

template <>
struct OptionPropertyTraits<utilvtk::ArrayBufferMapper::Source> {
    static const std::string& classIdentifier() {
        static const std::string identifier = "org.inviwo.OptionProperty.vtk.Source";
        return identifier;
    }
};

}  // namespace inviwo
