#include "original_ui_material50kg.h"
#include <utility>

namespace rr::assets {
namespace {
using Field = OriginalUiGsField50KG;
using Error = OriginalUiMaterialError50KG;
struct Spec { const char* name; std::int32_t initial; std::uint32_t templatePC, passPC; };
const Spec specs[] = {
#include "original_ui_material_fields50kg.inc"
};
static_assert(sizeof(specs) / sizeof(specs[0]) == originalUiGsFieldCount50KG, "GS field schema");
bool fail(OriginalUiMaterialDiagnostic50KG& d, Error error, const char* field,
    std::uint32_t pc = 0) {
    d.error = error; d.field = field; d.sourcePC = pc; return false;
}
bool same(const std::string& a, const char* b) noexcept {
    std::size_t i = 0;
    for (; i < a.size() && b[i]; ++i) {
        const auto fold = [](char c) { return c >= 'A' && c <= 'Z' ? char(c + 32) : c; };
        if (fold(a[i]) != fold(b[i])) return false;
    }
    return i == a.size() && !b[i];
}
bool integer(const OriginalUiDatSection50KG& section, const char* name,
    std::int32_t fallback, std::int32_t& out, OriginalUiMaterialDiagnostic50KG& d, std::uint32_t pc) {
    if (!originalUiInteger50KG(section, name, fallback, out, d.document, pc))
        return fail(d, Error::value, name, pc);
    return true;
}
bool string(const OriginalUiDatSection50KG& section, const char* name,
    const std::string& fallback, std::string& out, OriginalUiMaterialDiagnostic50KG& d,
    std::uint32_t pc, std::size_t maxBytes = 255) {
    const auto* field = section.find(name);
    out = field ? field->text : fallback;
    return out.size() <= maxBytes || fail(d, Error::value, name, pc);
}
bool apply(const OriginalUiDatSection50KG& section, const OriginalUiMemberIdentity50KG* identity,
    std::size_t index, OriginalUiGsState50KG& state, OriginalUiMaterialDiagnostic50KG& d,
    bool isTemplate, const char* alias = nullptr) {
    const auto& spec = specs[index];
    auto& field = state.fields[index];
    const char* name = alias ? alias : spec.name;
    const auto pc = isTemplate ? spec.templatePC : (alias ? 0x247a7cu : spec.passPC);
    std::int32_t resolved = field.value;
    if (!integer(section, name, field.value, resolved, d, pc)) return false;
    if (const auto* authored = section.find(name))
        field = {resolved, identity, authored->valueOffset, pc};
    return true;
}
bool loadTemplate(const OriginalUiArchive50KG& archive, const char* name,
    OriginalUiMaterialTemplateState50KG& result, OriginalUiMaterialDiagnostic50KG& d) {
    OriginalUiDat50KG data;
    if (!readOriginalUiDat50KG(archive, name, data, d.document,
        OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(d, Error::document, name, 0x24aa10u);
    const auto* section = data.section("DefaultShaderParams");
    if (!section) return fail(d, Error::section, "DefaultShaderParams", 0x24aaacu);
    result.identity = data.identity();
    if (!string(*section, "microProgramName", "", result.microprogram, d, 0x24aac8u, 63)) return false;
    for (std::size_t i = 0; i < originalUiGsFieldCount50KG; ++i) {
        result.gs.fields[i] = {specs[i].initial, nullptr, 0, 0x24a8e0u};
        if (!apply(*section, data.identity(), i, result.gs, d, true)) return false;
    }
    return true;
}
void runtimeValue(OriginalUiGsState50KG& state, Field field, std::int32_t value, std::uint32_t pc) {
    state.fields[static_cast<std::size_t>(field)] = {value, nullptr, 0, pc};
}
bool knownPassField(const std::string& key) noexcept {
    for (const auto& spec : specs) if (same(key, spec.name)) return true;
    const char* names[] = {"microProgramName", "EnableThisShader", "totalTextureMaps", "TextureMap_1",
        "TextureCoordinateIndex_1", "ShaderPassGroup", "inputAlphaD", "lodCalculationMethod",
        "maximumMipLevel", "magFilter", "minFilter", "L", "K", "TextureAnimation",
        "animatedMapInitialFrame", "animatedMapFramesPerSecond", "animatedMapLooping",
        "animatedMapReverseLoopOnEnd", "animatedMapLoopOnEndToFrame"};
    for (const auto* name : names) if (same(key, name)) return true;
    return false;
}
}
const char* originalUiGsFieldName50KG(Field field) noexcept {
    const auto i = static_cast<std::size_t>(field);
    return i < originalUiGsFieldCount50KG ? specs[i].name : nullptr;
}
bool OriginalUiMaterialTemplates50KG::load(const OriginalUiArchive50KG& archive,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {};
    try {
        if (loaded_) return fail(d, Error::alreadyLoaded, "APPSTART");
        if (!archive.identity() || archive.identity()->id != OriginalUiArchiveId50KG::appStart)
            return fail(d, Error::archive, "APPSTART");
        OriginalUiMaterialTemplateState50KG defaults, screen;
        if (!loadTemplate(archive, "AppStart\\DefaultMT.psm", defaults, d) ||
            !loadTemplate(archive, "AppStart\\ScreenMT.psm", screen, d)) return false;
        defaults_ = std::move(defaults); screen_ = std::move(screen); loaded_ = true;
        return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
const OriginalUiMaterialTemplateState50KG* OriginalUiMaterialTemplates50KG::get(
    OriginalUiTemplate50KG owner) const noexcept {
    if (!loaded_) return nullptr;
    if (owner == OriginalUiTemplate50KG::defaultMt) return &defaults_;
    if (owner == OriginalUiTemplate50KG::screenMt || owner == OriginalUiTemplate50KG::nullOwnerScreenFallback)
        return &screen_;
    return nullptr;
}
bool readOriginalUiMaterial50KG(const OriginalUiArchive50KG& archive, const std::string& member,
    const OriginalUiMaterialTemplates50KG& templates, OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime, OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {};
    try {
        const auto* inherited = templates.get(owner);
        if (!inherited || !runtime.overrideKnown) return fail(d, Error::templateOwner, "associated template/runtime override", 0x24714cu);
        OriginalUiDat50KG data;
        if (!readOriginalUiDat50KG(archive, member, data, d.document,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder)) return fail(d, Error::document, "PSM", 0x247110u);
        const auto* general = data.section("General");
        if (!general) return fail(d, Error::section, "General", 0x29b700u);
        std::int32_t count = 0;
        if (!integer(*general, "TotalShaderPasses", 0, count, d, 0x29b718u)) return false;
        if (count < 1 || count > 8) return fail(d, Error::passCount, "TotalShaderPasses", 0x29b718u);
        OriginalUiMaterial50KG result; result.identity = data.identity();
        for (std::int32_t n = 1; n <= count; ++n) {
            d.pass = static_cast<std::uint32_t>(n);
            const auto* section = data.section("ShaderPass_" + std::to_string(n));
            if (!section) return fail(d, Error::section, "ShaderPass_N", 0x24718cu);
            OriginalUiMaterialPass50KG pass;
            pass.identity = data.identity(); pass.inheritedTemplate = inherited->identity;
            pass.pass = static_cast<std::uint32_t>(n); pass.gs = inherited->gs; pass.authored = *section;
            if (!string(*section, "microProgramName", inherited->microprogram, pass.microprogram, d, 0x2471a8u, 63) ||
                !string(*section, "ShaderPassGroup", "none", pass.shaderPassGroup, d, 0x247550u, 127) ||
                !integer(*section, "EnableThisShader", 1, pass.enabled, d, 0x2471dcu) ||
                !integer(*section, "totalTextureMaps", 1, pass.textureCount, d, 0x247214u)) return false;
            // This bounded typed resolver covers the actual one-map path only.
            if (pass.textureCount != 1) return fail(d, Error::textureCount, "totalTextureMaps", 0x247490u);
            if (!string(*section, "TextureMap_1", "", pass.textureName, d, 0x247260u) ||
                !integer(*section, "TextureCoordinateIndex_1", 0, pass.textureCoordinateIndex, d, 0x247338u)) return false;
            if (pass.textureName.empty()) {
                // 247274..247298 uses the original registered material basename.
                const auto slash = member.find_last_of("\\/");
                pass.textureName = member.substr(slash == std::string::npos ? 0 : slash + 1);
                const auto dot = pass.textureName.find_last_of('.');
                if (dot != std::string::npos) pass.textureName.resize(dot);
            }
            if (pass.textureName.empty() || same(pass.textureName, "RenderTarget") ||
                pass.textureName.find_first_of("\\/.:\"") != std::string::npos)
                return fail(d, Error::textureName, "TextureMap_1", 0x2472a8u);
            // The one-map builder explicitly uses context=0 as its missing
            // pass value, independently of template.environmentContext0or1.
            runtimeValue(pass.gs, Field::environmentContext0or1, 0, 0x247c88u);
            for (std::size_t i = 0; i < originalUiGsFieldCount50KG; ++i) {
                // Compatibility alias is read first, then inputColorD wins if
                // present, unlike first-present duplicate spelling resolution.
                if (i == static_cast<std::size_t>(Field::inputColorD) &&
                    !apply(*section, data.identity(), i, pass.gs, d, false, "inputAlphaD")) return false;
                if (!apply(*section, data.identity(), i, pass.gs, d, false)) return false;
            }
            struct Local { const char* name; std::int32_t* value; std::uint32_t pc; };
            const Local local[] = {{"lodCalculationMethod", &pass.lodCalculationMethod, 0x247f70u},
                {"maximumMipLevel", &pass.maximumMipLevel, 0x247f88u},
                {"magFilter", &pass.magFilter, 0x247fa0u}, {"minFilter", &pass.minFilter, 0x247fb8u},
                {"L", &pass.lodL, 0x247fd0u}, {"K", &pass.lodK, 0x247fe8u}};
            for (const auto& field : local)
                if (!integer(*section, field.name, *field.value, *field.value, d, field.pc)) return false;
            if (runtime.override435670) {
                runtimeValue(pass.gs, Field::inputAlphaC, 2, 0x247ac4u);
                runtimeValue(pass.gs, Field::fixedAlphaValue, 16, 0x247accu);
                runtimeValue(pass.gs, Field::foggingOnOff, 0, 0x247d6cu);
                runtimeValue(pass.gs, Field::alphaBlendingOnOff, 1, 0x247d70u);
                runtimeValue(pass.gs, Field::depthTestMethod, 1, 0x247ed4u);
            }
            result.passes.push_back(std::move(pass));
        }
        output = std::move(result); d = {}; return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
bool prepareOriginalUiNativeMaterial50KG(const OriginalUiMaterialPass50KG& pass,
    const Texture48E& texture, OriginalUiNativeMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {}; d.pass = pass.pass;
    try {
        if (!pass.identity || !pass.inheritedTemplate || pass.microprogram != "ScreenObject" ||
            pass.enabled != 1 || pass.textureCount != 1 || pass.textureCoordinateIndex != 0)
            return fail(d, Error::profile, "enabled single-map ScreenObject", 0x2471bcu);
        for (const auto& field : pass.authored.values)
            if (field.sourceValuePresent && !knownPassField(field.key))
                return fail(d, Error::unsupportedField, field.key.c_str(), 0x247110u);
        const auto value = [&](Field field) { return pass.gs[field].value; };
        // Original packet construction shifts/ORs integers; do not let an
        // out-of-width value leak into a neighboring GS field while the
        // normalized CPU policy mistakenly treats that neighbor as unchanged.
        constexpr std::int32_t maxima[] = {1,3,255,255,1,2,2,2,2,255,
            3,3,1023,1023,1023,1023,1,1,1,1,1,1,1,1,1,1,1,1,7,255,3,1,1,3,1};
        static_assert(sizeof(maxima) / sizeof(maxima[0]) == originalUiGsFieldCount50KG, "GS field widths");
        for (std::size_t i = 0; i < originalUiGsFieldCount50KG; ++i)
            if (pass.gs.fields[i].value < 0 || pass.gs.fields[i].value > maxima[i])
                return fail(d, Error::unsupportedValue, specs[i].name, pass.gs.fields[i].sourcePC);
        const auto expect = [&](Field field, std::int32_t expected) {
            return value(field) == expected || fail(d, Error::unsupportedValue,
                originalUiGsFieldName50KG(field), pass.gs[field].sourcePC);
        };
        if (!expect(Field::textureColorComponent, 1) || !expect(Field::textureFunction, 0) ||
            !expect(Field::colorClampingMethod, 1) || !expect(Field::alphaCorrectionValue, 0) ||
            !expect(Field::alphaBlendingControlInUnitsOfPixels, 0) ||
            !expect(Field::gouraudShadingOnOff, 1) || !expect(Field::textureMappingOnOff, 1) ||
            !expect(Field::foggingOnOff, 0) || !expect(Field::onePassAntiAliasingOnOff, 0) ||
            !expect(Field::textureCoordinatesSTQorUV, 0) ||
            !expect(Field::environmentContext0or1, 0) ||
            !expect(Field::fragmentValueControlFixedOrNot, 0) || !expect(Field::alphaTestOnOff, 0) ||
            !expect(Field::destinationAlphaTestOnOff, 0)) return false;
        if (texture.revision != 1 || texture.flags || texture.kind > 1 || texture.mips.size() != 1)
            return fail(d, Error::texture, "single original indexed RGBA mip", 0x24e880u);
        const auto& mip = texture.mips.front();
        if (mip.width < 16 || mip.height < 16 || mip.width > 1024 || mip.height > 1024 ||
            (mip.width & (mip.width - 1)) || (mip.height & (mip.height - 1)) ||
            std::uint64_t(mip.width) * mip.height * 4 != mip.rgba.size())
            return fail(d, Error::texture, "atlas dimensions");
        for (std::size_t i = 3; i < mip.rgba.size(); i += 4)
            if (mip.rgba[i] > 128) return fail(d, Error::unsupportedValue, "atlas alpha above128");
        if (pass.maximumMipLevel != 0 || pass.magFilter < 0 || pass.magFilter > 1 ||
            pass.minFilter < 0 || pass.minFilter > 5 || pass.lodCalculationMethod < 0 ||
            pass.lodCalculationMethod > 1 || pass.lodL < 0 || pass.lodL > 3 ||
            pass.lodK < -2048 || pass.lodK > 2047)
            return fail(d, Error::unsupportedValue, "TEX1 sampler", 0x247f38u);
        if (value(Field::wrapModeHorizontal) < 0 || value(Field::wrapModeHorizontal) > 1 ||
            value(Field::wrapModeVertical) < 0 || value(Field::wrapModeVertical) > 1)
            return fail(d, Error::unsupportedValue, "CLAMP region modes", 0x247b14u);
        if (value(Field::depthTestMethod) < 0 || value(Field::depthTestMethod) > 3 ||
            value(Field::zWriteDisabled) < 0 || value(Field::zWriteDisabled) > 1 ||
            value(Field::environmentContext0or1) < 0 || value(Field::environmentContext0or1) > 1)
            return fail(d, Error::unsupportedValue, "depth/context", 0x247ed8u);
        OriginalUiNativeMaterial50KG policy;
        const auto blend = value(Field::alphaBlendingOnOff);
        if (blend == 1) {
            if (!expect(Field::inputColorA, 0) || !expect(Field::inputAlphaC, 0) ||
                !expect(Field::inputColorD, 1)) return false;
            const auto b = value(Field::inputColorB);
            if (b != 1 && b != 2) return fail(d, Error::unsupportedValue, "inputColorB", 0x247a4cu);
            policy.blend = b == 1 ? OriginalUiMaterialBlend50KG::sourceAlpha : OriginalUiMaterialBlend50KG::additive;
        } else if (blend != 0) return fail(d, Error::unsupportedValue, "alphaBlendingOnOff", 0x247cf0u);
        // MXL=0 collapses all mip levels. MMAG 0/1 is nearest/linear;
        // MMIN 0/2/3 uses nearest texels, 1/4/5 uses linear texels.
        policy.minification = (pass.minFilter == 0 || pass.minFilter == 2 || pass.minFilter == 3)
            ? OriginalUiMaterialFilter50KG::nearest : OriginalUiMaterialFilter50KG::linear;
        policy.magnification = pass.magFilter ? OriginalUiMaterialFilter50KG::linear : OriginalUiMaterialFilter50KG::nearest;
        // MXL=0 alone does not prove identical GS/native min-versus-mag
        // selection under LCM/L/K bias. Equal texel filters remove that
        // dependency without inventing an LOD owner (default1/5 is linear).
        if (policy.minification != policy.magnification)
            return fail(d, Error::unsupportedValue, "differing GS min/mag needs LOD owner", 0x247f38u);
        policy.u = value(Field::wrapModeHorizontal) ? OriginalUiMaterialWrap50KG::clampToEdge : OriginalUiMaterialWrap50KG::repeat;
        policy.v = value(Field::wrapModeVertical) ? OriginalUiMaterialWrap50KG::clampToEdge : OriginalUiMaterialWrap50KG::repeat;
        policy.depthMethod = static_cast<std::uint8_t>(value(Field::depthTestMethod));
        policy.depthWrite = value(Field::zWriteDisabled) == 0;
        policy.environmentContext = static_cast<std::uint8_t>(value(Field::environmentContext0or1));
        policy.requiresOriginalDepthOwner = policy.depthMethod != 1 || policy.depthWrite;
        policy.requiresSeparateAlphaStore = blend != 0;
        output = policy; d = {}; return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
}
