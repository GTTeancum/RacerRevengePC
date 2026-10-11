#pragma once
#include "vehicle_draw_gl50cv.h"
#include "vehicle_shader_gl50cz.h"
#include "material_texture_gl50eb.h"
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/native_asset48b.h"
#include "../../../src/assets/world_normals50hk.h"
#include "../../../src/assets/track_projected_uv.h"
#include "../../../src/assets/vehicle_draw_assets50cr.h"
#include "../../../src/game/draw_visibility50ei.h"
#include "../../../src/game/host_race_limits50ht.h"
#include <cstddef>
#include <cstdint>
#include <memory>

namespace rr::game { struct PodCableMesh50FD; }
namespace rr::platform {
using VehicleWindowGlProc50DS = void (*)();
enum class VehicleWindowMode50DS : std::uint8_t {
    unspecified, viewerNeutralUntextured, callerMaterialTextures,
    // Track-only host display policy: actual mesh RGB and geometric lighting.
    // Not recovered original-game materials, texture mapping, or lighting.
    sourceVertexColors
};
enum class VehicleWindowOutput50DS : std::uint8_t {
    defaultBackBuffer, headless
};
constexpr std::uint32_t maxVehicleWindowMaterials50DS = 4096; // RRSCENE1 per-component cap.
constexpr std::uint32_t maxVehicleWindowTrackSpans50DS = 65536;
constexpr std::uint32_t maxVehicleWindowTrackChunks50DS = 65536;
constexpr std::uint32_t maxVehicleWindowCompletedLaps50DS = 999;
constexpr std::uint32_t maxVehicleWindowStandings50DS = game::maxHostRaceRacers50HT;
constexpr std::uint32_t maxVehicleWindowRacerName50DS = 24;
constexpr std::uint32_t maxVehicleWindowLapTimes50DS = 3;
// 50ID's largest completed duration after signed-centisecond saturation.
constexpr float maxVehicleWindowLapSeconds50DS = 21474836.f;
constexpr std::uint32_t maxVehicleWindowOpponents50DS = game::maxHostRaceOpponents50HT;
constexpr std::uint32_t maxVehicleWindowRacerArchetypes50DS = maxVehicleWindowOpponents50DS;
enum class VehicleWindowMaterialMode50DS : std::uint8_t {
    unspecified, requiredTexture, untextured
};
// Explicit host material policy; defaults retain the previous opaque path.
enum class VehicleWindowBlend50DS : std::uint8_t { opaque, sourceAlpha, additive };
struct VehicleWindowMaterialBinding50DS {
    // Original material index is this entry's array index. No pass/slot or
    // texture is inferred. untextured must retain the unresolved sentinel;
    // requiredTexture must name an actual, published 50EB bank index.
    VehicleWindowMaterialMode50DS mode = VehicleWindowMaterialMode50DS::unspecified;
    std::uint32_t textureIndex = assets::unresolvedMaterialTexture50EA;
    VehicleWindowBlend50DS blend = VehicleWindowBlend50DS::opaque;
    // Independent of blending: caller selects the admitted source depth policy.
    bool depthWrite = true;
    // Original GS GEQUAL in a conventional GL depth map; TA defaults stay LESS.
    bool depthEqual = false;
    // Nonopaque source alpha = texture alpha * vertex alpha * alphaScale.
    // fixedBlendAlpha is admitted only with additive: RGB *= alphaScale and
    // ONE,ONE blending implements factors above one without alpha clamping.
    float alphaScale = 1.f;
    bool fixedBlendAlpha = false;
    // Bypass host geometric lighting; retain explicit RGB exposure. Opaque
    // racer bodies use source normals; nonopaque pod effects remain unlit.
    bool unlit = false;
    // Authenticated material program preserves precomputed vertex RGB.
    // Bypass added host illumination without suppressing native shadow receipt.
    bool sourcePrelit = false;
};
struct VehicleWindowComponentMaterials50DS {
    const VehicleWindowMaterialBinding50DS* bindings = nullptr;
    std::uint32_t count = 0;
};
struct VehicleWindowMaterials50DS {
    // Required only in callerMaterialTextures mode. Borrowed until close;
    // must outlive this renderer and retain the exact publication. Upload it
    // first with the same context AND isCurrent function pointer as 50DS.
    // Its sampler policy is used unchanged on texture unit 0, without an
    // external sampler override. No textures are uploaded or owned by 50DS.
    const MaterialTextureGl50EB* textures = nullptr;
    // Chariot/left-engine/right-engine. Exactly one explicit entry for EVERY
    // original scene material (even unused materials), at most 4096 per scene.
    // These arrays are borrowed unchanged during create only, then copied.
    VehicleWindowComponentMaterials50DS component[3]{};
};
struct VehicleWindowTrackSpan50DS {
    // Source-ordered triangle/vertex ranges into the unchanged flattened mesh.
    // First material passes partition both arrays contiguously, from zero;
    // adjacent continuation passes repeat that exact source surface range.
    // Every index must address this span's declared vertex range. Draws use
    // indexStart * sizeof(uint32_t) as their EBO offset and baseVertex = 0.
    std::uint32_t indexStart = 0, indexCount = 0, vertexStart = 0, vertexCount = 0;
    // Explicit required real bank texture, or explicit source-color-only span
    // with the unresolved sentinel. No missing texture silently changes mode.
    VehicleWindowMaterialBinding50DS material{};
    // Optional native zero-alpha discard for a required texture. Independent
    // explicit blend/depth policy above controls surviving fragments. Defaults
    // retain the earlier opaque cutout behavior; not original alpha-test parity.
    // Untextured spans must leave this false.
    bool discardZeroAlpha = false;
    // Pass one partitions the unchanged mesh. Further original passes must
    // immediately repeat that exact surface range, in ascending source order.
    std::uint32_t sourcePass = 1, sourcePassCount = 1;
    // 0 retains authored mesh UV; 1/2 selects the source-derived sidecar XY/ZW.
    std::uint32_t projectionPass = 0;
};
// Newly reconstructed native display policy, not recovered shader parity.
// Disabled unless the caller explicitly accepts the distance-source uncertainty.
struct VehicleWindowWorldFog50DS {
    bool enabled = false;
    float color[3]{};
    float start = 0.f, end = 1.f;
};
struct VehicleWindowTrack50DS {
    // Optional actual visual triangle mesh, already in world space. Borrowed
    // unchanged only during synchronous create(); success owns a GPU copy.
    // No root/placement transform, collision mesh, or missing-asset substitute
    // is inferred. The caller is responsible for authentic asset provenance.
    const assets::Mesh48B* mesh = nullptr;
    // Required nonzero byte ceilings when mesh is supplied, checked before
    // any renderer allocation or GL call. Only actual mesh bytes are uploaded.
    // Native Mesh48B count limits and GL signed size/count limits also apply.
    std::size_t maximumVertexBytes = 0, maximumIndexBytes = 0;
    // Separate explicit opt-in for track shading; never inherited/inferred.
    // viewerNeutralUntextured, sourceVertexColors, or callerMaterialTextures.
    // The latter two retain source RGB and light actual triangle orientation;
    // texture mode additionally consumes the explicit ordered span table.
    // Two-sided; material blending is explicit, with no general parity claim.
    VehicleWindowMode50DS mode = VehicleWindowMode50DS::unspecified;
    // Host display exposure, finite (0,8], used in sourceVertexColors and
    // callerMaterialTextures. Neutral/absent tracks require default 1.
    float vertexColorScale = 1.f;
    // Only for callerMaterialTextures. This exact 50EB publication/context/
    // isCurrent callback must survive until close. Can be separate from pod
    // textures. No texture is uploaded, deleted, or mutated by this renderer.
    const MaterialTextureGl50EB* textures = nullptr;
    // Borrowed only during create, then copied to bounded cold metadata.
    // Exactly ordered nonempty source spans, at most 65536. Other modes must
    // leave all three texture/span fields defaulted.
    const VehicleWindowTrackSpan50DS* spans = nullptr;
    std::uint32_t spanCount = 0;
    // Optional host visual optimization, disabled by default. Cold preparation
    // encloses the actual indexed world-space vertices of each source span
    // (or explicitly requested contiguous triangle chunk below);
    // nontextured modes use one whole-mesh draw. Submit uses the exact uploaded
    // matrix and conservative 50EI six-plane GL clipping (-W <= X,Y,Z <= W).
    // Touching/uncertain draws stay. Helper failure draws the full source list.
    // No caller display/collision bounds, sorting, LOD or gameplay visibility.
    bool enableFrustumCulling = false;
    // Optional finer host culling, zero preserves original source-span draws.
    // Nonzero requires callerMaterialTextures and enableFrustumCulling. Split
    // requiredTexture spans into at most this many contiguous triangles per
    // draw; explicitly untextured spans stay whole. Original span validation,
    // vertex/index bytes, source/material/triangle order and alpha/depth policy
    // are unchanged. At most 65536 total candidates, checked before allocation.
    // Chunk metadata/bounds/retained IDs allocate only during cold create().
    std::uint32_t cullingChunkTriangles = 0;
    // Optional authenticated world-space per-vertex normals, exact mesh order.
    // Cold-copy only; default preserves the older derivative-flat lighting.
    // Supplying normals opts into host directional/hemisphere illumination;
    // original game lighting parity is not implied. No welding or smoothing.
    // Unit float3 normals or exact zero (geometric fallback), one per vertex.
    const assets::WorldNormal50HK* sourceNormals = nullptr;
    std::uint32_t sourceNormalCount = 0;
    std::size_t maximumNormalBytes = 0;
    // Optional exact per-vertex source projection, copied into a separate VBO.
    // No generated textures or changes to the original mesh stream.
    const assets::TrackProjectedUv* sourceProjectionUvs = nullptr;
    std::uint32_t sourceProjectionCount = 0;
    std::size_t maximumProjectionBytes = 0;
    // Static-world opaque depth-writing surfaces and admitted projected pairs
    // only. Does not apply to sky, pod effects, UI or dynamic-world owners.
    VehicleWindowWorldFog50DS worldFog{};
};
// Narrow mutable world path for caller-animated authentic scene owners. The
// aggregate retains one fixed source topology/material table across the session.
constexpr std::uint32_t maxVehicleWindowDynamicVertices50DS = 4096;
constexpr std::uint32_t maxVehicleWindowDynamicIndices50DS = 16384;
constexpr std::uint32_t maxVehicleWindowDynamicSpans50DS = 64;
struct VehicleWindowDynamicWorldFrame50DS {
    // Exact cold vertex order/count. Only positions may change; authored UV and
    // color bytes must equal the cold mesh. Borrowed only through submit().
    const assets::Vertex48B* vertices = nullptr;
    std::uint32_t vertexCount = 0;
    // Required iff the cold descriptor supplied normals; same exact count.
    // Finite unit normals or exact zero. No transform/animation is inferred.
    const assets::WorldNormal50HK* sourceNormals = nullptr;
    std::uint32_t sourceNormalCount = 0;
};
struct VehicleWindowSky50DS {
    // Authored sky geometry only, copied cold into private GPU buffers. Its
    // positions use the separate per-frame skyWorldViewProjection matrix.
    // Exact span partition/material/budget checks match the track contract.
    // Caller must explicitly select unlit, opaque, non-depth-writing materials.
    const assets::Mesh48B* mesh = nullptr;
    std::size_t maximumVertexBytes = 0, maximumIndexBytes = 0;
    const MaterialTextureGl50EB* textures = nullptr;
    const VehicleWindowTrackSpan50DS* spans = nullptr;
    std::uint32_t spanCount = 0;
};
struct VehicleWindowMaterialUv50DS {
    // Optional per-frame scale/offset for an existing original pod material.
    // The array must be unique and sorted by component, then materialIndex.
    game::VehicleDrawComponent50CM component = game::VehicleDrawComponent50CM::chariot;
    std::uint32_t materialIndex = 0;
    float uScale = 1.f, vScale = 1.f, uOffset = 0.f, vOffset = 0.f;
};
struct VehicleWindowCables50DS {
    // Optional explicit real texture for the bounded 50FD two-cable mesh.
    // Borrow the exact publication/context/isCurrent until renderer close.
    // The bank can be separate from track/pod banks. Null requires the default
    // unresolved texture index and disables cable storage/drawing entirely.
    const MaterialTextureGl50EB* textures = nullptr;
    std::uint32_t textureIndex = assets::unresolvedMaterialTexture50EA;
};
// One explicit original static-track texture association and its source-ordered
// stored frame range in a separate immutable cold texture bank. Timing stays
// with the source owner; the renderer only validates and selects a frame name.
constexpr std::uint32_t maxVehicleWindowTrackAnimations50DS = 128;
constexpr std::uint32_t maxVehicleWindowAnimationFrames50DS = 64;
struct VehicleWindowTrackAnimation50DS {
    std::uint32_t sourceSpanIndex = assets::unresolvedMaterialTexture50EA;
    std::uint32_t sourceTextureIndex = assets::unresolvedMaterialTexture50EA;
    std::uint32_t firstTextureIndex = 0, frameCount = 0;
};
struct VehicleWindowRacerArchetype50DS {
    // Cold-loaded original chariot/left/right assets, kept unchanged until close.
    // The renderer consumes the caller's independently animated world vertices;
    // it never flattens component placement or substitutes a whole-pod transform.
    const assets::VehicleDrawAssets50CR* assets = nullptr;
    VehicleWindowMaterials50DS materials{};
    VehicleWindowCables50DS cables{};
};
struct VehicleWindowRequest50DS {
    // Borrowed until close. isCurrent must check this exact live desktop GL
    // context AND its host surface/currentness contract, on the creating thread,
    // without GL calls. Window output requires the exact window drawable;
    // headless output can use an existing OffscreenGlContext50DC and current().
    // Functions must not throw, reenter, or change the current context.
    void* context = nullptr;
    VehicleWindowGlProc50DS (*procAddress)(void*, const char*) noexcept = nullptr;
    bool (*isCurrent)(void*) noexcept = nullptr;
    // Allocate both private RGBA8/depth24 staging targets once at this size.
    // Every frame's pixel viewport must fit these explicit bounds.
    std::int32_t maximumWidth = 0, maximumHeight = 0;
    // Mandatory explicit opt-in; no missing texture triggers neutral shading.
    VehicleWindowMode50DS mode = VehicleWindowMode50DS::unspecified;
    // Leave entirely defaulted for the original vehicle-only path. A null
    // mesh with a mode, budgets or culling rejects instead of dropping track.
    VehicleWindowTrack50DS track{};
    // Leave entirely defaulted in viewerNeutralUntextured mode. Material
    // mapping/sampling decisions are caller policy, not recovered game state.
    VehicleWindowMaterials50DS materials{};
    // Compatible default: publish privately, then copy to framebuffer 0 BACK.
    // headless publishes only the same private target; no default framebuffer
    // format/completeness requirement, back-buffer blit, or host swap is needed.
    VehicleWindowOutput50DS output = VehicleWindowOutput50DS::defaultBackBuffer;
    VehicleWindowCables50DS cables{};
    // Explicit host-authored gameplay overlay, not original-game HUD parity.
    // Cold opt-in allocates bounded glyph/rectangle storage and two color-only
    // menu targets, independent of the two native frame targets. Default unchanged.
    bool enableHostHud = false;
    // NEW opt-in reconstructed original-art renderer. Disabled by default.
    // Exact CPU/GPU bank pair is checked cold; actual atlas alpha maxima are
    // combined with vertex alpha during draw admission (atlases may reach255).
    // Both immutable owners remain alive through close or an explicit bank
    // replacement with completed prior GPU work; no generated/font fallback.
    // Both may be null for explicit untextured-only draws; a textured span
    // still requires its real bank, and a partial bank pair always rejects.
    bool enableOriginalUi = false;
    // Cold opt-in: uses advertised coherent EXT_shader_framebuffer_fetch,
    // otherwise requires GL4.5 or advertised ARB_texture_barrier.
    // Allocates a separate fixed640x448 raw-RGBA8/D24 source target.
    bool enableOriginalUiSourceRgba = false;
    const assets::MaterialTextureAssets50EA* originalUiCpuTextures = nullptr;
    const MaterialTextureGl50EB* originalUiTextures = nullptr;
    VehicleWindowSky50DS sky{};
    // Native soft pod-footprint approximation on actual track receivers.
    // Explicit opt-in; no original shadow-program or exact-occlusion claim.
    bool enableNativePodShadows = false;
    // Optional cold-only array. Entries are selected by one-based frame IDs;
    // ID zero retains the create() player assets/materials/cables. Each entry
    // owns independently sized immutable indices and two cold vertex banks.
    // Material mapping arrays are copied; asset and texture owners stay alive.
    const VehicleWindowRacerArchetype50DS* racerArchetypes = nullptr;
    std::uint32_t racerArchetypeCount = 0;
    // Optional shared dynamic-world aggregate. Cold validation is the track
    // contract plus the narrower 4096/16384/64 bounds above. Requires explicit
    // callerMaterialTextures; no static culling/projection stream is admitted.
    // Mesh/spans/normals are borrowed only at create. Indices/materials/UV/color
    // stay fixed; frame.dynamicWorld supplies current positions and normals.
    // Two cold GPU vertex/normal banks share one immutable index buffer and the
    // existing private frame transaction. No extra clock or frame allocations.
    VehicleWindowTrack50DS dynamicWorld{};
    // Optional source-authenticated ATM bank. Borrow its exact publication until
    // close. Descriptors are copied cold; no texture creation/mutation at submit.
    // Every sourceSpanIndex names its exact original required-texture span;
    // sourceTextureIndex must match it. Distinct placed owners remain distinct.
    const MaterialTextureGl50EB* trackAnimationTextures = nullptr;
    const VehicleWindowTrackAnimation50DS* trackAnimations = nullptr;
    std::uint32_t trackAnimationCount = 0;
};
struct VehicleWindowStanding50DS {
    // Exact cold-verified original display name, including its terminator.
    // The native 5x7 font displays ASCII letters in uppercase. No shortening,
    // alternate name, source lookup, or inferred rank happens in the renderer.
    char displayName[maxVehicleWindowRacerName50DS + 1]{};
    std::uint32_t racerIndex = 0, place = 0, completedLaps = 0;
    bool finished = false;
};
struct VehicleWindowHostHud50DS {
    // Caller-supplied live values, finite [0,1]. Heat 0 = cool, 1 = heat limit;
    // damage 0 = pristine, 1 = fully damaged. Source units/thresholds and left/
    // right identity belong to the caller; the renderer never infers them.
    float heat01 = 0.f, leftDamage01 = 0.f, rightDamage01 = 0.f;
    bool boostActive = false, overheated = false;
    // Optional host route bearing to the caller's actual next track gate.
    // Enabled values must be finite [-1,1], with squared length within 0.001
    // of one. +X is screen right; +Y is ahead (drawn up). Disabled by default;
    // disabled coordinates are ignored. No route, heading or target is inferred.
    bool routeValid = false;
    float routeX = 0.f, routeY = 0.f;
    // Optional completed-lap count, displayed as HOST LAPS. Disabled values
    // are ignored; enabled counts above 999 reject the frame. No current-lap
    // increment, lap target, race-finished state or source progress is inferred.
    bool lapCountValid = false;
    std::uint32_t completedLaps = 0;
    // Optional caller-supplied direction warning. Replaces the HOST LAPS
    // label with WRONG WAY; the completed count remains visible. Ignored
    // unless lapCountValid is true. No direction or race result is inferred.
    bool wrongWay = false;
    // Optional native race presentation, supplied by the race state owner.
    // Disabled preserves HOST LAPS. No game/ranking result is inferred here.
    bool raceEnabled = false, raceGo = false, raceFinished = false;
    // HOST retained-scene result presentation after the normal four-second
    // source finish countdown; original menu/event transition is not projected.
    bool raceResultsReady = false;
    std::uint32_t raceLap = 1, raceLapTarget = 3, racePlace = 1, raceRacerCount = 1;
    std::uint32_t raceCountdown = 0;
    // Optional native completed times for the bounded three-lap race. The
    // caller copies CC0 entries and CB4 after the accepted timing boundary;
    // CB0 (the current lap) and HOST race/finish clocks are never substituted.
    // Disabled preserves mode2 presentation. Zero count shows no timed laps.
    bool completedLapTimesValid = false;
    std::uint32_t completedLapTimeCount = 0;
    float completedLapSeconds[maxVehicleWindowLapTimes50DS]{};
    float bestLapSeconds = 0.f;
    // Optional live native timing: CB8 is this racer's elapsed manager time;
    // CB0 is the current lap. No HOST double or summed lap times substitute.
    // Enabled values must be finite [0,maxVehicleWindowLapSeconds50DS], a
    // presentation bound only. Current-lap seconds are ignored after finish:
    // source CB0 has already restarted and is not the completed final lap.
    bool raceClocksValid = false;
    float raceElapsedSeconds = 0.f, currentLapSeconds = 0.f;
    // Optional complete field, already in the race owner's committed order.
    // Zero count disables it. Otherwise count must equal raceRacerCount and
    // fit the eight-row bound; indices form one permutation and place is 1..N.
    // Completed counts are source counts, never current-lap numbers. Finished
    // rows retain their sealed order; player highlighting uses this explicit ID.
    std::uint32_t standingsCount = 0, standingsPlayerIndex = 0;
    VehicleWindowStanding50DS standings[maxVehicleWindowStandings50DS]{};
    // Explicit nonblocking Results screen. The caller supplies the sealed
    // race observer's finish place/double seconds, separately from native
    // CB8/CC0 timing above. These values are not inferred or recomputed here.
    // Displaying/selecting this overlay does not pause world or audio owners.
    bool resultsPanelVisible = false;
    std::uint32_t resultsFinishPlace = 0, resultsSelectedAction = 0;
    double resultsFinishSeconds = 0.0;
    // Cancel-first confirmation stays on this live overlay; it never implies
    // a blocking Pause page or permission to change simulation/audio owners.
    bool resultsConfirming = false, resultsConfirmAccept = false;
};
struct VehicleWindowOpponent50DS {
    // Successful world-space 50CO output from the selected retained 50CR
    // archetype. Each racer requires independent live vertex arrays, while its
    // own original indices/spans remain unchanged. Borrowed synchronously only.
    const game::VehicleDrawBatch50CO* batch = nullptr;
    // Optional source-generated cables; requires the existing cold cable bank.
    const game::PodCableMesh50FD* cables = nullptr;
    // Same bounded, unique, source-ordered UV contract as the player. Missing
    // entries use identity, independently of player and other racer animation.
    const VehicleWindowMaterialUv50DS* materialUvs = nullptr;
    std::uint32_t materialUvCount = 0;
    // Zero selects the player bank; 1..request.racerArchetypeCount select the
    // explicitly supplied original geometry, material and cable banks.
    std::uint32_t racerArchetype = 0;
};
// Explicit HOST interface, independent of a native vehicle frame. Strings are
// borrowed only during submitHostMenu; each must terminate within 48 ASCII
// characters. No source menu assets, transitions, or sound events are inferred.
inline constexpr std::uint32_t maxVehicleWindowMenuRows50DS = 10;
inline constexpr std::uint32_t maxVehicleWindowMenuText50DS = 48;
struct VehicleWindowHostMenu50DS {
    const char* title = nullptr;
    const char* subtitle = nullptr;
    const char* rows[maxVehicleWindowMenuRows50DS]{};
    const char* footer = nullptr;
    std::uint32_t rowCount = 0, selectedRow = 0;
    // Copies the last accepted race image when one exists; never re-evaluates
    // native geometry. False shows a plain host backdrop, including at startup.
    bool retainRaceImage = false;
};
struct VehicleWindowFrame50DS {
    // Pixel viewport. Window output requires actual default-framebuffer drawable
    // dimensions, not logical window dimensions; skip minimized/zero drawables.
    // Headless output uses explicit positive dimensions within the cold bounds,
    // independent of the host surface's dimensions.
    std::int32_t width = 0, height = 0;
    // Sixteen finite floats: ROW-VECTOR world-to-GL-clip matrix in COLUMN-MAJOR
    // storage, as produced by 50DG. The shader uses vec4(world,1) * matrix.
    // Positions are already world-space; no model/root/camera is inferred.
    const float* worldViewProjection = nullptr;
    // Four finite [0,1] values. Matrix and clear color are copied before use.
    const float* clearRgba = nullptr;
    // Required iff request.cables supplied a bank. Actual current world-space
    // 50FD output: exactly 36 vertices/96 canonical indices. Borrowed only for
    // synchronous submit, validated before GL work, with no retained pointer.
    const game::PodCableMesh50FD* cables = nullptr;
    // Optional only with request.enableHostHud. Copied before frame GL work;
    // null hides it even when cold storage is enabled. Host-designed HEAT and
    // L/R DMG bars plus IDLE/BOOST/OVERHEAT, optional HOST/NEXT route cue and
    // optional HOST LAPS/WRONG WAY completed count, drawn before publication.
    // OVERHEAT takes visual priority when both supplied state flags are true.
    const VehicleWindowHostHud50DS* hostHud = nullptr;
    // Required exactly when an authored sky was supplied. Same row-vector,
    // column-major GL convention as worldViewProjection; no transform inferred.
    const float* skyWorldViewProjection = nullptr;
    // Borrowed synchronously and copied before any GL work; no frame allocation.
    // Missing entries use identity UV. Max 3*4096 entries; originals unchanged.
    const VehicleWindowMaterialUv50DS* materialUvs = nullptr;
    std::uint32_t materialUvCount = 0;
    // Optional zero-to-seven actual opponents in this ONE private transaction.
    // All batches are validated before drawing. All opaque racers precede track
    // and pod effects; sky, track and HUD are drawn once. Null/zero preserves
    // the original single-racer path. Enabled native footprint shadows use each
    // submitted racer's own current opaque component geometry and material bank.
    const VehicleWindowOpponent50DS* opponents = nullptr;
    std::uint32_t opponentCount = 0;
    VehicleWindowDynamicWorldFrame50DS dynamicWorld{};
    // Exactly one frame ordinal per cold animation descriptor. Borrowed only
    // through submit; every index is bounded before GL work. A supplied frame
    // always binds a genuine already-published image; no timer is advanced here.
    const std::uint32_t* trackAnimationFrames = nullptr;
    std::uint32_t trackAnimationFrameCount = 0;
    // Required iff track.worldFog.enabled. Three finite world-space camera
    // coordinates, copied during submit. Never derived from clip-space depth.
    const float* worldFogCamera = nullptr;
};
enum class VehicleWindowError50DS : std::uint8_t {
    none, alreadyOwned, request, assets, allocation, entryPoint, context,
    shader, limits, gl, indices, layout, framebuffer, notInitialized,
    wrongThread, notCurrent, frame, submission, presentation, cleanup, material,
    noImage, readback
};
struct VehicleWindowDiagnostic50DS {
    VehicleWindowError50DS error = VehicleWindowError50DS::none;
    const char* operation = nullptr; // Static; first failure wins.
    std::uint32_t component = 3, glError = 0, cleanupGlError = 0, framebufferStatus = 0;
    bool cleanupIncomplete = false;
    // The batch may have committed privately even if the final back-buffer
    // blit fails. Once that blit is attempted, its output can be partial on error.
    bool batchAccepted = false, defaultFramebufferMayBeModified = false;
    VehicleGlShaderDiagnostic50CZ shader{}, cleanupShader{};
    game::VehicleDrawSubmitDiagnostic50CS submission{};
    std::uint32_t material = assets::unresolvedMaterialTexture50EA;
    std::uint32_t texture = assets::unresolvedMaterialTexture50EA;
    std::uint32_t trackSpan = assets::unresolvedMaterialTexture50EA;
    // This call's real GL track draws that passed error checks. A later abort
    // can discard them; require batchAccepted for a committed image, and use
    // readback for actual pixels. Not GPU timing or visible-fragment counts.
    std::uint32_t trackDraws = 0, trackTexturedDraws = 0, trackUntexturedDraws = 0;
    std::uint32_t trackIndices = 0;
    // Dynamic-world checked GL commands; separate from immutable track counts.
    // An aborted frame does not publish these draws. No visible-pixel claim.
    std::uint32_t dynamicWorldDraws = 0, dynamicWorldIndices = 0;
    std::uint32_t dynamicWorldVerticesUploaded = 0, dynamicWorldNormalsUploaded = 0;
    std::uint32_t dynamicWorldSpan = assets::unresolvedMaterialTexture50EA;
    // Per-submit CPU selection receipts, also populated when culling is off.
    // Retained means selected, not necessarily submitted: trackDraws/Indices
    // above count checked GL commands; require batchAccepted for publication.
    // Candidates are chunks when explicitly requested, otherwise source spans
    // (one whole-mesh candidate in untextured modes). trackSpan above always
    // identifies the original source span, never an internal chunk ordinal.
    // Candidates = retained + culled. Uncertain is a subset of retained after
    // a successful helper call. Full-list fallback never consumes a partial or
    // stale helper output, and reports zero culled/uncertain counts.
    std::uint32_t trackCandidateDraws = 0, trackRetainedDraws = 0, trackCulledDraws = 0;
    std::uint32_t trackUncertainRetainedDraws = 0, trackCulledIndices = 0;
    bool trackCullingApplied = false, trackCullingFallback = false;
    game::DrawVisibilityDiagnostic50EI trackVisibility{};
    // Same checked-command/commit distinction as track receipts. One indexed
    // draw renders both actual cable strips, after the pod, before commit.
    std::uint32_t cableDraws = 0, cableVertices = 0, cableIndices = 0;
    // Checked effect commands, included in trackDraws when track effects.
    // Source order per pass, not reconstructed original global sorting.
    std::uint32_t transparentTrackDraws = 0, transparentPodDraws = 0;
    std::uint32_t transparentPodIndices = 0, skyDraws = 0, skyIndices = 0;
    // Explicit finishSubmittedWork or source-owner retirement: glFinish
    // returned with no reported GL error.
    // This is not a measured duration, presented swap, or hardware-GPU claim.
    bool submittedWorkCompleted = false;
    // Readback only: last committed viewport and required tightly packed RGBA8
    // bytes, supplied even for a null/undersized destination after currentness
    // and image checks. They do not report how many bytes were actually written.
    std::int32_t readbackWidth = 0, readbackHeight = 0;
    std::size_t readbackBytes = 0;
    // Set immediately before glReadPixels. On false with this set, any subset
    // of the required output bytes may have changed; discard the whole result.
    bool readbackBufferMayBeModified = false;
    // Host overlay checked draw receipt; require batchAccepted for publication.
    std::uint32_t hostHudDraws = 0, hostHudVertices = 0, hostHudIndices = 0;
    // UI-only publication, never a successful native vehicle batch receipt.
    bool hostMenuAccepted = false;
    bool originalUiAccepted = false;
    // Source acceptance is independent of opaque publication/window-copy failure.
    // Hidden source work leaves originalUiAccepted false and readback unchanged.
    bool originalUiSourceAccepted = false;
    // Source-publication/snapshot sequences only; neither counts host swaps.
    std::uint64_t originalUiPublicationGeneration = 0,originalUiSnapshotGeneration = 0;
    // Private byte-blend results only. No source/page update or publication.
    bool originalUiByteBlendAccepted = false;
    std::uint64_t originalUiByteBlendGeneration = 0, originalUiByteBlendSampleGeneration = 0;
    // Actual selected source path after successful create or for a source
    // submission. Ordinary RGB submissions leave this false.
    bool originalUiCoherentFramebufferFetch = false;
    std::uint32_t originalUiDraws = 0, originalUiQuads = 0;
    std::uint32_t originalUiSourceTriangles = 0, originalUiTextureBarriers = 0;
    std::uint64_t originalUiSourceFrameGeneration = 0, originalUiSourceTargetGeneration = 0;
    // Checked opaque opponent commands, not visible pixels. Require accepted
    // publication. Opponent effects are included in transparentPodDraws above.
    std::uint32_t opponentRacersDrawn = 0, opponentOpaqueDraws = 0, opponentOpaqueIndices = 0;
    // maxVehicleWindowOpponents50DS denotes no opponent-specific failure.
    std::uint32_t opponentIndex = maxVehicleWindowOpponents50DS;
};

// Unforgeable renderer-issued value. The live renderer revalidates its weak
// lifetime and exact target generation on every use; getters are observations,
// not authority. Current-source receipts do not pin the reusable target: the
// next source clear or owner retirement invalidates them. Completion is issued
// only after checked glFinish. Neither
// type claims a window swap, fresh Main route, observer/effect state, or parity.
class OriginalUiSourceSubmission50KN final {
public:
    std::uint64_t frameGeneration()const noexcept{return frameGeneration_;}
    std::uint64_t targetGeneration()const noexcept{return targetGeneration_;}
    std::uint64_t texturePublication()const noexcept{return texturePublication_;}
    bool completed()const noexcept{return completed_;}
    const OriginalUiSourceTerminal50KN& terminal()const noexcept{return terminal_;}
private:
    friend class VehicleWindowGl50DS;
    std::weak_ptr<const std::uint8_t> lifetime_;
    std::uint64_t frameGeneration_=0,targetGeneration_=0,texturePublication_=0;
    OriginalUiSourceTerminal50KN terminal_{};
    bool completed_=false;
};
class OriginalUiRetainedSource50KN final {
public:
    std::uint64_t snapshotGeneration()const noexcept{return snapshotGeneration_;}
    std::uint64_t publicationGeneration()const noexcept{return publicationGeneration_;}
    const OriginalUiSourceSubmission50KN& source()const noexcept{return source_;}
private:
    friend class VehicleWindowGl50DS;
    OriginalUiSourceSubmission50KN source_;
    std::uint64_t snapshotGeneration_=0,publicationGeneration_=0;
};

// Closed documented native sprite semantics, not a GS executor or measured
// silicon DDA parity claim. Source top-down639x447 coverage excludes its final
// column/row; all1087 border pixels preserve old RGBA on EVERY sample.
struct OriginalUiByteBlendContract50KO final {
    static constexpr std::int32_t width=640,height=448,coveredWidth=639,coveredHeight=447;
    static constexpr std::uint32_t coveredPixels=285633,preservedPixels=1087;
    static constexpr std::int32_t xyz0X=0,xyz0Y=0,xyz1X=639,xyz1Y=447;
    static constexpr std::int32_t uv0U16=1,uv0V16=1,uv1U16=10240,uv1V16=7168;
    static constexpr std::int32_t textureWidth=1024,textureHeight=512,bufferWidth=640;
    static constexpr std::uint32_t tex1=0,textureColorComponent=0,textureFunction=0;
    static constexpr std::uint32_t clampU=1,clampV=1,modulationRgb=128,minimumAlpha=0,maximumAlpha=128;
    static constexpr std::uint32_t psm=0,prim=0x156,test=0x31001;
    static constexpr std::uint64_t inheritedAlpha=0x0000008000000044ULL,inheritedClamp=0x5;
};
// Renderer-owned immutable pair: existing retained old snapshot plus a PRIVATE
// completed copy of incoming, not a pin of the reusable ordinary source target.
// Explicit release/close retires GPU ownership; destroying this value does not.
// It certifies no route history, observers, popup state, host swap or movement.
class OriginalUiByteBlend50KO final {
public:
    std::uint64_t generation()const noexcept{return generation_;}
    const OriginalUiRetainedSource50KN& oldSource()const noexcept{return old_;}
    const OriginalUiSourceSubmission50KN& incomingSource()const noexcept{return incoming_;}
private:
    friend class VehicleWindowGl50DS;
    OriginalUiRetainedSource50KN old_;
    OriginalUiSourceSubmission50KN incoming_;
    std::uint64_t generation_=0;
};
// Only the most recently accepted effect sample remains readable. An admitted
// sample attempt invalidates it BEFORE any target write, including on failure.
// Accepted is not completed. Neither state implies publication or a host swap.
class OriginalUiByteBlendSample50KO final {
public:
    std::uint64_t blendGeneration()const noexcept{return blendGeneration_;}
    std::uint64_t sampleGeneration()const noexcept{return sampleGeneration_;}
    std::uint32_t alpha()const noexcept{return alpha_;}
    bool completed()const noexcept{return completed_;}
private:
    friend class VehicleWindowGl50DS;
    std::weak_ptr<const std::uint8_t> lifetime_;
    std::uint64_t blendGeneration_=0,sampleGeneration_=0;
    std::uint32_t alpha_=0;
    bool completed_=false;
};

// Caller-owned desktop GL >=3.2. Window output requires a double-buffered,
// nonstereo, nonmultisampled window surface; headless output does not.
// Owns GPU storage only; never creates/binds/releases a context, polls, or swaps.
// Both modes use the same transactional 50CV targets and real geometry/material
// path. Window output separately blits to framebuffer 0's BACK buffer. Submit
// success is command acceptance only. On false, DO NOT swap: a window back
// buffer can be partially changed. The caller owns presentation and recovery.
//
// Cold create allocates bounded storage; submit has no application allocation.
// Optional track geometry is uploaded once. Opaque track precedes opaque pods
// and cables; selected track/pod effects follow, source-ordered in each pass.
// Authored sky precedes these using its separate caller-supplied matrix. All
// work remains in the same unpublished private target until final acceptance.
// Keep loaded 50CR assets unchanged and alive until close. Keep the same live
// 50EB publication when selected, with its exact context callbacks, until this
// renderer is closed. Use one thread, one unshared context, and no reentrant or
// concurrent calls. Host event processing
// and swap between calls are allowed. No other code may mutate these private
// GL objects; no unrelated rendering/state changes are supported. State is not
// saved/restored. Inputs, readback output and diagnostics must be distinct live
// storage. Readback output is borrowed only during its synchronous call.
class VehicleWindowGl50DS final {
public:
    VehicleWindowGl50DS() noexcept;
    // Call close before destroying the host/context. Like the host, unresolved
    // cleanup or wrong-context/thread destruction terminates; it never silently
    // abandons still-owned GL names. Inspect/recover via explicit close first.
    ~VehicleWindowGl50DS() noexcept;
    VehicleWindowGl50DS(const VehicleWindowGl50DS&) = delete;
    VehicleWindowGl50DS& operator=(const VehicleWindowGl50DS&) = delete;
    VehicleWindowGl50DS(VehicleWindowGl50DS&&) = delete;
    VehicleWindowGl50DS& operator=(VehicleWindowGl50DS&&) = delete;

    bool create(const VehicleWindowRequest50DS&, const assets::VehicleDrawAssets50CR&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Accepts actual, successful 50CO batches from the exact retained 50CR
    // prepared meshes. Original scene material indices remain validated even
    // in explicit neutral mode. A matrix/size/structural rejection does not blit.
    bool submit(const game::VehicleDrawBatch50CO&, const VehicleWindowFrame50DS&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Ordered authenticated original quads, separate RGBA8/D24 transaction.
    // Does not advance gameplay or replace the retained race publication.
    bool submitOriginalUi(const OriginalUiNativeDraw50KG&, std::int32_t width,
        std::int32_t height, VehicleWindowDiagnostic50DS&) noexcept;
    // Complete sealed Rider frame, fresh source RGBA, then opaque presentation.
    // Ordinary submitOriginalUi retains its existing RGB-only contract.
    bool submitOriginalUiSourceRgba(const OriginalUiSourceFrame50KM&, std::int32_t width,
        std::int32_t height, VehicleWindowDiagnostic50DS&) noexcept;
    bool originalUiSourceRgbaAvailable() const noexcept;
    // Dormant slice1A. The existing combined submit above remains the default.
    // Hidden submissions execute the whole sealed source frame and acknowledge
    // its material history exactly once, without touching opaque publication.
    // Output must be distinct from inputs. On false no output receipt is issued.
    bool submitOriginalUiSourceUnpublished(const OriginalUiSourceFrame50KM&,
        std::int32_t width,std::int32_t height,OriginalUiSourceSubmission50KN&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Retrieve the current successfully submitted source, including the source
    // of a combined normal submit. No GPU completion is inferred here.
    bool originalUiSourceSubmission(OriginalUiSourceSubmission50KN&,
        VehicleWindowDiagnostic50DS&) noexcept;
    bool completeOriginalUiSource(const OriginalUiSourceSubmission50KN&,
        OriginalUiSourceSubmission50KN&,VehicleWindowDiagnostic50DS&) noexcept;
    // Requires the exact still-current completed source. Copies RGB to the
    // independent opaque target and (when configured) window BACK buffer.
    // Caller still owns the real swap. Repeated publication is not a page update.
    bool publishOriginalUiSource(const OriginalUiSourceSubmission50KN&,
        std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS&) noexcept;
    // Retains the last successful renderer-published source BEFORE that source
    // target is cleared again. Lazily allocates a distinct640x448 RGBA8 texture,
    // copies all four channels and checks real GPU completion. One immutable
    // snapshot at a time, preserved across hidden submissions and owner retire.
    // This is NOT a host-displayed receipt; host swap provenance is still absent.
    // Failed allocation/copy leaves owned names for explicit release/close retry.
    bool retainPublishedOriginalUiSource(OriginalUiRetainedSource50KN&,
        VehicleWindowDiagnostic50DS&) noexcept;
    bool readRetainedOriginalUiSourceRgba(const OriginalUiRetainedSource50KN&,
        std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS&) noexcept;
    // Checked completion precedes deletion, including failed snapshot attempts.
    // Failure retains the unresolved ownership obligation for retry. After a
    // failed deletion, do not create/reuse GL names in this context (including
    // through other texture owners) until release or close completes, matching
    // the renderer's existing conservative failed-close ownership contract.
    bool releaseRetainedOriginalUiSource(VehicleWindowDiagnostic50DS&) noexcept;
    // Dormant renderer-only byte operation. Incoming must be a still-current
    // completed sealed ordinary Rider source newer than retained old, with the
    // actual terminal icon_x/pass1 ALPHA/CLAMP/COLCLAMP/PABE/FBA state. Copies
    // incoming into distinct640x448 RGBA8 and completes that copy before issuing
    // a lease. Retained old cannot be released until the blend is retired.
    // This is NOT full transition admission: closed route/host-swap ownership
    // must be supplied by a future integration. No frontend caller or enable flag is added.
    bool beginOriginalUiByteBlend(const OriginalUiRetainedSource50KN& oldSource,
        const OriginalUiSourceSubmission50KN& incoming,OriginalUiByteBlend50KO&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Every real sample starts from the immutable pair. Integer signed-floor
    // RGB, alpha byte0..128 on covered pixels, unchanged old RGBA on borders.
    // Even alpha0/128 execute a real draw. Source target and opaque publication
    // remain unchanged; this never updates a page, clock, audio or host swap.
    bool renderOriginalUiByteBlendSample(const OriginalUiByteBlend50KO&,
        std::uint32_t alpha,OriginalUiByteBlendSample50KO&,VehicleWindowDiagnostic50DS&) noexcept;
    bool completeOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO&,
        OriginalUiByteBlendSample50KO&,VehicleWindowDiagnostic50DS&) noexcept;
    // Full source/effect RGBA, bottom row first; requires a completed current
    // sample, distinct writable output and640*448*4 bytes. No publication.
    bool readOriginalUiByteBlendSampleRgba(const OriginalUiByteBlendSample50KO&,
        std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS&) noexcept;
    // Also retires partial/failed begin or sample work. Checked glFinish and
    // successful name deletion precede unlocking retained old. On failure,
    // keep the renderer/context alive and retry release/close. After ambiguous
    // deletion, no owner may create/reuse GL names until retirement succeeds.
    bool releaseOriginalUiByteBlend(VehicleWindowDiagnostic50DS&) noexcept;
    // Explicit destination-lifetime retirement, while this concrete composer
    // is still alive. Requires the live context/thread and an idle renderer;
    // rejects a different retained owner, drains all preceding GL work even
    // after a failed/unpublished attempt, then releases the generation borrow.
    // On failure the owner must stay alive for retry. A reconstructed Rider
    // destination can then begin its own nonzero generation sequence safely.
    bool retireOriginalUiSourceOwner(const OriginalUiRiderDrawContribution50KG&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Explicit destination resource publication between complete submissions.
    // Authenticates every CPU/GPU texture identity and caches actual alpha
    // maxima before replacing the retained borrow. False preserves the old
    // bank and image. No draw, upload, route admission or gameplay work occurs.
    // Caller retains the new immutable owners through the next replacement or
    // close, and retains prior GPU owners until their submitted work completes.
    bool bindOriginalUiTextures(const assets::MaterialTextureAssets50EA&,
        const MaterialTextureGl50EB&, VehicleWindowDiagnostic50DS&) noexcept;
    // Source-compatible fail-closed boundary: recovered draw-only packets lack
    // explicit source depth and destination-alpha-consumer ownership.
    bool submitOriginalUi(const OriginalUiDraw50KG&, std::int32_t width,
        std::int32_t height, VehicleWindowDiagnostic50DS&) noexcept;
    // Uses the opt-in HUD's bounded text storage and two private color-only
    // targets. Preserves both native GPU banks and the last accepted race image.
    // Safe while simulation is stopped or interrupted; no native data, source
    // audio, vehicle batch, or original menu state is inspected or advanced.
    // Success includes a window-back-buffer copy when configured; caller swaps.
    bool submitHostMenu(const VehicleWindowHostMenu50DS&, std::int32_t width,
        std::int32_t height, VehicleWindowDiagnostic50DS&) noexcept;
    // Both modes: read the last privately committed race, host-menu or original-UI viewport,
    // including a frame whose later window blit failed. No image exists before
    // the first successful private commit from a submission path.
    // RGBA unsigned bytes, tightly packed, bottom row first; width*height*4
    // bytes. No application allocation or pointer retention. Caller supplies
    // live writable storage of at least capacityBytes, disjoint from all inputs
    // and diagnostics. A failed call may partially modify it only if diagnostic
    // readbackBufferMayBeModified is set; it never changes private publication.
    // Readback can synchronize with GPU work; submit itself still does not.
    bool readbackRgba(std::uint8_t* rgba, std::size_t capacityBytes,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Explicit caller-controlled completion barrier for already submitted work,
    // e.g. bounded headless completed-frame timing. Requires an existing private
    // committed image and the exact live context/thread; calls glFinish, checks
    // errors, and sets diagnostic.submittedWorkCompleted only on success. No
    // allocation, readback, swap, image publication change, or implicit timing.
    // Normal submit/window paths never call this barrier automatically.
    bool finishSubmittedWork(VehicleWindowDiagnostic50DS&) noexcept;
    // Failed cleanup retains unresolved ownership for a retry. If cleanup is
    // complete the instance is empty even when stale GL errors were reported.
    bool close(VehicleWindowDiagnostic50DS&) noexcept;
    bool initialized() const noexcept;
    bool ownsResources() const noexcept;
private:
    bool submitOriginalUiImpl(const OriginalUiNativeDraw50KG&, std::int32_t width,
        std::int32_t height, bool sourceRgba, VehicleWindowDiagnostic50DS&,
        bool* sourceWorkStarted=nullptr,bool publishSource=true) noexcept;
    bool submitOriginalUiSourceFrame(const OriginalUiSourceFrame50KM&,std::int32_t width,
        std::int32_t height,bool publishSource,VehicleWindowDiagnostic50DS&) noexcept;
    bool matchesOriginalUiSource(const OriginalUiSourceSubmission50KN&)const noexcept;
    bool matchesOriginalUiByteBlend(const OriginalUiByteBlend50KO&)const noexcept;
    bool matchesOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO&)const noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
