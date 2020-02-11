/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2020 Inviwo Foundation
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

#include <inviwo/nanovgpy/processors/nanovgpyprocessor.h>

#include <inviwo/nanovgutils/nanovgcontext.h>
#include <inviwo/nanovgutils/nanovgutils.h>

#include <modules/opengl/texture/textureutils.h>

namespace inviwo {

namespace detail {
std::string defaultSource() {
    return "import inviwopy\nfrom inviwopy import nvg\nfrom inviwopy.glm import *\n\n\n\n";
}
}  // namespace detail

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo NanoVGPyProcessor::processorInfo_{
    "org.inviwo.NanoVGPyProcessor",  // Class identifier
    "Nano VGPy Processor",           // Display name
    "Nano VG",                       // Category
    CodeState::Experimental,         // Code state
    "Python",                        // Tags
};
const ProcessorInfo NanoVGPyProcessor::getProcessorInfo() const { return processorInfo_; }

NanoVGPyProcessor::NanoVGPyProcessor()
    : Processor()
    , background_("background")
    , inVec2_("inVec2")
    , inVec3_("inVec3")
    , inVec4_("inVec4")
    , inVols_("inVols")
    , inMeshes_("inMeshes")
    , inImgs_("inImgs")
    , outport_("outport")
    , src_("src", "Source", detail::defaultSource()) {

    addPort(background_);
    addPort(inVec2_);
    addPort(inVec3_);
    addPort(inVec4_);
    addPort(inVols_);
    addPort(inMeshes_);
    addPort(inImgs_);
    addPort(outport_);
    addProperty(src_);

    background_.setOptional(true);
    inVec2_.setOptional(true);
    inVec3_.setOptional(true);
    inVec4_.setOptional(true);
    inVols_.setOptional(true);
    inMeshes_.setOptional(true);
    inImgs_.setOptional(true);

    src_.setSemantics(PropertySemantics::PythonEditor);

    src_.onChange([this]() { script_.setSource(src_.get()); });
    script_.setSource(src_.get());
}

void NanoVGPyProcessor::process() {
    auto& nvg = nanovgutil::getContext();
    utilgl::activateTargetAndClearOrCopySource(outport_, background_, ImageType::ColorOnly);
    auto dims = outport_.getDimensions();
    nvg.activate(dims);

    std::unordered_map<std::string, pybind11::object> vars = {
        {"ctx", pybind11::cast(nvg, pybind11::return_value_policy::reference)},
        {"dims", pybind11::cast(dims)}};

    auto createVars = [&vars](auto& port, std::string name) {
        if (!port.isConnected()) return;
        size_t i = 0;
        for (auto d : port) {
            std::ostringstream oss;
            oss << name << i++;
            vars[oss.str()] = pybind11::cast(d.get(), pybind11::return_value_policy::reference);
        }
    };

    createVars(inVec2_, "vec2s");
    createVars(inVec3_, "vec3s");
    createVars(inVec4_, "vec4s");
    createVars(inVols_, "vol");
    createVars(inMeshes_, "mesh");
    createVars(inImgs_, "img");

    script_.run(vars);

    nvg.deactivate();

    utilgl::deactivateCurrentTarget();
}

}  // namespace inviwo
