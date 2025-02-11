/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2025 Inviwo Foundation
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

#include <inviwo/computeutils/computeutilsmoduledefine.h>
#include <modules/opengl/shader/shader.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/computeutils/algorithm/volumereductiongl.h>

namespace inviwo {
/** \class VolumeChannelSplitGL
 *
 * Splits multi-channel volumes into several single-channel volumes.
 */
class IVW_MODULE_COMPUTEUTILS_API VolumeChannelSplitGL {
public:
    template <typename Callback>
    VolumeChannelSplitGL(Callback C) : VolumeChannelSplitGL() {
        shader_.onReload(C);
    }

    VolumeChannelSplitGL();

    virtual ~VolumeChannelSplitGL() = default;

    /**
     * Splits multi-channel volumes into several single-channel volumes
     *
     * @param volume Volume to be split.
     * @return Set of single-channel volumes
     */
    std::vector<std::shared_ptr<Volume>> split(std::shared_ptr<const Volume> volume);

protected:
    Shader shader_;

    VolumeReductionGL volumeReductionGl_;
};

}  // namespace inviwo
