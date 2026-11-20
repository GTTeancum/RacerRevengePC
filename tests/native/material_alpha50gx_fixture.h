#pragma once
#include "track_materials50fa_fixture.h"
#include "../../src/assets/original_ui_material50kg.h"
#include <charconv>
#include <cstdint>
#include <string>

// Cold material policy (blend, depth, fog, alpha test, AA1) for every course's
// world passes. This does not alter pixels, geometry, parsers, GPU texture
// ownership or cables.
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
    // GS fixed-alpha additive pass (C=FIX, FIX=128): independent of source alpha.
    bool fixedBlendAlpha = false;
    // Common GS destination-alpha pair, from the pass's ALPHA fields:
    // 1 = A=Cs,B=0,C=As,D=Cd pass whose As the next pass reads, 2 = C=Ad pass.
    std::uint8_t gsDestinationAlphaPass = 0;
    // Pass program EnvironmentMapCompressed (projected UVs, ambient colour).
    bool sourceEnvironmentMap = false;
    // GS onePassAntiAliasingOnOff after the 2476A0 DefaultMT inheritance and the
    // pass's own value (PLAN B-C15: AA1 is authored data on each pass, every
    // course). The renderer lowers it to its four-sample coverage path.
    bool onePassAa1 = false;
};
namespace detail {
inline std::string lower(std::string value) {
    for (char& c : value) if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    return value;
}
} // namespace detail

// ONE material rule for every course and every pass (PLAN B-C15, original
// 2476A0/247A24..247C88 over 274160 -> 14D370/29E340): the APPSTART DefaultMT GS
// fields are inherited, then the pass's own authored values apply
// (inputAlphaD is the compatibility alias before inputColorD). Every form is
// chosen from the pass's own fields and program, never from a course, a
// material name or a texture name:
//   - one-pass PreLit / Diffuse / DiffuseTexTrans programs;
//   - two-pass ProjectedPrelit (pass 2 blends over pass 1);
//   - the GS destination-alpha pair (A=Cs,B=0,D=Cd; C=As on the writing pass 1,
//     C=Ad on the reading pass 2, EnvironmentMapCompressed only as the reader);
//   - the GS fixed-alpha additive pass (A=Cs,B=0,C=FIX,D=Cd);
//   - authored AA1 (onePassAntiAliasingOnOff) on any pass.
// Call only with a selection borrowed from a successfully loaded world owner.
// Transactional: an unsupported or mismatched selection leaves output unchanged
// and returns false (the caller refuses the load at 05.08 and logs it).
// The name is kept from the MA/SA route it generalises (PLAN 2.2, W0 3.7).
bool selectMaSa(const track_materials50fa_fixture::Selection& selection,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);

// The same rule read straight from a pass's Material48E fields (common
// track-animation route, track_animation_window50an.h). selectMaSa calls it.
bool selectDefaultMtFields(const rr::assets::MaterialFields48E& passFields,const std::string& program,bool textureResolved,
    const rr::assets::OriginalUiMaterialTemplates50KG& templates,
    const rr::assets::OriginalUiMaterialRuntime50KG& runtime, Policy& output);

} // namespace material_alpha50gx_fixture
