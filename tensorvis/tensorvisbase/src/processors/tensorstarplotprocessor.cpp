/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2017-2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 *FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#include <modules/nanovgutils/nanovgutilsmodule.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/tensorvisbase/processors/tensorstarplotprocessor.h>
#include <modules/vgfontrendering/vgfontrenderingmodule.h>
#include <inviwo/core/interaction/events/mouseevent.h>
#include <modules/opengl/buffer/renderbufferobject.h>
#include <modules/opengl/texture/texture2d.h>
#include <modules/opengl/buffer/framebufferobject.h>
#include <modules/tensorvisbase/util/misc.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming
// scheme
const ProcessorInfo TensorStarPlotProcessor::processorInfo_{
    "org.inviwo.TensorStarPlotProcessor",  // Class identifier
    "Tensor Star Plot Processor",          // Display name
    "Tensor Visualization",                // Category
    CodeState::Experimental,               // Code state
    "NanoVG",                              // Tags
};
const ProcessorInfo TensorStarPlotProcessor::getProcessorInfo() const { return processorInfo_; }

TensorStarPlotProcessor::TensorStarPlotProcessor()
    : Processor()
    , tensorField2DInport_("tensorField2DInport")
    , tensorField3DInport_("tensorField3DInport")
    , offsetInport_("offsetInport")
    , tensorField2DOutport_("tensorField2DOutport")
    , tensorField3DOutport_("tensorField3DOutport")
    , outport_("outport")
    , input_("input", "Input",
             {{"tf2d", "2D tensor", 0}, {"tf3d", "3D tensor", 1}, {"ev", "Eigenvalues", 2}})
    , plotProperties_("plotProperties", "Plot properties")
    , starProperties_("starProperties", "Star properties")
    , showStar_("showStar", "Show star", true)
    , fillColor_("fillColor", "Fill color", vec4(213.f, 73.f, 51.f, 150.f) / 255.f, vec4(0.f),
                 vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                 PropertySemantics::Color)
    , strokeColor_("strokeColor", "Stroke color", vec4(98.f, 98.f, 98.f, 255.f) / 255.f, vec4(0.f),
                   vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                   PropertySemantics::Color)
    , strokeThickness_("strokeThickness", "Stroke thickness", 2.0, 0.0, 20.0, 0.1)
    , flipLambda2Axis_("flipLambda2Axis", "Flip " + tensorutil::lamda2_str + " axis", true)
    , axisProperties_("axisProperties", "Axis properties")
    , showAxes_("showAxes", "Show axes", true)
    , axisThickness_("axisThickness", "Axis thickness", 3.0, 0.0, 20.0, 0.1)
    , axisColor_("axisColor_", "Axis color", vec4(61.f, 169.f, 171.f, 255.f) / 255.f, vec4(0.f),
                 vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                 PropertySemantics::Color)
    , lineCapMode_("lineCapMode", "Line cap mode",
                   {{"butt", "Butt", NanoVGContext::LineCapMode::Butt},
                    {"round", "Round", NanoVGContext::LineCapMode::Round},
                    {"square", "Square", NanoVGContext::LineCapMode::Square}})
    , showAllAxes_("showOnlyActiveAxes", "Only all axes", true)
    , tickProperties_("tickProperties", "Tick properties")
    , showTicks_("showTicks", "Show ticks", true)
    , numTicks_("numTicks", "Number of ticks", 20)
    , tickLength_("tickLength", "Tick length", 2.5, 0.0, 100.0, 0.1)
    , tickThickness_("tickThickness", "Tick thickness", 2.0, 0.0, 20.0, 0.1)
    , tickColor_("tickColor", "Tick color", vec4(43.f, 72.f, 200.f, 255.f) / 255.f, vec4(0.f),
                 vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                 PropertySemantics::Color)
    , gridProperties_("gridProperties", "Grid properties")
    , numTicksGrid_("numTicksGrid", "Number of ticks", 10)
    , showGrid_("showGrid", "Show grid", true)
    , gridThickness_("gridThickness", "Grid thickness", 1.0, 0.0, 20.0, 0.1)
    , gridColor_("gridColor_", "Grid color", vec4(136.f, 184.f, 216.f, 255.f) / 255.f, vec4(0.f),
                 vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                 PropertySemantics::Color)
    , fillGrid_("fillGrid", "Fill grid", true)
    , gridFillColor_("gridFillColor", "Grid fill color", vec4(80.f, 97.f, 255.f, 25.f) / 255.f,
                     vec4(0.f), vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                     PropertySemantics::Color)
    , labelProperties_("labelProperties", "Label properties")
    , showLabels_("showLabels", "Show labels", true)
    , fontSize_("fontSize", "Font size", 24.0, 0.0, 200.0, 0.1)
    , fontColor_("fontColor", "Font color", vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f),
                 vec4(0.001f), InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , spacing_("spacing", "Spacing", 20.0, 0.0, 200.0, 0.1)
    , dotProperties_("dotProperties", "Dot properties")
    , showDots_("showDots", "Show dots", true)
    , dotRadius_("dotRadius", "Dot radius", 7.2, 0.0, 50.0, 0.1)
    , dotColor_("dotColor", "Dot color", vec4(255.f, 155.f, 115.f, 255.f) / 255.f, vec4(0.f),
                vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , dotStrokeWidth_("dotStrokeWidth", "Dot stroke width", 1.3, 0.0, 20.0, 0.1)
    , dotStrokeColor_("dotstrokeColor", "Dot stroke color", vec4(95.f, 95.f, 95.f, 255.f) / 255.f,
                      vec4(0.f), vec4(1.f), vec4(0.001f), InvalidationLevel::InvalidOutput,
                      PropertySemantics::Color)
    , margin_("margin", "Margin", 40.0, 0.0, 500.0, 0.1)
    , rotate_("rotate_", "Rotate", false)
    , showControlPoints_("showControlPoints","Show control points",false)
    , eventPorperty_("position", "Position",
                     [this](Event* e) {
                         if (auto ev = e->getAs<MouseEvent>()) {
                             mousePos_ = ev->pos();
                             invalidate(InvalidationLevel::InvalidOutput);
                         }
                     },
                     MouseButton::None, MouseState::Move) {
    tensorField2DInport_.setOptional(true);
    tensorField3DInport_.setOptional(true);
    offsetInport_.setOptional(true);

    addPort(tensorField2DInport_);
    addPort(tensorField3DInport_);
    addPort(offsetInport_);

    addPort(tensorField2DOutport_);
    addPort(tensorField3DOutport_);

    addPort(outport_);

    lineCapMode_.setSelectedValue(NanoVGContext::LineCapMode::Round);

    addProperty(input_);

    addProperty(eigenvalues_);

    starProperties_.addProperty(showStar_);
    starProperties_.addProperty(fillColor_);
    starProperties_.addProperty(strokeColor_);
    starProperties_.addProperty(strokeThickness_);
    starProperties_.addProperty(flipLambda2Axis_);
    plotProperties_.addProperty(starProperties_);

    axisProperties_.addProperty(showAxes_);
    axisProperties_.addProperty(axisThickness_);
    axisProperties_.addProperty(axisColor_);
    axisProperties_.addProperty(lineCapMode_);
    axisProperties_.addProperty(showAllAxes_);
    plotProperties_.addProperty(axisProperties_);

    tickProperties_.addProperty(showTicks_);
    tickProperties_.addProperty(numTicks_);
    tickProperties_.addProperty(tickLength_);
    tickProperties_.addProperty(tickThickness_);
    tickProperties_.addProperty(tickColor_);
    plotProperties_.addProperty(tickProperties_);

    gridProperties_.addProperty(showGrid_);
    gridProperties_.addProperty(numTicksGrid_);
    gridProperties_.addProperty(gridThickness_);
    gridProperties_.addProperty(gridColor_);
    gridProperties_.addProperty(fillGrid_);
    gridProperties_.addProperty(gridFillColor_);
    plotProperties_.addProperty(gridProperties_);

    labelProperties_.addProperty(showLabels_);
    labelProperties_.addProperty(fontSize_);
    labelProperties_.addProperty(fontColor_);
    labelProperties_.addProperty(spacing_);
    plotProperties_.addProperty(labelProperties_);

    dotProperties_.addProperty(showDots_);
    dotProperties_.addProperty(dotRadius_);
    dotProperties_.addProperty(dotColor_);
    dotProperties_.addProperty(dotStrokeWidth_);
    dotProperties_.addProperty(dotStrokeColor_);
    plotProperties_.addProperty(dotProperties_);

    plotProperties_.addProperty(margin_);
    plotProperties_.addProperty(rotate_);

    addProperty(plotProperties_);

    addProperty(eventPorperty_);

    nvgContext_ =
        InviwoApplication::getPtr()->getModuleByType<NanoVGUtilsModule>()->getNanoVGContext();
    auto path = InviwoApplication::getPtr()->getModuleByType<VGFontRenderingModule>()->getPath() +
                "/fonts/";
    if (nvgContext_->createFont("fonty", path + "OpenSans-Regular.ttf") != -1) {
        LogInfo("Loaded " << path << "OpenSans-Regular.ttf");
    } else {
        LogError("Failed to load font.");
    }

    input_.onChange([this]() { eigenvalues_.setVisible(input_.get() == 2); });

    showAxes_.onChange([this]() {
        const auto showAxes = showAxes_.get();
        axisThickness_.setVisible(showAxes);
        axisColor_.setVisible(showAxes);
        lineCapMode_.setVisible(showAxes);
    });

    showTicks_.onChange([this]() {
        const auto showTicks = showTicks_.get();
        numTicks_.setVisible(showTicks);
        tickLength_.setVisible(showTicks);
        tickThickness_.setVisible(showTicks);
        tickColor_.setVisible(showTicks);
    });

    showDots_.onChange([this]() {
        const auto showDots = showDots_.get();
        dotRadius_.setVisible(showDots);
        dotColor_.setVisible(showDots);
        dotStrokeWidth_.setVisible(showDots);
        dotStrokeColor_.setVisible(showDots);
    });

    showGrid_.onChange([this]() {
        const auto showGrid = showGrid_.get();
        gridThickness_.setVisible(showGrid);
        gridColor_.setVisible(showGrid);
        fillGrid_.setVisible(showGrid);
        gridFillColor_.setVisible(fillGrid_.get());
    });

    fillGrid_.onChange([this]() { gridFillColor_.setVisible(fillGrid_.get()); });

    showLabels_.onChange([this]() {
        const auto showLabels = showLabels_.get();
        fontSize_.setVisible(showLabels);
        fontColor_.setVisible(showLabels);
        spacing_.setVisible(showLabels);
    });

    rotate_.onChange([this]() { recalculateAxes(); });
}

void TensorStarPlotProcessor::drawAxes2D() const {
    const auto tensorField = tensorField2DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];

    const auto showAll = showAllAxes_.get();
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    if (lambda1 >= 0 || showAll) drawAxis(center, center + (length * axis1_2d_));
    if (lambda2 >= 0 || showAll) drawAxis(center, center + (length * axis2_2d_));
    if (lambda1 < 0 || showAll) drawAxis(center, center - (length * axis1_2d_));
    if (lambda2 < 0 || showAll) drawAxis(center, center - (length * axis2_2d_));
}

void TensorStarPlotProcessor::drawAxes3D() const {
    const auto tensorField = tensorField3DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto lambda3 = eigenvalues[2];

    const auto showAll = showAllAxes_.get();
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    if (lambda1 >= 0 || showAll) drawAxis(center, center + (length * axis1_3d_));
    if (lambda2 >= 0 || showAll) drawAxis(center, center + (length * axis2_3d_));
    if (lambda3 >= 0 || showAll) drawAxis(center, center + (length * axis3_3d_));
    if (lambda1 < 0 || showAll) drawAxis(center, center - (length * axis1_3d_));
    if (lambda2 < 0 || showAll) drawAxis(center, center - (length * axis2_3d_));
    if (lambda3 < 0 || showAll) drawAxis(center, center - (length * axis3_3d_));
}

void TensorStarPlotProcessor::drawAxesEigenValues() const {
    const auto lambda1 = eigenvalues_.lambda1();
    const auto lambda2 = eigenvalues_.lambda2();
    const auto lambda3 = eigenvalues_.lambda3();
    const auto showAll = showAllAxes_.get();
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    if (lambda1 >= 0 || showAll) drawAxis(center, center + (length * axis1_3d_));
    if (lambda2 >= 0 || showAll) drawAxis(center, center + (length * axis2_3d_));
    if (lambda3 >= 0 || showAll) drawAxis(center, center + (length * axis3_3d_));
    if (lambda1 < 0 || showAll) drawAxis(center, center - (length * axis1_3d_));
    if (lambda2 < 0 || showAll) drawAxis(center, center - (length * axis2_3d_));
    if (lambda3 < 0 || showAll) drawAxis(center, center - (length * axis3_3d_));
}

void TensorStarPlotProcessor::drawGrid2D() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto length = center.y - margin_.get();

    const auto frac = length / static_cast<double>(numTicksGrid_.get());

    for (size_t i = numTicksGrid_.get(); i >= 1; i--) {
        nvgContext_->beginPath();
        nvgContext_->moveTo(center + (i * frac * axis1_2d_));
        nvgContext_->lineTo(center + (i * frac * axis2_2d_));
        nvgContext_->lineTo(center - (i * frac * axis1_2d_));
        nvgContext_->lineTo(center - (i * frac * axis2_2d_));
        nvgContext_->lineTo(center + (i * frac * axis1_2d_));

        nvgContext_->strokeColor(gridColor_.get());
        nvgContext_->strokeWidth(gridThickness_.get());

        nvgContext_->stroke();

        if (i == numTicksGrid_.get() && fillGrid_.get()) {
            nvgContext_->fillColor(gridFillColor_.get());
            nvgContext_->fill();
        }

        nvgContext_->closePath();
    }
}

void TensorStarPlotProcessor::drawGrid3D() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto length = center.y - margin_.get();

    const auto frac = length / static_cast<double>(numTicksGrid_.get());

    for (size_t i = numTicksGrid_.get(); i >= 1; i--) {
        nvgContext_->beginPath();
        nvgContext_->moveTo(center + (i * frac * axis1_3d_));
        nvgContext_->lineTo(center + (i * frac * axis2_3d_));
        nvgContext_->lineTo(center + (i * frac * axis3_3d_));
        nvgContext_->lineTo(center - (i * frac * axis1_3d_));
        nvgContext_->lineTo(center - (i * frac * axis2_3d_));
        nvgContext_->lineTo(center - (i * frac * axis3_3d_));
        nvgContext_->lineTo(center + (i * frac * axis1_3d_));

        nvgContext_->strokeColor(gridColor_.get());
        nvgContext_->strokeWidth(gridThickness_.get());

        if (i == numTicksGrid_.get() && fillGrid_.get()) {
            nvgContext_->fillColor(gridFillColor_.get());
            nvgContext_->fill();
        }

        nvgContext_->stroke();

        nvgContext_->closePath();
    }
}

void TensorStarPlotProcessor::drawTicks2D() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto length = center.y - margin_.get();

    const auto frac = length / static_cast<double>(numTicks_.get());

    auto drawTicksForAxis = [&](const dvec2& axis, const dvec2& tickDirection) {
        for (size_t i = numTicks_.get(); i >= 1; i--) {
            nvgContext_->beginPath();
            nvgContext_->moveTo(center + (i * frac * axis) + tickLength_.get() * tickDirection);
            nvgContext_->lineTo(center + (i * frac * axis) + tickLength_.get() * -tickDirection);

            nvgContext_->strokeColor(tickColor_.get());
            nvgContext_->strokeWidth(tickThickness_.get());

            nvgContext_->stroke();

            nvgContext_->closePath();
        }
    };

    const auto tensorField = tensorField2DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];

    const auto showAll = showAllAxes_.get();

    if (lambda1 >= 0 || showAll) drawTicksForAxis(axis1_3d_, tick1_2d_);
    if (lambda2 >= 0 || showAll) drawTicksForAxis(axis2_3d_, tick2_2d_);

    if (lambda1 < 0 || showAll) drawTicksForAxis(-axis1_3d_, tick1_2d_);
    if (lambda2 < 0 || showAll) drawTicksForAxis(-axis2_3d_, tick2_2d_);
}

void TensorStarPlotProcessor::drawTicks3D() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto length = center.y - margin_.get();

    const auto frac = length / static_cast<double>(numTicks_.get());

    auto drawTicksForAxis = [&](const dvec2& axis, const dvec2& tickDirection) {
        for (size_t i = numTicks_.get(); i >= 1; i--) {
            nvgContext_->beginPath();
            nvgContext_->moveTo(center + (i * frac * axis) + tickLength_.get() * tickDirection);
            nvgContext_->lineTo(center + (i * frac * axis) + tickLength_.get() * -tickDirection);

            nvgContext_->strokeColor(tickColor_.get());
            nvgContext_->strokeWidth(tickThickness_.get());

            nvgContext_->stroke();

            nvgContext_->closePath();
        }
    };

    const auto tensorField = tensorField3DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto lambda3 = eigenvalues[2];

    const auto showAll = showAllAxes_.get();

    if (lambda1 >= 0 || showAll) drawTicksForAxis(axis1_3d_, tick1_3d_);
    if (lambda2 >= 0 || showAll) drawTicksForAxis(axis2_3d_, tick2_3d_);
    if (lambda3 >= 0 || showAll) drawTicksForAxis(axis3_3d_, tick3_3d_);

    if (lambda1 < 0 || showAll) drawTicksForAxis(-axis1_3d_, tick1_3d_);
    if (lambda2 < 0 || showAll) drawTicksForAxis(-axis2_3d_, tick2_3d_);
    if (lambda3 < 0 || showAll) drawTicksForAxis(-axis3_3d_, tick3_3d_);
}

void TensorStarPlotProcessor::drawTicksEigenValues() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto length = center.y - margin_.get();

    const auto frac = length / static_cast<double>(numTicks_.get());

    auto drawTicksForAxis = [&](const dvec2& axis, const dvec2& tickDirection) {
        for (size_t i = numTicks_.get(); i >= 1; i--) {
            nvgContext_->beginPath();
            nvgContext_->moveTo(center + (i * frac * axis) + tickLength_.get() * tickDirection);
            nvgContext_->lineTo(center + (i * frac * axis) + tickLength_.get() * -tickDirection);

            nvgContext_->strokeColor(tickColor_.get());
            nvgContext_->strokeWidth(tickThickness_.get());

            nvgContext_->stroke();

            nvgContext_->closePath();
        }
    };

    const auto eigenvalues = eigenvalues_.get();
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto lambda3 = eigenvalues[2];

    const auto showAll = showAllAxes_.get();

    if (lambda1 >= 0 || showAll) drawTicksForAxis(axis1_3d_, tick1_3d_);
    if (lambda2 >= 0 || showAll) drawTicksForAxis(axis2_3d_, tick2_3d_);
    if (lambda3 >= 0 || showAll) drawTicksForAxis(axis3_3d_, tick3_3d_);

    if (lambda1 < 0 || showAll) drawTicksForAxis(-axis1_3d_, tick1_3d_);
    if (lambda2 < 0 || showAll) drawTicksForAxis(-axis2_3d_, tick2_3d_);
    if (lambda3 < 0 || showAll) drawTicksForAxis(-axis3_3d_, tick3_3d_);
}

void TensorStarPlotProcessor::drawDots2D() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto tensorField = tensorField2DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto max = std::max(std::abs(lambda1), std::abs(lambda2));

    const auto frac1 = lambda1 / max;
    const auto frac2 = lambda2 / max;

    const auto length = center.y - margin_.get();

    nvgContext_->beginPath();
    nvgContext_->circle(center + axis1_3d_ * frac1 * length, dotRadius_.get());
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();

    nvgContext_->beginPath();
    nvgContext_->circle(center + axis2_3d_ * frac2 * length, dotRadius_.get());
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();
}

void TensorStarPlotProcessor::drawDots3D(size_t offset) const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto tensorField = tensorField3DInport_.getData();

    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto lambda3 = eigenvalues[2];

    const auto frac1 = lambda1 / maxEigenValue_;
    const auto frac2 = lambda2 / maxEigenValue_;
    const auto frac3 = lambda3 / maxEigenValue_;

    const auto length = center.y - margin_.get();

    nvgContext_->beginPath();
    nvgContext_->circle(center + axis1_3d_ * frac1 * length, dotRadius_.get());
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();

    nvgContext_->beginPath();
    if (!flipLambda2Axis_.get())
        nvgContext_->circle(center + axis2_3d_ * frac2 * length, dotRadius_.get());
    else
        nvgContext_->circle(center - axis2_3d_ * frac2 * length, dotRadius_.get());
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();

    nvgContext_->beginPath();
    nvgContext_->circle(center + axis3_3d_ * frac3 * length, dotRadius_.get());
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();

    if(showControlPoints_.get()) {
        nvgContext_->beginPath();

        const auto p1 = center + axis1_3d_ * length * 0.7;
        const auto p2 = center - axis3_3d_ * length * 0.2;
        const auto p3 = center - axis2_3d_ * length * 0.4;
        const auto p4 = center - axis1_3d_ * length * 0.9;
        const auto p5 = center + axis3_3d_ * length * 0.6;
        const auto p6 = center + axis2_3d_ * length * 0.15;

        auto controlPoints =
                tensorutil::getCubicBezierCurveControlPoints({p5, p6, p1, p2, p3, p4, p5, p6, p1, p2, p3});
        const auto &firstControlPoints = controlPoints.first;
        const auto &secondControlPoints = controlPoints.second;

        nvgContext_->moveTo(p1);
        nvgContext_->bezierCurveTo(firstControlPoints[2], secondControlPoints[2], p2);
        nvgContext_->bezierCurveTo(firstControlPoints[3], secondControlPoints[3], p3);
        nvgContext_->bezierCurveTo(firstControlPoints[4], secondControlPoints[4], p4);
        nvgContext_->bezierCurveTo(firstControlPoints[5], secondControlPoints[5], p5);
        nvgContext_->bezierCurveTo(firstControlPoints[6], secondControlPoints[6], p6);
        nvgContext_->bezierCurveTo(firstControlPoints[7], secondControlPoints[7], p1);

        nvgContext_->closePath();
        nvgContext_->fillColor(vec4(0.8f, 0.3f, 0.0f, 0.7f));
        nvgContext_->fill();
        nvgContext_->strokeColor(vec4(0.8f, 0.3f, 0.0f, 1.0f));
        nvgContext_->strokeWidth(2.0f);
        nvgContext_->stroke();
    }
}

void TensorStarPlotProcessor::drawDotsEigenValues() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));

    const auto fivePercentOfTheRadius = dotRadius_.get() / 100.0f * 25.0f;

    const auto length = center.y - margin_.get();

    const auto lambda1 = eigenvalues_.lambda1();
    const auto lambda2 = eigenvalues_.lambda2();
    const auto lambda3 = eigenvalues_.lambda3();
    const auto max = std::max(std::abs(lambda1), std::max(std::abs(lambda2), std::abs(lambda3)));

    const auto frac1 = lambda1 / max;
    const auto frac2 = lambda2 / max;
    const auto frac3 = lambda3 / max;

    nvgContext_->beginPath();
    nvgContext_->circle(center + axis1_3d_ * frac1 * length, dotRadius_.get());
    nvgContext_->fillHitRegion(0);
    auto id = nvgContext_->hitTest(mousePos_.x, static_cast<float>(dimensions.y) - mousePos_.y,
                                   NanoVGContext::HitTestRegion::Fill);
    if (id == 0) {
        nvgContext_->restore();
        nvgContext_->beginPath();
        nvgContext_->circle(center + axis1_3d_ * frac1 * length,
                            dotRadius_.get() + fivePercentOfTheRadius);
    }

    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();

    nvgContext_->beginPath();
    if (!flipLambda2Axis_.get())
        nvgContext_->circle(center + axis2_3d_ * frac2 * length, dotRadius_.get());
    else
        nvgContext_->circle(center - axis2_3d_ * frac2 * length, dotRadius_.get());
    nvgContext_->fillHitRegion(1);
    id = nvgContext_->hitTest(mousePos_.x, static_cast<float>(dimensions.y) - mousePos_.y,
                              NanoVGContext::HitTestRegion::Fill);
    if (id == 1) {
        nvgContext_->restore();
        nvgContext_->beginPath();
        nvgContext_->circle(center - axis2_3d_ * frac2 * length,
                            dotRadius_.get() + fivePercentOfTheRadius);
    }
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();

    nvgContext_->beginPath();
    nvgContext_->circle(center + axis3_3d_ * frac3 * length, dotRadius_.get());
    nvgContext_->fillHitRegion(2);
    id = nvgContext_->hitTest(mousePos_.x, static_cast<float>(dimensions.y) - mousePos_.y,
                              NanoVGContext::HitTestRegion::Fill);
    if (id == 2) {
        nvgContext_->restore();
        nvgContext_->beginPath();
        nvgContext_->circle(center + axis3_3d_ * frac3 * length,
                            dotRadius_.get() + fivePercentOfTheRadius);
    }
    nvgContext_->strokeColor(dotStrokeColor_.get());
    nvgContext_->strokeWidth(dotStrokeWidth_.get());
    nvgContext_->fillColor(dotColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();
}

void TensorStarPlotProcessor::drawStar2D() const {}

void TensorStarPlotProcessor::drawStar3D(size_t offset) const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    const auto tensorField = tensorField3DInport_.getData();

    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto lambda3 = eigenvalues[2];

    const auto frac1 = lambda1 / maxEigenValue_;
    const auto frac2 = lambda2 / maxEigenValue_;
    const auto frac3 = lambda3 / maxEigenValue_;

    nvgContext_->beginPath();
    nvgContext_->moveTo(center + axis1_3d_ * length * frac1);
    if (!flipLambda2Axis_.get())
        nvgContext_->lineTo(center + axis2_3d_ * length * frac2);
    else
        nvgContext_->lineTo(center - axis2_3d_ * length * frac2);
    nvgContext_->lineTo(center + axis3_3d_ * length * frac3);
    nvgContext_->lineTo(center + axis1_3d_ * length * frac1);

    nvgContext_->strokeColor(strokeColor_.get());
    nvgContext_->strokeWidth(strokeThickness_.get());
    nvgContext_->fillColor(fillColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();
}

void TensorStarPlotProcessor::drawStarEigenValues() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    const auto lambda1 = eigenvalues_.lambda1();
    const auto lambda2 = eigenvalues_.lambda2();
    const auto lambda3 = eigenvalues_.lambda3();
    const auto max = std::max(std::abs(lambda1), std::max(std::abs(lambda2), std::abs(lambda3)));

    const auto frac1 = lambda1 / max;
    const auto frac2 = lambda2 / max;
    const auto frac3 = lambda3 / max;

    nvgContext_->beginPath();
    nvgContext_->moveTo(center + axis1_3d_ * length * frac1);
    if (!flipLambda2Axis_.get())
        nvgContext_->lineTo(center + axis2_3d_ * length * frac2);
    else
        nvgContext_->lineTo(center - axis2_3d_ * length * frac2);
    nvgContext_->lineTo(center + axis3_3d_ * length * frac3);
    nvgContext_->lineTo(center + axis1_3d_ * length * frac1);

    nvgContext_->strokeColor(strokeColor_.get());
    nvgContext_->strokeWidth(strokeThickness_.get());
    nvgContext_->fillColor(fillColor_.get());
    nvgContext_->fill();
    nvgContext_->stroke();
    nvgContext_->closePath();
}

void TensorStarPlotProcessor::drawLabels2D() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    drawLabel(center + (length * axis1_2d_) + (spacing_.get() * axis1_2d_), tensorutil::lamda1_str);
    drawLabel(center + (length * axis2_2d_) + (spacing_.get() * axis2_2d_), tensorutil::lamda2_str);
}

void TensorStarPlotProcessor::drawLabelsEigenValues() const {
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    const auto lambda1 = eigenvalues_.lambda1();
    const auto lambda2 = eigenvalues_.lambda2();
    const auto lambda3 = eigenvalues_.lambda3();
    const auto showAll = showAllAxes_.get();

    if (lambda1 >= 0 || showAll)
        drawLabel(center + (length * axis1_3d_) + (spacing_.get() * axis1_3d_),
                  tensorutil::lamda_str + "1");
    if (lambda2 >= 0 || showAll)
        drawLabel(center + (length * axis2_3d_) + (spacing_.get() * axis2_3d_),
                  (flipLambda2Axis_.get() ? "-" : "") + tensorutil::lamda_str + "2");
    if (lambda3 >= 0 || showAll)
        drawLabel(center + (length * axis3_3d_) + (spacing_.get() * axis3_3d_),
                  tensorutil::lamda_str + "3");
    if (lambda1 < 0 || showAll)
        drawLabel(center - ((length * axis1_3d_) + (spacing_.get() * axis1_3d_)),
                  "-" + tensorutil::lamda_str + "1");
    if (lambda2 < 0 || showAll)
        drawLabel(center - ((length * axis2_3d_) + (spacing_.get() * axis2_3d_)),
                  (flipLambda2Axis_.get() ? "" : "-") + tensorutil::lamda_str + "2");
    if (lambda3 < 0 || showAll)
        drawLabel(center - ((length * axis3_3d_) + (spacing_.get() * axis3_3d_)),
                  "-" + tensorutil::lamda_str + "3");
}

void TensorStarPlotProcessor::drawLabels3D() const {
    const auto tensorField = tensorField3DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);
    const auto lambda1 = eigenvalues[0];
    const auto lambda2 = eigenvalues[1];
    const auto lambda3 = eigenvalues[2];

    const auto showAll = showAllAxes_.get();
    const auto dimensions = outport_.getDimensions();
    const auto center = dvec2(dimensions / size2_t(2));
    const auto length = center.y - margin_.get();

    if (lambda1 >= 0 || showAll)
        drawLabel(center + (length * axis1_3d_) + (spacing_.get() * axis1_3d_),
                  tensorutil::lamda_str + "1");
    if (lambda2 >= 0 || showAll)
        drawLabel(center + (length * axis2_3d_) + (spacing_.get() * axis2_3d_),
                  (flipLambda2Axis_.get() ? "-" : "") + tensorutil::lamda_str + "2");
    if (lambda3 >= 0 || showAll)
        drawLabel(center + (length * axis3_3d_) + (spacing_.get() * axis3_3d_),
                  tensorutil::lamda_str + "3");
    if (lambda1 < 0 || showAll)
        drawLabel(center - ((length * axis1_3d_) + (spacing_.get() * axis1_3d_)),
                  "-" + tensorutil::lamda_str + "1");
    if (lambda2 < 0 || showAll)
        drawLabel(center - ((length * axis2_3d_) + (spacing_.get() * axis2_3d_)),
                  (flipLambda2Axis_.get() ? "" : "-") + tensorutil::lamda_str + "2");
    if (lambda3 < 0 || showAll)
        drawLabel(center - ((length * axis3_3d_) + (spacing_.get() * axis3_3d_)),
                  "-" + tensorutil::lamda_str + "3");
}

void TensorStarPlotProcessor::drawAxis(const dvec2& from, const dvec2& to) const {
    nvgContext_->beginPath();

    nvgContext_->moveTo(from);
    nvgContext_->lineTo(to);

    nvgContext_->strokeColor(axisColor_.get());
    nvgContext_->strokeWidth(axisThickness_.get());

    nvgContext_->stroke();

    nvgContext_->closePath();
}

void TensorStarPlotProcessor::drawLabel(const dvec2& at, const std::string& label) const {
    nvgContext_->beginPath();

    nvgContext_->fontSize(fontSize_.get());
    nvgContext_->fontFace("fonty");
    nvgContext_->textAlign(NanoVGContext::Alignment::Center_Middle);
    nvgContext_->fillColor(fontColor_.get());

    nvgContext_->text(at, label);

    nvgContext_->closePath();
}

void TensorStarPlotProcessor::recalculateAxes() {
    const auto angle2d = rotate_.get() ? -45.0 : 45.0;
    const auto angle3d = rotate_.get() ? -30.0 : 30.0;

    axis1_3d_ = glm::rotate(axis1_3d_, glm::radians(angle3d));
    axis2_3d_ = glm::rotate(axis2_3d_, glm::radians(angle3d));
    axis3_3d_ = glm::rotate(axis3_3d_, glm::radians(angle3d));

    axis1_2d_ = glm::rotate(axis1_2d_, glm::radians(angle2d));
    axis2_2d_ = glm::rotate(axis2_2d_, glm::radians(angle2d));

    tick1_3d_ = glm::rotate(tick1_3d_, glm::radians(angle3d));
    tick2_3d_ = glm::rotate(tick2_3d_, glm::radians(angle3d));
    tick3_3d_ = glm::rotate(tick3_3d_, glm::radians(angle3d));

    tick1_2d_ = glm::rotate(tick1_2d_, glm::radians(angle2d));
    tick2_2d_ = glm::rotate(tick2_2d_, glm::radians(angle2d));
}

void TensorStarPlotProcessor::calculateMaxEigenValue() {
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        maxEigenValue_ = std::numeric_limits<double>::lowest();

        const auto tensorField = tensorField3DInport_.getData();

        auto offsets = offsetInport_.getVectorData();

        for (auto offset : offsets) {
            const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(*offset);
            const auto lambda1 = eigenvalues[0];
            const auto lambda2 = eigenvalues[1];
            const auto lambda3 = eigenvalues[2];

            maxEigenValue_ = std::max(std::abs(lambda1), maxEigenValue_);
            maxEigenValue_ = std::max(std::abs(lambda2), maxEigenValue_);
            maxEigenValue_ = std::max(std::abs(lambda3), maxEigenValue_);
        }
    }
}

void TensorStarPlotProcessor::process() {
    auto dimensions = outport_.getDimensions();

    utilgl::activateAndClearTarget(outport_, ImageType::ColorOnly);

    RenderBufferObject stencilBufferObject;
    stencilBufferObject.activate();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, dimensions.x, dimensions.y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                              stencilBufferObject.getID());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              stencilBufferObject.getID());

    glClear(GL_STENCIL_BUFFER_BIT);

    nvgContext_->activate(dimensions);

    nvgContext_->lineCap(lineCapMode_.get());

    switch (input_.get()) {
        case 0:
            if (!tensorField2DInport_.isConnected() || !tensorField2DInport_.hasData()) {
                LogError("Selected input does not have data.");
                return;
            }

            if (showGrid_.get()) drawGrid2D();
            if (showAxes_.get()) drawAxes2D();
            if (showTicks_.get()) drawTicks2D();
            if (showStar_.get()) drawStar2D();
            if (showDots_.get()) drawDots2D();
            if (showLabels_.get()) drawLabels2D();
            break;
        case 1:
            if (!tensorField3DInport_.isConnected() || !tensorField3DInport_.hasData()) {
                LogError("Selected input does not have data.");
                return;
            }

            if (showGrid_.get()) drawGrid3D();
            if (showAxes_.get()) drawAxes3D();
            if (showTicks_.get()) drawTicks3D();

            if (!offsetInport_.isConnected() || !offsetInport_.hasData()) {
                const auto tensorField = tensorField3DInport_.getData();

                const auto eigenvalues = tensorField->getSortedEigenValuesForTensor(0);
                const auto lambda1 = eigenvalues[0];
                const auto lambda2 = eigenvalues[1];
                const auto lambda3 = eigenvalues[2];

                maxEigenValue_ = std::max(std::abs(lambda1), maxEigenValue_);
                maxEigenValue_ = std::max(std::abs(lambda2), maxEigenValue_);
                maxEigenValue_ = std::max(std::abs(lambda3), maxEigenValue_);

                if (showStar_.get()) drawStar3D(0);
                if (showDots_.get()) drawDots3D(0);
            } else {
                calculateMaxEigenValue();

                auto offsets = offsetInport_.getVectorData();

                for (auto offset : offsets) {
                    if (showStar_.get()) drawStar3D(*offset);
                    if (showDots_.get()) drawDots3D(*offset);
                }
            }

            if (showLabels_.get()) drawLabels3D();
            break;
        case 2:
            if (showGrid_.get()) drawGrid3D();
            if (showAxes_.get()) drawAxesEigenValues();
            if (showTicks_.get()) drawTicksEigenValues();
            if (showStar_.get()) drawStarEigenValues();
            if (showDots_.get()) drawDotsEigenValues();
            if (showLabels_.get()) drawLabelsEigenValues();
            break;
        default:
            break;
    }

    nvgContext_->deactivate();

    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
