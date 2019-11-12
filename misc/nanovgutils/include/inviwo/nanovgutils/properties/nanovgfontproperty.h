#pragma once

#include <inviwo/core/common/inviwo.h>
#include <inviwo/nanovgutils/nanovgutilsmoduledefine.h>
#include <inviwo/core/properties/compositeproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/nanovgutils/nanovgcontext.h>

namespace inviwo {
class IVW_MODULE_NANOVGUTILS_API NanoVGFontProperty : public CompositeProperty {
public:
    virtual std::string getClassIdentifier() const override;
    static const std::string classIdentifier;

    NanoVGFontProperty() = delete;
    NanoVGFontProperty(std::string identifier, std::string displayName);
    NanoVGFontProperty(const NanoVGFontProperty& rhs);
    virtual NanoVGFontProperty* clone() const override { return new NanoVGFontProperty(*this); }
    virtual ~NanoVGFontProperty() = default;

    using CompositeProperty::set;
    void set(int fontSize, const vec4& fontColor, const std::string& fontFace,
             NanoVGContext::Alignment fontAlignment);
    void set(const int fontSize) { fontSize_.set(fontSize); }
    void set(const vec4& fontColor) { fontColor_.set(fontColor); }
    void set(const std::string& fontFace) { fontFace_.set(fontFace); }
    void set(const NanoVGContext::Alignment fontAlignment) { fontAlignment_.set(fontAlignment); }

    int getFontSize() const { return fontSize_.get(); }
    vec4 getFontColor() const { return fontColor_.get(); }
    std::string getFontFace() const { return fontFace_.get(); }
    NanoVGContext::Alignment getFontAligntment() const { return fontAlignment_.get(); }
    std::tuple<int, vec4, std::string, NanoVGContext::Alignment> get() const {
        return {fontSize_.get(), fontColor_.get(), fontFace_.get(), fontAlignment_.get()};
    }

private:
    IntProperty fontSize_;
    FloatVec4Property fontColor_;
    OptionPropertyString fontFace_;
    TemplateOptionProperty<NanoVGContext::Alignment> fontAlignment_;

    auto props() { return std::tie(fontSize_, fontColor_, fontFace_, fontAlignment_); }
    auto props() const { return std::tie(fontSize_, fontColor_, fontFace_, fontAlignment_); }
};
}  // namespace inviwo
