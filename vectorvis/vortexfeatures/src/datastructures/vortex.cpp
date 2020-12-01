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
    , timeSlice(vort.timeSlice)
    , rotation(vort.rotation)
    , score(vort.score) {}

Vortex::Vortex(Vortex&& vort)
    : boundary(std::move(vort.boundary))
    , center(vort.center)
    , avgRadius(vort.avgRadius)
    , minRadius(vort.minRadius)
    , maxRadius(vort.maxRadius)
    , heightSlice(vort.heightSlice)
    , timeSlice(vort.timeSlice)
    , rotation(vort.rotation)
    , score(vort.score) {}

Vortex& Vortex::operator=(Vortex&& vort) {
    if (&vort == this) return *this;
    boundary = std::move(vort.boundary);
    center = vort.center;
    avgRadius = vort.avgRadius;
    minRadius = vort.minRadius;
    maxRadius = vort.maxRadius;
    heightSlice = vort.heightSlice;
    timeSlice = vort.timeSlice;
    rotation = vort.rotation;
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
    rotation = vort.rotation;
    score = vort.score;
    return *this;
}

Vortex::Vortex(const std::vector<dvec2>& boundary, const dvec2& center, double avgRadius,
               double minRadius, double maxRadius, size_t height, size_t time, Turning rotation)
    : boundary(boundary)
    , center(center)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time)
    , rotation(rotation)
    , score(-1.0) {
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
    if (rotation == Turning::Unknown) setTurning();
}

Vortex::Vortex(std::vector<dvec2>&& boundary, const dvec2& center, double avgRadius,
               double minRadius, double maxRadius, size_t height, size_t time, Turning rotation)
    : boundary(std::move(boundary))
    , center(center)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time)
    , rotation(rotation)
    , score(-1.0) {
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
    if (rotation == Turning::Unknown) setTurning();
}

Vortex::Vortex(const IntegralLine& boundary, const dvec2& center, double avgRadius,
               double minRadius, double maxRadius, size_t height, size_t time, Turning rotation)
    : boundary(boundary.getPositions().size())
    , center(center)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time)
    , rotation(rotation)
    , score(-1.0) {
    const auto& points = boundary.getPositions();
    for (size_t idx = 0; idx < points.size(); ++idx) {
        this->boundary[idx] = {points[idx].x, points[idx].y};
    }
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
    if (rotation == Turning::Unknown) setTurning();
}

Vortex::Vortex(const IntegralLine& boundary, double avgRadius, double minRadius, double maxRadius,
               size_t height, size_t time, Turning rotation)
    : boundary(boundary.getPositions().size())
    , center(0)
    , avgRadius(avgRadius)
    , minRadius(minRadius)
    , maxRadius(maxRadius)
    , heightSlice(height)
    , timeSlice(time)
    , rotation(rotation)
    , score(-1.0) {
    const auto& points = boundary.getPositions();
    for (size_t idx = 0; idx < points.size(); ++idx) {
        this->boundary[idx] = {points[idx].x, points[idx].y};
        center += this->boundary[idx];
    }
    center /= points.size();
    if (avgRadius < 0 || minRadius < 0 || maxRadius < 0) setRadii();
    if (rotation == Turning::Unknown) setTurning();
}

bool Vortex::roundUpBoundary(double boundaryPart) {
    if (size() < 3) return false;
    // Close the circle water tight.
    dvec2 startVec = glm::normalize(boundary[0] - center);
    double endAngle = glm::orientedAngle(startVec, glm::normalize(boundary.back() - center));
    // Is there a gap between the beginning and the end?
    bool openCircle = ((endAngle > 0) == (rotation == Turning::Clockwise));
    if (!openCircle) {
        for (size_t idx = size() - 1; idx > 2; --idx) {
            endAngle = glm::orientedAngle(startVec, glm::normalize(boundary[idx] - center));
            openCircle = ((endAngle > 0) == (rotation == Turning::Clockwise));
            if (openCircle) {
                boundary.resize(idx);
                break;
            }
        }
    }
    if (!openCircle) {
        // std::cout << "Didn't get closure." << std::endl;
        return false;
    }
    // Round off the end with a smooth step.
    size_t numPointsBound = 0.5 + boundaryPart * size();
    double initOffset = glm::distance(boundary[0], center) - glm::distance(boundary.back(), center);

    size_t tmpSize = size();
    for (size_t point = tmpSize - numPointsBound + 1; point < tmpSize; ++point) {
        double offsetScale = glm::smoothstep<double>(tmpSize - numPointsBound, tmpSize - 1, point);
        boundary[point] =
            boundary[point] + (glm::normalize(boundary[point] - center) * initOffset * offsetScale);
    }
    boundary.push_back(boundary[0]);
    return true;
}

void Vortex::setRadii() {
    avgRadius = 0;
    minRadius = 0;
    maxRadius = 0;
    for (const dvec2& point : boundary) {
        double radius = glm::distance(point, center);
        avgRadius += radius;
        std::min(minRadius, radius);
        std::max(maxRadius, radius);
    }
    avgRadius /= boundary.size();
}

void Vortex::setTurning() {
    if (size() < 3) {
        rotation = Turning::Unknown;
        return;
    }
    dvec2 deciders[3] = {boundary[0], boundary[size() / 3], boundary[(size() * 2) / 3]};
    double angle = glm::orientedAngle(glm::normalize(deciders[0] - deciders[1]),
                                      glm::normalize(deciders[2] - deciders[1]));
    rotation = (angle > 0) ? Turning::Clockwise : Turning::CounterClockwise;
    if (angle == 0) rotation = Turning::Unknown;
}

namespace detail {
bool pointInsideTriangle(const dvec2& point, const dvec2& t0, const dvec2& t1, const dvec2& t2) {

    // Compute vectors
    dvec2 b = t1 - t0;
    dvec2 c = t2 - t0;
    dvec2 p = point - t0;

    // Compute dot products
    double cc = glm::dot(c, c);
    double bc = glm::dot(b, c);
    double pc = glm::dot(c, p);
    double bb = glm::dot(b, b);
    double pb = glm::dot(b, p);

    // Compute barycentric coordinates
    double denom = cc * bb - bc * bc;
    double u = (bb * pc - bc * pb) / denom;
    double v = (cc * pb - bc * pc) / denom;

    return u > 0 && v > 0 && u + v < 1;
}
}  // namespace detail

bool Vortex::containsPoint(const dvec2& point) const {
    double centerDist = glm::distance(center, point);
    if (centerDist < minRadius) return true;
    if (centerDist > maxRadius) return false;

    dvec2 pc = point - center;
    for (size_t tri = 1; tri < size(); ++tri) {
        const dvec2& a = boundary[tri - 1];
        const dvec2& b = boundary[tri];
        if (glm::dot(pc, a - center) * glm::dot(pc, b - center) < 0) {
            return detail::pointInsideTriangle(point, a, b, center);
        }
    }
    return false;

    // // Negative direction from the initial radius.
    // dvec2 initDir = glm::normalize(center - boundary[0]);
    // dvec2 pointDir = glm::normalize(point - center);
    // double sign = glm::orientedAngle(initDir, glm::normalize(boundary[1] - center));

    // double pointAngle = glm::orientedAngle(initDir, pointDir);
    // // If between the first two points, compare directly,
    // // since the search starts with the second point.
    // if (pointAngle * sign > 0 && abs(pointAngle) < M_PI && abs(pointAngle) > abs(sign)) {
    //     return glm::distance(center, boundary[1]) > centerDist;
    // }
    // sign = -sign;  //(sign > 0) ? -1 : 1;
    // // Brute force - maybe make binary search.
    // for (size_t idx = 1; idx < boundary.size(); ++idx) {
    //     double boundAngle = glm::orientedAngle(initDir, glm::normalize(boundary[idx] - center));
    //     if (boundAngle * sign > pointAngle * sign) {
    //         return glm::distance(center, boundary[idx]) > centerDist;
    //     }
    // }

    // return glm::distance(center, boundary.back()) > centerDist;
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

VortexSet::VortexSet(const mat4& modelMat, const mat4& worldMat, std::vector<Vortex>&& vortices)
    : vortices(std::move(vortices))
    , timeRange_(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min())
    , heightRange_(std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::min())
    , modelMatrix_(modelMat)
    , worldMatrix_(worldMat) {
    vortexOffsets.push_back(0);
    vortexOffsets.push_back(vortices.size());
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

void VortexSet::clear() {
    vortices.clear();
    vortexOffsets.clear();
    vortexOffsets.push_back(0);
    vortexOffsets.push_back(0);
}

void VortexSet::startNewGroup() { vortexOffsets.push_back(vortexOffsets.back()); }

void VortexSet::mergeLastGroups() {
    vortexOffsets.pop_back();
    vortexOffsets.back() = vortices.size();
}

Vortex& VortexSet::operator[](size_t idx) { return vortices[idx]; }
const Vortex& VortexSet::operator[](size_t idx) const { return vortices[idx]; }
size_t VortexSet::size() const { return vortices.size(); }
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
    if (size() == 0) return;
    if (countFromEnd == 0 || vortices.size() < countFromEnd || vortices.size() == 0 ||
        vortices.end() - countFromEnd < vortices.begin()) {
        std::cout << "PROBLEM: " << countFromEnd << " from size " << vortices.size();
        LogWarn("PROBLEM: " << countFromEnd << " from size " << vortices.size());
        return;
    }
    if (countFromEnd == size()) {
        timeRange_ = {vortices[0].timeSlice, vortices[0].timeSlice};
        heightRange_ = {vortices[0].heightSlice, vortices[0].heightSlice};
        scoreRange_ = {vortices[0].score, vortices[0].score};
    }
    for (auto it = vortices.rbegin(); it != vortices.rbegin() + countFromEnd; ++it) {
        timeRange_.x = std::min(timeRange_.x, it->timeSlice);
        timeRange_.y = std::max(timeRange_.y, it->timeSlice);
        heightRange_.x = std::min(heightRange_.x, it->heightSlice);
        heightRange_.y = std::max(heightRange_.y, it->heightSlice);

        scoreRange_.x = std::min(scoreRange_.x, it->score);
        scoreRange_.y = std::max(scoreRange_.y, it->score);
    }
}

void VortexSet::updateScoreRange(size_t countFromEnd) const {
    if (size() == 0) return;
    if (countFromEnd == size()) {
        scoreRange_ = {vortices[0].score, vortices[0].score};
    }
    for (auto it = vortices.rbegin(); it != vortices.rbegin() + countFromEnd; ++it) {
        scoreRange_.x = std::min(scoreRange_.x, it->score);
        scoreRange_.y = std::max(scoreRange_.y, it->score);
    }
}

void VortexSet::updateParents() const {
    parentVortices_.resize(size());
    std::fill(parentVortices_.begin(), parentVortices_.end(), -1);

    for (int groupIdx = 1; groupIdx < static_cast<int>(parentVortices_.size()); ++groupIdx) {
        const Vortex* lastRefVortex = nullptr;  //, *refVortex;
        // int lastParentIdx = -1;
        for (auto refVortex = beginGroup(groupIdx) + 1; refVortex != endGroup(groupIdx);
             ++refVortex) {
            // Same time and height as a vortex already processed? Just copy parent index.
            if (lastRefVortex && lastRefVortex->timeSlice == refVortex->timeSlice &&
                lastRefVortex->heightSlice == refVortex->heightSlice) {
                parentVortices_[refVortex - begin()] = parentVortices_[refVortex - begin() - 1];
                continue;
            }
            lastRefVortex = refVortex.base();

            // Go through all parent groups in size ascending order, find one containing this
            // vortex.
            for (int parentIdx = groupIdx - 1; parentIdx >= 0; --parentIdx) {
                auto parentIt =
                    std::find_if(beginGroup(parentIdx), endGroup(parentIdx), [&](auto vort) {
                        return vort.heightSlice == refVortex->heightSlice &&
                               vort.timeSlice == refVortex->timeSlice;
                    });
                if (parentIt == endGroup(parentIdx)) {
                    continue;
                }

                // Check whether the potential parent vortex has a larger radius
                // and contains the center of the potential child.
                if (parentIt->avgRadius > refVortex->avgRadius &&
                    parentIt->containsPoint(refVortex->center)) {
                    parentVortices_[refVortex - begin()] = parentIdx;
                    break;
                }
            }
        }
    }
}

int VortexSet::getParentGroup(size_t vortIdx) const {
    if (parentVortices_.size() < size()) updateParents();
    return parentVortices_[vortIdx];
}

}  // namespace inviwo