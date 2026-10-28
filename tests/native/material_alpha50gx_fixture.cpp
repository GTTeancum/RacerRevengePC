// Out-of-line native implementation; owner layout and operation bodies are unchanged.
#include "material_alpha50gx_fixture.h"

namespace material_alpha50gx_fixture {

bool selectGc(const track_materials50fa_fixture::Selection& selection, Policy& output) {
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

bool selectGa(const track_materials50fa_fixture::Selection& selection,
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

bool select(const track_materials50fa_fixture::Selection& selection, Policy& output) {
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

bool selectMaSa(const track_materials50fa_fixture::Selection& selection,
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

bool selectMb(const track_materials50fa_fixture::Selection& selection,
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

bool selectTc(const track_materials50fa_fixture::Selection& selection,
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

bool selectRc(const track_materials50fa_fixture::Selection& selection,
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

bool selectSc(const track_materials50fa_fixture::Selection& selection,
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

bool selectTb(const track_materials50fa_fixture::Selection& selection,
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

bool selectGb(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    using Field = rr::assets::OriginalUiGsField50KG;
    if (selection.track != track_source_fixture::Track::gb || selection.assetIdentity != 0x4742 || selection.slot != 1 ||
        !selection.pass || selection.pass>selection.passCount || !selection.passCount || selection.passCount>2 ||
        !runtime.overrideKnown || runtime.override435670) return false;
    // These material keys belong to the authenticated GB world source closure.
    struct Identity {const char* name;const char* key;const char* program;const char* map;std::uint32_t pass,count;};
    static constexpr Identity identities[]={
        {"fience02","fceb894538ab31d248c7d46f15f3528d180c81c784f252177c6c1eaeb698fe9c","DiffuseCompressed","fience02",1,1},
        {"fience_opening","566f72fad9556a35bf5ee060cea31c7a1102976cd35feff4da29272296b1c499","DiffuseCompressed","fience_opening",1,1},
        {"market_hut_a01","daf1b6ae0d9ec8dc642487b0097cbf3bafe506338b8f65bdbdca8c9059190456","DiffuseCompressed","market_hut_a01",1,1},
        {"opening_a","9c9b1b0cca089b02eeae092e0a0bffa28b31848e2989244137b3c5bffae6bcda","DiffuseCompressed","opening_a",1,1},
        {"house_stand_e01","83d665d65d3498ab7e337efa5abbcfdb5b6deb7624029ba3bfc6a11331dbfed6","DiffuseCompressed","house_stand_e01",1,1},
        {"market_hut08","db0e6dc6df1576a4ed3c3251afb3da3da7d612ad0e9021ed968aced79199781d","DiffuseCompressed","market_hut08",1,1},
        {"market_hut07","3997bfd377561bb6eb730affa9935ae9debeb0ac7ae9eb6b275a7f75bff33ec1","DiffuseCompressed","market_hut07",1,1},

        {"arrowdroid01","5c3e3b274e39735e703c9b200c4ab3933ad17d96ceec41e045d7382b99a6fe80","PreLitCompressed","arrowdroid01",1,1},
        {"arrowdroid03","777bf3fb8d960848c40a05417f672bd211a859900a7c13deb84f02bf7e93ea10","PreLitCompressed","arrowdroid03",1,1},
        {"arrowred","c9b89baa1d81e4427ee5a4e594cdeea9f7708edde4e18c330a317004adb791bb","PrelitCompressed","arrowred",1,1},
        {"big_wilo","001dd20d06926f403cf9a75694328792b6ba31718584cf175a7562ac9ac0dc71","PreLitCompressed","big_wilo",1,1},
        {"big_wilo_leaf","249fbc2b447a5e83a90824747629acb0997e5a12b0afa19565a705be6ffe933b","PreLitCompressed","big_wilo_leaf",1,1},
        {"camdroid03","fe9fe0f1ac699c54b41af888077ad51b15f51b14619411554a4844b3361aaf7a","PreLitCompressed","camdroid03",1,1},
        {"cord","f412f61c202d72b1f0ba4ef66dc13d1be0bbbbafc9d297c65b9df1f4e2252270","PreLitCompressed","cord",1,1},
        {"fience","f098d94f3cc08be788c471148fa913b8984acba72020230cc1b27778fee71a5a","DiffuseCompressed","fience",1,1},
        {"fience01","5d9184c0c79325de5f4b80ca5ab472e3b87c740b1fd23ca575148e49bd203d15","PreLitCompressed","fience01",1,1},
        {"fort_mount_a","ae10eb4392dd4a039d21472edd168aa6dd26e53a9748f4a5fc92d85cb1bf85fa","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_a","ae10eb4392dd4a039d21472edd168aa6dd26e53a9748f4a5fc92d85cb1bf85fa","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_b","0183f9ab1bca443487affeb726bf080a8eb7bf0dc0f9712451cdf49771dd6bfe","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_b","0183f9ab1bca443487affeb726bf080a8eb7bf0dc0f9712451cdf49771dd6bfe","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_c","35359bf90db109e511083d1cef324fed1ee9006b6603a0e16e628fd4bf06898e","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_c","35359bf90db109e511083d1cef324fed1ee9006b6603a0e16e628fd4bf06898e","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_d","6d74eb72c41e10fba873832c020bc85a75d680d4b4c0cd93eb64cfe647867ee8","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_d","6d74eb72c41e10fba873832c020bc85a75d680d4b4c0cd93eb64cfe647867ee8","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_e","a17cc4b2ac214dabce2f4b71476b5e92a9a489ce71c2af3b03a529784a4c034d","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_e","a17cc4b2ac214dabce2f4b71476b5e92a9a489ce71c2af3b03a529784a4c034d","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_f","9c7df202b64892b695c2fde37a3f8a75c9013b0e16b45d1b072b541c35b56002","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_f","9c7df202b64892b695c2fde37a3f8a75c9013b0e16b45d1b072b541c35b56002","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_g","9ab314262a993dabb68ef16b7443da96e719f6fc00a7bfadf75acb54bf0b6e7c","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_g","9ab314262a993dabb68ef16b7443da96e719f6fc00a7bfadf75acb54bf0b6e7c","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_h","e273d1668a88bb63da094c7b9ae50f8a51b87133dafa1562a9863dde5b3a9000","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_h","e273d1668a88bb63da094c7b9ae50f8a51b87133dafa1562a9863dde5b3a9000","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_i","ecb1318ca653d7516584baeccfb55874cd436305d800b38e702e5784180c50e8","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_i","ecb1318ca653d7516584baeccfb55874cd436305d800b38e702e5784180c50e8","ProjectedPrelit","forttress_tex_a",2,2},
        {"fort_mount_j","ba410bec66be582d346de7911560cba32e22990213b2a2077c6fca5c58523603","ProjectedPrelit","forttress_tex_b",1,2},
        {"fort_mount_j","ba410bec66be582d346de7911560cba32e22990213b2a2077c6fca5c58523603","ProjectedPrelit","forttress_tex_a",2,2},
        {"fortress_a","4e5537c790dff88f65f7c6c69367669859c2fc6f744c87e1aa31083ba48d2e45","PreLitCompressed","fortress_a",1,1},
        {"fortress_b","59b29c1933e9d1433e8f5c9e72cdf1a67d62401856705813349a84f7d367f494","PreLitCompressed","fortress_b",1,1},
        {"fortress_c","f90d4d0f40cb5abb51965095e3cd6c0f57c97a66955150abd2b778caa65ebe31","PreLitCompressed","fortress_c",1,1},
        {"fortress_d","ef6ba835c5fd7eeae82a4e2111cd91af4b175e17c3f3491bd56ef2706ba0e0eb","PreLitCompressed","fortress_d",1,1},
        {"fortress_f","d99d7a97202b5a8be52e9fa10a21f6072ff1e451be25dd61ffddfddadb11fd98","PreLitCompressed","fortress_f",1,1},
        {"fortress_g","1fbdec2dfa6fb55929a0acd63cf209e5cd6ea75ff9afed31238200755c4586b5","PreLitCompressed","fortress_g",1,1},
        {"gam_brige","1becb1615c1c69d53ffc77de0640627a676b498bdc92f25aabbfc1ebeb9e9508","PreLitCompressed","gam_brige",1,1},
        {"gam_flare","ab2e7318e09f3847d419427e22695a122e1413e9583467b1ae8282419c02b65b","PrelitCompressed","gam_flare",1,1},
        {"gam_tree_g","a183946c633493fb160165ac24a9d8014d7ebcf775f400ab24330867c191022b","PreLitCompressed","gam_tree_g",1,1},
        {"gb_z10_surface01","d2bea88d12503c72eba02e139c87699fdacd8570b0012826cd52c3fa2a0c483f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface01","d2bea88d12503c72eba02e139c87699fdacd8570b0012826cd52c3fa2a0c483f","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface02","13d10d3516e6a05cb59e06dae2debb836b25be9430e03274f0871fdf71ab6aed","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface02","13d10d3516e6a05cb59e06dae2debb836b25be9430e03274f0871fdf71ab6aed","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface03","01ea8659ed951a493c8ac228a8aa933702c2f17357ed458a2d95c27ee9860ad7","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface03","01ea8659ed951a493c8ac228a8aa933702c2f17357ed458a2d95c27ee9860ad7","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface04","9e72b5f30e0b48c69abe99f8c0fcc3f2b725902d00ab73c3d9afeea72e158c82","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface04","9e72b5f30e0b48c69abe99f8c0fcc3f2b725902d00ab73c3d9afeea72e158c82","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface05","da51036e7a47dd46defd3ecbd9aebf03dbd40073c9ffa79c2711b0aaef59eab8","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface05","da51036e7a47dd46defd3ecbd9aebf03dbd40073c9ffa79c2711b0aaef59eab8","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface06","76b0a6145e04b4bdb352ecc3aea12ea20b81aa04b5ff0974ca7f42e313c09c28","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface06","76b0a6145e04b4bdb352ecc3aea12ea20b81aa04b5ff0974ca7f42e313c09c28","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface07","0f8510d2824837b276f0030376e6ab674e24d7bfd6ecaad7011a0d7e4a15dc4f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface07","0f8510d2824837b276f0030376e6ab674e24d7bfd6ecaad7011a0d7e4a15dc4f","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface08","6545276ac2721a13e490adfd53cd4a6463f40e4b5a62ca44731fae4136b61cbf","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface08","6545276ac2721a13e490adfd53cd4a6463f40e4b5a62ca44731fae4136b61cbf","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface09","abd427cf857ea63c2a74f505d532995755a2accfbf8b2a360a40eee93ee0f1e1","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface09","abd427cf857ea63c2a74f505d532995755a2accfbf8b2a360a40eee93ee0f1e1","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface10","772c8f8a5575be946c9ff68e296ccbbde6bc5eae6ff3c196aee2c03a891b8ae6","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface10","772c8f8a5575be946c9ff68e296ccbbde6bc5eae6ff3c196aee2c03a891b8ae6","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface11","9121b3da705966be1667c42120ea60f7cb580b37d56cd3ee50eec3c6aecbc3fb","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface11","9121b3da705966be1667c42120ea60f7cb580b37d56cd3ee50eec3c6aecbc3fb","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface12","46c72f5c5496d5b346fe191da6729ac7bf1fa337f71296583bccdefc73f2ec5f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface12","46c72f5c5496d5b346fe191da6729ac7bf1fa337f71296583bccdefc73f2ec5f","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface13","ff7a3adccbf317fbb40d360ecb4a24e90342dd271fa42510a1e2e347bc28afd2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface13","ff7a3adccbf317fbb40d360ecb4a24e90342dd271fa42510a1e2e347bc28afd2","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface14","2b59f299c491f0eb66d1195882f5d811aa335b1d4e1287d29c781328fa2d007f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface14","2b59f299c491f0eb66d1195882f5d811aa335b1d4e1287d29c781328fa2d007f","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_surface15","a9327990f617e6a17444293ff5ae28c88ae1c80a6f55dc0db01991e52edbc358","ProjectedPrelit","surfacetex",1,2},
        {"gb_z10_surface15","a9327990f617e6a17444293ff5ae28c88ae1c80a6f55dc0db01991e52edbc358","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_track_a","0fa432355078fc138aaae7e6b48fad5c224620d680656df3ae7aed91e458c46c","ProjectedPrelit","roadtex",1,2},
        {"gb_z10_track_a","0fa432355078fc138aaae7e6b48fad5c224620d680656df3ae7aed91e458c46c","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_track_b","48c87ae5b7f31f1575c4664b6380e9cc5a2e27ecc27994fa96c9d0ceb7254150","ProjectedPrelit","roadtex",1,2},
        {"gb_z10_track_b","48c87ae5b7f31f1575c4664b6380e9cc5a2e27ecc27994fa96c9d0ceb7254150","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_track_c","f4b2e7cbf8eab7c83e35b86d5b9cb88be0d81fb28a5bc6a3634de2966b606723","ProjectedPrelit","roadtex",1,2},
        {"gb_z10_track_c","f4b2e7cbf8eab7c83e35b86d5b9cb88be0d81fb28a5bc6a3634de2966b606723","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_track_d","6b55d0216e60ed8c9cb72fd9d977b487de57ef09895e985c459f069b1e65e41a","ProjectedPrelit","roadtex",1,2},
        {"gb_z10_track_d","6b55d0216e60ed8c9cb72fd9d977b487de57ef09895e985c459f069b1e65e41a","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_track_e","63aebb959bd11405194b602333752d9b8a875de55049ae24746cc75367104982","ProjectedPrelit","roadtex",1,2},
        {"gb_z10_track_e","63aebb959bd11405194b602333752d9b8a875de55049ae24746cc75367104982","ProjectedPrelit","groundtex",2,2},
        {"gb_z10_track_f","29e4e66a6bb04b8b132ddaecbde05fd04842ccb85209110dde9fda503b5f655c","ProjectedPrelit","roadtex",1,2},
        {"gb_z10_track_f","29e4e66a6bb04b8b132ddaecbde05fd04842ccb85209110dde9fda503b5f655c","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface01","65fa535764827ca881a38ba49828ba50f812811f2ac113938ae2a67eb9fe249c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface01","65fa535764827ca881a38ba49828ba50f812811f2ac113938ae2a67eb9fe249c","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface02","fc2a96ce122b1475c955dc8305e6323f0af66f9f7e6b0b8b549dfe18b53eb8b5","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface02","fc2a96ce122b1475c955dc8305e6323f0af66f9f7e6b0b8b549dfe18b53eb8b5","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface03","4dd65784b8a5822e743757854088faa84e33bfc630982dc8d4e8cb2997243578","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface03","4dd65784b8a5822e743757854088faa84e33bfc630982dc8d4e8cb2997243578","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface04","f45b9cae3146783e077f636752a1402ebda570d73ecfccd6c58b8b48350bb070","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface04","f45b9cae3146783e077f636752a1402ebda570d73ecfccd6c58b8b48350bb070","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface05","8ecc76292a7e33ecb164ab2b437167e198c9ef32123cffb37cb75b5bda8046c3","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface05","8ecc76292a7e33ecb164ab2b437167e198c9ef32123cffb37cb75b5bda8046c3","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface06","132efa34d9ec563eab2b9c3894833ef7902da40f2998c50ea8bb16d17440d316","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface06","132efa34d9ec563eab2b9c3894833ef7902da40f2998c50ea8bb16d17440d316","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_surface07","f8802bb670488f6c0cfa41106cbdc2a2ad715b86511fd361175c5b38ba81cec6","ProjectedPrelit","surfacetex",1,2},
        {"gb_z11_surface07","f8802bb670488f6c0cfa41106cbdc2a2ad715b86511fd361175c5b38ba81cec6","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_track_a","f554b3727c0fac6e8206cfd9c6cac6d3256db86c07f904b650f2668bf6517a8d","ProjectedPrelit","roadtex",1,2},
        {"gb_z11_track_a","f554b3727c0fac6e8206cfd9c6cac6d3256db86c07f904b650f2668bf6517a8d","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_track_b","cf381ace0304a3014b9faf8bb3b831a4642b79f6b0401d9cbec9b3e450453e52","ProjectedPrelit","roadtex",1,2},
        {"gb_z11_track_b","cf381ace0304a3014b9faf8bb3b831a4642b79f6b0401d9cbec9b3e450453e52","ProjectedPrelit","groundtex",2,2},
        {"gb_z11_track_c","6238d86ccee24ab5dbb928e951fd3e525812823234a956c3b286abaa27612c41","ProjectedPrelit","roadtex",1,2},
        {"gb_z11_track_c","6238d86ccee24ab5dbb928e951fd3e525812823234a956c3b286abaa27612c41","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface01","4584e2befae361c1b7c248ed7c6e27fb49fd5cf92c4f5029e22a29a0859e6a5f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface01","4584e2befae361c1b7c248ed7c6e27fb49fd5cf92c4f5029e22a29a0859e6a5f","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface02","982e7a5321d349b5bc84ad48d57605ff98b3ccb86fe1e0818ef5ce919ace86d7","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface02","982e7a5321d349b5bc84ad48d57605ff98b3ccb86fe1e0818ef5ce919ace86d7","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface03","ab208de378234693da20c35dc8938bce5c48b64f2a28aa41c1a01aad61a43579","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface03","ab208de378234693da20c35dc8938bce5c48b64f2a28aa41c1a01aad61a43579","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface04","ac58160e59e28877f3caa1845f30b7289d6df88f62b0cf995c2eb570d612dd4f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface04","ac58160e59e28877f3caa1845f30b7289d6df88f62b0cf995c2eb570d612dd4f","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface05","bc8e2e4d1fa257f763765b68b61f8ca11343dc326995081b53208b677d9610e2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface05","bc8e2e4d1fa257f763765b68b61f8ca11343dc326995081b53208b677d9610e2","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface06","605a113fe6835fe8814b841e6d8749c5a113a4ada1ccbbc230aa05bb2e90b93d","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface06","605a113fe6835fe8814b841e6d8749c5a113a4ada1ccbbc230aa05bb2e90b93d","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface07","e9e47881432edae7504f6ba77b4a1835b1270d1de2870e2d9c8d2ba93d78f9d3","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface07","e9e47881432edae7504f6ba77b4a1835b1270d1de2870e2d9c8d2ba93d78f9d3","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface08","c2e7398926a39ab41b4deecedd42ec98c28c52fd7058bf1b10089fc125351803","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface08","c2e7398926a39ab41b4deecedd42ec98c28c52fd7058bf1b10089fc125351803","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface09","ea1e0804dc18362f6db6c1d9195c17948742c36c1645faa6dc814466d98f47c5","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface09","ea1e0804dc18362f6db6c1d9195c17948742c36c1645faa6dc814466d98f47c5","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface10","452008b5ef5d0a7eff92a0e18b69e576d4656e1a27aff5c22fea23ac8cca4543","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface10","452008b5ef5d0a7eff92a0e18b69e576d4656e1a27aff5c22fea23ac8cca4543","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface11","7a254e931dcfeac8b22bdeff16f400742967ce9ec77965f0423114d726081763","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface11","7a254e931dcfeac8b22bdeff16f400742967ce9ec77965f0423114d726081763","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_surface12","5b4f73a5672be27167e1ef9e8fad3ae522c854acbb15776106c0ce4186610fad","ProjectedPrelit","surfacetex",1,2},
        {"gb_z12_surface12","5b4f73a5672be27167e1ef9e8fad3ae522c854acbb15776106c0ce4186610fad","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_track_a","3310c6de4e3a008143fdbf3e8a901a9c5d9d1767bb2ec80cdcf62150af04f4cd","ProjectedPrelit","roadtex",1,2},
        {"gb_z12_track_a","3310c6de4e3a008143fdbf3e8a901a9c5d9d1767bb2ec80cdcf62150af04f4cd","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_track_b","8e368c3fab22e3929cf4f2f8e1a75aef58316af539e356dd2ea403053fd4b484","ProjectedPrelit","roadtex",1,2},
        {"gb_z12_track_b","8e368c3fab22e3929cf4f2f8e1a75aef58316af539e356dd2ea403053fd4b484","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_track_c","55b27029d04eb2244f0a534c0498d2c82ca98c3aaf0b777f1d501fb6f9383885","ProjectedPrelit","roadtex",1,2},
        {"gb_z12_track_c","55b27029d04eb2244f0a534c0498d2c82ca98c3aaf0b777f1d501fb6f9383885","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_track_d","5d83f333375b5e64e2dde6e17da85edb690ee9d75041a0828662cdfd15cd6854","ProjectedPrelit","roadtex",1,2},
        {"gb_z12_track_d","5d83f333375b5e64e2dde6e17da85edb690ee9d75041a0828662cdfd15cd6854","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_track_e","1771e64ed6cec3178729f5a8d1179c0d0cbe2b06ada3be82617687cc05db0e85","ProjectedPrelit","roadtex",1,2},
        {"gb_z12_track_e","1771e64ed6cec3178729f5a8d1179c0d0cbe2b06ada3be82617687cc05db0e85","ProjectedPrelit","groundtex",2,2},
        {"gb_z12_track_f","8d5f47c308973161f8b78bcc66a7884ffd7ddc556cbc4e147a0c0f41a64ae73a","ProjectedPrelit","roadtex",1,2},
        {"gb_z12_track_f","8d5f47c308973161f8b78bcc66a7884ffd7ddc556cbc4e147a0c0f41a64ae73a","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_surface01","04dd86480378f75bc388aefb1ee0d91a6e80825b4858498f2ebc216fdabf46ff","ProjectedPrelit","surfacetex",1,2},
        {"gb_z13_surface01","04dd86480378f75bc388aefb1ee0d91a6e80825b4858498f2ebc216fdabf46ff","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_surface02","bdfedee3b92f84dc2ea04c682e9aa870d84cdfa39efb4997b9b9fc61b109b927","ProjectedPrelit","surfacetex",1,2},
        {"gb_z13_surface02","bdfedee3b92f84dc2ea04c682e9aa870d84cdfa39efb4997b9b9fc61b109b927","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_surface03","80cbf8ff3ad02aed68adcdf0bbefefdc371ef2020408fb4c2bd02432b2d46317","ProjectedPrelit","surfacetex",1,2},
        {"gb_z13_surface03","80cbf8ff3ad02aed68adcdf0bbefefdc371ef2020408fb4c2bd02432b2d46317","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_surface04","59553e482f16e16b875149e976561a8083dc48f2d90dffa1b375c659c6f8b71b","ProjectedPrelit","surfacetex",1,2},
        {"gb_z13_surface04","59553e482f16e16b875149e976561a8083dc48f2d90dffa1b375c659c6f8b71b","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_surface05","c14611d86b2209e4e1508a452f6a9953c17c39fc0e49404ef4f90c00c6d4f133","ProjectedPrelit","surfacetex",1,2},
        {"gb_z13_surface05","c14611d86b2209e4e1508a452f6a9953c17c39fc0e49404ef4f90c00c6d4f133","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_surface06","e3fd1fd70fa297c60e276c142ed1f7574448c6d4910ce9dc603e6cb2e83345d7","ProjectedPrelit","surfacetex",1,2},
        {"gb_z13_surface06","e3fd1fd70fa297c60e276c142ed1f7574448c6d4910ce9dc603e6cb2e83345d7","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_a","835b69b8cc9fa3204ad942e230b676a4fa69184255b20dd51d614b73fc18989c","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_a","835b69b8cc9fa3204ad942e230b676a4fa69184255b20dd51d614b73fc18989c","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_b","687d7bbf4ee98bf642cf659d6dd11b09f760288fdf55dff94a50352423b3ef05","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_b","687d7bbf4ee98bf642cf659d6dd11b09f760288fdf55dff94a50352423b3ef05","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_c","c64e4a8bd6b8c4786ae8962e55e96568a65e21cd8591b0d959ff9061ed8797ae","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_c","c64e4a8bd6b8c4786ae8962e55e96568a65e21cd8591b0d959ff9061ed8797ae","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_d","c541114e44438f94a0f12cc8f2f679f69ae5d986969d64939a2d134231f8d522","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_d","c541114e44438f94a0f12cc8f2f679f69ae5d986969d64939a2d134231f8d522","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_e","79732f1051a024bd96c3fe61ee4c7f9542a6f047f5f5077671b674ac342ab34d","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_e","79732f1051a024bd96c3fe61ee4c7f9542a6f047f5f5077671b674ac342ab34d","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_f","6492d15d309edd199ae59f91cfc34ba5721c1f5a84b790c95047e3061b8371f0","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_f","6492d15d309edd199ae59f91cfc34ba5721c1f5a84b790c95047e3061b8371f0","ProjectedPrelit","groundtex",2,2},
        {"gb_z13_track_g","6c07f31ee96f08333ecd438db5e2711237cacfdf9d5d56038b914d7eb103091a","ProjectedPrelit","roadtex",1,2},
        {"gb_z13_track_g","6c07f31ee96f08333ecd438db5e2711237cacfdf9d5d56038b914d7eb103091a","ProjectedPrelit","groundtex",2,2},
        {"gb_z14_track_a","71bcc8d1b11741e63dc73583d36bae9daf7e0857a6ff3151c09b28629de9dcb5","ProjectedPrelit","roadtex",1,2},
        {"gb_z14_track_a","71bcc8d1b11741e63dc73583d36bae9daf7e0857a6ff3151c09b28629de9dcb5","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_surface01","b8113effa94efa6a3fbb037627790512b213ce207de5c23e601252b139615494","ProjectedPrelit","surfacetex",1,2},
        {"gb_z1_surface01","b8113effa94efa6a3fbb037627790512b213ce207de5c23e601252b139615494","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_surface02","a9b292415ea16f6ac17fdbc64c78aa92ea321536ef09b71bf02216ea99194691","ProjectedPrelit","surfacetex",1,2},
        {"gb_z1_surface02","a9b292415ea16f6ac17fdbc64c78aa92ea321536ef09b71bf02216ea99194691","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_track_a","c92ba60e7224c8c0ac269ef2e6163b25f63195426125780c2fec331187096905","ProjectedPrelit","roadtex",1,2},
        {"gb_z1_track_a","c92ba60e7224c8c0ac269ef2e6163b25f63195426125780c2fec331187096905","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_track_b","fac98e24be04fc7e0303b55b6b6f5b90aa702b1f201a27bc72cf474acaa35ac9","ProjectedPrelit","roadtex",1,2},
        {"gb_z1_track_b","fac98e24be04fc7e0303b55b6b6f5b90aa702b1f201a27bc72cf474acaa35ac9","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_track_c","b55d89d60b58d059031fef6331776aa778a2a6515bd3abd86b605227ee77b5ea","ProjectedPrelit","roadtex",1,2},
        {"gb_z1_track_c","b55d89d60b58d059031fef6331776aa778a2a6515bd3abd86b605227ee77b5ea","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_track_d","7cc5b03714c29e39ce46675a0ff484ff5eee648281e3e65a456bf78a16b37944","ProjectedPrelit","roadtex",1,2},
        {"gb_z1_track_d","7cc5b03714c29e39ce46675a0ff484ff5eee648281e3e65a456bf78a16b37944","ProjectedPrelit","groundtex",2,2},
        {"gb_z1_track_e","f46ad42c96efcddc498ed3844388bead3d6119ee41eb145eeb2605ffdb322828","ProjectedPrelit","roadtex",1,2},
        {"gb_z1_track_e","f46ad42c96efcddc498ed3844388bead3d6119ee41eb145eeb2605ffdb322828","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_surface01","d577420e26e672158e711bcdcd9feaecde517a9cc122bb332177a442ca506d34","ProjectedPrelit","surfacetex",1,2},
        {"gb_z2_surface01","d577420e26e672158e711bcdcd9feaecde517a9cc122bb332177a442ca506d34","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_surface02","b68901b05a53263cddcc0e7a56822463f92d705ed5117347a1e87491efcebd30","ProjectedPrelit","surfacetex",1,2},
        {"gb_z2_surface02","b68901b05a53263cddcc0e7a56822463f92d705ed5117347a1e87491efcebd30","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_surface03","05e7a4dc20d8ba5772bd0f4475ae9d5468bece5568faf35aa504418e60fa47d4","ProjectedPrelit","surfacetex",1,2},
        {"gb_z2_surface03","05e7a4dc20d8ba5772bd0f4475ae9d5468bece5568faf35aa504418e60fa47d4","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_a","eb467a613b45153619bcbea6cceaeb4545d5ed1c35aa4d42da178b1d88a1573f","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_a","eb467a613b45153619bcbea6cceaeb4545d5ed1c35aa4d42da178b1d88a1573f","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_b","039da6c1b5393638380bf074a5d886e1539ccabb8f157761d9021388ed68b8ff","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_b","039da6c1b5393638380bf074a5d886e1539ccabb8f157761d9021388ed68b8ff","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_c","6cc9af25f9b337a6ce5cd03b8a8c660a800e2c5c86641c6da50f1eeb5697eae4","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_c","6cc9af25f9b337a6ce5cd03b8a8c660a800e2c5c86641c6da50f1eeb5697eae4","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_d","443673c25908e860078bfce1107377159e1b920ec02b43910ed9f78007c11006","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_d","443673c25908e860078bfce1107377159e1b920ec02b43910ed9f78007c11006","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_e","93da00518a06520b4e6111ffa57a2551bc9df7af5edce3d8c0b2ad4094a8afaf","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_e","93da00518a06520b4e6111ffa57a2551bc9df7af5edce3d8c0b2ad4094a8afaf","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_f","94b5423fb6d3e2662ff15eb3fa9f6c99b2e58db0116cdeab94cdfcce264952cd","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_f","94b5423fb6d3e2662ff15eb3fa9f6c99b2e58db0116cdeab94cdfcce264952cd","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_g","60073321dabaa5fbcc9d3339a4ce68cf7bcb9ca68f6a338bbafe1f83bf125509","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_g","60073321dabaa5fbcc9d3339a4ce68cf7bcb9ca68f6a338bbafe1f83bf125509","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_h","e71d853c3a7da55ad8945a52a5a22c580ab11f44738e4e800b8f9235e9c5ced1","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_h","e71d853c3a7da55ad8945a52a5a22c580ab11f44738e4e800b8f9235e9c5ced1","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_i","4e2685f3741d71bcba35ee9cd0dd05332925ec007845656c85d996840ec45585","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_i","4e2685f3741d71bcba35ee9cd0dd05332925ec007845656c85d996840ec45585","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_j","333f192c3ec3deccc7a64a33f5e10fb59139004bfd09e364ca4d314b5467fa65","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_j","333f192c3ec3deccc7a64a33f5e10fb59139004bfd09e364ca4d314b5467fa65","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_k","b47c01a01d0b4fba38b4337bf5679d1088ff34014edd3e292cfabc196652795b","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_k","b47c01a01d0b4fba38b4337bf5679d1088ff34014edd3e292cfabc196652795b","ProjectedPrelit","groundtex",2,2},
        {"gb_z2_track_l","0c9c888071dca7ea2e5006f990cd5f4778737ac87b3b977b67bfc13cd0290da4","ProjectedPrelit","roadtex",1,2},
        {"gb_z2_track_l","0c9c888071dca7ea2e5006f990cd5f4778737ac87b3b977b67bfc13cd0290da4","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface01","1293d0347961bf105638dae00e6531f082989bc43e41381d94bfc8aaf0cc03ab","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface01","1293d0347961bf105638dae00e6531f082989bc43e41381d94bfc8aaf0cc03ab","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface02","76edf8e0bdd92ac7bdd2cd8407b4f62244a467f39928d46c4a7fb542e432543c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface02","76edf8e0bdd92ac7bdd2cd8407b4f62244a467f39928d46c4a7fb542e432543c","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface03","d8f401ad6796742429273fd29803edb815f4f227a0c0a2803530e3ea7d164aa4","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface03","d8f401ad6796742429273fd29803edb815f4f227a0c0a2803530e3ea7d164aa4","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface04","75417a667d193ae88f90591e73b820e63898787efd4ba1bbfae34b335c14373c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface04","75417a667d193ae88f90591e73b820e63898787efd4ba1bbfae34b335c14373c","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface05","1f84a2bc383312ce8edf213e13f30d07881063541b0d84b2b323770b7d605ae3","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface05","1f84a2bc383312ce8edf213e13f30d07881063541b0d84b2b323770b7d605ae3","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface06","b850d339ccb57d73fdc8f7d7935793bd9104db8d8f2d6a0d93e0aaeb2c6f33ff","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface06","b850d339ccb57d73fdc8f7d7935793bd9104db8d8f2d6a0d93e0aaeb2c6f33ff","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface07","76e64a8cb9590ff4feba302ca476fcf15d733f9f1a5a07c68a5014776f448ffd","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface07","76e64a8cb9590ff4feba302ca476fcf15d733f9f1a5a07c68a5014776f448ffd","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_surface08","989ab8a9dda08a2c2ed1a24aa26eb3d4f0e1ba42acb7fecde3683d3a93dd93b2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z3_surface08","989ab8a9dda08a2c2ed1a24aa26eb3d4f0e1ba42acb7fecde3683d3a93dd93b2","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_a","5d8160af92b32b6140622b1347556efb2171550ed448b4858a56a5d6f64b66e2","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_a","5d8160af92b32b6140622b1347556efb2171550ed448b4858a56a5d6f64b66e2","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_b","1fe11eef52ad604782a8c89fa2fccdfb69f0a42d42f658d0427b067bde20f65f","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_b","1fe11eef52ad604782a8c89fa2fccdfb69f0a42d42f658d0427b067bde20f65f","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_c","9bbc1757a51ee462503b6e82ae8e626cd43ea735220eb070e4deb36a4febc8fe","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_c","9bbc1757a51ee462503b6e82ae8e626cd43ea735220eb070e4deb36a4febc8fe","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_d","e7fe0674f7353d76aa121ab95f031b6a65bf117423c3cc3699a61c000ef50287","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_d","e7fe0674f7353d76aa121ab95f031b6a65bf117423c3cc3699a61c000ef50287","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_e","c74c8765c54650d9983ea5d23fa84f6024305dc28b1d7ab8b116f87b5ae063e2","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_e","c74c8765c54650d9983ea5d23fa84f6024305dc28b1d7ab8b116f87b5ae063e2","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_f","ff9a6b6fc4f94df19929aedee586ca1cea30c3fa2095724c02708b1018d7cbc0","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_f","ff9a6b6fc4f94df19929aedee586ca1cea30c3fa2095724c02708b1018d7cbc0","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_g","e3d76bc967c7c64d4754d57c799c3ce6e5bb9f3e0be8faaa4821e037c3735c14","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_g","e3d76bc967c7c64d4754d57c799c3ce6e5bb9f3e0be8faaa4821e037c3735c14","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_h","e96d960e91981efbd81f1f15d49f8d5754597df4ffd23ecc291d4c95bf73c571","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_h","e96d960e91981efbd81f1f15d49f8d5754597df4ffd23ecc291d4c95bf73c571","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_i","6cfa3fa81eb3fc8356eb97c264e1c80c33effd4a2d1d3ac49dfd5c10383f4655","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_i","6cfa3fa81eb3fc8356eb97c264e1c80c33effd4a2d1d3ac49dfd5c10383f4655","ProjectedPrelit","groundtex",2,2},
        {"gb_z3_track_j","1d551421caca0b18e1365b89eee1248bf23eb6950fdfb9df32b3efd11b0c1ca9","ProjectedPrelit","roadtex",1,2},
        {"gb_z3_track_j","1d551421caca0b18e1365b89eee1248bf23eb6950fdfb9df32b3efd11b0c1ca9","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface01","5ddaa231b8d92a25a5daae49070c66246f712a240cb1339d3703eb8a4dd5f6c3","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface01","5ddaa231b8d92a25a5daae49070c66246f712a240cb1339d3703eb8a4dd5f6c3","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface02","c903d09922226b6a4058abedb39d51414d256bffc790bfe2a5bfae0874db39ba","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface02","c903d09922226b6a4058abedb39d51414d256bffc790bfe2a5bfae0874db39ba","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface03","19e38ada429a424e45f5d76eba27173badb405eb981b9c348b48133f442db741","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface03","19e38ada429a424e45f5d76eba27173badb405eb981b9c348b48133f442db741","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface04","85ab20d850bd636985bd00a9a1997e554a4bf90b069cd06c28cdbe88fefdfa56","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface04","85ab20d850bd636985bd00a9a1997e554a4bf90b069cd06c28cdbe88fefdfa56","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface05","4e677fc3f3cd24cf187680fbc4fe562012fc9eb6e567cff5972ef1f68debc55c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface05","4e677fc3f3cd24cf187680fbc4fe562012fc9eb6e567cff5972ef1f68debc55c","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface06","682072e2fcb89ba7df1ed1dfa14159a2a3a251067a94e839fcd7bd9a4f481043","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface06","682072e2fcb89ba7df1ed1dfa14159a2a3a251067a94e839fcd7bd9a4f481043","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface07","77427e27292b7931f89ed6576eba6ed94908202ad075d120426d6cac0ad975ee","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface07","77427e27292b7931f89ed6576eba6ed94908202ad075d120426d6cac0ad975ee","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface08","e4227b76bb17e842a7ad63dae7ccd73c7719d1ff24e9b14ce974588c04030d41","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface08","e4227b76bb17e842a7ad63dae7ccd73c7719d1ff24e9b14ce974588c04030d41","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface09","4b9583c5a0a354a09519a70701a6f89e80eef624ac7a28b1e913240bc7dc23e0","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface09","4b9583c5a0a354a09519a70701a6f89e80eef624ac7a28b1e913240bc7dc23e0","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface10","473401a2515879001fe42ea4715f3b3dd6f0d26e22fd5dd4b0bc4b51b59f2cda","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface10","473401a2515879001fe42ea4715f3b3dd6f0d26e22fd5dd4b0bc4b51b59f2cda","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface11","dbc9338d2d307a1cc2e4eaf3b66b4d3decfda173649377eeffca2d3b9140c097","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface11","dbc9338d2d307a1cc2e4eaf3b66b4d3decfda173649377eeffca2d3b9140c097","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface12","2f91e72a4355e2fd8758c129490d72b7ad0df38b6e524417e5e472929358a23f","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface12","2f91e72a4355e2fd8758c129490d72b7ad0df38b6e524417e5e472929358a23f","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface13","b59c0a4f886e12e835f6d814cb0a2d574bc4d4804be75752ceb8c9821b55facb","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface13","b59c0a4f886e12e835f6d814cb0a2d574bc4d4804be75752ceb8c9821b55facb","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface14","1937a4cd95583a0546c8e7aaef94e413d536af9548658103f3d029bf8889a5b9","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface14","1937a4cd95583a0546c8e7aaef94e413d536af9548658103f3d029bf8889a5b9","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface15","1f853e24d91fc7e52ad491ab446af54345101242b16f269eeed6405a9828ae8c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface15","1f853e24d91fc7e52ad491ab446af54345101242b16f269eeed6405a9828ae8c","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface16","6ee82d910c1b7b28620d19ee6d08da60a6c58f0f6263905fa7b6d7deb33f6a30","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface16","6ee82d910c1b7b28620d19ee6d08da60a6c58f0f6263905fa7b6d7deb33f6a30","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface17","9b940bf59e978a21b553797fbfecc08e78fc6f7663f61b612978dd5423552228","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface17","9b940bf59e978a21b553797fbfecc08e78fc6f7663f61b612978dd5423552228","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface18","88955facc03b2df94be9108233b76c704aaafe25f0de34a26eafb9290e57eb96","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface18","88955facc03b2df94be9108233b76c704aaafe25f0de34a26eafb9290e57eb96","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface19","1194789c516af397eb59a5008e69fe535a8bae185a967bb599ac0c4e1cd8a03c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface19","1194789c516af397eb59a5008e69fe535a8bae185a967bb599ac0c4e1cd8a03c","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface20","62107400112457b3e98329d7872a1dc21a3f55c28fe945e6ecbf604ffdd072e3","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface20","62107400112457b3e98329d7872a1dc21a3f55c28fe945e6ecbf604ffdd072e3","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface21","c6e48863d16815fe5efe94840f608747dca42a75d2089e2cee79c566925ac220","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface21","c6e48863d16815fe5efe94840f608747dca42a75d2089e2cee79c566925ac220","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface22","f02737c6895fff4f69576014b5e48a83b758d8de5c76a6e9c4a46a5214b87dd6","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface22","f02737c6895fff4f69576014b5e48a83b758d8de5c76a6e9c4a46a5214b87dd6","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface23","a0b8c59d62132ebcb04ed734ff711a16acf2efa604188f3c123fd56e73aafdcc","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface23","a0b8c59d62132ebcb04ed734ff711a16acf2efa604188f3c123fd56e73aafdcc","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface24","99cacd61b0e03afaf474cd4f552b2b1791999186cb6ecc63288be2972f5e26d1","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface24","99cacd61b0e03afaf474cd4f552b2b1791999186cb6ecc63288be2972f5e26d1","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface25","51d6bd1ee342eac3c3913ec1dbdacb6e1e4c0876b6c802df8372337a4442558e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface25","51d6bd1ee342eac3c3913ec1dbdacb6e1e4c0876b6c802df8372337a4442558e","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_surface26","b937fff846ffbc1f36e5883f4773f6468a9f12298a6e2bc4c60c246929522ddf","ProjectedPrelit","surfacetex",1,2},
        {"gb_z4_surface26","b937fff846ffbc1f36e5883f4773f6468a9f12298a6e2bc4c60c246929522ddf","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_a","afd0f3d921be6b8920fe939585a64c84be320146aaadc9be22fb449c385c7372","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_a","afd0f3d921be6b8920fe939585a64c84be320146aaadc9be22fb449c385c7372","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_b","d8b6086ed16225e159b9780401200ff5c31f434cb8f2dea8558cdcc42e50fe32","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_b","d8b6086ed16225e159b9780401200ff5c31f434cb8f2dea8558cdcc42e50fe32","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_c","1eff00423bdf96c8ec1a1bc0b9aa475716d089228f218cef61c4a79ad87c8736","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_c","1eff00423bdf96c8ec1a1bc0b9aa475716d089228f218cef61c4a79ad87c8736","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_d","104750764fe0e8f8f8b79e12988c4a0156ce097b06cb060fe53b3ff408f026f5","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_d","104750764fe0e8f8f8b79e12988c4a0156ce097b06cb060fe53b3ff408f026f5","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_e","5e2a18d83b326904792f8d1cf75c48bd862b570f333e3967f0ea88ee6b6bf39d","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_e","5e2a18d83b326904792f8d1cf75c48bd862b570f333e3967f0ea88ee6b6bf39d","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_f","8886106cb5c5dcd0e66c585e86633a310c2c2021e81363f39a30c3d82107493f","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_f","8886106cb5c5dcd0e66c585e86633a310c2c2021e81363f39a30c3d82107493f","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_g","e42478c967872074a7bb73e4b55560d789531862fdd0238c5c437db4c9d0d7f6","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_g","e42478c967872074a7bb73e4b55560d789531862fdd0238c5c437db4c9d0d7f6","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_h","826b7bd77b9d367d6c5a573e3e104c1913ca29b0728496117d408df09c114a02","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_h","826b7bd77b9d367d6c5a573e3e104c1913ca29b0728496117d408df09c114a02","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_i","b5a55597936549124102907ee681a4d927c7f2b8a3a0583417471eb5443a4b33","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_i","b5a55597936549124102907ee681a4d927c7f2b8a3a0583417471eb5443a4b33","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_j","5070c8abd7749c5f5da9ae31bae634ec62bf6f5b7c6b1d0cecc791a87cf42de7","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_j","5070c8abd7749c5f5da9ae31bae634ec62bf6f5b7c6b1d0cecc791a87cf42de7","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_k","a6985f081db2d0c079afa88166576507842d1f787ad0f8c1aef60a1de9547d5f","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_k","a6985f081db2d0c079afa88166576507842d1f787ad0f8c1aef60a1de9547d5f","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_l","3fd26e6403cdedb5c1fe998fdfbc381069538215be56eb9217a58cd2988a336b","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_l","3fd26e6403cdedb5c1fe998fdfbc381069538215be56eb9217a58cd2988a336b","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_m","780bb05b377a2eb4b4fcfeaeeb4608fad3f0d728693117a188a715b2d119b5db","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_m","780bb05b377a2eb4b4fcfeaeeb4608fad3f0d728693117a188a715b2d119b5db","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_n","461f84c38c3f13dc61f18dc63c6c9de01cccb9d4dfd36bb1530a0eeb55e677a5","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_n","461f84c38c3f13dc61f18dc63c6c9de01cccb9d4dfd36bb1530a0eeb55e677a5","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_o","165e8f43fa32af0caa93e497a12e8d2cfde3c814268fd33c4a8bc0435bbdb6a3","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_o","165e8f43fa32af0caa93e497a12e8d2cfde3c814268fd33c4a8bc0435bbdb6a3","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_p","1be0805cedb3ae9ba89c22eef7de973daf722e980987e368d26049ef6263f9a0","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_p","1be0805cedb3ae9ba89c22eef7de973daf722e980987e368d26049ef6263f9a0","ProjectedPrelit","groundtex",2,2},
        {"gb_z4_track_q","342780557190078a695165805e0ed7e18fed0f1ee5e8e43c80598c39a7f590fa","ProjectedPrelit","roadtex",1,2},
        {"gb_z4_track_q","342780557190078a695165805e0ed7e18fed0f1ee5e8e43c80598c39a7f590fa","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface01","dabed8edd008049f3151d053f9316a9d1e906ba3e74d52fa53dd85eb2f5064c7","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface01","dabed8edd008049f3151d053f9316a9d1e906ba3e74d52fa53dd85eb2f5064c7","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface02","de6aa8e59eeaf642cd6677171ad53987962cbadbebc3b1e581051dfbe23c4179","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface02","de6aa8e59eeaf642cd6677171ad53987962cbadbebc3b1e581051dfbe23c4179","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface03","6fb425137791d31a38e09b76c852c99d21b3c97ef67e1e0c08073eb48ce17a11","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface03","6fb425137791d31a38e09b76c852c99d21b3c97ef67e1e0c08073eb48ce17a11","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface04","9e701269d49623fb3ba912ea6e82488b0e81fa9590e123ba3da47ccc6e8956d9","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface04","9e701269d49623fb3ba912ea6e82488b0e81fa9590e123ba3da47ccc6e8956d9","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface05","30becc37f38cdcf2b47771326d203b0e84f9d837b124712d34aa315390cac959","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface05","30becc37f38cdcf2b47771326d203b0e84f9d837b124712d34aa315390cac959","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface06","3675993b21f0eb76b62a5d2136157375c039db46959066bb9ceec4eec76f721c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface06","3675993b21f0eb76b62a5d2136157375c039db46959066bb9ceec4eec76f721c","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface07","a4700a10c0535ae9b448bd80ce551d0a75edf19761b0bfc8fe183895643ef0f4","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface07","a4700a10c0535ae9b448bd80ce551d0a75edf19761b0bfc8fe183895643ef0f4","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface08","1464942948caf8de5bc35cfb5a1235b0f46359ae4d824ec037b088e5d9c0b7a8","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface08","1464942948caf8de5bc35cfb5a1235b0f46359ae4d824ec037b088e5d9c0b7a8","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface09","3fabebd75004aedf5dc24b74ebd0596aea60cfd24309130b35be935e1f077849","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface09","3fabebd75004aedf5dc24b74ebd0596aea60cfd24309130b35be935e1f077849","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface10","6367556e2e9a3ddc8f8a230a3afceb14adc9a1e647e594845aedba5aec675c1e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface10","6367556e2e9a3ddc8f8a230a3afceb14adc9a1e647e594845aedba5aec675c1e","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface11","279482deb87879d37cff4cdc97d3d7d4899e4ae38d71fd555c250b1e11d0bdd7","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface11","279482deb87879d37cff4cdc97d3d7d4899e4ae38d71fd555c250b1e11d0bdd7","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_surface12","dac0b8820e8337d9656182e067f38e22e4424b4f0dbbe5434fb47434a90f1350","ProjectedPrelit","surfacetex",1,2},
        {"gb_z5_surface12","dac0b8820e8337d9656182e067f38e22e4424b4f0dbbe5434fb47434a90f1350","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_a","663ced8b1d541ab2faa083cf535ec1474c3464637e66545680ae9eff7cafe32e","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_a","663ced8b1d541ab2faa083cf535ec1474c3464637e66545680ae9eff7cafe32e","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_b","41bd1159323a27a29c55f509a547828cc9d3c4460016e3618ca14b50ffc13369","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_b","41bd1159323a27a29c55f509a547828cc9d3c4460016e3618ca14b50ffc13369","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_c","09ff6e9901e925c057b107d775bb2fb654bffd23049aa3899d89c5db3c1e41c2","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_c","09ff6e9901e925c057b107d775bb2fb654bffd23049aa3899d89c5db3c1e41c2","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_d","88092d6fe839e96934e26e31601bb81beb80bb970607cb38cc9150350648a7db","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_d","88092d6fe839e96934e26e31601bb81beb80bb970607cb38cc9150350648a7db","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_e","53f563701a327e4f1156b5de1971c0625c2ac445c98d69c2fd18af8ff42b1b1f","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_e","53f563701a327e4f1156b5de1971c0625c2ac445c98d69c2fd18af8ff42b1b1f","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_f","f735f485775420c069b6356572140ad2c8051371f0392dd04595fffe38ce7d34","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_f","f735f485775420c069b6356572140ad2c8051371f0392dd04595fffe38ce7d34","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_g","2a434bdb032b1ed0d643bb741ef51603d2e7a8ceace5aeb5f4650c129c3e94a5","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_g","2a434bdb032b1ed0d643bb741ef51603d2e7a8ceace5aeb5f4650c129c3e94a5","ProjectedPrelit","groundtex",2,2},
        {"gb_z5_track_h","8649d4002c7b2e89ee0a2dba04708e8c3e029e64e0245734918e5cb6c2de33af","ProjectedPrelit","roadtex",1,2},
        {"gb_z5_track_h","8649d4002c7b2e89ee0a2dba04708e8c3e029e64e0245734918e5cb6c2de33af","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface01","dc198b49c09eb5b59fc1a1f79c6d06ac9947a057efc52216775c02634430b6f2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface01","dc198b49c09eb5b59fc1a1f79c6d06ac9947a057efc52216775c02634430b6f2","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface02","427f97535de98e50f47aa55bda0f5a2d65fb3853a1a6b5ac2f1a6b35b340a8c2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface02","427f97535de98e50f47aa55bda0f5a2d65fb3853a1a6b5ac2f1a6b35b340a8c2","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface03","f3ad6413a898340eea604b6c6f2234824f733eabb849d235ec3acc7101dcf42b","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface03","f3ad6413a898340eea604b6c6f2234824f733eabb849d235ec3acc7101dcf42b","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface04","a9a9f1c4df1e02b845bfacd45d40bac23d576388d00a27824c9d6d4ba2f97d9a","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface04","a9a9f1c4df1e02b845bfacd45d40bac23d576388d00a27824c9d6d4ba2f97d9a","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface05","3b54508abe2193aab8d5d60fe1a13fc69fec3bea2c00942367f495ebbb237361","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface05","3b54508abe2193aab8d5d60fe1a13fc69fec3bea2c00942367f495ebbb237361","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface06","a9148f51aa9dfe09e10587d03b308c4ea252faa151368fe923c57bed186d8cfb","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface06","a9148f51aa9dfe09e10587d03b308c4ea252faa151368fe923c57bed186d8cfb","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface07","bc4643d3ba330cb4b7be5c9aabbdba8f040a0438c2a2b01aafc8ab3dc9148ad4","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface07","bc4643d3ba330cb4b7be5c9aabbdba8f040a0438c2a2b01aafc8ab3dc9148ad4","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface08","8766abe59b5e34ad7386ba3ce44b976db18792e4ad184a3cc895dbb5dfa6d5fb","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface08","8766abe59b5e34ad7386ba3ce44b976db18792e4ad184a3cc895dbb5dfa6d5fb","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface09","ce062548a6979305debb340a2e90f85f31b03facf214500746143ae64055191a","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface09","ce062548a6979305debb340a2e90f85f31b03facf214500746143ae64055191a","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface10","6b1d1113659c40379a1e7a475ab4d9469ee34df2d0a651e43c48b17b251d2f82","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface10","6b1d1113659c40379a1e7a475ab4d9469ee34df2d0a651e43c48b17b251d2f82","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface11","dcf1c30da3c328b20d92764fdb0534d89c1d4c3157873761006a007080b0733c","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface11","dcf1c30da3c328b20d92764fdb0534d89c1d4c3157873761006a007080b0733c","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface12","abc6f19afad87418492a71cb34f1814ff498bfb902c3d6a223e1a707b24dcf8e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface12","abc6f19afad87418492a71cb34f1814ff498bfb902c3d6a223e1a707b24dcf8e","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface13","57c0a19615ef9193c273233b04f4e1818739cbe693fd1919c51e9260c5bb5164","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface13","57c0a19615ef9193c273233b04f4e1818739cbe693fd1919c51e9260c5bb5164","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_surface14","3654d7a1a5070f652151baed9b15c91653b775e189642ad8f939cf2ddeeba66e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z6_surface14","3654d7a1a5070f652151baed9b15c91653b775e189642ad8f939cf2ddeeba66e","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_track_a","68678af882b7334d233bc30ce9af75825edfb2380323a83cbf18082cd5505bae","ProjectedPrelit","roadtex",1,2},
        {"gb_z6_track_a","68678af882b7334d233bc30ce9af75825edfb2380323a83cbf18082cd5505bae","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_track_b","e4b7705f1951f5df0717d3653c07c7dd80e18977c9e0de8b3bbefee7b8619b78","ProjectedPrelit","roadtex",1,2},
        {"gb_z6_track_b","e4b7705f1951f5df0717d3653c07c7dd80e18977c9e0de8b3bbefee7b8619b78","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_track_c","06b012c335c79c7e50466e631740728041f5fde0fdb0599e22bd6d5b3a179832","ProjectedPrelit","roadtex",1,2},
        {"gb_z6_track_c","06b012c335c79c7e50466e631740728041f5fde0fdb0599e22bd6d5b3a179832","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_track_d","a2c506d91e19625ce979684b599e508b8f7bd939fe546beaeac9d4039131cf04","ProjectedPrelit","roadtex",1,2},
        {"gb_z6_track_d","a2c506d91e19625ce979684b599e508b8f7bd939fe546beaeac9d4039131cf04","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_track_e","8be805b2c0630bf6ce659b4e8d8e6824dc40d3308f14c61b3608f2f61c301e87","ProjectedPrelit","roadtex",1,2},
        {"gb_z6_track_e","8be805b2c0630bf6ce659b4e8d8e6824dc40d3308f14c61b3608f2f61c301e87","ProjectedPrelit","groundtex",2,2},
        {"gb_z6_track_f","040a1f6842112dd0b6e9c0b0577f373fce9e0239192216dc34285b6e95753e1f","ProjectedPrelit","roadtex",1,2},
        {"gb_z6_track_f","040a1f6842112dd0b6e9c0b0577f373fce9e0239192216dc34285b6e95753e1f","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface01","675f1b369dd3619bbca53ac76af7c0eab9cc15e0b37284c2d8895755589a9c8e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface01","675f1b369dd3619bbca53ac76af7c0eab9cc15e0b37284c2d8895755589a9c8e","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface02","21e822ff4ac1b1bde6231e72aec936df88b5d0bde87c0710c489352bdfcea79d","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface02","21e822ff4ac1b1bde6231e72aec936df88b5d0bde87c0710c489352bdfcea79d","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface03","9b339023d528670ef50018e19fb16e773b183ad0296e7af8be97143768c5e0b8","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface03","9b339023d528670ef50018e19fb16e773b183ad0296e7af8be97143768c5e0b8","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface04","12718f7a2ce5d1e903421ab4ee9e17f51337e88fecd3e7f92a7609b9c7ddd8d6","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface04","12718f7a2ce5d1e903421ab4ee9e17f51337e88fecd3e7f92a7609b9c7ddd8d6","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface05","9689503effea9cca143e4f23b82511707bc784a206e07ddb420164c9c9443eb2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface05","9689503effea9cca143e4f23b82511707bc784a206e07ddb420164c9c9443eb2","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface06","7b566cfabc2ef86d4b67a287664b09d4974c528cc9cffd82f23701e88214f05e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface06","7b566cfabc2ef86d4b67a287664b09d4974c528cc9cffd82f23701e88214f05e","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface07","6210efbf2ee66a2f44c0d482ddad54a0e71b841fc9721736707f83d69dfeadee","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface07","6210efbf2ee66a2f44c0d482ddad54a0e71b841fc9721736707f83d69dfeadee","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface08","11604a343e454dce016d74bdfee65bedb7868244981f517954bd8373bec24965","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface08","11604a343e454dce016d74bdfee65bedb7868244981f517954bd8373bec24965","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface09","d2b5ca21758b15f169633789e65990ac5ce6e672d5280a47add95cb1f3e2ebd1","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface09","d2b5ca21758b15f169633789e65990ac5ce6e672d5280a47add95cb1f3e2ebd1","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface10","4fe84bbfe38b33da519e86c73ed39e226790f14d0c0a2a26744fce669a944c88","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface10","4fe84bbfe38b33da519e86c73ed39e226790f14d0c0a2a26744fce669a944c88","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface11","3050e2a87a0bf9496d2f41435e0ad8024a6c9d57a3d892aeba47470715a479d7","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface11","3050e2a87a0bf9496d2f41435e0ad8024a6c9d57a3d892aeba47470715a479d7","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_surface12","9565703dc80b064e7c093e0fa12018d64e80f3fbb21d0d4bf8fff5d9aa448463","ProjectedPrelit","surfacetex",1,2},
        {"gb_z7_surface12","9565703dc80b064e7c093e0fa12018d64e80f3fbb21d0d4bf8fff5d9aa448463","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_track_a","d14eef8574f827d48a36c35c7b7b89ea94d4c9d9e0eac3f8730a6e559942b4cc","ProjectedPrelit","roadtex",1,2},
        {"gb_z7_track_a","d14eef8574f827d48a36c35c7b7b89ea94d4c9d9e0eac3f8730a6e559942b4cc","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_track_b","38fcdbe2ddc68979a77c21df4b4bae7340a3cdce97609f1310466bdb5ac27408","ProjectedPrelit","roadtex",1,2},
        {"gb_z7_track_b","38fcdbe2ddc68979a77c21df4b4bae7340a3cdce97609f1310466bdb5ac27408","ProjectedPrelit","groundtex",2,2},
        {"gb_z7_track_c","9f566717a633156fc5182f2b17944bd7deed3271edc109d2692c0ef9ccf0ab85","ProjectedPrelit","roadtex",1,2},
        {"gb_z7_track_c","9f566717a633156fc5182f2b17944bd7deed3271edc109d2692c0ef9ccf0ab85","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface01","9a9339a9f4508bbdb59ab81c5860b3d5429544c910ddf48d8aee9e287372b4dc","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface01","9a9339a9f4508bbdb59ab81c5860b3d5429544c910ddf48d8aee9e287372b4dc","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface02","0dd7e4ea94bc25e12dcc92c26ba6392b73e5a04ba3f112c56495e77588927933","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface02","0dd7e4ea94bc25e12dcc92c26ba6392b73e5a04ba3f112c56495e77588927933","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface03","f5277e5a4bf9d3ca3940069f546e580ed3b07651bbea7e25d6618fac3ae9e72a","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface03","f5277e5a4bf9d3ca3940069f546e580ed3b07651bbea7e25d6618fac3ae9e72a","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface04","f5aa161b34dad83c8b2440de5da1df22f7e718e698c6c38ddd4863c211bd914b","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface04","f5aa161b34dad83c8b2440de5da1df22f7e718e698c6c38ddd4863c211bd914b","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface05","02725fd751ae2f2ed9eef137e441d59f737fd1a6f1b2af4db803093a7ca4cc8d","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface05","02725fd751ae2f2ed9eef137e441d59f737fd1a6f1b2af4db803093a7ca4cc8d","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface06","cbfa37c2e2bd9c9611c14acd2084986d2c124187e79bfd76d3df309a873586d9","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface06","cbfa37c2e2bd9c9611c14acd2084986d2c124187e79bfd76d3df309a873586d9","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface07","a300b928d5c5fdb5ffa885dca7825aa9be79692744690665bf4841770da2a055","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface07","a300b928d5c5fdb5ffa885dca7825aa9be79692744690665bf4841770da2a055","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface08","5807616a943cd5a5e5a093e831be796e5979fb218ce969be51eba3f9d56c23a4","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface08","5807616a943cd5a5e5a093e831be796e5979fb218ce969be51eba3f9d56c23a4","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_surface09","73e3c752e68d21e08b785f3a29882dde09721ece6cc170594765f073a88cefe5","ProjectedPrelit","surfacetex",1,2},
        {"gb_z8_surface09","73e3c752e68d21e08b785f3a29882dde09721ece6cc170594765f073a88cefe5","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_track_a","11a71b65bdf334e7efe6571841aa588e24712f6a11e6cf6fc89f83a15e644039","ProjectedPrelit","roadtex",1,2},
        {"gb_z8_track_a","11a71b65bdf334e7efe6571841aa588e24712f6a11e6cf6fc89f83a15e644039","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_track_b","8b816a3da4f2562e6f47272635760f01a382f4e5a5e9882289d1feb196ddf392","ProjectedPrelit","roadtex",1,2},
        {"gb_z8_track_b","8b816a3da4f2562e6f47272635760f01a382f4e5a5e9882289d1feb196ddf392","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_track_c","83d093c1e1358e2f041dd920e60943d057567e952cf69ab8b08dcf24ead0f98e","ProjectedPrelit","roadtex",1,2},
        {"gb_z8_track_c","83d093c1e1358e2f041dd920e60943d057567e952cf69ab8b08dcf24ead0f98e","ProjectedPrelit","groundtex",2,2},
        {"gb_z8_track_d","211c050a12b716e4eecc341e65edab4addab96054ee61a631419e42cfa77e66e","ProjectedPrelit","roadtex",1,2},
        {"gb_z8_track_d","211c050a12b716e4eecc341e65edab4addab96054ee61a631419e42cfa77e66e","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface01","e4f6333d6c52d232597bde28a28bf9bcffd1f2b94bbbc4699165257a534a1ff0","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface01","e4f6333d6c52d232597bde28a28bf9bcffd1f2b94bbbc4699165257a534a1ff0","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface02","8f8dc47b0ca7510a44c167c5d3260b5de38229add12816ebed83d90c7aaa4f0e","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface02","8f8dc47b0ca7510a44c167c5d3260b5de38229add12816ebed83d90c7aaa4f0e","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface03","0bd2608015fa50083c7da70aa6e8fc9cae53b84930a1eae605107f9f41114a5d","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface03","0bd2608015fa50083c7da70aa6e8fc9cae53b84930a1eae605107f9f41114a5d","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface04","97f8e41759f9263f5abcab39643125046c6c2fa242f962844a22ec9f7f144401","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface04","97f8e41759f9263f5abcab39643125046c6c2fa242f962844a22ec9f7f144401","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface05","ca762bff4b123cadba3265802cc12326a883e048a77dc710c69604d6d45a7395","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface05","ca762bff4b123cadba3265802cc12326a883e048a77dc710c69604d6d45a7395","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface06","1b4b7bf15c9d587642aaa3ddb4b507b373c9bfced1f2c88110fdc97007c5dd2d","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface06","1b4b7bf15c9d587642aaa3ddb4b507b373c9bfced1f2c88110fdc97007c5dd2d","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface07","a11b8a5a19ec7a2a7ceb125fd7cf4b0217f5cbdc2b12adbb11a80b4105304c22","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface07","a11b8a5a19ec7a2a7ceb125fd7cf4b0217f5cbdc2b12adbb11a80b4105304c22","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface08","3e2afe1b387b7e745a57a74af7f1b6c98c90084a69f30274234aee67cf7895a4","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface08","3e2afe1b387b7e745a57a74af7f1b6c98c90084a69f30274234aee67cf7895a4","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_surface09","b1bc7a1f61492da7aba0837bdcc34a84a6ab87eb20c46bccc7fe1651bc1359b2","ProjectedPrelit","surfacetex",1,2},
        {"gb_z9_surface09","b1bc7a1f61492da7aba0837bdcc34a84a6ab87eb20c46bccc7fe1651bc1359b2","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_track_a","83cdfca9c67f4f4a41f34523f211e62ad3aefee48dd390826882123899bca402","ProjectedPrelit","roadtex",1,2},
        {"gb_z9_track_a","83cdfca9c67f4f4a41f34523f211e62ad3aefee48dd390826882123899bca402","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_track_b","f5e4ac6010f7e1cbae2142fad482107f2136cd48e2d3d8d131d2eac356a2a563","ProjectedPrelit","roadtex",1,2},
        {"gb_z9_track_b","f5e4ac6010f7e1cbae2142fad482107f2136cd48e2d3d8d131d2eac356a2a563","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_track_c","4e3f212337b4e7f1d73142cdfd0253e4ff152f53d47aa4f06f6a13d620d2687e","ProjectedPrelit","roadtex",1,2},
        {"gb_z9_track_c","4e3f212337b4e7f1d73142cdfd0253e4ff152f53d47aa4f06f6a13d620d2687e","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_track_d","76039a52cc688ecbfcd447c18f4838867a2c18fa2d898abb1ae671fa3f4b1bea","ProjectedPrelit","roadtex",1,2},
        {"gb_z9_track_d","76039a52cc688ecbfcd447c18f4838867a2c18fa2d898abb1ae671fa3f4b1bea","ProjectedPrelit","groundtex",2,2},
        {"gb_z9_track_e","134e82f8b787f77a4198d231def981a5cba5574d785c6cb3610730da99361060","ProjectedPrelit","roadtex",1,2},
        {"gb_z9_track_e","134e82f8b787f77a4198d231def981a5cba5574d785c6cb3610730da99361060","ProjectedPrelit","groundtex",2,2},
        {"gun_a","b9dd6c83f1035f298a914f64dd65434e74571e541c41be956b9594dbab4dfc89","PreLitCompressed","gun_a",1,1},
        {"gun_b","9f2803f28ca7477b83cdbb194d7d87ae5cbabb4f2f58ad2c2cfb29a3ab76aef9","PreLitCompressed","gun_b",1,1},
        {"gun_c","866f7657f9b2523e718121adcd0a67a9c2447dca9ca3792efec410b5fd066a7f","PreLitCompressed","gun_c",1,1},
        {"gun_d","52580086c467b9180d0d0627464cbc4e0735c75793481571b681359211fb37a8","PreLitCompressed","gun_d",1,1},
        {"hanging_leaf","75eca0d81eb2ee2290eea5f3684b583fce687fe86a536817f31322b3929a91c9","PreLitCompressed","hanging_leaf",1,1},
        {"house_stand_a","20b27cbb3dc795da5127600ff732ceeb884fe854280fe310c484f8f7d96fa418","PreLitCompressed","house_stand_a",1,1},
        {"house_stand_b","422d4031b74b009655bfab45a462923456d1ed72e978215ec7633b88143b51da","PreLitCompressed","house_stand_b",1,1},
        {"house_stand_c","0e0e350a9a5195ac8bd0c6aabfdc2b122c9552cdef1db3c48b43b40454f3dc9b","PreLitCompressed","house_stand_c",1,1},
        {"house_stand_d","7f2def2feb31d1c18c1a8458dd6a675b70596b9bc6d94419d5fd2fafa739abd1","PreLitCompressed","house_stand_d",1,1},
        {"house_stand_e","e035716eb4a51592575098ffa149c494a697620fa8e944bf68663c209c21bfe0","PreLitCompressed","house_stand_e",1,1},
        {"jag_rock","a125ba57f680e209204e273bb24729f98d9ce396b7b2d80635bca44de9e8d0d7","PreLitCompressed","jag_rock",1,1},
        {"market_hut","03650d6f6c8b1494883fac97763a7265936d3a1b6804000afc7477045886f180","PreLitCompressed","market_hut",1,1},
        {"market_hut01","3272ef7fa28457db76f34e1c4f25eddb809931e188150d7d26fcababd07a4d3b","PreLitCompressed","market_hut01",1,1},
        {"market_hut02","8eaaf89af327ccefba4d9d24a91666fd001994f391e851a6b018130d113ec310","PreLitCompressed","market_hut02",1,1},
        {"market_hut03","e14449b11480fd7b31d88c7c33dd75696a45288c931c9f8531d710ee2e0c6cae","PreLitCompressed","market_hut03",1,1},
        {"market_hut04","494da4546ae444040813a3f6d1eaf14fc34f5a6f60dbba9f0ce74364a2577b44","PreLitCompressed","market_hut04",1,1},
        {"market_hut05","254f344ae28bb4dd1470eae0c4dc754fb2e8712c580bd013600181aaca8002f0","PreLitCompressed","market_hut05",1,1},
        {"market_hut06","9e0a5590c7227f666d820d0fb1db29070b28f9faa9a43875c219c89f6372c6ff","PreLitCompressed","market_hut06",1,1},
        {"market_hut_a","acb0170276b0bfc6fd2359af0e0ee6ae0fd06b40d84e1640b9bfd60051bd67b1","PreLitCompressed","market_hut_a",1,1},
        {"market_hut_door","4c5b37022c331a958b2f3f2b392ea738acd6eae86ed915de7a5ca6a3ed7d6043","PreLitCompressed","market_hut_door",1,1},
        {"market_hut_roof","cd7e20de2673102bd1080ad96ad608fc085121460d773ef47a8b4461fec74fa8","PreLitCompressed","market_hut_roof",1,1},
        {"market_table","68662f116ba7b90c74f756ed62e2b3ad3f0f0d75d2b645624af3f5d8d52b4928","PreLitCompressed","market_table",1,1},
        {"metal_hut_sep","ce2f97ecf65745c54d030b7211079bb8452b1ac0bcb2635c4f1ade2c80fc2aed","PreLitCompressed","metal_hut_sep",1,1},
        {"metal_hut_sep01","230aa35419e3e93979a5aa83703bc113184f0d8ac0be20bbc683e0cd8588001d","PreLitCompressed","metal_hut_sep01",1,1},
        {"metal_hut_sep02","ff2142b528957ab18f2f7ae5f7a24fdd0b60c5f0e17b07f8323b75558cfabe37","PreLitCompressed","metal_hut_sep02",1,1},
        {"metal_hut_sep03","27394db54cea6de50a41738b43fcf8f1672ff4a86e092432abe6ac70df2520e9","PreLitCompressed","metal_hut_sep03",1,1},
        {"pig_head","4bb462c93a872bcbaf7ede31a60bdcabb08aaa8331f2b4a04d595d14d15a7d0f","PreLitCompressed","pig_head",1,1},
        {"planks_wood_a","adce161ea2e5c789bec59c7709edc0ad44eb6bf823257b94573c97cb9502b9e5","PreLitCompressed","planks_wood_a",1,1},
        {"refianim","e49ce801acd0c540b8fc47b817e315db95d562089299411bf8eb5a6978db90a4","DiffuseCompressed","refianim",1,1},
        {"s_flaga","e9e9c30e4f393ed53f19341be95284e344b55aa48a9c79f7aa7450cc9ab77c5b","PreLitCompressed","s_flaga",1,1},
        {"s_flagb","3065bdb1c4ba2a299f9343d9b9d1d67c17fdf7ec094d3be08b8fd5a6407b9bbd","PreLitCompressed","s_flagb",1,1},
        {"s_flagc","85d2528875d0f2c6a03c6806c626b6e7448d120a45691514c25882e840feb372","PreLitCompressed","s_flagc",1,1},
        {"shade","41b7077920f9da942e40a6b400ef6865189ae64aa7b37267607c09fa81e486a6","PreLitCompressed","shade",1,1},
        {"ship_a","a1927ea1f17e0838ea510e1d40fcc1e1349d147dca97106632ae7b8501e252e6","PreLitCompressed","ship_a",1,1},
        {"ship_b","3ccd20e659573e66a27bce42af441bf3d86a7c5dc5be5127317d9b54d4b587f6","PreLitCompressed","ship_b",1,1},
        {"ship_c","963d83620e44d9e638c2b47446eca74e09cc051708b964b74503e9f20cf18b24","PreLitCompressed","ship_c",1,1},
        {"ship_d","1b9c240676e61c5ee1d46f5bbe6f412fd3349db480e99c1927a07dd2a73b5ae8","PreLitCompressed","ship_d",1,1},
        {"ship_e","ad7bed57f530807cda718645d396a975b0a4f667bdc4b7e9b6cbde659691727e","PreLitCompressed","ship_e",1,1},
        {"ship_f","42487eecb680494285ea2dfaccfd2f01e05f06054ae47edf86a44fea203a6432","PreLitCompressed","ship_f",1,1},
        {"ship_g","9e9add4b40b99d8c314f1fdedb81a3badabd91cdc4adb9f6fafbcdb38981296f","PreLitCompressed","ship_g",1,1},
        {"ship_outside_a","4229259f936249680c483bf397a19cec05793537f38024b6cd4cec1ea26a32d9","PreLitCompressed","ship_outside_a",1,1},
        {"test_a","bd453f3f2929fa6092e1ad5a6f77b56ecc73b165b6cacd00788daa75c97cd11f","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_a","bd453f3f2929fa6092e1ad5a6f77b56ecc73b165b6cacd00788daa75c97cd11f","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_b","6b66349f84818dca7f3f9cc4aa64d84db679ea259c9da12868b516ef9c2a0955","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_b","6b66349f84818dca7f3f9cc4aa64d84db679ea259c9da12868b516ef9c2a0955","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_c","6dde3979475febec1b653b4c583a0f6ba3d6f4fe4b29801ab1b547bc34dda93f","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_c","6dde3979475febec1b653b4c583a0f6ba3d6f4fe4b29801ab1b547bc34dda93f","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_d","3b8d9f6282b14824b7c1e3a2405145ae26e02b66b339a5be0e42e6edc9812923","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_d","3b8d9f6282b14824b7c1e3a2405145ae26e02b66b339a5be0e42e6edc9812923","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_e","bb4da4a645e71d90dd2b4d2e9c465fdd1cac6b3d9261f3826b39259e67c9b3aa","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_e","bb4da4a645e71d90dd2b4d2e9c465fdd1cac6b3d9261f3826b39259e67c9b3aa","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_f","d225e18e6eca880b54b409e3155605cefafc53b5d4d0221bbf6a1d6c6db038f6","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_f","d225e18e6eca880b54b409e3155605cefafc53b5d4d0221bbf6a1d6c6db038f6","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_g","611dd5cf914f822c17486a017e650e718af79974f4ab6f23e63855bff27f661d","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_g","611dd5cf914f822c17486a017e650e718af79974f4ab6f23e63855bff27f661d","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_i","9f6a0927e576b409be948dc30e9c7bb5c2f15330ec3c6b1b62527440757097d6","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_i","9f6a0927e576b409be948dc30e9c7bb5c2f15330ec3c6b1b62527440757097d6","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_l","04fed8d18742d3b1e322ddc606a778fad47c7b4a546c60c2d0060efcbab2ca13","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_l","04fed8d18742d3b1e322ddc606a778fad47c7b4a546c60c2d0060efcbab2ca13","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_m","e3782cee4bcfc6ab7dcaece4c6d39a8f06310b6eabe5a0b841f27374441bef66","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_m","e3782cee4bcfc6ab7dcaece4c6d39a8f06310b6eabe5a0b841f27374441bef66","ProjectedPrelit","forttress_tex_a",2,2},
        {"test_n","5d7e52629973fc6ed2722262c6c0057556f068675f1020161f771005b4fa86ae","ProjectedPrelit","forttress_tex_b",1,2},
        {"test_n","5d7e52629973fc6ed2722262c6c0057556f068675f1020161f771005b4fa86ae","ProjectedPrelit","forttress_tex_a",2,2},
        {"tree_branch_a","b28e2304d293a35f356c278051e026c442f71579abf938176dea38c7fa03fcdb","PreLitCompressed","tree_branch_a",1,1},
        {"worp_a","6bd4dafe59d174d9d8f62626bfb062eb6370a51d40b9aed302c7aec25ff5ff69","DiffuseCompressed","worp_a",1,1},
        {"worp_b","68ae5ff162320a19a624ad86caa529f76c35b2fe7a616858c137eb2dba37e234","DiffuseCompressed","worp_b",1,1},
        {"worp_c","322071f046459f4878bda2013739e668baaa4c5de64e03f6a0999ca583bd033e","DiffuseCompressed","worp_c",1,1},
        {"worp_d","5ce9053d2a1b7c650e523105b7e7bd0d2dea74c9f95551c9548c46878b4a5db5","DiffuseCompressed","worp_d",1,1},
        {"worp_e","fafceb0e801f5c13b56ef1019576f7cafd7e1444745940ed43da322e1819f382","DiffuseCompressed","worp_e",1,1},
        {"worp_f","4c844c1efb3c1928272b03e08c8921f45f312a2e5c265b63fb5b14e3b9870b53","DiffuseCompressed","worp_f",1,1},
        {"worp_g","6c4abf3d1b830727c645da9471bf8d5fced87553ca72fd275d4efca70d77886e","DiffuseCompressed","worp_g",1,1},
        {"worp_h","21f9fd21bfb5b7f21b4b653d2e00c93ec49b8e9fb343560f2c288a3627e9529a","DiffuseCompressed","worp_h",1,1},
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
    if(selection.projectedPrelit){
        if(selection.program!="ProjectedPrelit"||selection.passCount!=2||get(Field::alphaTestOnOff)!=0||
            get(Field::alphaBlendingOnOff)!=(selection.pass==1?0:1))return false;
    }else if(selection.passCount!=1||selection.pass!=1||
        (program!="prelitcompressed"&&program!="diffusecompressed"))return false;
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
