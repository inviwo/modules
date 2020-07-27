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

#include <inviwo/vortexfeatures/datastructures/vortex.h>
#include <glm/gtx/vector_angle.hpp>

namespace inviwo {

Vortex::Vortex(const Vortex& vort)
    : boundary(vort.boundary)
    , center(vort.center)
    , avgRadius(vort.avgRadius)
    , minRadius(vort.minRadius)
    , maxRadius(vort.maxRadius)
    , heightSlice(vort.heightSlice)
    , timeSlice(vort.timeSlice) {}

Vortex::Vortex(Vortex&& vort)
    : boundary(std::move(vort.boundary))
    , center(vort.center)
    , avgRadius(vort.avgRadius)
    , minRadius(vort.minRadius)
    , maxRadius(vort.maxRadius)
    , heightSlice(vort.heightSlice)
    , timeSlice(vort.timeSlice) {}

Vortex& Vortex::operator=(Vortex&& vort) {
    if (&vort == this) return *this;
    boundary = std::move(vort.boundary);
    center = vort.center;
    avgRadius = vort.avgRadius;
    minRadius = vort.minRadius;
    maxRadius = vort.maxRadius;
    heightSlice = vort.heightSlice;
    timeSlice = vort.timeSlice;
    return *this;
}

Vortex& Vortex::operator=(const Vortex& vort) {
    if (&vort == this) return *this;
    boundary = vort.boundary;
    center = vort.center;
    avgRadius = vort.avgRadius;
    minRadius = vort.minRadius;
    maxRadius = vort.maxRadius;
    heightSlice = vort.heightSlice;
    timeSlice = vort.timeSlice;
    return *this;
}

Vortex::Vortex(const std::vector<dvec2>& boundary, const dvec2& center, double avgRadius,
               double minRadius, double maxRadius, size_t height, size_t time)
    : boundary(boundary)
    , center(center)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time) {
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
}

Vortex::Vortex(std::vector<dvec2>&& boundary, const dvec2& center, double avgRadius,
               double minRadius, double maxRadius, size_t height, size_t time)
    : boundary(std::move(boundary))
    , center(center)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time) {
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
}

Vortex::Vortex(const IntegralLine& boundary, const dvec2& center, double avgRadius,
               double minRadius, double maxRadius, size_t height, size_t time)
    : boundary(boundary.getPositions().size())
    , center(center)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time) {
    const auto& points = boundary.getPositions();
    for (size_t idx = 0; idx < points.size(); ++idx) {
        this->boundary[idx] = {points[idx].x, points[idx].y};
    }
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
}

Vortex::Vortex(const IntegralLine& boundary, double avgRadius, double minRadius, double maxRadius,
               size_t height, size_t time)
    : boundary(boundary.getPositions().size())
    , center(0)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time) {
    const auto& points = boundary.getPositions();
    for (size_t idx = 0; idx < points.size(); ++idx) {
        this->boundary[idx] = {points[idx].x, points[idx].y};
        center += this->boundary[idx];
    }
    center /= points.size();
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
}

void Vortex::setRadii() {
    avgRadius = 0;
    minRadius = 0;
    maxRadius = 0;
    for (const dvec2& point : boundary) {
        double radius = glm::distance2(point, center);
        avgRadius += radius;
        std::min(minRadius, radius);
        std::max(maxRadius, radius);
    }
    avgRadius /= boundary.size();
}

bool Vortex::containsPoint(const dvec2& point) const {
    double centerDist = glm::distance2(center, point);
    if (centerDist < minRadius) return true;
    if (centerDist > maxRadius) return false;

    // Negative direction from the initial radius.
    dvec2 initDir = glm::normalize(center - boundary[0]);
    dvec2 pointDir = glm::normalize(point - center);
    double sign = glm::orientedAngle(initDir, glm::normalize(boundary[1] - center));

    double pointAngle = glm::orientedAngle(initDir, pointDir);
    // If between the first two points, compare directly,
    // since the search starts with the second point.
    if (pointAngle * sign > 0 && abs(pointAngle) < M_PI && abs(pointAngle) > abs(sign)) {
        return glm::distance2(center, boundary[1]) > centerDist;
    }
    sign = -sign;  //(sign > 0) ? -1 : 1;
    // Brute force - maybe make binary search.
    for (size_t idx = 1; idx < boundary.size(); ++idx) {
        double boundAngle = glm::orientedAngle(initDir, glm::normalize(boundary[idx] - center));
        if (boundAngle * sign > pointAngle * sign) {
            return glm::distance2(center, boundary[idx]) > centerDist;
        }
    }
    // Did not fulfill a circle? Just take last point.
    // std::cout << "Noooo, point angle " << pointAngle << " not between " << sign << " < "
    //           << glm::orientedAngle(initDir, glm::normalize(boundary.back() - center)) <<
    //           std::endl;

    return glm::distance2(center, boundary.back()) > centerDist;
}

// ~~~~~~~ Vortex Set ~~~~~~~ //

VortexSet::VortexSet(const mat4& modelMat, const mat4& worldMat)
    : timeRange_(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min())
    , heightRange_(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min())
    , modelMatrix_(modelMat)
    , worldMatrix_(worldMat) {
    vortexOffsets.push_back(0);
    vortexOffsets.push_back(0);
}

VortexSet::VortexSet(const IntegralLineSet& lineSet)
    : timeRange_(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min())
    , heightRange_(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min())
    , modelMatrix_(lineSet.getModelMatrix())
    , worldMatrix_(lineSet.getWorldMatrix()) {
    vortexOffsets.push_back(0);
    vortexOffsets.push_back(0);
}

void VortexSet::push_back(const Vortex& vort) {
    vortices.push_back(vort);
    vortexOffsets.back()++;
    updateRanges();
}
void VortexSet::push_back(Vortex&& vort) {
    vortices.push_back(std::move(vort));
    vortexOffsets.back()++;
    updateRanges();
}
void VortexSet::append(const std::vector<Vortex>& vorts) {
    vortices.insert(vortices.end(), vorts.begin(), vorts.end());
    vortexOffsets.back() += vorts.size();
    updateRanges(vorts.size());
}
void VortexSet::append(const VortexSet& vorts) {
    vortices.insert(vortices.end(), vorts.begin(), vorts.end());
    vortexOffsets.reserve(vortexOffsets.size() + vorts.size());

    size_t sizeBefore = vortexOffsets.back();
    for (size_t idx = 1; idx < vorts.size(); ++idx) {
        vortexOffsets.push_back(sizeBefore + vorts.vortexOffsets[idx]);
    }
    updateRanges(vorts.size());
}
void VortexSet::append(const IntegralLineSet& vorts, size_t height, size_t time) {
    for (size_t idx = 0; idx < vorts.size(); ++idx) {
        vortices.push_back({vorts[idx], -1, -1, -1, height, time});
    }
    updateRanges(vorts.size());
}

Vortex VortexSet::pop_back() {
    Vortex pop(std::move(vortices.back()));
    vortices.pop_back();
    vortexOffsets.back()--;
    return pop;
}

Vortex& VortexSet::back() { return vortices.back(); }

const Vortex& VortexSet::back() const { return vortices.back(); }

void VortexSet::startNewGroup() { vortexOffsets.push_back(vortexOffsets.back()); }

void VortexSet::mergeLastGroups() {
    vortexOffsets.pop_back();
    vortexOffsets.back() = vortices.size();
}

Vortex& VortexSet::operator[](size_t idx) { return vortices[idx]; }
const Vortex& VortexSet::operator[](size_t idx) const { return vortices[idx]; }
size_t VortexSet::size() const { return vortexOffsets.size() - 1; }
size_t VortexSet::sizeGroup(size_t idx) const {
    return vortexOffsets[idx + 1] - vortexOffsets[idx];
}
size_t VortexSet::numGroups() const { return vortexOffsets.size() - 1; }

std::vector<Vortex>::iterator VortexSet::begin() { return vortices.begin(); }
std::vector<Vortex>::iterator VortexSet::end() { return vortices.end(); }
std::vector<Vortex>::const_iterator VortexSet::begin() const { return vortices.begin(); }
std::vector<Vortex>::const_iterator VortexSet::end() const { return vortices.end(); }

std::vector<Vortex>::iterator VortexSet::beginGroup(size_t idx) {
    return vortices.begin() + vortexOffsets[idx];
}

std::vector<Vortex>::iterator VortexSet::endGroup(size_t idx) {
    return vortices.begin() + vortexOffsets[idx + 1];
}

std::vector<Vortex>::const_iterator VortexSet::beginGroup(size_t idx) const {
    return vortices.begin() + vortexOffsets[idx];
}

std::vector<Vortex>::const_iterator VortexSet::endGroup(size_t idx) const {
    return vortices.begin() + vortexOffsets[idx + 1];
}

void VortexSet::updateRanges(size_t countFromEnd) {
    if (countFromEnd == 0 || vortices.size() < countFromEnd || vortices.size() == 0 ||
        vortices.end() - countFromEnd < vortices.begin()) {
        std::cout << "PROBLEM: " << countFromEnd << " from size " << vortices.size();
        LogWarn("PROBLEM: " << countFromEnd << " from size " << vortices.size());
        return;
    }
    for (auto it = vortices.rbegin(); it != vortices.rbegin() + countFromEnd; ++it) {
        // if ((void*)it == (void*)0xffffffffffffffc0) {
        //     std::cout << "PROBLEM: " << countFromEnd << " from size " << vortices.size();
        //     LogWarn("PROBLEM: " << countFromEnd << " from size " << vortices.size());
        //     break;
        // }
        timeRange_.x = std::min(timeRange_.x, it->timeSlice);
        timeRange_.y = std::max(timeRange_.y, it->timeSlice);
        heightRange_.x = std::min(heightRange_.x, it->heightSlice);
        heightRange_.y = std::max(heightRange_.y, it->heightSlice);
    }
}

}  // namespace inviwo