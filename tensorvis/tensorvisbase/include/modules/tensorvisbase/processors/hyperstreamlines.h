#pragma once

#include <modules/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/util/utilities.h>
#include <inviwo/core/util/foreach.h>
#include <modules/vectorfieldvisualization/datastructures/integrallineset.h>
#include <modules/vectorfieldvisualization/ports/seedpointsport.h>
#include <modules/tensorvisbase/datastructures/hyperstreamlinetracer.h>

namespace inviwo {

class IVW_MODULE_TENSORVISBASE_API HyperStreamlines : public Processor {
public:
    HyperStreamlines();
    virtual ~HyperStreamlines();

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    DataInport<SpatialSampler<3,3,double>> sampler_;
    SeedPointsInport<SpatialSampler<3,3,double>::SpatialDimensions> seeds_;

    IntegralLineSetOutport lines_;

    IntegralLineProperties properties_;
};
}  // namespace inviwo