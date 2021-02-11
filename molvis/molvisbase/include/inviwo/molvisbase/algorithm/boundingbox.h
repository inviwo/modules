/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2020-2021 Inviwo Foundation
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

#include <inviwo/molvisbase/molvisbasemoduledefine.h>
#include <inviwo/molvisbase/ports/molecularstructureport.h>

#include <inviwo/core/util/glm.h>

#include <functional>
#include <optional>

namespace inviwo {

class Inport;

namespace molvis {

class MolecularStructure;

/**
 * Calculate a bounding box of the molecular \p structure using the atom positions and van der Waals
 * radii. The bounding box is represented using a mat4, where all positions are between
 * `bbox * (x,y,z,1)` where x, y, and z are between 0 and 1.
 */
IVW_MODULE_MOLVISBASE_API mat4 boundingBox(const MolecularStructure& structure);

/**
 * Calculate a bounding box of all molecular \p structures using the atom positions and van der
 * Waals radii. The bounding box is represented using a mat4, where all positions are between
 * `bbox * (x,y,z,1)` where x, y, and z are between 0 and 1.
 */
IVW_MODULE_MOLVISBASE_API mat4
boundingBox(const std::vector<std::shared_ptr<const MolecularStructure>>& structures);

/**
 * Constructs a function that returns the bounding box of the molecular structure in the port. If
 * the port is empty the function should return std::nullopt;
 * Note that the port has to outlive the functor.
 */
/**@{*/
IVW_MODULE_MOLVISBASE_API std::function<std::optional<mat4>()> boundingBox(
    const MolecularStructureInport& structure);
IVW_MODULE_MOLVISBASE_API std::function<std::optional<mat4>()> boundingBox(
    const MolecularStructureMultiInport& structures);
IVW_MODULE_MOLVISBASE_API std::function<std::optional<mat4>()> boundingBox(
    const MolecularStructureFlatMultiInport& structures);
IVW_MODULE_MOLVISBASE_API std::function<std::optional<mat4>()> boundingBox(
    const MolecularStructureOutport& structure);
/**@*/

}  // namespace molvis

}  // namespace inviwo
