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

#pragma once

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/properties/listproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/datastructures/buffer/buffer.h>
#include <inviwo/core/interaction/pickingmapper.h>
#include <inviwo/core/datastructures/camera/orthographiccamera.h>

#include <modules/basegl/rendering/linerenderer.h>
#include <modules/basegl/datastructures/linesettings.h>
#include <modules/basegl/properties/linesettingsproperty.h>

#include <modules/opengl/shader/shader.h>

#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/dataframe/properties/dataframeproperty.h>
#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

#include <modules/plotting/properties/marginproperty.h>
#include <modules/plotting/properties/axisproperty.h>
#include <modules/plotting/properties/axisstyleproperty.h>
#include <modules/plottinggl/utils/axisrenderer.h>

namespace inviwo {

class Mesh;

namespace plot {

class LineAxisProperty : public BoolCompositeProperty {
public:
    virtual std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

    LineAxisProperty(std::string identifier, std::string displayName, size_t firstColIndex = 0,
                     bool vertical = true,
                     InvalidationLevel invalidationLevel = InvalidationLevel::InvalidOutput,
                     PropertySemantics semantics = PropertySemantics::Default);

    LineAxisProperty(const LineAxisProperty& rhs);
    virtual LineAxisProperty* clone() const override;
    virtual ~LineAxisProperty() = default;

    DataFrameColumnProperty column_;
    FloatVec4Property color_;
    AxisProperty style_;

    AxisRenderer axisRenderer_;

private:
    auto props() { return std::tie(column_, color_, style_); }
    auto props() const { return std::tie(column_, color_, style_); }
};

/** \docpage{org.inviwo.LinePlot, Line Plot}
 * ![](org.inviwo.LinePlot.png?classIdentifier=org.inviwo.LinePlot)
 * This processor provides the functionality to generate a line plot from a DataFrame.
 *
 * ### Inports
 *   * __DataFrame__  data input for plotting
 *   * __BrushingAndLinking__   inport for brushing & linking interactions
 *
 * ### Outports
 *   * __outport__   rendered image of the scatter plot
 *
 */
class IVW_MODULE_VORTEXFEATURES_API LinePlot : public Processor, public PropertyOwnerObserver {
public:
    LinePlot();
    virtual ~LinePlot() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    // virtual void invokeEvent(Event* event) override;
private:
    void objectPicked(PickingEvent* p);

    virtual void onDidAddProperty(Property* property, size_t index) override;
    virtual void onWillRemoveProperty(Property* property, size_t index) override;

    void onAxisChange(LineAxisProperty* p);

    std::shared_ptr<Mesh> createLines();
    void drawDataPoints(ivec2 plotDims);

    DataFrameInport dataFramePort_;
    BrushingAndLinkingInport brushingPort_;
    ImageInport bgInport_;
    ImageOutport outport_;

    AxisStyleProperty axisStyle_;

    LineAxisProperty xAxis_;
    ListProperty yAxisList_;

    BoolCompositeProperty dataPoints_;
    FloatProperty pointRadius_;
    FloatProperty borderWidth_;
    FloatVec4Property borderColor_;
    BoolProperty useCircle_;

    FloatVec4Property hoverColor_;
    FloatVec4Property selectionColor_;
    MarginProperty margins_;
    FloatProperty axisMargin_;
    IntProperty axisSpacing_;

    BoolProperty hovering_;
    LineSettingsProperty lineSettings_;
    ButtonProperty syncColorsWithCaptions_;
    DataFrameColumnProperty splitColumn_;

    algorithm::LineRenderer lineRenderer_;
    Shader pointShader_;

    PickingMapper picking_;
    OrthographicCamera camera_;

    std::shared_ptr<Mesh> mesh_;

    bool meshDirty_ = false;
};

}  // namespace plot

}  // namespace inviwo
