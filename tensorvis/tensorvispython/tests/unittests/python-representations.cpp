/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2024 Inviwo Foundation
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

#include <warn/push>
#include <warn/ignore/all>
#include <gtest/gtest.h>
#include <warn/pop>

#include <inviwo/core/common/inviwoapplication.h>
#include <modules/python3/python3module.h>
#include <modules/python3/pythonscript.h>
#include <modules/python3/pybindutils.h>

#include <inviwo/tensorvisbase/datastructures/tensorfield3d.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <glm/gtc/epsilon.hpp>

#include <array>
#include <algorithm>

namespace inviwo {

TEST(Python3Representations, TensorField3D) {
    // create a 4 x 3 x 1 volume with 3x3 tensors
    // tensor values, stored in column-major, increase in x direction
    PythonScript s;
    s.setSource(R"(
import numpy as np
import ivwtensorvis

arr = np.arange(9 * 12, dtype=np.uint8)
arr.shape = (1, 3, 4, 9)

tensorfield = ivwtensorvis.TensorField3D(arr)
)");

    bool status = false;
    s.run([&](pybind11::dict dict) {
        ASSERT_TRUE(dict.contains("tensorfield"));

        auto ob = dict["tensorfield"];
        ASSERT_TRUE(pybind11::isinstance<TensorField3D>(ob));

        auto tensorfield = pybind11::cast<TensorField3D>(ob);

        EXPECT_EQ(size3_t(4, 3, 1), tensorfield.getDimensions());

        const auto& tensors = tensorfield.tensors();
        int expected = 0;
        for (size_t t = 0; t < tensors.size(); ++t) {
            for (int j = 0; j < 3; ++j) {
                for (int i = 0; i < 3; ++i) {
                    EXPECT_DOUBLE_EQ(expected + j * 3 + i, tensors[t][j][i]);
                }
            }
            expected += 9;
        }
        status = true;
    });
    EXPECT_TRUE(status);
}

}  // namespace inviwo
