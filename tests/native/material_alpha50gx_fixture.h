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
    // Common GS destination-alpha pair, from the pass's ALPHA fields:
    // 1 = A=Cs,B=0,C=As,D=Cd pass whose As the next pass reads, 2 = C=Ad pass.
    std::uint8_t gsDestinationAlphaPass = 0;
    // Pass program EnvironmentMapCompressed (projected UVs, ambient colour).
    bool sourceEnvironmentMap = false;
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
bool selectGc(const track_materials50fa_fixture::Selection& selection, Policy& output);

// GA keeps its existing lighting/fog/blend policy, but must inherit the real
// DefaultMT alpha test. Its flag PSMs omit these fields; omission is not OFF.
bool selectGa(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);

// Call only with a selection borrowed from a successfully loaded 50FA owner.
// Transactional: unsupported/mismatched selections leave output unchanged.
// Blank source blending fields retain the previous explicit opaque host policy;
// they are not claimed as recovered original defaults. Alpha scaling below is
// a declared GS-style host modulation policy, not original microprogram parity.
bool select(const track_materials50fa_fixture::Selection& selection, Policy& output);

// The MA/SA ordinary world route borrows the actual authenticated APPSTART
// DefaultMT owner. 2476A0 inherits its GS fields, then applies nonempty pass
// values; inputAlphaD is the compatibility alias before inputColorD. This is
// deliberately separate from the older TA/GC/GA explicit native policies.
bool selectMaSa(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);

// Exact MB one-pass policy; existing MA/SA admissions are unchanged.
bool selectMb(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);
// Exact TC one-pass policy. Both source textrans materials retain their own
// names and hashes; UV-controller admission is a separate typed owner.
bool selectTc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);
// RC owns its authenticated material selections. The exact crystal pair
// requires the separate explicit source reflection / destination feedback path.
bool selectRc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);
// SC borrows its own authenticated selections and the actual APPSTART defaults.
// Only the pinned lava material admits two active UV passes and fixed additive.
bool selectSc(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);
// Exact TB source-keyed projected and ordinary material admission.
bool selectTb(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);

// Exact GB source-keyed projected and ordinary material admission.
bool selectGb(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);

} // namespace material_alpha50gx_fixture
