/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019 Inviwo Foundation
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

#include <modules/tensorvisio/util/vtkoutputlogger.h>

#include <inviwo/core/util/logcentral.h>

#include <warn/push>
#include <warn/ignore/all>
#include <vtkOutputWindow.h>
#include <vtkObjectFactory.h>
#include <warn/pop>

namespace inviwo {

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

vtkStandardNewMacro(InviwoVtkOutputWindow);

void InviwoVtkOutputWindow::DisplayText(const char* msg) { LogInfoCustom("VTK", msg); }

void InviwoVtkOutputWindow::DisplayErrorText(const char* msg) { LogErrorCustom("VTK (error)", msg); }

void InviwoVtkOutputWindow::DisplayWarningText(const char* msg) { LogWarnCustom("VTK (warn)", msg); }

void InviwoVtkOutputWindow::DisplayGenericWarningText(const char* msg) {
    LogWarnCustom("VTK (generic)", msg);
}

void InviwoVtkOutputWindow::DisplayDebugText(const char* msg) { LogInfoCustom("VTK (debug)", msg); }

VtkOutputLogger::VtkOutputLogger() : outputWindow_{vtkSmartPointer<InviwoVtkOutputWindow>::New()} {
    vtkOutputWindow::SetInstance(outputWindow_);
}

}  // namespace inviwo
