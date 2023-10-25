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

#include <inviwo/ttk/util/vtkbufferutils.h>

#include <inviwo/core/util/utilities.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/interaction/events/pickingevent.h>
#include <inviwo/ttk/arrayutils.h>

#include <fmt/format.h>

#include <vtkType.h>
#include <vtkCellType.h>
#include <vtkDataObject.h>
#include <vtkDataSet.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkAbstractArray.h>
#include <vtkArrayDispatch.h>
#include <vtkDataArrayMeta.h>

namespace inviwo::utilvtk {

namespace {

template <typename T = double, typename U = T>
OrdinalPropertyState<T> transformState(T val = T{1}) {
    return {val,
            util::filled<T>(static_cast<util::value_type_t<T>>(-100)),
            ConstraintBehavior::Ignore,
            util::filled<T>(static_cast<util::value_type_t<T>>(100)),
            ConstraintBehavior::Ignore,
            util::filled<T>(static_cast<util::value_type_t<T>>(0.1)),
            InvalidationLevel::InvalidOutput,
            PropertySemantics::Text};
}

ArrayUsage toArrayUsage(BufferType t) {
    switch (t) {
        case BufferType::PositionAttrib:
            return ArrayUsage::Position;
        case BufferType::NormalAttrib:
            return ArrayUsage::Normal;
        case BufferType::ColorAttrib:
            return ArrayUsage::Color;
        case BufferType::TexCoordAttrib:
            return ArrayUsage::TexCoord;
        case BufferType::CurvatureAttrib:
            return ArrayUsage::Curvature;
        case BufferType::IndexAttrib:
            return ArrayUsage::Index;
        case BufferType::RadiiAttrib:
            return ArrayUsage::Radii;
        case BufferType::PickingAttrib:
            return ArrayUsage::Picking;
        case BufferType::ScalarMetaAttrib:
            return ArrayUsage::Scalar;
        default:
            return ArrayUsage::Unknown;
    }
}

}  // namespace

ArrayBufferMapper::ArrayBufferMapper(Processor& processor, ArrayUsageSelection types,
                                     PickingAction::Callback callback)
    : infos{{{BufferType::PositionAttrib,
              {SourceType::Point, 1},
              OrdinalProperty<dmat4>("transform", "Transform", transformState<dmat4>())},
             {BufferType::NormalAttrib,
              {SourceType::Attribute, vtkDataSetAttributes::NORMALS},
              OrdinalProperty<dmat4>("transform", "Transform", transformState<dmat4>())},
             {BufferType::ColorAttrib,
              {SourceType::Attribute, vtkDataSetAttributes::SCALARS},
              TransferFunctionProperty("transform", "Transform")},
             {BufferType::TexCoordAttrib,
              {SourceType::Attribute, vtkDataSetAttributes::TCOORDS},
              OrdinalProperty<dmat4>("transform", "Transform", transformState<dmat4>())},
             {BufferType::CurvatureAttrib, {SourceType::None, 0}, ScaleAndOffset{}},
             {BufferType::IndexAttrib,
              {SourceType::Attribute, vtkDataSetAttributes::GLOBALIDS},
              ScaleAndOffset{}},
             {BufferType::RadiiAttrib, {SourceType::None, 0}, ScaleAndOffset{}},
             {BufferType::PickingAttrib,
              {SourceType::None, 0},
              OffsetAndPicking{&processor, 0, callback}},
             {BufferType::ScalarMetaAttrib, {SourceType::None, 0}, ScaleAndOffset{}}}} {

    updateSources(nullptr);

    for (auto& info : infos) {
        if (types.contains(toArrayUsage(info.type))) {
            processor.addProperty(info.comp);
        }
    }
}

void ArrayBufferMapper::setVisible(ArrayUsageSelection types, bool visible) {
    for (auto& info : infos) {
        if (types.contains(toArrayUsage(info.type))) {
            info.comp.setVisible(visible);
        }
    }
}

bool ArrayBufferMapper::getVisible(BufferType type) const {
    auto it = util::find_if(infos, [type](const auto& info) { return info.type == type; });
    if (it != infos.end()) {
        return it->comp.getVisible();
    } else {
        return false;
    }
}

bool ArrayBufferMapper::isModified() const {
    bool update = false;
    for (auto& info : infos) {
        update |= info.comp.isModified() && info.comp.getOwner();
    }
    return update;
}

void ArrayBufferMapper::updateSources(vtkDataSet* data) {
    std::vector<OptionPropertyOption<Source>> opts;

    opts.emplace_back("default", "Default", Source{SourceType::Default, 0});
    opts.emplace_back("points", "Points (3D Double)", Source{SourceType::Point, 0});
    opts.emplace_back("pointsFloat", "Points (3D Float)", Source{SourceType::Point, 1});

    if (data) {
        {
            auto arrays = data->GetPointData();
            const auto nArrays = arrays->GetNumberOfArrays();
            for (int i = 0; i < nArrays; ++i) {
                auto array = arrays->GetAbstractArray(i);
                opts.emplace_back(
                    util::stripIdentifier(array->GetName()),
                    fmt::format("PointData: {} ({}D {})", array->GetName(),
                                array->GetNumberOfComponents(), array->GetDataTypeAsString()),
                    Source{SourceType::PointData, i});
            }
        }

        {
            auto arrays = data->GetCellData();
            const auto nArrays = arrays->GetNumberOfArrays();
            for (int i = 0; i < nArrays; ++i) {
                auto array = arrays->GetAbstractArray(i);
                opts.emplace_back(
                    util::stripIdentifier(array->GetName()),
                    fmt::format("CellData: {}  ({}D {})", array->GetName(),
                                array->GetNumberOfComponents(), array->GetDataTypeAsString()),
                    Source{SourceType::CellData, i});
            }
        }
    }

    for (auto& info : infos) {
        info.source.replaceOptions(opts);
    }
}

template <typename Repeat>
std::shared_ptr<BufferBase> vtkArrayToBuffer(vtkDataArray* array,
                                             ArrayBufferMapper::BufferInfo::Transform* transform,
                                             Repeat repeat) {

    if (transform) {
        return std::visit(
            util::overloaded{
                [&](const OrdinalProperty<dmat4>& mat) {
                    auto trans = [&](auto val) -> decltype(val) {
                        using T = decltype(val);
                        const auto dval = static_cast<util::same_extent_t<T, double>>(val);
                        if constexpr (util::extent_v<T> == 1) {
                            auto res = mat.get() * dvec4{dval, 0.0, 0.0, 1.0};
                            return static_cast<T>(res[0]);
                        } else if constexpr (util::extent_v<T> == 2) {
                            auto res = mat.get() * dvec4{dval, 0.0, 1.0};
                            return static_cast<T>(res);
                        } else if constexpr (util::extent_v<T> == 3) {
                            auto res = mat.get() * dvec4{dval, 1.0};
                            return static_cast<T>(res);
                        } else if constexpr (util::extent_v<T> == 4) {
                            return static_cast<T>(mat.get() * dval);
                        } else {
                            throw Exception("no matching type");
                        }
                    };
                    return vtk::arrayToBuffer(array, trans, repeat);
                },
                [&](ArrayBufferMapper::OffsetAndPicking& so) {
                    dvec2 range;
                    array->GetRange(glm::value_ptr(range));

                    so.pickingMapper.resize(static_cast<size_t>(range[1] - range[0] + 1.0));

                    auto trans = [&](auto val) {
                        auto i =
                            util::glmcomp(val, 0) + so.offset.get() - static_cast<int>(range[0]);
                        i = std::clamp(i, decltype(i){0},
                                       static_cast<decltype(i)>(so.pickingMapper.getSize()));
                        return static_cast<unsigned int>(
                            so.pickingMapper.getPickingId(static_cast<size_t>(i)));
                    };
                    return vtk::arrayToBuffer(array, trans, repeat);
                },
                [&](const ArrayBufferMapper::ScaleAndOffset& so) {
                    auto trans = [&](auto val) {
                        using T = decltype(val);
                        const auto dval = static_cast<util::same_extent_t<T, double>>(val);
                        return static_cast<T>(so.scale.get() * (dval + so.offset.get()));
                    };
                    return vtk::arrayToBuffer(array, trans, repeat);
                },
                [&](const TransferFunctionProperty& tf) {
                    dvec2 range;
                    array->GetRange(glm::value_ptr(range));
                    auto trans = [&](auto val) {
                        const auto n = (util::glmcomp(val, 0) - range[0]) / (range[1] - range[0]);
                        return tf.get().sample(n);
                    };
                    return vtk::arrayToBuffer(array, trans, repeat);
                }},
            *transform);
    } else {
        auto identity = [](auto x) { return x; };
        return vtk::arrayToBuffer(array, identity, repeat);
    }
}

std::shared_ptr<BufferBase> vtkPointArrayToBuffer(
    vtkDataArray* array, ArrayBufferMapper::BufferInfo::Transform* transform) {
    auto repeat = [](int) { return 1; };
    return vtkArrayToBuffer(array, transform, repeat);
}

std::shared_ptr<BufferBase> vtkCellArrayToBuffer(
    vtkDataArray* array, vtkDataSet* dataSet, ArrayBufferMapper::BufferInfo::Transform* transform) {
    // We need to repeat each Cell value for each Point.
    auto repeat = [dataSet](int i) { return dataSet->GetCellSize(i); };
    return vtkArrayToBuffer(array, transform, repeat);
}

auto pointTransformer(const OrdinalProperty<dmat4>& mat) {
    return [m = mat.get()](auto val) {
        using T = decltype(val);
        return T{m * dvec4{val, 1.0}};
    };
}

Mesh::BufferVector ArrayBufferMapper::getBuffers(vtkDataSet* data) {
    if (!data) return {};

    Mesh::BufferVector buffers;
    for (auto& info : infos) {
        info.range.set(dvec2{0, 0});
        if (info.comp && info.source.size() > 0) {
            auto source = info.source.getSelectedValue();
            if (source.type == SourceType::Default) {
                source = info.defaultSource;
            }

            if (source.type == SourceType::PointData) {
                if (auto array = data->GetPointData()->GetArray(source.index)) {
                    dvec2 range;
                    array->GetRange(glm::value_ptr(range));
                    info.range.set(range);

                    buffers.emplace_back(
                        info.type,
                        vtkPointArrayToBuffer(
                            array, info.doTransform.isChecked() ? &info.transform : nullptr));
                }
            } else if (source.type == SourceType::CellData) {
                if (auto array = data->GetCellData()->GetArray(source.index)) {
                    dvec2 range;
                    array->GetRange(glm::value_ptr(range));
                    info.range.set(range);

                    buffers.emplace_back(
                        info.type,
                        vtkCellArrayToBuffer(
                            array, data, info.doTransform.isChecked() ? &info.transform : nullptr));
                }
            } else if (source.type == SourceType::Point) {
                dvec2 range{std::numeric_limits<double>::max(),
                            std::numeric_limits<double>::lowest()};
                if (source.index == 0) {
                    std::vector<dvec3> points(data->GetNumberOfPoints());
                    if (info.doTransform && info.transform.index() == 0) {
                        auto trans = pointTransformer(std::get<0>(info.transform));
                        for (vtkIdType i = 0; i < data->GetNumberOfPoints(); ++i) {
                            data->GetPoint(i, glm::value_ptr(points[i]));
                            range[0] = std::min(range[0], glm::compMin(points[i]));
                            range[1] = std::max(range[1], glm::compMax(points[i]));
                            points[i] = trans(points[i]);
                        }
                    } else {
                        for (vtkIdType i = 0; i < data->GetNumberOfPoints(); ++i) {
                            data->GetPoint(i, glm::value_ptr(points[i]));
                            range[0] = std::min(range[0], glm::compMin(points[i]));
                            range[1] = std::max(range[1], glm::compMax(points[i]));
                        }
                    }
                    buffers.emplace_back(info.type, util::makeBuffer(std::move(points)));
                } else {
                    std::vector<vec3> points;
                    points.reserve(data->GetNumberOfPoints());
                    if (info.doTransform && info.transform.index() == 0) {
                        auto trans = pointTransformer(std::get<0>(info.transform));
                        for (vtkIdType i = 0; i < data->GetNumberOfPoints(); ++i) {
                            dvec3 point;
                            data->GetPoint(i, glm::value_ptr(point));
                            range[0] = std::min(range[0], glm::compMin(point));
                            range[1] = std::max(range[1], glm::compMax(point));
                            point = trans(point);
                            points.push_back(static_cast<vec3>(point));
                        }

                    } else {
                        for (vtkIdType i = 0; i < data->GetNumberOfPoints(); ++i) {
                            dvec3 point;
                            data->GetPoint(i, glm::value_ptr(point));
                            range[0] = std::min(range[0], glm::compMin(point));
                            range[1] = std::max(range[1], glm::compMax(point));
                            points.push_back(static_cast<vec3>(point));
                        }
                    }

                    buffers.emplace_back(info.type, util::makeBuffer(std::move(points)));
                }
                info.range.set(range);
            }
        }
    }
    return buffers;
}

auto ArrayBufferMapper::getBufferInfo(ArrayUsage type) const -> const BufferInfo* {
    auto it = util::find_if(infos, [type](auto& info) { return toArrayUsage(info.type) == type; });
    if (it != infos.end()) {
        return &(*it);
    } else {
        return nullptr;
    }
}

auto ArrayBufferMapper::getBufferInfo(ArrayUsage type) -> BufferInfo* {
    auto it = util::find_if(infos, [type](auto& info) { return toArrayUsage(info.type) == type; });
    if (it != infos.end()) {
        return &(*it);
    } else {
        return nullptr;
    }
}

ArrayBufferMapper::BufferInfo::BufferInfo(BufferType type, Source defaultSource,
                                          Transform aTransform)
    : type{type}
    , comp{toLower(enumToStr(type)), enumToStr(type)}
    , source{"source", "Source"}
    , range("range", "Range", "Range of input data, before any transforms"_help, 0, 0,
            std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max(), 1, 0,
            InvalidationLevel::Valid, PropertySemantics::Text)
    , doTransform("doTransform", "Apply transformation", false)
    , transform{std::move(aTransform)}
    , defaultSource{defaultSource} {
    comp.addProperties(source, range, doTransform);
    range.setReadOnly(true);
    std::visit(
        util::overloaded{[&](Property& item) { doTransform.addProperty(item); },
                         [&](ScaleAndOffset& item) {
                             doTransform.addProperty(item.offset);
                             doTransform.addProperty(item.scale);
                         },
                         [&](OffsetAndPicking& item) { doTransform.addProperty(item.offset); }},
        transform);
}

dvec2 ArrayBufferMapper::BufferInfo::getDataRange() const {
    if (doTransform.isChecked()) {
        return std::visit(util::overloaded{[&](const Property&) { return range.get(); },
                                           [&](const ScaleAndOffset& item) {
                                               return (range.get() + dvec2{item.offset}) *
                                                      dvec2{item.scale};
                                           },
                                           [&](const OffsetAndPicking& item) {
                                               return range.get() + dvec2(item.offset.get());
                                           }},
                          transform);
    } else {
        return range.get();
    }
}

ArrayBufferMapper::ScaleAndOffset::ScaleAndOffset()
    : scale{"scale", "Scale", transformState<double>(1.0)}
    , offset{"offset", "Offset", transformState<double>(0.0)} {}

ArrayBufferMapper::OffsetAndPicking::OffsetAndPicking(Processor* p, size_t size,
                                                      std::function<void(PickingEvent*)> callback)
    : offset{"offset", "Offset", transformState<int>(0)}
    , pickingMapper(p, size, std::move(callback)) {}

}  // namespace inviwo::utilvtk
