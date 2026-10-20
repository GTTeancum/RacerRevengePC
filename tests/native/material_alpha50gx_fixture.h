#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/assets/original_ui_material50kg.h"
#include <charconv>
#include <cstdint>
#include <string>

// Cold policy bridge for the exact authenticated TA material selections. This
// does not alter pixels, geometry, parsers, GPU texture ownership or cables.
// Source fields and hashes are retained in solo-alpha50gx/source-material-inspection.json.
namespace material_alpha50gx_fixture {
enum class Blend : std::uint8_t { opaque, sourceAlpha, additive };
struct Policy {
    Blend blend = Blend::opaque;
    bool depthWrite = true;
    bool sourceDepthWriteSpecified = false;
    bool depthEqual = false;
    bool discardZeroAlpha = false;
    bool sourceAlphaTest = false;
    std::uint8_t sourceAlphaReference = 0;
    bool sourceFogKnown = false, sourceFogEnabled = true;
    bool unlit = false;
    float alphaScale = 1.0f;
    // Actual prelit program copies authored RGB; distinct from emissive/unlit.
    bool sourcePrelit = false;
    // Exact SC lava pass 2 only: fixed GS alpha, independent of source alpha.
    bool fixedBlendAlpha = false;
    // Exact RC crystal only: 1 prelit source-alpha, 2 Environment destination-alpha.
    std::uint8_t rcCrystalPass = 0;
};
namespace detail {
struct Material {
    const char* name;
    const char* key;
    const char* program;
    Blend blend;
    bool sourceDepthWriteSpecified;
};
// Every key is already authenticated against the retained record by 50FA.
// All explicit zWriteDisabled values in this selected corpus are zero.
inline constexpr Material materials[] = {
    {"arrowdroid01", "5c3e3b274e39735e703c9b200c4ab3933ad17d96ceec41e045d7382b99a6fe80", "PreLitCompressed", Blend::opaque, false},
    {"arrowdroid03", "777bf3fb8d960848c40a05417f672bd211a859900a7c13deb84f02bf7e93ea10", "PreLitCompressed", Blend::opaque, false},
    {"arrowred", "b7ca19102a16afe6ac29384d9b42380070526ee5b80a901a9acf06b15d2f6960", "PrelitCompressed", Blend::sourceAlpha, true},
    {"bantha", "dc7e8ea64f8d6c62b2b309064102754c877a1c0d6e8e381dae260bad377dadc4", "diffuseCompressed", Blend::sourceAlpha, false},
    {"bone", "7651bc58ce6ac81b6038812273ec9afa5d5adf623f6f99096736a380844780c5", "PrelitCompressed", Blend::opaque, false},
    {"camdroid03", "fe9fe0f1ac699c54b41af888077ad51b15f51b14619411554a4844b3361aaf7a", "PreLitCompressed", Blend::opaque, false},
    {"cord", "f412f61c202d72b1f0ba4ef66dc13d1be0bbbbafc9d297c65b9df1f4e2252270", "PreLitCompressed", Blend::opaque, false},
    {"crawlera", "64ec02f6fe7a90e4b94fd0d1526235d438b7b1f93ebbb2bb6dccb10d2c1513d1", "PreLitCompressed", Blend::opaque, false},
    {"crowd", "2367f573cb30ca975777a7f4ac5473f2006d091c63e9e0b3346bbdbfec01d5b8", "PrelitCompressed", Blend::sourceAlpha, false},
    {"horn", "90b2420d835f9ef4a3a504ca921862c948108e1fcf661212701167d6eba8b608", "DiffuseCompressed", Blend::opaque, false},
    {"hutr01", "b97b9138898ec94056e8095678bbc12a2af1e36d23c57d2c27f9d133f0c023e6", "PreLitCompressed", Blend::opaque, false},
    {"jabba01", "2f2c8cc10c5d4afe73a873894bb0a5fe83d69c21db48b40ae1133245322e40a2", "PreLitCompressed", Blend::opaque, false},
    {"moseisley1", "ecc3b776319b318a5cfc44f2a5ee8e487855e46fca3fc75b628bf7dff895f397", "PreLitCompressed", Blend::opaque, false},
    {"moseisley2", "853d0b6abe279de7635abd0c11222da08a72c25d45cfde372e0b14fad9dad705", "PreLitCompressed", Blend::opaque, false},
    {"pole01", "850be68a21c10362eaac2ddee99a2f980fa69c6ffae2e6b8e14ff8de1dfa8b21", "PreLitCompressed", Blend::opaque, false},
    {"rclightstrip", "0e8d108a38f22235038543b86fb9d6d8c1c648d7ba6959cdadef6c026d04b47e", "DiffuseCompressed", Blend::additive, true},
    {"ststand1", "23a20b4ac8b593cf8b0ab73d50fafc390c534e3b071f64e2a5070a74394eb85e", "PreLitCompressed", Blend::opaque, false},
    {"ststand3", "2adece9f298ac7a3ef0da036d861ea252b1c4729d516f5c4e8a3e45b3b47f470", "PreLitCompressed", Blend::opaque, false},
    {"ta_blend4", "07ba209cb21123dc4a6e9a0a3205c616d692cd990c3d81292a9ecdd432f1221e", "PreLitCompressed", Blend::opaque, false},
    {"ta_newblnd", "b62bc6b3a2dfabd084c724e35c1a9f8f60e7f1912bd101a1bb32bcd5902f3486", "PreLitCompressed", Blend::opaque, false},
    {"ta_newrck", "089b5eaf88eba11f6d1367e52137a9ba5a8ce580d541da0ff6a105fea3e36075", "PreLitCompressed", Blend::opaque, false},
    {"ta_rock2", "269c14352b6505cf0a109bb6660de38ece192973767adb5ca3db9b39c91f75bf", "PreLitCompressed", Blend::opaque, false},
    {"ta_sand8", "2c47cd9b4d487f3941c679c7afb77f0a8aeac2152548cc1eade2ab4d81d7cb13", "PreLitCompressed", Blend::opaque, false},
    {"ta_sandground", "80bd392860b58f6b6898b56b659fd2dfa6b3c937761ceda1ca1fb4552b6ea9af", "PreLitCompressed", Blend::opaque, false},
    {"tarock", "f1d0ff12d25ed38034e1818683bcb8226c3202ac8da9fcbb4291bd9098dc4dce", "PreLitCompressed", Blend::opaque, false},
    {"track", "9d9a385d4abcf351346607125ba809c4f83ec3c1777bd36c9811f3123f7a8b60", "PreLitCompressed", Blend::opaque, false},
    {"trackdark", "11364865576d246fdcf4ef358d75ed721d65a26c88bbe6395ce7660a4eb7332d", "PreLitCompressed", Blend::opaque, false},
    {"trackside", "8db0658cc78fddb5126225805fd3c5df66f83f58bbb3e4920a124fad6d3208ad", "PreLitCompressed", Blend::opaque, false},
    {"trackwheel", "40d311d4ec4d3e784317ffa69deb200be19e8614fdb4ed64b27443b79d7d1581", "PreLitCompressed", Blend::opaque, false},
};
inline std::string lower(std::string value) {
    for (char& c : value) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return value;
}
} // namespace detail

// GC selections carry the exact authenticated original Material48E fields.
// ProjectedPrelit preserves source prelit color; no host geometric light is
// added. Unspecified blend fields keep an explicit opaque host policy pending
// the separately retained startup default-template identity.
inline bool selectGc(const track_materials50fa_fixture::Selection& selection, Policy& output) {
    if ((selection.track != track_source_fixture::Track::gc &&
         selection.track != track_source_fixture::Track::ga) || selection.slot != 1 ||
        !selection.pass || selection.pass > selection.passCount || selection.passCount > 2)
        return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto value = [&](const char* name, std::string& result) {
        bool found = false;
        for (const auto& field : fields.at("authored")) {
            if (!field.is_object() || !field.contains("name") || !field.contains("value")) return false;
            if (field.at("name") != name) continue;
            if (found || !field.at("value").is_string()) return false;
            result = field.at("value").get<std::string>(); found = true;
        }
        return true;
    };
    std::string blending, depthDisabled, alphaTest, a, b, c, d;
    if (!value("alphaBlendingOnOff", blending) || !value("zWriteDisabled", depthDisabled) ||
        !value("alphaTestOnOff", alphaTest) || !value("inputColorA", a) ||
        !value("inputColorB", b) || !value("inputAlphaC", c) || !value("inputAlphaD", d)) return false;
    if ((!blending.empty() && blending != "0" && blending != "1") ||
        (!depthDisabled.empty() && depthDisabled != "0" && depthDisabled != "1") ||
        (!alphaTest.empty() && alphaTest != "0")) return false;
    Policy next;
    next.depthWrite = depthDisabled != "1";
    next.sourceDepthWriteSpecified = !depthDisabled.empty();
    // Original GS GEQUAL reverses to LEQUAL in this conventional GL depth map.
    next.depthEqual = true;
    if (blending == "1") {
        if (a != "0" || b != "1" || c != "0" || d != "1" ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = Blend::sourceAlpha;
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    if (selection.projectedPrelit) {
        if (selection.program != "ProjectedPrelit" || selection.passCount != 2 ||
            blending != (selection.pass == 1 ? "0" : "1") || alphaTest != "0") return false;
        next.unlit = true;
    } else if (selection.passCount != 1 ||
        (selection.program != "PreLitCompressed" && selection.program != "PrelitCompressed" &&
         selection.program != "DiffuseCompressed")) return false;
    next.sourcePrelit = selection.projectedPrelit ||
        detail::lower(selection.program) == "prelitcompressed";
    output = next;
    return true;
}

// GA keeps its existing lighting/fog/blend policy, but must inherit the real
// DefaultMT alpha test. Its flag PSMs omit these fields; omission is not OFF.
inline bool selectGa(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    Policy next;
    if(selection.track!=track_source_fixture::Track::ga||!runtime.overrideKnown||
       runtime.override435670||!selectGc(selection,next))return false;
    const auto* inherited=templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if(!inherited||!inherited->identity||!inherited->identity->sha256||
       std::string(inherited->identity->sha256)!=
           "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7")return false;
    const Field fields[]{Field::alphaTestOnOff,Field::alphaTestMethod,
        Field::alphaReferenceValue,Field::processingMethodWhenFailedInAlphaTest};
    std::array<std::int32_t,4> values{};
    for(std::size_t i=0;i<values.size();++i){
        values[i]=inherited->gs[fields[i]].value;
        bool found=false;
        const auto name=detail::lower(rr::assets::originalUiGsFieldName50KG(fields[i]));
        for(const auto& item:selection.sourcePassFields.at("authored")){
            if(!item.contains("name")||!item.at("name").is_string())return false;
            if(detail::lower(item.at("name").get<std::string>())!=name)continue;
            if(found||!item.contains("value")||!item.at("value").is_string())return false;
            const auto text=item.at("value").get<std::string>();
            const auto parsed=std::from_chars(text.data(),text.data()+text.size(),values[i]);
            if(parsed.ec!=std::errc{}||parsed.ptr!=text.data()+text.size())return false;
            found=true;
        }
    }
    if(values[0]!=0&&values[0]!=1)return false;
    if(values[0]){
        if(values[1]!=6||values[2]<0||values[2]>255||values[3]!=0)return false;
        next.sourceAlphaTest=true;
        next.sourceAlphaReference=static_cast<std::uint8_t>(values[2]);
    }
    output=next;return true;
}

// Call only with a selection borrowed from a successfully loaded 50FA owner.
// Transactional: unsupported/mismatched selections leave output unchanged.
// Blank source blending fields retain the previous explicit opaque host policy;
// they are not claimed as recovered original defaults. Alpha scaling below is
// a declared GS-style host modulation policy, not original microprogram parity.
inline bool select(const track_materials50fa_fixture::Selection& selection, Policy& output) {
    if (selection.track == track_source_fixture::Track::gc ||
        selection.track == track_source_fixture::Track::ga) return selectGc(selection, output);
    if (selection.track != track_source_fixture::Track::ta) return false;
    if (selection.pass != 1 || selection.slot != 1) return false;
    for (const auto& material : detail::materials) {
        if (selection.materialKey != material.key) continue;
        if (detail::lower(selection.materialName) != material.name ||
            detail::lower(selection.textureName) != material.name ||
            selection.program != material.program) return false;
        Policy next;
        next.blend = material.blend;
        next.sourceDepthWriteSpecified = material.sourceDepthWriteSpecified;
        // The three authored beam placements have use_lighting=false. Keep
        // their explicit host source-color scale, but omit geometric lighting.
        // This is limited to the ray material, never applied to opaque track.
        next.unlit = selection.materialKey ==
            "0e8d108a38f22235038543b86fb9d6d8c1c648d7ba6959cdadef6c026d04b47e";
        // Preserve the existing crowd/arrow zero-alpha cutout. Their remaining
        // fragments now use the authored source-over blend equation.
        next.discardZeroAlpha = selection.textureIndex != track_materials50fa_fixture::unresolved &&
            (selection.materialKey == "2367f573cb30ca975777a7f4ac5473f2006d091c63e9e0b3346bbdbfec01d5b8" ||
             selection.materialKey == "b7ca19102a16afe6ac29384d9b42380070526ee5b80a901a9acf06b15d2f6960");
        if (next.blend != Blend::opaque) {
            if (selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
            // Texture alpha and the raw vertex alpha are uploaded unchanged.
            // Modulate in 128-based GS units, then convert the result into the
            // 128-based GS blend factor. For normalized GL inputs this is the
            // product below. Clamp the resulting factor in the draw shader.
            next.alphaScale = (255.0f / 128.0f) * (255.0f / 128.0f);
        }
        next.sourcePrelit = detail::lower(selection.program) == "prelitcompressed";
        output = next;
        return true;
    }
    return false;
}

// The MA/SA ordinary world route borrows the actual authenticated APPSTART
// DefaultMT owner. 2476A0 inherits its GS fields, then applies nonempty pass
// values; inputAlphaD is the compatibility alias before inputColorD. This is
// deliberately separate from the older TA/GC/GA explicit native policies.
inline bool selectMaSa(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if ((selection.track != track_source_fixture::Track::ma &&
         selection.track != track_source_fixture::Track::sa) || selection.slot != 1 ||
        selection.pass != 1 || selection.passCount != 1 || selection.projectedPrelit ||
        !runtime.overrideKnown || runtime.override435670) return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    const bool shield = selection.track == track_source_fixture::Track::ma &&
        detail::lower(selection.materialName) == "ma_shield" &&
        detail::lower(selection.textureName) == "ma_shield";
    if (program != "prelitcompressed" && program != "diffusecompressed" &&
        !(shield && program == "diffusetextranscompressed")) return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = program == "prelitcompressed";
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}

// Exact MB one-pass policy; existing MA/SA admissions are unchanged.
inline bool selectMb(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if ((selection.track != track_source_fixture::Track::mb) || selection.slot != 1 ||
        selection.pass != 1 || selection.passCount != 1 || selection.projectedPrelit ||
        !runtime.overrideKnown || runtime.override435670) return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    const bool shield = selection.track == track_source_fixture::Track::mb &&
        detail::lower(selection.materialName) == "ma_shield" &&
        detail::lower(selection.textureName) == "ma_shield";
    if (program != "prelitcompressed" && program != "diffusecompressed" &&
        !(shield && program == "diffusetextranscompressed")) return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = program == "prelitcompressed";
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}
// Exact TC one-pass policy. Both source textrans materials retain their own
// names and hashes; UV-controller admission is a separate typed owner.
inline bool selectTc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if ((selection.track != track_source_fixture::Track::tc) || selection.slot != 1 ||
        selection.pass != 1 || selection.passCount != 1 || selection.projectedPrelit ||
        selection.assetIdentity != 0x5443ull ||
        !runtime.overrideKnown || runtime.override435670) return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    const bool shield = detail::lower(selection.materialName) == "tc_shield" &&
        detail::lower(selection.textureName) == "tc_shield" &&
        selection.materialKey == "7b96457eb7f6a8d0e9e16834ec7fd0f1d658e641fb2e083be713122318febd89";
    const bool sun = detail::lower(selection.materialName) == "tcsun" &&
        detail::lower(selection.textureName) == "tcsun" &&
        selection.materialKey == "8245b15cce3052af8f2c45d9436dea795aea16803d006eb12e2e27ee9a663f6f";
    if ((detail::lower(selection.materialName) == "tc_shield" && !shield) ||
        (detail::lower(selection.materialName) == "tcsun" && !sun)) return false;
    const bool textrans = program == "diffusetextranscompressed";
    if (program != "prelitcompressed" && program != "diffusecompressed" &&
        !((shield || sun) && textrans)) return false;
    if ((shield || sun) && (!textrans || selection.staticAnimationFrame ||
        selection.clampU || selection.clampV ||
        selection.textureIndex == track_materials50fa_fixture::unresolved ||
        get(Field::inputColorA) != 0 || get(Field::inputColorB) != 2 ||
        get(Field::inputAlphaC) != 0 || get(Field::inputColorD) != 1 ||
        get(Field::alphaBlendingOnOff) != 1 || get(Field::zWriteDisabled) != 1 ||
        get(Field::foggingOnOff) != 1 || get(Field::alphaTestOnOff) != 1 ||
        get(Field::alphaReferenceValue) != 0 ||
        get(Field::wrapModeHorizontal) != 0 || get(Field::wrapModeVertical) != 0)) return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = program == "prelitcompressed";
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}
// RC owns its authenticated material selections. The exact crystal pair
// requires the separate explicit source reflection / destination feedback path.
inline bool selectRc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if ((selection.track != track_source_fixture::Track::rc) || selection.slot != 1 ||
        !selection.pass || selection.pass > selection.passCount || selection.passCount > 2 || selection.projectedPrelit ||
        selection.assetIdentity != 0x5243ull ||
        !runtime.overrideKnown || runtime.override435670) return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    const bool crystalName = detail::lower(selection.materialName) == "crystal01";
    const bool crystal = crystalName && selection.materialKey ==
        "ceaa6deb22d9183430934971e7022810eae0e0d867b9565f23de68e0d31fbd3b";
    if (crystalName && !crystal) return false;
    if (crystal) {
        if (selection.passCount != 2 || selection.staticAnimationFrame ||
            selection.clampU || selection.clampV ||
            selection.textureIndex == track_materials50fa_fixture::unresolved ||
            detail::lower(selection.textureName) != (selection.pass == 1 ? "crystal01" : "specmap5") ||
            program != (selection.pass == 1 ? "prelitcompressed" : "environmentmapcompressed")) return false;
        // All constructor/default and authored effective GS fields are exact.
        // Blanks inherit actual DefaultMT, whose only GS override is AREF0.
        const std::array<std::int32_t,rr::assets::originalUiGsFieldCount50KG> expected{{
            1,0,255,255,0, 0,2,selection.pass==1?0:1,1,128,
            0,0,0,0,0,0, 1,0,0,1,1,selection.pass==1?0:1,1,0,0,0,0,
            selection.pass==1?0:1,6,0,0,0,1,2,1}};
        if (gs != expected) return false;
    } else if (selection.passCount != 1 ||
        (program != "prelitcompressed" && program != "diffusecompressed")) return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = program == "prelitcompressed";
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (crystal) {
        // Both passes write source alpha independently and do not write depth.
        // Pass1 RGB uses As, pass2 uses actual current Ad. Ordinary GL additive
        // cannot represent this; renderer admission requires rcCrystalPass.
        next.blend = Blend::additive;
        next.alphaScale = 1.f;
        next.rcCrystalPass = static_cast<std::uint8_t>(selection.pass);
        next.unlit = selection.pass == 2;
    } else if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}
// SC borrows its own authenticated selections and the actual APPSTART defaults.
// Only the pinned lava material admits two active UV passes and fixed additive.
inline bool selectSc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if ((selection.track != track_source_fixture::Track::sc) || selection.slot != 1 ||
        !selection.pass || selection.pass > selection.passCount || selection.passCount > 2 || selection.projectedPrelit ||
        !runtime.overrideKnown || runtime.override435670) return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    const bool lava = detail::lower(selection.materialName) == "lavajold03" &&
        detail::lower(selection.textureName) == "lavajold03" &&
        selection.materialKey == "43cf6e8c8a2b26c5db88f99b2f65247a76d09f01875978d2c638e39af305ad33";
    if (lava) {
        if (selection.passCount != 2 || program != "diffusetextranscompressed" ||
            selection.staticAnimationFrame || selection.clampU || selection.clampV ||
            selection.textureIndex == track_materials50fa_fixture::unresolved ||
            get(Field::inputColorA) != 0 || get(Field::inputColorD) != 1 ||
            get(Field::zWriteDisabled) != 0 || get(Field::foggingOnOff) != 1 ||
            get(Field::wrapModeHorizontal) != 0 || get(Field::wrapModeVertical) != 0 ||
            get(Field::alphaBlendingOnOff) != (selection.pass == 1 ? 0 : 1) ||
            get(Field::inputColorB) != (selection.pass == 1 ? 1 : 2) ||
            get(Field::inputAlphaC) != (selection.pass == 1 ? 0 : 2) ||
            get(Field::fixedAlphaValue) != 128 ||
            get(Field::alphaTestOnOff) != (selection.pass == 1 ? 0 : 1) ||
            get(Field::alphaReferenceValue) != 0) return false;
    } else if (selection.passCount != 1 ||
        (program != "prelitcompressed" && program != "diffusecompressed")) return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = program == "prelitcompressed";
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (lava && selection.pass == 2) {
        // (Cs - 0) * FIX/128 + Cd, FIX=128. This uses the existing
        // native ONE,ONE RGB path without multiplying texture/vertex alpha.
        // Native interpolation/lighting/rasterization remains a display policy.
        next.blend = Blend::additive;
        next.fixedBlendAlpha = true;
        next.alphaScale = 1.f;
    } else if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}
// Exact TB source-keyed projected and ordinary material admission.
inline bool selectTb(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if (selection.track != track_source_fixture::Track::tb || selection.assetIdentity != 0x5442 || selection.slot != 1 ||
        !selection.pass || selection.pass>selection.passCount || !selection.passCount || selection.passCount>2 ||
        !runtime.overrideKnown || runtime.override435670) return false;
    // These material keys belong to the authenticated TB world source closure.
    struct Identity {const char* name;const char* key;const char* program;const char* map;std::uint32_t pass,count;};
    static constexpr Identity identities[]={
        {"tb_door","74fb81fd40ed71c112d7faa7df010dd22ad04f2b302e5d9100617ca6ccdd6fd4","DiffuseCompressed","tb_door",1,1},
        {"arinac01","ac7256616330de68a8cf378cd07a2fc32635b839d93836ee7b01bc2b389c26c1","PreLitCompressed","arinac01",1,1},
        {"jawa","9bc04c9bdaed9b924c67a3e01093176753343b85f6f61a3bcfcce166d714bd97","PrelitCompressed","jawa",1,1},
        {"forcefield_dome","cdb3417a9042ed1f12aa09b15aa1dcbf4160562ac3a7ee1573f79ef103e37e7a","DiffuseCompressed","ForceField_Dome",1,1},
        {"camdroid01","4de7402258107f2f71b1f2f6cf8b947f05db5092f86a55868f2a2a07d316fe14","PreLitCompressed","camdroid01",1,1},
        {"camdroid02","4c401d168a7058c323b9e9e7737ce9c84827efe9f07ed08ca921b7d4cd9338ee","PreLitCompressed","camdroid02",1,1},
        {"camdroid03","fe9fe0f1ac699c54b41af888077ad51b15f51b14619411554a4844b3361aaf7a","PreLitCompressed","camdroid03",1,1},
        {"cnsesns1","91866366aa80e1e36930cc06f25c6fe00402262f506030663df7de9f0140c73c","PreLitCompressed","cnsesns1",1,1},
        {"seating2","ae03ce13183e2af8fc99f5d10fb0e842af124bda86bcddef264b633df64ba3cf","PreLitCompressed","seating2",1,1},
        {"crowd","2367f573cb30ca975777a7f4ac5473f2006d091c63e9e0b3346bbdbfec01d5b8","PrelitCompressed","crowd",1,1},
        {"ststand1","2a06ddb173371eb284861ed8b5a1a5fcd42f61c98b0c3127575a26120abd8f4a","PreLitCompressed","ststand1",1,1},
        {"ststand3","bb60b333c5637bb428ff9f45532ff66546fabc1a429e68fc03b30ea919c63f96","PreLitCompressed","ststand3",1,1},
        {"fence02","2d319511a2a938c8f3fd0c0571817deaa847e6786cf1dbbfa8600831bf49c9a9","PreLitCompressed","fence02",1,1},
        {"fix01","3e97f0d4a48c2e0118b2fe686f1b4d7e0d40d5167fcb8f5c4abe9538bdf1511e","PreLitCompressed","fix01",1,1},
        {"float01","eeddf6afeff26753431731c265d12040f7c9244f91ae25a36a7f4068b9886ed0","PreLitCompressed","float01",1,1},
        {"float02","fe0210ec209595151bec1256c6b20419082eb0c612d460680882366271c96251","PreLitCompressed","float02",1,1},
        {"hutr01","0993442205405d76064aa7e4aa6f705dc9122b699d227b1ba897e88dfb7d1b4b","PreLitCompressed","hutr01",1,1},
        {"jabarb01","5f362f0de8d8e4eedacc08d83f753b9a52c206d4156c4ef876bc4d71b3adf009","PreLitCompressed","jabarb01",1,1},
        {"ahuts06","a5826f7697f75bbed0989c4ec0d97ec06ed8eb775870a97d5f090044bb40019c","PreLitCompressed","ahuts06",1,1},
        {"ahuts04","ed76e03d63c7a0c78f6e9b7279b74bf9829cc8aa3b894cc31398c4f283c76dfc","PreLitCompressed","ahuts04",1,1},
        {"ahuts05","3f561ba305fed092d2b94eab3e06e92c0ef71bd20cbd1af037687ddb57559e47","PreLitCompressed","ahuts05",1,1},
        {"mount02","4791061a5dd1bc1f4f6ca5b0a0269b20eecfa062b9956ce6e4dd73a862080190","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount02","4791061a5dd1bc1f4f6ca5b0a0269b20eecfa062b9956ce6e4dd73a862080190","ProjectedPrelit","ta_rock2",2,2},
        {"mount03","00031d022926d81ff2752d89de1fc29f7e2d0a39b49a7196e45cf76e3f6ed2b2","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount03","00031d022926d81ff2752d89de1fc29f7e2d0a39b49a7196e45cf76e3f6ed2b2","ProjectedPrelit","ta_rock2",2,2},
        {"mount04","a40ae16777f484170aba4a64456934bf5bcecf46bcf901f41167c6d86fe873a2","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount04","a40ae16777f484170aba4a64456934bf5bcecf46bcf901f41167c6d86fe873a2","ProjectedPrelit","ta_rock2",2,2},
        {"mount05","6466bbd1dd6fbcd0b572b6aac5a687625dd0ced68f980ddf1421b75b7cc29883","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount05","6466bbd1dd6fbcd0b572b6aac5a687625dd0ced68f980ddf1421b75b7cc29883","ProjectedPrelit","ta_rock2",2,2},
        {"mount06","30f895f95bbf06a7bfd127436046cbfd39e29e6529ec97454bc047e936bdcc0c","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount06","30f895f95bbf06a7bfd127436046cbfd39e29e6529ec97454bc047e936bdcc0c","ProjectedPrelit","ta_rock2",2,2},
        {"mount07","a3167eea19b0c181cad88e5397eff4e1a2e2062e654b21a05c3c1e260db81a02","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount07","a3167eea19b0c181cad88e5397eff4e1a2e2062e654b21a05c3c1e260db81a02","ProjectedPrelit","ta_rock2",2,2},
        {"mount08","2d5149491860f02c303a2ad8cee2abe77c11b0d1fbf2a48c2cb156045298983a","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount08","2d5149491860f02c303a2ad8cee2abe77c11b0d1fbf2a48c2cb156045298983a","ProjectedPrelit","ta_rock2",2,2},
        {"mount09","3cb2637f66fc9d848a05ec1260ff13e68c9617922ab7aacec432a763a9a8725d","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount09","3cb2637f66fc9d848a05ec1260ff13e68c9617922ab7aacec432a763a9a8725d","ProjectedPrelit","ta_rock2",2,2},
        {"mount10","79d37e39e626cf049ae7a0947b70cac1660eef1444a9f653204de9a95de18bfd","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount10","79d37e39e626cf049ae7a0947b70cac1660eef1444a9f653204de9a95de18bfd","ProjectedPrelit","ta_rock2",2,2},
        {"ta_rock2","269c14352b6505cf0a109bb6660de38ece192973767adb5ca3db9b39c91f75bf","PreLitCompressed","ta_rock2",1,1},
        {"mount12","355d6336946bf4569fd33638ff5549824706b6aa9b1ffe63f2949160c5e76fd0","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount12","355d6336946bf4569fd33638ff5549824706b6aa9b1ffe63f2949160c5e76fd0","ProjectedPrelit","ta_rock2",2,2},
        {"mount13","aa0ae896145070e3366790dd9575e46608de8f12a7ae48d679df6177f0bc59b5","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount13","aa0ae896145070e3366790dd9575e46608de8f12a7ae48d679df6177f0bc59b5","ProjectedPrelit","ta_rock2",2,2},
        {"mount14","982ad1cb98f1b0e0802e5adf104b266481e334f2dd888d0d024a77b1e0b36de4","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount14","982ad1cb98f1b0e0802e5adf104b266481e334f2dd888d0d024a77b1e0b36de4","ProjectedPrelit","ta_rock2",2,2},
        {"mount15","f5f9d1142ea8f629e222aa7e94e1aff6a989854fc0e46daff346fa3b1bbd4262","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount15","f5f9d1142ea8f629e222aa7e94e1aff6a989854fc0e46daff346fa3b1bbd4262","ProjectedPrelit","ta_rock2",2,2},
        {"mount16","7844b41207e8c732b76172d04283b8739774ceaf3f456c761f2def8d0f000de9","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount16","7844b41207e8c732b76172d04283b8739774ceaf3f456c761f2def8d0f000de9","ProjectedPrelit","ta_rock2",2,2},
        {"mount17","00c20ed11c28bd461b081b69eb038302436a403a64663146aa62b5f4d9a6ca67","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount17","00c20ed11c28bd461b081b69eb038302436a403a64663146aa62b5f4d9a6ca67","ProjectedPrelit","ta_rock2",2,2},
        {"mount18","55ab51434d742b9e8004e8e2bed7924e4a30b657d14f4d616c86174a667333b7","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount18","55ab51434d742b9e8004e8e2bed7924e4a30b657d14f4d616c86174a667333b7","ProjectedPrelit","ta_rock2",2,2},
        {"mount19","bdd554766bba64b88242fe3f2dc7cc67173aa342077cea0ada3fbe3fb3c4ebe5","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount19","bdd554766bba64b88242fe3f2dc7cc67173aa342077cea0ada3fbe3fb3c4ebe5","ProjectedPrelit","ta_rock2",2,2},
        {"mount20","20acf995e3008f98f79771d11f366c4a333d93a7010693684f53f026b837b435","ProjectedPrelit","ta_rockb_a",1,2},
        {"mount20","20acf995e3008f98f79771d11f366c4a333d93a7010693684f53f026b837b435","ProjectedPrelit","ta_rock2",2,2},
        {"object02","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rockb_a",1,2},
        {"object02","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rock2",2,2},
        {"object04","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rockb_a",1,2},
        {"object04","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rock2",2,2},
        {"pithan01","ce191f4652777afef3b45ea845fd2e347bb9b89269274f949c8b9e2daa153b4f","PreLitCompressed","pithan01",1,1},
        {"ahuts02","4d42de7b8c08095ecad2c5b6cbd81cdfefde314fda0adb0de92893e4900d2fd3","PreLitCompressed","ahuts02",1,1},
        {"ahuts01","fd10e21bd963c9e7c649234b59260ffc8d8b841afe9c6cc09aa9a1ff5ea374b7","PreLitCompressed","ahuts01",1,1},
        {"ahuts03","2c42f4855054089e0edba3eb9607ee84f3d9deac9b247dde06b0ddd3b2c521a9","PreLitCompressed","ahuts03",1,1},
        {"track","9d9a385d4abcf351346607125ba809c4f83ec3c1777bd36c9811f3123f7a8b60","PreLitCompressed","track",1,1},
        {"trackdark","11364865576d246fdcf4ef358d75ed721d65a26c88bbe6395ce7660a4eb7332d","PreLitCompressed","trackdark",1,1},
        {"trackwheel","40d311d4ec4d3e784317ffa69deb200be19e8614fdb4ed64b27443b79d7d1581","PreLitCompressed","trackwheel",1,1},
        {"trackside","8db0658cc78fddb5126225805fd3c5df66f83f58bbb3e4920a124fad6d3208ad","PreLitCompressed","trackside",1,1},
        {"crawlera","298b51b2ddd803bb20514a2cf3c520c4aec620007c2e2b675e41ca73b84c579b","PreLitCompressed","crawlera",1,1},
        {"seating1","fbc1a476b0d3b3f512752931c85480666ef90342dbf50b82f1acb281223541dc","PreLitCompressed","seating1",1,1},
        {"shade01","d0603f8c20f7b8f7b42da7b76ba126f9f116c5c693d8fcacd31237c082754503","PreLitCompressed","shade01",1,1},
        {"tc_shield","7b96457eb7f6a8d0e9e16834ec7fd0f1d658e641fb2e083be713122318febd89","diffuseTextransCompressed","tc_Shield",1,1},
        {"stop01","f40b968b36331c661d305f7bd1760cd7de373863a556f0322695c87bb2a97516","PreLitCompressed","stop01",1,1},
        {"ststand2","121b7287b39ee6f3bc4b9fcc0b947dba4e1c64c3a22169c91b3edd0d79dc86e2","PreLitCompressed","ststand2",1,1},
        {"top01","50d8f11f2c1ce22f8d590efd9ef75790b79f7133a56f25787652600a5b95c43e","PreLitCompressed","top01",1,1},
        {"tb_arenafloor02","994e0b6e4368b518e18469e6a0d6060704df0233e129e69c42d7349cdc297474","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor02","994e0b6e4368b518e18469e6a0d6060704df0233e129e69c42d7349cdc297474","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor03","ca4c91ba2bf3bde5435f3ae5d9a172ea065b0d14aa05000b0ecac20077be63d7","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor03","ca4c91ba2bf3bde5435f3ae5d9a172ea065b0d14aa05000b0ecac20077be63d7","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor04","8c4f9d373e199fbdaacf2c3db90124c6ee1cfb7202967f400f255b5d768c466a","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor04","8c4f9d373e199fbdaacf2c3db90124c6ee1cfb7202967f400f255b5d768c466a","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor05","ddeb6a3047c1a236ffe3dd7816f31c5d45fd367c0907c9efdfc60d7071376ffc","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor05","ddeb6a3047c1a236ffe3dd7816f31c5d45fd367c0907c9efdfc60d7071376ffc","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor06","fb8f0b02ea6c9ce332af626b8bd3711f7971a900f6238a8a8598ebe54bdb62d9","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor06","fb8f0b02ea6c9ce332af626b8bd3711f7971a900f6238a8a8598ebe54bdb62d9","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor07","45f930d8b6494c4643b3c5e19b2a0b92782e8a1ad3e506ae064de6660c24db3a","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor07","45f930d8b6494c4643b3c5e19b2a0b92782e8a1ad3e506ae064de6660c24db3a","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor08","3fff9d9dcb53446361a9530bef0c744f93c2bc399ecd9c2ddf6f536c116c54cd","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor08","3fff9d9dcb53446361a9530bef0c744f93c2bc399ecd9c2ddf6f536c116c54cd","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor09","6d4457229e0b2212ac62270aedb38dfa573ae9fc78e212f2634a308d7d2acd1d","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor09","6d4457229e0b2212ac62270aedb38dfa573ae9fc78e212f2634a308d7d2acd1d","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor10","43e87780baa52d6fe87bb243aff01add67bd938eea40ae68d904de14d1c458f7","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor10","43e87780baa52d6fe87bb243aff01add67bd938eea40ae68d904de14d1c458f7","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor11","6898557383b32b976c6f4bf731aaf4190eb77e603fc41e8841113e076bb7034c","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor11","6898557383b32b976c6f4bf731aaf4190eb77e603fc41e8841113e076bb7034c","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor12","90a470563fb300416f71220d8375e7e27a7c3d05ddff6e1f9d84c890242ba560","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor12","90a470563fb300416f71220d8375e7e27a7c3d05ddff6e1f9d84c890242ba560","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor13","ca466d029f8c34eff21a8e1af49b44a9f1f2db87194e7fe4cdb6b9ebc425952a","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor13","ca466d029f8c34eff21a8e1af49b44a9f1f2db87194e7fe4cdb6b9ebc425952a","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor14","6717a99491554ac6e3d32c5a9c6c248f7aadedae4c34860d4909bca280c27659","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor14","6717a99491554ac6e3d32c5a9c6c248f7aadedae4c34860d4909bca280c27659","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor15","42cf0747d4b7eb1dab1623dbb9d95a11ef595b7249ac63c0d02ef6db7305534d","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor15","42cf0747d4b7eb1dab1623dbb9d95a11ef595b7249ac63c0d02ef6db7305534d","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor16","e908228d8f814547123d302937f194375f4a937ab6fba2ae04368918b387aac8","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor16","e908228d8f814547123d302937f194375f4a937ab6fba2ae04368918b387aac8","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor17","a53b2cfa3a2f0e6db12e9597c75e52afc21e4e473d1f76babc110eb248f5441b","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor17","a53b2cfa3a2f0e6db12e9597c75e52afc21e4e473d1f76babc110eb248f5441b","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor18","f0d2a5e175827f11d8f9379523ffcf048008d86310dd551ad197f85dbf078012","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor18","f0d2a5e175827f11d8f9379523ffcf048008d86310dd551ad197f85dbf078012","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor19","0ac3e29f01d4eea0838d7710a2acd38e2bf7344a621afbbc5699d229f4ddd272","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor19","0ac3e29f01d4eea0838d7710a2acd38e2bf7344a621afbbc5699d229f4ddd272","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor20","af5f773f5fa15ac4cda4adc61dabfd4cbdae61e0a8d659706d8b236d9c1d73eb","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor20","af5f773f5fa15ac4cda4adc61dabfd4cbdae61e0a8d659706d8b236d9c1d73eb","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor30","613c51d328dff9223254821053ab32faacba870b5adfc2e5e6bd9e9f3ed785a6","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor30","613c51d328dff9223254821053ab32faacba870b5adfc2e5e6bd9e9f3ed785a6","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor_wall02a","059716ee87e0f12219c3c24ff09c908cc3cd498c397e2a48fb21288eaaae083a","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_arenafloor_wall02a","059716ee87e0f12219c3c24ff09c908cc3cd498c397e2a48fb21288eaaae083a","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor_wall02b","b608d1ff7bdffb8f09d7de14672c86467069fbcf7c0f6b8956c2eb00c7bd5886","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_arenafloor_wall02b","b608d1ff7bdffb8f09d7de14672c86467069fbcf7c0f6b8956c2eb00c7bd5886","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor_wall03a","66f789c5e87af2ba9f974b77cc3a80411df9593dfac417f1641ddfbc7172a0f2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_arenafloor_wall03a","66f789c5e87af2ba9f974b77cc3a80411df9593dfac417f1641ddfbc7172a0f2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor_wall04a","00700f5cacb7bb547d0d4ff182c57316a7018f13c9240073c31423370f48961c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_arenafloor_wall04a","00700f5cacb7bb547d0d4ff182c57316a7018f13c9240073c31423370f48961c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor_wall04d","b08e542854019ec0726fca19d7c2d7141dd91bf8626ffb77782e4e8fecf2919a","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_arenafloor_wall04d","b08e542854019ec0726fca19d7c2d7141dd91bf8626ffb77782e4e8fecf2919a","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor_wall04e","8ac42b6c32ea444995fe648db17046a69791383417d6a44f8ae2e8bae352a23f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_arenafloor_wall04e","8ac42b6c32ea444995fe648db17046a69791383417d6a44f8ae2e8bae352a23f","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb01a","1ac4c70b5fef1ebaf9fd057c86b0c03d34ee03abf9d64a952ea995beb7fd4d35","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb01a","1ac4c70b5fef1ebaf9fd057c86b0c03d34ee03abf9d64a952ea995beb7fd4d35","ProjectedPrelit","tb_sand2",2,2},
        {"tb_hillclimb01b","9cbcf759f3035fdbe59d3120a1e72f852b13772fd2797abe1b80659fd98308b5","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb01b","9cbcf759f3035fdbe59d3120a1e72f852b13772fd2797abe1b80659fd98308b5","ProjectedPrelit","ta_rock2",2,2},
        {"tb_sand2","d11f3925d903f4d48c2ffc868b42e72f9e22381b22ccdc0e419e63ef30f78804","PreLitCompressed","tb_sand2",1,1},
        {"tb_hillclimb1b","326bbdbc5e80def71eae4e50675dd919d568b3ac7d25877e4e864842a91d6786","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb1b","326bbdbc5e80def71eae4e50675dd919d568b3ac7d25877e4e864842a91d6786","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb1c","326bbdbc5e80def71eae4e50675dd919d568b3ac7d25877e4e864842a91d6786","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb1c","326bbdbc5e80def71eae4e50675dd919d568b3ac7d25877e4e864842a91d6786","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_01","023aab8d48acd82bdafa871845ee4c7871e1a46d5e57074e2047e5c4aaa6ef98","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_01","023aab8d48acd82bdafa871845ee4c7871e1a46d5e57074e2047e5c4aaa6ef98","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_01_a","7857ea747068ca28bddf3728560b9e994282459041a960adc8d454ce96025089","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_01_a","7857ea747068ca28bddf3728560b9e994282459041a960adc8d454ce96025089","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_a01","f6f2ec1da463eb8c5c39190d00bd9e753f9ffdfd57bf51ac7af9acc6ddca5f13","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_a01","f6f2ec1da463eb8c5c39190d00bd9e753f9ffdfd57bf51ac7af9acc6ddca5f13","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff02a","80b022ad830882d086532b76d34fd86423b3de4b2a3da594c5c0cce095d018af","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff02a","80b022ad830882d086532b76d34fd86423b3de4b2a3da594c5c0cce095d018af","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff04","2d477f04d3c882aa2aab5614fe77ecc5dc2897753966d7a3f0cbdeeb7b6283e3","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff04","2d477f04d3c882aa2aab5614fe77ecc5dc2897753966d7a3f0cbdeeb7b6283e3","ProjectedPrelit","ta_rock2",2,2},
        {"pole01","39d7362004e4df4ecb2bae19429c8f68d14722f22caf235962079ab3b21bc735","PreLitCompressed","pole01",1,1},
        {"arrowdroid03","777bf3fb8d960848c40a05417f672bd211a859900a7c13deb84f02bf7e93ea10","PreLitCompressed","arrowdroid03",1,1},
        {"arrowred","15cbf5ca143675889bbba37be9e9e249febf45cb04735cc1b89a2abcd0263164","PrelitCompressed","arrowred",1,1},
        {"arrowdroid01","5c3e3b274e39735e703c9b200c4ab3933ad17d96ceec41e045d7382b99a6fe80","PreLitCompressed","arrowdroid01",1,1},
        {"cord","f412f61c202d72b1f0ba4ef66dc13d1be0bbbbafc9d297c65b9df1f4e2252270","PreLitCompressed","cord",1,1},
        {"pole02","0453b00ed12961fa5e1b11b755d374ec64db08fae9d3d6908f4c3a6fe9c2fee5","DiffuseCompressed","pole02",1,1},
        {"view01","fd8404bd245cd72eaf616d807623cebc8f080962b2817f987ac223b6e72496d4","DiffuseCompressed","view01",1,1},
        {"tb_hillclimb_cliff01_a04","e52174300703488df5aef73637e3f79f7f46b1831095c1e673dde4da89e09549","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_a04","e52174300703488df5aef73637e3f79f7f46b1831095c1e673dde4da89e09549","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb02a","1be24ad16109f67d8f73c958a1adf7a28eb00088ed9b3ac4c9823f387f8d811b","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb02a","1be24ad16109f67d8f73c958a1adf7a28eb00088ed9b3ac4c9823f387f8d811b","ProjectedPrelit","tb_sand2",2,2},
        {"tb_hillclimb02b","68b63bb96dd95ad46df42a62971ec7837531f09dcb2220a00b222c80b99e1fe3","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb02b","68b63bb96dd95ad46df42a62971ec7837531f09dcb2220a00b222c80b99e1fe3","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb03a","53ee734ad52291d4085cbf8a961e5c85809007dcb8432b5299756e29ed65639f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb03a","53ee734ad52291d4085cbf8a961e5c85809007dcb8432b5299756e29ed65639f","ProjectedPrelit","tb_sand2",2,2},
        {"tb_hillclimb03b","7a8dd3fd025a7a9630bb824ce22e063048698a0c887ef20f32367bbbc03cd595","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb03b","7a8dd3fd025a7a9630bb824ce22e063048698a0c887ef20f32367bbbc03cd595","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_02","8d112a1ae319d337fa11c8b3e8e9c41d8f721876f146846142ad88b1a61b0ac0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_02","8d112a1ae319d337fa11c8b3e8e9c41d8f721876f146846142ad88b1a61b0ac0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_03","abf203e261b3348e9026f3154368080ead7adfd996db24243a98babcc0a7bb15","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_03","abf203e261b3348e9026f3154368080ead7adfd996db24243a98babcc0a7bb15","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_04","e8294ce6c68c2a49e938d5f45fcb9a95fe9d0a2bf5a1840a1dc37b82734ca46f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_04","e8294ce6c68c2a49e938d5f45fcb9a95fe9d0a2bf5a1840a1dc37b82734ca46f","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_a02","2e19ff9931737f4c7842f2a95862a9bddbbd82b520f82af4d160836681daab1f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_a02","2e19ff9931737f4c7842f2a95862a9bddbbd82b520f82af4d160836681daab1f","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff01_a03","e755a3ccffc3ae7ae291e5e6345ed3ceec43163d8bfdcad05207c8e71776bba1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff01_a03","e755a3ccffc3ae7ae291e5e6345ed3ceec43163d8bfdcad05207c8e71776bba1","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff02b","8b5c226236f1a7df2703fe28f7f2f993980a3338805b11df0c6a3d4e00c46108","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff02b","8b5c226236f1a7df2703fe28f7f2f993980a3338805b11df0c6a3d4e00c46108","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff02c","7c0ef89eb01eb8887b4d95200de2642a799ceba13cc8e1ec1582ff412bbdd9b9","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff02c","7c0ef89eb01eb8887b4d95200de2642a799ceba13cc8e1ec1582ff412bbdd9b9","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff02d","7087104f3242395ad245012f1096e5d351ea1ad1f163d484bfc7962a4a05d202","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff02d","7087104f3242395ad245012f1096e5d351ea1ad1f163d484bfc7962a4a05d202","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff02e","76b555f5995e1625befb014cbed2c13253259a04516d1c82e55d60922e99bf71","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff02e","76b555f5995e1625befb014cbed2c13253259a04516d1c82e55d60922e99bf71","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff02f","5f7674029018b1ad8c1a2a49b933054c82dc265262604197bf7211a18b4b8f84","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff02f","5f7674029018b1ad8c1a2a49b933054c82dc265262604197bf7211a18b4b8f84","ProjectedPrelit","ta_rock2",2,2},
        {"tb_split01a","16c8038bcf4faf5debbf2cfc4948018198c81ef6f2dacc8723b8179088c11b10","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_split01a","16c8038bcf4faf5debbf2cfc4948018198c81ef6f2dacc8723b8179088c11b10","ProjectedPrelit","tb_sand2",2,2},
        {"tb_split01b","5721086c3846e455728d42e57e46bee93b23bb168e54639754fc5489132ef3a2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_split01b","5721086c3846e455728d42e57e46bee93b23bb168e54639754fc5489132ef3a2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_split01c","95a91950ba2b34669afc5f2e62de56610eea70d853959e491c6b972c064815d4","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_split01c","95a91950ba2b34669afc5f2e62de56610eea70d853959e491c6b972c064815d4","ProjectedPrelit","ta_rock2",2,2},
        {"object08","eab5d7340542f94602d4b5283c66b0e862e36e5c0af1d06fadb4d4ef1f8adc3c","ProjectedPrelit","ta_rockb_a",1,2},
        {"object08","eab5d7340542f94602d4b5283c66b0e862e36e5c0af1d06fadb4d4ef1f8adc3c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_brancha01a","affa867cbddbe12fdc78987dcb25331c2c598cb88294e200ab89b5c9c71c8586","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha01a","affa867cbddbe12fdc78987dcb25331c2c598cb88294e200ab89b5c9c71c8586","ProjectedPrelit","tb_sand2",2,2},
        {"tb_brancha01b","7cf55eee6abd9bda6bd5f390415bfc292b5876cc31707a9d3ca21581c9fafcc9","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha01b","7cf55eee6abd9bda6bd5f390415bfc292b5876cc31707a9d3ca21581c9fafcc9","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump01a","c1c240fb0a4cf56009f85af648fd3f7478aac2b95a1dcf912fa2ad1ef4e77b90","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump01a","c1c240fb0a4cf56009f85af648fd3f7478aac2b95a1dcf912fa2ad1ef4e77b90","ProjectedPrelit","tb_sand2",2,2},
        {"tb_gorgejump01b","bcc375cc903b13a1ab03c192d7456bb4f48f68d9e52b0c961794c72566f39dc8","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump01b","bcc375cc903b13a1ab03c192d7456bb4f48f68d9e52b0c961794c72566f39dc8","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump01c","aec34740972ea6f514dd3d2e31fff8a7805c401e54fa2378ff744dec3b1043e1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump01c","aec34740972ea6f514dd3d2e31fff8a7805c401e54fa2378ff744dec3b1043e1","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump01d","e89a66682fb4f1bfcdaf6637cbdc7dfa38073c7abaa7c083b887977415953932","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump01d","e89a66682fb4f1bfcdaf6637cbdc7dfa38073c7abaa7c083b887977415953932","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump01e","33771f8810af2dcd0d0486277dcd6b33d3e75ea1422ced651caf6d161f744f31","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump01e","33771f8810af2dcd0d0486277dcd6b33d3e75ea1422ced651caf6d161f744f31","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump02a_floor","51cacdc1b08a69362d384953558d801774a3e058b18035df470c347a56720c18","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump02a_floor","51cacdc1b08a69362d384953558d801774a3e058b18035df470c347a56720c18","ProjectedPrelit","tb_sand2",2,2},
        {"tb_gorgejump02b","92a6942ca3b20def67cafd62472ef05e7d34f2e3b9d8ec504bd9245b2ef499a2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump02b","92a6942ca3b20def67cafd62472ef05e7d34f2e3b9d8ec504bd9245b2ef499a2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump02c","01cd128dd9c82926d2485dda368a42dfb9d6b0c40ff85b6494455e5a5a651827","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump02c","01cd128dd9c82926d2485dda368a42dfb9d6b0c40ff85b6494455e5a5a651827","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump02d","64809ee4d1e90095c811a6b681b09cf0b4b057301596a10f785f4f7c58e5c838","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump02d","64809ee4d1e90095c811a6b681b09cf0b4b057301596a10f785f4f7c58e5c838","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump02e","db442deaa255db643d3cb14fe6ebe80affcd76e4d8e5fbf07c8ceba860bc2b42","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump02e","db442deaa255db643d3cb14fe6ebe80affcd76e4d8e5fbf07c8ceba860bc2b42","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump02f","7505a57a38c3a07c73a96c81de003154372de9994031497825f28b66e91bb60b","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump02f","7505a57a38c3a07c73a96c81de003154372de9994031497825f28b66e91bb60b","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliff05","cc92c8425362ce9f77a0b678f32442054dfa24acedf6fb52be105115e911c0c5","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliff05","cc92c8425362ce9f77a0b678f32442054dfa24acedf6fb52be105115e911c0c5","ProjectedPrelit","ta_rock2",2,2},
        {"tb_hillclimb_cliffa05","0a181288ce47343e4ef3332d3971bc683e3f2f3e7423aa8b2810f13c1c3fdbb0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_hillclimb_cliffa05","0a181288ce47343e4ef3332d3971bc683e3f2f3e7423aa8b2810f13c1c3fdbb0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_split01b01","11766d9dbdf64cf51a0d4c17c49f6d3b430ddf8787a982da322bbffec0ffe72b","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_split01b01","11766d9dbdf64cf51a0d4c17c49f6d3b430ddf8787a982da322bbffec0ffe72b","ProjectedPrelit","ta_rock2",2,2},
        {"tb_split01d","3d2a3e8f40722a799fa180308a34cd76c95f84bb9e8071f1ae4e3695c8522161","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_split01d","3d2a3e8f40722a799fa180308a34cd76c95f84bb9e8071f1ae4e3695c8522161","ProjectedPrelit","tb_sand2",2,2},
        {"startl01","b123b9e8267c6b3d6cda3ad7f9d5ed3fe754eaf949a7cb993d472c270eed3d78","PreLitCompressed","startl01",1,1},
        {"jawa004","02c709ab7489280b4da6ab5ecaef54d779201c855920768c24019c6e2917cdfd","PreLitCompressed","jawa004",1,1},
        {"farmlg01","d21b242dfbf0dd05cdb2dd200a6b96c8c4fb0c5526fae1276404fe8ba724644c","PreLitCompressed","farmlg01",1,1},
        {"farmlg02","9695d47ccc58a2f488d3b747c6bcbafa319a57d53b063b65c1b28e4ef831ef0f","PreLitCompressed","farmlg02",1,1},
        {"farmlg03","3c44a212f97363b7a19e50919c4f1e010a7ab7179a2534733f62404aff182fde","PreLitCompressed","farmlg03",1,1},
        {"hanger01","352735436a48e6a52f81ef30a06bd25abb8021598eeb50cb34372b4a7bf149e3","PreLitCompressed","hanger01",1,1},
        {"hanger02","66754676feed3ce146ceddfd5442271b09bedb16bc8004766d72a1063fc794af","PreLitCompressed","hanger02",1,1},
        {"hanger03","121a72147a61dae7e5a41b8e6bd7e0b2327cb8e5857a5203030d1cca2a2abda4","PreLitCompressed","hanger03",1,1},
        {"hanger04","6788dfe88d1dbd8e7ef087b96a2975715bae9791f4d278102c4b883d140f2c26","PreLitCompressed","hanger04",1,1},
        {"hanger05","fe13ea5bc16577a461371d3de1f4b7bd34d1973716e019eccce04dd0e4cfd4ca","PreLitCompressed","hanger05",1,1},
        {"moisfr01","ab87cc22ac54a02d915cbb135ae99c1c46db794d863848665097fac64ee84833","PreLitCompressed","moisfr01",1,1},
        {"moisfr02","392145cf691fe6b2f4e0a5646964785cff837665230ee962f467ae8ebd9b1d5d","PreLitCompressed","moisfr02",1,1},
        {"moisfr03","1b42b9316786939c360586baea47e08203ad4da2424c97df9a0b45236b137580","PreLitCompressed","moisfr03",1,1},
        {"whouse01","8f37ca19aa80e036ee1b2e96f3f5bdbedb89443a2a92b524d1a02e0be98f60ec","PreLitCompressed","whouse01",1,1},
        {"whouse02","09128d4353f0adc40b8cb24f4ff04efd232a8193d622615e38c77556177155a3","PreLitCompressed","whouse02",1,1},
        {"whouse03","7eda9ae73c1a818e3346b0ce14cd5993019c3505fc629b4ef74b61f6ffe843a2","PreLitCompressed","whouse03",1,1},
        {"whouse04","125452a162b8de7776779499215fc59dc3728527e60a49059f742f9c754bffc7","PreLitCompressed","whouse04",1,1},
        {"whouse05","267cf11f6178a2274d67c53d3465e33b2e6363a31f170d383cc35d5de5ad8094","PreLitCompressed","whouse05",1,1},
        {"whouse06","2856fc39f6b4cb07a38806cd8f4db7c6b9ef30b8b6f664121d88d5e5990acf46","PreLitCompressed","whouse06",1,1},
        {"jawa003","576e0d5968517eb80cb833c5fba4b1b4c0038555207cd7ea4cf96d5878a4514b","PreLitCompressed","jawa003",1,1},
        {"jawa002","a22d81ec7a1b9684ab6b8c5dbc64fe5f16b26ba2712c692c2120f208b7f6b0af","PreLitCompressed","jawa002",1,1},
        {"jawa001","9599728780d0e2b2b622518908c6c7e422c6aaa3028ea740d7e40fd26378a939","PreLitCompressed","jawa001",1,1},
        {"jawa006","aaf230878726e37b4f554d96540cc9fa7d097f6c72d6326ca8b776728923a086","PreLitCompressed","jawa006",1,1},
        {"jawa007","ee3735aa0f9941588b4d15427c8958ca39763f746c69dd6fcb2b61fe547ae2ff","PreLitCompressed","jawa007",1,1},
        {"jawa008","59116596ed0a16abcf5ea0aab22a35e74a16fdba59f40a753a110e433be6ca49","PreLitCompressed","jawa008",1,1},
        {"jawa009","b7c6b6eeec90dd2ceea6d80635c561fd3e656a69483fe4671fc85e3da4f5eb37","PreLitCompressed","jawa009",1,1},
        {"jawa011","5639334acb9c90b3ea3bdd243fa1d97d149dc016ea3950a930ebe09cd23ab037","PreLitCompressed","jawa011",1,1},
        {"jawa012","a588393ca67271d7c81c014c3208be5af4b3db757510394a6cb14ea3dc7a8d3a","PreLitCompressed","jawa012",1,1},
        {"jawa005","4c77bad3c171701627944c3e9f0a9b8705e912f8df07e483baf0d4ee8094f2f8","PreLitCompressed","jawa005",1,1},
        {"jawa010","50b58a9bfc12fd44cfc540d6cae9fda1e08c578a4748de903fbe85b78e8d30e2","PreLitCompressed","jawa010",1,1},
        {"utinni","ae28f568ae7b4ebf5262a43e9c52122570e0440b23ddb5d23d29f2c534ac42d5","PreLitCompressed","utinni",1,1},
        {"junk01","59f612901ca80dbb7efebd8fd76e29c9029f1ad3d0746d28c678f66feabc88bd","PreLitCompressed","junk01",1,1},
        {"junk05","246bedaf4929860abdf86e7fdce2ffb3574f444cafe9383f0c4fe5fec2c86b6a","PreLitCompressed","junk05",1,1},
        {"junk06","e9fe40553ea87179d004d8b52e1cb8165ebaeb077d92443a7d748001f69777d0","PreLitCompressed","junk06",1,1},
        {"junk07","c00ada6052f84f59920946c3253831992623386ed0892c49c4cb865b44c3f68a","PreLitCompressed","junk07",1,1},
        {"junk02","5f247058a28e6a12202a1d44d6f2586685898962142217a6d330f27f9d4c585c","PreLitCompressed","junk02",1,1},
        {"junk03","e3cfea6447ee4d11a95ebdf9a3d4abb68fdfd368d5fd30da842da43019c12c9a","PreLitCompressed","junk03",1,1},
        {"junk04","210d33e63dc8c8d57324365f976d7fb8ede2488b9ce74c39677621f1d7e969be","PreLitCompressed","junk04",1,1},
        {"junk08","046c7fd0955d836d16ed5070bdfec13b457aa254c8e1b625f2e4554c6a6907e4","PreLitCompressed","junk08",1,1},
        {"junk12","7f29f4a82924231b572d26afb8b359093a89f386acd4d3d30a47ae3f16808b4e","PreLitCompressed","junk12",1,1},
        {"junk09","3822d9ba928c4b815dcf6cc270642142fcd29eaedcb9a9ff68005a3926f139e6","PreLitCompressed","junk09",1,1},
        {"junk10","53126d9d70b965e0db2f3b49090107a30301fb920654bd310678be798871833e","PreLitCompressed","junk10",1,1},
        {"junk11","e2321ca6e9e7b801fdea0b703326576b1fa1616d8646e275c8889df32c863365","PreLitCompressed","junk11",1,1},
        {"junk13","694729b96036ee75e36841b466260c159af9bce24dd21f553a345ce5daf7050f","PreLitCompressed","junk13",1,1},
        {"junk14","7e9dd472778eb74364f4ecc4fa920ace52741f9101139a605712d84361335a7e","PreLitCompressed","junk14",1,1},
        {"junk15","19643ec23f0f1f26bf8f5522636ba323e547d91aa0ec88e42ad1cffd75b2b5e4","PreLitCompressed","junk15",1,1},
        {"mvap","2a25eb859ddb9e542051949c2eeb5feeffb1155e42f9f3bcb7baf8b050063caa","PreLitCompressed","mvap",1,1},
        {"landspeeder03","739e308b9e531436441a8b662956af81991ce5debb025d6b809fa6092841239d","PreLitCompressed","landspeeder03",1,1},
        {"landspeeder04","9953c1a0ae4f7e7d6c9fd7484417ba979bb67486cef6f984045ee539f8ba203d","PreLitCompressed","landspeeder04",1,1},
        {"landcruser01","6efccd3a0c6fc2e170275557bd3839b49cb0afc262394a3b3309bf8caf7d1686","PreLitCompressed","landcruser01",1,1},
        {"tb_branchacap","192763164179aa4ce8a08a6bbef101aec10a38ac396a9f271e7a0ea4686e91f0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_branchacap","192763164179aa4ce8a08a6bbef101aec10a38ac396a9f271e7a0ea4686e91f0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcap01a","5d018b77fb98959d0814c414f833db0d624c3577f7a3c04b3ed0d0181c3ad7ed","ProjectedPrelit","tb_sand1",1,2},
        {"tb_farmcap01a","5d018b77fb98959d0814c414f833db0d624c3577f7a3c04b3ed0d0181c3ad7ed","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmcap01b","64eacd65709e73009d7049f407f47dca594185ac49bdbcccb3593b6d033bd54d","ProjectedPrelit","tb_sand1",1,2},
        {"tb_farmcap01b","64eacd65709e73009d7049f407f47dca594185ac49bdbcccb3593b6d033bd54d","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmcap01c","817c702b814ff2111c3f828dbfb7bd4d35783244e248d6efb715d7faa4ed35bc","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcap01c","817c702b814ff2111c3f828dbfb7bd4d35783244e248d6efb715d7faa4ed35bc","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcap01d","c34a85d997db53713e922efad65eb6497d57dfca04f281852a95a361f7887619","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcap01d","c34a85d997db53713e922efad65eb6497d57dfca04f281852a95a361f7887619","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcap01e","ea917d249c5691eace02c7161ed2b8d90c4c342aa3a9d9aff8f5ef4a5d1fde32","ProjectedPrelit","tb_sand1",1,2},
        {"tb_farmcap01e","ea917d249c5691eace02c7161ed2b8d90c4c342aa3a9d9aff8f5ef4a5d1fde32","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmcap02","64dcb9ad3ca902e89859e4146cb1d55a1231bf815fece8eee18fa96d1c3c0163","ProjectedPrelit","tb_sand1",1,2},
        {"tb_farmcap02","64dcb9ad3ca902e89859e4146cb1d55a1231bf815fece8eee18fa96d1c3c0163","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmcap03","1f6177feb5161e2bd8276c71dddcffe8c7fbdd5c67a94f57109af9effebece57","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcap03","1f6177feb5161e2bd8276c71dddcffe8c7fbdd5c67a94f57109af9effebece57","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmcliff01a","5e0f5ae55170ad12973dfca7b0578c7208922d3251a4ed820ec9bd0c19731ed6","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01a","5e0f5ae55170ad12973dfca7b0578c7208922d3251a4ed820ec9bd0c19731ed6","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01a1","0091b533cab8017f75d958dbcb835609240b3114987f359b196feff5f07b2e98","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01a1","0091b533cab8017f75d958dbcb835609240b3114987f359b196feff5f07b2e98","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01a2","d5a8908283ba78c2296306f90f68ef5d8a04f965e1f18517fcdb28a39f2e8205","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01a2","d5a8908283ba78c2296306f90f68ef5d8a04f965e1f18517fcdb28a39f2e8205","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01b1","4ed4dc71ff75a45632c92dfa4d98821cf199325378a293ea4d888436191b9e33","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01b1","4ed4dc71ff75a45632c92dfa4d98821cf199325378a293ea4d888436191b9e33","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01b2","a3adc2cb5c6e66cd0bb346256bd8395de0ecec98728b1d1b4c47e4fd927cb8b9","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01b2","a3adc2cb5c6e66cd0bb346256bd8395de0ecec98728b1d1b4c47e4fd927cb8b9","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01b3","2471168ece64784ca30ed140fdb82690210125af9e90a5859d1d217ffcba4c23","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01b3","2471168ece64784ca30ed140fdb82690210125af9e90a5859d1d217ffcba4c23","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01c1","1483776b6d6e12aaa118180d0033db85cfa21e815bc439e54c6f8bc71dbfd8a0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01c1","1483776b6d6e12aaa118180d0033db85cfa21e815bc439e54c6f8bc71dbfd8a0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff01c2","cfd92c7fcf51a808b1d7fcb0dc909effdf9d99d85da05ee1bac1c66869f10433","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff01c2","cfd92c7fcf51a808b1d7fcb0dc909effdf9d99d85da05ee1bac1c66869f10433","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02a1","c0fa41da27bd63cd7d26f8a7f9a07f1d333a994f2b64e9c1c9ba46c097c244dd","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02a1","c0fa41da27bd63cd7d26f8a7f9a07f1d333a994f2b64e9c1c9ba46c097c244dd","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02a2","0c4302b2ca7355947b7531c51d4f3ec07254a6edb8ead7597ede60ef748476da","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02a2","0c4302b2ca7355947b7531c51d4f3ec07254a6edb8ead7597ede60ef748476da","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02b1","70265f2ab59d683971408520d55fe34e1296fa8a2c97ebc14bb9ba164e089cb7","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02b1","70265f2ab59d683971408520d55fe34e1296fa8a2c97ebc14bb9ba164e089cb7","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02b2","f05b1f5c89b8f63b9760ee133a14c835a030c7995a980702004e46be7d26cfd6","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02b2","f05b1f5c89b8f63b9760ee133a14c835a030c7995a980702004e46be7d26cfd6","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02b3","5f5812de628172ae56be0f4627eecb0c5e418f26272bc05fd12e36815e261fbc","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02b3","5f5812de628172ae56be0f4627eecb0c5e418f26272bc05fd12e36815e261fbc","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02c","be0b327d39dd3c4777d2694d63edff9888e366206acbbcd56a6afe2666d66127","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02c","be0b327d39dd3c4777d2694d63edff9888e366206acbbcd56a6afe2666d66127","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02d1","5c7ff50cc22351ecb74883e73bb22609c2fdc55ce30a51ce556b756c0d20b337","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02d1","5c7ff50cc22351ecb74883e73bb22609c2fdc55ce30a51ce556b756c0d20b337","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmcliff02d2","3daffbc72b0f79960fcf618ae2ec43794aae9ecc2400552e5e0f026535b75a6d","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmcliff02d2","3daffbc72b0f79960fcf618ae2ec43794aae9ecc2400552e5e0f026535b75a6d","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01a","fa1e9f2adb350563ed7a7cf89ecc65fbdfba4e41af60adaba02732158902c954","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01a","fa1e9f2adb350563ed7a7cf89ecc65fbdfba4e41af60adaba02732158902c954","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01b","6728bb0968dbd29ed40a5401c8c0d9959e492510d0e40494a192eb6465a58ce6","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01b","6728bb0968dbd29ed40a5401c8c0d9959e492510d0e40494a192eb6465a58ce6","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01c","cd2ab96a4095aeaa279483aa7d48a2c56355a23b685bd059f261ab569d1addb1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01c","cd2ab96a4095aeaa279483aa7d48a2c56355a23b685bd059f261ab569d1addb1","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01d","cde67665f42f089fea65ea1e75191bbef7935cc1d5b4187ebcd5f690b92a6e3e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01d","cde67665f42f089fea65ea1e75191bbef7935cc1d5b4187ebcd5f690b92a6e3e","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01e","728819b11a60ad25bd63913550e977e49eeee6218910fe69693f080bc8c1e76e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01e","728819b11a60ad25bd63913550e977e49eeee6218910fe69693f080bc8c1e76e","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01f","d8cebd9dab9cab1fd6ebb7a7b130285899640ff0c6329a0c1d239ba0f01ad18c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01f","d8cebd9dab9cab1fd6ebb7a7b130285899640ff0c6329a0c1d239ba0f01ad18c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01g","fb6c256c26ed1a6532d95c65a8cd712d9f308edf88b704c70c001a5ae22fa86e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01g","fb6c256c26ed1a6532d95c65a8cd712d9f308edf88b704c70c001a5ae22fa86e","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01h","b620f500bbe381173633436729c76636c6688b32954854737c02f4115f4a03bb","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01h","b620f500bbe381173633436729c76636c6688b32954854737c02f4115f4a03bb","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmclifftop01i","7cc14af11bfe83e71930a9f32f5f10a851300985421e56213e5056b65450230a","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmclifftop01i","7cc14af11bfe83e71930a9f32f5f10a851300985421e56213e5056b65450230a","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01a","93a8275d7fc505b03c280416e7557f1d86d78f78b1f49eae889ddfe03859fd2b","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01a","93a8275d7fc505b03c280416e7557f1d86d78f78b1f49eae889ddfe03859fd2b","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01b","f63c65a46b4240e3a8abc396fe102c2c0c719200023fb94b5ab338dc6074059c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01b","f63c65a46b4240e3a8abc396fe102c2c0c719200023fb94b5ab338dc6074059c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01d","1b81d75fe118e0f7d90f86449570c67a81ba68e170149110cdf11dad446b31f1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01d","1b81d75fe118e0f7d90f86449570c67a81ba68e170149110cdf11dad446b31f1","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01e","3df39a4dcb24d1d282ae56f78f17c4863b9472f6f57354dd40ceea937f5c9b63","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01e","3df39a4dcb24d1d282ae56f78f17c4863b9472f6f57354dd40ceea937f5c9b63","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01f","ea3ef296c4f7bbc62b478d9e2c029e8f5f38fda6332da030f70aba41b8615856","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01f","ea3ef296c4f7bbc62b478d9e2c029e8f5f38fda6332da030f70aba41b8615856","ProjectedPrelit","tb_sand2",2,2},
        {"ta_rockb_a","bd2e55c3bfebf25d4d3219876f25268d661e6c580a9c8e017c58efe3553a499f","PreLitCompressed","ta_rockb_a",1,1},
        {"tb_farmpaths01g01","bfd467d8dc2e8eb86015628e0f7a919c9a6129c51c88da243fb0334aabef1140","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01g01","bfd467d8dc2e8eb86015628e0f7a919c9a6129c51c88da243fb0334aabef1140","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01h","f633e5a7b751b241e11ffbfe2c3bb9710943b100a4beaa6268b9427fa263e6d2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01h","f633e5a7b751b241e11ffbfe2c3bb9710943b100a4beaa6268b9427fa263e6d2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01h01","0afe846638cce669f01905a37b96410e06d703430e1e9de957db7d817fd25715","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01h01","0afe846638cce669f01905a37b96410e06d703430e1e9de957db7d817fd25715","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01i","934455a72130c9e3d913f1d32bd2171f3dbfb9947dc1ef930b520cf5f594cc1f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01i","934455a72130c9e3d913f1d32bd2171f3dbfb9947dc1ef930b520cf5f594cc1f","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01j","24acae118e0064aba9705bbe2358bf5cca967a1654b8e711289a8a39e0dca468","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01j","24acae118e0064aba9705bbe2358bf5cca967a1654b8e711289a8a39e0dca468","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01k","bbb57bdfd153810c6b8ea6f0e0c562d1ed2b5c07d48f29ac180030bb25fa48bb","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01k","bbb57bdfd153810c6b8ea6f0e0c562d1ed2b5c07d48f29ac180030bb25fa48bb","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01l","02e9a09cad9dd9c0051460d7981b0fc02fac6c224d18cd63d717799d9d5b2583","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01l","02e9a09cad9dd9c0051460d7981b0fc02fac6c224d18cd63d717799d9d5b2583","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01m","33d8fdc030c90e544f1caca1b7d32b22c6dd0fd7125c12c37d210dee37da5798","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01m","33d8fdc030c90e544f1caca1b7d32b22c6dd0fd7125c12c37d210dee37da5798","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01m01","0e3f0bdf112f7368b5f0176046d1365fd3689b37bd2eff72398c2725777791dc","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01m01","0e3f0bdf112f7368b5f0176046d1365fd3689b37bd2eff72398c2725777791dc","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01n","ab0d19746f5fb2046364fcfe7a67aea36eef1061a7026b4b3e671be13df770f4","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01n","ab0d19746f5fb2046364fcfe7a67aea36eef1061a7026b4b3e671be13df770f4","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01o","38668b8e8a6b3bd687744222ce152108eaee1a5223c697713a3db1545b7ac3f1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01o","38668b8e8a6b3bd687744222ce152108eaee1a5223c697713a3db1545b7ac3f1","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01p","c149fee941c7159b748810757fbf2544b772f48f8ecaab8c77c258be88b2fb41","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01p","c149fee941c7159b748810757fbf2544b772f48f8ecaab8c77c258be88b2fb41","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01q","1b65d72b1105eb3acff4149eee8a59f1a5000adc566b91b9dff12d611471748f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01q","1b65d72b1105eb3acff4149eee8a59f1a5000adc566b91b9dff12d611471748f","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01r","493e85e22ef0b95db5751bc4d70b5d7c25158dd8b15c1bdbd86a6e88bee8580e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01r","493e85e22ef0b95db5751bc4d70b5d7c25158dd8b15c1bdbd86a6e88bee8580e","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01s","a0587b2622b2eb9f5525d48cf26cd0e4849f6085436f432df933790d42762fe5","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01s","a0587b2622b2eb9f5525d48cf26cd0e4849f6085436f432df933790d42762fe5","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01s01","ee3e0b2d57772006d24032b0b8445b4f5de30a5ecd1ad2ce8ed7ce77374c2ca8","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01s01","ee3e0b2d57772006d24032b0b8445b4f5de30a5ecd1ad2ce8ed7ce77374c2ca8","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01t","f57120806fd412d7f084f2de6969cb524675c1b81129712eb136b54647557a9e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01t","f57120806fd412d7f084f2de6969cb524675c1b81129712eb136b54647557a9e","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01u","b61bedb2c16bd3b595642cfa8ff4d74367747fde22cee10f8b030006ad7c476c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01u","b61bedb2c16bd3b595642cfa8ff4d74367747fde22cee10f8b030006ad7c476c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01v","3f4d5ff7043f3350ad8cb92885ef53b849f306387cc78c3045160ca800ae32c8","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01v","3f4d5ff7043f3350ad8cb92885ef53b849f306387cc78c3045160ca800ae32c8","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01w","3ad28af6b4035d5586e28399a989e30fbdbf281374e407e5d1333de06edba491","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01w","3ad28af6b4035d5586e28399a989e30fbdbf281374e407e5d1333de06edba491","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths01z","8c850f2bdd23df19947ba52f780566a011fa2f728e97a7cac32684553a6d0e7c","ProjectedPrelit","tb_sand1",1,2},
        {"tb_farmpaths01z","8c850f2bdd23df19947ba52f780566a011fa2f728e97a7cac32684553a6d0e7c","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths01z01","1f8043459e50721493e4764df90d8a462676d800417528112af2c79bf5bb0b9c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths01z01","1f8043459e50721493e4764df90d8a462676d800417528112af2c79bf5bb0b9c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend01a","a5fc4f32dcd842ca1e81040da8c7f5b8c9fb23d44c0ad3e5236297d31a5068df","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01a","a5fc4f32dcd842ca1e81040da8c7f5b8c9fb23d44c0ad3e5236297d31a5068df","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend01b","5a9158399253351378b1dc320c340732da683d28787cf91b7c8224f71f06bdc0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01b","5a9158399253351378b1dc320c340732da683d28787cf91b7c8224f71f06bdc0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend01c","c9fdad7ec34225f3e25ea81fa64382e2b0ff80e7943e6141ba50e514ebc617db","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01c","c9fdad7ec34225f3e25ea81fa64382e2b0ff80e7943e6141ba50e514ebc617db","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend01d","c4217373495f1a38dc1efe82c53b2fcec241351c388939e99060b67da970e515","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01d","c4217373495f1a38dc1efe82c53b2fcec241351c388939e99060b67da970e515","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend01d_a","ab10d6510654abb776068687d1d0c4e2cc86012ce593d94e3a7dfe5c561dc827","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01d_a","ab10d6510654abb776068687d1d0c4e2cc86012ce593d94e3a7dfe5c561dc827","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend01e","499d50886889efc8278d731df9609a074a7e6bdd965a48cf1bde851db9c23fc7","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01e","499d50886889efc8278d731df9609a074a7e6bdd965a48cf1bde851db9c23fc7","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend01e_a","9f88b500403fabf329cd73015483de4383664743faadc5859cfd818f6fc0d695","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend01e_a","9f88b500403fabf329cd73015483de4383664743faadc5859cfd818f6fc0d695","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02a","bbf9724e039306cf125ce1d153df9169b2cf69cf4e4a891778e0dcde7e6d8359","ProjectedPrelit","tb_sand1",1,2},
        {"tb_bend02a","bbf9724e039306cf125ce1d153df9169b2cf69cf4e4a891778e0dcde7e6d8359","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend02b","a4af93bb401a8601b8fecb1c04f9e16bb024936a1740287f6f156e0896009e57","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02b","a4af93bb401a8601b8fecb1c04f9e16bb024936a1740287f6f156e0896009e57","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02c","c3823ca2fd7fb378fa57ad0bb07fc10a680d7b5a8f1a595d058edccd9eb796a5","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02c","c3823ca2fd7fb378fa57ad0bb07fc10a680d7b5a8f1a595d058edccd9eb796a5","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02d","90977d4eeb1608b9c984c73eedc5b5ec23ad3bcb058f9f981611c130acce07dd","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02d","90977d4eeb1608b9c984c73eedc5b5ec23ad3bcb058f9f981611c130acce07dd","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02d_a","50144181528c9bd7884c4416d7e1eb8130c4c7cf55c6efd3977faefa8b637096","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02d_a","50144181528c9bd7884c4416d7e1eb8130c4c7cf55c6efd3977faefa8b637096","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02d_b","30b2b28b42d503f202ed13a300b179936280fc60016c6534d2bf0621b8d38d07","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02d_b","30b2b28b42d503f202ed13a300b179936280fc60016c6534d2bf0621b8d38d07","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02e","29084c271063b2830bcf95eb3fa9dad051a368631b5ede4ef5c37501ed95a0a0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02e","29084c271063b2830bcf95eb3fa9dad051a368631b5ede4ef5c37501ed95a0a0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend02f","222782f99b2395c2991d223f589ab07aba5cb913e8d29c55f93bb0e353819597","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend02f","222782f99b2395c2991d223f589ab07aba5cb913e8d29c55f93bb0e353819597","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend03a","0743ea7d1502ab4600f836ea3358d54e547b9387e84aef0b48e7c19c70be0055","ProjectedPrelit","tb_sand1",1,2},
        {"tb_bend03a","0743ea7d1502ab4600f836ea3358d54e547b9387e84aef0b48e7c19c70be0055","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend03b","a6f608e9ca6fdcc6ecf7d34d3f51ecc417024dd30428a1f3ed8cc19f39b13a6c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend03b","a6f608e9ca6fdcc6ecf7d34d3f51ecc417024dd30428a1f3ed8cc19f39b13a6c","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend03c","ba678600ea1dda0cfbb61d7314de35d2c6c0392db6a5d82810350ad595f6e169","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend03c","ba678600ea1dda0cfbb61d7314de35d2c6c0392db6a5d82810350ad595f6e169","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend03d","d6ca1f05bd87bf1f11539c02631312081d5a6c734f0cf20f562362238470719f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend03d","d6ca1f05bd87bf1f11539c02631312081d5a6c734f0cf20f562362238470719f","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend03e","657a5053972e0be81c503bf11e6d75512f89f87126236e49c815d44954458b4d","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend03e","657a5053972e0be81c503bf11e6d75512f89f87126236e49c815d44954458b4d","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend04a","e4d6b7b9d68b9f117bb428815781b93b4def81961b5add5fee6311b800478ae3","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend04a","e4d6b7b9d68b9f117bb428815781b93b4def81961b5add5fee6311b800478ae3","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05a","deb3b94f439de35d3284fd528d55a8433b7165022987c0f26cfd228870272b3d","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05a","deb3b94f439de35d3284fd528d55a8433b7165022987c0f26cfd228870272b3d","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend05b","fdedb5836c3650cb1a016c5afe750dc8fd4ffd04bb401c4c9b0b7f58074238c3","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05b","fdedb5836c3650cb1a016c5afe750dc8fd4ffd04bb401c4c9b0b7f58074238c3","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05b_a","5396c92cd86ad68182558192cc5dc217f3e95d9cda108e31f4be0bf17f506861","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05b_a","5396c92cd86ad68182558192cc5dc217f3e95d9cda108e31f4be0bf17f506861","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05c","8a142aa6b53e7dc23ca13800811e3fb61e6097042da2fd62f9a18e47b8a296f2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05c","8a142aa6b53e7dc23ca13800811e3fb61e6097042da2fd62f9a18e47b8a296f2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05c_a","75633198acbb794f3294ffbd43a3a73da12e04171ed35d0b622158e4f383abbb","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05c_a","75633198acbb794f3294ffbd43a3a73da12e04171ed35d0b622158e4f383abbb","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05c_b","3809eb02d51783140084a83bf1cc70e67a49580dc4d4c092071e0f88b9ce1837","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05c_b","3809eb02d51783140084a83bf1cc70e67a49580dc4d4c092071e0f88b9ce1837","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05c_c","a65c6a8b6bde3f2e6e30df2d344ed3d68191e332c8310a1116856315c79201d6","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05c_c","a65c6a8b6bde3f2e6e30df2d344ed3d68191e332c8310a1116856315c79201d6","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05d","6598061b35912956271275b2371a953e78bb4598ebc9c90474a6f0bcdd931493","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05d","6598061b35912956271275b2371a953e78bb4598ebc9c90474a6f0bcdd931493","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend05e","e15840c0fa2f15a3c9c96f3c6563309371c6c5682187695524dd0bf1ecca2581","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend05e","e15840c0fa2f15a3c9c96f3c6563309371c6c5682187695524dd0bf1ecca2581","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend07a","5a71e1cf8f500e6c4b0c40d6d57aeae6d71b2b64e23b055825f6f45325ce4401","ProjectedPrelit","tb_sand1",1,2},
        {"tb_bend07a","5a71e1cf8f500e6c4b0c40d6d57aeae6d71b2b64e23b055825f6f45325ce4401","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend07b","79fe0d30ac57879d3e3ed24b29d6e1f85bbc97f881711ff7a179f1a8de6d965a","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend07b","79fe0d30ac57879d3e3ed24b29d6e1f85bbc97f881711ff7a179f1a8de6d965a","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend07c","c6cf8d857c9ab665cbf59f155e4788eab0163a8c0b9a358ebcfd811d99d12cf2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend07c","c6cf8d857c9ab665cbf59f155e4788eab0163a8c0b9a358ebcfd811d99d12cf2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend07d","8aee0082fe3dc79692185ea79d56b0d7f3aa64b15b82e874a423414fe32517a1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend07d","8aee0082fe3dc79692185ea79d56b0d7f3aa64b15b82e874a423414fe32517a1","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend08a","4637b2b3d4bc94ff1b5a4d1739cd9c11a575bd3db4b38767db6dd0f9cf67b1f0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend08a","4637b2b3d4bc94ff1b5a4d1739cd9c11a575bd3db4b38767db6dd0f9cf67b1f0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend08b","311b4dadcfe8e8c146189a862e95b67325d5f42ea45525bb2b83aa267544e191","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend08b","311b4dadcfe8e8c146189a862e95b67325d5f42ea45525bb2b83aa267544e191","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend08c","fd4ef094790ee1ecb4328ad229bdc475cd556f71539f48fbbbec22cdd932f8a7","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend08c","fd4ef094790ee1ecb4328ad229bdc475cd556f71539f48fbbbec22cdd932f8a7","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend08d","0f7dfa98a90a8d01141b5d0ab3a44381bd4dc0bb660f27a00d76f0948f1ee17d","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend08d","0f7dfa98a90a8d01141b5d0ab3a44381bd4dc0bb660f27a00d76f0948f1ee17d","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend08e","92e0cad90bfeeb18605effbc343f55859fd0fdec2cfadf4120d7a06580d217c0","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend08e","92e0cad90bfeeb18605effbc343f55859fd0fdec2cfadf4120d7a06580d217c0","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend09a","763d5fc2171e1e89e9e4f1a328ba10022555fe28343f1b91e40e82dd00d35851","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend09a","763d5fc2171e1e89e9e4f1a328ba10022555fe28343f1b91e40e82dd00d35851","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend09b","ffdd7a8a0db9fa57f52713310fa0e594b40bbc99afa6dbe0952c0b3e48df3643","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend09b","ffdd7a8a0db9fa57f52713310fa0e594b40bbc99afa6dbe0952c0b3e48df3643","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend09c","a26911bc37f0f1113bf9602ec4aacf65429b806a70d0850f08ddfad26d480462","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend09c","a26911bc37f0f1113bf9602ec4aacf65429b806a70d0850f08ddfad26d480462","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend10a","d53291b32cddaf499f3b8c6f4fb0c3bc6d3177f556630e8ef14731ec19710523","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend10a","d53291b32cddaf499f3b8c6f4fb0c3bc6d3177f556630e8ef14731ec19710523","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend10b","0496d2a89ae067f6005aa1c4c0be355e1093263dc0a8c584ee891cf976ca1e9d","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend10b","0496d2a89ae067f6005aa1c4c0be355e1093263dc0a8c584ee891cf976ca1e9d","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend10c","121286f0a5ff38d2a64c4d26f26e74d21724d0fce1ccd428d14ce40a8d7ba557","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend10c","121286f0a5ff38d2a64c4d26f26e74d21724d0fce1ccd428d14ce40a8d7ba557","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend11a","1e5021c0dadae4aeff23409999370a06866e9aaaf7163d7f841068f374acb4fb","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend11a","1e5021c0dadae4aeff23409999370a06866e9aaaf7163d7f841068f374acb4fb","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend11b","507e3bd870e3e4b100781409cd8085d05b625b1a3407dfb871c8874999c6eb91","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend11b","507e3bd870e3e4b100781409cd8085d05b625b1a3407dfb871c8874999c6eb91","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend11c","bce5fec0b629c157d6f3115738523f441f661c0340f569425c19f545fca7ac71","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend11c","bce5fec0b629c157d6f3115738523f441f661c0340f569425c19f545fca7ac71","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend12a","26f58b74c2cc4259ca34dcc0dfd0fe0cd8d42af2ac7d9e00428ac8b50830667e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend12a","26f58b74c2cc4259ca34dcc0dfd0fe0cd8d42af2ac7d9e00428ac8b50830667e","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend12b","bad8e2f537b29091e7298105cd3da15788580876235515158b9304a1aa68c205","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend12b","bad8e2f537b29091e7298105cd3da15788580876235515158b9304a1aa68c205","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend12c","11098c971762f86521af481b98f4042c5545dbc43a376d5fbdedaa1f6ee2bf08","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend12c","11098c971762f86521af481b98f4042c5545dbc43a376d5fbdedaa1f6ee2bf08","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend13a","c405d5010a7025be45e92444979c8249535bc1fcda27cc73aad466591a162dbf","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend13a","c405d5010a7025be45e92444979c8249535bc1fcda27cc73aad466591a162dbf","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend13b","8a9c9d87beada331521e6e2c9fff9a6d22f7b74568e48300be6c8b3a89cc46ac","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend13b","8a9c9d87beada331521e6e2c9fff9a6d22f7b74568e48300be6c8b3a89cc46ac","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend13c","08e981594338793ed8df660617a515b6e3887b9a1deaa9c59d0ac7912d44b037","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend13c","08e981594338793ed8df660617a515b6e3887b9a1deaa9c59d0ac7912d44b037","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend13c_a","2620e968f99960695264d67755ddf670309beaa36747a4299619a3a01390e9cc","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend13c_a","2620e968f99960695264d67755ddf670309beaa36747a4299619a3a01390e9cc","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend14a","4c52fca71b66940c8230cd246b5980746f84ebef5b09f54c285a6fedea51b876","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend14a","4c52fca71b66940c8230cd246b5980746f84ebef5b09f54c285a6fedea51b876","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend14b","6ab129109a38484e5b25068f7344863dd3b342ea89f6526cf89e95b04c330194","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend14b","6ab129109a38484e5b25068f7344863dd3b342ea89f6526cf89e95b04c330194","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend15a","5dbcaeba7be618da1d5af6282c94ed29d379e2cbd5fa901ad28e3abba0541ba1","ProjectedPrelit","tb_sand1",1,2},
        {"tb_bend15a","5dbcaeba7be618da1d5af6282c94ed29d379e2cbd5fa901ad28e3abba0541ba1","ProjectedPrelit","tb_sand2",2,2},
        {"tb_bend15b","4dc66e688f0ea9993f05b8e0b4334fef1550258950182c5ac98e5fcc94599070","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend15b","4dc66e688f0ea9993f05b8e0b4334fef1550258950182c5ac98e5fcc94599070","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend15c","3341561cc773f8949b2e525638031655e61498bb7a30ec530fca5bba8a40b802","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend15c","3341561cc773f8949b2e525638031655e61498bb7a30ec530fca5bba8a40b802","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend_obj08","af13b13f39d8df4246d5514dbdfeac0f8849f0340e57c3be83d28e8aaf41fc85","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend_obj08","af13b13f39d8df4246d5514dbdfeac0f8849f0340e57c3be83d28e8aaf41fc85","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend_obj09","646a18d88cc543d23653d5f7de7bb0d976266b2c58e22301477dc618233ee5c2","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend_obj09","646a18d88cc543d23653d5f7de7bb0d976266b2c58e22301477dc618233ee5c2","ProjectedPrelit","ta_rock2",2,2},
        {"tb_bend_obj10","e71179d7f3abd90d204d76421233ecd5f5ba4b9fd431f6964c0b795bee201d83","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_bend_obj10","e71179d7f3abd90d204d76421233ecd5f5ba4b9fd431f6964c0b795bee201d83","ProjectedPrelit","ta_rock2",2,2},
        {"zone5_arch","b3c99cf5935fc0adf502cec5a623f3948d6ead4dc52c9256de24086a482fa710","ProjectedPrelit","ta_rockb_a",1,2},
        {"zone5_arch","b3c99cf5935fc0adf502cec5a623f3948d6ead4dc52c9256de24086a482fa710","ProjectedPrelit","ta_rock2",2,2},
        {"mush_2a","840736680687f144cf515e3b9f9b09cbb8d2f3d232eb63b72b950db3974d8516","PreLitCompressed","mush_2a",1,1},
        {"mush_3","7a0fe4ea66844d83b95927a99bcc5a6f7f0df96115be8816de76340696a76cc1","prelitCompressed","mush_3",1,1},
        {"mush_1","208795595c5cd846fc55a9ccef6b14e496ae5de758f7bde8d658d5e12d765461","DiffuseCompressed","mush_1",1,1},
        {"mush_3a","aaef181def732574171d63730229324cb47fb36b4ea9753b0a5ad87e1f0a75aa","DiffuseCompressed","mush_3a",1,1},
        {"mush_1a","aa5df40bd96ab8072d699f214275476effade94139b1205112d989bad9f23aae","DiffuseCompressed","mush_1a",1,1},
        {"mush_4","034987c1631352f571271658a154d1d7b0da2c2631515e0e99fd80a468f51b11","DiffuseCompressed","mush_4",1,1},
        {"tb_waldo_01","9824daacee3e248b1457de872a8b3cf0f4ef085e5adf3105b211a337e9988514","ProjectedPrelit","tb_sand1",1,2},
        {"tb_waldo_01","9824daacee3e248b1457de872a8b3cf0f4ef085e5adf3105b211a337e9988514","ProjectedPrelit","tb_sand2",2,2},
        {"tb_waldo_02","3067cccb0b2a0528d48bb95e3a4c780db263e1c9193831b9d0149b6911cfab0e","ProjectedPrelit","tb_sand1",1,2},
        {"tb_waldo_02","3067cccb0b2a0528d48bb95e3a4c780db263e1c9193831b9d0149b6911cfab0e","ProjectedPrelit","tb_sand2",2,2},
        {"tb_waldo_03","c013a6a4c2722d967f6d7a69e40325f50246ca5e460aa6af8ad5c5a513c777d6","ProjectedPrelit","tb_sand1",1,2},
        {"tb_waldo_03","c013a6a4c2722d967f6d7a69e40325f50246ca5e460aa6af8ad5c5a513c777d6","ProjectedPrelit","tb_sand2",2,2},
        {"tb_waldo_04","ad152ae7c1b8417b8b1a14ac242f2688f306cc5c4c782117e09a67b99c54dca8","ProjectedPrelit","tb_sand1",1,2},
        {"tb_waldo_04","ad152ae7c1b8417b8b1a14ac242f2688f306cc5c4c782117e09a67b99c54dca8","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor27","d564a4011df44652a0feae33f24ed68576dbc8ef105ecb2e3981cc7f5a6258e0","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor27","d564a4011df44652a0feae33f24ed68576dbc8ef105ecb2e3981cc7f5a6258e0","ProjectedPrelit","tb_sand2",2,2},
        {"object03","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rockb_a",1,2},
        {"object03","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rock2",2,2},
        {"object05","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rockb_a",1,2},
        {"object05","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rock2",2,2},
        {"object06","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rockb_a",1,2},
        {"object06","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor01","ccf6cca2d72c48417d4119c902f759b9e16b3bc000cee9596e233ca9456366f0","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor01","ccf6cca2d72c48417d4119c902f759b9e16b3bc000cee9596e233ca9456366f0","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor21","37152a406457717a090e6e3fec5684b165193b6f10015de3ea47292fece511a3","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor21","37152a406457717a090e6e3fec5684b165193b6f10015de3ea47292fece511a3","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor22","885c87fab2a7b8a36c9c31cd105c465bd9c9f9c9476de574f78ecc86291d662c","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor22","885c87fab2a7b8a36c9c31cd105c465bd9c9f9c9476de574f78ecc86291d662c","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor23","af7e38a2f7b54d2358a61d5cd0fcba24c7976007655c448ebe2d2b2b1ac4fed7","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor23","af7e38a2f7b54d2358a61d5cd0fcba24c7976007655c448ebe2d2b2b1ac4fed7","ProjectedPrelit","tb_sand2",2,2},
        {"object01","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rockb_a",1,2},
        {"object01","fcf23c5bfbd6b65fe276f596254b3406c93cd6c358ddb67a5b81b43aa1db335a","ProjectedPrelit","ta_rock2",2,2},
        {"tb_arenafloor28","fca809f3542adc1aac551b39af5c2ceff75c96cb919421c19d8abb6cf557d116","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor28","fca809f3542adc1aac551b39af5c2ceff75c96cb919421c19d8abb6cf557d116","ProjectedPrelit","tb_sand2",2,2},
        {"tb_arenafloor29","027294cc56ffa86712e0311bf3a87069efcdd6acd16f0a1d089c5cb839d29f87","ProjectedPrelit","tb_sand1",1,2},
        {"tb_arenafloor29","027294cc56ffa86712e0311bf3a87069efcdd6acd16f0a1d089c5cb839d29f87","ProjectedPrelit","tb_sand2",2,2},
        {"tb_brancha02a","1e15a32ba750320461e97776bdcd1ca17b1e728f33e71f49ae221ad38d4da406","ProjectedPrelit","tb_sand1",1,2},
        {"tb_brancha02a","1e15a32ba750320461e97776bdcd1ca17b1e728f33e71f49ae221ad38d4da406","ProjectedPrelit","tb_sand2",2,2},
        {"tb_brancha02b","be5daed4a2a773b9330696eff3769700578ad5aca333c601b66cc8ef952db719","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha02b","be5daed4a2a773b9330696eff3769700578ad5aca333c601b66cc8ef952db719","ProjectedPrelit","ta_rock2",2,2},
        {"tb_brancha03a","e1c4fa979dc4e88facd854ecb25b3de9d9a5085c3165f6b89f731efcf5cb5e90","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha03a","e1c4fa979dc4e88facd854ecb25b3de9d9a5085c3165f6b89f731efcf5cb5e90","ProjectedPrelit","tb_sand2",2,2},
        {"tb_brancha03b","74a946d9583c7ba1d3c6f51f6523c568571d3a1a57d3d06dfb19c6edc24eeff7","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha03b","74a946d9583c7ba1d3c6f51f6523c568571d3a1a57d3d06dfb19c6edc24eeff7","ProjectedPrelit","ta_rock2",2,2},
        {"tb_brancha04a","8f587d181e70fab50c9eabc08b2c3af4348b61187d7faeda274120db670913cd","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha04a","8f587d181e70fab50c9eabc08b2c3af4348b61187d7faeda274120db670913cd","ProjectedPrelit","tb_sand2",2,2},
        {"tb_brancha04b","5df112ae89c50fb0d3c891266543b222f2eb501faa7cc4ce63d68fdeb150c051","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha04b","5df112ae89c50fb0d3c891266543b222f2eb501faa7cc4ce63d68fdeb150c051","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgecap03c","09ed4a9987ea3f21c9d8615b6753a6c36b5c0056692bd2b9f297ab41321f5a02","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgecap03c","09ed4a9987ea3f21c9d8615b6753a6c36b5c0056692bd2b9f297ab41321f5a02","ProjectedPrelit","ta_rock2",2,2},
        {"tb_brancha04x","42f5c83d97012d1e8dc077b079d1015c0656a601d06c93f707b6afe4f66b4122","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_brancha04x","42f5c83d97012d1e8dc077b079d1015c0656a601d06c93f707b6afe4f66b4122","ProjectedPrelit","ta_rock2",2,2},
        {"tb_brancha04y","123fe72fefa2e5b9f06f4043138a6bab83e411b9fa7c457c3dd1476d351afed1","ProjectedPrelit","tb_sand1",1,2},
        {"tb_brancha04y","123fe72fefa2e5b9f06f4043138a6bab83e411b9fa7c457c3dd1476d351afed1","ProjectedPrelit","tb_sand2",2,2},
        {"tb_branchacap1","c532294327b7e0ceea8a3792d86c4970b5a6ed087d173396ddaf078b79db7f66","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_branchacap1","c532294327b7e0ceea8a3792d86c4970b5a6ed087d173396ddaf078b79db7f66","ProjectedPrelit","ta_rocka_a",2,2},
        {"tb_farmpaths101a","98bb1948e9a4f741021df4a2e7316e00be1fcae80d9ab1d58f2d540a6fe66a03","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths101a","98bb1948e9a4f741021df4a2e7316e00be1fcae80d9ab1d58f2d540a6fe66a03","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths101b","4a887eae821956421c91c17845e0bbdb7d0c3fe56c77fc3fa394ed06df06d4fb","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths101b","4a887eae821956421c91c17845e0bbdb7d0c3fe56c77fc3fa394ed06df06d4fb","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths101c","e4da50feb7df501e140ca83875014d939b8106078d7dd2769caed130eb4bc66e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths101c","e4da50feb7df501e140ca83875014d939b8106078d7dd2769caed130eb4bc66e","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths101d","4e1767879951b46b8c7d45a8a58e06f40aee7104af2c1f56735060f14c2bdf1b","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths101d","4e1767879951b46b8c7d45a8a58e06f40aee7104af2c1f56735060f14c2bdf1b","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths102a","4ae8775a2ed8bd0c261a343dbb1a099548d04769887fe72e20e525e99560544d","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths102a","4ae8775a2ed8bd0c261a343dbb1a099548d04769887fe72e20e525e99560544d","ProjectedPrelit","tb_sand2",2,2},
        {"tb_farmpaths102b","bb81032784ee0928512f4fb0d9531f3eadff7474ed5595216239588bc578ee09","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths102b","bb81032784ee0928512f4fb0d9531f3eadff7474ed5595216239588bc578ee09","ProjectedPrelit","ta_rock2",2,2},
        {"tb_farmpaths102c","499817f2beb3d3149cf86752bebe717a784c44bb05b29c6c17a4a643d73dfbf3","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_farmpaths102c","499817f2beb3d3149cf86752bebe717a784c44bb05b29c6c17a4a643d73dfbf3","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgecap01a","8347c8bc02b66ea930d44c811f9402dc6e375b62b12ea47117aeb9d6c9e84832","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgecap01a","8347c8bc02b66ea930d44c811f9402dc6e375b62b12ea47117aeb9d6c9e84832","ProjectedPrelit","tb_sand2",2,2},
        {"tb_gorgecap01c","0e12b0fb69ac1a7461f1a5aa976154558f29dc29a98d509370b9d42281cfa70c","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgecap01c","0e12b0fb69ac1a7461f1a5aa976154558f29dc29a98d509370b9d42281cfa70c","ProjectedPrelit","tb_sand2",2,2},
        {"tb_gorgecap01d","375ecc56824ce33e221936af3dc36cddd5b639a8505ad5cfd7884cd14474b2de","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgecap01d","375ecc56824ce33e221936af3dc36cddd5b639a8505ad5cfd7884cd14474b2de","ProjectedPrelit","tb_sand2",2,2},
        {"tb_gorgecap03a","83066b96268d027b7e883b6b0f87d0ae8eb2e29c1de46f2925750d24c9b3b1f1","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgecap03a","83066b96268d027b7e883b6b0f87d0ae8eb2e29c1de46f2925750d24c9b3b1f1","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgecap03b","909d618f41ed80aac8c4877a9c81d49d49872d5f6bd301a9cf14bd1887761efb","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgecap03b","909d618f41ed80aac8c4877a9c81d49d49872d5f6bd301a9cf14bd1887761efb","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump03a","ba4288767ddb15b8f994fa2fea46a94e9215396f2d7284158b4f7eb11605e4ba","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump03a","ba4288767ddb15b8f994fa2fea46a94e9215396f2d7284158b4f7eb11605e4ba","ProjectedPrelit","tb_sand2",2,2},
        {"tb_gorgejump03b","060acdb5ffb939b62ebd11e9c14d25de227e01b8349987782eaff80d6d26f42f","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump03b","060acdb5ffb939b62ebd11e9c14d25de227e01b8349987782eaff80d6d26f42f","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump03c","58ca969d542926ec5f9de5d09e2ff7dcfadc6e734500f06403c91c77a0ad5087","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump03c","58ca969d542926ec5f9de5d09e2ff7dcfadc6e734500f06403c91c77a0ad5087","ProjectedPrelit","ta_rock2",2,2},
        {"tb_gorgejump03d","ad51b7cf6023d272ed29a93f40004df451179953e15c518310215b995c08cb8e","ProjectedPrelit","ta_rockb_a",1,2},
        {"tb_gorgejump03d","ad51b7cf6023d272ed29a93f40004df451179953e15c518310215b995c08cb8e","ProjectedPrelit","ta_rock2",2,2},
    };
    bool identity=false;
    for(const auto& row:identities)if(selection.materialKey==row.key&&detail::lower(selection.materialName)==row.name&&
        selection.program==row.program&&selection.textureName==row.map&&selection.pass==row.pass&&selection.passCount==row.count){identity=true;break;}
    if(!identity)return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    const auto& fields = selection.sourcePassFields;
    if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
    auto sourceInteger = [&](const char* name, std::int32_t& result) {
        bool found = false;
        for (const auto& item : fields.at("authored")) {
            if (!item.is_object() || !item.contains("name") || !item.contains("value")) return false;
            if (!item.at("name").is_string()) return false;
            if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
            if (found || !item.at("value").is_string()) return false;
            const auto text = item.at("value").get<std::string>();
            const auto parsed = std::from_chars(text.data(), text.data()+text.size(), result);
            if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
            found = true;
        }
        return true;
    };
    std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG> gs{};
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !sourceInteger("inputAlphaD", gs[i])) return false;
        if (!sourceInteger(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const auto program = detail::lower(selection.program);
    const bool shield = selection.track == track_source_fixture::Track::tb &&
        detail::lower(selection.materialName) == "tc_shield" &&
        detail::lower(selection.textureName) == "tc_shield";
    if(selection.projectedPrelit){
        if(selection.program!="ProjectedPrelit"||selection.passCount!=2||get(Field::alphaTestOnOff)!=0||
            get(Field::alphaBlendingOnOff)!=(selection.pass==1?0:1))return false;
    }else if(selection.passCount!=1||selection.pass!=1||
        (program!="prelitcompressed"&&program!="diffusecompressed"&&!(shield&&program=="diffusetextranscompressed")))return false;
    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = selection.projectedPrelit || program == "prelitcompressed";
    next.unlit = selection.projectedPrelit; // Existing source-space ProjectedPrelit presentation path.
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    for (const auto& item : fields.at("authored"))
        if (detail::lower(item.at("name").get<std::string>()) == "zwritedisabled") next.sourceDepthWriteSpecified = true;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || (get(Field::alphaReferenceValue) < 0 || get(Field::alphaReferenceValue) > 255) ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (get(Field::alphaBlendingOnOff)) {
        if (get(Field::inputColorA) != 0 || get(Field::inputAlphaC) != 0 ||
            get(Field::inputColorD) != 1 || (get(Field::inputColorB) != 1 && get(Field::inputColorB) != 2) ||
            selection.textureIndex == track_materials50fa_fixture::unresolved) return false;
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Existing declared native GS-unit modulation policy. This does not
        // assert PS2 fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}

} // namespace material_alpha50gx_fixture
