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
#include <modules/vectorfieldvisualization/vectorfieldvisualizationmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/processors/processortraits.h>
#include <inviwo/core/processors/poolprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/imageport.h>
#include <inviwo/core/ports/meshport.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/util/foreach.h>
#include <modules/vectorfieldvisualization/algorithms/integrallineoperations.h>
#include <modules/vectorfieldvisualization/integrallinetracer.h>
#include <modules/vectorfieldvisualization/ports/seedpointsport.h>

namespace inviwo {

/** \docpage{org.inviwo.WindingAngle, Winding Angle}
 * ![](org.inviwo.WindingAngle.png?classIdentifier=org.inviwo.WindingAngle)
 * \brief Winding angle criterion to detect eddies (closed vortices) in 2D
 *
 * Based loosely on
 * "Detection, quantification, and tracking of vortices using streamline geometry"
 * by Saderjon and Post, 2000 (https://doi.org/10.1016/S0097-8493(00)00029-7).
 *
 * Given a 2D vector field as an image, integrate stream lines. Pick out stream lines that fulfill
 * a circle (i.e., whose inner angles add up to +-360°).
 * In a second step, we filter them by the end point distance and radius variance.
 * The resulting lines are clustered based on their center points, and the outmost line is chosen as
 * the eddy boundary.
 *
 * The output are all selected vortices, sorted outest to innermost in groups.
 */
class IVW_MODULE_VORTEXFEATURES_API WindingAngle : public PoolProcessor {
public:
    WindingAngle();
    virtual ~WindingAngle() = default;

    virtual void process() override;
    static IntegralLine* windStreamline(const StreamLine2DTracer& tracer, const dvec2& seed,
                                        size_t maxTotalSteps);
    void integrateLines();
    bool filterLines();
    void updateScores();

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

public:
    struct ScoreProperty : public BoolCompositeProperty {
        ScoreProperty(const std::string& identifier, const std::string& displayName);
        DoubleProperty roundnessWeight_, sizeWeight_, maxSize_;
        DoubleProperty accumMaxSize_;
        double getScore(const Vortex& vortex) const;
        void computeScores(const VortexSet& vortices);
    };

private:
    ImageInport inImage_;

    /** All vortices. Enclosing boundaries are ordered within groups. **/
    VortexSetOutport vortexOut_;
    /** Mutable version for updating scores. **/
    std::shared_ptr<VortexSet> vortices_;

    /** General integration properties. **/
    IntegralLineProperties properties_;
    /** How many steps can maximally be taken when integrating? **/
    IntProperty maxTotalSteps_;
    /** Use absolute or relative threshold proeprty? **/
    BoolProperty relativeThreshold_;
    /** What is the maximum end point distance relative to the line length? **/
    DoubleProperty relativeDistanceThreshold_;
    /** What is the maximum distance of the start and end point? **/
    DoubleProperty absoluteDistanceThreshold_;
    /** Limit the ratio between the longest and shortest radius. **/
    BoolProperty limitRadiusRatio_;
    /** How much may the min and max radius deviate from each other? **/
    DoubleProperty maxRadiusRatio_;

    bool integrate_ = false;
    bool filter_ = false;

    /** All lines integrated and closed to 360°. **/
    std::shared_ptr<IntegralLineSet> allLines_;

    ScoreProperty scoreProperties_;
};

}  // namespace inviwo
