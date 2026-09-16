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
    updateHeaderImpl();

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
    updateParametersImpl();

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

// The updateHeader and updateParameters functions are not exported using EZC3D_API so we can't call
// them So as a _ugly_ _ugly_ workaround, we copy the code here. This is not ideal but it works for now.

void C3D::updateHeaderImpl() {
    // Parameter is always consider as the right value.
    const auto& points(parameters().group("POINT"));
    size_t nbFrames(static_cast<size_t>(points.parameter("FRAMES").valuesConvertedAsInt()[0]));
    if (nbFrames != 0 && nbFrames != header().nbFrames()) {
        // The nbFrames != 0 is to account for Kistler implementation which does not
        // declare points If there is a discrepancy between them, change the header,
        // while keeping the firstFrame value
        _header->lastFrame(nbFrames + _header->firstFrame() - 1);
    }
    double pointRate(points.parameter("RATE").valuesAsDouble()[0]);
    float buffer(10000);  // For decimal truncature
    if (static_cast<int>(pointRate * buffer) != static_cast<int>(header().frameRate() * buffer)) {
        // If there are points but the rate don't match keep the one from header
        if (points.parameter("RATE").valuesAsDouble()[0] == 0.0 &&
            points.parameter("USED").valuesAsInt()[0] != 0) {
            ezc3d::ParametersNS::GroupNS::Parameter rate("RATE");
            rate.set(header().frameRate());
            parameter("POINT", rate);
        } else
            _header->frameRate(static_cast<float>(pointRate));
    }
    if (static_cast<size_t>(points.parameter("USED").valuesAsInt()[0]) != header().nb3dPoints()) {
        _header->nb3dPoints(static_cast<size_t>(points.parameter("USED").valuesAsInt()[0]));
    }

    // Compare the subframe with data when possible, otherwise go with the
    // parameters
    const auto& analog(parameters().group("ANALOG"));
    if (_data != nullptr && data().nbFrames() > 0 && data().frame(0).analogs().nbSubframes() != 0) {
        if (data().frame(0).analogs().nbSubframes() != header().nbAnalogByFrame())
            _header->nbAnalogByFrame(data().frame(0).analogs().nbSubframes());
    } else if (static_cast<size_t>(pointRate) != 0 &&
               static_cast<size_t>(analog.parameter("RATE").valuesAsDouble()[0] / pointRate) !=
                   header().nbAnalogByFrame()) {
        if (header().nbAnalogByFrame() == 1 && parameters().isGroup("SHADOW")) {
            // The SHADOW company is not following the standard so they did not
            // set analog rate ezc3d automatically sets it to zero which results
            // in a discrepancy
            ezc3d::ParametersNS::GroupNS::Parameter& analogNonConst =
                _parameters->group("ANALOG").parameter("RATE");
            analogNonConst.set(static_cast<float>(header().nbAnalogByFrame()));
        } else {
            _header->nbAnalogByFrame(
                static_cast<size_t>(analog.parameter("RATE").valuesAsDouble()[0] / pointRate));
        }
    }

    if (static_cast<size_t>(analog.parameter("USED").valuesAsInt()[0]) != header().nbAnalogs())
        _header->nbAnalogs(static_cast<size_t>(analog.parameter("USED").valuesAsInt()[0]));

    if (parameters().isGroup("ROTATION")) _header->hasRotationalData(true);
}

namespace {

void removeTrailingSpaces(std::string& s) {
    // Remove the spaces at the end of the strings
    for (int i = static_cast<int>(s.size()); i >= 0; --i)
        if (s.size() > 0 && s[s.size() - 1] == ' ')
            s.pop_back();
        else
            break;
}

}  // namespace

void C3D::updateParametersImpl() {
    std::vector<std::string> newPoints{};
    std::vector<std::string> newAnalogs{};

    // If frames has been added
    ezc3d::ParametersNS::GroupNS::Group& grpPoint(
        _parameters->group(parameters().groupIdx("POINT")));
    size_t nFrames(data().nbFrames());
    if (nFrames != static_cast<size_t>(grpPoint.parameter("FRAMES").valuesConvertedAsInt()[0])) {
        size_t idx(grpPoint.parameterIdx("FRAMES"));
        grpPoint.parameter(idx).set(nFrames);
    }

    // If points has been added
    size_t nPoints;
    if (data().nbFrames() > 0)
        nPoints = data().frame(0).points().nbPoints();
    else
        nPoints = parameters().group("POINT").parameter("USED").valuesAsInt()[0] + newPoints.size();
    int oldPointUsed(grpPoint.parameter("USED").valuesAsInt()[0]);
    if (nPoints != static_cast<size_t>(oldPointUsed)) {
        grpPoint.parameter("USED").set(nPoints);

        std::vector<std::string> newLabels;
        std::vector<std::string> newDescriptions;
        std::vector<std::string> newUnits;
        std::vector<std::string> ptsNames(pointNames());
        ptsNames.insert(ptsNames.end(), newPoints.begin(), newPoints.end());
        for (size_t i = nPoints - newPoints.size(); i < nPoints; ++i) {
            std::string name;
            if (data().nbFrames() == 0) {
                if (i < static_cast<size_t>(oldPointUsed))
                    name = parameters().group("POINT").parameter("LABELS").valuesAsString()[i];
                else
                    name = newPoints[i - oldPointUsed];
            } else {
                name = ptsNames[i];
                if (!options.getKeepParametersTrailingSpaces()) removeTrailingSpaces(name);
            }
            newLabels.push_back(name);
            newDescriptions.push_back("");
            newUnits.push_back("mm");
        }

        // Dispatch names in LABELS, LABELS2, etc.
        size_t first_idx = 0;
        size_t last_idx = 0;
        size_t i = 0;
        while (last_idx < newLabels.size()) {
            std::string mod("");
            if (i != 0) {
                mod = std::to_string(i + 1);
                if (!grpPoint.isParameter("LABELS" + mod)) {
                    ezc3d::ParametersNS::GroupNS::Parameter labels("LABELS" + mod);
                    labels.set(std::vector<std::string>() = {});
                    grpPoint.parameter(labels);
                }
                if (!grpPoint.isParameter("DESCRIPTIONS" + mod)) {
                    ezc3d::ParametersNS::GroupNS::Parameter descriptions("DESCRIPTIONS" + mod);
                    descriptions.set(std::vector<std::string>() = {});
                    grpPoint.parameter(descriptions);
                }
                if (!grpPoint.isParameter("UNITS" + mod)) {
                    ezc3d::ParametersNS::GroupNS::Parameter units("UNITS" + mod);
                    units.set(std::vector<std::string>() = {});
                    grpPoint.parameter(units);
                }
            }
            auto labels = grpPoint.parameter("LABELS" + mod).valuesAsString();
            auto descriptions = grpPoint.parameter("DESCRIPTIONS" + mod).valuesAsString();
            auto units = grpPoint.parameter("UNITS" + mod).valuesAsString();

            if (labels.size() != 255) {
                size_t off = grpPoint.parameter("LABELS" + mod).valuesAsString().size();
                last_idx = newLabels.size() >= first_idx + 255 - off ? first_idx + 255 - off
                                                                     : newLabels.size();
                labels.insert(labels.end(), newLabels.begin() + first_idx,
                              newLabels.begin() + last_idx);
                descriptions.insert(descriptions.end(), newDescriptions.begin() + first_idx,
                                    newDescriptions.begin() + last_idx);
                units.insert(units.end(), newUnits.begin() + first_idx,
                             newUnits.begin() + last_idx);

                grpPoint.parameter("LABELS" + mod).set(labels);
                grpPoint.parameter("DESCRIPTIONS" + mod).set(descriptions);
                grpPoint.parameter("UNITS" + mod).set(units);

                // Prepare next for
                first_idx = last_idx;
            }
            ++i;
        }
    }

    // If analogous data has been added
    ezc3d::ParametersNS::GroupNS::Group& grpAnalog(
        _parameters->group(parameters().groupIdx("ANALOG")));
    size_t nAnalogs;
    if (data().nbFrames() > 0) {
        if (data().frame(0).analogs().nbSubframes() > 0)
            nAnalogs = data().frame(0).analogs().subframe(0).nbChannels();
        else
            nAnalogs = 0;
    } else
        nAnalogs =
            parameters().group("ANALOG").parameter("USED").valuesAsInt()[0] + newAnalogs.size();

    // Should always be greater than 0..., but we have to take in
    // account Optotrak lazyness
    if (parameters().group("ANALOG").nbParameters()) {
        int oldAnalogUsed(grpAnalog.parameter("USED").valuesAsInt()[0]);
        if (nAnalogs != static_cast<size_t>(oldAnalogUsed)) {
            grpAnalog.parameter("USED").set(nAnalogs);

            std::vector<std::string> newLabels;
            std::vector<std::string> newDescriptions;
            std::vector<double> newScale;
            std::vector<int> newOffset;
            std::vector<std::string> newUnits;
            std::vector<std::string> chanNames(channelNames());
            chanNames.insert(chanNames.end(), newAnalogs.begin(), newAnalogs.end());
            for (size_t i = nAnalogs - newAnalogs.size(); i < nAnalogs; ++i) {
                std::string name;
                if (data().nbFrames() == 0) {
                    if (i < static_cast<size_t>(oldAnalogUsed))
                        name = parameters().group("ANALOG").parameter("LABELS").valuesAsString()[i];
                    else
                        name = newAnalogs[i - oldAnalogUsed];
                } else {
                    name = chanNames[i];
                    if (!options.getKeepParametersTrailingSpaces()) removeTrailingSpaces(name);
                }
                newLabels.push_back(name);
                newDescriptions.push_back("");
                newScale.push_back(1.0);
                newOffset.push_back(0);
                newUnits.push_back("");
            }

            // Dispatch names in LABELS, LABELS2, etc.
            size_t first_idx = 0;
            size_t last_idx = 0;
            size_t i = 0;
            while (last_idx < newLabels.size()) {
                std::string mod("");
                if (i != 0) {
                    mod = std::to_string(i + 1);
                    if (!grpAnalog.isParameter("LABELS" + mod)) {
                        ezc3d::ParametersNS::GroupNS::Parameter labels("LABELS" + mod);
                        labels.set(std::vector<std::string>() = {});
                        grpAnalog.parameter(labels);
                    }
                    if (!grpAnalog.isParameter("DESCRIPTIONS" + mod)) {
                        ezc3d::ParametersNS::GroupNS::Parameter descriptions("DESCRIPTIONS" + mod);
                        descriptions.set(std::vector<std::string>() = {});
                        grpAnalog.parameter(descriptions);
                    }
                    if (!grpAnalog.isParameter("SCALE" + mod)) {
                        ezc3d::ParametersNS::GroupNS::Parameter scale("SCALE" + mod);
                        scale.set(std::vector<double>() = {});
                        grpAnalog.parameter(scale);
                    }
                    if (!grpAnalog.isParameter("OFFSET" + mod)) {
                        ezc3d::ParametersNS::GroupNS::Parameter offset("OFFSET" + mod);
                        offset.set(std::vector<int>() = {});
                        grpAnalog.parameter(offset);
                    }
                    if (!grpAnalog.isParameter("UNITS" + mod)) {
                        ezc3d::ParametersNS::GroupNS::Parameter units("UNITS" + mod);
                        units.set(std::vector<std::string>() = {});
                        grpAnalog.parameter(units);
                    }
                }

                auto labels = grpAnalog.parameter("LABELS" + mod).valuesAsString();
                auto descriptions = grpAnalog.parameter("DESCRIPTIONS" + mod).valuesAsString();
                auto scale = grpAnalog.parameter("SCALE" + mod).valuesAsDouble();
                auto offset = grpAnalog.parameter("OFFSET" + mod).valuesAsInt();
                auto units = grpAnalog.parameter("UNITS" + mod).valuesAsString();

                if (labels.size() != 255) {
                    size_t off = grpAnalog.parameter("LABELS" + mod).valuesAsString().size();
                    last_idx = newLabels.size() >= first_idx + 255 - off ? first_idx + 255 - off
                                                                         : newLabels.size();
                    labels.insert(labels.end(), newLabels.begin() + first_idx,
                                  newLabels.begin() + last_idx);
                    descriptions.insert(descriptions.end(), newDescriptions.begin() + first_idx,
                                        newDescriptions.begin() + last_idx);
                    scale.insert(scale.end(), newScale.begin() + first_idx,
                                 newScale.begin() + last_idx);
                    offset.insert(offset.end(), newOffset.begin() + first_idx,
                                  newOffset.begin() + last_idx);
                    units.insert(units.end(), newUnits.begin() + first_idx,
                                 newUnits.begin() + last_idx);

                    grpAnalog.parameter("LABELS" + mod).set(labels);
                    grpAnalog.parameter("DESCRIPTIONS" + mod).set(descriptions);
                    grpAnalog.parameter("SCALE" + mod).set(scale);
                    grpAnalog.parameter("OFFSET" + mod).set(offset);
                    grpAnalog.parameter("UNITS" + mod).set(units);

                    // Prepare next for
                    first_idx = last_idx;
                }
                ++i;
            }
        }
    }

    // Deal with ACTUAL_START_FIELD and ACTUAL_END_FIELD from VICON, if they are
    // present
    bool isVicon = parameters().isGroup("MANUFACTURER") &&
                   parameters().group("MANUFACTURER").isParameter("COMPANY") &&
                   parameters()
                           .group("MANUFACTURER")
                           .parameter("COMPANY")
                           .valuesAsString()
                           .at(0)
                           .find("Vicon") != std::string::npos;
    if (isVicon && parameters().group("TRIAL").isParameter("ACTUAL_START_FIELD")) {
        // Make sure "ACTUAL_START_FIELD" is of type INT
        _parameters->group("TRIAL")
            .parameter("ACTUAL_START_FIELD")
            .staticCastType(ezc3d::DATA_TYPE::INT);
    }
    if (isVicon && parameters().group("TRIAL").isParameter("ACTUAL_END_FIELD")) {
        // Make sure "ACTUAL_END_FIELD" is of type INT
        _parameters->group("TRIAL")
            .parameter("ACTUAL_END_FIELD")
            .staticCastType(ezc3d::DATA_TYPE::INT);
    }

    // Adjust some ROTATION parameters
    if (_parameters->isGroup("ROTATION")) {
        ezc3d::ParametersNS::GroupNS::Group& grpRotation(
            _parameters->group(parameters().groupIdx("ROTATION")));
        size_t nbRotations = 0;
        if (_data->frame(0).rotations().nbSubframes() > 0) {
            nbRotations = _data->frame(0).rotations().subframe(0).nbRotations();
        }
        grpRotation.parameter("USED").set(nbRotations);
    }

    updateHeaderImpl();
}

}  // namespace inviwo
