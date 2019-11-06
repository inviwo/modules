#pragma once

#include <inviwo/tensorvisbase/tensorvisbasemoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/tensorvisbase/datastructures/invariantspace.h>
#include <inviwo/dataframe/datastructures/dataframe.h>

namespace inviwo {

class IVW_MODULE_TENSORVISBASE_API InvariantSpaceToDataFrame : public Processor {
public:
    InvariantSpaceToDataFrame();
    virtual ~InvariantSpaceToDataFrame() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    InvariantSpaceInport inport_;
    DataFrameOutport outport_;
};

}  // namespace inviwo
