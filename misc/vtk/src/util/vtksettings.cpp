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

#include <inviwo/vtk/util/vtksettings.h>

namespace inviwo {

VTKSettings::VTKSettings()
    : Settings("VTK Settings")
    , globalWarningDisplay{"globalWarningDisplay", "Global Warning Display ", true}
    , verbosity{"verbosity",
                "Log Verbosity",
                {{"OFF", "OFF", vtkLogger::VERBOSITY_OFF},
                 {"ERROR", "ERROR", vtkLogger::VERBOSITY_ERROR},
                 {"WARNING", "WARNING", vtkLogger::VERBOSITY_WARNING},
                 {"INFO", "INFO", vtkLogger::VERBOSITY_INFO},
                 {"0", "0", vtkLogger::VERBOSITY_0},
                 {"1", "1", vtkLogger::VERBOSITY_1},
                 {"2", "2", vtkLogger::VERBOSITY_2},
                 {"3", "3", vtkLogger::VERBOSITY_3},
                 {"4", "4", vtkLogger::VERBOSITY_4},
                 {"5", "5", vtkLogger::VERBOSITY_5},
                 {"6", "6", vtkLogger::VERBOSITY_6},
                 {"7", "7", vtkLogger::VERBOSITY_7},
                 {"8", "8", vtkLogger::VERBOSITY_8},
                 {"9", "9", vtkLogger::VERBOSITY_9}},
                3} {

    addProperties(globalWarningDisplay, verbosity);
    load();
}

}  // namespace inviwo
