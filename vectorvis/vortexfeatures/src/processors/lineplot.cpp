/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/processors/lineplot.h>
#include <inviwo/core/util/foreacharg.h>
#include <inviwo/core/util/assertion.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/rendercontext.h>
#include <inviwo/core/network/networklock.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/datastructures/buffer/bufferram.h>
#include <modules/base/algorithm/dataminmax.h>
#include <inviwo/core/datastructures/geometry/mesh.h>
#include <inviwo/core/datastructures/geometry/typedmesh.h>

#include <modules/opengl/rendering/meshdrawergl.h>

#include <modules/opengl/openglutils.h>
#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/texture/textureutils.h>

#include <algorithm>

namespace inviwo {

namespace plot {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo LinePlot::processorInfo_{
    "org.inviwo.LinePlot",    // Class identifier
    "Line Plot",              // Display name
    "Plotting",               // Category
    CodeState::Experimental,  // Code state
    "GL, Plotting",           // Tags
};
const ProcessorInfo LinePlot::getProcessorInfo() const { return processorInfo_; }

const std::string LineAxisProperty::classIdentifier = "org.inviwo.LineAxisProperty";
std::string LineAxisProperty::getClassIdentifier() const { return classIdentifier; }

LineAxisProperty::LineAxisProperty(std::string identifier, std::string displayName,
                                   size_t firstColIndex, bool vertical,
                                   InvalidationLevel invalidationLevel, PropertySemantics semantics)
    : BoolCompositeProperty{identifier, displayName, true, invalidationLevel, semantics}
    , column_{"column", "Column", false, firstColIndex}
    , color_{"lineColor", "Line Color", util::ordinalColor(0.11f, 0.42f, 0.63f)}
    , style_{"axisStyle", "Style",
             vertical ? AxisProperty::Orientation::Vertical : AxisProperty::Orientation::Horizontal}
    , axisRenderer_{style_} {

    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());
    if (vertical) {
        style_.flipped_.set(true);
        style_.captionSettings_.setChecked(true);
        style_.setCaption(displayName);
    }
    style_.setCurrentStateAsDefault();

    column_.onChange([this]() { style_.setCaption(column_.getColumnHeader()); });
}

LineAxisProperty::LineAxisProperty(const LineAxisProperty& rhs)
    : BoolCompositeProperty{rhs}
    , column_{rhs.column_}
    , color_{rhs.color_}
    , style_{rhs.style_}
    , axisRenderer_{style_} {
    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());
    column_.onChange([this]() { style_.setCaption(column_.getColumnHeader()); });
}

auto LineAxisProperty::clone() const -> LineAxisProperty* {
    RenderContext::getPtr()->activateDefaultRenderContext();
    return new LineAxisProperty(*this);
}

LinePlot::LinePlot()
    : Processor{}
    , dataFramePort_{"dataFrame_"}
    , brushingPort_{"brushing"}
    , bgInport_{"bg"}
    , outport_{"outport"}

    , axisStyle_{"axisStyle", "Global Axis Style"}
    , xAxis_{"xAxis", "X-axis", 0, false}
    , yAxisList_{"yaxisList", "Y Axes",
                 std::make_unique<LineAxisProperty>("yaxis1", "Y Axis 1", 2, true)}

    , dataPoints_{"dataPoints", "Data Points", false}
    , pointRadius_{"pointRadius", "Radius", 5, 0, 10, 0.01f}
    , borderWidth_{"borderWidth", "Border Width", 2, 0, 20}
    , borderColor_{"borderColor", "Border Color", util::ordinalColor(0, 0, 0, 1)}
    , useCircle_("useCircle", "Use Circles (else squares)", true)

    , hoverColor_{"hoverColor", "Hover Color", util::ordinalColor(1.0f, 0.906f, 0.612f, 1)}
    , selectionColor_{"selectionColor", "Selection Color",
                      util::ordinalColor(1.0f, 0.769f, 0.247f, 1)}
    , margins_{"margins", "Margins", 10.0f, 10.0f, 30.0f, 100.0f}
    , axisMargin_{"axisMargin", "Axis Margin", 15.0f, 0.0f, 50.0f}
    , axisSpacing_{"axisSpacing", "Axis Spacing", 50, 0, 200}
    , hovering_{"hovering", "Enable Hovering", true}
    , lineSettings_{"lineSettings", "Line Settings"}
    , syncColorsWithCaptions_{"syncColorsWithCaptions", "Sync Captions with Line Colors",
                              [this]() {
                                  for (auto p : yAxisList_) {
                                      if (auto lineProp = dynamic_cast<LineAxisProperty*>(p)) {
                                          lineProp->style_.captionSettings_.color_.set(
                                              lineProp->color_.get());
                                      }
                                  }
                              }}
    , splitColumn_{"splitColumn", "Split Column", dataFramePort_, true}

    , lineRenderer_{&lineSettings_}
    , pointShader_{"lineplotdot.vert", "scatterplot.geom", "scatterplot.frag"}
    , picking_{this, 1, [this](PickingEvent* p) { objectPicked(p); }}
    , camera_{vec3{0.5f, 0.5f, 1.0f},
              vec3{0.5f, 0.5f, 0.0f},
              vec3{0.0f, 1.0f, 0.0f},
              0.1f,
              2.0f,
              1.0f} {

    addPort(dataFramePort_);
    addPort(brushingPort_);
    addPort(bgInport_);
    addPort(outport_);

    bgInport_.setOptional(true);

    dataPoints_.addProperties(pointRadius_, borderWidth_, borderColor_, useCircle_);
    dataPoints_.setCollapsed(true);
    dataPoints_.setCurrentStateAsDefault();

    lineSettings_.lineWidth_.set(2.0f);
    lineSettings_.pseudoLighting_.set(false);
    lineSettings_.roundDepthProfile_.set(false);
    lineSettings_.setCollapsed(true);
    lineSettings_.setCurrentStateAsDefault();

    addProperties(axisStyle_, xAxis_, yAxisList_, syncColorsWithCaptions_, dataPoints_, hoverColor_,
                  selectionColor_, margins_, axisMargin_, axisSpacing_, hovering_, lineSettings_,
                  splitColumn_);

    axisStyle_.registerProperty(xAxis_.style_);
    axisStyle_.setCollapsed(true);
    axisStyle_.labelFormat_.set("%g");
    axisStyle_.setCurrentStateAsDefault();

    xAxis_.color_.setVisible(false);
    yAxisList_.PropertyOwnerObservable::addObserver(this);

    dataFramePort_.onChange([this]() {
        xAxis_.column_.setOptions(dataFramePort_.getData());
        for (auto p : yAxisList_) {
            if (auto lineProp = dynamic_cast<LineAxisProperty*>(p)) {
                lineProp->column_.setOptions(dataFramePort_.getData());
            }
        }
    });
    xAxis_.onChange([this]() { onAxisChange(&xAxis_); });
    splitColumn_.onChange([this]() { meshDirty_ = true; });
    // add first y axis
    yAxisList_.constructProperty(0);

    pointShader_.onReload([this]() { invalidate(InvalidationLevel::InvalidOutput); });
}

void LinePlot::process() {
    utilgl::activateTargetAndClearOrCopySource(outport_, bgInport_);
    utilgl::DepthFuncState depthfunc(GL_ALWAYS);

    const auto dims = outport_.getDimensions();
    const ivec2 lowerLeft(margins_.getLeft(), margins_.getBottom());
    const ivec2 upperRight(dims.x - 1 - margins_.getRight(), dims.y - 1 - margins_.getTop());

    const int padding = static_cast<int>(axisMargin_.get());

    // draw horizontal axis
    xAxis_.axisRenderer_.render(dims, lowerLeft + ivec2(padding, 0),
                                ivec2(upperRight.x - padding, lowerLeft.y));
    // draw vertical axes
    int xOffset = 0;
    for (auto p : yAxisList_) {
        if (auto lineProp = dynamic_cast<LineAxisProperty*>(p); lineProp && lineProp->isChecked()) {
            lineProp->axisRenderer_.render(dims, lowerLeft + ivec2(xOffset, padding),
                                           ivec2(lowerLeft.x + xOffset, upperRight.y - padding));
            xOffset -= axisSpacing_;
        }
    }

    if (meshDirty_) {
        mesh_ = createLines();
        meshDirty_ = false;
    }

    if (mesh_) {
        // restrict line rendering to plotting area
        const ivec2 plotDims{upperRight.x - lowerLeft.x - 2 * padding,
                             upperRight.y - lowerLeft.y - 2 * padding};
        utilgl::ViewportState viewport(lowerLeft.x + padding, lowerLeft.y + padding, plotDims.x,
                                       plotDims.y);
        utilgl::BlendModeState blending(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        lineRenderer_.render(*mesh_.get(), camera_, plotDims, &lineSettings_);

        if (dataPoints_.isChecked()) {
            drawDataPoints(plotDims);
        }
    }

    utilgl::deactivateCurrentTarget();
}

void LinePlot::onDidAddProperty(Property* property, size_t) {
    NetworkLock lock(property);

    auto p = static_cast<LineAxisProperty*>(property);
    axisStyle_.registerProperty(p->style_);

    p->style_.setCaption(p->getDisplayName());
    p->column_.onChange([this, p]() { onAxisChange(p); });
    p->column_.setOptions(dataFramePort_.getData());

    p->getBoolProperty()->onChange([this]() { meshDirty_ = true; });
    p->color_.onChange([this]() { meshDirty_ = true; });
    p->style_.range_.onChange([this]() { meshDirty_ = true; });

    invalidate(InvalidationLevel::InvalidOutput);
}

void LinePlot::onWillRemoveProperty(Property* property, size_t) {
    axisStyle_.unregisterProperty(*static_cast<AxisProperty*>(property));
    invalidate(InvalidationLevel::InvalidOutput);
}

void LinePlot::objectPicked(PickingEvent*) {}

void LinePlot::onAxisChange(LineAxisProperty* p) {
    if (dataFramePort_.hasData()) {
        auto data = dataFramePort_.getData();

        auto col = data->getColumn(p->column_);
        IVW_ASSERT(col, "column not found after axis change");

        auto minmax = util::bufferMinMax(col->getBuffer().get(), IgnoreSpecialValues::Yes);
        p->style_.setRange(dvec2{minmax.first.x, minmax.second.x});

        meshDirty_ = true;
    }
}

std::shared_ptr<Mesh> LinePlot::createLines() {
    if (!dataFramePort_.hasData()) {
        return {};
    }

    using LineMesh = TypedMesh<buffertraits::PositionsBuffer1D, buffertraits::ColorsBuffer,
                               buffertraits::PickingBuffer>;

    //
    // TODO: reuse x axis values in separate vertex attribute and custom shader?
    //
    // TODO: filter points based on x axis
    //
    // TODO: sorting values or use sorting from DataFrame?
    //

    auto mesh = std::make_shared<Mesh>(DrawType::Lines, ConnectivityType::StripAdjacency);
    auto dataframe = dataFramePort_.getData();

    const auto lineRowIndices = [this]() -> std::vector<std::vector<size_t>> {
        if (splitColumn_.getSelectedValue() > -1) {
            const auto buffer =
                splitColumn_.getColumn()->getBuffer()->getRepresentation<BufferRAM>();
            return buffer->dispatch<std::vector<std::vector<size_t>>, dispatching::filter::Scalars>(
                [](auto buf) {
                    using ValueType = util::PrecisionValueType<decltype(buf)>;
                    std::vector<std::vector<size_t>> rows;
                    std::map<ValueType, size_t> valueIndex;
                    for (auto&& [row, v] : util::enumerate(buf->getDataContainer())) {
                        if (auto it = valueIndex.find(v); it != valueIndex.end()) {
                            rows[it->second].push_back(row);
                        } else {
                            valueIndex[v] = rows.size();
                            rows.emplace_back(1, row);
                        }
                    }
                    return rows;
                });
        } else {
            std::vector<size_t> rows(dataFramePort_.getData()->getNumberOfRows());
            std::iota(rows.begin(), rows.end(), 0u);
            return {std::move(rows)};
        }
    }();

    auto getValues = [](const LineAxisProperty& p) {
        const vec2 range{p.style_.getRange()};
        const auto buffer = p.column_.getColumn()->getBuffer()->getRepresentation<BufferRAM>();

        return buffer->dispatch<std::vector<float>, dispatching::filter::Scalars>(
            [range](auto buf) {
                auto& data = buf->getDataContainer();
                std::vector<float> dst(data.size(), 0.0f);
                std::transform(data.begin(), data.end(), dst.begin(), [range](auto v) {
                    return (static_cast<float>(v) - range.x) / (range.y - range.x);
                });
                return dst;
            });
    };

    auto xpos = getValues(xAxis_);

    const size_t numValues = xpos.size() * yAxisList_.size();

    std::vector<vec2> positions;
    positions.reserve(numValues);
    std::vector<vec4> colors;
    colors.reserve(numValues);
    // std::vector<uint32_t> pickingIds;

    for (const auto& rows : lineRowIndices) {
        for (const auto p : yAxisList_) {
            if (const auto lineProp = dynamic_cast<const LineAxisProperty*>(p);
                lineProp && lineProp->isChecked()) {
                const auto startIndex = positions.size();

                std::for_each(rows.begin(), rows.end(),
                              [&positions, xpos, ypos = getValues(*lineProp)](auto row) {
                                  positions.emplace_back(xpos[row], ypos[row]);
                              });

                const auto endIndex = positions.size();
                colors.resize(colors.size() + endIndex - startIndex, lineProp->color_);
                // pickingIds.resize(pickingIds.size() + endIndex - startIndex, id);

                std::vector<uint32_t> indices(endIndex - startIndex + 2);
                indices.front() = static_cast<uint32_t>(startIndex);
                std::iota(indices.begin() + 1, indices.end(), indices.front());
                indices.back() = indices.back() - 1;
                mesh->addIndices(Mesh::MeshInfo(DrawType::Lines, ConnectivityType::StripAdjacency),
                                 util::makeIndexBuffer(std::move(indices)));
            }
        }
    }

    mesh->addBuffer(BufferType::PositionAttrib, util::makeBuffer(std::move(positions)));
    mesh->addBuffer(BufferType::ColorAttrib, util::makeBuffer(std::move(colors)));

    return mesh;
}

void LinePlot::drawDataPoints(ivec2 plotDims) {
    pointShader_.activate();
    const vec2 pixelSize = vec2(1) / vec2(plotDims);
    pointShader_.setUniform("radius", pointRadius_.get());
    pointShader_.setUniform("borderWidth", borderWidth_.get());
    pointShader_.setUniform("borderColor", borderColor_.get());
    pointShader_.setUniform("pixelSize", pixelSize);
    pointShader_.setUniform("dims", ivec2(plotDims));
    pointShader_.setUniform("circle", useCircle_.get() ? 1 : 0);
    pointShader_.setUniform("pickingEnabled", picking_.isEnabled());

    MeshDrawerGL::DrawObject drawer(mesh_->getRepresentation<MeshGL>(),
                                    mesh_->getDefaultMeshInfo());
    utilgl::setShaderUniforms(pointShader_, *mesh_.get(), "geometry");

    drawer.draw(MeshDrawerGL::DrawMode::Points);

    pointShader_.deactivate();
}

}  // namespace plot

}  // namespace inviwo
