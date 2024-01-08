/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2024 Inviwo Foundation
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

#include <inviwo/integrallinefiltering/processors/integrallinestodataframe.h>

#include <inviwo/integrallinefiltering/algorithm/shannonentropy.h>

namespace inviwo {

IntegralLinesToDataFrame::MetaDataSettings::MetaDataSettings(std::string identifier,
                                                             std::string displayName)
    : BoolCompositeProperty(identifier, displayName, true) {

    setSerializationMode(PropertySerializationMode::All);

    addProperty(dataType_);
    addProperty(useMagnitude_);
    addProperty(log_);
    addProperty(x_);
    addProperty(y_);
    addProperty(z_);
    addProperty(w_);

    addProperty(avg_);
    addProperty(sd_);
    addProperty(percentiles_);

    dataType_.setReadOnly(true);

    setCollapsed(true);
}

IntegralLinesToDataFrame::MetaDataSettings::MetaDataSettings(
    const IntegralLinesToDataFrame::MetaDataSettings& that)
    : BoolCompositeProperty(that)
    , dataType_{that.dataType_}
    , useMagnitude_{that.useMagnitude_}
    , x_{that.x_}
    , y_{that.y_}
    , z_{that.z_}
    , w_{that.w_}
    , log_{that.log_}
    , avg_{that.avg_}
    , sd_{that.sd_}
    , percentiles_{that.percentiles_} {
    addProperty(dataType_);
    addProperty(useMagnitude_);
    addProperty(log_);
    addProperty(x_);
    addProperty(y_);
    addProperty(z_);
    addProperty(w_);

    addProperty(avg_);
    addProperty(sd_);
    addProperty(percentiles_);
}

void IntegralLinesToDataFrame::MetaDataSettings::updateDataFormat(const DataFormatBase* df) {
    const auto c = df->getComponents();
    dataType_.set(df->getString());
    useMagnitude_.setVisible(c > 1);
    x_.setVisible(c > 1);
    y_.setVisible(c > 1);
    z_.setVisible(c > 2);
    w_.setVisible(c > 3);
}

void IntegralLinesToDataFrame::MetaDataSettings::initFunctions(
    std::vector<MetricCalcFunction>& funcs, const BufferRAM* ram, DataFrame& dataFrame) {

    if (!isChecked()) {
        return;
    }

    auto name = Property::getDisplayName();

    const auto c = ram->getDataFormat()->getComponents();

    std::vector<double> percentiles;
    std::istringstream iss(percentiles_.get());
    double d;
    while (iss >> d) {
        if (d < 0) {
            throw Exception(fmt::format("Percentile cant be negative (got: {})", d), IVW_CONTEXT);
        }
        if (d > 100) {

            throw Exception(fmt::format("Percentile cant needs to be less than 100 (got: {})", d),
                            IVW_CONTEXT);
        }
        if (d > 1) d /= 100;  // from percent to ratio
        percentiles.push_back(d);
    }

    if (!avg_.get() && !sd_.get() && percentiles.empty()) {
        return;
    }

    if (c == 1) {  // scalars
        ram->dispatch<void, dispatching::filter::Scalars>([=, &funcs, df = &dataFrame](auto ramT) {
            using T = typename util::PrecisionValueType<decltype(ramT)>;
            createFunction<T>(funcs, *df, percentiles, name,
                              [=](const T& v) -> float { return static_cast<float>(v); });
        });

    } else {  // vectors
        return ram->dispatch<void, dispatching::filter::Vecs>([this, &funcs, name, percentiles, c,
                                                               df = &dataFrame](auto ramT) {
            using T = typename util::PrecisionValueType<decltype(ramT)>;
            if (useMagnitude_.get()) {
                createFunction<T>(funcs, *df, percentiles, name + "", [c](const T& v) -> float {
                    float l = 0;
                    for (size_t i = 0; i < c; i++) {
                        l += static_cast<float>(util::glmcomp(v, i) * util::glmcomp(v, i));
                    }
                    return std::sqrt(l);
                });
            }
            if (x_.get()) {
                createFunction<T>(funcs, *df, percentiles, name + "-x", [](const T& v) -> float {
                    return static_cast<float>(util::glmcomp(v, 0));
                });
            }
            if (y_.get()) {
                createFunction<T>(funcs, *df, percentiles, name + "-y", [](const T& v) -> float {
                    return static_cast<float>(util::glmcomp(v, 1));
                });
            }
            if (z_.get()) {
                createFunction<T>(funcs, *df, percentiles, name + "-w", [](const T& v) -> float {
                    return static_cast<float>(util::glmcomp(v, 2));
                });
            }
            if (w_.get()) {
                createFunction<T>(funcs, *df, percentiles, name + "-z", [](const T& v) -> float {
                    return static_cast<float>(util::glmcomp(v, 3));
                });
            }
        });
    }
}

const std::string IntegralLinesToDataFrame::MetaDataSettings::classIdentifier =
    "org.inviwo.IntegralLinesToDataFrame.MetaDataProperty";

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo IntegralLinesToDataFrame::processorInfo_{
    "org.inviwo.IntegralLinesToDataFrame",  // Class identifier
    "Integral Lines To Data Frame",         // Display name
    "Integral Line Filtering",              // Category
    CodeState::Experimental,                // Code state
    Tags::CPU,                              // Tags
};
const ProcessorInfo IntegralLinesToDataFrame::getProcessorInfo() const { return processorInfo_; }

IntegralLinesToDataFrame::IntegralLinesToDataFrame()
    : Processor()
    , lines_("lines")
    , dataframe_("dataframe")
    , metaDataSettings_("metaDataSettings", "Meta Data Settings")
    , includeLineID_("includeLineID", "Include Line ID", true)
    , includeNumberOfPoints_("includeNumberOfPoints", "Include Number of points", false)

    , includeLineLength_("includeLineLength", "Include Line Length", true)
    , includeTortuosity_("includeTurtuosity", "Include Tortuosity", true)
    , includeTerminationReason_("includeTerminationReason", "Include Termination Reasons", true)
    , includeEntropy_("includeEntropy", "Include Entropy", true)
    , includeStartPositions_("includeStartPositions", "Include Line Start Coordinates", false)
    , includeEndPositions_("includeEndPositions", "Include Line End Coordinates", false) {

    addPort(lines_);
    addPort(dataframe_);

    addProperties(includeLineID_, includeNumberOfPoints_, includeLineLength_, includeTortuosity_,
                  includeTerminationReason_, includeEntropy_, includeStartPositions_,
                  includeEndPositions_, metaDataSettings_);

    lines_.onChange([this]() {
        if (auto lines = lines_.getData()) {
            if (lines->size() > 0) {
                for (const auto& p : metaDataSettings_.getPropertiesByType<MetaDataSettings>()) {
                    p->setVisible(false);
                }

                for (const auto& keyBuf : lines->front().getMetaDataBuffers()) {
                    auto prop = geMetaDataSettings(keyBuf.first);
                    prop->setVisible(true);
                    prop->updateDataFormat(keyBuf.second->getDataFormat());
                }
            }
        }
    });
}

namespace detail {
template <typename T>
std::vector<T>& createColumn(DataFrame& df, std::string name) {
    return df.addColumn<T>(name)
        ->getTypedBuffer()
        ->getEditableRAMRepresentation()
        ->getDataContainer();
}
}  // namespace detail

void IntegralLinesToDataFrame::process() {
    auto lines = lines_.getData();

    auto df = std::make_shared<DataFrame>(static_cast<std::uint32_t>(lines->size()));

    if (lines->size() > 1) {

        const auto& firstLine = lines->front();

        std::vector<std::function<void(const IntegralLine& line)>> funcs;

        if (includeLineID_.get()) {
            auto& ids = detail::createColumn<glm::uint32>(*df, "Line ID");
            funcs.push_back([&ids](const IntegralLine& line) {
                ids.emplace_back((glm::uint32)line.getIndex());
            });
        }

        if (includeNumberOfPoints_.get()) {
            auto& numPoints = detail::createColumn<glm::uint32>(*df, "#Points");
            funcs.push_back([&numPoints](const IntegralLine& line) {
                numPoints.emplace_back(static_cast<uint32_t>(line.getPositions().size()));
            });
        }

        if (includeLineLength_.get()) {
            auto& lengths = detail::createColumn<float>(*df, "Length");
            funcs.push_back([&lengths](const IntegralLine& line) {
                lengths.push_back(static_cast<float>(line.getLength()));
            });
        }
        if (includeTortuosity_.get()) {
            auto& tortuosities = detail::createColumn<float>(*df, "Tortuosity");
            funcs.push_back([&tortuosities](const IntegralLine& line) {
                double v = glm::length(line.getPositions().front() - line.getPositions().back());
                tortuosities.emplace_back(static_cast<float>(line.getLength() / v));
            });
        }

        if (firstLine.hasMetaData("timestamp")) {
            auto& startTimes = detail::createColumn<float>(*df, "StartTimes");
            auto& endTimes = detail::createColumn<float>(*df, "EndTimes");
            auto& durations = detail::createColumn<float>(*df, "Durations");

            funcs.push_back([&startTimes, &endTimes, &durations](const IntegralLine& line) {
                line.getMetaDataBuffer("timestamp")
                    ->getRepresentation<BufferRAM>()
                    ->dispatch<void, dispatching::filter::Scalars>(
                        [&startTimes, &endTimes, &durations](auto ram) {
                            auto a = static_cast<float>(ram->getDataContainer().front());
                            auto b = static_cast<float>(ram->getDataContainer().back());
                            startTimes.emplace_back(a);
                            endTimes.emplace_back(b);
                            durations.emplace_back(std::abs(b - a));
                        });
            });
        }

        if (includeTerminationReason_.get()) {
            auto fwdTarminationReason = df->addCategoricalColumn("Termination Reason (fwd)");
            auto bwdTarminationReason = df->addCategoricalColumn("Termination Reason (bwd)");

            funcs.push_back(
                [fwd = fwdTarminationReason, bwd = bwdTarminationReason](const IntegralLine& line) {
                    fwd->add(inviwo::toString(line.getForwardTerminationReason()));
                    bwd->add(inviwo::toString(line.getBackwardTerminationReason()));
                });
        }

        if (includeEntropy_.get()) {
            if (firstLine.hasMetaData("velocity")) {
                auto& entropies = detail::createColumn<float>(*df, "Entropy");
                funcs.push_back([&](const IntegralLine& line) {
                    entropies.push_back(static_cast<float>(entropy::shannonEntropyDirectional(
                        line.getMetaData<dvec3>("velocity"), 33)));
                });
            }
        }

        if (includeStartPositions_.get()) {
            auto& startXs = detail::createColumn<float>(*df, "StartX");
            auto& startYs = detail::createColumn<float>(*df, "StartY");
            auto& startZs = detail::createColumn<float>(*df, "StartZ");
            funcs.push_back([&](const IntegralLine& line) {
                const auto& s = line.getPositions().front();
                startXs.push_back((float)s.x);
                startYs.push_back((float)s.y);
                startZs.push_back((float)s.z);
            });
        }

        if (includeEndPositions_.get()) {
            auto& endXs = detail::createColumn<float>(*df, "EndX");
            auto& endYs = detail::createColumn<float>(*df, "EndY");
            auto& endZs = detail::createColumn<float>(*df, "EndZ");
            funcs.push_back([&](const IntegralLine& line) {
                const auto& e = line.getPositions().back();
                endXs.push_back((float)e.x);
                endYs.push_back((float)e.y);
                endZs.push_back((float)e.z);
            });
        }

        for (const auto& keyBuf : firstLine.getMetaDataBuffers()) {
            auto prop = geMetaDataSettings(keyBuf.first);
            prop->initFunctions(funcs, keyBuf.second->getRepresentation<BufferRAM>(), *df);
        }

        auto& idBuf = df->getIndexColumn()
                          ->getTypedBuffer()
                          ->getEditableRAMRepresentation()
                          ->getDataContainer();
        idBuf.clear();
        for (const auto& line : *lines) {
            if (line.getPositions().size() < 2) {
                continue;
            }
            idBuf.push_back(static_cast<uint32_t>(line.getIndex()));
            for (auto& fun : funcs) {
                fun(line);
            }
        }
    }

    dataframe_.setData(df);
}

IntegralLinesToDataFrame::MetaDataSettings* IntegralLinesToDataFrame::geMetaDataSettings(
    std::string key) {
    auto id = util::stripIdentifier(toLower(key));
    auto p = metaDataSettings_.getPropertyByIdentifier(id);
    if (p) {
        if (auto mdp = dynamic_cast<MetaDataSettings*>(p)) {
            return mdp;
        }
        throw inviwo::Exception("Not a MetaDataSettings", IvwContext);
    }
    auto newProp = std::make_unique<MetaDataSettings>(id, key);
    auto newPropPtr = newProp.get();
    metaDataSettings_.addProperty(newProp.release());
    return newPropPtr;
}

}  // namespace inviwo
