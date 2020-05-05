/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <inviwo/integrallinefiltering/integrallinefilteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/util/glm.h>

#include <unordered_map>

namespace inviwo {

/*
 * A class to represent sparse histograms. Supports both signed and unsigned indices (of integral
 * type) and multi dimensional histogram through the IndexType. For example, `SparseHistogram<int>`
 * is a 1D histogram accepting both negative and positive indices, i.e. [INT_MIN INT_MAX].
 * SparseHistgram can represent multi dimensional histogram when using vec as index type: For
 * example, `SparseHistogram<glm::ivec2>` represents a 2D histogram.
 */
template <typename IndexType = glm::i64>
class SparseHistogram {
public:
    using index_type = IndexType;
    using value_type = typename util::value_type<IndexType>::type;

    static_assert(std::is_integral_v<value_type>);

    SparseHistogram() = default;
    SparseHistogram(const SparseHistogram&) = default;
    SparseHistogram(SparseHistogram&&) noexcept = default;
    SparseHistogram& operator=(const SparseHistogram&) = default;
    SparseHistogram& operator=(SparseHistogram&&) noexcept = default;

    size_t operator[](index_type binId) const {
        const auto it = bins_.find(binId);
        return it != bins_.end() ? it->second : 0;
    }

    size_t& operator[](index_type binId) { return bins_[binId]; }

    auto begin() { return bins_.begin(); }
    auto end() { return bins_.end(); }
    auto begin() const { return bins_.begin(); }
    auto end() const { return bins_.end(); }

    /*
     * Returns the current number of used bins.
     */
    size_t numberOfBins() const { return bins_.size(); }

    /*
     * Removed bins where count is zero.
     */
    void cleanup() {
        for (auto it = bins_.begin(); it != bins_.end();) {
            if (it->second == 0) {
                it = bins_.erase(it);
            } else {
                ++it;
            }
        }
    }

private:
    std::unordered_map<IndexType, size_t> bins_;
};

}  // namespace inviwo
