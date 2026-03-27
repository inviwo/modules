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
#pragma once

#include <inviwo/c3d/c3dmoduledefine.h>

#include <memory>
#include <string>

#include <ezc3d/ezc3d.h>

namespace inviwo {

/**
 * \brief Create an independent deep copy of this ezc3d::c3d.
 *
 * The default copy shares the underlying ezc3d::c3d object via shared_ptr.
 * This function creates a new copy of the ezc3d::c3d data so the result
 * can be modified without affecting the original.
 */
IVW_MODULE_C3D_API std::shared_ptr<ezc3d::c3d> copy(const ezc3d::c3d&);

IVW_MODULE_C3D_API ezc3d::DataNS::Points3dNS::Points copyPoints(
    const ezc3d::DataNS::Frame& srcFrame);
IVW_MODULE_C3D_API void copyPoints(const ezc3d::DataNS::Frame& srcFrame,
                                   ezc3d::DataNS::Frame& dstFrame);

IVW_MODULE_C3D_API ezc3d::DataNS::AnalogsNS::Analogs copyAnalogs(
    const ezc3d::DataNS::Frame& srcFrame);
IVW_MODULE_C3D_API void copyAnalogs(const ezc3d::DataNS::Frame& srcFrame,
                                    ezc3d::DataNS::Frame& dstFrame);

IVW_MODULE_C3D_API ezc3d::DataNS::RotationNS::Rotations copyRotations(
    const ezc3d::DataNS::Frame& srcFrame);
IVW_MODULE_C3D_API void copyRotations(const ezc3d::DataNS::Frame& srcFrame,
                                      ezc3d::DataNS::Frame& dstFrame);

}  // namespace inviwo
