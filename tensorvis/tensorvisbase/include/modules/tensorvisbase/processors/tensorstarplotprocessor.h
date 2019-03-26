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

#ifndef IVW_TENSORSTARPLOTPROCESSOR_H
#define IVW_TENSORSTARPLOTPROCESSOR_H

#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/eventproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <modules/nanovgutils/nanovgutils.h>
#include <modules/tensorvisbase/ports/tensorfieldport.h>
#include <modules/tensorvisbase/properties/eigenvalueproperty.h>
#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>

namespace inviwo {

/** \docpage{org.inviwo.TensorStarPlotProcessor, Tensor Star Plot Processor}
 * ![](org.inviwo.TensorStarPlotProcessor.png?classIdentifier=org.inviwo.TensorStarPlotProcessor)
 * Explanation of how to use the processor.
 *
 * ### Inports
 *   * __<Inport1>__ <description>.
 *
 * ### Outports
 *   * __<Outport1>__ <description>.
 *
 * ### Properties
 *   * __<Prop1>__ <description>.
 *   * __<Prop2>__ <description>
 */

/**
 * \class TensorStarPlotProcessor
 * \brief VERY_BRIEFLY_DESCRIBE_THE_PROCESSOR
 * DESCRIBE_THE_PROCESSOR_FROM_A_DEVELOPER_PERSPECTIVE
 */
class IVW_MODULE_TENSORVISBASE_API TensorStarPlotProcessor : public Processor {
public:
    TensorStarPlotProcessor();
    virtual ~TensorStarPlotProcessor() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    TensorField2DInport tensorField2DInport_;
    TensorField3DInport tensorField3DInport_;
    DataInport<size_t, 0> offsetInport_;

    TensorField2DOutport tensorField2DOutport_;
    TensorField3DOutport tensorField3DOutport_;

    ImageOutport outport_;

    OptionPropertyInt input_;

    EigenValueProperty eigenvalues_;

    CompositeProperty plotProperties_;

    CompositeProperty starProperties_;
    BoolProperty showStar_;
    FloatVec4Property fillColor_;
    FloatVec4Property strokeColor_;
    DoubleProperty strokeThickness_;
    BoolProperty flipLambda2Axis_;

    CompositeProperty axisProperties_;
    BoolProperty showAxes_;
    DoubleProperty axisThickness_;
    FloatVec4Property axisColor_;
    TemplateOptionProperty<NanoVGContext::LineCapMode> lineCapMode_;
    BoolProperty showAllAxes_;

    CompositeProperty tickProperties_;
    BoolProperty showTicks_;
    OrdinalProperty<size_t> numTicks_;
    DoubleProperty tickLength_;
    DoubleProperty tickThickness_;
    FloatVec4Property tickColor_;

    CompositeProperty gridProperties_;
    OrdinalProperty<size_t> numTicksGrid_;
    BoolProperty showGrid_;
    DoubleProperty gridThickness_;
    FloatVec4Property gridColor_;
    BoolProperty fillGrid_;
    FloatVec4Property gridFillColor_;

    CompositeProperty labelProperties_;
    BoolProperty showLabels_;
    DoubleProperty fontSize_;
    FloatVec4Property fontColor_;
    DoubleProperty spacing_;

    CompositeProperty dotProperties_;
    BoolProperty showDots_;
    DoubleProperty dotRadius_;
    FloatVec4Property dotColor_;
    DoubleProperty dotStrokeWidth_;
    FloatVec4Property dotStrokeColor_;

    DoubleProperty margin_;
    BoolProperty rotate_;

    BoolProperty showControlPoints_;

    NanoVGContext* nvgContext_;

    EventProperty eventPorperty_;

    vec2 mousePos_;

    void drawAxes2D() const;
    void drawAxes3D() const;
    void drawAxesEigenValues() const;

    void drawGrid2D() const;
    void drawGrid3D() const;

    void drawTicks2D() const;
    void drawTicks3D() const;
    void drawTicksEigenValues() const;

    void drawDots2D() const;
    void drawDots3D(size_t offset) const;
    void drawDotsEigenValues() const;

    void drawStar2D() const;
    void drawStar3D(size_t offset) const;
    void drawStarEigenValues() const;

    void drawLabels2D() const;
    void drawLabels3D() const;
    void drawLabelsEigenValues() const;

    void drawAxis(const dvec2& from, const dvec2& to) const;
    void drawLabel(const dvec2& at, const std::string& label) const;
    void recalculateAxes();

    dvec2 axis1_3d_ = dvec2(0.0, -1.0);
    dvec2 axis2_3d_ = glm::normalize(glm::rotate(dvec2(0.0, -1.0), glm::radians(60.0)));
    dvec2 axis3_3d_ = glm::normalize(glm::rotate(dvec2(0.0, -1.0), glm::radians(120.0)));

    dvec2 axis1_2d_ = dvec2(0.0, -1.0);
    dvec2 axis2_2d_ = dvec2(1.0, 0.0);

    dvec2 tick1_3d_ = dvec2(1.0, 0.0);
    dvec2 tick2_3d_ = glm::normalize(glm::rotate(dvec2(0.0, -1.0), glm::radians(60.0 + 90.0)));
    dvec2 tick3_3d_ = glm::normalize(glm::rotate(dvec2(0.0, -1.0), glm::radians(120.0 + 90.0)));

    dvec2 tick1_2d_ = dvec2(1.0, 0.0);
    dvec2 tick2_2d_ = dvec2(0.0, -1.0);

    void calculateMaxEigenValue();

    double maxEigenValue_;
};

}  // namespace inviwo

#endif  // IVW_TENSORSTARPLOTPROCESSOR_H
