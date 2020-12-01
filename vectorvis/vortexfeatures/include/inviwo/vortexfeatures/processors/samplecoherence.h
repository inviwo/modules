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
#include <inviwo/core/processors/poolprocessor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/dataframe/datastructures/dataframe.h>
#include <modules/vectorfieldvisualization/algorithms/integrallineoperations.h>
#include <modules/vectorfieldvisualization/integrallinetracer.h>
#include <inviwo/core/util/volumesequencesampler.h>

namespace inviwo {

namespace detail {
struct CoherenceInfo {
    std::vector<std::shared_ptr<IntegralLineSet>> Lines;
    std::vector<double> CoherencePerGroup;
    std::vector<size_t> SizePerGroup;
};

class ComputeCoherenceHelper {
public:
    ComputeCoherenceHelper(std::shared_ptr<const VolumeSequence> velocities,
                           std::shared_ptr<const VolumeSequence> masks,
                           std::shared_ptr<VolumeSequenceSampler> sampler,
                           PathLine3DTracer&& tracerFwd, PathLine3DTracer&& tracerBwd,
                           bool singleGroup, size_t group)
        : velocities_(velocities)
        , masks_(masks)
        , sampler_(sampler)
        , tracerFwd_(std::move(tracerFwd))
        , tracerBwd_(std::move(tracerBwd))
        , singleGroup_(singleGroup)
        , group_(group) {}

    detail::CoherenceInfo* operator()();
    // const pool::Stop& stop, const pool::Progress& progress) const;

    static bool isInside(const VolumeRAM& mask, dvec3 pos, double maskOrigin);

private:
    std::shared_ptr<const VolumeSequence> velocities_, masks_;
    std::shared_ptr<VolumeSequenceSampler> sampler_;
    PathLine3DTracer tracerFwd_, tracerBwd_;
    bool singleGroup_;
    size_t group_;
};
}  // namespace detail

/** \docpage{org.inviwo.SampleCoherence, Sample Coherence}
 * ![](org.inviwo.SampleCoherence.png?classIdentifier=org.inviwo.SampleCoherence)
 * From an unsteady vector field and a segmentation mask, calculate the coherence:
 * Starting in a segmented volume, how long will pathlines stay within the same label?
 */
class IVW_MODULE_VORTEXFEATURES_API SampleCoherence : public PoolProcessor {
public:
    SampleCoherence();
    virtual ~SampleCoherence() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    void postProcessCoherence(detail::CoherenceInfo* coherence);

    VolumeSequenceInport maskVolumeIn_;
    VolumeSequenceInport velocityIn_;
    // DataFrameInport vortexStatisticsIn_;
    DataFrameOutport coherenceOut_;
    IntegralLineSetOutport linesOut_;

    IntegralLineProperties integratorSettings_;
    IntSizeTProperty samplingStep_;
    IntSizeTProperty lineOutputHeightStep_;
    BoolProperty singleGroup_;
    IntSizeTProperty group_;
    DoubleProperty averageCoherence_;
    DoubleProperty weightedCoherence_;

    std::vector<std::shared_ptr<IntegralLineSet>> timeLineSets_;
};

}  // namespace inviwo
