#include <modules/tensorvisbase/datastructures/invariantspace.h>

namespace inviwo {
InvariantSpace::InvariantSpace(size_t numberOfDimensions,
                               const std::vector<std::string> &identifiers) {
    if (numberOfDimensions != identifiers.size()) {
        LogError(
            "Number of dimensions does not correlate with number of identifiers. Empty invariant "
            "space created.");
        return;
    }

    setNumberOfDimensions(numberOfDimensions);

    identifiers_ = identifiers;

    minmax_.resize(numberOfDimensions,
                   std::array<glm::f64, 2>{std::numeric_limits<double>::max(),
                                           std::numeric_limits<double>::lowest()});
}

}  // namespace inviwo
