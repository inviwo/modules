#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/imageport.h>

#include <inviwo/nanovgutils/nanovgutils.h>
#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <modules/tensorvisbase/properties/eigenvalueproperty.h>

namespace inviwo {

class IVW_MODULE_TENSORVISBASE_API MohrCircleProcessor : public Processor {
public:
    MohrCircleProcessor();
    virtual ~MohrCircleProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    TensorField3DInport tensorField3DInport_;
    DataInport<size_t> offsetInport_;

    TensorField3DOutport tensorField3DOutport_;

    ImageOutport outport_;

    OptionPropertyInt input_;

    EigenValueProperty eigenvalues_;

    CompositeProperty majorPrincipalStressProperties_;
    FloatVec4Property majorPrincipalStressFillColor_;
    FloatVec4Property majorPrincipalStressStrokeColor_;
    DoubleProperty majorPrincipalStressStrokeWidth_;

    CompositeProperty intermediatePrincipalStressProperties_;
    FloatVec4Property intermediatePrincipalStressFillColor_;
    FloatVec4Property intermediatePrincipalStressStrokeColor_;
    DoubleProperty intermediatePrincipalStressStrokeWidth_;

    CompositeProperty minorPrincipalStressProperties_;
    FloatVec4Property minorPrincipalStressFillColor_;
    FloatVec4Property minorPrincipalStressStrokeColor_;
    DoubleProperty minorPrincipalStressStrokeWidth_;

    CompositeProperty coordinateSystemProperties_;
    BoolProperty showCoordinateSystem_;
    DoubleProperty axisThickness_;
    DoubleProperty horizontalMargin_;
    DoubleProperty verticalMargin_;
    DoubleProperty yAxisPlacement_;
    FloatVec4Property axisColor_;
    DoubleProperty arrowSize_;
    DoubleProperty fontSize_;
    FloatVec4Property fontColor_;
    TemplateOptionProperty<NanoVGContext::LineCapMode> lineCapMode_;

    DoubleProperty margin_;

    NanoVGContext* nvgContext_;

    void draw3D();
    void drawEigenvalues() const;

    void drawCircle(const dvec2& position, const double radius, const vec4& fillColor,
                    const vec4& strokeColor, const double strokeWidth) const;
    void drawCoordinateSystem() const;
    void drawArrowHead(const dvec2& position, const dvec2& direction, double size) const;
};

}  // namespace inviwo
