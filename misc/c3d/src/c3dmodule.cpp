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

#include <inviwo/c3d/c3dmodule.h>

#include <inviwo/c3d/io/c3dreader.h>
#include <inviwo/c3d/ports/c3dport.h>
#include <inviwo/c3d/processors/c3daveragedpositions.h>
#include <inviwo/c3d/processors/c3dpointalignment.h>
#include <inviwo/c3d/processors/c3dsource.h>
#include <inviwo/c3d/processors/c3dtodataframe.h>
#include <inviwo/c3d/processors/c3dtomesh.h>
#include <inviwo/c3d/processors/c3dtransformpoints.h>

#include <modules/brushingandlinking/processors/sequencebrush.h>

#include <inviwo/dataframe/processors/sequencetodataframe.h>

namespace inviwo {

template <>
struct SequenceToDataFrameTraits<C3D> {
    static auto header(DataFrame& df) {
        return std::tuple{df.addColumn<int>("Frames"), df.addColumn<int>("Points"),
                          df.addColumn<int>("Analogs"), df.addColumn<float>("Frame Rate"),
                          df.addCategoricalColumn("Filename")};
    }
    static void add(const C3D& c3d, auto& cols) {
        auto& [frames, points, analogs, frameRate, file] = cols;

        const auto& header = c3d.header();
        frames->add(static_cast<int>(header.nbFrames()));
        points->add(static_cast<int>(header.nb3dPoints()));
        analogs->add(static_cast<int>(header.nbAnalogs()));
        frameRate->add(header.frameRate());
        file->add(c3d.file());
    }
};

C3DModule::C3DModule(InviwoApplication* app) : InviwoModule(app, "C3D") {
    registerProcessor<C3DAveragedPositions>();
    registerProcessor<C3DPointAlignment>();
    registerProcessor<C3DSource>();
    registerProcessor<C3DSequenceSource>();
    registerProcessor<C3DToDataFrame>();
    registerProcessor<C3DToMesh>();
    registerProcessor<C3DTransformPoints>();
    registerProcessor<SequenceBrush<C3D>>();
    registerProcessor<SequenceToDataFrame<C3D>>();

    registerDefaultsForDataType<C3D>();

    registerDataReader(std::make_unique<C3DReader>());
}

}  // namespace inviwo
