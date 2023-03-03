/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2023 Inviwo Foundation
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

#include <warn/push>
#include <warn/ignore/all>
#include <vtkSmartPointer.h>
#include <vtkOutputWindow.h>
#include <warn/pop>

namespace inviwo {

/** \class VtkOutputLogger
 * \brief mapping VTK logging output to Inviwo's logcentral instead of the regular VTK output window
 */
class IVW_MODULE_VTK_API VtkOutputLogger {

    class InviwoVtkOutputWindow : public vtkOutputWindow {
    public:
        InviwoVtkOutputWindow() = default;
        virtual ~InviwoVtkOutputWindow() = default;

        static InviwoVtkOutputWindow* New();

        virtual void DisplayText(const char*) override;
        virtual void DisplayErrorText(const char*) override;
        virtual void DisplayWarningText(const char*) override;
        virtual void DisplayGenericWarningText(const char*) override;
        virtual void DisplayDebugText(const char*) override;
    };

public:
    VtkOutputLogger();
    virtual ~VtkOutputLogger() = default;

private:
    vtkSmartPointer<InviwoVtkOutputWindow> outputWindow_;
};

}  // namespace inviwo
