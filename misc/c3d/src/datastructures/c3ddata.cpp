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

#include <inviwo/c3d/datastructures/c3ddata.h>

#include <ezc3d/ezc3d.h>
#include <ezc3d/Header.h>
#include <ezc3d/Parameters.h>
#include <ezc3d/Data.h>

namespace inviwo {

ezc3d::DataNS::Points3dNS::Points copyPoints(const ezc3d::DataNS::Frame& srcFrame) {
    ezc3d::DataNS::Points3dNS::Points pts;
    for (size_t i = 0; i < srcFrame.points().nbPoints(); ++i) {
        const auto& sp = srcFrame.points().point(i);
        ezc3d::DataNS::Points3dNS::Point pt;
        pt.set(sp.x(), sp.y(), sp.z(), sp.residual());
        pt.cameraMask(sp.cameraMask());
        pts.point(pt);
    }
    return pts;
}

ezc3d::DataNS::AnalogsNS::Analogs copyAnalogs(const ezc3d::DataNS::Frame& srcFrame) {
    ezc3d::DataNS::AnalogsNS::Analogs analogs;
    for (size_t s = 0; s < srcFrame.analogs().nbSubframes(); ++s) {
        const auto& srcSf = srcFrame.analogs().subframe(s);
        ezc3d::DataNS::AnalogsNS::SubFrame sub;
        for (size_t c = 0; c < srcSf.nbChannels(); ++c) {
            ezc3d::DataNS::AnalogsNS::Channel ch;
            ch.data(srcSf.channel(c).data());
            sub.channel(ch);
        }
        analogs.subframe(sub);
    }
    return analogs;
}

ezc3d::DataNS::RotationNS::Rotations copyRotations(const ezc3d::DataNS::Frame& srcFrame) {
    ezc3d::DataNS::RotationNS::Rotations rots;
    for (size_t s = 0; s < srcFrame.rotations().nbSubframes(); ++s) {
        const auto& srcSf = srcFrame.rotations().subframe(s);
        ezc3d::DataNS::RotationNS::SubFrame sub;
        for (size_t r = 0; r < srcSf.nbRotations(); ++r) {
            const auto& sr = srcSf.rotation(r);
            ezc3d::DataNS::RotationNS::Rotation rot;
            rot.set(sr(0, 0), sr(0, 1), sr(0, 2), sr(0, 3), sr(1, 0), sr(1, 1), sr(1, 2), sr(1, 3),
                    sr(2, 0), sr(2, 1), sr(2, 2), sr(2, 3), sr(3, 0), sr(3, 1), sr(3, 2), sr(3, 3),
                    sr.reliability());
            sub.rotation(rot);
        }
        rots.subframe(sub);
    }
    return rots;
}

void copyAnalogs(const ezc3d::DataNS::Frame& srcFrame, ezc3d::DataNS::Frame& dstFrame) {
    if (!srcFrame.analogs().isEmpty()) {
        ezc3d::DataNS::AnalogsNS::Analogs analogs = copyAnalogs(srcFrame);
        dstFrame.add(analogs);
    }
}

void copyRotations(const ezc3d::DataNS::Frame& srcFrame, ezc3d::DataNS::Frame& dstFrame) {
    if (!srcFrame.rotations().isEmpty()) {
        ezc3d::DataNS::RotationNS::Rotations rots = copyRotations(srcFrame);
        dstFrame.add(rots);
    }
}

void copyPoints(const ezc3d::DataNS::Frame& srcFrame, ezc3d::DataNS::Frame& dstFrame) {
    if (!srcFrame.points().isEmpty()) {
        ezc3d::DataNS::Points3dNS::Points pts = copyPoints(srcFrame);
        dstFrame.add(pts);
    }
}

std::shared_ptr<ezc3d::c3d> copy(const ezc3d::c3d& src) {
    auto dst = std::make_shared<ezc3d::c3d>();

    // Register point names. This initializes the POINT configuration
    // (POINT:LABELS, POINT:USED, header, etc.) in the new c3d object.
    for (const auto& name : src.pointNames()) {
        dst->point(name);
    }

    // Register analog channel names. This initializes the ANALOG configuration.
    for (const auto& name : src.channelNames()) {
        dst->analog(name);
    }

    // Deep copy all data frames. Each frame is rebuilt from scratch so that
    // the new c3d object owns its own Points, Analogs, and Rotations data
    // rather than sharing via shared_ptr with the source.
    for (size_t f = 0; f < src.data().nbFrames(); ++f) {
        const auto& srcFrame = src.data().frame(f);
        ezc3d::DataNS::Frame dstFrame;

        // Deep copy points
        copyPoints(srcFrame, dstFrame);

        // Deep copy analogs
        copyAnalogs(srcFrame, dstFrame);

        // Deep copy rotations (if present)
        copyRotations(srcFrame, dstFrame);

        dst->frame(dstFrame);
    }

    // Copy parameter groups that are not auto-managed by point()/analog()/frame().
    // POINT and ANALOG groups are already set up correctly by the calls above.
    const auto& srcParams = src.parameters();
    for (size_t g = 0; g < srcParams.nbGroups(); ++g) {
        const auto& group = srcParams.group(g);
        if (group.name() == "POINT" || group.name() == "ANALOG") {
            continue;
        }
        for (size_t p = 0; p < group.nbParameters(); ++p) {
            dst->parameter(group.name(), group.parameter(p));
        }
    }

    return dst;
}

}  // namespace inviwo
