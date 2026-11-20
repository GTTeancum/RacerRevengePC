#pragma once
#include "pod_materials50ed_fixture.h"
#include "original_racer_material_sources50la_fixture.h"
#include "../../src/assets/original_standard_material_domain50la.h"
#include "../../src/assets/track_projected_uv.h"
#include "../../src/assets/import_decode48c.h"
#include <cfenv>
#include <cmath>
#include <locale>

// Shared world-material types for every course: the load request, one
// selected material pass (Selection), one draw span (Span) and the load
// diagnostic. Produced by the one world owner (single_pass_world50LM, R3);
// read by the one material rule (material_alpha50gx_fixture.h) and the renderer.
// No course identity is carried: the race's Track is the identity.
namespace track_materials50fa_fixture {
namespace a = rr::assets;
constexpr std::uint32_t unresolved = a::unresolvedMaterialTexture50EA;
using MipMode = pod_materials50ed_fixture::MipMode;
using Choice = pod_materials50ed_fixture::Choice;
using TextureSelection = pod_materials50ed_fixture::TextureSelection;
// Caller supplies a reviewed original-format decoder. Input is the exact
// source member, authenticated here before the callback. Output must be a
// real decoded source mip/frame serialized as RRTEX001; never fallback pixels.
using SourceTextureDecoder = bool (*)(const char* sourceMember,
    const std::vector<std::uint8_t>& source, std::vector<std::uint8_t>& rrtex,
    std::uint32_t& mipIndex, std::string& error);
struct Request {
    std::filesystem::path root; // Retained visual49G directory.
    std::filesystem::path exportReport; // Matching 49G placed export report.
    Choice choice{}; // Explicit authored pass/slot and missing-texture policy.
    std::size_t coldSourceByteCapacity = 0;
    a::MaterialTextureBudgets50EA textureBudgets{};
    std::filesystem::path sourceArchive; // The course archive the world loads from.
    SourceTextureDecoder decodeSourceTexture = nullptr;
    // Deliberate host display policy: use an actual decoded ATM start frame until
    // an animation owner exists. False leaves selected ATM sources unresolved.
    bool allowStaticAnimationFrame = false;
    // Every authored pass of every material (274160 loads them all).
    bool includeAllAuthoredPasses = false;
    // The original-Main canonical material route. All three are required
    // together, from the SAME live Application/Video. No caller-set readiness
    // or image callback.
    std::shared_ptr<a::OriginalStandardMaterialDomain50LA> canonicalDomain;
    std::shared_ptr<const a::OriginalStandardMaterialContext50LA> canonicalContext;
    a::OriginalStandardMaterialPath50LA canonicalMaterialPath;
    struct CanonicalRacerModel {
        const a::OriginalStandardModelResource50LA* resource=nullptr;
        const a::OriginalStandardMaterialReferences50LA* references=nullptr;
    };
    // Actual completed engine resource/ref owners, borrowed for this
    // cold scan. No alternate table or duplicate26E640 retain is introduced.
    std::vector<CanonicalRacerModel> canonicalRacerModels;
};
struct Selection {
    std::uint64_t assetIdentity = 0;
    std::uint32_t materialIndex = 0, pass = 0, slot = 0;
    std::uint32_t bankSlot = unresolved, textureIndex = unresolved, mipIndex = unresolved;
    std::string sceneKey, materialName, materialKey, program, textureName, textureKey;
    std::string unresolvedReason;
    std::string decodedSourceMember, decodedSourceSha256;
    bool staticAnimationFrame = false;
    std::uint32_t passCount = 1;
    bool projectedPrelit = false;
    bool clampU = false, clampV = false;
    std::array<double,16> texGenMatrix{};
    nlohmann::json sourcePassFields; // Exact authenticated original PSM fields.
    // Diagnostic identities read from the real model1C4 ->382B38 pass ->
    //383400 resource chain that selected this SAME submitted bank slot.
    std::int32_t canonicalMaterialRow = -1;
    std::int16_t canonicalTextureRow = -1;

};
struct Span {
    std::uint32_t indexStart = 0, indexCount = 0, vertexStart = 0, vertexCount = 0;
    std::uint32_t textureIndex = unresolved, materialSelection = unresolved;
    std::uint32_t worldInstanceIndex = 0, surfaceIndex = 0;
    bool hasOriginalColors = false;
    std::uint32_t sourcePass = 1, sourcePassCount = 1, projectionPass = 0;
};
struct Diagnostic {
    std::uint32_t instance = unresolved, surface = unresolved, material = unresolved;
    const char* stage = "input";
    std::string sourceMember;
    std::string message;
    a::OriginalStandardMaterialDiagnostic50LA canonical{};
    original_racer_material_sources50LA::Diagnostic racerSource{};
    a::MaterialTextureAssetDiagnostic50EA bank{};
};
// The legacy TA/GC/GA material loader that read the retained visual49G
// exports (Owner::load, its per-course Track branches and detail helpers) is
// retired (PLAN B-C15): every course's world materials load through the one
// single-pass world owner (single_pass_world50LM::Owner, R3) from the course
// archive, and the 05.08 step runs the one material rule over them. Owner stays
// declared only for the frozen publication context member that still names it
// (W0 request r7 item 2, D:\Programming\GitHub\RacerRevengePC\work\scratch\common-rewrite\w0-requests\r7.txt).
class Owner;
} // namespace track_materials50fa_fixture
