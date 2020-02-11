/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2020 Inviwo Foundation
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
#include <inviwo/vtk/vtkmoduledefine.h>

#include <inviwo/core/common/inviwo.h>
#include <utility>
#include <optional>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkSmartPointer.h>
#include <vtkDataSet.h>
#include <warn/pop>

namespace inviwo {

/**
 * \brief Wrapper class for vtkDataSet pointer
 * Wrapper class for vtkDataSet pointer so we don't have to explicitly take care of memory handling
 * etc.
 */
class IVW_MODULE_VTK_API VTKDataSet {
public:
    VTKDataSet() = delete;
    explicit VTKDataSet(vtkSmartPointer<vtkDataSet> dataSet) : dataSet_(dataSet) {}
    virtual ~VTKDataSet() {}

    vtkSmartPointer<vtkDataSet> operator->() const { return dataSet_; }
    vtkSmartPointer<vtkDataSet> operator*() const { return dataSet_; }

    std::string getDataInfo() const;

    /** Attempts to get the dimensions of the data set. This will only work if the data set
     * is of type rectiliniar grid, structured grid or structured points. Otherwise [0,0,0] will be
     * returned. The method is here because the GetDimensions() method is not declared const in VTK
     * so if you have a VTKDataSet on the inport you will not be able to determine its
     * dimensions unless you clone it...
     * */
    std::optional<size3_t> getDimensions() const;

private:
    vtkSmartPointer<vtkDataSet> dataSet_;

    // Helpers
    inline std::string getHTMLTableColumnString(const std::string& item) const {
        return "  <td style='color:#bbb;padding-right:8px;'>"
               "  <span style=\"white-space: nowrap;\">" +
               item +
               "  </span>"
               "  </td>";
    }

    inline std::string getHTMLTableColumnSpanString(const std::string& item,
                                                    const size_t numberOfColumnsToSpan) const {
        return "  <td colspan=\"" + std::to_string(numberOfColumnsToSpan) +
               "\" style='color:#bbb;padding-right:8px;'>"
               "  <span style=\"white-space: nowrap;\">"
               "<strong>" +
               item +
               "</strong></span>"
               "</td>";
    }

    inline std::string getHTMLTableRowString(const std::string& left,
                                             const std::string& right) const {
        return "<tr>" + getHTMLTableColumnString(left) + getHTMLTableColumnString(right) + "</tr>";
    }

    inline std::string getHTMLTableRowString(const std::string& left, const dvec3& right) const {
        return "<tr>" + getHTMLTableColumnString(left) +
               getHTMLTableColumnString(glm::to_string(right)) + "</tr>";
    }

    inline std::string getHTMLTableRowString(const std::string& left, const size3_t& right) const {
        return "<tr>" + getHTMLTableColumnString(left) +
               getHTMLTableColumnString(glm::to_string(uvec3(right))) + "</tr>";
    }

    inline std::string getHTMLTableRowString(const std::string& left, const size_t right) const {
        return "<tr>" + getHTMLTableColumnString(left) +
               getHTMLTableColumnString(std::to_string(right)) + "</tr>";
    }

    inline std::string getHTMLTableRowString(const std::string& left, const double right) const {
        return "<tr>" + getHTMLTableColumnString(left) +
               getHTMLTableColumnString(std::to_string(right)) + "</tr>";
    }

    inline std::string getHTMLTableIntermediateHeaderString(
        const std::string& header, size_t numberOfColumnsToSpan = 2) const {
        return "<tr>" + getHTMLTableColumnSpanString(header, numberOfColumnsToSpan) + "</tr>";
    }
};

}  // namespace inviwo
