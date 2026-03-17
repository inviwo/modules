/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2026 Inviwo Foundation
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

#include <inviwo/c3d/processors/c3dtodataframe.h>

#include <inviwo/dataframe/datastructures/column.h>

#include <ezc3d/ezc3d.h>

#include <fmt/format.h>

namespace inviwo {

const ProcessorInfo C3DToDataFrame::processorInfo_{
    "org.inviwo.C3DToDataFrame",  // Class identifier
    "C3D To DataFrame",           // Display name
    "C3D",                        // Category
    CodeState::Experimental,      // Code state
    Tags::CPU | Tag{"C3D"} | Tag{"DataFrame"},
    R"(Converts C3D point and analog data into Inviwo DataFrames.

    **Points output**: One row per frame. Columns include Frame index, Time, and
    for each marker: MarkerName_X, MarkerName_Y, MarkerName_Z (and optionally
    MarkerName_Residual).

    **Analogs output**: One row per analog sample. Columns include Frame index,
    SubFrame index, Time, and one column per analog channel.
    )"_unindentHelp,
};
const ProcessorInfo& C3DToDataFrame::getProcessorInfo() const { return processorInfo_; }

C3DToDataFrame::C3DToDataFrame()
    : Processor{}
    , inport_{"inport", ""_help}
    , pointsOutport_{"points", "DataFrame containing 3D point data"_help}
    , analogOutport_{"analogs", "DataFrame containing analog channel data"_help}
    , includeResiduals_{"includeResiduals", "Include Residuals",
                        "Include point residual values as additional columns"_help, true} {

    addPorts(inport_, pointsOutport_, analogOutport_);
    addProperties(includeResiduals_);
}

void C3DToDataFrame::process() {
    const auto& c3dData = *inport_.getData();
    const auto& c3d = c3dData.data();
    const auto& header = c3d.header();

    const auto pointNames = c3d.pointNames();
    const size_t nbFrames = header.nbFrames();
    const size_t nbPoints = header.nb3dPoints();
    const float frameRate = header.frameRate();

    // Build the points DataFrame
    {
        auto df = std::make_shared<DataFrame>();

        // Frame index column
        std::vector<int> frameIndices(nbFrames);
        std::vector<float> timeValues(nbFrames);
        for (size_t f = 0; f < nbFrames; ++f) {
            frameIndices[f] = static_cast<int>(f);
            timeValues[f] = (frameRate > 0.0f) ? static_cast<float>(f) / frameRate : 0.0f;
        }
        df->addColumn("Frame", std::move(frameIndices));
        df->addColumn("Time", std::move(timeValues));

        // Per-point columns
        for (size_t p = 0; p < nbPoints; ++p) {
            const auto& name = pointNames[p];

            std::vector<float> xs(nbFrames);
            std::vector<float> ys(nbFrames);
            std::vector<float> zs(nbFrames);
            std::vector<float> residuals;
            if (includeResiduals_) {
                residuals.resize(nbFrames);
            }

            for (size_t f = 0; f < nbFrames; ++f) {
                const auto& point = c3d.data().frame(f).points().point(p);
                xs[f] = static_cast<float>(point.x());
                ys[f] = static_cast<float>(point.y());
                zs[f] = static_cast<float>(point.z());
                if (includeResiduals_) {
                    residuals[f] = static_cast<float>(point.residual());
                }
            }

            df->addColumn(fmt::format("{}_X", name), std::move(xs));
            df->addColumn(fmt::format("{}_Y", name), std::move(ys));
            df->addColumn(fmt::format("{}_Z", name), std::move(zs));
            if (includeResiduals_) {
                df->addColumn(fmt::format("{}_Residual", name), std::move(residuals));
            }
        }

        df->updateIndexBuffer();
        pointsOutport_.setData(df);
    }

    // Build the analog DataFrame
    {
        auto df = std::make_shared<DataFrame>();

        const auto channelNames = c3d.channelNames();
        const size_t nbAnalogs = header.nbAnalogs();
        const size_t nbAnalogByFrame = header.nbAnalogByFrame();

        if (nbAnalogs > 0 && nbAnalogByFrame > 0) {
            const size_t nbSubframes = nbAnalogByFrame / nbAnalogs;
            const size_t totalSamples = nbFrames * nbSubframes;

            // Frame and subframe index columns
            std::vector<int> frameIndices(totalSamples);
            std::vector<int> subFrameIndices(totalSamples);
            std::vector<float> timeValues(totalSamples);

            const float analogRate =
                (frameRate > 0.0f) ? frameRate * static_cast<float>(nbSubframes) : 0.0f;

            for (size_t f = 0; f < nbFrames; ++f) {
                for (size_t sf = 0; sf < nbSubframes; ++sf) {
                    const size_t idx = f * nbSubframes + sf;
                    frameIndices[idx] = static_cast<int>(f);
                    subFrameIndices[idx] = static_cast<int>(sf);
                    timeValues[idx] =
                        (analogRate > 0.0f) ? static_cast<float>(idx) / analogRate : 0.0f;
                }
            }

            df->addColumn("Frame", std::move(frameIndices));
            df->addColumn("SubFrame", std::move(subFrameIndices));
            df->addColumn("Time", std::move(timeValues));

            // Per-channel columns
            for (size_t ch = 0; ch < nbAnalogs; ++ch) {
                std::vector<float> values(totalSamples);

                for (size_t f = 0; f < nbFrames; ++f) {
                    const auto& analogs = c3d.data().frame(f).analogs();
                    for (size_t sf = 0; sf < nbSubframes; ++sf) {
                        const size_t idx = f * nbSubframes + sf;
                        values[idx] =
                            static_cast<float>(analogs.subframe(sf).channel(ch).data());
                    }
                }

                const std::string& name =
                    (ch < channelNames.size()) ? channelNames[ch] : fmt::format("Channel_{}", ch);
                df->addColumn(name, std::move(values));
            }
        }

        df->updateIndexBuffer();
        analogOutport_.setData(df);
    }
}

}  // namespace inviwo
