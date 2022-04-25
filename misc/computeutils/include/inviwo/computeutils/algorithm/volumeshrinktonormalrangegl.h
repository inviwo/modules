/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2021 Inviwo Foundation
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

namespace inviwo {
/** \class VolumeShrinkToNormalRangeGL
 *
 * GL implementation of volume shrink operation. The algorithm takes in a volume and shrinks its
 * data in the selected channels to range [0,1] + offset where offset is the percentual deviation of
 * the minimum from 0. For example, range [-0.5, 1.0] will be shrunk to [-0.33, 0.66].
 */
class IVW_MODULE_COMPUTEUTILS_API VolumeShrinkToNormalRangeGL {
public:
    template <typename Callback>
    VolumeShrinkToNormalRangeGL(Callback C) : VolumeShrinkToNormalRangeGL() {
        shader_.onReload(C);
    }

    VolumeShrinkToNormalRangeGL();

    virtual ~VolumeShrinkToNormalRangeGL() = default;

    /**
     * Shrinks volume data in the selected channels to range [0,1] + offset where offset is the
     * percentual deviaton of the minimum from 0. For example, range [-0.5, 1.0] will be shrunk to
     * [-0.33, 0.66].
     *
     * @param volume Input volume to be shrunk.
     * @return A volume whose selected channels have been shrunk.
     */
    std::shared_ptr<Volume> shrink(const Volume& volume);

    /**
     * Sets the channel that are to be shrunk. In practice, this method in-/ejects shader
     * defines for every channel.
     *
     * @param channel Channel for which the shrinking should be set to true or false.
     * @param shrink Boolean value indicating whether or not the selected channel should be shrunk.
     */
    void setShrinkChannel(size_t channel, bool shrink);

    /**
     * Sets the channels that are to be shrunk. In practice, this method in-/ejects shader
     * defines for every channel.
     *
     * @param shrink Set of boolean values indicating which channels to shrink.
     */
    void setShrinkChannels(bvec4 shrink);

    /**
     * Resets the skrinking settings. Channel 0 is set to true, rest to false.
     */
    void reset();

protected:
    Shader shader_;

private:
    bool needsCompilation_;
};

}  // namespace inviwo
