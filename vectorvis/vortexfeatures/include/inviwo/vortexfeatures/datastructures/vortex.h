/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020 Inviwo Foundation
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

#include <inviwo/vortexfeatures/vortexfeaturesmoduledefine.h>
#include <modules/vectorfieldvisualization/datastructures/integrallineset.h>
#include <fmt/format.h>

namespace inviwo {

/** Simple vortex representations of a boundary and a center point. **/
struct IVW_MODULE_VORTEXFEATURES_API Vortex {
    Vortex() {}
    Vortex(const Vortex& vort);
    Vortex(Vortex&& vort);
    Vortex& operator=(const Vortex& vort);
    Vortex& operator=(Vortex&& vort);
    Vortex(const std::vector<dvec2>& boundary, const dvec2& center, double avgRadius = -1,
           double minRadius = -1, double maxRadius = -1, size_t height = 0, size_t time = 0);
    Vortex(std::vector<dvec2>&& boundary, const dvec2& center, double avgRadius = -1,
           double minRadius = -1, double maxRadius = -1, size_t height = 0, size_t time = 0);
    Vortex(const IntegralLine& boundary, const dvec2& center, double avgRadius = -1,
           double minRadius = -1, double maxRadius = -1, size_t height = 0, size_t time = 0);
    /** Calculate center as average boundary position. **/
    Vortex(const IntegralLine& boundary, double avgRadius = -1, double minRadius = -1,
           double maxRadius = -1, size_t height = 0, size_t time = 0);

    /** Sets the radii to the minimal, maximal and average distance of boundary points to the
     * center. **/
    void setRadii();
    /** Check whether a given point is within the boundary. **/
    bool containsPoint(const dvec2& point) const;
    /** Get the number of point in the boundary. **/
    size_t size() const { return boundary.size(); }

public:
    /** Boundary points, expect a loose circle. **/
    std::vector<dvec2> boundary;
    /** Center point. **/
    dvec2 center;
    /** Representative boundary minimal, average and maximal radius. **/
    double avgRadius, minRadius, maxRadius;
    /** Potential information about the height and time this vortex occurs at. **/
    size_t heightSlice, timeSlice;
};

/**
 * \brief A collection of vortex structs, contained in groups.
 * Contains a list of 2D vortices that can be iterated and accessed directly.
 * In addition, these vortices can be organized in groups, i.e., to collect several boundaries of
 * the same vortex, different heights, or different time slices. Groups are accessed directly using
 * indexed iterators (i.e., beginGroup(4)) and size (i.e., sizeGroup(4)).
 */
struct IVW_MODULE_VORTEXFEATURES_API VortexSet {
    /** A new, empty vortex set. **/
    VortexSet(const mat4& modelMat, const mat4& worldMat);
    /** A new vortex set with integral lines added. **/
    VortexSet(const IntegralLineSet& lineSet);
    /** Add one vortex element to the last group. **/
    void push_back(const Vortex& vort);
    /** Emplace one vortex element to the last group. **/
    void push_back(Vortex&& vort);
    /** Add a list of vortices. **/
    void append(const std::vector<Vortex>& vorts);
    /** Add another set of vortices, keeping the groups apart. **/
    void append(const VortexSet& vorts);
    /** Add a set of vortices. **/
    void append(const IntegralLineSet& vorts, size_t height = 0, size_t time = 0);
    /** Remove and return the last element. **/
    Vortex pop_back();
    /** Return the last vortex. Fails if empty. **/
    Vortex& back();
    /** Return the last vortex. Fails if empty. **/
    const Vortex& back() const;
    /** Start a new group of 0 elements. Next vortices added will go in there. **/
    void startNewGroup();
    /** Merge the last two groups into one (or remove an empty trailing group). **/
    void mergeLastGroups();

    /** Return the nth vortex, regardless of group. **/
    Vortex& operator[](size_t idx);
    /** Return the nth vortex, regardless of group. **/
    const Vortex& operator[](size_t idx) const;
    /** Return the number of vortices, regardless of group. **/
    size_t size() const;
    /** Return the number of vortices in the nth group. **/
    size_t sizeGroup(size_t idx) const;
    /** Return the number of groups. **/
    size_t numGroups() const;

    /** Return the range of time steps present in set. **/
    const size2_t& getTimeRange() const { return timeRange_; }
    /** Return the range of height slices present in set. **/
    const size2_t& getHeightRange() const { return heightRange_; }
    /** Return matrix to transform vortex positions to model space. **/
    const mat4& getModelMatrix() const { return modelMatrix_; }
    /** Return matrix to transform vortex positions to world space. **/
    const mat4& getWorldMatrix() const { return worldMatrix_; }
    /** Update the time and height range. **/
    void updateRanges(size_t countFromEnd = 1);

    std::vector<Vortex>::iterator begin();
    std::vector<Vortex>::iterator end();
    std::vector<Vortex>::const_iterator begin() const;
    std::vector<Vortex>::const_iterator end() const;

    std::vector<Vortex>::iterator beginGroup(size_t idx);
    std::vector<Vortex>::iterator endGroup(size_t idx);
    std::vector<Vortex>::const_iterator beginGroup(size_t idx) const;
    std::vector<Vortex>::const_iterator endGroup(size_t idx) const;

private:
    /** All vortices in this set. **/
    std::vector<Vortex> vortices;
    /** The offsets of the groups into the boundary list, starting with 0.
     * Groups lie consecutive in memory. **/
    std::vector<size_t> vortexOffsets;

    /** Minimum and maximum time stamp present in vortices.**/
    size2_t timeRange_;
    /** Minimum and maximum height slice present in vortices.**/
    size2_t heightRange_;

    /** Trasform from given coordinates to model space. **/
    mat4 modelMatrix_;
    /** Trasform from given coordinates to world space. **/
    mat4 worldMatrix_;
};

template <>
struct DataTraits<VortexSet> {
    static std::string classIdentifier() { return "org.inviwo.VortexSet"; }
    static std::string dataName() { return "VortexSet"; }
    static uvec3 colorCode() {
        return uvec3(220, 170, 50);
    }  // Between IntegralLineSet and Mesh color.
    static Document info(const VortexSet& data) {
        Document doc;
        doc.append("p", fmt::format("Vortex set with {} vortices in {} groups.", data.size(),
                                    data.numGroups()));
        return doc;
    }
};  // namespace inviwo

}  // namespace inviwo