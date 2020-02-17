#include <inviwo/tensorvisbase/processors/hyperstreamlines.h>
#include <inviwo/tensorvisbase/tensorvisbasemodule.h>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo HyperStreamlines::processorInfo_{
    "org.inviwo.HyperStreamlines",  // Class identifier
    "HyperStreamlines",             // Display name
    "Tensor Visualization",         // Category
    CodeState::Experimental,        // Code state
    tag::OpenTensorVis | Tag::CPU,  // Tags
};
const ProcessorInfo HyperStreamlines::getProcessorInfo() const { return processorInfo_; }

HyperStreamlines::HyperStreamlines()
    : sampler_("sampler")
    , seeds_("seeds")
    , lines_("lines")
    , properties_("properties", "Properties") {
    addPort(sampler_);
    addPort(seeds_);
    addPort(lines_);

    addProperty(properties_);

    properties_.normalizeSamples_.set(true);
    properties_.normalizeSamples_.setCurrentStateAsDefault();
}

HyperStreamlines::~HyperStreamlines() {}

void HyperStreamlines::process() {
    auto sampler = sampler_.getData();
    auto lines =
        std::make_shared<IntegralLineSet>(sampler->getModelMatrix(), sampler->getWorldMatrix());

    HyperStreamLineTracer tracer(sampler, properties_);

    std::mutex mutex;
    size_t startID = 0;
    for (const auto &seeds : seeds_) {
        util::forEachParallel(*seeds, [&](const auto &p, size_t i) {
            IntegralLine line = tracer.traceFrom(p);
            auto size = line.getPositions().size();
            if (size > 1) {
                std::lock_guard<std::mutex> lock(mutex);
                lines->push_back(std::move(line), startID + i);
            }
        });
        startID += seeds->size();
    }

    lines_.setData(lines);
}
}  // namespace inviwo