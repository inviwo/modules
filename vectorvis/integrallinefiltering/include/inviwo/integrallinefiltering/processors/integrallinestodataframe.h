/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018 Inviwo Foundation
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

#ifndef IVW_INTEGRALLINESTODATAFRAME_H
#define IVW_INTEGRALLINESTODATAFRAME_H

#include <inviwo/integrallinefiltering/integrallinefilteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/ports/imageport.h>
#include <modules/vectorfieldvisualization/datastructures/integrallineset.h>
#include <inviwo/dataframe/datastructures/dataframe.h>

#include <inviwo/core/properties/boolcompositeproperty.h>
#include <inviwo/core/util/zip.h>

#include <inviwo/core/util/utilities.h>

#include <fmt/format.h>

namespace inviwo {

/** \docpage{org.inviwo.IntegralLinesToDataFrame, Integral Lines To Data Frame}
 * ![](org.inviwo.IntegralLinesToDataFrame.png?classIdentifier=org.inviwo.IntegralLinesToDataFrame)
 *
 * Processor that converts a IntegralLineSet to DataFrame which can be used together with the
 * processors in Plotting and PlottingGL for interactive filtering of Integral Lines
 *
 * ### Inports
 *   * __lines__ The set of lines.
 *
 * ### Outports
 *   * __dataframe__ Constructed dataframe.
 *
 * ### Properties
 *   * __Include Line ID__ Check to include Line ID as a parameter.
 *   * __Include Number of points__ Check to include the number of vertices in the line as a
 * parameter.
 *   * __Include Line Length__ Check to include the arc length of the line as a parameter.
 *   * __Include Tortuosity__ Check to include the lines tortuosity as a parameter.
 *   * __Include Termination Reasons__ Check to include each lines termination reason as a
 * parameter.
 *   * __Include Entropy__ Check to include each lines entropy as a parameter.
 *   * __Include Line Start Coordinates__ Check to include the {x,y,z} of the first vertex as a
 * parameter.
 *   * __Include Line End Coordinates__ Check to include {x,y,z} of the last vertex as a parameter.
 *   * __Meta Data Settings__ Group of properties related meta data of the lines and wether or not
 * to include it as a parameter.
 *
 */
class IVW_MODULE_INTEGRALLINEFILTERING_API IntegralLinesToDataFrame : public Processor {
public:
    using F = std::function<void(const IntegralLine &line)>;
    class MetaDataSettings : public BoolCompositeProperty {
    public:
        virtual std::string getClassIdentifier() const override { return classIdentifier; }

        static const std::string classIdentifier;

        MetaDataSettings(std::string identifier, std::string displayName);
        MetaDataSettings(const MetaDataSettings &that);

        virtual MetaDataSettings *clone() const override { return new MetaDataSettings(*this); }

        void updateDataFormat(const DataFormatBase *df);

        StringProperty dataType_{"dataType", "Data Type"};

        BoolProperty useMagnitude_{"magnitude", "Use Magnitude", true};
        BoolProperty x_{"x", "Use X-values", false};
        BoolProperty y_{"y", "Use Y-values", false};
        BoolProperty z_{"z", "Use Z-values", false};
        BoolProperty w_{"w", "Use W-values", false};

        BoolProperty log_{"log", "Log values", false};

        BoolProperty avg_{"avg", "Include Average", true};
        BoolProperty sd_{"sd", "Include Standard deviation", false};
        StringProperty percentiles_{"percentiles",
                                    "Percentiles (space separated, float [0-1] or ints (0-100) )"};

        void initFunctions(std::vector<F> &funcs, const BufferRAM *ram, DataFrame &dataFrame);

        template <typename T, typename C>
        void createFunction(std::vector<F> &funcs, DataFrame &dataFrame,
                            std::vector<double> percentiles, std::string name, C toFloat) {
            std::vector<float> *avg = nullptr;
            std::vector<float> *sds = nullptr;
            if (avg_.get()) {
                avg = &dataFrame.addColumn<float>(name + u8" μ")
                           ->getTypedBuffer()
                           ->getEditableRAMRepresentation()
                           ->getDataContainer();
            }
            if (sd_.get()) {
                sds = &dataFrame.addColumn<float>(name + u8" σ")
                           ->getTypedBuffer()
                           ->getEditableRAMRepresentation()
                           ->getDataContainer();
            }
            std::vector<std::vector<float> *> percentilesColumns;
            for (auto p : percentiles) {
                percentilesColumns.push_back(
                    &dataFrame.addColumn<float>(name + " (p:" + std::to_string(p) + ")")
                         ->getTypedBuffer()
                         ->getEditableRAMRepresentation()
                         ->getDataContainer());
            }
            funcs.push_back([=](const IntegralLine &line) {
                auto &vec = line.getMetaData<T>(Property::getDisplayName());
                std::vector<float> values;
                std::transform(vec.begin() + 1, vec.end() - 1, std::back_inserter(values),
                               [&](const auto &v) {
                                   if (log_.get()) {
                                       return std::log(1 + toFloat(v));
                                   } else {
                                       return toFloat(v);
                                   }
                               });
                
                if (avg || sds) {
                    const auto mean = std::accumulate(values.begin(), values.end(), 0.f) / std::max(size_t(1),values.size());
                    if (avg) {
                        avg->push_back(mean);
                    }
                    if (sds) {
                        float stdev = std::accumulate(
                            values.begin(), values.end(), 0.0f,
                            [mean](float d, auto &v) { return d + (v - mean) * (v - mean); });
                        stdev = std::sqrt(stdev / (values.size() - 1));
                        sds->push_back(stdev);
                    }
                }
                if (!percentiles.empty()) {
                    std::sort(values.begin(), values.end());
                    for (auto zipped : util::zip(percentilesColumns, percentiles)) {
                        get<0>(zipped)->push_back(
                            values[static_cast<size_t>(get<1>(zipped) * (values.size() - 1))]);
                    }
                }
            });
        }
    };

    IntegralLinesToDataFrame();
    virtual ~IntegralLinesToDataFrame() = default;

    virtual void process() override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

private:
    MetaDataSettings *geMetaDataSettings(std::string key);

    IntegralLineSetInport lines_;
    DataFrameOutport dataframe_;

    CompositeProperty metaDataSettings_;

    BoolProperty includeLineID_;
    BoolProperty includeNumberOfPoints_;
    BoolProperty includeLineLength_;
    BoolProperty includeTortuosity_;
    BoolProperty includeTerminationReason_;
    BoolProperty includeEntropy_;
    BoolProperty includeStartPositions_;
    BoolProperty includeEndPositions_;
};

}  // namespace inviwo

#endif  // IVW_INTEGRALLINESTODATAFRAME_H
