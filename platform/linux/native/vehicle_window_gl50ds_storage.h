#pragma once
// Renderer-private complete Storage definition. Keep every data member in its
// original order: this split changes compilation boundaries, not ownership,
// resource identity, initialization/destruction order or publication authority.
// Only the existing small nested scope helpers and proc-loading template remain
// inline; executable families have one definition in the sibling .cpp files.
#include "../../../src/game/original_options_mutation_interlock50kg.h"
struct __GLsync; // OpenGL opaque sync type, never a native-dereferenced pointer.
#include "vehicle_window_gl50ds.h"
#include "original_ui_menu_raster_inputs50kg.h"
#include "../../../tests/native/ra_static_world50la.h"
#include "../../../tests/native/original_local_flare_emission50la.h"
#include "../../../tests/native/ra_static_fog_observation50la.h"
#include "original_ui_coverage_bounds50kg.h"
#include "native_camera_view50dx.h"
#include "../../../tests/native/original_race_f8_native_bars50la.h"
#include "../../../tests/native/original_results_native_frame50la.h"
#include "../../../src/game/original_racer_specular50la.h"
#include "../../../src/game/original_racer_diffuse50la.h"
#include "../../../src/game/original_racer_model_packets50la.h"
#include "../../../src/game/original_camera_start_effects.h"
#include "original_gameplay_blur_native_pass.h"
#include "material_texture_functions50ee.h"
#include "../../../src/game/original_race_hud50kg.h"
#include "../../../src/game/original_time_stats50la.h"
#include "../../../src/game/original_damage50la.h"
#include "../../../src/game/original_pause_native_draw50la.h"
#include "../../../src/game/original_position_stats_hud_draw50la.h"
#include "../../../src/game/original_speed_boost50la.h"
#include "../../../src/game/original_ui_text50kg.h"
#include "../../../src/game/original_ui_text_position50kg.h"
#include "../../../src/game/chariot_scene49u.h"
#include "native_pod_shadow50hm.h"
#include "../../../src/game/pod_cables50fd.h"
#include "../../../src/game/original_gameplay_binder_candidate.h"
#include <algorithm>
#include <chrono>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <limits>
#include <new>
#include <thread>
#include <utility>


namespace rr::platform {
namespace vehicle_window_gl50ds_detail {
using U = std::uint32_t;
using I = std::int32_t;
inline constexpr U arrayBuffer = 0x8892, elementBuffer = 0x8893;
inline constexpr U bufferSize = 0x8764, bufferUsage = 0x8765, dynamicDraw = 0x88E8, staticDraw = 0x88E4;
inline constexpr U framebuffer = 0x8D40, readFramebuffer = 0x8CA8, drawFramebuffer = 0x8CA9;
inline constexpr U framebufferComplete = 0x8CD5, backBuffer = 0x0405;
inline constexpr U renderbuffer = 0x8D41, colorAttachment = 0x8CE0, depthAttachment = 0x8D00;
inline constexpr U rgba8 = 0x8058, depth24 = 0x81A6, unsignedByte = 0x1401, floatType = 0x1406;
inline constexpr U rgba = 0x1908, pixelPackBuffer = 0x88EB;
inline constexpr U colorBufferBit = 0x00004000, depthBufferBit = 0x00000100;
inline constexpr U texture2D = 0x0DE1, textureUnit0 = 0x84C0;
// Keep the CPU capacity tied to the deliberately unrolled GLSL contract.
static_assert(maxVehicleWindowOpponents50DS == 7, "update explicit shadow caster slots when racer capacity changes");
inline constexpr U nativeShadowRacerCount = 1 + maxVehicleWindowOpponents50DS;
// Existing native shadow approximation retains its three primary-model casters.
// Optional models are rendered normally; no additional shadow parity is claimed.
inline constexpr U nativeShadowCasterCount = 3 * nativeShadowRacerCount;
inline constexpr U maxComponents = game::maxVehicleDrawComponents50CM;
// Bounded body slots: primary left/right, then actual optional lower left/right.
// Body slot N maps only to component N + 1; chariot has no source body slot.
inline constexpr U sourceBodyCount50LA = 4;
static_assert(maxComponents == 5, "source body slots require the exact five-component topology");
inline constexpr game::VehicleDrawComponent50CM components[] = {
    game::VehicleDrawComponent50CM::chariot, game::VehicleDrawComponent50CM::leftEngine,
    game::VehicleDrawComponent50CM::rightEngine, game::VehicleDrawComponent50CM::bottomLeftEngine,
    game::VehicleDrawComponent50CM::bottomRightEngine
};
bool fail(VehicleWindowDiagnostic50DS& d, VehicleWindowError50DS error,
    const char* operation) noexcept;
U componentIndex(game::VehicleDrawComponent50CM component) noexcept;
bool sourceBodyComponent50LA(U component, U componentCount) noexcept;
U trackSpanChunkCount(U indexCount, bool textured, U maximumTriangles) noexcept;
bool validateMaterialPolicy(const VehicleWindowMaterialBinding50DS& material,
    VehicleWindowDiagnostic50DS& d) noexcept;
bool validateMaterials(const VehicleWindowRequest50DS& request,
    const assets::VehicleDrawAssets50CR& owner, VehicleWindowDiagnostic50DS& d) noexcept;
// This checks live canonical state without touching create-only span arrays.
bool currentRaStaticFire(const VehicleWindowTrack50DS& track,VehicleWindowDiagnostic50DS& d) noexcept;
bool validateRaStaticFire(const VehicleWindowTrack50DS& track,VehicleWindowDiagnostic50DS& d) noexcept;
bool validateTbWorld(const VehicleWindowRequest50DS& request,VehicleWindowDiagnostic50DS& d) noexcept;
bool validateTrack(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept;
bool validateTrackAnimations(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept;
bool validateDynamicWorld(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept;
bool validateSky(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept;
bool validateCables(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept;
U cableIndex50DS(U index) noexcept;
bool validateCableFrame(const game::PodCableMesh50FD& mesh, VehicleWindowDiagnostic50DS& d) noexcept;

// Deliberately host-authored 5x7 letters and solid rectangles, never a source
// asset replacement. At most 8192 quads / 768 KiB CPU vertices, allocated cold
// only when explicitly enabled. Horizontal glyph runs share a single quad.
// Race counters/status, named standings and route/heat/damage share this batch.
// A 5x7 glyph has at most 21 runs. The existing 1024-quad allowance plus eight
// rows of (1 rank + 24 name + 3 completed laps + 3 FIN) glyphs, thirteen heading
// glyphs, two panel rectangles and eight highlights fits below 8192 quads.
// Completed timing adds fewer than 1200 quads: four duration values with at
// most ten digits (12 runs each), punctuation, and their fixed labels.
// Live clocks add fewer than 360 quads: two ten-digit durations, punctuation,
// and the fixed RACE / LAP TIME labels. The same 8192-quad bound is retained.
inline constexpr U hostHudMaximumQuads = 8192;
static_assert(maxVehicleWindowStandings50DS <= 9, "standings rank glyph requires a single-digit field");


std::uint64_t hostHudGlyph(char letter) noexcept;
bool validHostHudName(const char (&name)[maxVehicleWindowRacerName50DS + 1]) noexcept;
struct HostHudGeometry {
    assets::Vertex48B vertices[hostHudMaximumQuads * 4]{};
    U count = 0;
    float scale = 1.f, inverseWidth = 1.f, inverseHeight = 1.f;
    bool overflow = false;

    void quad(const float (&positions)[4][2], U rgb) noexcept;
    void rectangle(float x, float y, float w, float h, U rgb) noexcept;
    void text(float x, float y, const char* label, U rgb) noexcept;
    void bar(float y, float value, U rgb) noexcept;
    bool buildMenu(const VehicleWindowHostMenu50DS& input, I width, I height) noexcept;
    bool buildResults(const VehicleWindowHostHud50DS& input, I width, I height) noexcept;
    void standingsPanel(const VehicleWindowHostHud50DS& input, I width, I height,
        float reservedTop=0.f,float maximumScale=0.f) noexcept;
    bool build(const VehicleWindowHostHud50DS& input, I width, I height,
        float admittedOriginalBottom=0.f) noexcept;
};

// Linux desktop GL scalar signatures; resolved only through the caller's host.
// Match GL APIENTRY on Windows without importing platform GL headers.
#if defined(_WIN32)
using CopyImageSubData50DS = void (__stdcall *)(U, U, I, I, I, I,
    U, U, I, I, I, I, I, I, I);
#else
using CopyImageSubData50DS = void (*)(U, U, I, I, I, I,
    U, U, I, I, I, I, I, I, I);
#endif

struct OwnerGl {
    void (*genBuffers)(I, U*) = nullptr;
    void (*deleteBuffers)(I, const U*) = nullptr;
    void (*bufferData)(U, std::intptr_t, const void*, U) = nullptr;
    void (*getBufferParameteri64v)(U, U, std::int64_t*) = nullptr;
    void (*getBufferParameteriv)(U, U, I*) = nullptr;
    void (*getBufferSubData)(U, std::intptr_t, std::intptr_t, void*) = nullptr;
    void (*genVertexArrays)(I, U*) = nullptr;
    void (*deleteVertexArrays)(I, const U*) = nullptr;
    void (*enableVertexAttribArray)(U) = nullptr;
    void (*vertexAttribPointer)(U, I, U, std::uint8_t, I, const void*) = nullptr;
    void (*getVertexAttribiv)(U, U, I*) = nullptr;
    void (*getVertexAttribPointerv)(U, U, void**) = nullptr;
    void (*genFramebuffers)(I, U*) = nullptr;
    void (*deleteFramebuffers)(I, const U*) = nullptr;
    void (*bindFramebuffer)(U, U) = nullptr;
    U (*checkFramebufferStatus)(U) = nullptr;
    void (*framebufferRenderbuffer)(U, U, U, U) = nullptr;
    void (*genRenderbuffers)(I, U*) = nullptr;
    void (*deleteRenderbuffers)(I, const U*) = nullptr;
    void (*bindRenderbuffer)(U, U) = nullptr;
    void (*renderbufferStorage)(U, U, I, I) = nullptr;
    void (*getRenderbufferParameteriv)(U, U, I*) = nullptr;
    void (*getIntegerv)(U, I*) = nullptr;
    void (*useProgram)(U) = nullptr;
    void (*uniformMatrix4fv)(I, I, std::uint8_t, const float*) = nullptr;
    void (*uniform1i)(I, I) = nullptr;
    void (*uniform1f)(I, float) = nullptr;
    void (*uniform2i)(I, I, I) = nullptr;
    void (*uniform4i)(I, I, I, I, I) = nullptr;
    void (*uniform4f)(I, float, float, float, float) = nullptr;
    void (*blendFunc)(U, U) = nullptr;
    void (*blendEquation)(U) = nullptr;
    void (*blendFuncSeparate)(U, U, U, U) = nullptr; // core GL1.4
    // Resolved only for source-RGBA texture-feedback fallback after capability proof.
    void (*textureBarrier)() = nullptr;
    void (*clipControl)(U,U) = nullptr;
    void (*genTextures)(I,U*) = nullptr;
    void (*deleteTextures)(I,const U*) = nullptr;
    void (*texImage2D)(U,I,I,I,I,I,U,U,const void*) = nullptr;
    void (*texImage2DMultisample)(U,I,U,I,I,std::uint8_t) = nullptr;
    void (*drawBuffers)(I,const U*) = nullptr;
    void (*clearBufferfv)(U,I,const float*) = nullptr;
    void (*texParameteri)(U,U,I) = nullptr;
    void (*getTexLevelParameteriv)(U,I,U,I*) = nullptr;
    void (*framebufferTexture2D)(U,U,U,U,I) = nullptr;
    void (*activeTexture)(U) = nullptr;
    void (*bindTexture)(U, U) = nullptr;
    void (*getTexParameteriv)(U, U, I*) = nullptr;
    // Only loaded if GL >= 3.3 or ARB_sampler_objects is advertised.
    void (*bindSampler)(U, U) = nullptr;
    // Loaded only when the actual Garage batch first needs its own sampling state.
    void (*genSamplers)(I, U*) = nullptr;
    void (*deleteSamplers)(I, const U*) = nullptr;
    void (*samplerParameteri)(U, U, I) = nullptr;
    void (*getSamplerParameteriv)(U, U, I*) = nullptr;
    const unsigned char* (*getStringi)(U, U) = nullptr;
    void (*viewport)(I, I, I, I) = nullptr;
    void (*scissor)(I, I, I, I) = nullptr;
    void (*enable)(U) = nullptr;
    void (*disable)(U) = nullptr;
    void (*depthFunc)(U) = nullptr;
    void (*depthMask)(std::uint8_t) = nullptr;
    void (*depthRange)(double, double) = nullptr;
    void (*clearDepth)(double) = nullptr;
    void (*colorMask)(std::uint8_t, std::uint8_t, std::uint8_t, std::uint8_t) = nullptr;
    void (*polygonMode)(U, U) = nullptr;
    void (*clearColor)(float, float, float, float) = nullptr;
    void (*clear)(U) = nullptr;
    void (*drawBuffer)(U) = nullptr;
    void (*readBuffer)(U) = nullptr;
    void (*pixelStorei)(U, I) = nullptr;
    void (*readPixels)(I, I, I, I, U, U, void*) = nullptr;
    void (*finish)() = nullptr;
    void (*garageFlush24C160)()=nullptr; //Lazy same-context core glFlush, no ordinary startup dependency.
    // Garage-only lazy GL3.2 core observation. Ordinary startup is unchanged.
    __GLsync* (*fenceSync)(U,U)=nullptr;
    void (*getSynciv)(__GLsync*,U,I,I*,I*)=nullptr;
    std::uint8_t (*isSync)(__GLsync*)=nullptr;
    void (*deleteSync)(__GLsync*)=nullptr;
    // Dormant blend-only lookup; normal renderer creation is unchanged.
    void (*drawArrays)(U,I,I) = nullptr;
    const unsigned char* (*getString)(U) = nullptr;
    void (*blitFramebuffer)(I, I, I, I, I, I, I, I, U, U) = nullptr;
    // Optional, admitted only by the actual desktop GL 4.3+ context.
    CopyImageSubData50DS copyImageSubData = nullptr;
};
} // namespace vehicle_window_gl50ds_detail
// Private implementation header only; not part of vehicle_window_gl50ds.h.
using namespace vehicle_window_gl50ds_detail;

struct VehicleWindowGl50DS::Storage {
    std::shared_ptr<ra_static_fire_visual50LA::Owner> raStaticFireOwner;
    const VehicleWindowGl50DS* raStaticFireRenderer=nullptr;
    std::uint64_t raStaticFirePublicationSerial=0;
    void retireRaStaticFirePublication() noexcept;
    std::shared_ptr<rc_particle_visual50RC::Window> rcParticleWindow;
    const VehicleWindowGl50DS* rcParticleRenderer=nullptr;
    std::uint64_t rcParticleAttempt=0,rcParticlePendingAttempt=0,rcParticleCompletedBefore=0;
    bool cancelRcParticleAttempt() noexcept;
    bool beginRcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept;
    bool completeRcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept;
    void retireRcParticlePublication() noexcept;
    struct RcParticleAttemptScope {
        Storage& state;VehicleWindowDiagnostic50DS& diagnostic;bool retainPending=false;
        ~RcParticleAttemptScope() noexcept {
            if(!retainPending&&!state.cancelRcParticleAttempt()){
                diagnostic.cleanupIncomplete=true;
                fail(diagnostic,VehicleWindowError50DS::cleanup,"RC failed draw attempt retirement rejected");
            }
        }
    };
    std::shared_ptr<gb_particle_visual50GB::Window> gbParticleWindow;
    const VehicleWindowGl50DS* gbParticleRenderer=nullptr;
    std::uint64_t gbParticleAttempt=0,gbParticlePendingAttempt=0,gbParticleCompletedBefore=0;
    bool cancelGbParticleAttempt() noexcept;
    bool beginGbParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept;
    bool completeGbParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept;
    void retireGbParticlePublication() noexcept;
    struct GbParticleAttemptScope {
        Storage& state;VehicleWindowDiagnostic50DS& diagnostic;bool retainPending=false;
        ~GbParticleAttemptScope() noexcept {
            if(!retainPending&&!state.cancelGbParticleAttempt()){
                diagnostic.cleanupIncomplete=true;
                fail(diagnostic,VehicleWindowError50DS::cleanup,"GB failed draw attempt retirement rejected");
            }
        }
    };
    std::shared_ptr<mc_particle_visual50MC::Window> mcParticleWindow;
    const VehicleWindowGl50DS* mcParticleRenderer=nullptr;
    std::uint64_t mcParticleAttempt=0,mcParticlePendingAttempt=0,mcParticleCompletedBefore=0;
    bool cancelMcParticleAttempt() noexcept;
    bool beginMcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept;
    bool completeMcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept;
    void retireMcParticlePublication() noexcept;
    struct McParticleAttemptScope {
        Storage& state;VehicleWindowDiagnostic50DS& diagnostic;bool retainPending=false;
        ~McParticleAttemptScope() noexcept {
            if(!retainPending&&!state.cancelMcParticleAttempt()){
                diagnostic.cleanupIncomplete=true;
                fail(diagnostic,VehicleWindowError50DS::cleanup,"MC failed draw attempt retirement rejected");
            }
        }
    };
    struct MaterialUv { float value[4] = {1.f, 1.f, 0.f, 0.f}; };
    struct Material {
        U textureIndex = assets::unresolvedMaterialTexture50EA, name = 0;
        U specularName50LA = 0;
        std::uint64_t textureIdentity50LA = 0, specularIdentity50LA = 0;
        VehicleWindowMaterialBinding50DS policy{};
        float uv[4] = {1.f, 1.f, 0.f, 0.f};
    };
    struct TrackSpan {
        NativeTrackCoverage50DS nativeCoverage = NativeTrackCoverage50DS::aa0;
        U indexStart = 0, indexCount = 0;
        Material material{};
        bool discardZeroAlpha = false;
        bool sourceAlphaTest = false, sourceUvAnimation = false;
        std::uint8_t sourceAlphaReference = 0;
        U sourcePass = 1, sourcePassCount = 1, projectionPass = 0;
        std::uint8_t rcCrystalPass=0;
        bool worldFogEligible = false;
        bool allowPodShadow = true;
        U animation = assets::unresolvedMaterialTexture50EA;
        bool submittedGameplayCandidate = false, submittedCompletedGameplay = false;
    };
    struct TrackChunk {
        U sourceSpan = 0, indexStart = 0, indexCount = 0;
    };
    struct SpecularGroup50LA { U indexStart = 0, indexCount = 0; };
    struct SpecularGeometry50LA {
        bool enabled = false, singleDiffuse = false;
        U fogBuffers[2]{};
        game::original_racer_diffuse50LA::Geometry diffuseGeometry;
        U buffers[2]{}, groupCount = 0;
        U spanFirst[game::maxVehicleDrawSpans50CM]{}, spanCount[game::maxVehicleDrawSpans50CM]{};
        std::unique_ptr<SpecularGroup50LA[]> groups;
    };
    std::array<SpecularGeometry50LA, sourceBodyCount50LA> sourceSpecularGeometry50LA;
    bool hasSpecularPairs50LA = false, hasSingleDiffuse50LA = false;
    // Single-sample paired bodies use fixed-function blending (src*DST_ALPHA
    // + dst, alpha replaced; destination alpha kept in source byte/128 scale)
    // instead of per-triangle destination feedback. The same split runs on
    // NV2A-class hardware. RR_EXACT_PS2_SPECULAR=1 restores the byte-exact
    // feedback route for comparison. AA coverage keeps its exact bridge.
    bool fixedSpecularBlend50LA = true;
    std::unique_ptr<float[]> frameDiffuseFog50LA;
    std::unique_ptr<game::original_racer_diffuse50LA::Workspace> diffuseWorkspace50LA;
    game::OriginalRacerDiffuseCameraSnapshot50LA frameDiffuseCamera50LA;
    const game::OriginalRacerDiffuseCamera50LA* frameDiffuseEndpoint50LA=nullptr;
    bool frameDiffuseSelected50LA[1 + maxVehicleWindowOpponents50DS][sourceBodyCount50LA]{};
    U sourcePairColors50LA[2]{};
    std::uint64_t specularPublication50LA = 0;
    std::unique_ptr<std::array<float, 2>[]> frameSpecularUvs50LA;
    std::unique_ptr<game::OriginalRacerSpecularWorkspace50LA> specularWorkspace50LA;
    game::OriginalRacerSpecularRequest50LA frameSpecularCamera50LA{};
    std::uintptr_t frameSpecularCameraOwner50LA = 0;
    std::uint64_t frameSpecularCameraPublication50LA = 0;
    bool frameSpecularSecond50LA[1 + maxVehicleWindowOpponents50DS][sourceBodyCount50LA]{};
    VehicleWindowRequest50DS request{};
    VehicleGlFunctions50CV drawGl{};
    VehicleGlShaderFunctions50CZ shaderGl{};
    OwnerGl gl{};
    VehicleGlResources50CV resources{};
    U racerNormalBuffers[2][maxComponents]{};
    std::unique_ptr<VehicleGlShader50CZ> shader;
    std::unique_ptr<VehicleGlBackend50CV> adapter;
    const std::thread::id thread = std::this_thread::get_id();
    const game::VehicleDrawMesh50CM* meshes[maxComponents]{};
    const assets::Scene48D* scenes[maxComponents]{};
    std::unique_ptr<Material[]> materials;
    std::unique_ptr<MaterialUv[]> opponentMaterialUvs;
    U opponentMaterialStride = 0;
    U materialOffsets[maxComponents]{}, materialCount = 0;
    // Derived alongside the private cold policy copies. Frame UV updates and
    // UI texture-bank replacement never modify these racer material policies.
    struct RacerMaterialSummary {
        bool sourceDiffuse = false, sourcePair = false, normalDependent = false;
        bool hasDeferredEffects = false;
    };
    static bool podMaterialUsesNormals(const VehicleWindowMaterialBinding50DS& policy) noexcept;
    RacerMaterialSummary materialSummary[maxComponents]{};
    struct RacerArchetype {
        const game::VehicleDrawMesh50CM* meshes[maxComponents]{};
        const assets::Scene48D* scenes[maxComponents]{};
        VehicleGlResources50CV resources{};
        U normalBuffers[2][maxComponents]{};
        std::unique_ptr<Material[]> materials;
        U materialOffsets[maxComponents]{}, materialCount = 0;
        RacerMaterialSummary materialSummary[maxComponents]{};
        const MaterialTextureGl50EB* textures = nullptr;
        const MaterialTextureGl50EB* specularTextures50LA = nullptr;
        std::uint64_t texturePublication = 0, specularPublication50LA = 0;
        std::array<SpecularGeometry50LA, sourceBodyCount50LA> specularGeometry50LA;
        VehicleWindowCables50DS cables{};
        std::uint64_t cableTexturePublication = 0;
        U cableTextureName = 0;
    };
    RacerArchetype racerArchetypes[maxVehicleWindowRacerArchetypes50DS]{};
    U racerArchetypeCount = 0;
    struct RacerView {
        const game::VehicleDrawMesh50CM* const* meshes;
        const assets::Scene48D* const* scenes;
        const VehicleGlResources50CV* resources;
        const U (*normalBuffers)[maxComponents];
        Material* materials;
        const U* materialOffsets;
        const RacerMaterialSummary* materialSummary;
        U materialCount;
        U cableTextureName;
        const SpecularGeometry50LA* specular;
    };
    RacerView racerView(U archetype) const noexcept;
    std::uint64_t texturePublication = 0;
    std::unique_ptr<TrackSpan[]> trackSpans;
    U trackSpanCount = 0;
    U trackVisibilityPendingTarget = 0;
    std::uint64_t trackVisibilityCompletedGeneration = 0;
    bool trackVisibilityCompletedValid = false;
    void acceptCompletedTrackVisibility() noexcept;
    struct TrackAnimation {
        U sourceTextureIndex = assets::unresolvedMaterialTexture50EA;
        U firstTextureIndex = 0, frameCount = 0, selectedFrame = 0;
        U names[maxVehicleWindowAnimationFrames50DS]{};
    };
    std::unique_ptr<TrackAnimation[]> trackAnimations;
    U trackAnimationCount = 0;
    std::uint64_t trackAnimationTexturePublication = 0;
    std::unique_ptr<TrackSpan[]> dynamicWorldSpans;
    std::unique_ptr<assets::Vertex48B[]> dynamicWorldSourceVertices;
    U dynamicWorldSpanCount = 0, dynamicWorldVertexCount = 0, dynamicWorldNormalCount = 0;
    U dynamicWorldVertexBuffers[2]{}, dynamicWorldNormalBuffers[2]{}, dynamicWorldArrays[2]{};
    U dynamicWorldIndexBuffer = 0;
    I dynamicWorldIndexCount = 0;
    std::uint64_t dynamicWorldTexturePublication = 0;
    VehicleWindowDynamicWorldFrame50DS frameDynamicWorld{};
    std::unique_ptr<TrackSpan[]> skySpans;
    U skySpanCount = 0, skyVertexBuffer = 0, skyIndexBuffer = 0, skyVertexArray = 0;
    I skyIndexCount = 0;
    std::uint64_t skyTexturePublication = 0;
    // Address-only identity of actual successful cold geometry upload. Never
    // dereferenced; CPU bytes remain owned by the original immutable sky owner.
    const assets::Mesh48B* nativeSkyMeshIdentity129280=nullptr;
    float skyMatrix[16]{};
    bool frameSkyEnabled = true;
    std::unique_ptr<TrackChunk[]> trackChunks;
    U trackChunkCount = 0;
    std::unique_ptr<game::StaticDrawBounds50EI[]> trackDrawBounds;
    std::unique_ptr<U[]> trackVisibleDrawIds;
    U trackDrawCount = 0, frameTrackDrawCount = 0;
    bool frameTrackCullingApplied = false;
    VehicleWindowTrackUvFrame50DS frameTrackUv{};
    std::uint64_t trackTexturePublication = 0;
    std::uint64_t cableTexturePublication = 0;
    U cableTextureName = 0, cableVertexBuffer = 0, cableIndexBuffer = 0, cableVertexArray = 0;
    const game::PodCableMesh50FD* frameCables = nullptr;
    game::VehicleDrawBatch50CO framePlayerBatch{}, frameOpponentBatches[maxVehicleWindowOpponents50DS]{};
    // Borrow submit's validated value snapshots only through its synchronous
    // callbacks. Its scope guard clears these before the local snapshots die;
    // no source owner/lease pointer is borrowed.
    const game::NativeGcSelectedLightsCandidate* framePlayerLights = nullptr;
    const game::NativeGcSelectedLightsCandidate* frameOpponentLights[maxVehicleWindowOpponents50DS]{};
    const game::PodCableMesh50FD* frameOpponentCables[maxVehicleWindowOpponents50DS]{};
    U frameOpponentCount = 0;
    U frameOpponentArchetypes[maxVehicleWindowOpponents50DS]{};
    std::unique_ptr<HostHudGeometry> hostHudGeometry;
    U hostHudProgram = 0, hostHudVertexShader = 0, hostHudFragmentShader = 0;
    U hostHudVertexBuffer = 0, hostHudIndexBuffer = 0, hostHudVertexArray = 0;
    bool frameHostHud = false;
    U trackCandidateIndexCount = 0; // Sum of original material-pass index ranges.
    U trackVertexBuffer = 0, trackIndexBuffer = 0, trackVertexArray = 0, trackNormalBuffer = 0, trackProjectionBuffer = 0;
    U trackProgram = 0, trackVertexShader = 0, trackFragmentShader = 0;
    I trackMatrixUniform = -1, trackColorScaleUniform = -1;
    I trackUseTextureUniform = -1, trackImageUniform = -1;
    I trackDiscardZeroAlphaUniform = -1, trackUseSourceNormalsUniform = -1, trackProjectionPassUniform = -1;
    I trackUvTranslationUniform = -1, trackAlphaReferenceUniform = -1;
    I trackEffectModeUniform = -1, trackAlphaScaleUniform = -1, trackUnlitUniform = -1;
    I worldFogEnabledUniform = -1, worldFogCameraUniform = -1;
    I worldFogColorUniform = -1, worldFogRangeUniform = -1;
    float worldFogCamera[3]{};
    I trackIndexCount = 0;
    NativePodShadow50HM podShadows[nativeShadowRacerCount][3]{};
    float podShadowOpponentBounds[maxVehicleWindowOpponents50DS][4]{};
    I nativeShadowEnabledUniform = -1;
    I nativeShadowOpponentCountUniform = -1;
    I nativeShadowCenterUniforms[nativeShadowCasterCount]{}, nativeShadowShapeUniforms[nativeShadowCasterCount]{};
    I nativeShadowOpponentBoundsUniforms[maxVehicleWindowOpponents50DS]{};
    U targets[2]{}, colors[2]{}, depths[2]{};
    // Native F8-only scratch plane: actual opaque source alpha/depth writes
    // never overwrite the unrelated gameplay world's depth attachment.
    U nativeF8BarTarget=0,nativeF8BarColor=0,nativeF8BarDepth=0;
    bool nativeF8BarReady=false,nativeF8BarPreparing=false,nativeF8BarPreparationFailed=false;
    const ::original_main_entry50KG::NativeF8BarPrefix50LA* frameNativeF8Bars=nullptr;
    const ::original_main_entry50KG::NativeResultsFrame50LA* frameNativeResults=nullptr;
    const VehicleWindowFrame50DS* nativeResultsFrame=nullptr;
    const VehicleWindowGl50DS* nativeResultsRenderer=nullptr;
    const VehicleWindowFrame50DS* nativeF8BarFrame=nullptr;
    const VehicleWindowGl50DS* nativeF8BarRenderer=nullptr;
    std::array<assets::Vertex48B,16> nativeF8BarVertices{};
    // Native copied-image custody only. Canonical source/map owners remain
    // scoped by the actual F8 batch; no GPU name is an original texture handle.
    MaterialTextureGl50EB nativeF8SuffixTextures;
    std::shared_ptr<const assets::OriginalUiScreenAssets50KG> nativeF8SuffixImages;
    std::uint64_t nativeF8SuffixPublication=0;
    std::array<MaterialTextureSampler50EB,3> nativeF8SuffixSamplers{};
    ::original_main_entry50KG::NativeF8BarPrefix50LA::Suffix nativeF8Suffix;
    // Bounded immutable image+sampler variants, not one upload per glyph,
    // packet or occurrence. MaterialTextureGl couples sampler and image.
    // Per-occurrence geometry/policy remains separate from pixel reuse.
    // 126 fixed Results references + 7 per row in the existing bounded
    // eight-row CPU page. This is a native rejection ceiling, not a claim
    // that arbitrary animations/policies fit or original load caps at eight.
    static constexpr std::size_t maximumNativeResultsVariants=126+7*8;
    // Match the existing TimeStats/PositionStats/Damage native upload budget.
    static constexpr std::size_t maximumNativeResultsImageBytes=16u*1024u*1024u;
    struct NativeResultsVariant {
        std::shared_ptr<const assets::OriginalUiScreenAssets50KG> images;
        std::uint32_t image=0;
        MaterialTextureSampler50EB sampler{};
        assets::MaterialTextureAssets50EA pixels;
        MaterialTextureGl50EB textures;
        std::uint64_t publication=0;
    };
    struct NativeResultsPacket {
        std::vector<assets::Vertex48B> vertices;
        assets::OriginalUiNativeMaterial50KG textured;
        assets::OriginalUiNativeUntexturedMaterial50KG untextured;
        std::size_t variant=maximumNativeResultsVariants;
        bool hasTexture=false;
    };
    std::vector<std::unique_ptr<NativeResultsVariant>> nativeResultsVariants;
    std::vector<NativeResultsPacket> nativeResultsPackets;
    bool nativeResultsPreparationFailed=false;
    U nativeResultsTarget=0,nativeResultsColor=0,nativeResultsDepth=0;
    bool nativeResultsDrawResourcesReady=false,nativeResultsDrawPreparing=false;
    bool createNativeResultsDrawResources()noexcept;
    // Uncalled concrete native draw leaf. A future caller must first join
    // source slot48/B4/base/Frame authority; this leaf cannot mint it.
    bool drawNativeResultsPackets(U bank)noexcept;
    bool hasNativeResultsResources()const noexcept;
    bool deleteNativeResultsResources(VehicleWindowDiagnostic50DS&)noexcept;
    // Distinct native RACE display-fade storage. These are never UI receipts
    // or gameplay blur/history surfaces. Names remain renderer-owned after
    // retirement so the last complete sample stays available for readback.
    OriginalVideoBlockingNativeBackend* displayFadeOwner=nullptr;
    U displayFadeSourceTarget=0,displayFadeSourceColor=0;
    U displayFadeTargets[2]{},displayFadeColors[2]{};
    U displayFadeProgram=0,displayFadeVertexShader=0,displayFadeFragmentShader=0,displayFadeVertexArray=0;
    I displayFadeImageUniform=-1,displayFadeColorUniform=-1;
    bool displayFadeDeletionPending=false;
    U menuTargets[2]{}, menuColors[2]{}, displayedTarget = 0;
    std::uint8_t menuPublished = 0;
    U uiTargets[2]{}, uiColors[2]{}, uiDepths[2]{};
    std::shared_ptr<const std::uint8_t> uiDirectVideoLifetime;
    U uiSourceTarget=0,uiSourceColor=0,uiSourceDepth=0;
    U uiRetainedTarget=0,uiRetainedColor=0;
    U uiBlendIncomingTarget=0,uiBlendIncomingColor=0,uiBlendEffectTarget=0,uiBlendEffectColor=0;
    U uiBlendProgram=0,uiBlendVertexShader=0,uiBlendFragmentShader=0,uiBlendVertexArray=0;
    I uiBlendOldUniform=-1,uiBlendIncomingUniform=-1,uiBlendAlphaUniform=-1;
    // Nonzero borrow also guards failed begin/deletion until explicit retirement.
    std::uint64_t uiBlendOldSnapshot=0,uiBlendIncomingSource=0;
    std::uint64_t uiBlendSerial=0,uiBlendGeneration=0,uiBlendSampleSerial=0,uiBlendSampleGeneration=0;
    std::uint32_t uiBlendAlpha=0;
    bool uiBlendPoisoned=false;
    float uiBlendClear[4]{}; // Captured incoming clear, never a later source's.
    U uiPromotedIncomingTarget=0,uiPromotedIncomingColor=0;
    std::uint64_t uiPromotedIncomingGeneration=0;
    // Distinct from ordinary source publication and sample generation. A
    // private opaque copy does not imply a window copy or actual host swap.
    std::uint64_t uiPrivatePublicationSerial=0,uiPrivatePublicationGeneration=0;
    // Historical successful BACK copies only, independent of every private
    // publication/source/sample sequence. Never a live swap-authority epoch.
    std::uint64_t uiWindowBackBufferCopySerial=0;
    // The weak receipt identity is allocated only on first explicit slice1A use.
    // No context ownership is fabricated: every operation still checks current().
    std::shared_ptr<const std::uint8_t> uiSourceReceiptLifetime;
    bool uiSourceLeaseWork=false;
    std::uint64_t uiSourceAcceptedTargetGeneration=0,uiSourceAcceptedTexturePublication=0;
    std::uint64_t uiSourcePublishedTargetGeneration=0,uiSourcePublicationGeneration=0;
    std::uint64_t uiSnapshotSerial=0,uiSnapshotGeneration=0;
    std::uint64_t uiRetainedSourceTargetGeneration=0,uiRetainedPublicationGeneration=0;
    OriginalUiSourceTerminal50KN uiSourceTerminal{};
    float uiSourceClear[4]{};
    bool uiSourceCoherentFramebufferFetch=false;
    const OriginalUiSourceOwner50KM* uiSourceFrameOwner=nullptr;
    std::uint64_t uiSourceFrameGeneration=0,uiSourceTargetGeneration=0;
    U uiProgram = 0, uiVertexShader = 0, uiFragmentShader = 0;
    U uiVertexBuffer = 0, uiIndexBuffer = 0, uiVertexArray = 0, uiParticleFogBuffer = 0, uiSourceRasterWBuffer = 0, uiSourcePacketStqBuffer = 0;
    struct UiSourceIndexStream {
        static_assert(std::uint64_t(originalUiMaximumSourceQuads50KM) * 6 <=
            std::uint64_t(std::numeric_limits<I>::max()), "source UI draw count must fit GLsizei");
        static_assert(std::uint64_t(originalUiMaximumSourceQuads50KM) * 6 * sizeof(U) <=
            std::uint64_t(std::numeric_limits<std::intptr_t>::max()) &&
            std::uint64_t(originalUiMaximumSourceQuads50KM) * 6 * sizeof(U) <=
            std::numeric_limits<std::size_t>::max(), "source UI index bytes must fit buffer sizes");
        struct Span { U firstIndex = 0, indexCount = 0; };
        U indices[originalUiMaximumSourceQuads50KM * 6];
        Span spans[originalUiMaximumSpans50KG];
    };
    // Source-only cold storage; the ordinary RGB quad index buffer stays immutable.
    std::unique_ptr<UiSourceIndexStream> uiSourceIndices;
    U uiSourceIndexBuffer = 0;
    // Eight immutable effective policies: matching nearest/linear, repeat/clamp S/T.
    // These own no pixels and never modify a published MaterialTextureGl image.
    U garageSamplers50KG[8]{};
    bool garageSamplersReady50KG=false;
    I uiCanvasUniform = -1, uiImageUniform = -1, uiAlphaUniform = -1, uiTexturedUniform = -1;
    I uiParticleFogColorUniform = -1, uiSourceRasterUniform = -1;
    I uiSourceFogEnabledUniform = -1, uiSourceAlphaTestUniform = -1;
    I uiSourceCompositionUniform=-1,uiSourceTargetUniform=-1,uiSourceBlendUniform=-1;
    I uiCoveragePayloadUniform=-1;
    // Cold immutable atlas maxima; native blend admission is a joint texture
    // and vertex bound. Real Main atlases legitimately contain raw alpha255.
    std::uint8_t uiTextureMaximumAlpha[assets::maxMaterialTextureAssets50EA]{};
    std::uint8_t uiPublished = 0;
    std::uint64_t uiTexturePublication = 0,uiTextureEpoch=1;
    const MaterialTextureGl50EB* riderTextureBorrow=nullptr;
    I raceWidth = 0, raceHeight = 0;
    bool retainedRaceDisplayCompleted=false,retainedRaceContainsPause=false;
    std::uint64_t pendingRaceCameraPublication=0,retainedRaceCameraPublication=0;
    I maximumWidth = 0, maximumHeight = 0, width = 0, height = 0, clipDistances = 0;
    I committedWidth = 0, committedHeight = 0;
    float matrix[16]{}, clearRgba[4]{};
    VehicleWindowDiagnostic50DS* diagnostic = nullptr; // Only during synchronous calls.
    std::uint8_t published = 0;
    bool initialized = false, active = false, frameSet = false, imageCommitted = false;
    bool frontendOnly = false;
    __GLsync* garagePollFence50KG=nullptr;
    // This native raster lease is independent of guest Camera/Garage prelude
    // authority and source-frame image acceptance. It survives failed GL work.
    const OriginalUiSharedDialogs50KG* menuRasterShared50KG=nullptr;
    const OriginalUiSourceOwner50KM* menuRasterOwner50KG=nullptr;
    OriginalUiMenuRasterInputs50KG menuRasterInputs50KG;
    std::uint64_t menuRasterEpoch50KG=0,menuRasterInvocation50KG=0,menuRasterDisplay50KG=0;
    std::uint64_t menuRasterTextureEpoch50KG=0,menuRasterTargetGeneration50KG=0,menuRasterFrame50KG=0;
    U menuRasterTarget50KG=0,menuRasterColor50KG=0,menuRasterDepth50KG=0;
    const OriginalUiNativeDraw50KG* menuRasterNative50KG=nullptr;
    bool menuRasterApplied50KG=false,menuRasterFailed50KG=false,menuRasterBackendActive50KG=false;
    bool menuRasterSourceAccepted50KG=false;
    const OriginalUiSharedDialogs50KG* menuRasterRetiredShared50KG=nullptr;
    const OriginalUiSourceOwner50KM* menuRasterRetiredOwner50KG=nullptr;
    std::uint64_t menuRasterRetiredEpoch50KG=0,menuRasterRetiredInvocation50KG=0;
    void clearMenuRasterLease50KG()noexcept;
    const OriginalUiSourceOwner50KM* garagePreludeOwner50KG=nullptr;
    std::uint64_t garageCompletedPublication50KG=0,garageCompletedTarget50KG=0,garagePublishedPublication50KG=0;

    std::uint64_t garagePreludePublication50KG=0;
    bool garagePreludeFailed50KG=false;
    bool nativeGlFaultObserved50KG = false; // Never reset while this Storage lives.
    std::uint64_t contextDrainGeneration = 0;

#include "original_gameplay_blur_storage.inl"
#include "native_coverage_storage.inl"
bool uiCoverageDisjointTraced=false;
#include "original_ui_coverage_storage.inl"
#include "ra_static_world_gl_storage.inl"
#include "original_gc_particle_gl_storage50kg.inl"
#include "rc_crystal_gl_storage.inl"
#include "original_race_hud_render.inl"
#include "original_time_stats_render.inl"
#include "original_position_stats_render.inl"
#include "original_damage_render.inl"
#include "original_pause_derived_render.inl"
#include "original_speed_boost_render.inl"

    bool current(VehicleWindowDiagnostic50DS& d,
        const OriginalVideoBlockingNativeBackend* fadeOwner=nullptr,
        const OriginalUiSharedDialogs50KG* menuOwner=nullptr,
        const OriginalOptionsMutationInterlock50KG* optionsOwner=nullptr,
        const NativeVideoInvocation50LA* invocation=nullptr) const noexcept;
    std::uint64_t gameplayBinderPublication=0;
    const game::original_gameplay_binder_candidate::Mesh* frameGameplayBinders=nullptr;
    U frameGameplayBinderCount=0,frameGameplayBinderImage=0;
    bool gameplayBinderFirstDrawRecorded=false;
    bool texturesCurrent(VehicleWindowDiagnostic50DS& d) const noexcept;
    bool usesTextures() const noexcept;
    bool usesTrackShader() const noexcept;
    bool createTrackMaterials(const VehicleWindowTrack50DS& input) noexcept;
    bool createTrackAnimations(const VehicleWindowRequest50DS& input) noexcept;
    bool validateTrackAnimationFrames(const VehicleWindowFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d) const noexcept;
    bool createDynamicWorldMaterials(const VehicleWindowTrack50DS& input) noexcept;
    bool createSkyMaterials(const VehicleWindowSky50DS& input) noexcept;
    bool createTrackVisibility(const VehicleWindowTrack50DS& input) noexcept;
    void prepareTrackVisibility() noexcept;
    bool createMaterials(const VehicleWindowMaterials50DS& input) noexcept;
    bool validateMaterialUvs(const VehicleWindowMaterialUv50DS* uvs, U count,
        VehicleWindowDiagnostic50DS& d, U archetype = 0) const noexcept;
    // Only submit's callback-free CPU preflight owns these stack receipts.
    // They never survive that scope or cross a caller-currentness/GL callback.
    struct RacerNormalReceipt {
        struct Component {
            const assets::Vertex48B* vertices = nullptr;
            const assets::WorldNormal50HK* normals = nullptr;
            U vertexCount = 0, normalCount = 0;
        } component[maxComponents]{};
        const game::VehicleDrawBatch50CO* completedBatch = nullptr;
        U componentCount = 0;
        bool matches(const game::VehicleDrawBatch50CO& batch) const noexcept {
            if (completedBatch != &batch || componentCount != batch.componentCount) return false;
            for (U c = 0; c < componentCount; ++c) {
                const auto& part = batch.component[c];
                const auto& saved = component[c];
                if (saved.vertices != part.vertices || saved.normals != part.sourceNormals ||
                    saved.vertexCount != part.vertexCount || saved.normalCount != part.sourceNormalCount)
                    return false;
            }
            return true;
        }
        void accept(const game::VehicleDrawBatch50CO& batch) noexcept {
            componentCount = batch.componentCount;
            for (U c = 0; c < componentCount; ++c) {
                const auto& part = batch.component[c];
                component[c] = {part.vertices, part.sourceNormals, part.vertexCount, part.sourceNormalCount};
            }
            completedBatch = &batch;
        }
    };
    bool retainedMesh(const game::VehicleDrawBatch50CO& batch, VehicleWindowDiagnostic50DS& d,
        U archetype = 0, RacerNormalReceipt* normalReceipt = nullptr) const noexcept;
    bool validateRacerNormals(const game::VehicleDrawBatch50CO& batch,
        VehicleWindowDiagnostic50DS& d) const noexcept;
    bool validateOpponents(const game::VehicleDrawBatch50CO& player, const VehicleWindowFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d,
        RacerNormalReceipt (&normalReceipts)[1 + maxVehicleWindowOpponents50DS]) const noexcept;
    bool copyNativeGcLights(NativeRacerLightingPolicy50DS policy,
        const game::NativeGcSelectedLightsCandidate* input, U expectedRacerSlot,
        const game::VehicleDrawBatch50CO& batch, U archetype,
        std::uintptr_t& fleetOwner, game::NativeGcSelectedLightsCandidate& output,
        VehicleWindowDiagnostic50DS& d, RacerNormalReceipt& normalReceipt) const noexcept;
    bool hasDisplayFadeNames() const noexcept;
    bool loadDisplayFadeFunctions() noexcept;
    bool createDisplayFadeTarget(U& target,U& color,I w,I h) noexcept;
    bool createDisplayFadeProgram() noexcept;
    bool restoreDisplayFadeBindings(I w,I h) noexcept;
    bool deleteDisplayFadeNames(VehicleWindowDiagnostic50DS& d) noexcept;
    bool hasGlResources() const noexcept;
    bool check(const char* operation) noexcept;
    template<class F> bool load(F& pointer, const char* name) noexcept {
        pointer = reinterpret_cast<F>(request.procAddress(request.context, name));
        return pointer || fail(*diagnostic, VehicleWindowError50DS::entryPoint, name);
    }
    bool loadFunctions() noexcept;
    bool compileTrackShader(U type, const char* source, U& name) noexcept;
    bool createTrackShader() noexcept;
    bool verifyBuffer(U target, std::size_t bytes, U usage) noexcept;
    bool verifyLayout(U vertexBuffer, U indices) noexcept;
    bool createGeometry(VehicleGlResources50CV& resources,
        const game::VehicleDrawMesh50CM* const* meshes) noexcept;
    bool createStaticGeometry(const assets::Mesh48B* mesh, U& trackVertexBuffer,
        U& trackIndexBuffer, U& trackVertexArray, I& trackIndexCount,
        U vertexUsage = staticDraw, bool reuseIndices = false) noexcept;
    bool createWorldNormals(const VehicleWindowTrack50DS& track, U& trackNormalBuffer,
        U trackVertexArray, U usage = staticDraw) noexcept;
    // Preserve original PSG MSCNT group boundaries rather than pairing whole
    // surfaces or individual base/map-2 triangles. Each strip's triangle order
    // is checked against BOTH the semantic indices and immutable flattened EBO.
    bool createSpecularGroups50LA(U archetype, U body, SpecularGeometry50LA& out) noexcept;
    bool createDiffuseFogBuffers50LA(U archetype,U body,SpecularGeometry50LA& target) noexcept;
    bool createSpecularUvBuffers50LA(U archetype, U body, SpecularGeometry50LA& target) noexcept;
    bool prepareSpecularFrame50LA(const game::VehicleDrawBatch50CO& player,
        const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d,
        RacerNormalReceipt (&normalReceipts)[1 + maxVehicleWindowOpponents50DS]) noexcept;
    bool uploadSpecularUvs50LA(std::uint8_t bank, U slot, U archetype) noexcept;
    bool createRacerNormals(const VehicleGlResources50CV& geometry,
        U (&buffers)[2][maxComponents]) noexcept;
    bool uploadRacerNormals(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0, bool effectsOnly = false) noexcept;
    bool createTrackProjection(const VehicleWindowTrack50DS& track) noexcept;
    bool createDynamicWorldGeometry(const VehicleWindowTrack50DS& world) noexcept;
    bool validateDynamicWorldFrame(const VehicleWindowDynamicWorldFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d) const noexcept;
    bool uploadDynamicWorld(U bank) noexcept;
    bool createCableGeometry() noexcept;
    bool compileHostHudShader(U type, const char* source, U& name) noexcept;
    bool validateOriginalUiBank(const VehicleWindowRequest50DS& input,std::uint8_t* stagedAlpha=nullptr) noexcept;
    bool createOriginalUi() noexcept;
    bool hasNativeF8BarResources()const noexcept;
    bool createNativeF8BarResources()noexcept;
    bool prepareNativeF8BarFrame(const VehicleWindowGl50DS&,const VehicleWindowFrame50DS&,
        VehicleWindowDiagnostic50DS&)noexcept;
    bool drawNativeF8Bars(U bank)noexcept;
    bool deleteNativeF8BarNames(VehicleWindowDiagnostic50DS&)noexcept;
    bool createHostHud() noexcept;
    bool drawHostHud() noexcept;
    bool drawCables(const game::PodCableMesh50FD* cables, U textureName) noexcept;
    bool drawGameplayBinders() noexcept;
    bool createAttachment(U& name, U format, U attachment) noexcept;
    bool ensureUiSourceReceiptLifetime(VehicleWindowDiagnostic50DS& d) noexcept;
    bool createRetainedUiSourceTarget() noexcept;
    bool hasUiByteBlendNames()const noexcept;
    bool createUiByteBlendTarget(U& target,U& color) noexcept;
    bool createUiByteBlendProgram() noexcept;
    bool restoreUiByteBlendBindings() noexcept;
    bool deleteUiByteBlendNames(VehicleWindowDiagnostic50DS& d) noexcept;
    bool copyUiSourceToOpaque(U sourceTarget,const float* clear,I outputWidth,I outputHeight) noexcept;
    bool publishUiSource(const float* clear,I outputWidth,I outputHeight) noexcept;
    bool createOriginalUiSourceTarget() noexcept;
    bool createSpecularColorAttachment50LA(U& name) noexcept;
    bool createTargets() noexcept;
    bool checkDefaultFramebuffer() noexcept;
    bool checkTextureSampling(I major, I minor) noexcept;
    U garageSampler50KG(const MaterialTextureSampler50EB& sampler)const noexcept;
    bool ensureGarageSamplers50KG()noexcept;
    bool checkOriginalUiSourceSupport(I major,I minor) noexcept;
    bool checkSpecularPairSupport50LA(I major, I minor) noexcept;
    bool checkContext() noexcept;
    bool createRacerArchetypes(const VehicleWindowRequest50DS& input) noexcept;
    bool initializeOriginalFrontend(const VehicleWindowRequest50DS& input) noexcept;
    bool initialize(const VehicleWindowRequest50DS& input,
        const assets::VehicleDrawAssets50CR& owner) noexcept;
    // Only use a retained, preflighted batch and its own cold material bank.
    // The live world-space payload is the same one this transaction will draw.
    bool buildRacerPodShadows(const game::VehicleDrawBatch50CO& batch, U archetype,
        NativePodShadow50HM (&output)[3]) noexcept;
    static bool ready(void* context, const VehicleGlResources50CV& resources, std::uint8_t bank,
        const game::VehicleDrawBatch50CO& batch, const game::VehicleDrawPlan50CS&) noexcept;
    static bool materialReady(void* context, game::VehicleDrawComponent50CM component, U material) noexcept;
    static I effectMode(const VehicleWindowMaterialBinding50DS& policy) noexcept;
    void configureBlend(const VehicleWindowMaterialBinding50DS& policy, bool layeredDepth = false) noexcept;
    void unbindSpecularFeedback50LA() noexcept;
    void resetNativeGcLighting() noexcept;
    void resetPodState() noexcept;
    bool drawSky() noexcept;
    bool drawTrack(bool transparent) noexcept;
    bool drawDynamicWorld(U bank, bool transparent) noexcept;
    bool applyPodMaterial(const Material& material, bool suppress, const float* overrideUv = nullptr,
        const game::original_racer_lighting50KM::LightMatrices* selectedLights = nullptr) noexcept;
    bool drawSpecularSpan50LA(std::uint8_t bank, U slot, U archetype, U component,
        U ordinal, const assets::SceneSpan48D& span) noexcept;
    static bool drawSpan50LA(void* context, game::VehicleDrawComponent50CM component,
        U ordinal, const assets::SceneSpan48D& span, I baseVertex) noexcept;
    bool uploadRacer(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0, bool effectsOnly = false) noexcept;
    bool drawOpaqueOpponent(std::uint8_t bank, U opponent) noexcept;
    bool drawTransparentPod(std::uint8_t bank, U opponent = maxVehicleWindowOpponents50DS) noexcept;
    static bool begin(void* context, std::uint8_t bank) noexcept;
    static bool bindMaterial(void* context, game::VehicleDrawComponent50CM component, U material) noexcept;
    struct PlayerTimingCounter {std::uint64_t calls=0,wallNs=0;};
    struct PlayerTiming {
        PlayerTimingCounter value[game::maxVehicleDrawComponents50CM][static_cast<unsigned>(VehicleGlTimingPhase50CV::count)]{};
        bool invalid=false;
    } playerTiming;
    static void observePlayerTiming(void* context,VehicleGlTimingPhase50CV phase,U component,
        std::uint64_t ns,bool valid) noexcept;
    void reportPlayerTiming(bool accepted) const noexcept;
    // Diagnostic-only stamps. Default gameplay takes no clock samples here.
    std::chrono::steady_clock::time_point rendererPlayerStageStart{};
    std::chrono::steady_clock::time_point rendererStageNow() const noexcept;
    void traceRaStage(const char* stage,std::chrono::steady_clock::time_point& start,bool nested=true) noexcept;
    static bool commit(void* context, std::uint8_t bank) noexcept;
    static void abort(void* context, std::uint8_t) noexcept;
    bool copyToWindow() noexcept;
    bool copyToWindow(I copyWidth,I copyHeight) noexcept;
    bool copyRgba(std::uint8_t* destination,U target=0,I readWidth=0,I readHeight=0) noexcept;
    bool drainCleanupErrors(VehicleWindowDiagnostic50DS& d) noexcept;
    bool deleteName(U& name, void (*deleter)(I, const U*), VehicleWindowDiagnostic50DS& d) noexcept;
    bool deleteShaderName(U& name, bool program, VehicleWindowDiagnostic50DS& d) noexcept;
    bool cleanup(VehicleWindowDiagnostic50DS& d) noexcept;
    // Appended native-only packet target ownership. No source receipt or public GLuint.
    U nativeVideoPacketTargets50LA[2]{},nativeVideoPacketColors50LA[2]{};
    I nativeVideoPacketWidth50LA=0,nativeVideoPacketHeight50LA=0;
    std::size_t nativeVideoPacketRgbaBytes50LA=0;
    OriginalVideoPacketCpuRenderer50LA::NativePolicies nativeVideoPacketPolicies50LA{};
    std::uint64_t nativeVideoPacketSerial50LA=0;
    std::uint8_t nativeVideoPacketPublished50LA=0;
    bool nativeVideoPacketAttempted50LA=false,nativeVideoPacketReady50LA=false;
    bool nativeVideoPacketImage50LA=false;
    // One retained native marker, never Garage's marker or source CSR state.
    // Private command-stream custody. Never supplied by a public caller or
    // inferred from the copyable numeric observations of a native FINISH.
    const NativeVideoInvocation50LA* nativeVideoInvocation50LA=nullptr;
    __GLsync* nativeVideoFinishFence50LA=nullptr;
    void (*nativeVideoFinishFlush50LA)()=nullptr;
    NativeVideoPacketFinishState50LA nativeVideoFinishState50LA=NativeVideoPacketFinishState50LA::idle;
    std::uint64_t nativeVideoFinishOperation50LA=0,nativeVideoFinishImage50LA=0,nativeVideoFinishCapture50LA=0;
    std::uint8_t nativeVideoFinishBank50LA=0;
    void observeNativeVideoPacketFinish50LA(NativeVideoPacketFinishInfo50LA&) const noexcept;
    bool retireNativeVideoPacketFinish50LA(VehicleWindowDiagnostic50DS&) noexcept;
    void (*nativeVideoColorMaski50LA)(U,std::uint8_t,std::uint8_t,std::uint8_t,std::uint8_t)=nullptr;
    void (*nativeVideoGetBooleani50LA)(U,U,std::uint8_t*)=nullptr;
    void (*nativeVideoEnablei50LA)(U,U)=nullptr;
    void (*nativeVideoDisablei50LA)(U,U)=nullptr;
    std::uint8_t (*nativeVideoIsEnabledi50LA)(U,U)=nullptr;
    bool nativeVideoIndexedScissor50LA=false;
    bool prepareNativeVideoPacketFunctions50LA() noexcept;
    struct NativeVideoPacketSavedState50LA {
        I draw=0,read=0,renderbuffer=0,enabled[4]{};
        std::uint8_t colorMask[4]{};
    };
    bool nativeVideoPacketContextIdle50LA(VehicleWindowDiagnostic50DS&,const NativeVideoInvocation50LA* =nullptr) const noexcept;
    bool nativeVideoPacketIdle50LA(VehicleWindowDiagnostic50DS&,const NativeVideoInvocation50LA* =nullptr) const noexcept;
    bool saveNativeVideoPacketState50LA(NativeVideoPacketSavedState50LA&) noexcept;
    bool restoreNativeVideoPacketState50LA(const NativeVideoPacketSavedState50LA&) noexcept;
    bool allocateNativeVideoPacketPair50LA(U (&targets)[2],U (&colors)[2]) noexcept;
    bool deleteNativeVideoPacketTargets50LA(VehicleWindowDiagnostic50DS&) noexcept;
    U nativeVideoSnapshotTargets50LA[2]{},nativeVideoSnapshotColors50LA[2]{};
    bool nativeVideoSnapshotAttempted50LA=false,nativeVideoSnapshotReady50LA=false,nativeVideoSnapshotImage50LA=false;
    std::uint8_t nativeVideoSnapshotPublished50LA=0;
    std::uint64_t nativeVideoSnapshotSerial50LA=0,nativeVideoSnapshotImageSerial50LA=0;
    std::uint64_t nativeVideoSnapshotClearSerial50LA=0,nativeVideoPacketClearAncestor50LA=0;
    NativeVideoPacketWrite50LA nativeVideoPacketLastWrite50LA=NativeVideoPacketWrite50LA::none;
    bool transferNativeVideoPacketImage50LA(U source,U destination) noexcept;
    bool readNativeVideoPacketImage50LA(U target,std::uint8_t*,std::size_t,VehicleWindowDiagnostic50DS&) noexcept;
    // Private native compute overlay resources. No sampler, FBO or source lease.
    U nativeVideoOverlayTextures50LA[2]{},nativeVideoOverlayProgram50LA=0,nativeVideoOverlayShader50LA=0;
    I nativeVideoOverlayExtent50LA=-1,nativeVideoOverlayRgba50LA=-1;
    bool nativeVideoOverlayAttempted50LA=false,nativeVideoOverlayReady50LA=false;
    NativeVideoPacketOverlayPolicies50LA nativeVideoOverlayPolicies50LA{};
    std::uint64_t nativeVideoOverlayImageSerial50LA=0,nativeVideoOverlayCaptureSerial50LA=0;
    U nativeVideoOverlayPackedRgba50LA=0;
#if defined(_WIN32)
#define RR_NATIVE_VIDEO_GL_CALL50LA __stdcall
#else
#define RR_NATIVE_VIDEO_GL_CALL50LA
#endif
    struct NativeVideoOverlayFunctions50LA {
        void (RR_NATIVE_VIDEO_GL_CALL50LA *createTextures)(U,I,U*)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *textureStorage2D)(U,I,U,I,I)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *textureParameteri)(U,U,I)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *getTextureLevelParameteriv)(U,I,U,I*)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *getIntegeri)(U,U,I*)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *bindImageTexture)(U,U,I,std::uint8_t,I,U,U)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *uniform1ui)(I,U)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *dispatchCompute)(U,U,U)=nullptr;
        void (RR_NATIVE_VIDEO_GL_CALL50LA *memoryBarrier)(U)=nullptr;
    } nativeVideoOverlayGl50LA;
#undef RR_NATIVE_VIDEO_GL_CALL50LA
    struct NativeVideoOverlaySavedState50LA {I program=0,image[2][6]{};};
    bool prepareNativeVideoOverlayFunctions50LA() noexcept;
    bool allocateNativeVideoOverlay50LA() noexcept;
    bool saveNativeVideoOverlayState50LA(NativeVideoOverlaySavedState50LA&) noexcept;
    bool restoreNativeVideoOverlayState50LA(const NativeVideoOverlaySavedState50LA&) noexcept;
    bool deleteNativeVideoOverlay50LA(VehicleWindowDiagnostic50DS&) noexcept;
};
} // namespace rr::platform
