#include <modules/tensorvisbase/datastructures/invariantspace.h>
#include <modules/tensorvisbase/util/misc.h>

namespace inviwo {
InvariantSpace::InvariantSpace(size_t numberOfDimensions,
                               const std::vector<std::string>& identifiers,
                               const std::vector<TensorFeature>& metaDataTypes) {
    if (numberOfDimensions != identifiers.size()) {
        LogError(
            "Number of dimensions does not correlate with number of identifiers. Empty invariant "
            "space created.");
        return;
    }

    setNumberOfDimensions(numberOfDimensions);

    identifiers_ = identifiers;

    metaDataTypes_ = metaDataTypes;

    minmax_.resize(numberOfDimensions,
                   std::array<glm::f64, 2>{std::numeric_limits<double>::max(),
                                           std::numeric_limits<double>::lowest()});
}
std::string InvariantSpace::getDataInfo() const {
    std::stringstream ss;
    ss << "<table border='0' cellspacing='0' cellpadding='0' "
          "style='border-color:white;white-space:pre;'>/n"
       << tensorutil::getHTMLTableRowString("Type", "Invariant space")
       << tensorutil::getHTMLTableRowString("Number of dimensions",
                                            std::to_string(getNumberOfDimensions()))
       << tensorutil::getHTMLTableRowString("Number of entries", std::to_string(getNumElements()))
       << tensorutil::getHTMLTableIntermediateHeaderString("Min/max");

    for (size_t i{0}; i < minmax_.size(); ++i) {
        ss << tensorutil::getHTMLTableRowString(identifiers_[i], tensorutil::to_string(minmax_[i]));
    }
    ss << "</table>";
    return ss.str();
}

}  // namespace inviwo
