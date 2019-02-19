#include <modules/tensorvisbase/processors/mohrcircleprocessor.h>
#include <modules/nanovgutils/nanovgutilsmodule.h>
#include <modules/opengl/texture/textureutils.h>
#include <modules/vgfontrendering/vgfontrenderingmodule.h>
#include <modules/tensorvisbase/util/tensorutil.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo MohrCircleProcessor::processorInfo_{
    "org.inviwo.MohrCircleProcessor",  // Class identifier
    "Mohr Circle Processor",           // Display name
    "Tensor Visualization",            // Category
    CodeState::Experimental,           // Code state
    "NanoVG",                        // Tags
};
const ProcessorInfo MohrCircleProcessor::getProcessorInfo() const { return processorInfo_; }

MohrCircleProcessor::MohrCircleProcessor()
    : Processor()
    , tensorField3DInport_("tensorField3DInport")
    , offsetInport_("offsetInport")
    , tensorField3DOutport_("tensorField3DOutport")
    , outport_("outport")
    , input_("input", "Input", {{"tf3d", "3D tensor", 1}, {"ev", "Eigenvalues", 2}})
    , majorPrincipalStressProperties_("majorPrincipalStressProperties",
                                      "Major Principal Stress Properties")
    , majorPrincipalStressFillColor_("majorPrincipalStressFillColor",
                                     "Major Principal Stress Fill Color", vec4(0.f, 0.f, 0.f, 1.f),
                                     vec4(0.f), vec4(1.f), vec4(0.001f),
                                     InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , majorPrincipalStressStrokeColor_("majorPrincipalStressStrokeColor",
                                       "Major Principal Stress Stroke Color",
                                       vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f), vec4(0.001f),
                                       InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , majorPrincipalStressStrokeWidth_("majorPrincipalStressStrokeWidth",
                                       "Major Principal Stress Stroke Width", 3.0, 0.0, 30.0, 0.1)
    , intermediatePrincipalStressProperties_("intermediatePrincipalStressProperties",
                                             "Intermediate Principal Stress Properties")
    , intermediatePrincipalStressFillColor_(
          "intermediatePrincipalStressFillColor", "Intermediate Principal Stress Fill Color",
          vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f), vec4(0.001f),
          InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , intermediatePrincipalStressStrokeColor_(
          "intermediatePrincipalStressStrokeColor", "Intermediate Principal Stress Stroke Color",
          vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f), vec4(0.001f),
          InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , intermediatePrincipalStressStrokeWidth_("intermediatePrincipalStressStrokeWidth",
                                              "Intermediate Principal Stress Stroke Width", 3.0,
                                              0.0, 30.0, 0.1)
    , minorPrincipalStressProperties_("minorPrincipalStressProperties",
                                      "Minor Principal Stress Properties")
    , minorPrincipalStressFillColor_("minorPrincipalStressFillColor",
                                     "Minor Principal Stress Fill Color", vec4(0.f, 0.f, 0.f, 1.f),
                                     vec4(0.f), vec4(1.f), vec4(0.001f),
                                     InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , minorPrincipalStressStrokeColor_("minorPrincipalStressStrokeColor",
                                       "Minor PrincipalStress Stroke Color",
                                       vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f), vec4(0.001f),
                                       InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , minorPrincipalStressStrokeWidth_("minorPrincipalStressStrokeWidth",
                                       "Minor Principal Stress Stroke Width", 3.0, 0.0, 30.0, 0.1)
    , coordinateSystemProperties_("coordinateSystemProperties", "Coordinate system properties")
    , showCoordinateSystem_("showCoordinateSystem", "Shwo coordinate system", true)
    , axisThickness_("axisThickness", "Axis thickness", 5.0, 0.0, 20.0, 0.1)
    , horizontalMargin_("horizontalMargin", "Horizontal margin", 20.0, 0.0, 100.0, 0.1)
    , verticalMargin_("verticalMargin", "Vertical margin", 20.0, 0.0, 100.0, 0.1)
    , yAxisPlacement_("yAxisPlacement", "Y axis placement", 20.f, 0.0, 100.0, 0.1)
    , axisColor_("axisColor", "Axis color", vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f),
                 vec4(0.001f), InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , arrowSize_("arrowSize", "Arrow size", 20.0, 0.0, 50.0, 0.1)
    , fontSize_("fontSize", "Font size", 24.0, 0, 128.0, 0.1)
    , fontColor_("fontColor", "Font color", vec4(0.f, 0.f, 0.f, 1.f), vec4(0.f), vec4(1.f),
                 vec4(0.001f), InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , lineCapMode_("lineCapMode", "Line cap mode",
                   {{"butt", "Butt", NanoVGContext::LineCapMode::Butt},
                    {"round", "Round", NanoVGContext::LineCapMode::Round},
                    {"square", "Square", NanoVGContext::LineCapMode::Square}})
    , margin_("margin", "Margin", 40.0, 0.0, 500.0, 0.1) {
    tensorField3DInport_.setOptional(true);
    offsetInport_.setOptional(true);
    addPort(tensorField3DInport_);
    addPort(offsetInport_);

    addPort(tensorField3DOutport_);

    addPort(outport_);

    addProperty(input_);

    addProperty(eigenvalues_);

    majorPrincipalStressProperties_.addProperty(majorPrincipalStressFillColor_);
    majorPrincipalStressProperties_.addProperty(majorPrincipalStressStrokeColor_);
    majorPrincipalStressProperties_.addProperty(majorPrincipalStressStrokeWidth_);
    addProperty(majorPrincipalStressProperties_);

    intermediatePrincipalStressProperties_.addProperty(intermediatePrincipalStressFillColor_);
    intermediatePrincipalStressProperties_.addProperty(intermediatePrincipalStressStrokeColor_);
    intermediatePrincipalStressProperties_.addProperty(intermediatePrincipalStressStrokeWidth_);
    addProperty(intermediatePrincipalStressProperties_);

    minorPrincipalStressProperties_.addProperty(minorPrincipalStressFillColor_);
    minorPrincipalStressProperties_.addProperty(minorPrincipalStressStrokeColor_);
    minorPrincipalStressProperties_.addProperty(minorPrincipalStressStrokeWidth_);
    addProperty(minorPrincipalStressProperties_);

    coordinateSystemProperties_.addProperty(showCoordinateSystem_);
    coordinateSystemProperties_.addProperty(axisThickness_);
    coordinateSystemProperties_.addProperty(horizontalMargin_);
    coordinateSystemProperties_.addProperty(verticalMargin_);
    coordinateSystemProperties_.addProperty(yAxisPlacement_);
    coordinateSystemProperties_.addProperty(axisColor_);
    coordinateSystemProperties_.addProperty(arrowSize_);
    coordinateSystemProperties_.addProperty(fontSize_);
    coordinateSystemProperties_.addProperty(fontColor_);
    coordinateSystemProperties_.addProperty(lineCapMode_);
    addProperty(coordinateSystemProperties_);

    addProperty(margin_);

    nvgContext_ =
        InviwoApplication::getPtr()->getModuleByType<NanoVGUtilsModule>()->getNanoVGContext();
    auto path = InviwoApplication::getPtr()->getModuleByType<VGFontRenderingModule>()->getPath() +
                "/fonts/";
    if (nvgContext_->createFont("fonty", path + "Roboto-Regular.ttf") != -1) {
        LogInfo("Loaded " << path << "Roboto-Regular.ttf");
    } else {
        LogError("Failed to load font.");
    }

    showCoordinateSystem_.onChange([this]() {
        const auto showCoordinateSystem = showCoordinateSystem_.get();
        axisThickness_.setVisible(showCoordinateSystem);
        horizontalMargin_.setVisible(showCoordinateSystem);
        verticalMargin_.setVisible(showCoordinateSystem);
        axisColor_.setVisible(showCoordinateSystem);
    });
}

void MohrCircleProcessor::draw3D() {
    const auto dimensions = outport_.getDimensions();

    const auto tensorField = tensorField3DInport_.getData();
    size_t offset = 0;
    if (offsetInport_.isConnected() && offsetInport_.hasData()) {
        offset = *offsetInport_.getData();
    }
    auto eigenvalues = tensorField->getSortedEigenValuesForTensor(offset);

    const auto width = static_cast<double>(std::max(dimensions.x, dimensions.y)) - margin_.get();
    const auto radiusC1 = width * 0.5;

    auto centerC1 =
        dvec2(static_cast<double>(dimensions.y) / 2.0, static_cast<double>(dimensions.y) / 2.0);
    auto radiusRef = (eigenvalues[0] - eigenvalues[2]) * 0.5;

    auto radiusC2 = (eigenvalues[0] - eigenvalues[1]) * 0.5;
    radiusC2 = (radiusC2 / radiusRef) * radiusC1;
    auto centerC2 =
        dvec2(centerC1.x + radiusC1 - radiusC2, static_cast<double>(dimensions.y) / 2.0);

    auto radiusC3 = (eigenvalues[1] - eigenvalues[2]) * 0.5;
    radiusC3 = (radiusC3 / radiusRef) * radiusC1;
    auto centerC3 =
        dvec2(centerC1.x - radiusC1 + radiusC3, static_cast<double>(dimensions.y) / 2.0);

    drawCircle(centerC1, radiusC1, majorPrincipalStressFillColor_.get(),
        majorPrincipalStressStrokeColor_.get(), majorPrincipalStressStrokeWidth_.get());

    drawCircle(centerC2, radiusC2, intermediatePrincipalStressFillColor_.get(),
        intermediatePrincipalStressStrokeColor_.get(),
        intermediatePrincipalStressStrokeWidth_.get());

    drawCircle(centerC3, radiusC3, minorPrincipalStressFillColor_.get(),
        minorPrincipalStressStrokeColor_.get(), minorPrincipalStressStrokeWidth_.get());
}

void MohrCircleProcessor::drawEigenvalues() const {
    const auto dimensions = outport_.getDimensions();

    auto eigenvalues = eigenvalues_.get();

    const auto width = static_cast<double>(std::max(dimensions.x, dimensions.y)) - margin_.get();
    const auto radiusC1 = width * 0.5;

    auto centerC1 =
        dvec2(static_cast<double>(dimensions.y) / 2.0, static_cast<double>(dimensions.y) / 2.0);
    auto radiusRef = (eigenvalues[0] - eigenvalues[2]) * 0.5;

    auto radiusC2 = (eigenvalues[0] - eigenvalues[1]) * 0.5;
    radiusC2 = (radiusC2 / radiusRef) * radiusC1;
    auto centerC2 =
        dvec2(centerC1.x + radiusC1 - radiusC2, static_cast<double>(dimensions.y) / 2.0);

    auto radiusC3 = (eigenvalues[1] - eigenvalues[2]) * 0.5;
    radiusC3 = (radiusC3 / radiusRef) * radiusC1;
    auto centerC3 =
        dvec2(centerC1.x - radiusC1 + radiusC3, static_cast<double>(dimensions.y) / 2.0);

    drawCircle(centerC1, radiusC1, majorPrincipalStressFillColor_.get(),
               majorPrincipalStressStrokeColor_.get(), majorPrincipalStressStrokeWidth_.get());

    drawCircle(centerC2, radiusC2, intermediatePrincipalStressFillColor_.get(),
               intermediatePrincipalStressStrokeColor_.get(),
               intermediatePrincipalStressStrokeWidth_.get());

    drawCircle(centerC3, radiusC3, minorPrincipalStressFillColor_.get(),
               minorPrincipalStressStrokeColor_.get(), minorPrincipalStressStrokeWidth_.get());
}

void MohrCircleProcessor::drawCircle(const dvec2& position, const double radius,
                                     const vec4& fillColor, const vec4& strokeColor,
                                     const double strokeWidth) const {
    nvgContext_->beginPath();
    nvgContext_->fillColor(fillColor);
    nvgContext_->strokeColor(strokeColor);
    nvgContext_->strokeWidth(strokeWidth);
    nvgContext_->circle(position, radius);

    nvgContext_->fill();
    nvgContext_->stroke();

    nvgContext_->closePath();
}

void MohrCircleProcessor::drawCoordinateSystem() const {
    const auto dimensions = outport_.getDimensions();
    const auto center =
        dvec2(static_cast<double>(dimensions.x) / 2.0, static_cast<double>(dimensions.y) / 2.0);
    const auto horizontalMargin = horizontalMargin_.get();
    const auto verticalMargin = verticalMargin_.get();

    const auto xCoordForYAxis = horizontalMargin + yAxisPlacement_.get();
    const auto fromYAxis = verticalMargin;
    const auto toYAxis = static_cast<double>(dimensions.y) - verticalMargin;

    nvgContext_->lineCap(lineCapMode_.get());

    nvgContext_->beginPath();
    nvgContext_->moveTo(dvec2(xCoordForYAxis, fromYAxis));
    nvgContext_->lineTo(dvec2(xCoordForYAxis, toYAxis));

    nvgContext_->strokeWidth(axisThickness_.get());
    nvgContext_->strokeColor(axisColor_.get());

    nvgContext_->stroke();

    nvgContext_->closePath();

    drawArrowHead(dvec2(xCoordForYAxis, fromYAxis), dvec2(0.0, -1.0), arrowSize_.get());

    const auto yCoordForXAxis = center.y;
    const auto fromXAxis = horizontalMargin;
    const auto toXAxis = static_cast<double>(dimensions.x) - horizontalMargin;

    nvgContext_->beginPath();
    nvgContext_->moveTo(dvec2(fromXAxis, yCoordForXAxis));
    nvgContext_->lineTo(dvec2(toXAxis, yCoordForXAxis));

    nvgContext_->strokeWidth(axisThickness_.get());
    nvgContext_->strokeColor(axisColor_.get());

    nvgContext_->stroke();

    nvgContext_->closePath();

    drawArrowHead(dvec2(toXAxis, yCoordForXAxis), dvec2(1.0, 0.0), arrowSize_.get());

    nvgContext_->beginPath();
    nvgContext_->fontSize(fontSize_.get());
    nvgContext_->fontFace("fonty");
    nvgContext_->textAlign(NanoVGContext::Alignment::Center_Bottom);
    nvgContext_->fillColor(fontColor_.get());
    nvgContext_->text(dvec2(toXAxis + arrowSize_.get(), yCoordForXAxis), tensorutil::sigma_str);
    nvgContext_->closePath();

    nvgContext_->beginPath();
    nvgContext_->fontSize(fontSize_.get());
    nvgContext_->fontFace("fonty");
    nvgContext_->textAlign(NanoVGContext::Alignment::Right_Middle);
    nvgContext_->fillColor(fontColor_.get());
    nvgContext_->text(dvec2(xCoordForYAxis, fromYAxis - arrowSize_.get() - 12.0),
                      tensorutil::tau_str);
    nvgContext_->closePath();
}

void MohrCircleProcessor::drawArrowHead(const dvec2& position, const dvec2& direction,
                                        double size) const {
    const auto orth = glm::normalize(glm::rotate(direction, glm::radians(90.0)));
    const auto width = (1.0 / 3.0) * size;
    const auto nudge = size / 7.0;
    const auto nudgePos = position + direction * nudge;

    nvgContext_->lineCap(lineCapMode_.get());

    nvgContext_->beginPath();
    nvgContext_->moveTo(nudgePos);
    nvgContext_->lineTo(position - orth * width);
    nvgContext_->lineTo(position + direction * size);
    nvgContext_->lineTo(position + orth * width);
    nvgContext_->lineTo(nudgePos);

    /*nvgContext_->fillColor(axisColor_.get());
    nvgContext_->fill();*/

    nvgContext_->strokeColor(axisColor_.get());
    nvgContext_->stroke();

    nvgContext_->closePath();
}

void MohrCircleProcessor::process() {
    auto dimensions = outport_.getDimensions();

    utilgl::activateAndClearTarget(outport_, ImageType::ColorDepth);

    nvgContext_->activate(dimensions);

    switch (input_.get()) {
        case 1:
            if (!tensorField3DInport_.isConnected() || !tensorField3DInport_.hasData()) {
                LogError("Selected input does not have data.");
                return;
            }
            draw3D();
            if (showCoordinateSystem_.get()) drawCoordinateSystem();
            break;
        case 2:
            drawEigenvalues();
            if (showCoordinateSystem_.get()) drawCoordinateSystem();
            break;
        default:
            break;
    }

    nvgContext_->deactivate();

    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
