/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/devtools/devtoolsmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/minmaxproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/imageport.h>

#include <inviwo/core/datastructures/geometry/typedmesh.h>

#include <modules/plottinggl/utils/axisrenderer.h>
#include <modules/opengl/shader/shader.h>
#include <inviwo/core/interaction/pickingmapper.h>

#include <modules/opengl/buffer/bufferobject.h>
#include <modules/opengl/buffer/bufferobjectarray.h>

namespace inviwo {

class InviwoApplication;

/** \docpage{org.inviwo.InviwoStatsPlot, Inviwo Stats Plot}
 * ![](org.inviwo.InviwoStatsPlot.png?classIdentifier=org.inviwo.InviwoStatsPlot)
 * Visualize data from the Inviwo Statistics Collector
 *
 * ### Inports
 *   * __backgroundPort__ Optional background port.
 *
 * ### Outports
 *   * __outport__ Time line plot of statistics.
 *
 * ### Properties
 *   * __timeWindow___ Size of sliding time window.
 *   * __dataRange___  Range of data values
 */
class IVW_MODULE_DEVTOOLS_API InviwoStatsPlot : public Processor {
public:
    InviwoStatsPlot(InviwoApplication* app);
    virtual ~InviwoStatsPlot() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    void linePicked(PickingEvent* p);

    InviwoApplication* app_;

    ImageInport backgroundPort_;
    ImageOutport outport_;

    BoolProperty enable_;
    DoubleProperty timeWindow_;
    DoubleMinMaxProperty dataRange_;

    IntSize2Property position_;
    IntSize2Property size_;

    plot::AxisData timeAxisSettings_;
    plot::AxisData durationAxisSettings_;

    plot::AxisRenderer timeAxis_;
    plot::AxisRenderer durationAxis_;

    Shader shader_;

    struct Lines {
        Lines(size_t size);
        Lines(const Lines&) = delete;
        Lines(Lines&&) = delete;
        Lines& operator=(const Lines&) = delete;
        Lines& operator=(Lines&&) = delete;
        ~Lines();

        void bind();
        void unbind();

        void resize(size_t newSize);

        void map();
        void unmap();


        struct Name {
            static constexpr GLuint time = 0;
            static constexpr GLuint duration = 1;
            static constexpr GLuint color = 2;
            static constexpr GLuint pickid = 3;
        };
        std::array<GLuint, 4> bufferIds;
        GLuint vertexArrayId;
        size_t size;

        float* times = nullptr;
        float* durations = nullptr;
        vec4* colors = nullptr;
        unsigned int* pickids = nullptr;


    };
    Lines lines_;
    IndexBuffer indices_;

    PickingMapper pickingMapper_;
};

}  // namespace inviwo
