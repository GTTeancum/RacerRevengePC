#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"
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
void applyRuntimeOverrides(OriginalUiGsState50KG& state, const OriginalUiMaterialRuntime50KG& runtime) {
    if (!runtime.override435670) return;
    runtimeValue(state, Field::inputAlphaC, 2, 0x247ac4u);
    runtimeValue(state, Field::fixedAlphaValue, 16, 0x247accu);
    runtimeValue(state, Field::foggingOnOff, 0, 0x247d6cu);
    runtimeValue(state, Field::alphaBlendingOnOff, 1, 0x247d70u);
    runtimeValue(state, Field::depthTestMethod, 1, 0x247ed4u);
}
const OriginalUiMemberIdentity50KG* catalogMember(OriginalUiArchiveId50KG id, const char* name) {
    const auto* archive = originalUiArchiveIdentity50KG(id);
    if (!archive) return nullptr;
    for (std::size_t i = 0; i < archive->memberCount; ++i) {
        const auto* member = originalUiMemberIdentity50KG(id, i);
        if (member && same(member->name, name)) return member;
    }
    return nullptr;
}
struct PromptSource {
    const char* texture;const char* member;const char* registeredName;const char* psm;
    std::uint32_t callPC;
};
constexpr PromptSource promptSources[]={
    {"icon_x","ui\\textures\\icon_x.pst","ui\\textures\\icon_x","ui\\textures\\icon_x.psm",0x1d682c},
    {"icon_tri","ui\\textures\\icon_tri.pst","ui\\textures\\icon_tri","ui\\textures\\icon_tri.psm",0x1d68dc},
    {"icon_sqr","ui\\textures\\icon_sqr.pst","ui\\textures\\icon_sqr","ui\\textures\\icon_sqr.psm",0x1d6994},
    {"icon_cir","ui\\textures\\icon_cir.pst","ui\\textures\\icon_cir","ui\\textures\\icon_cir.psm",0x1d6a4c}
};
constexpr PromptSource riderStatSources[]={
    {"podstat","ui\\textures\\podstat.pst","ui\\textures\\podstat","ui\\textures\\podstat.psm",0x1e89d8},
    {"podstat2","ui\\textures\\podstat2.pst","ui\\textures\\podstat2","ui\\textures\\podstat2.psm",0x1e89f0},
    {"notch","ui\\textures\\notch.pst","ui\\textures\\notch","ui\\textures\\notch.psm",0x1e8a38}
};
bool sourceIdentityKnown(const OriginalUiMaterialPass50KG& pass) {
    if (pass.provenance.origin == OriginalUiMaterialOrigin50KG::authoredPsm)
        return pass.identity != nullptr;
    const auto& p=pass.provenance;
    if(pass.identity || pass.inheritedTemplate!=catalogMember(OriginalUiArchiveId50KG::appStart,"AppStart\\ScreenMT.psm") ||
        p.creationPC!=0x2475c0u || pass.pass!=1 || !pass.authored.values.empty() || !pass.authored.name.empty())return false;
    if(p.origin==OriginalUiMaterialOrigin50KG::mainLogoMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::mainMenu) &&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::mainMenu,"ui\\textures\\mmlogo.pst") &&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::mainMenu,"UI\\UIMain\\Widgets.Dat") &&
            p.registeredName=="ui\\textures\\mmlogo" && p.attemptedPsmMember=="ui\\textures\\mmlogo.psm" &&
            pass.textureName=="mmlogo" && !p.fallbackPageSource && !p.sourceCallerPC;
    if(p.origin==OriginalUiMaterialOrigin50KG::riderStatMissingPsmFallback){
        if(p.archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::rider,"ui\\uirider\\rider.dat"))return false;
        for(const auto& source:riderStatSources)
            if(p.sourceCallerPC==source.callPC&&p.registeredName==source.registeredName&&
                p.attemptedPsmMember==source.psm&&pass.textureName==source.texture&&
                p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::rider,source.member))return true;
        return false;
    }
    const bool main=p.origin==OriginalUiMaterialOrigin50KG::mainPromptMissingPsmFallback;
    const bool options=p.origin==OriginalUiMaterialOrigin50KG::optionsMenuPromptMissingPsmFallback;
    const bool gameOptions=p.origin==OriginalUiMaterialOrigin50KG::optionsGamePromptMissingPsmFallback;
    if(gameOptions){
        const auto& source=promptSources[2];
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)&&
            !p.fallbackWidgetSource&&p.fallbackPageSource==catalogMember(OriginalUiArchiveId50KG::options,"UI\\UIOption\\options.dat")&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::options,source.member)&&
            p.sourceCallerPC==source.callPC&&p.registeredName==source.registeredName&&
            p.attemptedPsmMember==source.psm&&pass.textureName==source.texture;
    }
    if((!main&&!options) || p.archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays) ||
        p.fallbackWidgetSource || p.fallbackPageSource!=catalogMember(
            main?OriginalUiArchiveId50KG::mainMenu:OriginalUiArchiveId50KG::options,
            main?"UI\\UIMain\\main.dat":"UI\\UIOption\\menu.dat"))return false;
    for(std::size_t i=0;i<(main?4u:2u);++i) {
        const auto& source=promptSources[i];
        if(p.sourceCallerPC==source.callPC && p.registeredName==source.registeredName &&
            p.attemptedPsmMember==source.psm && pass.textureName==source.texture &&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::overlays,source.member))return true;
    }
    return false;
}
// Common original2475C0 ->2476A0 NULL-config builder. There is no authored
// shader section to synthesize, and constructor/pass defaults stay in one place.
OriginalUiMaterial50KG screenTextureFallback(const OriginalUiMaterialProvenance50KG& provenance,
    const char* texture,const OriginalUiMaterialTemplateState50KG& inherited,
    const OriginalUiMaterialRuntime50KG& runtime) {
    OriginalUiMaterial50KG result;result.provenance=provenance;
    OriginalUiMaterialPass50KG pass;pass.provenance=provenance;
    pass.inheritedTemplate=inherited.identity;pass.pass=1;pass.microprogram=inherited.microprogram;
    pass.shaderPassGroup="none";pass.textureName=texture;pass.gs=inherited.gs;
    runtimeValue(pass.gs,Field::environmentContext0or1,0,0x247c88u);
    applyRuntimeOverrides(pass.gs,runtime);result.passes.push_back(std::move(pass));return result;
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
        result.provenance.archive = archive.identity();
        result.provenance.creationPC = 0x247110u;
        for (std::int32_t n = 1; n <= count; ++n) {
            d.pass = static_cast<std::uint32_t>(n);
            const auto* section = data.section("ShaderPass_" + std::to_string(n));
            if (!section) return fail(d, Error::section, "ShaderPass_N", 0x24718cu);
            OriginalUiMaterialPass50KG pass;
            pass.identity = data.identity(); pass.inheritedTemplate = inherited->identity;
            pass.provenance = result.provenance;
            pass.pass = static_cast<std::uint32_t>(n); pass.gs = inherited->gs; pass.authored = *section;
            if (!string(*section, "microProgramName", inherited->microprogram, pass.microprogram, d, 0x2471a8u, 63) ||
                !string(*section, "ShaderPassGroup", "none", pass.shaderPassGroup, d, 0x247550u, 127) ||
                !integer(*section, "EnableThisShader", 1, pass.enabled, d, 0x2471dcu) ||
                !integer(*section, "totalTextureMaps", 1, pass.textureCount, d, 0x247214u)) return false;
            // 247350 skips the entire map loop for zero maps; 247444..2474BC
            // selects the same single-context GS builder for zero/one maps.
            if (pass.textureCount < 0 || pass.textureCount > 1)
                return fail(d, Error::textureCount, "totalTextureMaps", 0x247490u);
            if (pass.textureCount == 1) {
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
            }
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
            applyRuntimeOverrides(pass.gs, runtime);
            result.passes.push_back(std::move(pass));
        }
        output = std::move(result); d = {}; return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
bool readOriginalUiMainLogoFallbackMaterial50KG(const OriginalUiArchive50KG& archive,
    const OriginalUiMaterialTemplates50KG& templates, OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime, OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {};
    try {
        if (!archive.loaded() || !archive.identity() ||
            archive.identity()->id != OriginalUiArchiveId50KG::mainMenu)
            return fail(d, Error::archive, "authenticated UIMAIN", 0x29b550u);
        const auto* inherited = templates.get(owner);
        if (owner != OriginalUiTemplate50KG::screenMt || !inherited || !runtime.overrideKnown)
            return fail(d, Error::templateOwner, "explicit ScreenTexture ScreenMT/runtime owner", 0x2df10cu);
        const auto texture = archive.find("ui\\textures\\mmlogo.pst");
        if (!texture.identity || !texture.data || texture.size != texture.identity->bytes ||
            archive.find("ui\\textures\\mmlogo.psm").identity)
            return fail(d, Error::fallbackSource, "original mmlogo texture and absent PSM", 0x29b638u);
        OriginalUiDat50KG widgets;
        if (!readOriginalUiDat50KG(archive, "UI\\UIMain\\Widgets.Dat", widgets, d.document))
            return fail(d, Error::document, "Main original widget source", 0x2ac650u);
        const auto* type = widgets.find("Widget 35", "Type");
        const auto* name = widgets.find("Widget 35", "Texture");
        if (!type || type->text != "Icon" || !name || name->text != "mmlogo")
            return fail(d, Error::fallbackSource, "Main Icon35 original mmlogo reference", 0x2ac650u);
        OriginalUiMaterial50KG result;
        result.provenance.origin = OriginalUiMaterialOrigin50KG::mainLogoMissingPsmFallback;
        result.provenance.archive = archive.identity();
        result.provenance.fallbackTexture = texture.identity;
        result.provenance.fallbackWidgetSource = widgets.identity();
        result.provenance.registeredName = "ui\\textures\\mmlogo";
        result.provenance.attemptedPsmMember = "ui\\textures\\mmlogo.psm";
        result.provenance.creationPC = 0x2475c0u;
        result=screenTextureFallback(result.provenance,"mmlogo",*inherited,runtime);
        output = std::move(result); d = {}; return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
bool readOriginalUiMainPromptFallbackMaterial50KG(const OriginalUiArchive50KG& mainArchive,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!mainArchive.loaded()||mainArchive.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::mainMenu)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Main and shared OVERLAYS",0x1d682c);
        const auto index=static_cast<std::size_t>(prompt);
        if(index>=sizeof(promptSources)/sizeof(promptSources[0]))return fail(d,Error::fallbackSource,"original page prompt");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=overlays.find(source.member);
        const auto page=mainArchive.find("UI\\UIMain\\main.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||mainArchive.find(source.psm).identity)
            return fail(d,Error::fallbackSource,"actual prompt texture/page and absent PSM",source.callPC);
        OriginalUiMaterialProvenance50KG provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::mainPromptMissingPsmFallback;
        provenance.archive=overlays.identity();provenance.fallbackTexture=texture.identity;
        provenance.fallbackPageSource=page.identity;provenance.sourceCallerPC=source.callPC;
        provenance.registeredName=source.registeredName;provenance.attemptedPsmMember=source.psm;
        provenance.creationPC=0x2475c0;
        auto result=screenTextureFallback(provenance,source.texture,*inherited,runtime);
        output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiOptionsMenuPromptFallbackMaterial50KG(const OriginalUiArchive50KG& options,
    const OriginalUiArchive50KG& overlays,OriginalUiOptionsMenuPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!options.loaded()||options.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Options and shared OVERLAYS",0x1f8e54);
        const auto index=static_cast<std::size_t>(prompt);
        if(index>=2)return fail(d,Error::fallbackSource,"Options menu cross/triangle only");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=overlays.find(source.member);const auto page=options.find("UI\\UIOption\\menu.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||options.find(source.psm).identity||options.find(source.member).identity)
            return fail(d,Error::fallbackSource,"shared prompt texture and original Options menu",source.callPC);
        OriginalUiMaterialProvenance50KG provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::optionsMenuPromptMissingPsmFallback;
        provenance.archive=overlays.identity();provenance.fallbackTexture=texture.identity;
        provenance.fallbackPageSource=page.identity;provenance.sourceCallerPC=source.callPC;
        provenance.registeredName=source.registeredName;provenance.attemptedPsmMember=source.psm;
        provenance.creationPC=0x2475c0;
        auto result=screenTextureFallback(provenance,source.texture,*inherited,runtime);
        output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiGameOptionsPromptFallbackMaterial50KG(const OriginalUiArchive50KG& options,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!options.loaded()||options.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Options and shared OVERLAYS",0x1f4724);
        const auto index=static_cast<std::size_t>(prompt);
        if(index!=2)return fail(d,Error::fallbackSource,"Game Options local square only");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=options.find(source.member);const auto page=options.find("UI\\UIOption\\options.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||options.find(source.psm).identity)
            return fail(d,Error::fallbackSource,"Options-local square texture and original Game Options page",source.callPC);
        OriginalUiMaterialProvenance50KG provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::optionsGamePromptMissingPsmFallback;
        provenance.archive=options.identity();provenance.fallbackTexture=texture.identity;
        provenance.fallbackPageSource=page.identity;provenance.sourceCallerPC=source.callPC;
        provenance.registeredName=source.registeredName;provenance.attemptedPsmMember=source.psm;
        provenance.creationPC=0x2475c0;
        auto result=screenTextureFallback(provenance,source.texture,*inherited,runtime);
        output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiRiderStatFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    OriginalUiRiderStatTexture50KG requested,const OriginalUiMaterialTemplates50KG& templates,
    OriginalUiTemplate50KG owner,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!rider.loaded()||rider.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider))
            return fail(d,Error::archive,"authenticated Rider archive",0x1e89d8);
        const auto index=static_cast<std::size_t>(requested);
        if(index>=3)return fail(d,Error::fallbackSource,"Rider podstat/podstat2/notch only");
        const auto& source=riderStatSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=rider.find(source.member);const auto page=rider.find("ui\\uirider\\rider.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||rider.find(source.psm).identity)
            return fail(d,Error::fallbackSource,"actual Rider stat texture and absent original PSM",source.callPC);
        OriginalUiMaterialProvenance50KG provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::riderStatMissingPsmFallback;
        provenance.archive=rider.identity();provenance.fallbackTexture=texture.identity;
        provenance.fallbackPageSource=page.identity;provenance.sourceCallerPC=source.callPC;
        provenance.registeredName=source.registeredName;provenance.attemptedPsmMember=source.psm;
        provenance.creationPC=0x2475c0;
        auto result=screenTextureFallback(provenance,source.texture,*inherited,runtime);
        output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
static bool prepareOriginalUiOneMapMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const Texture48E& texture, OriginalUiNativeMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d, bool litProgram,
    bool& fogEnabled, bool& alphaTestGreaterZero, bool authenticatedRiderIpum = false) noexcept {
    d = {}; d.pass = pass.pass;
    try {
        if (pass.textureCount == 0)
            return fail(d, Error::untexturedAdapterRequired, "zero-map requires untextured material policy", 0x2474b8u);
        const bool programMatches = litProgram ?
            ((same(pass.microprogram, "Diffuse") || same(pass.microprogram, "DiffuseCompressed")) &&
             pass.provenance.origin == OriginalUiMaterialOrigin50KG::authoredPsm &&
             pass.inheritedTemplate == catalogMember(OriginalUiArchiveId50KG::appStart,
                 "AppStart\\DefaultMT.psm")) : pass.microprogram == "ScreenObject";
        if (!sourceIdentityKnown(pass) || !pass.inheritedTemplate || !programMatches ||
            pass.enabled != 1 || pass.textureCount != 1 || pass.textureCoordinateIndex != 0)
            return fail(d, Error::profile, litProgram ? "authored DefaultMT Diffuse/Compressed" :
                "enabled single-map ScreenObject", 0x2471bcu);
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
            (!litProgram && !expect(Field::foggingOnOff, 0)) || !expect(Field::onePassAntiAliasingOnOff, 0) ||
            !expect(Field::textureCoordinatesSTQorUV, 0) ||
            !expect(Field::environmentContext0or1, 0) ||
            !expect(Field::fragmentValueControlFixedOrNot, 0) ||
            (!litProgram && !expect(Field::alphaTestOnOff, 0)) ||
            !expect(Field::destinationAlphaTestOnOff, 0)) return false;
        const bool sourceFog = litProgram && value(Field::foggingOnOff) != 0;
        const bool sourceAlphaTest = litProgram && value(Field::alphaTestOnOff) != 0;
        if (sourceAlphaTest && (!expect(Field::alphaTestMethod, 6) ||
            !expect(Field::alphaReferenceValue, 0) ||
            !expect(Field::processingMethodWhenFailedInAlphaTest, 0))) return false;
        const bool textureShape = authenticatedRiderIpum ?
            (litProgram && texture.kind == 2 && texture.mips.size() == originalUiIpumMipCount50KG) :
            (texture.kind <= 1 && texture.mips.size() == 1);
        if (texture.revision != 1 || texture.flags || !textureShape)
            return fail(d, Error::texture, "admitted original texture/mip provenance", 0x24e880u);
        // Typed kind2 path below authenticates this actual bank image. Native
        // highest-resolution policy binds the last stored (source level0) mip;
        // no historical residentF4 or unuploaded GPU mip levels are implied.
        const auto& mip = texture.mips.back();
        if (mip.width < 16 || mip.height < 16 || mip.width > 1024 || mip.height > 1024 ||
            (mip.width & (mip.width - 1)) || (mip.height & (mip.height - 1)) ||
            std::uint64_t(mip.width) * mip.height * 4 != mip.rgba.size())
            return fail(d, Error::texture, "atlas dimensions");
        // Real Main PST palettes use the full source alpha byte (up to255).
        // Do not rewrite or reject those authentic bytes. The native renderer
        // cold-caches each exact atlas maximum and, before target writes,
        // requires vertexAlpha * atlasMaximumAlpha <= 128*128 for blended
        // spans. That joint bound admits Main's63/64 packet alpha while
        // rejecting GS coefficients above1 that fixed-function GL cannot express.
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
        output = policy; fogEnabled = sourceFog; alphaTestGreaterZero = sourceAlphaTest;
        d = {}; return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
bool prepareOriginalUiNativeMaterial50KG(const OriginalUiMaterialPass50KG& pass,
    const Texture48E& texture, OriginalUiNativeMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    bool fog = false, alphaTest = false;
    return prepareOriginalUiOneMapMaterial50KM(pass, texture, output, d, false, fog, alphaTest);
}
bool prepareOriginalUiLitMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const Texture48E& texture, OriginalUiLitMaterial50KM& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    OriginalUiLitMaterial50KM next;
    if (!prepareOriginalUiOneMapMaterial50KM(pass, texture, next.raster, d, true,
        next.fogEnabled, next.alphaTestGreaterZero)) return false;
    next.program = same(pass.microprogram, "Diffuse") ? OriginalUiLitProgram50KM::diffuse :
        OriginalUiLitProgram50KM::diffuseCompressed;
    output = next; return true;
}
bool prepareOriginalUiRider19IpumMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19IpumMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {}; d.pass = pass.pass;
    try {
        const auto* source = images.sourceProvenance(imageIndex);
        const auto* ipum = images.ipumProvenance(imageIndex);
        const auto* texture = images.decodedTexture(imageIndex);
        const auto* bank = images.textures();
        const auto* image = bank ? bank->texture(imageIndex) : nullptr;
        const auto* member = catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\19c_1.pst");
        if (!images.loaded() || !source || !ipum || !texture || !image || !member ||
            source->memberIdentity != member || source->celIdentity || source->pstOffset != 0 ||
            source->pstBytes != member->bytes || ipum->sourcePstSha256 != member->sha256 ||
            ipum->colorPolicyKind != OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1 ||
            texture->mips.size() != originalUiIpumMipCount50KG)
            return fail(d,Error::texture,"authenticated original Rider19 IPUM image bank",0x24e880u);
        const auto& base=texture->mips.back();
        if (image->texture.width != base.width || image->texture.height != base.height ||
            image->texture.rgba != base.rgba)
            return fail(d,Error::texture,"published native image must equal original largest mip",0x24eb90u);
        if (pass.identity != catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\19c_1.psm") ||
            pass.pass != 1 || !same(pass.microprogram,"DiffuseCompressed") || !same(pass.textureName,"19c_1") ||
            pass.gs[Field::alphaBlendingOnOff].value != 0 || pass.gs[Field::alphaTestOnOff].value != 0)
            return fail(d,Error::profile,"actual opaque alpha-test-off Rider19c_1 pass",0x2476a0u);
        OriginalUiRider19IpumMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,true)) return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIpumImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=static_cast<std::uint32_t>(texture->mips.size()-1);
        next.sourceMipCount=static_cast<std::uint32_t>(texture->mips.size());
        output=next;return true;
    } catch (...) { d.error=Error::allocation; return false; }
}
bool prepareOriginalUiNativeUntexturedMaterial50KG(const OriginalUiMaterialPass50KG& pass,
    OriginalUiNativeUntexturedMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {}; d.pass = pass.pass;
    try {
        if (pass.provenance.origin != OriginalUiMaterialOrigin50KG::authoredPsm ||
            !pass.identity || !pass.inheritedTemplate || pass.microprogram != "ScreenObject" ||
            pass.enabled != 1 || pass.textureCount != 0 || !pass.textureName.empty())
            return fail(d, Error::profile, "enabled zero-map authored ScreenObject", 0x2474b8u);
        for (const auto& field : pass.authored.values)
            if (field.sourceValuePresent && !knownPassField(field.key))
                return fail(d, Error::unsupportedField, field.key.c_str(), 0x247110u);
        const auto value = [&](Field field) { return pass.gs[field].value; };
        // Bound packet integers even for unused texture registers; never let
        // an out-of-width input claim a different normalized hardware state.
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
        if (!expect(Field::colorClampingMethod, 1) || !expect(Field::alphaCorrectionValue, 0) ||
            !expect(Field::alphaBlendingControlInUnitsOfPixels, 0) ||
            !expect(Field::gouraudShadingOnOff, 1) || !expect(Field::textureMappingOnOff, 0) ||
            !expect(Field::foggingOnOff, 0) || !expect(Field::onePassAntiAliasingOnOff, 0) ||
            !expect(Field::environmentContext0or1, 0) ||
            !expect(Field::fragmentValueControlFixedOrNot, 0) || !expect(Field::alphaTestOnOff, 0) ||
            !expect(Field::destinationAlphaTestOnOff, 0)) return false;
        OriginalUiNativeUntexturedMaterial50KG policy;
        const auto blend = value(Field::alphaBlendingOnOff);
        if (blend == 1) {
            if (!expect(Field::inputColorA, 0) || !expect(Field::inputAlphaC, 0) ||
                !expect(Field::inputColorD, 1)) return false;
            const auto b = value(Field::inputColorB);
            if (b != 1 && b != 2)
                return fail(d, Error::unsupportedValue, "inputColorB", 0x247a4cu);
            policy.blend = b == 1 ? OriginalUiMaterialBlend50KG::sourceAlpha : OriginalUiMaterialBlend50KG::additive;
        } else if (blend != 0) return fail(d, Error::unsupportedValue, "alphaBlendingOnOff", 0x247cf0u);
        // TME=0 uses raw packet vertex RGB/alpha. There is no MODULATE scale
        // and no texture sampler/LOD/atlas. Only RGB blending divides A by128;
        // framebuffer alpha is still stored in raw byte units independently.
        policy.depthMethod = static_cast<std::uint8_t>(value(Field::depthTestMethod));
        policy.depthWrite = value(Field::zWriteDisabled) == 0;
        policy.environmentContext = static_cast<std::uint8_t>(value(Field::environmentContext0or1));
        policy.requiresOriginalDepthOwner = policy.depthMethod != 1 || policy.depthWrite;
        policy.requiresSeparateAlphaStore = blend != 0;
        output = policy; d = {}; return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
}
