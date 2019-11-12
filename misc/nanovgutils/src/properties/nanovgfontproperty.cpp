#include <inviwo/nanovgutils/properties/nanovgfontproperty.h>
#include <inviwo/nanovgutils/nanovgutils.h>
#include <modules/fontrendering/util/fontutils.h>

namespace inviwo {
const std::string NanoVGFontProperty::classIdentifier{"org.inviwo.NanoVGFontProperty"};
std::string NanoVGFontProperty::getClassIdentifier() const { return classIdentifier; }

NanoVGFontProperty::NanoVGFontProperty(std::string identifier, std::string displayName)
    : CompositeProperty(identifier, displayName)
    , fontSize_("fontSize", "Font size", 24.0f, 8.0f, 128.0f, 2.0f)
    , fontColor_("fontColor", "Font color", vec4{1.0f}, vec4{0.0f}, vec4{1.0f}, vec4{0.01f},
                 InvalidationLevel::InvalidOutput, PropertySemantics::Color)
    , fontFace_("fontFace", "Font face")
    , fontAlignment_("fontAlignment", "Align",
                     {{"lt", "Left top", NanoVGContext::Alignment::Left_Top},
                      {"lm", "Left middle", NanoVGContext::Alignment::Left_Middle},
                      {"lb", "Left bottom", NanoVGContext::Alignment::Left_Bottom},
                      {"lbl", "Left baseline", NanoVGContext::Alignment::Left_Baseline},
                      {"ct", "Center top", NanoVGContext::Alignment::Center_Top},
                      {"cm", "Center middle", NanoVGContext::Alignment::Center_Middle},
                      {"cb", "Center bottom", NanoVGContext::Alignment::Center_Bottom},
                      {"cbl", "Center baseline", NanoVGContext::Alignment::Center_Baseline},
                      {"rt", "Right top", NanoVGContext::Alignment::Right_Top},
                      {"rm", "Right middle", NanoVGContext::Alignment::Right_Middle},
                      {"rb", "Right bottom", NanoVGContext::Alignment::Right_Bottom},
                      {"rbl", "Right baseline", NanoVGContext::Alignment::Right_Baseline}},
                     5) {
    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());

    auto fonts = util::getAvailableFonts();
    for (auto font : fonts) {
        fontFace_.addOption(font.first, font.first);
    }
    fontFace_.setCurrentStateAsDefault();
}

NanoVGFontProperty::NanoVGFontProperty(const NanoVGFontProperty& rhs)
    : CompositeProperty(rhs)
    , fontSize_(rhs.fontSize_)
    , fontColor_(rhs.fontColor_)
    , fontFace_(rhs.fontFace_)
    , fontAlignment_(rhs.fontAlignment_) {
    util::for_each_in_tuple([&](auto& e) { this->addProperty(e); }, props());
}

void NanoVGFontProperty::set(const float fontSize, const vec4& fontColor,
                             const std::string& fontFace,
                             const NanoVGContext::Alignment fontAlignment) {
    fontSize_.set(fontSize);
    fontColor_.set(fontColor);
    fontFace_.set(fontFace);
    fontAlignment_.set(fontAlignment);
}

}  // namespace inviwo