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
#include <inviwo/vortexfeatures/ports/vortexport.h>
#include <inviwo/vortexfeatures/processors/windingangle.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/meshport.h>
#include <modules/vectorfieldvisualization/algorithms/integrallineoperations.h>
#include <modules/vectorfieldvisualization/integrallinetracer.h>

namespace inviwo {

/** \docpage{org.inviwo.AssembleWindingAngle, Assemble Winding Angle}
 * ![](org.inviwo.AssembleWindingAngle.png?classIdentifier=org.inviwo.AssembleWindingAngle)
 *
 * \brief Processor triggering and assembling winding angle results across 4 dimensions.
 *
 * Makes use of the WindingAngle processor, which operates on 2D data.
 * To assemble the result of 4D data (3 spatial + time), this processor iterates through slices to
 * be selected through properties (one time slice, wherein one height slice).
 * The result of the winding angle processor is then fed into this processors inport.
 * From all incoming centers and winding angle streamlines, a set is chosen that represents the
 * eddy. Candidates are discarded as eddies if they can not be trces over a number of time steps.
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
class IVW_MODULE_VORTEXFEATURES_API AssembleWindingAngle : public Processor {
public:
    AssembleWindingAngle();
    virtual ~AssembleWindingAngle() = default;

    virtual void process() override;
    void triggerAccumulate();
    void assemble();

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    enum class ExportVortices { NoMatching, ExportAll, ExportOnlyFirst };

    VortexSetInport vortexIn_;
    VortexSetOutport vortexOut_;
    std::shared_ptr<VortexSet> outVortices_;

    /** Time slice to pick. Assume connection to volume sequence selection or loader setting. **/
    IntSizeTProperty timeSlice_;
    /** Height slice to pick. Assume connection to volume slice selection or loader setting. **/
    IntSizeTProperty heightSlice_;
    /** Start moving through time and height slices, accumulating the results. **/
    ButtonProperty triggerAccumulation_;

    TemplateOptionProperty<ExportVortices> exportVortices_;
    WindingAngle::ScoreProperty scoreProperties_;

    struct MatchProperties : public CompositeProperty {
        MatchProperties(const std::string& identifier, const std::string& displayName);
        DoubleProperty scoreWeight_, sizeDiffWeight_;
        const Vortex& bestMatchInGroup(const Vortex& seedVort, const VortexSet& vortices,
                                       size_t group);
    } matchProperties_;

    /** For each time step, one vortex set per height. **/
    std::vector<std::vector<std::shared_ptr<const VortexSet>>> vortexList_;
    std::shared_ptr<const VortexSet> lastVortexSet_;

    bool accumulating_ = false;
};

}  // namespace inviwo
