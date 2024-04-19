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

#pragma once

#include <inviwo/ttk/ttkmoduledefine.h>

#include <optional>
#include <string>
#include <functional>

class vtkInformation;
class vtkAlgorithm;

namespace inviwo {

namespace ttk {

/**
 * To be used in conjunction with VTKGenericProcessor. If the VTKTraits struct has a member
 * <tt>outportDataTypeFunc</tt> of this type, the function will be used to infer the type of the
 * nth outport.
 * This is necessary to account for TTK's <tt>ttkAlgorithm::SAME_DATA_TYPE_AS_INPUT_PORT()</tt>.
 *
 * \code{.cpp} template <> struct VTKTraits<ttkDistanceField> {
 *     ...
 *     ttk::OutportDataTypeFunc outportDataTypeFunc = ttk::getOutportDataType;
 *     ...
 * };
 * \endcode
 *
 * \see getOutportDataType
 */
using OutportDataTypeFunc = std::function<std::optional<std::string>(vtkAlgorithm*, int)>;

/**
 * Infer the data type of outport \p portNumber of the VTK filter \p filter. This function allows
 * for the outport having the same data type as its corresponding inport as defined by
 * <tt>ttkAlgorithm::SAME_DATA_TYPE_AS_INPUT_PORT()</tt>.
 *
 */
IVW_MODULE_TTK_API std::optional<std::string> getOutportDataType(vtkAlgorithm* filter,
                                                                 int portNumber);

}  // namespace ttk

}  // namespace inviwo
