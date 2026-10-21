#include "original_ui_material50kg.h"
#include "original_ui_screen_assets50kg.h"
#include "original_ui_controls_specmap50kg.h"
#include "original_standard_material_object50la.h"
#include "original_standard_material_animation50la.h"
#include <utility>

namespace rr::assets {
// Values remain the existing authenticated24A8E0/template-file projection.
// Only ownership changes: all users of one retained APPSTART get these exact
// immutable object addresses, including cold consumers between UI sessions.
struct OriginalUiMaterialTemplateStorage50KG {
    OriginalUiMaterialTemplateState50KG defaults, screen;
};
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
// HasPopup1D6698 synchronously constructs these four ScreenTextures before
// the shared page prompt objects at1D682C..1D6A4C. Tournament's fallback
// provenance records its first real registration caller, not the later borrow.
constexpr std::uint32_t tournamentPromptFirstCallerPC[]{0x1ff058,0x1ff110,0x1ff1c8,0x1ff280};
constexpr PromptSource riderStatSources[]={
    {"podstat","ui\\textures\\podstat.pst","ui\\textures\\podstat","ui\\textures\\podstat.psm",0x1e89d8},
    {"podstat2","ui\\textures\\podstat2.pst","ui\\textures\\podstat2","ui\\textures\\podstat2.psm",0x1e89f0},
    {"notch","ui\\textures\\notch.pst","ui\\textures\\notch","ui\\textures\\notch.psm",0x1e8a38}
};
bool riderPortraitIndex(std::uint8_t rider) noexcept {return rider>=1&&rider<=26&&rider!=5&&rider!=6&&rider!=13;}
std::string riderPortraitName(std::uint8_t rider) {
    std::string name="00_flrg";name[0]=char('0'+rider/10);name[1]=char('0'+rider%10);return name;
}
// Finite original names and first source callers. The panel is shared by
// seven genuine ScreenTextures; triangle9 has its own primitive constructor.
struct SpeedBoostSource50LA {const char* name;std::uint32_t caller;};
constexpr SpeedBoostSource50LA speedBoostSources50LA[]={
    {"spdbost1",0x1895F4},{"boost_a1",0x189F7C},{"boost_a2",0x18A080},
    {"boost_a3",0x18A184},{"boost_a4",0x18A288},{"boost_a5",0x18A38C},
    {"boost_a6",0x18A490},{"boost_a7",0x18A594},{"boost_a8",0x18A698},
    {"boost_a9",0x18A7C0}
};
bool sourceIdentityKnown(const OriginalUiMaterialPass50KG& pass) {
    if (pass.provenance.origin == OriginalUiMaterialOrigin50KG::authoredPsm)
        return pass.identity != nullptr;
    const auto& p=pass.provenance;
    if(pass.identity || pass.inheritedTemplate!=catalogMember(OriginalUiArchiveId50KG::appStart,"AppStart\\ScreenMT.psm") ||
        p.creationPC!=0x2475c0u || pass.pass!=1 || !pass.authored.values.empty() || !pass.authored.name.empty())return false;
    if(p.origin==OriginalUiMaterialOrigin50KG::pauseBackdropMissingPsmFallback){
        constexpr auto id=OriginalUiArchiveId50KG::overlays;
        if(p.archive!=originalUiArchiveIdentity50KG(id)||p.fallbackWidgetSource||p.fallbackPageSource)return false;
        for(unsigned i=0;i<2;++i){
            const char* name=i?"results2":"results1";
            const std::string base=std::string("ui\\textures\\")+name;
            const auto* texture=catalogMember(id,(base+".pst").c_str());
            if(texture&&!catalogMember(id,(base+".psm").c_str())&&p.fallbackTexture==texture&&
               p.sourceCallerPC==(i?0x1B898Cu:0x1B8840u)&&p.registeredName==base&&
               p.attemptedPsmMember==base+".psm"&&pass.textureName==name)return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::damageTextureMissingPsmFallback){
        // Exact provenance emitted by readOriginalDamageFallbackMaterial50LA:
        // first ScreenTexture loader18C748, genuine OVERLAYS PST, absent PSM.
        // Shared checks above still require an unauthored ScreenMT fallback;
        // native GS/program/sampler admission below remains unchanged.
        constexpr auto id=OriginalUiArchiveId50KG::overlays;
        const auto* texture=catalogMember(id,"ui\\textures\\dmgradar.pst");
        return texture&&p.archive==originalUiArchiveIdentity50KG(id)&&
            !p.fallbackWidgetSource&&!p.fallbackPageSource&&
            !catalogMember(id,"ui\\textures\\dmgradar.psm")&&p.fallbackTexture==texture&&
            p.sourceCallerPC==0x18C748&&p.registeredName=="ui\\textures\\dmgradar"&&
            p.attemptedPsmMember=="ui\\textures\\dmgradar.psm"&&pass.textureName=="dmgradar";
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::speedBoostTextureMissingPsmFallback||
       p.origin==OriginalUiMaterialOrigin50KG::speedBoostTriangleMissingPsmFallback){
        constexpr auto id=OriginalUiArchiveId50KG::overlays;
        if(p.archive!=originalUiArchiveIdentity50KG(id)||p.fallbackWidgetSource||p.fallbackPageSource)return false;
        for(std::size_t i=0;i<10;++i){
            const auto& source=speedBoostSources50LA[i];
            const auto origin=i==9?OriginalUiMaterialOrigin50KG::speedBoostTriangleMissingPsmFallback:
                OriginalUiMaterialOrigin50KG::speedBoostTextureMissingPsmFallback;
            const std::string base=std::string("ui\\textures\\")+source.name;
            const auto* texture=catalogMember(id,(base+".pst").c_str());
            if(p.origin==origin&&texture&&!catalogMember(id,(base+".psm").c_str())&&
               p.fallbackTexture==texture&&p.sourceCallerPC==source.caller&&p.registeredName==base&&
               p.attemptedPsmMember==base+".psm"&&pass.textureName==source.name)return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::tournamentPortraitMissingPsmFallback){
        constexpr auto id=OriginalUiArchiveId50KG::tournament;
        if(p.archive!=originalUiArchiveIdentity50KG(id)||p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(id,"UI\\UITourn\\standing.dat")||
            p.sourceCallerPC!=0x1EF034)return false;
        for(std::uint8_t career=1;career<=26;++career){
            if(!riderPortraitIndex(career))continue;
            const auto name=riderPortraitName(career),registered=std::string("ui/textures/")+name;
            const auto catalogName=std::string("ui\\textures\\")+name;
            const auto* texture=catalogMember(id,(catalogName+".pst").c_str());
            if(texture&&!catalogMember(id,(catalogName+".psm").c_str())&&
                p.fallbackTexture==texture&&p.registeredName==registered&&
                p.attemptedPsmMember==registered+".psm"&&pass.textureName==name)return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::tournamentNotchMissingPsmFallback){
        constexpr auto id=OriginalUiArchiveId50KG::tournament;
        const auto* texture=catalogMember(id,"ui\\textures\\notch.pst");
        return texture&&p.archive==originalUiArchiveIdentity50KG(id)&&!p.fallbackWidgetSource&&
            p.fallbackPageSource==catalogMember(id,"UI\\UITourn\\standing.dat")&&
            !catalogMember(id,"ui\\textures\\notch.psm")&&p.fallbackTexture==texture&&
            p.sourceCallerPC==0x1F1424&&p.registeredName=="ui/textures/notch"&&
            p.attemptedPsmMember=="ui/textures/notch.psm"&&pass.textureName=="notch";
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::courseTrophyMissingPsmFallback){
        constexpr auto id=OriginalUiArchiveId50KG::course;
        if(p.archive!=originalUiArchiveIdentity50KG(id)||p.fallbackWidgetSource||
           p.fallbackPageSource!=catalogMember(id,"UI\\UICourse\\track.dat"))return false;
        constexpr const char* names[]{"trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza"};
        constexpr std::uint32_t pcs[]{0x1D999C,0x1D99B4,0x1D99CC,0x1D99E4,0x1D99FC,0x1D9A14};
        for(std::size_t i=0;i<6;++i){const auto registered=std::string("ui/textures/")+names[i];
            const auto catalogName=std::string("ui\\textures\\")+names[i];
            const auto* texture=catalogMember(id,(catalogName+".pst").c_str());
            if(texture&&!catalogMember(id,(catalogName+".psm").c_str())&&p.sourceCallerPC==pcs[i]&&
               p.registeredName==registered&&p.attemptedPsmMember==registered+".psm"&&
               pass.textureName==names[i]&&p.fallbackTexture==texture)return true;
        }return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::tournamentTrophyMissingPsmFallback){
        constexpr auto id=OriginalUiArchiveId50KG::tournament;
        if(p.archive!=originalUiArchiveIdentity50KG(id)||p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(id,"UI\\UITourn\\standing.dat"))return false;
        constexpr const char* names[]{"trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza"};
        constexpr std::uint32_t pcs[]{0x1EF920,0x1EF964,0x1EF9A8,0x1EF938,0x1EF97C,0x1EF9C0};
        for(std::size_t i=0;i<6;++i){
            const auto registered=std::string("ui/textures/")+names[i];
            const auto psm=registered+".psm";
            const auto catalogName=std::string("ui\\textures\\")+names[i];
            const auto* texture=catalogMember(id,(catalogName+".pst").c_str());
            if(texture&&!catalogMember(id,(catalogName+".psm").c_str())&&p.sourceCallerPC==pcs[i]&&
                p.registeredName==registered&&p.attemptedPsmMember==psm&&
                pass.textureName==names[i]&&p.fallbackTexture==texture)return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::raceHudFontMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::overlays,"font\\denmarkbold16ingame.pst")&&
            !p.fallbackWidgetSource&&!p.fallbackPageSource&&p.sourceCallerPC==0x1a8dd4&&
            p.registeredName=="font\\denmarkbold16ingame"&&
            p.attemptedPsmMember=="font\\denmarkbold16ingame.psm"&&pass.textureName=="denmarkbold16ingame";
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
    if(p.origin==OriginalUiMaterialOrigin50KG::riderPortraitMissingPsmFallback){
        if(p.archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::rider,"ui\\uirider\\rider.dat")||
            p.sourceCallerPC!=0x1e9448)return false;
        for(std::uint8_t rider=1;rider<=26;++rider){
            if(!riderPortraitIndex(rider))continue;
            const auto name=riderPortraitName(rider);const auto registered="ui\\textures\\"+name;
            if(p.registeredName==registered&&p.attemptedPsmMember==registered+".psm"&&pass.textureName==name&&
                p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::rider,(registered+".pst").c_str()))return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::riderNameBlurMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\nameblur.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::rider,"UI\\UIRider\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2ac6b0&&
            p.registeredName=="ui\\textures\\nameblur"&&p.attemptedPsmMember=="ui\\textures\\nameblur.psm"&&
            pass.textureName=="nameblur";
    if(p.origin==OriginalUiMaterialOrigin50KG::riderBackgroundMissingPsmFallback){
        if(p.archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::backgrounds)||p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::rider,"UI\\UIRider\\rider.dat")||
            p.sourceCallerPC!=0x1e8dc0)return false;
        for(std::uint8_t rider=1;rider<=26;++rider){
            if(!riderPortraitIndex(rider))continue;
            std::string name="00_bkg";name[0]=char('0'+rider/10);name[1]=char('0'+rider%10);
            const auto registered="ui\\textures\\"+name;
            if(p.registeredName==registered&&p.attemptedPsmMember==registered+".psm"&&pass.textureName==name&&
                p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::backgrounds,(registered+".pst").c_str()))return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::riderPromptMissingPsmFallback){
        if(p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::rider,"UI\\UIRider\\rider.dat"))return false;
        for(std::size_t i=0;i<4;++i){
            const auto& source=promptSources[i];
            const auto archive=i<2?OriginalUiArchiveId50KG::overlays:OriginalUiArchiveId50KG::rider;
            if(p.archive==originalUiArchiveIdentity50KG(archive)&&p.sourceCallerPC==source.callPC&&
                p.registeredName==source.registeredName&&p.attemptedPsmMember==source.psm&&
                pass.textureName==source.texture&&p.fallbackTexture==catalogMember(archive,source.member))return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::courseNameBlurMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::course,"ui\\textures\\nameblur.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::course,"UI\\UICourse\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2ac6b0&&
            p.registeredName=="ui\\textures\\nameblur"&&p.attemptedPsmMember=="ui\\textures\\nameblur.psm"&&
            pass.textureName=="nameblur";
    if(p.origin==OriginalUiMaterialOrigin50KG::coursePromptMissingPsmFallback){
        if(p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::course,"UI\\UICourse\\track.dat"))return false;
        for(std::size_t i=0;i<4;++i){
            const auto& source=promptSources[i];
            const auto archive=i<2?OriginalUiArchiveId50KG::overlays:OriginalUiArchiveId50KG::course;
            if(p.archive==originalUiArchiveIdentity50KG(archive)&&p.sourceCallerPC==source.callPC&&
                p.registeredName==source.registeredName&&p.attemptedPsmMember==source.psm&&
                pass.textureName==source.texture&&p.fallbackTexture==catalogMember(archive,source.member))return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::tournamentPromptMissingPsmFallback){
        if(p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::tournament,"UI\\UITourn\\standing.dat"))return false;
        for(std::size_t i=0;i<4;++i){
            const auto& source=promptSources[i];
            const auto archive=i<2?OriginalUiArchiveId50KG::overlays:OriginalUiArchiveId50KG::tournament;
            if(p.archive==originalUiArchiveIdentity50KG(archive)&&p.sourceCallerPC==tournamentPromptFirstCallerPC[i]&&
                p.registeredName==source.registeredName&&p.attemptedPsmMember==source.psm&&
                pass.textureName==source.texture&&p.fallbackTexture==catalogMember(archive,source.member))return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::tournamentNameBlurMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\nameblur.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::tournament,"UI\\UITourn\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2ac6b0&&
            p.registeredName=="ui\\textures\\nameblur"&&p.attemptedPsmMember=="ui\\textures\\nameblur.psm"&&
            !catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\nameblur.psm")&&
            pass.textureName=="nameblur";
    if(p.origin==OriginalUiMaterialOrigin50KG::hallNameBlurMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::hallOfFame)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::hallOfFame,"ui\\textures\\nameblur.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::hallOfFame,"UI\\UIHOF\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2ac6b0&&
            p.registeredName=="ui\\textures\\nameblur"&&p.attemptedPsmMember=="ui\\textures\\nameblur.psm"&&
            pass.textureName=="nameblur";
    if(p.origin==OriginalUiMaterialOrigin50KG::hallPromptMissingPsmFallback){
        if(p.fallbackWidgetSource||
            p.fallbackPageSource!=catalogMember(OriginalUiArchiveId50KG::hallOfFame,"UI\\UIHOF\\hall.dat"))return false;
        for(std::size_t i=0;i<4;++i){
            const auto& source=promptSources[i];
            const auto archive=i<2?OriginalUiArchiveId50KG::overlays:OriginalUiArchiveId50KG::hallOfFame;
            if(p.archive==originalUiArchiveIdentity50KG(archive)&&p.sourceCallerPC==source.callPC&&
                p.registeredName==source.registeredName&&p.attemptedPsmMember==source.psm&&
                pass.textureName==source.texture&&p.fallbackTexture==catalogMember(archive,source.member))return true;
        }
        return false;
    }
    if(p.origin==OriginalUiMaterialOrigin50KG::hallFontMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::fonts)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::fonts,"font\\venus16.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::hallOfFame,"UI\\UIHOF\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2dfaa0&&p.registeredName=="font\\venus16"&&
            p.attemptedPsmMember=="font\\venus16.psm"&&pass.textureName=="venus16";
    if(p.origin==OriginalUiMaterialOrigin50KG::controlsNameBlurMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::options,"ui\\textures\\nameblur.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::options,"UI\\UIOption\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2ac6b0&&p.registeredName=="ui\\textures\\nameblur"&&
            p.attemptedPsmMember=="ui\\textures\\nameblur.psm"&&pass.textureName=="nameblur";
    if(p.origin==OriginalUiMaterialOrigin50KG::controlsFontMissingPsmFallback)
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::fonts)&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::fonts,"font\\denmrk12.pst")&&
            p.fallbackWidgetSource==catalogMember(OriginalUiArchiveId50KG::options,"UI\\UIOption\\Widgets.Dat")&&
            !p.fallbackPageSource&&p.sourceCallerPC==0x2dfaa0&&p.registeredName=="font\\denmrk12"&&
            p.attemptedPsmMember=="font\\denmrk12.psm"&&pass.textureName=="denmrk12";
    if(p.origin==OriginalUiMaterialOrigin50KG::controlsPromptMissingPsmFallback){
        const auto& source=promptSources[3];
        return p.archive==originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)&&
            !p.fallbackWidgetSource&&p.fallbackPageSource==catalogMember(OriginalUiArchiveId50KG::options,"UI\\UIOption\\config.dat")&&
            p.fallbackTexture==catalogMember(OriginalUiArchiveId50KG::options,source.member)&&
            p.sourceCallerPC==source.callPC&&p.registeredName==source.registeredName&&
            p.attemptedPsmMember==source.psm&&pass.textureName==source.texture;
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
        if (storage_) return fail(d, Error::alreadyLoaded, "APPSTART");
        if (!archive.identity() || archive.identity()->id != OriginalUiArchiveId50KG::appStart)
            return fail(d, Error::archive, "APPSTART");
        if (archive.materialTemplates_) {
            storage_ = archive.materialTemplates_;
            return true;
        }
        auto next = std::make_shared<OriginalUiMaterialTemplateStorage50KG>();
        if (!loadTemplate(archive, "AppStart\\DefaultMT.psm", next->defaults, d) ||
            !loadTemplate(archive, "AppStart\\ScreenMT.psm", next->screen, d)) return false;
        // Publish neither the archive cache nor this handle until BOTH genuine
        // template loaders completed. Runtime overrides are still pass-local.
        archive.materialTemplates_ = next;
        storage_ = std::move(next);
        return true;
    } catch (...) { d.error = Error::allocation; return false; }
}
const OriginalUiMaterialTemplateState50KG* OriginalUiMaterialTemplates50KG::get(
    OriginalUiTemplate50KG owner) const noexcept {
    if (!storage_) return nullptr;
    if (owner == OriginalUiTemplate50KG::defaultMt) return &storage_->defaults;
    if (owner == OriginalUiTemplate50KG::screenMt || owner == OriginalUiTemplate50KG::nullOwnerScreenFallback)
        return &storage_->screen;
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
bool readOriginalUiRiderPortraitFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    std::uint8_t riderIndex,const OriginalUiMaterialTemplates50KG& templates,
    OriginalUiTemplate50KG owner,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!rider.loaded()||rider.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider))
            return fail(d,Error::archive,"authenticated Rider archive",0x1e9448);
        if(!riderPortraitIndex(riderIndex))return fail(d,Error::fallbackSource,"original Rider portrait index",0x1e8d54);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto name=riderPortraitName(riderIndex);const auto registered="ui\\textures\\"+name;
        const auto texture=rider.find(registered+".pst");const auto page=rider.find("ui\\uirider\\rider.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||rider.find(registered+".psm").identity)
            return fail(d,Error::fallbackSource,"actual Rider portrait and absent original PSM",0x1e9448);
        OriginalUiMaterialProvenance50KG provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::riderPortraitMissingPsmFallback;
        provenance.archive=rider.identity();provenance.fallbackTexture=texture.identity;
        provenance.fallbackPageSource=page.identity;provenance.sourceCallerPC=0x1e9448;
        provenance.registeredName=registered;provenance.attemptedPsmMember=registered+".psm";
        provenance.creationPC=0x2475c0;
        auto result=screenTextureFallback(provenance,name.c_str(),*inherited,runtime);
        output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiRiderNameBlurFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!rider.loaded()||rider.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider))
            return fail(d,Error::archive,"authenticated Rider",0x2ac6b0);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=rider.find("ui\\textures\\nameblur.pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
            rider.find("ui\\textures\\nameblur.psm").identity)
            return fail(d,Error::fallbackSource,"actual Rider nameblur and absent PSM",0x29b638);
        OriginalUiDat50KG widgets;
        if(!readOriginalUiDat50KG(rider,"UI\\UIRider\\Widgets.Dat",widgets,d.document,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::document,"Rider widgets",0x2ac650);
        const auto* type=widgets.find("Widget 88","Type");const auto* name=widgets.find("Widget 88","Texture");
        if(!type||type->text!="Icon"||!name||name->text!="nameblur")
            return fail(d,Error::fallbackSource,"Rider Icon88 nameblur",0x2ac650);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::riderNameBlurMissingPsmFallback;
        p.archive=rider.identity();p.fallbackTexture=texture.identity;p.fallbackWidgetSource=widgets.identity();
        p.sourceCallerPC=0x2ac6b0;p.registeredName="ui\\textures\\nameblur";
        p.attemptedPsmMember="ui\\textures\\nameblur.psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,"nameblur",*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiControlsFallbackMaterial50KG(const OriginalUiArchive50KG& options,
    const OriginalUiArchive50KG& fonts,OriginalUiControlsFallback50KG kind,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try{
        if(!options.loaded()||options.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::options)||
            !fonts.loaded()||fonts.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::fonts))
            return fail(d,Error::archive,"authenticated Controls/Fonts",0x1f2fd0);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"actual ScreenMT/runtime",0x2df10c);
        OriginalUiMaterialProvenance50KG p;const char* name=nullptr;
        const OriginalUiArchive50KG* archive=&options;
        if(kind==OriginalUiControlsFallback50KG::circle){
            const auto& source=promptSources[3];name=source.texture;
            p.origin=OriginalUiMaterialOrigin50KG::controlsPromptMissingPsmFallback;
            p.registeredName=source.registeredName;p.attemptedPsmMember=source.psm;p.sourceCallerPC=source.callPC;
            p.fallbackPageSource=options.find("UI/UIOption/config.dat").identity;
            if(!p.fallbackPageSource)return fail(d,Error::fallbackSource,"Controls page",0x1f2fe8);
        }else{
            OriginalUiDat50KG widgets;
            if(!readOriginalUiDat50KG(options,"UI/UIOption/Widgets.Dat",widgets,d.document,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
                return fail(d,Error::document,"Controls widgets",0x2ac650);
            const bool font=kind==OriginalUiControlsFallback50KG::smallFont;
            if(!font&&kind!=OriginalUiControlsFallback50KG::nameBlur)return fail(d,Error::fallbackSource,"Controls fallback kind");
            const auto* type=widgets.find(font?"Widget 129":"Widget 131","Type");
            const auto* reference=widgets.find(font?"Widget 129":"Widget 131",font?"FontName":"Texture");
            name=font?"denmrk12":"nameblur";
            if(!type||type->text!=(font?"Overlay_Text":"Icon")||!reference||reference->text!=name)
                return fail(d,Error::fallbackSource,"authentic Controls descriptor",0x2ac650);
            if(font)archive=&fonts;
            p.origin=font?OriginalUiMaterialOrigin50KG::controlsFontMissingPsmFallback:OriginalUiMaterialOrigin50KG::controlsNameBlurMissingPsmFallback;
            p.registeredName=std::string(font?"font\\":"ui\\textures\\")+name;
            p.attemptedPsmMember=p.registeredName+".psm";p.fallbackWidgetSource=widgets.identity();p.sourceCallerPC=font?0x2dfaa0:0x2ac6b0;
        }
        const auto texture=archive->find(p.registeredName+".pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
            options.find(p.attemptedPsmMember).identity||fonts.find(p.attemptedPsmMember).identity)
            return fail(d,Error::fallbackSource,"actual Controls texture and absent PSM",0x29b638);
        p.archive=archive->identity();p.fallbackTexture=texture.identity;p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,name,*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiRiderBackgroundFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    const OriginalUiArchive50KG& backgrounds,std::uint8_t riderIndex,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!rider.loaded()||rider.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||
            !backgrounds.loaded()||backgrounds.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::backgrounds))
            return fail(d,Error::archive,"authenticated Rider and UIBKG",0x1e8dc0);
        if(!riderPortraitIndex(riderIndex))return fail(d,Error::fallbackSource,"original Rider background index",0x1e8d58);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        std::string name="00_bkg";name[0]=char('0'+riderIndex/10);name[1]=char('0'+riderIndex%10);
        const auto registered="ui\\textures\\"+name;
        const auto texture=backgrounds.find(registered+".pst");const auto page=rider.find("UI\\UIRider\\rider.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            rider.find(registered+".psm").identity||backgrounds.find(registered+".psm").identity)
            return fail(d,Error::fallbackSource,"actual UIBKG image and absent PSM",0x1e8dc0);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::riderBackgroundMissingPsmFallback;
        p.archive=backgrounds.identity();p.fallbackTexture=texture.identity;p.fallbackPageSource=page.identity;
        p.sourceCallerPC=0x1e8dc0;p.registeredName=registered;p.attemptedPsmMember=registered+".psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,name.c_str(),*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiRiderPromptFallbackMaterial50KG(const OriginalUiArchive50KG& rider,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!rider.loaded()||rider.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Rider and OVERLAYS",0x1e8074);
        const auto index=static_cast<std::size_t>(prompt);
        if(index>=4)return fail(d,Error::fallbackSource,"original shared Rider page prompt");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto& textureOwner=index<2?overlays:rider;
        const auto texture=textureOwner.find(source.member);const auto page=rider.find("UI\\UIRider\\rider.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||rider.find(source.psm).identity||(index<2&&rider.find(source.member).identity))
            return fail(d,Error::fallbackSource,"actual shared/local Rider prompt and absent PSM",source.callPC);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::riderPromptMissingPsmFallback;
        p.archive=textureOwner.identity();p.fallbackTexture=texture.identity;p.fallbackPageSource=page.identity;
        p.sourceCallerPC=source.callPC;p.registeredName=source.registeredName;p.attemptedPsmMember=source.psm;p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,source.texture,*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiCourseNameBlurFallbackMaterial50KG(const OriginalUiArchive50KG& course,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!course.loaded()||course.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course))
            return fail(d,Error::archive,"authenticated Course",0x2ac6b0);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=course.find("ui\\textures\\nameblur.pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
            course.find("ui\\textures\\nameblur.psm").identity)
            return fail(d,Error::fallbackSource,"actual Course nameblur and absent PSM",0x29b638);
        OriginalUiDat50KG widgets;
        if(!readOriginalUiDat50KG(course,"UI\\UICourse\\Widgets.Dat",widgets,d.document,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::document,"Course widgets",0x2ac650);
        const auto* type=widgets.find("Widget 6","Type");const auto* name=widgets.find("Widget 6","Texture");
        if(!type||type->text!="Icon"||!name||name->text!="nameblur")
            return fail(d,Error::fallbackSource,"Course Icon6 nameblur",0x2ac650);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::courseNameBlurMissingPsmFallback;
        p.archive=course.identity();p.fallbackTexture=texture.identity;p.fallbackWidgetSource=widgets.identity();
        p.sourceCallerPC=0x2ac6b0;p.registeredName="ui\\textures\\nameblur";
        p.attemptedPsmMember="ui\\textures\\nameblur.psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,"nameblur",*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiCoursePromptFallbackMaterial50KG(const OriginalUiArchive50KG& course,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!course.loaded()||course.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Course and OVERLAYS",0x1d8f10);
        const auto index=static_cast<std::size_t>(prompt);
        if(index>=4)return fail(d,Error::fallbackSource,"original shared Course page prompt");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto& textureOwner=index<2?overlays:course;
        const auto texture=textureOwner.find(source.member);const auto page=course.find("UI\\UICourse\\track.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||course.find(source.psm).identity||(index<2&&course.find(source.member).identity))
            return fail(d,Error::fallbackSource,"actual shared/local Course prompt and absent PSM",source.callPC);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::coursePromptMissingPsmFallback;
        p.archive=textureOwner.identity();p.fallbackTexture=texture.identity;p.fallbackPageSource=page.identity;
        p.sourceCallerPC=source.callPC;p.registeredName=source.registeredName;p.attemptedPsmMember=source.psm;p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,source.texture,*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiTournamentPromptFallbackMaterial50KG(const OriginalUiArchive50KG& tournament,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!tournament.loaded()||tournament.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Tournament and OVERLAYS",0x1d8f10);
        const auto index=static_cast<std::size_t>(prompt);
        if(index>=4)return fail(d,Error::fallbackSource,"original shared Tournament page prompt");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto& textureOwner=index<2?overlays:tournament;
        const auto texture=textureOwner.find(source.member);const auto page=tournament.find("UI\\UITourn\\standing.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||tournament.find(source.psm).identity||(index<2&&tournament.find(source.member).identity))
            return fail(d,Error::fallbackSource,"actual shared/local Tournament prompt and absent PSM",source.callPC);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::tournamentPromptMissingPsmFallback;
        p.archive=textureOwner.identity();p.fallbackTexture=texture.identity;p.fallbackPageSource=page.identity;
        p.sourceCallerPC=tournamentPromptFirstCallerPC[index];p.registeredName=source.registeredName;p.attemptedPsmMember=source.psm;p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,source.texture,*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiTournamentNameBlurFallbackMaterial50KG(const OriginalUiArchive50KG& tournament,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!tournament.loaded()||tournament.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
            return fail(d,Error::archive,"authenticated Tournament",0x2ac6b0);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=tournament.find("ui\\textures\\nameblur.pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
            tournament.find("ui\\textures\\nameblur.psm").identity)
            return fail(d,Error::fallbackSource,"actual Tournament nameblur and absent PSM",0x29b638);
        OriginalUiDat50KG widgets;
        if(!readOriginalUiDat50KG(tournament,"UI\\UITourn\\Widgets.Dat",widgets,d.document,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::document,"Tournament widgets",0x2ac650);
        const auto* type=widgets.find("Widget 30","Type");const auto* name=widgets.find("Widget 30","Texture");
        if(!type||type->text!="Icon"||!name||name->text!="nameblur")
            return fail(d,Error::fallbackSource,"Tournament Icon30 nameblur",0x2ac650);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::tournamentNameBlurMissingPsmFallback;
        p.archive=tournament.identity();p.fallbackTexture=texture.identity;p.fallbackWidgetSource=widgets.identity();
        p.sourceCallerPC=0x2ac6b0;p.registeredName="ui\\textures\\nameblur";
        p.attemptedPsmMember="ui\\textures\\nameblur.psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,"nameblur",*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiHallNameBlurFallbackMaterial50KG(const OriginalUiArchive50KG& hall,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!hall.loaded()||hall.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::hallOfFame))
            return fail(d,Error::archive,"authenticated Hall",0x2ac6b0);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto texture=hall.find("ui\\textures\\nameblur.pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
            hall.find("ui\\textures\\nameblur.psm").identity)
            return fail(d,Error::fallbackSource,"actual Hall nameblur and absent PSM",0x29b638);
        OriginalUiDat50KG widgets;
        if(!readOriginalUiDat50KG(hall,"UI\\UIHOF\\Widgets.Dat",widgets,d.document,
            OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))return fail(d,Error::document,"Hall widgets",0x2ac650);
        const auto* type=widgets.find("Widget 6","Type");const auto* name=widgets.find("Widget 6","Texture");
        if(!type||type->text!="Icon"||!name||name->text!="nameblur")
            return fail(d,Error::fallbackSource,"Hall Icon6 nameblur",0x2ac650);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::hallNameBlurMissingPsmFallback;
        p.archive=hall.identity();p.fallbackTexture=texture.identity;p.fallbackWidgetSource=widgets.identity();
        p.sourceCallerPC=0x2ac6b0;p.registeredName="ui\\textures\\nameblur";
        p.attemptedPsmMember="ui\\textures\\nameblur.psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,"nameblur",*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiHallPromptFallbackMaterial50KG(const OriginalUiArchive50KG& hall,
    const OriginalUiArchive50KG& overlays,OriginalUiMainPrompt50KG prompt,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!hall.loaded()||hall.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::hallOfFame)||
            !overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Hall and OVERLAYS",0x1d8f10);
        const auto index=static_cast<std::size_t>(prompt);
        if(index>=4)return fail(d,Error::fallbackSource,"original shared Hall page prompt");
        const auto& source=promptSources[index];const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenTexture ScreenMT/runtime owner",0x2df10c);
        const auto& textureOwner=index<2?overlays:hall;
        const auto texture=textureOwner.find(source.member);const auto page=hall.find("UI\\UIHOF\\hall.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            overlays.find(source.psm).identity||hall.find(source.psm).identity||(index<2&&hall.find(source.member).identity))
            return fail(d,Error::fallbackSource,"actual shared/local Hall prompt and absent PSM",source.callPC);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::hallPromptMissingPsmFallback;
        p.archive=textureOwner.identity();p.fallbackTexture=texture.identity;p.fallbackPageSource=page.identity;
        p.sourceCallerPC=source.callPC;p.registeredName=source.registeredName;p.attemptedPsmMember=source.psm;p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,source.texture,*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiHallFontFallbackMaterial50KG(const OriginalUiArchive50KG& hall,
    const OriginalUiArchive50KG& fonts,const OriginalUiMaterialTemplates50KG& templates,
    OriginalUiTemplate50KG owner,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try{
        if(!hall.loaded()||hall.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::hallOfFame)||
            !fonts.loaded()||fonts.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::fonts))
            return fail(d,Error::archive,"authenticated Hall/Fonts",0x2a9940);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"actual ScreenMT/runtime",0x2df10c);
        OriginalUiDat50KG widgets;
        if(!readOriginalUiDat50KG(hall,"UI/UIHOF/Widgets.Dat",widgets,d.document,OriginalUiRepeatedKeyPolicy50KG::preserveInOrder))
            return fail(d,Error::document,"Hall widgets",0x2a9000);
        for(const char* id:{"Widget 17","Widget 18"}){
            const auto* type=widgets.find(id,"Type");const auto* font=widgets.find(id,"FontName");
            if(!type||type->text!="Overlay_Text"||!font||font->text!="venus16"||widgets.find(id,"MaterialName"))
                return fail(d,Error::fallbackSource,"authentic blank Hall venus16 descriptor",0x2a90b8);
        }
        const auto texture=fonts.find("font/venus16.pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
            hall.find("font/venus16.psm").identity||fonts.find("font/venus16.psm").identity)
            return fail(d,Error::fallbackSource,"actual venus16 texture and absent Hall PSM",0x29b638);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::hallFontMissingPsmFallback;
        p.archive=fonts.identity();p.fallbackTexture=texture.identity;p.fallbackWidgetSource=widgets.identity();
        p.sourceCallerPC=0x2dfaa0;p.registeredName="font\\venus16";
        p.attemptedPsmMember="font\\venus16.psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,"venus16",*inherited,runtime);output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalRaceHudFontFallbackMaterial50KG(const OriginalUiArchive50KG& overlays,
    const OriginalUiMaterialTemplates50KG& templates,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated race OVERLAYS",0x1a8dd4);
        const auto* inherited=templates.get(OriginalUiTemplate50KG::screenMt);
        if(!inherited||!runtime.overrideKnown)return fail(d,Error::templateOwner,"actual ScreenMT/runtime",0x2dfaa0);
        const auto texture=overlays.find("font\\denmarkbold16ingame.pst");
        const auto cel=overlays.find("font\\denmarkbold16ingame.cel");
        if(!texture.identity||!texture.data||!cel.identity||!cel.data||
            overlays.find("font\\denmarkbold16ingame.psm").identity)
            return fail(d,Error::fallbackSource,"actual in-game16 font and absent PSM",0x1a8dd4);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::raceHudFontMissingPsmFallback;
        p.archive=overlays.identity();p.fallbackTexture=texture.identity;p.sourceCallerPC=0x1a8dd4;
        p.registeredName="font\\denmarkbold16ingame";p.attemptedPsmMember=p.registeredName+".psm";p.creationPC=0x2475c0;
        auto result=screenTextureFallback(p,"denmarkbold16ingame",*inherited,runtime);
        output=std::move(result);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
static bool prepareOriginalUiOneMapMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const Texture48E& texture, OriginalUiNativeMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d, bool litProgram,
    bool& fogEnabled, bool& alphaTestGreaterZero, bool authenticatedRiderIpum = false,
    bool authenticatedRiderIndexedBody = false, bool authenticatedRiderSpecular = false,
    bool authenticatedRiderWindow = false, bool authenticatedRiderShadow = false,
    bool authenticatedCharacterFixed64 = false, bool authenticatedControlsTga1555 = false,
    bool authenticatedRaceGleam = false, bool authenticatedRaceClear = false,
    bool authenticatedRider17Cutout = false, bool canonicalPauseAnimation = false,
    bool authenticatedAa1Ipum = false) noexcept {
    d = {}; d.pass = pass.pass;
    try {
        if (pass.textureCount == 0)
            return fail(d, Error::untexturedAdapterRequired, "zero-map requires untextured material policy", 0x2474b8u);
        const bool programMatches = litProgram ?
            ((same(pass.microprogram, "Diffuse") || same(pass.microprogram, "DiffuseCompressed") ||
              (authenticatedRiderSpecular && same(pass.microprogram,"SpecularMapCompressed"))) &&
             pass.provenance.origin == OriginalUiMaterialOrigin50KG::authoredPsm &&
             pass.inheritedTemplate == catalogMember(OriginalUiArchiveId50KG::appStart,
                 "AppStart\\DefaultMT.psm")) : pass.microprogram == "ScreenObject";
        if (!sourceIdentityKnown(pass) || !pass.inheritedTemplate || !programMatches ||
            pass.enabled != 1 || pass.textureCount != 1 || pass.textureCoordinateIndex != 0)
            return fail(d, Error::profile, litProgram ? "authored DefaultMT Diffuse/Compressed" :
                "enabled single-map ScreenObject", 0x2471bcu);
        for (const auto& field : pass.authored.values)
            if (field.sourceValuePresent && !knownPassField(field.key) &&
                !(canonicalPauseAnimation && same(field.key,"animatesWhilePaused")))
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
            (!litProgram && !expect(Field::foggingOnOff, 0)) || !expect(Field::onePassAntiAliasingOnOff, authenticatedAa1Ipum ? 1 : 0) ||
            !expect(Field::textureCoordinatesSTQorUV, 0) ||
            !expect(Field::environmentContext0or1, 0) ||
            !expect(Field::fragmentValueControlFixedOrNot, 0) ||
            (!litProgram && !expect(Field::alphaTestOnOff, authenticatedRaceClear?1:0)) ||
            !expect(Field::destinationAlphaTestOnOff, 0)) return false;
        if(authenticatedRaceClear && (!expect(Field::alphaTestMethod,0) ||
            !expect(Field::alphaReferenceValue,0) ||
            !expect(Field::processingMethodWhenFailedInAlphaTest,1)))return false;
        const bool sourceFog = litProgram && value(Field::foggingOnOff) != 0;
        const bool sourceAlphaTest = litProgram && value(Field::alphaTestOnOff) != 0;
        if (sourceAlphaTest && (!expect(Field::alphaTestMethod, 6) ||
            !expect(Field::alphaReferenceValue, 0) ||
            !expect(Field::processingMethodWhenFailedInAlphaTest, 0))) return false;
        const bool textureShape = authenticatedRider17Cutout ?
            (litProgram && texture.kind == 1 && texture.mips.size() == 4) : authenticatedControlsTga1555 ?
            (litProgram && texture.kind==originalUiControlsTga1555Kind50KG && texture.mips.size()==1) : authenticatedRiderIpum ?
            (litProgram && texture.kind == 2 && !texture.mips.empty() && texture.mips.size() <= originalUiIpumMipCount50KG) :
            authenticatedRiderIndexedBody ? (litProgram && texture.kind == 1 && !texture.mips.empty() && texture.mips.size() <= 5) :
            authenticatedRiderWindow ? (litProgram && texture.kind == 1 && (texture.mips.size() == 3 || texture.mips.size() == 4)) :
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
        // cold-caches each exact atlas maximum. The ordinary RGB-only route
        // requires vertexAlpha * atlasMaximumAlpha <= 128*128 before blended
        // writes; its fixed-function factor cannot express larger values.
        // The sealed source-RGBA route instead evaluates admitted byte factors
        // explicitly and preserves independent source-alpha replacement.
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
        if(authenticatedRaceGleam){
            if(!expect(Field::alphaBlendingOnOff,1)||!expect(Field::inputColorA,0)||
                !expect(Field::inputColorB,2)||!expect(Field::inputAlphaC,1)||
                !expect(Field::inputColorD,1))return false;
            policy.blend=OriginalUiMaterialBlend50KG::destinationAlphaAdditive;
        } else if (authenticatedRiderSpecular) {
            if (!expect(Field::alphaBlendingOnOff,1) || !expect(Field::inputColorA,0) ||
                !expect(Field::inputColorB,2) || !expect(Field::inputColorD,1) ||
                !expect(Field::zWriteDisabled,authenticatedCharacterFixed64?0:1))return false;
            if(authenticatedCharacterFixed64){
                if(!expect(Field::inputAlphaC,2)||!expect(Field::fixedAlphaValue,64))return false;
                policy.blend=OriginalUiMaterialBlend50KG::fixed64Additive;
            }else if (value(Field::inputAlphaC)==1)policy.blend=OriginalUiMaterialBlend50KG::destinationAlphaAdditive;
            else if (value(Field::inputAlphaC)==2 && expect(Field::fixedAlphaValue,255))
                policy.blend=OriginalUiMaterialBlend50KG::fixed255Additive;
            else return fail(d,Error::unsupportedValue,"actual Rider specular ALPHA.C/FIX",0x247a64u);
        } else if (authenticatedRiderShadow) {
            if (!expect(Field::alphaBlendingOnOff,1) || !expect(Field::inputColorA,2) ||
                !expect(Field::inputColorB,1) || !expect(Field::inputAlphaC,0) ||
                !expect(Field::inputColorD,1))return false;
            policy.blend=OriginalUiMaterialBlend50KG::destinationSourceAlphaAttenuation;
        } else if (blend == 1) {
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
bool prepareOriginalPauseAnimatedMaterial50LA(const OriginalUiMaterialPass50KG& pass,
    const Texture48E& texture,const OriginalStandardMaterialTextureAnimation50LA& controller,
    OriginalUiNativeMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try {
        const bool left=pass.identity==catalogMember(OriginalUiArchiveId50KG::overlays,
            "ui\\textures\\arrwleftPause.psm");
        const bool right=pass.identity==catalogMember(OriginalUiArchiveId50KG::overlays,
            "ui\\textures\\arrwrghtPause.psm");
        const auto* owner=controller.materialOwner();
        const auto* source=owner?owner->source():nullptr;
        OriginalStandardMaterialPassView50LA canonical;
        const auto* animation=pass.authored.find("TextureAnimation");
        bool paused=false;
        if((!left&&!right)||!controller.unique18()||!owner||!owner->constructed29B550()||
           !source||!source->member()||!same(*source->member(),pass.identity->name)||
           pass.pass!=controller.passOrdinal()+1||
           !owner->pass4C(controller.passOrdinal(),canonical)||canonical.textureAnimation10!=&controller||
           !animation||!same(animation->text,"unique")||
           !pass.authored.find("animatesWhilePaused")||
           !originalUiBoolean50KG(pass.authored,"animatesWhilePaused",false,paused,d.document,0x29AB30)||
           paused!=controller.animatesWhilePaused18())
            return fail(d,Error::profile,"actual Pause authored animation controller",0x29AB30);
        bool fog=false,alphaTest=false;
        return prepareOriginalUiOneMapMaterial50KM(pass,texture,output,d,false,fog,alphaTest,
            false,false,false,false,false,false,false,false,false,false,true);
    }catch(...){d.error=Error::allocation;return false;}
}
bool prepareOriginalRaceHudGleamMaterial50KG(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t index,
    OriginalRaceHudGleamMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try{
    const bool bright=pass.identity==catalogMember(OriginalUiArchiveId50KG::overlays,"ui\\textures\\gleam.psm");
    const bool clear=pass.identity==catalogMember(OriginalUiArchiveId50KG::overlays,"ui\\textures\\gleamclear.psm");
    const auto* source=images.sourceProvenance(index);const auto* texture=images.decodedTexture(index);
    if((!bright&&!clear)||pass.pass!=1||!same(pass.textureName,"gleam")||
        !same(pass.shaderPassGroup,"none")||
        pass.inheritedTemplate!=catalogMember(OriginalUiArchiveId50KG::appStart,"AppStart\\ScreenMT.psm")||
        !source||source->memberIdentity!=catalogMember(OriginalUiArchiveId50KG::overlays,"ui\\textures\\gleam.pst")||
        source->celIdentity||source->pstOffset||source->pstBytes!=604||!texture)
        return fail(d,Error::profile,"actual race gleam/gleamclear and atlas",0x2e6770);
    OriginalRaceHudGleamMaterial50KG next;bool fog=false,alphaTest=false;
    if(!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.raster,d,false,fog,alphaTest,
        false,false,false,false,false,false,false,bright,clear))return false;
    if(next.raster.depthMethod!=2||!next.raster.depthWrite||fog||alphaTest)
        return fail(d,Error::unsupportedValue,"original ScreenMT gleam depth",0x2e6770);
    next.clearFramebufferOnly=clear;output=next;return true;
    }catch(...){d.error=Error::allocation;return false;}
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
bool prepareOriginalUiControlsSpecularMaterial50KG(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t index,
    OriginalUiControlsSpecularMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try{
        bool material=false;
        for(const auto* name:{"ps2_blue","ps2_bluedark","ps2_bluelight","ps2_buttons"})
            material|=pass.identity==catalogMember(OriginalUiArchiveId50KG::options,(std::string("ui\\textures\\")+name+".psm").c_str());
        const auto* source=images.sourceProvenance(index);const auto* texture=images.decodedTexture(index);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(index):nullptr;
        if(!material||pass.pass!=2||!same(pass.microprogram,"SpecularMapCompressed")||!same(pass.textureName,"specmap3")||
            !source||!isOriginalUiControlsSpecmap50KG(source->memberIdentity)||source->pstOffset||source->pstBytes!=196652||source->celIdentity||
            !texture||!image||texture->kind!=originalUiControlsTga1555Kind50KG||texture->mips.size()!=1||
            pass.gs[Field::sourceAlphaValueWhenAis0].value!=255||pass.gs[Field::sourceAlphaValueWhenAis1].value!=255||
            pass.gs[Field::methodOfExpandingTextureAlpha].value!=0||
            pass.gs[Field::inputAlphaC].value!=1)return fail(d,Error::texture,"actual Controls specmap3 TGA-to1555 second pass",0x2476a0);
        const auto& mip=texture->mips[0];if(mip.width!=256||mip.height!=256||image->texture.rgba!=mip.rgba)
            return fail(d,Error::texture,"actual Controls specmap3 image bank",0x249780);
        OriginalUiControlsSpecularMaterial50KG next;
        if(!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,true,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::specularMapCompressed;next.images=&images;next.imageIndex=index;next.imageIdentity=image->identity;
        output=next;return true;
    }catch(...){d.error=Error::allocation;return false;}
}
namespace {
// Finite original identities only. A name/suffix or compatible layout cannot
// authorize a different rider, member, or replacement image.
// Clegg03 identities match the retained04 material states; these historical
// helper names do not grant profile unlock or preview-owner capability.
// Requested09/23/24 body and shadow PSM states equal04 after replacing
// their own texture names (only trailing whitespace differs);09c_1
// likewise equals04c_1. Remaining explicit identities retain the same gates:
// body pairs match04_1, shadows match04_shdw, opaque IPUM matches04c_1.
// 18c_1 inherits ABE=0 and declares only pass1; its spare pass2 stays inactive.
// 08c_3 has the existing03c_3 FIX64 three-pass state and one packed-kind0 mip.
// 25c_7 AA1 and22_3 special programs remain unadmitted.
// 17_3 uses its separate exact-source cutout policy, never these identity sets.
const char* unlockedIpumName(const OriginalUiMaterialPass50KG& pass) {
    constexpr const char* names[]{"19c_1","03c_1","03c_2","04c_1","04c_2","10c_1","10c_2","12c_1","15c_1","15c_2","07c_2","09c_1",
        "01c_1","01c_2","08c_1","08c_2","14c_2","16c_1","16c_2","17c_1","17c_2","18c_1","18c_2","22c_1","25c_1","25c_2","25c_3","25c_4","25c_5"};
    if(!pass.identity)return nullptr;
    for(const auto* name:names){
        const auto member=std::string("ui\\textures\\")+name+".psm";
        if(pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,member.c_str()))return name;
    }
    return nullptr;
}
const char* unlockedShadowName(const OriginalUiMaterialPass50KG& pass) {
    // Original26_shdw.psm is byte-identical to25_shdw.psm and names25_shdw.pst.
    // Preserve that authored cross-rider source rather than substituting26_shdw.pst.
    if(pass.identity&&pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\26_shdw.psm"))return "25_shdw";
    constexpr const char* names[]{"02_shdw","03_shdw","04_shdw","07_shdw","10_shdw","11_shdw","12_shdw","15_shdw","19_shdw","20_shdw","09_shdw","23_shdw","24_shdw",
        "01_shdw","08_shdw","14_shdw","16_shdw","17_shdw","18_shdw","21_shdw","22_shdw","25_shdw"};
    if(!pass.identity)return nullptr;
    for(const auto* name:names){
        const auto member=std::string("ui\\textures\\")+name+".psm";
        if(pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,member.c_str()))return name;
    }
    return nullptr;
}
const char* unlockedBodyName(const OriginalUiMaterialPass50KG& pass) {
    constexpr const char* names[]{"02_1","02_2","03_1","03_2","04_1","04_2","07_1","07_2","10_1","10_2","11_1","11_2",
        "12_1","12_2","15_1","15_2","19_1","19_2","20_1","20_2","09_1","09_2","23_1","23_2","24_1","24_2",
        "01_1","01_2","08_1","08_2","14_1","14_2","16_1","16_2","17_1","17_2","18_1","18_2","21_1","21_2","22_1","22_2","25_1","25_2","26_1","26_2"};
    if(!pass.identity)return nullptr;
    for(const auto* name:names){
        const auto member=std::string("ui\\textures\\")+name+".psm";
        if(pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,member.c_str()))return name;
    }
    return nullptr;
}
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
        const auto* name=unlockedIpumName(pass);
        if(!name||pass.pass!=1||!same(pass.microprogram,"DiffuseCompressed")||!same(pass.textureName,name))
            return fail(d,Error::profile,"canonical unlocked Rider IPUM first diffuse pass",0x2476a0u);
        const auto memberName=std::string("ui\\textures\\")+name+".pst";
        const auto* member=catalogMember(OriginalUiArchiveId50KG::rider,memberName.c_str());
        if (!images.loaded() || !source || !ipum || !texture || !image || !member ||
            source->memberIdentity != member || source->celIdentity || source->pstOffset != 0 ||
            source->pstBytes != member->bytes || ipum->sourcePstSha256 != member->sha256 ||
            !same(ipum->sourceMember,member->name) ||
            ipum->sourceArchiveSha256 != originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)->sha256 ||
            ipum->colorPolicyKind != OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1 ||
            !ipum->mipCount || ipum->mipCount > originalUiIpumMipCount50KG ||
            texture->mips.size() != ipum->mipCount)
            return fail(d,Error::texture,"authenticated per-source unlocked Rider IPUM image bank",0x24e880u);
        const auto& base=texture->mips.back();
        if (image->texture.width != base.width || image->texture.height != base.height ||
            image->texture.rgba != base.rgba)
            return fail(d,Error::texture,"published native image must equal original largest mip",0x24eb90u);
        // Preserve the established19 contract; additions still pass every
        // source-backed common lit raster/sampler/blend validation below.
        if((same(name,"19c_1")&&texture->mips.size()!=5)||
           pass.gs[Field::alphaBlendingOnOff].value!=0||pass.gs[Field::alphaTestOnOff].value!=0)
            return fail(d,Error::profile,"actual opaque alpha-test-off Rider IPUM pass",0x2476a0u);
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
bool prepareOriginalUiAa1IpumMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19IpumMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d = {}; d.pass = pass.pass;
    try {
        const auto* source = images.sourceProvenance(imageIndex);
        const auto* ipum = images.ipumProvenance(imageIndex);
        const auto* texture = images.decodedTexture(imageIndex);
        const auto* bank = images.textures();
        const auto* image = bank ? bank->texture(imageIndex) : nullptr;
        const auto* name=pass.textureName.c_str();
        const auto psmName=std::string("ui\\textures\\")+name+".psm";
        if(!*name||pass.identity!=catalogMember(OriginalUiArchiveId50KG::rider,psmName.c_str())||
           pass.pass!=1||!same(pass.microprogram,"DiffuseCompressed"))
            return fail(d,Error::profile,"authenticated UI IPUM AA1 first diffuse pass",0x2476a0u);
        const auto memberName=std::string("ui\\textures\\")+name+".pst";
        const auto* member=catalogMember(OriginalUiArchiveId50KG::rider,memberName.c_str());
        if (!images.loaded() || !source || !ipum || !texture || !image || !member ||
            source->memberIdentity != member || source->celIdentity || source->pstOffset != 0 ||
            source->pstBytes != member->bytes || ipum->sourcePstSha256 != member->sha256 ||
            !same(ipum->sourceMember,member->name) ||
            ipum->sourceArchiveSha256 != originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider)->sha256 ||
            ipum->colorPolicyKind != OriginalUiIpumColorPolicy50KG::original50FcRgb5Texa255Aem1 ||
            !ipum->mipCount || ipum->mipCount > originalUiIpumMipCount50KG ||
            texture->mips.size() != ipum->mipCount)
            return fail(d,Error::texture,"authenticated per-source unlocked Rider IPUM image bank",0x24e880u);
        const auto& base=texture->mips.back();
        if (image->texture.width != base.width || image->texture.height != base.height ||
            image->texture.rgba != base.rgba)
            return fail(d,Error::texture,"published native image must equal original largest mip",0x24eb90u);
        // AA1 is retained, not rewritten. This shared capability supports the
        // authored source-alpha tuple; other blend/alpha-test modes remain closed.
        if(pass.gs[Field::alphaBlendingOnOff].value!=1 ||
           pass.gs[Field::onePassAntiAliasingOnOff].value!=1 ||
           pass.gs[Field::alphaTestOnOff].value!=0 ||
           pass.gs[Field::inputColorA].value!=0 || pass.gs[Field::inputColorB].value!=1 ||
           pass.gs[Field::inputAlphaC].value!=0 || pass.gs[Field::inputColorD].value!=1)
            return fail(d,Error::profile,"source-alpha AA1 alpha-test-off IPUM pass",0x2476a0u);
        OriginalUiRider19IpumMaterial50KM next;
        if(!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,true,
            false,false,false,false,false,false,false,false,false,false,true))return false;
        next.material.coverage=OriginalUiCoveragePolicy50KM::fourSampleAa1;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIpumImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=static_cast<std::uint32_t>(texture->mips.size()-1);
        next.sourceMipCount=static_cast<std::uint32_t>(texture->mips.size());
        output=next;return true;
    } catch (...) { d.error=Error::allocation; return false; }
}
bool prepareOriginalUiRider19BodyMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19BodyMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        const auto* name=unlockedBodyName(pass);
        const bool original19=name&&(same(name,"19_1")||same(name,"19_2"));
        if(!name||pass.pass!=1||!same(pass.microprogram,"DiffuseCompressed")||!same(pass.textureName,name)||
           (pass.gs[Field::alphaBlendingOnOff].value!=0||pass.gs[Field::alphaTestOnOff].value!=0))
            return fail(d,Error::profile,"canonical unlocked Rider indexed body first pass",0x2476a0u);
        const auto memberName=std::string("ui\\textures\\")+name+".pst";
        const auto* member=catalogMember(OriginalUiArchiveId50KG::podTextures,memberName.c_str());
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();
        const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.empty() || texture->mips.size()>5 || (original19&&texture->mips.size()!=5))
            return fail(d,Error::texture,"authenticated original UIPODTEX body image bank",0x24e880u);
        const auto& base=texture->mips.back();
        if ((original19&&(base.width!=256 || base.height!=256)) || image->texture.width!=base.width ||
            image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published native body image must equal original largest mip",0x24eb90u);
        OriginalUiRider19BodyMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=static_cast<std::uint32_t>(texture->mips.size()-1);
        next.sourceMipCount=static_cast<std::uint32_t>(texture->mips.size());
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiRider17CutoutMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider17CutoutMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        const auto* psm=catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\17_3.psm");
        if(!psm || pass.identity!=psm ||
            pass.provenance.archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::rider) ||
            pass.provenance.origin!=OriginalUiMaterialOrigin50KG::authoredPsm ||
            pass.pass!=1 || !same(pass.microprogram,"DiffuseCompressed") || !same(pass.textureName,"17_3") ||
            !same(pass.shaderPassGroup,"none") || pass.enabled!=1 || pass.textureCount!=1 || pass.textureCoordinateIndex!=0 ||
            pass.lodCalculationMethod!=0 || pass.maximumMipLevel!=0 || pass.magFilter!=1 || pass.minFilter!=5 ||
            pass.lodL!=0 || pass.lodK!=-75)
            return fail(d,Error::profile,"canonical Rider17_3 single cutout pass/sampler",0x2476a0u);
        // Every canonical GS value is the constructor value except REF=0,
        // inherited from authenticated DefaultMT. Runtime overrides fail closed.
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
            const auto expected=i==static_cast<std::size_t>(Field::alphaReferenceValue)?0:specs[i].initial;
            if(pass.gs.fields[i].value!=expected)
                return fail(d,Error::unsupportedValue,specs[i].name,pass.gs.fields[i].sourcePC);
        }
        const auto* member=catalogMember(OriginalUiArchiveId50KG::podTextures,"ui\\textures\\17_3.pst");
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();
        const auto* image=bank?bank->texture(imageIndex):nullptr;
        if(!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=4)
            return fail(d,Error::texture,"authenticated original UIPODTEX17_3 image bank",0x24e880u);
        constexpr std::size_t offsets[]{1052,1324,2364,6476};
        for(std::size_t i=0;i<4;++i){
            const auto& mip=texture->mips[i];const auto side=16u<<i;
            if(mip.width!=side || mip.height!=side || mip.rgba.size()!=std::size_t(side)*side*4 ||
                mip.source_indices_offset!=offsets[i] || mip.source_header_offset!=offsets[i]-16)
                return fail(d,Error::texture,"original17_3 ascending16..128 mip chain",0x24e880u);
        }
        const auto& base=texture->mips.back();
        if(image->texture.width!=base.width || image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published17_3 image must equal original largest mip",0x24eb90u);
        OriginalUiRider17CutoutMaterial50KM next;
        if(!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,
            false,false,false,false,false,false,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=3;next.sourceMipCount=4;
        output=next;return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool prepareOriginalUiRiderWindowMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRiderWindowMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        const bool character=pass.identity&&pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\12c_4.psm");
        const auto* name=character?"12c_4":"pod_wndw";
        if ((!character&&pass.identity!=catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\pod_wndw.psm")) ||
            (pass.pass!=1 && pass.pass!=3) || !same(pass.microprogram,"DiffuseCompressed") ||
            !same(pass.textureName,name) || pass.gs[Field::alphaBlendingOnOff].value!=1 ||
            pass.gs[Field::inputColorA].value!=0 || pass.gs[Field::inputColorB].value!=1 ||
            pass.gs[Field::inputAlphaC].value!=0 || pass.gs[Field::inputColorD].value!=1 ||
            pass.gs[Field::alphaTestOnOff].value!=1 || pass.gs[Field::foggingOnOff].value!=1 ||
            pass.gs[Field::depthTestMethod].value!=2 || pass.gs[Field::zWriteDisabled].value!=0)
            return fail(d,Error::profile,"actual pod_wndw/12c_4 DiffuseCompressed pass1 or3",0x2476a0u);
        const auto* member=catalogMember(OriginalUiArchiveId50KG::rider,
            character?"ui\\textures\\12c_4.pst":"ui\\textures\\pod_wndw.pst");
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=(character?4u:3u))
            return fail(d,Error::texture,"authenticated original pod_wndw/12c_4 indexed image",0x24e880u);
        const auto& base=texture->mips.back();
        if (base.width!=(character?128u:64u) || base.height!=(character?128u:64u) || image->texture.width!=base.width ||
            image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published native window image must equal original largest mip",0x24eb90u);
        OriginalUiRiderWindowMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=character?3u:2u;next.sourceMipCount=character?4u:3u;next.sourcePass=pass.pass;
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiRider19ShadowMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19ShadowMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        if (!pass.identity)return fail(d,Error::profile,"canonical Rider shadow PSM identity",0x2476a0u);
        const auto* name=unlockedShadowName(pass);
        if (!name || pass.pass!=1 || !same(pass.microprogram,"DiffuseCompressed") ||
            !same(pass.textureName,name) ||
            pass.gs[Field::alphaBlendingOnOff].value!=1 || pass.gs[Field::inputColorA].value!=2 ||
            pass.gs[Field::inputColorB].value!=1 || pass.gs[Field::inputAlphaC].value!=0 ||
            pass.gs[Field::inputColorD].value!=1 || pass.gs[Field::alphaTestOnOff].value!=1 ||
            pass.gs[Field::alphaTestMethod].value!=6 || pass.gs[Field::alphaReferenceValue].value!=0 ||
            pass.gs[Field::processingMethodWhenFailedInAlphaTest].value!=0 ||
            pass.gs[Field::foggingOnOff].value!=1 || pass.gs[Field::depthTestMethod].value!=2 ||
            pass.gs[Field::zWriteDisabled].value!=0)
            return fail(d,Error::profile,"actual default-unlocked Rider shadow attenuation pass1",0x2476a0u);
        // Exact canonical PSM selects its corresponding original PST.
        const auto memberName=std::string("ui\\textures\\")+name+".pst";
        const auto* member=catalogMember(OriginalUiArchiveId50KG::rider,memberName.c_str());
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=1)
            return fail(d,Error::texture,"authenticated original default-unlocked Rider shadow image",0x24e880u);
        const auto& mip=texture->mips.front();
        if (mip.width!=128 || mip.height!=128 || image->texture.width!=mip.width ||
            image->texture.height!=mip.height || image->texture.rgba!=mip.rgba)
            return fail(d,Error::texture,"published source default-unlocked Rider shadow image");
        OriginalUiRider19ShadowMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiRider19SpecularMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19SpecularMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        if (!pass.identity)return fail(d,Error::profile,"canonical Rider specular PSM identity",0x2476a0u);
        // Original03c_3/08c_3 pass2 has the same authored FIX64/depth-writing
        // specular state as12c_4. Its single-mip diffuse passes stay generic.
        const bool character=pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\12c_4.psm") ||
            pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\03c_3.psm") ||
            pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\08c_3.psm");
        const auto* bodyName=unlockedBodyName(pass);
        const bool original19=bodyName&&(same(bodyName,"19_1")||same(bodyName,"19_2"));
        const bool window=pass.identity==catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\pod_wndw.psm");
        if ((!bodyName&&!window&&!character) || pass.pass!=2 || !same(pass.microprogram,"SpecularMapCompressed") ||
            !same(pass.textureName,"specmap1") ||
            ((bodyName||window)&&pass.gs[Field::inputAlphaC].value!=(window?2:1)) ||
            (window && pass.gs[Field::fixedAlphaValue].value!=255))
            return fail(d,Error::profile,"canonical unlocked Rider single-map specular pass2",0x2476a0u);
        if(character&&(pass.gs[Field::inputAlphaC].value!=2||pass.gs[Field::fixedAlphaValue].value!=64||
           pass.gs[Field::zWriteDisabled].value!=0||pass.gs[Field::foggingOnOff].value!=1||
           pass.gs[Field::alphaTestOnOff].value!=1||pass.gs[Field::depthTestMethod].value!=2))
            return fail(d,Error::profile,"actual03c_3/08c_3/12c_4 FIX64 depth-writing specular pass2",0x2476a0u);
        // Preserve the existing20 exact inherited fog/ATE/GEQUAL state;
        // added canonical bodies require all common source-backed validations; the common gate below retains ALPHA(0,2,1,1),
        // enabled blending and disabled depth writes. Existing19 admission is
        // unchanged, including the distinct pod_wndw FIX255 pass.
        if (bodyName&&!original19 && (pass.gs[Field::foggingOnOff].value!=1 ||
            pass.gs[Field::alphaTestOnOff].value!=1 || pass.gs[Field::alphaTestMethod].value!=6 ||
            pass.gs[Field::alphaReferenceValue].value!=0 ||
            pass.gs[Field::processingMethodWhenFailedInAlphaTest].value!=0 ||
            pass.gs[Field::depthTestMethod].value!=2))
            return fail(d,Error::profile,"actual unlocked body specular fog alpha-test depth state",0x2476a0u);
        const auto* member=catalogMember(OriginalUiArchiveId50KG::rider,"ui\\textures\\specmap1.pst");
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=1)
            return fail(d,Error::texture,"authenticated original Rider specmap1 image",0x24e880u);
        const auto& mip=texture->mips.front();
        if (mip.width!=256 || mip.height!=256 || image->texture.width!=mip.width ||
            image->texture.height!=mip.height || image->texture.rgba!=mip.rgba)
            return fail(d,Error::texture,"published source specmap1 image");
        OriginalUiRider19SpecularMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,true,false,false,character))return false;
        next.material.program=OriginalUiLitProgram50KM::specularMapCompressed;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}

// Tournament invokes the same native raster kernels from its own authentic
// PSM/PST owners. The catalog authenticates matching source bytes; every call
// below still verifies the actual Tournament identity and provenance. Rider
// entry points and their finite identity sets above remain unchanged.
namespace {
const char* tournamentUnlockedBodyName(const OriginalUiMaterialPass50KG& pass) {
    constexpr const char* names[]{"02_1","02_2","03_1","03_2","04_1","04_2","07_1","07_2","10_1","10_2","11_1","11_2",
        "12_1","12_2","15_1","15_2","19_1","19_2","20_1","20_2","09_1","09_2","23_1","23_2","24_1","24_2",
        "01_1","01_2","08_1","08_2","14_1","14_2","16_1","16_2","17_1","17_2","18_1","18_2","21_1","21_2","22_1","22_2","25_1","25_2","26_1","26_2"};
    if(!pass.identity)return nullptr;
    for(const auto* name:names){
        const auto member=std::string("ui\\textures\\")+name+".psm";
        if(pass.identity==catalogMember(OriginalUiArchiveId50KG::tournament,member.c_str()))return name;
    }
    return nullptr;
}
const char* tournamentUnlockedShadowName(const OriginalUiMaterialPass50KG& pass) {
    // Original26_shdw.psm is byte-identical to25_shdw.psm and names25_shdw.pst.
    // Preserve that authored cross-rider source rather than substituting26_shdw.pst.
    if(pass.identity&&pass.identity==catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\26_shdw.psm"))return "25_shdw";
    constexpr const char* names[]{"02_shdw","03_shdw","04_shdw","07_shdw","10_shdw","11_shdw","12_shdw","15_shdw","19_shdw","20_shdw","09_shdw","23_shdw","24_shdw",
        "01_shdw","08_shdw","14_shdw","16_shdw","17_shdw","18_shdw","21_shdw","22_shdw","25_shdw"};
    if(!pass.identity)return nullptr;
    for(const auto* name:names){
        const auto member=std::string("ui\\textures\\")+name+".psm";
        if(pass.identity==catalogMember(OriginalUiArchiveId50KG::tournament,member.c_str()))return name;
    }
    return nullptr;
}
}
bool prepareOriginalUiTournamentPodBodyMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19BodyMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        const auto* name=tournamentUnlockedBodyName(pass);
        const bool original19=name&&(same(name,"19_1")||same(name,"19_2"));
        if(!name||pass.pass!=1||!same(pass.microprogram,"DiffuseCompressed")||!same(pass.textureName,name)||
           (pass.gs[Field::alphaBlendingOnOff].value!=0||pass.gs[Field::alphaTestOnOff].value!=0))
            return fail(d,Error::profile,"canonical unlocked Rider indexed body first pass",0x2476a0u);
        const auto memberName=std::string("ui\\textures\\")+name+".pst";
        const auto* member=catalogMember(OriginalUiArchiveId50KG::podTextures,memberName.c_str());
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();
        const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.empty() || texture->mips.size()>5 || (original19&&texture->mips.size()!=5))
            return fail(d,Error::texture,"authenticated original UIPODTEX body image bank",0x24e880u);
        const auto& base=texture->mips.back();
        if ((original19&&(base.width!=256 || base.height!=256)) || image->texture.width!=base.width ||
            image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published native body image must equal original largest mip",0x24eb90u);
        OriginalUiRider19BodyMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=static_cast<std::uint32_t>(texture->mips.size()-1);
        next.sourceMipCount=static_cast<std::uint32_t>(texture->mips.size());
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiTournamentPodShadowMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19ShadowMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        if (!pass.identity)return fail(d,Error::profile,"canonical Rider shadow PSM identity",0x2476a0u);
        const auto* name=tournamentUnlockedShadowName(pass);
        if (!name || pass.pass!=1 || !same(pass.microprogram,"DiffuseCompressed") ||
            !same(pass.textureName,name) ||
            pass.gs[Field::alphaBlendingOnOff].value!=1 || pass.gs[Field::inputColorA].value!=2 ||
            pass.gs[Field::inputColorB].value!=1 || pass.gs[Field::inputAlphaC].value!=0 ||
            pass.gs[Field::inputColorD].value!=1 || pass.gs[Field::alphaTestOnOff].value!=1 ||
            pass.gs[Field::alphaTestMethod].value!=6 || pass.gs[Field::alphaReferenceValue].value!=0 ||
            pass.gs[Field::processingMethodWhenFailedInAlphaTest].value!=0 ||
            pass.gs[Field::foggingOnOff].value!=1 || pass.gs[Field::depthTestMethod].value!=2 ||
            pass.gs[Field::zWriteDisabled].value!=0)
            return fail(d,Error::profile,"actual default-unlocked Rider shadow attenuation pass1",0x2476a0u);
        // Exact canonical PSM selects its corresponding original PST.
        const auto memberName=std::string("ui\\textures\\")+name+".pst";
        const auto* member=catalogMember(OriginalUiArchiveId50KG::tournament,memberName.c_str());
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=1)
            return fail(d,Error::texture,"authenticated original default-unlocked Rider shadow image",0x24e880u);
        const auto& mip=texture->mips.front();
        if (mip.width!=128 || mip.height!=128 || image->texture.width!=mip.width ||
            image->texture.height!=mip.height || image->texture.rgba!=mip.rgba)
            return fail(d,Error::texture,"published source default-unlocked Rider shadow image");
        OriginalUiRider19ShadowMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiTournamentPodSpecularMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider19SpecularMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        if (!pass.identity)return fail(d,Error::profile,"canonical Rider specular PSM identity",0x2476a0u);
        // Original03c_3/08c_3 pass2 has the same authored FIX64/depth-writing
        // specular state as12c_4. Its single-mip diffuse passes stay generic.
        constexpr bool character=false; // Tournament User_Rotate has no character graph.
        const auto* bodyName=tournamentUnlockedBodyName(pass);
        const bool original19=bodyName&&(same(bodyName,"19_1")||same(bodyName,"19_2"));
        const bool window=pass.identity==catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\pod_wndw.psm");
        if ((!bodyName&&!window&&!character) || pass.pass!=2 || !same(pass.microprogram,"SpecularMapCompressed") ||
            !same(pass.textureName,"specmap1") ||
            ((bodyName||window)&&pass.gs[Field::inputAlphaC].value!=(window?2:1)) ||
            (window && pass.gs[Field::fixedAlphaValue].value!=255))
            return fail(d,Error::profile,"canonical unlocked Rider single-map specular pass2",0x2476a0u);
        if(character&&(pass.gs[Field::inputAlphaC].value!=2||pass.gs[Field::fixedAlphaValue].value!=64||
           pass.gs[Field::zWriteDisabled].value!=0||pass.gs[Field::foggingOnOff].value!=1||
           pass.gs[Field::alphaTestOnOff].value!=1||pass.gs[Field::depthTestMethod].value!=2))
            return fail(d,Error::profile,"actual03c_3/08c_3/12c_4 FIX64 depth-writing specular pass2",0x2476a0u);
        // Preserve the existing20 exact inherited fog/ATE/GEQUAL state;
        // added canonical bodies require all common source-backed validations; the common gate below retains ALPHA(0,2,1,1),
        // enabled blending and disabled depth writes. Existing19 admission is
        // unchanged, including the distinct pod_wndw FIX255 pass.
        if (bodyName&&!original19 && (pass.gs[Field::foggingOnOff].value!=1 ||
            pass.gs[Field::alphaTestOnOff].value!=1 || pass.gs[Field::alphaTestMethod].value!=6 ||
            pass.gs[Field::alphaReferenceValue].value!=0 ||
            pass.gs[Field::processingMethodWhenFailedInAlphaTest].value!=0 ||
            pass.gs[Field::depthTestMethod].value!=2))
            return fail(d,Error::profile,"actual unlocked body specular fog alpha-test depth state",0x2476a0u);
        const auto* member=catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\specmap1.pst");
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=1)
            return fail(d,Error::texture,"authenticated original Rider specmap1 image",0x24e880u);
        const auto& mip=texture->mips.front();
        if (mip.width!=256 || mip.height!=256 || image->texture.width!=mip.width ||
            image->texture.height!=mip.height || image->texture.rgba!=mip.rgba)
            return fail(d,Error::texture,"published source specmap1 image");
        OriginalUiRider19SpecularMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,true,false,false,character))return false;
        next.material.program=OriginalUiLitProgram50KM::specularMapCompressed;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiTournamentPodWindowMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRiderWindowMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        constexpr bool character=false; // Exact Tournament pod_wndw only.
        const auto* name=character?"12c_4":"pod_wndw";
        if ((!character&&pass.identity!=catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\pod_wndw.psm")) ||
            (pass.pass!=1 && pass.pass!=3) || !same(pass.microprogram,"DiffuseCompressed") ||
            !same(pass.textureName,name) || pass.gs[Field::alphaBlendingOnOff].value!=1 ||
            pass.gs[Field::inputColorA].value!=0 || pass.gs[Field::inputColorB].value!=1 ||
            pass.gs[Field::inputAlphaC].value!=0 || pass.gs[Field::inputColorD].value!=1 ||
            pass.gs[Field::alphaTestOnOff].value!=1 || pass.gs[Field::foggingOnOff].value!=1 ||
            pass.gs[Field::depthTestMethod].value!=2 || pass.gs[Field::zWriteDisabled].value!=0)
            return fail(d,Error::profile,"actual pod_wndw/12c_4 DiffuseCompressed pass1 or3",0x2476a0u);
        const auto* member=catalogMember(OriginalUiArchiveId50KG::tournament,
            character?"ui\\textures\\12c_4.pst":"ui\\textures\\pod_wndw.pst");
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();const auto* image=bank?bank->texture(imageIndex):nullptr;
        if (!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=(character?4u:3u))
            return fail(d,Error::texture,"authenticated original pod_wndw/12c_4 indexed image",0x24e880u);
        const auto& base=texture->mips.back();
        if (base.width!=(character?128u:64u) || base.height!=(character?128u:64u) || image->texture.width!=base.width ||
            image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published native window image must equal original largest mip",0x24eb90u);
        OriginalUiRiderWindowMaterial50KM next;
        if (!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=character?3u:2u;next.sourceMipCount=character?4u:3u;next.sourcePass=pass.pass;
        output=next;return true;
    } catch (...) {d.error=Error::allocation;return false;}
}
bool prepareOriginalUiTournamentPodCutoutMaterial50KM(const OriginalUiMaterialPass50KG& pass,
    const OriginalUiScreenAssets50KG& images,std::uint32_t imageIndex,
    OriginalUiRider17CutoutMaterial50KM& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};d.pass=pass.pass;
    try {
        const auto* psm=catalogMember(OriginalUiArchiveId50KG::tournament,"ui\\textures\\17_3.psm");
        if(!psm || pass.identity!=psm ||
            pass.provenance.archive!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament) ||
            pass.provenance.origin!=OriginalUiMaterialOrigin50KG::authoredPsm ||
            pass.pass!=1 || !same(pass.microprogram,"DiffuseCompressed") || !same(pass.textureName,"17_3") ||
            !same(pass.shaderPassGroup,"none") || pass.enabled!=1 || pass.textureCount!=1 || pass.textureCoordinateIndex!=0 ||
            pass.lodCalculationMethod!=0 || pass.maximumMipLevel!=0 || pass.magFilter!=1 || pass.minFilter!=5 ||
            pass.lodL!=0 || pass.lodK!=-75)
            return fail(d,Error::profile,"canonical Rider17_3 single cutout pass/sampler",0x2476a0u);
        // Every canonical GS value is the constructor value except REF=0,
        // inherited from authenticated DefaultMT. Runtime overrides fail closed.
        for(std::size_t i=0;i<originalUiGsFieldCount50KG;++i){
            const auto expected=i==static_cast<std::size_t>(Field::alphaReferenceValue)?0:specs[i].initial;
            if(pass.gs.fields[i].value!=expected)
                return fail(d,Error::unsupportedValue,specs[i].name,pass.gs.fields[i].sourcePC);
        }
        const auto* member=catalogMember(OriginalUiArchiveId50KG::podTextures,"ui\\textures\\17_3.pst");
        const auto* source=images.sourceProvenance(imageIndex);
        const auto* texture=images.decodedTexture(imageIndex);
        const auto* bank=images.textures();
        const auto* image=bank?bank->texture(imageIndex):nullptr;
        if(!images.loaded() || !member || !source || !texture || !image || images.ipumProvenance(imageIndex) ||
            source->memberIdentity!=member || source->celIdentity || source->pstOffset!=0 ||
            source->pstBytes!=member->bytes || texture->kind!=1 || texture->mips.size()!=4)
            return fail(d,Error::texture,"authenticated original UIPODTEX17_3 image bank",0x24e880u);
        constexpr std::size_t offsets[]{1052,1324,2364,6476};
        for(std::size_t i=0;i<4;++i){
            const auto& mip=texture->mips[i];const auto side=16u<<i;
            if(mip.width!=side || mip.height!=side || mip.rgba.size()!=std::size_t(side)*side*4 ||
                mip.source_indices_offset!=offsets[i] || mip.source_header_offset!=offsets[i]-16)
                return fail(d,Error::texture,"original17_3 ascending16..128 mip chain",0x24e880u);
        }
        const auto& base=texture->mips.back();
        if(image->texture.width!=base.width || image->texture.height!=base.height || image->texture.rgba!=base.rgba)
            return fail(d,Error::texture,"published17_3 image must equal original largest mip",0x24eb90u);
        OriginalUiRider17CutoutMaterial50KM next;
        if(!prepareOriginalUiOneMapMaterial50KM(pass,*texture,next.material.raster,d,true,
            next.material.fogEnabled,next.material.alphaTestGreaterZero,
            false,false,false,false,false,false,false,false,false,true))return false;
        next.material.program=OriginalUiLitProgram50KM::diffuseCompressed;
        next.imagePolicy=OriginalUiNativeIndexedImagePolicy50KM::highestResolutionBaseOnly;
        next.images=&images;next.imageIndex=imageIndex;next.imageIdentity=image->identity;
        next.sourceStoredMipIndex=3;next.sourceMipCount=4;
        output=next;return true;
    }catch(...){d.error=Error::allocation;return false;}
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

bool readOriginalUiTournamentTrophyFallbackMaterial50KG(
    const OriginalUiArchive50KG& tournament,OriginalUiTournamentTrophy50KG trophy,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        constexpr const char* names[]{"trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza"};
        constexpr std::uint32_t pcs[]{0x1EF920,0x1EF964,0x1EF9A8,0x1EF938,0x1EF97C,0x1EF9C0};
        const auto index=static_cast<std::size_t>(trophy);
        if(index>=6)return fail(d,Error::fallbackSource,"Tournament trophy",0x1EEC40);
        if(!tournament.loaded()||tournament.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
            return fail(d,Error::archive,"authenticated Tournament",pcs[index]);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenMT/runtime owner",0x2DF10C);
        const auto registered=std::string("ui/textures/")+names[index];
        const auto texture=tournament.find(registered+".pst");
        const auto page=tournament.find("UI/UITourn/standing.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            tournament.find(registered+".psm").identity)
            return fail(d,Error::fallbackSource,"original trophy PST and absent PSM",pcs[index]);
        OriginalUiMaterialProvenance50KG provenance;
        provenance.origin=OriginalUiMaterialOrigin50KG::tournamentTrophyMissingPsmFallback;
        provenance.archive=tournament.identity();provenance.fallbackTexture=texture.identity;
        provenance.fallbackPageSource=page.identity;provenance.sourceCallerPC=pcs[index];
        provenance.registeredName=registered;provenance.attemptedPsmMember=registered+".psm";
        provenance.creationPC=0x2475C0;
        auto next=screenTextureFallback(provenance,names[index],*inherited,runtime);
        output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiCourseTrophyFallbackMaterial50KG(
    const OriginalUiArchive50KG& course,OriginalUiTournamentTrophy50KG trophy,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};try{
        constexpr const char* names[]{"trphgldi","trphslvi","trphbrzi","trphglda","trphslva","trphbrza"};
        constexpr std::uint32_t pcs[]{0x1D999C,0x1D99B4,0x1D99CC,0x1D99E4,0x1D99FC,0x1D9A14};
        const auto i=static_cast<std::size_t>(trophy);
        if(i>=6)return fail(d,Error::fallbackSource,"Course trophy",0x1D8F10);
        if(!course.loaded()||course.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::course))
            return fail(d,Error::archive,"authenticated Course",pcs[i]);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenMT/runtime owner",0x2DF10C);
        const auto registered=std::string("ui/textures/")+names[i];
        const auto texture=course.find(registered+".pst"),page=course.find("UI/UICourse/track.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
           course.find(registered+".psm").identity)return fail(d,Error::fallbackSource,"original Course trophy PST and absent PSM",pcs[i]);
        OriginalUiMaterialProvenance50KG p;p.origin=OriginalUiMaterialOrigin50KG::courseTrophyMissingPsmFallback;
        p.archive=course.identity();p.fallbackTexture=texture.identity;p.fallbackPageSource=page.identity;
        p.sourceCallerPC=pcs[i];p.registeredName=registered;p.attemptedPsmMember=registered+".psm";p.creationPC=0x2475C0;
        auto next=screenTextureFallback(p,names[i],*inherited,runtime);output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalSpeedBoostFallbackMaterial50LA(
    const OriginalUiArchive50KG& overlays,OriginalSpeedBoostImage50LA image,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        const auto index=static_cast<std::size_t>(image);
        if(index>=10)return fail(d,Error::fallbackSource,"bounded Speed/Boost image",0x1895B0);
        const auto& source=speedBoostSources50LA[index];
        if(!overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Speed/Boost OVERLAYS",source.caller);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"actual Speed/Boost ScreenMT/runtime",index==9?0x2E20F0:0x2DF10C);
        const std::string base=std::string("ui\\textures\\")+source.name;
        const auto texture=overlays.find(base+".pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
           overlays.find(base+".psm").identity)
            return fail(d,Error::fallbackSource,"original Speed/Boost PST and absent PSM",source.caller);
        OriginalUiMaterialProvenance50KG p;
        p.origin=index==9?OriginalUiMaterialOrigin50KG::speedBoostTriangleMissingPsmFallback:
            OriginalUiMaterialOrigin50KG::speedBoostTextureMissingPsmFallback;
        p.archive=overlays.identity();p.fallbackTexture=texture.identity;p.sourceCallerPC=source.caller;
        p.registeredName=base;p.attemptedPsmMember=base+".psm";p.creationPC=0x2475C0;
        auto next=screenTextureFallback(p,source.name,*inherited,runtime);
        output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}

bool readOriginalDamageFallbackMaterial50LA(
    const OriginalUiArchive50KG& overlays,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        constexpr std::uint32_t caller=0x18C748;
        if(!overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Damage OVERLAYS",caller);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"actual Damage ScreenMT/runtime",0x2DF10C);
        const std::string base=std::string("ui\\textures\\dmgradar");
        const auto texture=overlays.find(base+".pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
           overlays.find(base+".psm").identity)
            return fail(d,Error::fallbackSource,"original Damage PST and absent PSM",caller);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::damageTextureMissingPsmFallback;
        p.archive=overlays.identity();p.fallbackTexture=texture.identity;p.sourceCallerPC=caller;
        p.registeredName=base;p.attemptedPsmMember=base+".psm";p.creationPC=0x2475C0;
        auto next=screenTextureFallback(p,"dmgradar",*inherited,runtime);
        output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}

bool readOriginalPauseBackdropFallbackMaterial50LA(
    const OriginalUiArchive50KG& overlays,std::uint8_t index,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(index>1)return fail(d,Error::fallbackSource,"Pause backdrop index",0x1B8810);
        const std::uint32_t caller=index?0x1B898C:0x1B8840;
        const char* name=index?"results2":"results1";
        if(!overlays.loaded()||overlays.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::overlays))
            return fail(d,Error::archive,"authenticated Pause backdrop OVERLAYS",caller);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"actual Pause backdrop ScreenMT/runtime",0x2DF10C);
        const std::string base=std::string("ui\\textures\\")+name;
        const auto texture=overlays.find(base+".pst");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||
           overlays.find(base+".psm").identity)
            return fail(d,Error::fallbackSource,"original Pause backdrop PST and absent PSM",caller);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::pauseBackdropMissingPsmFallback;
        p.archive=overlays.identity();p.fallbackTexture=texture.identity;p.sourceCallerPC=caller;
        p.registeredName=base;p.attemptedPsmMember=base+".psm";p.creationPC=0x2475C0;
        auto next=screenTextureFallback(p,name,*inherited,runtime);
        output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}

bool readOriginalUiTournamentNotchFallbackMaterial50KG(
    const OriginalUiArchive50KG& tournament,const OriginalUiMaterialTemplates50KG& templates,
    OriginalUiTemplate50KG owner,const OriginalUiMaterialRuntime50KG& runtime,
    OriginalUiMaterial50KG& output,OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!tournament.loaded()||tournament.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
            return fail(d,Error::archive,"authenticated Tournament",0x1F1424);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenMT/runtime owner",0x2DF10C);
        const auto texture=tournament.find("ui/textures/notch.pst");
        const auto page=tournament.find("UI/UITourn/standing.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            tournament.find("ui/textures/notch.psm").identity)
            return fail(d,Error::fallbackSource,"original notch PST and absent PSM",0x1F1424);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::tournamentNotchMissingPsmFallback;
        p.archive=tournament.identity();p.fallbackTexture=texture.identity;
        p.fallbackPageSource=page.identity;p.sourceCallerPC=0x1F1424;
        p.registeredName="ui/textures/notch";p.attemptedPsmMember="ui/textures/notch.psm";p.creationPC=0x2475C0;
        auto next=screenTextureFallback(p,"notch",*inherited,runtime);
        output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}
bool readOriginalUiTournamentPortraitFallbackMaterial50KG(
    const OriginalUiArchive50KG& tournament,std::uint8_t career,
    const OriginalUiMaterialTemplates50KG& templates,OriginalUiTemplate50KG owner,
    const OriginalUiMaterialRuntime50KG& runtime,OriginalUiMaterial50KG& output,
    OriginalUiMaterialDiagnostic50KG& d) noexcept {
    d={};
    try {
        if(!tournament.loaded()||tournament.identity()!=originalUiArchiveIdentity50KG(OriginalUiArchiveId50KG::tournament))
            return fail(d,Error::archive,"authenticated Tournament",0x1EF034);
        if(!riderPortraitIndex(career))return fail(d,Error::fallbackSource,"authored career portrait",0x1EF034);
        const auto* inherited=templates.get(owner);
        if(owner!=OriginalUiTemplate50KG::screenMt||!inherited||!runtime.overrideKnown)
            return fail(d,Error::templateOwner,"explicit ScreenMT/runtime owner",0x2DF10C);
        const auto name=riderPortraitName(career),registered=std::string("ui/textures/")+name;
        const auto texture=tournament.find(registered+".pst");
        const auto page=tournament.find("UI/UITourn/standing.dat");
        if(!texture.identity||!texture.data||texture.size!=texture.identity->bytes||!page.identity||
            tournament.find(registered+".psm").identity)
            return fail(d,Error::fallbackSource,"original portrait PST and absent PSM",0x1EF034);
        OriginalUiMaterialProvenance50KG p;
        p.origin=OriginalUiMaterialOrigin50KG::tournamentPortraitMissingPsmFallback;
        p.archive=tournament.identity();p.fallbackTexture=texture.identity;
        p.fallbackPageSource=page.identity;p.sourceCallerPC=0x1EF034;
        p.registeredName=registered;p.attemptedPsmMember=registered+".psm";p.creationPC=0x2475C0;
        auto next=screenTextureFallback(p,name.c_str(),*inherited,runtime);
        output=std::move(next);return true;
    }catch(...){d.error=Error::allocation;return false;}
}

}
