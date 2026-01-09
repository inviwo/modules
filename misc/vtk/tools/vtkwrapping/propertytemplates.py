# -*- coding: utf-8 -*-
# ********************************************************************************
#
# Inviwo - Interactive Visualization Workshop
#
# Copyright (c) 2024-2026 Inviwo Foundation
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# ********************************************************************************

doubleTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    DoubleProperty property{{"{identifier}", "{displayName}", R"({doc})"_help,
                             {defaultValue[0]},
                             std::pair{{{minValue[0]}, ConstraintBehavior::Ignore}},
                             std::pair{{{maxValue[0]}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec2Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
        return true;
    }}
    DoubleVec2Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                                dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
                                std::pair{{dvec2{{{minValue[0]},{minValue[1]}}},
                                          ConstraintBehavior::Ignore}},
                                std::pair{{dvec2{{{maxValue[0]},{maxValue[1]}}},
                                          ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec3Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2));
        return true;
    }}
    DoubleVec3Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                                dvec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                                std::pair{{dvec3{{{minValue[0]},{minValue[1]},{minValue[2]}}},
                                          ConstraintBehavior::Ignore}},
                                std::pair{{dvec3{{{maxValue[0]},{maxValue[1]},{maxValue[2]}}},
                                          ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec4Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2), property.get(3));
        return true;
    }}
    DoubleVec3Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                                dvec4{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}, {defaultValue[3]}}},
                                std::pair{{dvec4{{{minValue[0]}, {minValue[1]}, {minValue[2]}, {minValue[3]}}}, ConstraintBehavior::Ignore}},
                                std::pair{{dvec4{{{maxValue[0]}, {maxValue[1]}, {maxValue[2]}, {maxValue[3]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

doubleVec6Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(x.get(0), x.get(1), y.get(0), y.get(1), z.get(0), z.get(1));
        return true;
    }}
    DoubleVec2Property x{{"x", "X",
        dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
        std::pair{{dvec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};

    DoubleVec2Property y{{"y", "Y",
        dvec2{{{defaultValue[2]}, {defaultValue[3]}}},
        std::pair{{dvec2{{{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};

    DoubleVec2Property z{{"z", "Z",
        dvec2{{{defaultValue[4]}, {defaultValue[5]}}},
        std::pair{{dvec2{{{minValue[4]},{minValue[5]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[4]},{maxValue[5]}}}, ConstraintBehavior::Ignore}}}};

    CompositeProperty property{{[&](){{
        CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
        tmp.addProperties(x,y,z);
        return tmp;
        }}()
    }};

}};
"""


intTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    IntProperty property{{"{identifier}", "{displayName}", R"({doc})"_help,
                         {defaultValue[0]},
                         std::pair{{{minValue[0]}, ConstraintBehavior::Ignore}},
                         std::pair{{{maxValue[0]}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec2Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1));
        return true;
    }}
    IntVec2Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                            ivec2{{{defaultValue[0]}, {defaultValue[1]}}},
                            std::pair{{ivec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec3Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2));
        return true;
    }}
    IntVec3Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                            ivec3{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]}}},
                            std::pair{{ivec3{{{minValue[0]},{minValue[1]},{minValue[2]}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec3{{{maxValue[0]},{maxValue[1]},{maxValue[2]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec4Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get(0), property.get(1), property.get(2), property.get(3));
        return true;
    }}
    IntVec4Property property{{"{identifier}", "{displayName}", R"({doc})"_help,
                            ivec4{{{defaultValue[0]}, {defaultValue[1]}, {defaultValue[2]},{defaultValue[3]}}},
                            std::pair{{ivec4{{{minValue[0]},{minValue[1]},{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
                            std::pair{{ivec4{{{maxValue[0]},{maxValue[1]},{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};
}};
"""

intVec6Template = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(x.get(0), x.get(1), y.get(0), y.get(1), z.get(0), z.get(1));
        return true;
    }}
    IntVec2Property x{{"x", "X",
        dvec2{{{defaultValue[0]}, {defaultValue[1]}}},
        std::pair{{dvec2{{{minValue[0]},{minValue[1]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[0]},{maxValue[1]}}}, ConstraintBehavior::Ignore}}}};

    IntVec2Property y{{"y", "Y",
        dvec2{{{defaultValue[2]}, {defaultValue[3]}}},
        std::pair{{dvec2{{{minValue[2]},{minValue[3]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[2]},{maxValue[3]}}}, ConstraintBehavior::Ignore}}}};

    IntVec2Property z{{"z", "Z",
        dvec2{{{defaultValue[4]}, {defaultValue[5]}}},
        std::pair{{dvec2{{{minValue[4]},{minValue[5]}}}, ConstraintBehavior::Ignore}},
        std::pair{{dvec2{{{maxValue[4]},{maxValue[5]}}}, ConstraintBehavior::Ignore}}}};

    CompositeProperty property{{[&](){{
        CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
        tmp.addProperties(x,y,z);
        return tmp;
        }}()
    }};

}};
"""


boolTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    BoolProperty property{{"{identifier}", "{displayName}", R"({doc})"_help, {defaultValue}}};
}};
"""

stringTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get().c_str());
        return true;
    }}
    StringProperty property{{"{identifier}", "{displayName}", R"({doc})"_help, "{defaultValue}"}};
}};
"""

fileTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        if(property.get().empty()) return false;
        filter.{command}(property.get().string().c_str());
        return true;
    }}
    FileProperty property{{"{identifier}", "{displayName}", R"({doc})"_help,
                           std::filesystem::path{{"{defaultValue}"}}}};
}};
"""

buttonTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}();
        return true;
    }}
    ButtonProperty property{{"{identifier}", "{displayName}", R"({doc})"_help}};
}};
"""

extentTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(x.getStart(), x.getEnd(), y.getStart(), y.getEnd(),
            z.getStart(), z.getEnd());
        return true;
    }

    IntSizeTMinMaxProperty x{{"xExtent", "Y Extent", 0, 100, 0, 1000}};
    IntSizeTMinMaxProperty y{{"yExtent", "Y Extent", 0, 100, 0, 1000}};
    IntSizeTMinMaxProperty z{{"zExtent", "X Extent", 0, 100, 0, 1000}};

    CompositeProperty property{{[&](){{
            CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
            tmp.addProperties(x,y,z);
            return tmp;
        }}()
    }};
}};
"""

optionTemplate = """
struct {structName} {{
    bool set({className}& filter) {{
        filter.{command}(property.get());
        return true;
    }}
    OptionPropertyInt property{{"{identifier}", "{displayName}",
                               R"({doc})"_help, {{{options}}}, {defaultValue}}};
}};
"""

fieldSelectionTemplate = """
struct {structName} : FieldSelection {{
    bool set({className}& filter) {{
        if(name.size() == 0) return false;
        filter.{command}({defaultValue[0]}, {defaultValue[1]},
                         {defaultValue[2]}, fieldAssociation.get(), name.get().c_str());
        return true;
    }}
    OptionPropertyString name{{"name", "Name", {{{defaultoptions}}}, 0}};

    OptionProperty<vtkDataObject::FieldAssociations> fieldAssociation{{"fieldAssociation", "Field Association",
        {{
            {{"points","Points",vtkDataObject::FIELD_ASSOCIATION_POINTS}},
            {{"cells","Cells",vtkDataObject::FIELD_ASSOCIATION_CELLS}},
            {{"none","None",vtkDataObject::FIELD_ASSOCIATION_NONE}},
            {{"pointsThenCells","Points then Cells",vtkDataObject::FIELD_ASSOCIATION_POINTS_THEN_CELLS}}
        }}, {defaultValue[3]} }};

    CompositeProperty property{{[&](){{
            CompositeProperty tmp{{"{identifier}", "{displayName}", R"({doc})"_help}};
            tmp.addProperties(name, fieldAssociation);
            return tmp;
        }}()
    }};

    static constexpr std::string_view inport = "{inport}";
}};
"""
