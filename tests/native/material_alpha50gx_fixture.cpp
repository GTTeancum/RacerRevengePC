// ONE world material rule for every course (PLAN B-C15). Out-of-line native
// implementation of material_alpha50gx_fixture.h.
//
// Original: 274160 reads each placed model's materials; 14D370/29E340 load
// them through 2476A0, which copies the APPSTART DefaultMT GS fields and then
// applies the pass's own nonempty values (247A24..247B10 ALPHA_1, 247C88 the
// pass-local environment context). Nothing on that chain compares a course,
// a material name or a texture name. The per-course selectors that used to
// live here (select/selectGc/selectGa/selectMb/selectTc/selectRc/selectSc/
// selectTb/selectGb) were port stand-ins with the same core; they are retired
// into the one rule below, which keeps every form any of them admitted.
#include "material_alpha50gx_fixture.h"
#include <array>
#include <functional>
#include <optional>

namespace material_alpha50gx_fixture {

namespace {
using Field = rr::assets::OriginalUiGsField50KG;
using Gs = std::array<std::int32_t, rr::assets::originalUiGsFieldCount50KG>;
// One authored-field lookup: returns false on a malformed or duplicated field,
// leaves `result` unchanged when the pass does not author the field.
using AuthoredInteger = std::function<bool(const char*, std::int32_t&)>;
using AuthoredPresent = std::function<bool(const char*)>;

// 2476A0: inherit the DefaultMT GS fields, then apply the pass's own values.
bool inheritDefaultMt(const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, const AuthoredInteger& authored, Gs& gs) {
    if (!runtime.overrideKnown || runtime.override435670) return false;
    const auto* inherited = templates.get(rr::assets::OriginalUiTemplate50KG::defaultMt);
    // The APPSTART DefaultMT asset identity (one application asset, not a course).
    if (!inherited || !inherited->identity || !inherited->identity->sha256 ||
        std::string(inherited->identity->sha256) !=
            "1907587ff3a8153a9f687dc43e6858ad0dff6ac3827db74d8518acad5644b4f7") return false;
    for (std::size_t i = 0; i < gs.size(); ++i) {
        const auto field = static_cast<Field>(i);
        gs[i] = inherited->gs[field].value;
        if (field == Field::environmentContext0or1) gs[i] = 0; // 247C88 pass-local default.
        if (field == Field::inputColorD && !authored("inputAlphaD", gs[i])) return false;
        if (!authored(rr::assets::originalUiGsFieldName50KG(field), gs[i])) return false;
    }
    return true;
}

struct Pass {
    std::string program;             // lower-case
    std::uint32_t pass = 1, passCount = 1;
    bool projectedPrelit = false;    // ProjectedPrelit two-pass form
    bool textureResolved = true;
    bool staticAnimationFrame = false;
    bool slotOne = true;
};

// The common form decision over the inherited+authored GS fields.
bool policyFromFields(const Pass& p, const Gs& gs, const AuthoredPresent& present, Policy& output) {
    auto get = [&](Field field) { return gs[static_cast<std::size_t>(field)]; };
    auto flag = [&](Field field) { return get(field) == 0 || get(field) == 1; };
    if (!p.slotOne || !p.pass || p.pass > p.passCount || p.passCount > 2) return false;
    if (!flag(Field::alphaBlendingOnOff) || !flag(Field::alphaTestOnOff) ||
        !flag(Field::zWriteDisabled) || !flag(Field::foggingOnOff) || !flag(Field::onePassAntiAliasingOnOff) ||
        get(Field::destinationAlphaTestOnOff) != 0 ||
        get(Field::depthTestMethod) != 2 || get(Field::textureColorComponent) != 1 ||
        get(Field::textureFunction) != 0 || get(Field::alphaCorrectionValue) != 0 ||
        get(Field::alphaBlendingControlInUnitsOfPixels) != 0) return false;
    const bool blending = get(Field::alphaBlendingOnOff) == 1;
    const bool ordinaryProgram = p.program == "prelitcompressed" || p.program == "diffusecompressed" ||
        p.program == "diffusetextranscompressed"; // by program only (298C80), no material name
    const bool environment = p.program == "environmentmapcompressed";
    // GS ALPHA_1 forms (247A24..247B10), from the pass's own A/B/C/D/FIX fields.
    const bool additiveForm = blending && get(Field::inputColorA) == 0 &&
        get(Field::inputColorB) == 2 && get(Field::inputColorD) == 1;
    const bool readsDestinationAlpha = additiveForm && get(Field::inputAlphaC) == 1;
    const bool writesForReader = additiveForm && get(Field::inputAlphaC) == 0 &&
        p.passCount == 2 && p.pass == 1;
    const bool fixedAlpha = additiveForm && get(Field::inputAlphaC) == 2;
    const bool ordinaryBlend = blending && get(Field::inputColorA) == 0 && get(Field::inputAlphaC) == 0 &&
        get(Field::inputColorD) == 1 && (get(Field::inputColorB) == 1 || get(Field::inputColorB) == 2);
    if (p.projectedPrelit) {
        // Two-pass ProjectedPrelit: pass 2 blends over pass 1, no alpha test.
        if (p.program != "projectedprelit" || p.passCount != 2 || get(Field::alphaTestOnOff) != 0 ||
            blending != (p.pass == 2)) return false;
    } else if (environment) {
        // Environment map only as the destination-alpha reading pass (its
        // projected UVs and ambient vertex colour come from the environment-map owner).
        if (!readsDestinationAlpha || p.pass != 2 || p.passCount != 2) return false;
    } else if (!ordinaryProgram) return false;
    if (p.passCount == 2 && !p.projectedPrelit &&
        (p.staticAnimationFrame || !p.textureResolved)) return false;
    if (readsDestinationAlpha && p.passCount != 2) return false;      // C=Ad needs a writer pass
    if ((readsDestinationAlpha || writesForReader) && p.program == "diffusecompressed")
        return false;                                                  // lit: no GS byte path
    // PORT STAND-IN (flagged): the native fixed-alpha path draws FIX=128 only
    // ((Cs-0)*FIX/128+Cd = ONE,ONE); any other FIX is refused, not approximated.
    if (fixedAlpha && get(Field::fixedAlphaValue) != 128) return false;
    if (blending && !readsDestinationAlpha && !writesForReader && !fixedAlpha && !ordinaryBlend) return false;
    if (blending && !p.textureResolved) return false;

    Policy next;
    next.depthWrite = get(Field::zWriteDisabled) == 0;
    next.depthEqual = true; // Source GEQUAL becomes LEQUAL in the native depth map.
    next.sourcePrelit = p.projectedPrelit || p.program == "prelitcompressed";
    next.unlit = p.projectedPrelit || environment; // source-space ProjectedPrelit / ambient-row envmap
    next.sourceFogKnown = true;
    next.sourceFogEnabled = get(Field::foggingOnOff) != 0;
    next.sourceDepthWriteSpecified = present("zWriteDisabled");
    next.onePassAa1 = get(Field::onePassAntiAliasingOnOff) == 1;
    if (get(Field::alphaTestOnOff)) {
        if (get(Field::alphaTestMethod) != 6 || get(Field::alphaReferenceValue) < 0 ||
            get(Field::alphaReferenceValue) > 255 ||
            get(Field::processingMethodWhenFailedInAlphaTest) != 0) return false;
        next.sourceAlphaTest = true;
        next.sourceAlphaReference = static_cast<std::uint8_t>(get(Field::alphaReferenceValue));
    }
    if (readsDestinationAlpha || writesForReader) {
        // Both passes write source alpha independently (GS blends RGB only).
        // Pass N RGB uses As, pass N+1 the current Ad; the renderer's
        // destination-alpha route represents it (it refuses an unpaired 1 or 2).
        next.blend = Blend::additive;
        next.alphaScale = 1.f;
        next.gsDestinationAlphaPass = readsDestinationAlpha ? 2 : 1;
        next.sourceEnvironmentMap = environment;
    } else if (fixedAlpha) {
        // (Cs - 0) * FIX/128 + Cd with FIX=128: the native ONE,ONE RGB path,
        // without multiplying texture or vertex alpha.
        next.blend = Blend::additive;
        next.fixedBlendAlpha = true;
        next.alphaScale = 1.f;
    } else if (blending) {
        next.blend = get(Field::inputColorB) == 2 ? Blend::additive : Blend::sourceAlpha;
        // Declared native GS-unit modulation policy. This does not assert PS2
        // fragment quantization, microprogram or framebuffer parity.
        next.alphaScale = (255.0f/128.0f)*(255.0f/128.0f);
    }
    output = next;
    return true;
}

bool parseInteger(const std::string& text, std::int32_t& result) {
    std::int32_t value = 0;
    const auto parsed = std::from_chars(text.data(), text.data()+text.size(), value);
    if (parsed.ec != std::errc{} || parsed.ptr != text.data()+text.size()) return false;
    result = value;
    return true;
}
} // namespace

bool selectDefaultMtFields(const rr::assets::MaterialFields48E& passFields,const std::string& program,bool textureResolved,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    try {
        const AuthoredInteger authored = [&](const char* name, std::int32_t& result) {
            bool found = false;
            for (const auto& item : passFields.authored) {
                if (detail::lower(item.name) != detail::lower(name)) continue;
                if (found || !parseInteger(item.value, result)) return false;
                found = true;
            }
            return true;
        };
        const AuthoredPresent present = [&](const char* name) {
            for (const auto& item : passFields.authored)
                if (detail::lower(item.name) == detail::lower(name)) return true;
            return false;
        };
        Gs gs{};
        if (!inheritDefaultMt(templates, runtime, authored, gs)) return false;
        Pass p;
        p.program = detail::lower(program);
        p.textureResolved = textureResolved;
        return policyFromFields(p, gs, present, output);
    } catch (...) { return false; }
}

bool selectMaSa(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output) {
    try {
        const auto& fields = selection.sourcePassFields;
        if (!fields.is_object() || !fields.contains("authored") || !fields.at("authored").is_array()) return false;
        for (const auto& item : fields.at("authored"))
            if (!item.is_object() || !item.contains("name") || !item.contains("value") ||
                !item.at("name").is_string() || !item.at("value").is_string()) return false;
        const AuthoredInteger authored = [&](const char* name, std::int32_t& result) {
            bool found = false;
            for (const auto& item : fields.at("authored")) {
                if (detail::lower(item.at("name").get<std::string>()) != detail::lower(name)) continue;
                if (found || !parseInteger(item.at("value").get<std::string>(), result)) return false;
                found = true;
            }
            return true;
        };
        const AuthoredPresent present = [&](const char* name) {
            for (const auto& item : fields.at("authored"))
                if (detail::lower(item.at("name").get<std::string>()) == detail::lower(name)) return true;
            return false;
        };
        Gs gs{};
        if (!inheritDefaultMt(templates, runtime, authored, gs)) return false;
        Pass p;
        p.program = detail::lower(selection.program);
        p.pass = selection.pass;
        p.passCount = selection.passCount;
        p.projectedPrelit = selection.projectedPrelit;
        p.textureResolved = selection.textureIndex != track_materials50fa_fixture::unresolved;
        p.staticAnimationFrame = selection.staticAnimationFrame;
        p.slotOne = selection.slot == 1;
        Policy next;
        if (!policyFromFields(p, gs, present, next)) return false;
        // PORT STAND-IN (flagged, material bytes only, any course): the stock
        // beam material (PSM key 0e8d10...; its placements author
        // use_lighting=false) draws unlit until the placement use_lighting
        // field is read from data by the world owner. Keyed on the material's
        // own bytes, never on a course or a name.
        if (selection.materialKey == "0e8d108a38f22235038543b86fb9d6d8c1c648d7ba6959cdadef6c026d04b47e")
            next.unlit = true;
        output = next;
        return true;
    } catch (...) { return false; }
}

} // namespace material_alpha50gx_fixture
