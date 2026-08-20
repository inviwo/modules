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

#include <ezc3d/AnalogsInfo.h>
#include <ezc3d/PointsInfo.h>
#include <ezc3d/RotationsInfo.h>

#include <fmt/std.h>

namespace inviwo {

C3D::C3D(const std::filesystem::path& path, Options options) : ezc3d::c3d{} {
    _filePath = path.generic_string();

    std::fstream stream(path, std::ios::in | std::ios::binary);
    c_float = std::vector<char>(m_nByteToRead_float + 1);
    c_float_tp = std::vector<char>(m_nByteToRead_float + 1);
    c_int = std::vector<char>(m_nByteToReadMax_int + 1);
    c_int_tp = std::vector<char>(m_nByteToReadMax_int + 1);

    if (!stream.is_open()) {
        throw std::ios_base::failure("The c3d file could not be opened, please verify the path");
    }

    // Read all the section
    _header = std::make_shared<ezc3d::Header>(*this, stream);
    _parameters = std::make_shared<ezc3d::ParametersNS::Parameters>(*this, stream);

    // header may be inconsistent with the parameters, so it must be
    // update to make sure sizes are consistent
    updateHeader();

    // Now read the data
    _data = std::make_shared<ezc3d::DataNS::Data>();

    // This is a copy of ezc3d::DataNS::Data::Data(ezc3d::c3d &c3d, std::fstream &file)
    // With some changes to be able to skip analogs for example
    {
        // Firstly move the pointer to the data start position
        stream.seekg(static_cast<int>(_header->dataStart() - 1) * 512, std::ios::beg);

        // Read the data
        ezc3d::DataNS::Points3dNS::Info pointsInfo{*this};
        ezc3d::DataNS::AnalogsNS::Info analogsInfo{*this};
        ezc3d::DataNS::RotationNS::Info rotationsInfo{*this};

        size_t nbFrames = _header->nbFrames();
        if (nbFrames == 0xFFFF && !_header->hasRotationalData()) {
            // This is a special case to account for Vicon files which don't provide the
            // actual number of frames in the header nor in the parameters when the
            // number of frames is larger than 65535. We need to make sure the
            // rotational data are not present in the file after the points and analogs
            // data in order to use "all-of-file" (nbFrames = -1) reading
            nbFrames = -1;
        }

        for (size_t j = 0; j < nbFrames; ++j) {
            ezc3d::DataNS::Frame f;
            // Read point 3d
            f.add(ezc3d::DataNS::Points3dNS::Points(*this, stream, pointsInfo));

            // Read analogs
            if (options.readAnalogs) {
                f.add(ezc3d::DataNS::AnalogsNS::Analogs(*this, stream, analogsInfo));
            } else {
                stream.seekg(sizeof(float) * _header->nbAnalogByFrame() * _header->nbAnalogs(),
                             std::ios::cur);
            }
            // If we ran out of space, then leave. The reason we test here is because
            // is set after failing, resulting in one extra frame added if this if
            // is after the push_back
            if (stream.eof()) break;

            _data->frame(f);
        }

        // Read the rotation data
        if (_header->hasRotationalData() && options.readRotations) {
            // Prepare the reading

            // If the max length of the file is smaller than the data start, then there
            // is no data
            std::streampos fileSize = stream.seekg(0, std::ios::end).tellg();
            int targetPos(static_cast<int>(rotationsInfo.dataStart() - 1) * 512);
            if (fileSize < targetPos) {
                return;
            }
            stream.seekg(targetPos, std::ios::beg);

            for (size_t i = 0; i < _header->nbFrames(); ++i) {
                if (stream.eof()) break;

                _data->frame(i).add(
                    ezc3d::DataNS::RotationNS::Rotations(*this, stream, rotationsInfo));
            }
        }
    }

    // Parameters and header may be inconsistent with data,
    // so reprocess them if needed
    updateParameters();

    // Close the file
    stream.close();
}

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
        const ezc3d::DataNS::AnalogsNS::Analogs analogs = copyAnalogs(srcFrame);
        dstFrame.add(analogs);
    }
}

void copyRotations(const ezc3d::DataNS::Frame& srcFrame, ezc3d::DataNS::Frame& dstFrame) {
    if (!srcFrame.rotations().isEmpty()) {
        const ezc3d::DataNS::RotationNS::Rotations rots = copyRotations(srcFrame);
        dstFrame.add(rots);
    }
}

void copyPoints(const ezc3d::DataNS::Frame& srcFrame, ezc3d::DataNS::Frame& dstFrame) {
    if (!srcFrame.points().isEmpty()) {
        const ezc3d::DataNS::Points3dNS::Points pts = copyPoints(srcFrame);
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
