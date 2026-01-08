/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2018-2026 Inviwo Foundation
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

#include <inviwo/nanovgpy/interface/nanovgpyinterface.h>
#include <inviwo/nanovgutils/nanovgutils.h>

namespace py = pybind11;

namespace inviwo {
namespace nanopy {
void init(py::module ivwmodule, InviwoApplication* app) {
    auto m = ivwmodule.def_submodule("nvg", "NanoVG Python Interface");
    m.def(
        "getContext", [app]() -> auto& { return nanovgutil::getContext(app); },
        py::return_value_policy::reference);

    py::enum_<NanoVGContext::LineCapMode>(m, "ConnectivityType")
        .value("Butt", NanoVGContext::LineCapMode::Butt)
        .value("Round", NanoVGContext::LineCapMode::Round)
        .value("Square", NanoVGContext::LineCapMode::Square);

    py::enum_<NanoVGContext::LineJoinMode>(m, "LineJoinMode")
        .value("Miter", NanoVGContext::LineJoinMode::Miter)
        .value("Round", NanoVGContext::LineJoinMode::Round)
        .value("Bevel", NanoVGContext::LineJoinMode::Bevel);

    py::enum_<NanoVGContext::Winding>(m, "Winding")
        .value("CCW", NanoVGContext::Winding::CCW)
        .value("CW", NanoVGContext::Winding::CW);

    py::enum_<NanoVGContext::Alignment>(m, "Alignment")
        .value("Left_Top", NanoVGContext::Alignment::Left_Top)
        .value("Left_Middle", NanoVGContext::Alignment::Left_Middle)
        .value("Left_Bottom", NanoVGContext::Alignment::Left_Bottom)
        .value("Left_Baseline", NanoVGContext::Alignment::Left_Baseline)
        .value("Center_Top", NanoVGContext::Alignment::Center_Top)
        .value("Center_Middle", NanoVGContext::Alignment::Center_Middle)
        .value("Center_Bottom", NanoVGContext::Alignment::Center_Bottom)
        .value("Center_Baseline", NanoVGContext::Alignment::Center_Baseline)
        .value("Right_Top", NanoVGContext::Alignment::Right_Top)
        .value("Right_Middle", NanoVGContext::Alignment::Right_Middle)
        .value("Right_Bottom", NanoVGContext::Alignment::Right_Bottom)
        .value("Right_Baseline", NanoVGContext::Alignment::Right_Baseline);

    py::classh<NanoVGContext> nanoVGContext(m, "NanoVGContext");
    nanoVGContext.def(
        "activate",
        [](NanoVGContext* self, int windowWidth, int windowHeight, float pixelRatio) {
            self->activate(windowWidth, windowHeight, pixelRatio);
        },
        py::arg("windowWidth"), py::arg("windowHeight"), py::arg("pixelRatio") = 1.0f);
    nanoVGContext.def(
        "activate",
        [](NanoVGContext* self, size2_t dimensions, float pixelRatio) {
            self->activate(dimensions, pixelRatio);
        },
        py::arg("dimensions"), py::arg("pixelRatio") = 1.0f);

    nanoVGContext.def("deactivate", &NanoVGContext::deactivate);
    nanoVGContext.def("cancel", &NanoVGContext::cancel);

    nanoVGContext.def("save", &NanoVGContext::save);
    nanoVGContext.def("restore", &NanoVGContext::restore);
    nanoVGContext.def("reset", &NanoVGContext::reset);

    nanoVGContext.def("shapeAntiAlias", &NanoVGContext::shapeAntiAlias, py::arg("enabled"));
    nanoVGContext.def("enableAntiAlias", &NanoVGContext::enableAntiAlias);
    nanoVGContext.def("disableAntiAlias", &NanoVGContext::disableAntiAlias);

    nanoVGContext.def("beginPath", &NanoVGContext::beginPath);
    nanoVGContext.def("closePath", &NanoVGContext::closePath);

    nanoVGContext.def("pathWinding", &NanoVGContext::pathWinding, py::arg("winding_direction"));

    nanoVGContext.def("lineCap", &NanoVGContext::lineCap, py::arg("line_cap_mode"));
    nanoVGContext.def("lineJoin", &NanoVGContext::lineJoin, py::arg("line_join_mode"));

    nanoVGContext.def("moveTo", &NanoVGContext::moveTo, py::arg("coordinates"));
    nanoVGContext.def("lineTo", &NanoVGContext::lineTo, py::arg("coordinates"));

    nanoVGContext.def("circle", &NanoVGContext::circle, py::arg("coordinates"), py::arg("radius"));
    nanoVGContext.def("ellipse", &NanoVGContext::ellipse, py::arg("coordinates"),
                      py::arg("radiusA"), py::arg("radiusB"));
    nanoVGContext.def("rect", &NanoVGContext::rect, py::arg("coordinates"), py::arg("size"));
    nanoVGContext.def("roundedRect", &NanoVGContext::roundedRect, py::arg("coordinates"),
                      py::arg("size"), py::arg("radius"));

    nanoVGContext.def("arc", &NanoVGContext::arc, py::arg("center"), py::arg("radius"),
                      py::arg("angle1"), py::arg("angle2"), py::arg("direction"));

    nanoVGContext.def("arcTo", &NanoVGContext::arcTo, py::arg("point1"), py::arg("point2"),
                      py::arg("radius"));

    nanoVGContext.def("bezierCurveTo", &NanoVGContext::bezierCurveTo, py::arg("controlPoint1"),
                      py::arg("controlPoint2"), py::arg("end"));

    nanoVGContext.def("quadraticCurveTo", &NanoVGContext::quadraticCurveTo, py::arg("controlPoint"),
                      py::arg("end"));

    nanoVGContext.def("fillColor", &NanoVGContext::fillColor, py::arg("color"));
    nanoVGContext.def("fill", &NanoVGContext::fill);

    nanoVGContext.def("strokeColor", &NanoVGContext::strokeColor, py::arg("color"));
    nanoVGContext.def("strokeWidth", &NanoVGContext::strokeWidth, py::arg("width"));
    nanoVGContext.def("stroke", &NanoVGContext::stroke);

    nanoVGContext.def("fontSize", &NanoVGContext::fontSize, py::arg("size"));

    nanoVGContext.def("fontFace", &NanoVGContext::fontFace, py::arg("name"));

    nanoVGContext.def(
        "textAlign", [](NanoVGContext* self, int flags) { self->textAlign(flags); },
        py::arg("flags"));
    nanoVGContext.def(
        "textAlign",
        [](NanoVGContext* self, NanoVGContext::Alignment flags) { self->textAlign(flags); },
        py::arg("flags"));
    nanoVGContext.def("fontBlur", &NanoVGContext::fontBlur, py::arg("blur"));

    nanoVGContext.def("text", &NanoVGContext::text, py::arg("coordinates"), py::arg("text"),
                      py::arg("text_cutoff") = 0);
    nanoVGContext.def("textBox", &NanoVGContext::textBox, py::arg("coordinates"),
                      py::arg("textBoxWidth"), py::arg("text"), py::arg("text_cutoff") = 0);
    nanoVGContext.def("createFont", &NanoVGContext::createFont, py::arg("name"),
                      py::arg("filename"));

    nanoVGContext.def(
        "textBounds",
        py::overload_cast<const ivec2&, const std::string&>(&NanoVGContext::textBounds),
        py::arg("position"), py::arg("string"));
    nanoVGContext.def("textBoxBounds", &NanoVGContext::textBoxBounds, py::arg("position"),
                      py::arg("textBoxWidth"), py::arg("string"));

    nanoVGContext.def("textMetrics", &NanoVGContext::textMetrics);
    nanoVGContext.def("textMetricsAscender", &NanoVGContext::textMetricsAscender);
    nanoVGContext.def("textMetricsDescender", &NanoVGContext::textMetricsDescender);
    nanoVGContext.def("textMetricsLineHeight", &NanoVGContext::textMetricsLineHeight);

    nanoVGContext.def("findFont", &NanoVGContext::findFont, py::arg("name"));

    nanoVGContext.def("transform", &NanoVGContext::transform, py::arg("a"), py::arg("b"),
                      py::arg("c"), py::arg("d"), py::arg("e"), py::arg("f"));
    nanoVGContext.def("translate", &NanoVGContext::translate, py::arg("dx"), py::arg("dy"));
    nanoVGContext.def("rotate", &NanoVGContext::rotate, py::arg("radians"));
    nanoVGContext.def("scale", &NanoVGContext::scale, py::arg("sx"), py::arg("sy"));
    nanoVGContext.def("skewX", &NanoVGContext::skewX, py::arg("kx"));
    nanoVGContext.def("skewY", &NanoVGContext::skewY, py::arg("ky"));

    nanoVGContext.def("arrow", &nanovgutil::arrow, py::arg("from"), py::arg("to"),
                      py::arg("width") = 30, py::arg("headSize") = 30, py::arg("normalize") = true);
}

}  // namespace nanopy
}  // namespace inviwo
