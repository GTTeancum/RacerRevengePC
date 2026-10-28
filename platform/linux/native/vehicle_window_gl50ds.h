#include "../../../src/game/original_options_mutation_interlock50kg.h"
#pragma once
#include "vehicle_draw_gl50cv.h"
#include "../../../src/game/original_racer_diffuse_camera50la.h"
#include "vehicle_shader_gl50cz.h"
#include "material_texture_gl50eb.h"
#include "original_ui_rider_residency50kg.h"
#include "original_ui_native_draw50kg.h"
#include "../../../src/assets/native_asset48b.h"
#include "../../../src/assets/world_normals50hk.h"
#include "../../../src/assets/track_projected_uv.h"
#include "../../../src/assets/vehicle_draw_assets50cr.h"
#include "../../../src/game/draw_visibility50ei.h"
#include "../../../src/game/host_race_limits50ht.h"
#include "../../../src/game/original_gameplay_blur_leaf.h"
#include "../../../src/game/native_gc_selected_lights_candidate.h"
#include "../../../src/game/original_gc_particle_render50kg.h"
#include "../../../src/assets/original_gc_particle_material50kg.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace ra_cold_aggregate50LA { class Owner; }
namespace rr::game { class OriginalResults10450LA; }
namespace rc_crystal50RC { class Owner; }
namespace tb_original_world50LM { class Owner; }
namespace rc_particle_visual50RC { class Window; }
namespace rc_particle_render50RC { class Owner; }
namespace gb_particle_visual50GB { class Window; }
namespace mc_particle_visual50MC { class Window; }
namespace gb_particle_render50GB { class Owner; }
namespace mc_particle_render50MC { class Owner; }
namespace original_main_entry50KG { struct Application; class RaceDisplayLifetime50LA; class NativeF8BarPrefix50LA; class ResultsRendererLease50LA; class NativeResultsFrame50LA; }
namespace ra_static_fire_visual50LA {class Owner;class CompletedDrawReceipt;}
namespace rr::game { struct CameraViewOutput49F; class OriginalRaceHud50KG; class OriginalTimeStats50LA; class OriginalDamage50LA; class OriginalPauseNativeDraw50LA; class OriginalPositionStatsNativeOwner50LA; class OriginalSpeedBoost50LA; struct PodCableMesh50FD; namespace original_gameplay_binder_candidate { struct Mesh; } }
namespace rr::game { class OriginalRacerModelPackets50LA; class OriginalRacerModelRenderGlobals50LA; struct OriginalRaceFogFields; }
namespace rr::platform {
// Precharged common named-payload allowance for the GB binding metadata added
// to the renderer Storage plus its retained Request, one input Request/Frame
// and Diagnostic. Applies on every renderer route, including frontend/legacy.
// This is not a claim about driver/allocator overhead or stack high-water use.
inline constexpr std::size_t gbParticleRendererMetadataReservation50DS=256;
inline constexpr std::size_t mcParticleRendererMetadataReservation50DS=256;
class GameHost50ER;
class OriginalUiGarageDrawContribution50KG;
class OriginalUiSharedDialogs50KG;
struct GameHostDiagnostic50ER;
class OriginalVideoBlockingNativeBackend;
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
// Explicit native replacement of the existing opaque-textured host light.
// This does not claim original material, normal, fog, alpha or specular parity.
enum class NativeRacerLightingPolicy50DS : std::uint8_t {
    unchangedHost, nativeGcSelectedLights
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
    // Bounded primary/actual optional lower-engine SpecularMapDualCompressed base map.
    // Consume CPU-generated original early-bias packed RGB with byte MODULATE
    // and screen-linear Gouraud interpolation. Unpaired output alpha remains 1;
    // the explicitly admitted pair below owns its bounded RGBA producer/consumer.
    // Requires exact source-color mask/light publication on the submitted part.
    bool sourceDiffuseCompressed50LA = false;
    // Cold caller-authenticated dual material, including both original context
    // states and exact CPU/GPU texture identities. Primary left/right bodies
    // and the complete actual five-model pod's optional lower bodies admit this
    // pair: opaque, inclusive depth, base writes / map 2 does not.
    // Each original PSG group writes base RGBA before its matching map 2 reads
    // current destination A; no general gameplay-alpha validity is asserted.
    bool sourceSpecularPair50LA = false;
    bool sourceSingleDiffuseRgba50LA = false, sourceBaseAlphaTest50LA = false;
    std::uint32_t specularTextureIndex50LA = assets::unresolvedMaterialTexture50EA;
    bool sourceBaseFog50LA = false, sourceSpecularFog50LA = false;
    // Optional actual map-2 GREATER/0/KEEP alpha test; base remains ATE0.
    bool sourceSpecularAlphaTest50LA = false;
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
    // Chariot/left/right, then optional bottom-left/bottom-right. One entry for EVERY
    // original scene material (even unused materials), at most 4096 per scene.
    // These arrays are borrowed unchanged during create only, then copied.
    VehicleWindowComponentMaterials50DS component[game::maxVehicleDrawComponents50CM]{};
    std::uint32_t componentCount = game::defaultVehicleDrawComponents50CM;
    // Independently retained immutable second-map publication. Required exactly
    // when this material table contains an authenticated source specular pair.
    const MaterialTextureGl50EB* specularTextures50LA = nullptr;
};
// Explicit native policy, not GS sample parity. Source owners retain the actual
// AA field and map it here only after authentic material admission.
enum class NativeTrackCoverage50DS : std::uint8_t { aa0, fourSampleAa1 };
// Explicit native track-only UV policy; old callers retain the one-pass guard.
// scLavaTwoPass is issued by the authenticated SC lava association owner.
enum class NativeTrackUvPolicy50DS : std::uint8_t { legacySinglePass, scLavaTwoPass };
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
    // Source static-model +148 gate for the existing native shadow policy.
    //Default true retains prior callers; false suppresses receiver shading only.
    bool allowPodShadow = true;
    // Source GREATER/KEEP test after vertex/texture alpha modulation. This
    // distinct mode preserves the older texture-only cutout policy above.
    bool sourceAlphaTest = false;
    std::uint8_t sourceAlphaReference = 0;
    // Actual material controller is serviced at this span's selected draw.
    // The current native adapter admits translation-only UV matrices.
    bool sourceUvAnimation = false;
    // Actual inherited/overridden material fog gate; false preserves the old
    // native opaque/paired-pass eligibility policy.
    bool sourceFogKnown = false, sourceFogEnabled = true;
    // Appended so existing positional aggregates retain their AA0 defaults.
    NativeTrackCoverage50DS nativeCoverage = NativeTrackCoverage50DS::aa0;
    NativeTrackUvPolicy50DS sourceUvPolicy = NativeTrackUvPolicy50DS::legacySinglePass;
    // Issued only by actual RC crystal source association: 1 base,2 Environment.
    std::uint8_t rcCrystalPass = 0;
};
// Pure cold validation of the only newly admitted two-pass UV presentation.
// Authentic source provenance is supplied by sc_lava_uv_fixture::Owner; this
// checks its lowered render values and is also used by renderer preflight.
inline bool validScLavaUvPair50DS(const VehicleWindowTrackSpan50DS& first,
    const VehicleWindowTrackSpan50DS& second) noexcept {
    const auto policy=[](const VehicleWindowTrackSpan50DS& s,bool secondPass) {
        const auto& m=s.material;
        return s.sourceUvPolicy==NativeTrackUvPolicy50DS::scLavaTwoPass &&
            s.sourceUvAnimation && s.sourcePass==(secondPass?2u:1u) && s.sourcePassCount==2 &&
            !s.projectionPass && m.mode==VehicleWindowMaterialMode50DS::requiredTexture &&
            m.textureIndex!=assets::unresolvedMaterialTexture50EA && m.depthWrite && m.depthEqual &&
            m.alphaScale==1.f && !m.unlit && !m.sourcePrelit && !m.sourceDiffuseCompressed50LA &&
            !m.sourceSpecularPair50LA && !m.sourceSingleDiffuseRgba50LA && !m.sourceBaseAlphaTest50LA && !s.discardZeroAlpha && !s.sourceAlphaReference &&
            s.sourceFogKnown && s.sourceFogEnabled && s.nativeCoverage==NativeTrackCoverage50DS::aa0 &&
            m.blend==(secondPass?VehicleWindowBlend50DS::additive:VehicleWindowBlend50DS::opaque) &&
            m.fixedBlendAlpha==secondPass && s.sourceAlphaTest==secondPass;
    };
    return policy(first,false) && policy(second,true) &&
        first.material.textureIndex==second.material.textureIndex &&
        first.indexStart==second.indexStart && first.indexCount==second.indexCount &&
        first.vertexStart==second.vertexStart && first.vertexCount==second.vertexCount &&
        first.indexCount && !(first.indexCount%3) && first.vertexCount;
}
// Closed RC pair policy; authentic source/image ownership is separately checked
// against the retained rc_crystal50RC::Owner before any allocation or GL call.
inline bool validRcCrystalPair50DS(const VehicleWindowTrackSpan50DS& first,
    const VehicleWindowTrackSpan50DS& second) noexcept {
    const auto valid=[](const VehicleWindowTrackSpan50DS& s,std::uint8_t pass){
        const auto& m=s.material;
        return s.rcCrystalPass==pass&&s.sourcePass==pass&&s.sourcePassCount==2&&
            !s.projectionPass&&!s.sourceUvAnimation&&!s.allowPodShadow&&
            s.sourceUvPolicy==NativeTrackUvPolicy50DS::legacySinglePass&&
            s.nativeCoverage==NativeTrackCoverage50DS::aa0&&s.sourceFogKnown&&
            s.sourceFogEnabled==(pass==2)&&s.sourceAlphaTest==(pass==2)&&
            !s.sourceAlphaReference&&!s.discardZeroAlpha&&
            m.mode==VehicleWindowMaterialMode50DS::requiredTexture&&
            m.textureIndex!=assets::unresolvedMaterialTexture50EA&&
            m.blend==VehicleWindowBlend50DS::additive&&!m.depthWrite&&m.depthEqual&&
            m.alphaScale==1.f&&!m.fixedBlendAlpha&&m.unlit==(pass==2)&&m.sourcePrelit==(pass==1)&&
            !m.sourceDiffuseCompressed50LA&&!m.sourceSpecularPair50LA&&!m.sourceSingleDiffuseRgba50LA&&!m.sourceBaseAlphaTest50LA;
    };
    return valid(first,1)&&valid(second,2)&&first.material.textureIndex!=second.material.textureIndex&&
        first.indexStart==second.indexStart&&first.indexCount==second.indexCount&&
        first.vertexStart==second.vertexStart&&first.vertexCount==second.vertexCount&&
        first.indexCount&&!(first.indexCount%3)&&first.vertexCount;
}
// CPU receipt of the last GPU-completed gameplay submission. Span IDs index
// the exact cold VehicleWindowTrack50DS::spans array, including material passes.
// A submitted span issued at least one real indexed draw; this is not a claim
// that it survived fragment clipping/depth/alpha tests or was pixel-visible.
struct VehicleWindowTrackVisibilityReceipt50DS {
    std::uint64_t completedGameplayGeneration = 0;
    std::uint32_t sourceSpanCount = 0;
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
    // Optional bounded four-span native FireP0t publication. No RA admission.
    // Actual renderer and image owners retain private source leases until
    // checked retirement; caller cannot close canonical facets underneath them.
    std::shared_ptr<ra_static_fire_visual50LA::Owner> raStaticFire;
    // Actual RC source/world/lighting lifetime; excludes generic projection.
    // Exact float2 GPU capacity and compact CPU staging are caller charged.
    std::shared_ptr<rc_crystal50RC::Owner> rcCrystal;
    // Same authenticated immutable TB world retained through renderer retirement.
    // Cold validation binds exact mesh, projection, spans and source texture bank.
    std::shared_ptr<const tb_original_world50LM::Owner> tbWorld;
    std::size_t maximumRcProjectionBytes=0,maximumRcStagingBytes=0;
    // Exact RC world/attachment owner, retained through actual GPU retirement.
    // Native per-instance completion policy: private gameplay draw attempts,
    // never a caller-authored scene stamp or source portal-stamp equivalence.
    std::shared_ptr<rc_particle_visual50RC::Window> rcParticleWindow;
    // Exact 55-placement GB static-world/material/complete-span authority.
    std::shared_ptr<gb_particle_visual50GB::Window> gbParticleWindow;
    std::shared_ptr<mc_particle_visual50MC::Window> mcParticleWindow;
};
// Narrow mutable world path for caller-animated authentic scene owners. The
// aggregate retains one fixed source topology/material table across the session.
constexpr std::uint32_t maxVehicleWindowDynamicVertices50DS = 8192;
constexpr std::uint32_t maxVehicleWindowDynamicIndices50DS = 32768;
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
    // Actual display-sealed direct-transfer binding, retained through local HUD uploads.
    std::shared_ptr<const assets::OriginalNativeTextureUploadBinding50LA> originalUploadBinding;
    // Actual retained RA aggregate and its complete immutable 49-image GPU bank.
    // This selects the dedicated byte-material world path, never legacy track
    // shading. Prepare current native lighting/diffuse/fog bindings before submit.
    std::shared_ptr<ra_cold_aggregate50LA::Owner> raStaticWorld;
    const MaterialTextureGl50EB* raStaticWorldTextures = nullptr;

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
    // Cold native displayed-history resources. Only a recovered producer may
    // supply a frame request. Adds three RGBA8 images at the maximum extent.
    // Actual history advances only through completeGameplayDisplay or the
    // original display-fade owner, never through submit/readback alone.
    bool enableOriginalGameplayBlur = false;
    // Optional host wall-time observation of existing display operations only.
    // Disabled performs no clock reads; never changes completion or acceptance.
    bool enableGameplayDisplayWallTiming = false;
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
    // Source-only ordinary gameplay Binder candidate. Exact ten-frame cold
    // source bank, shared immutable images; timing remains CPU-owned.
    // Requires caller-material track program and cable storage. Uses authored
    // world fog by default and honors its explicit diagnostic opt-out.
    const MaterialTextureGl50EB* gameplayBinderTextures = nullptr;
    // Genuine shared GC manager material/atlas. CPU/GPU publication and parsed
    // material stay alive unchanged through close. No UI scene is authorized.
    const assets::MaterialTextureAssets50EA* gcParticleCpuTextures = nullptr;
    const MaterialTextureGl50EB* gcParticleTextures = nullptr;
    const assets::OriginalGcParticleMaterial50KG* gcParticleMaterial = nullptr;
};
// Dedicated frontend context. There is no vehicle/world asset slot and no
// gameplay opt-in; construction cannot certify any race owner as prepared.
// Retains the same authenticated UI CPU/GPU bank and context lifetime as the
// ordinary renderer. Source-RGBA support is independently validated by GL.
struct OriginalFrontendWindowRequest50LA {
    void* context = nullptr;
    VehicleWindowGlProc50DS (*procAddress)(void*, const char*) noexcept = nullptr;
    bool (*isCurrent)(void*) noexcept = nullptr;
    std::int32_t maximumWidth = 0, maximumHeight = 0;
    VehicleWindowOutput50DS output = VehicleWindowOutput50DS::defaultBackBuffer;
    bool enableSourceRgba = false;
    const assets::MaterialTextureAssets50EA* cpuTextures = nullptr;
    const MaterialTextureGl50EB* textures = nullptr;
};
struct VehicleWindowStanding50DS {
    // Exact cold-verified original display name, including its terminator.
    // The native 5x7 font displays ASCII letters in uppercase. No shortening,
    // alternate name, source lookup, or inferred rank happens in the renderer.
    char displayName[maxVehicleWindowRacerName50DS + 1]{};
    std::uint32_t racerIndex = 0, place = 0, completedLaps = 0;
    bool finished = false;
    // Actual native retirement marker, independent of the finish flag. Only
    // a retired, unfinished row is DNF; neither place nor laps implies it.
    std::uint32_t retirementCD4 = 0;
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
    std::uint32_t raceRetirementCD4 = 0;
    // HOST retained-scene result presentation. Native human Course supplies
    // this only after the genuine finish/retirement ending clock, audio wait
    // and displayed fade; original menu/profile transition is not projected.
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
    // presentation bound only. Current-lap seconds are ignored after finish
    // or retirement; elapsed manager time never becomes a DNF finish time.
    bool raceClocksValid = false;
    float raceElapsedSeconds = 0.f, currentLapSeconds = 0.f;
    // Optional complete field, already in the race owner's committed order.
    // Zero count disables it. Otherwise count must equal raceRacerCount and
    // fit the eight-row bound; indices form one permutation and place is 1..N.
    // Completed counts are source counts, never current-lap numbers. Finished
    // rows retain their sealed order; player highlighting uses this explicit ID.
    std::uint32_t standingsCount = 0, standingsPlayerIndex = 0;
    // Native1B0400 ranks can tie and newly sealed1B02F0 flags are published
    //after that rank pass. Render the supplied source rows without imposing
    //the independent HOST observer's permutation/finished-first policy.
    bool standingsSourceRanks = false;
    VehicleWindowStanding50DS standings[maxVehicleWindowStandings50DS]{};
    // Explicit nonblocking Results screen. For a finish the caller supplies
    // the sealed finish place/double seconds, separately from native CC0
    // timing above. A retired, unfinished racer supplies zero finish fields
    // and displays DNF instead. No outcome/place/time is inferred here.
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
    // Borrowed synchronously, then copied by value before GL. Requested
    // packets must be genuine owner publications for real roster slot i+1,
    // where i is this entry's fixed opponent index. Keep the packet paired
    // with its actual retained batch, including stopped opponents.
    // Any nonempty opaque host-lit untextured span rejects before GL; unlit,
    // sourcePrelit and nonopaque spans do not participate in this policy.
    NativeRacerLightingPolicy50DS lightingPolicy = NativeRacerLightingPolicy50DS::unchangedHost;
    const game::NativeGcSelectedLightsCandidate* selectedLights = nullptr;
    // Actual engine-model preparation owners, borrowed only through submit.
    // Indices 0/1 are primary left/right components 1/2; indices 2/3 are actual
    // optional lower left/right components 3/4, null for three-model pods.
    // Keep each owner paired with its own current/retained successful batch.
    // View-dependent distance/flags refresh even for retained stopped meshes.
    std::array<game::OriginalRacerModelPackets50LA*, 4> sourceBodyModelPackets50LA{};
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
// Single-view native join of the recovered manager request at Render3D.
// No host strength, speed, boost or elapsed-render-time input exists here.
struct GameplayBlurFrame50DS {
    game::GameplayBlurPending* pending = nullptr; // Synchronous borrowed producer owner.
    std::int32_t originalWidth = 0, originalHeight = 0;
    std::int32_t logicalView = 0, physicalView = 0; // Current join admits 0/0.
    game::GameplayBlurRect cameraRectangle{}; // Exact original top-left viewport.
};
struct VehicleWindowTrackUvFrame50DS {
    void* owner = nullptr;
    // Synchronous borrowed callback: service this exact cold source-span ID
    // and return its actual controller translation. Never called for culls.
    bool (*service)(void*, std::uint32_t, float& u, float& v) noexcept = nullptr;
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
    // Explicit source world/camera zone selection; keeps cold sky storage alive.
    // Default preserves existing callers. Matrix remains required when hidden.
    bool skyEnabled = true;
    // Borrowed synchronously and copied before any GL work; no frame allocation.
    // Missing entries use identity UV. Max 5*4096 entries; originals unchanged.
    const VehicleWindowMaterialUv50DS* materialUvs = nullptr;
    std::uint32_t materialUvCount = 0;
    // Optional zero-to-seven actual opponents in this ONE private transaction.
    // All batches are validated before drawing. All opaque racers precede track
    // and pod effects; sky, track and HUD are drawn once. Null/zero preserves
    // the original single-racer path. Enabled native footprint shadows use each
    // submitted racer's three primary-model opaque geometries and material bank.
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
    // Player roster slot 0, independently selected for this exact real batch.
    // Requested policy requires a ready packet; unchangedHost requires null.
    // Owner/graphics/publication identity is copied with its active components.
    // Runtime owns the current-batch/packet pairing and invalidation boundary.
    // Every nonempty opaque host-lit span must name its real required texture;
    // unsupported untextured spans reject before GL instead of falling back.
    // Source-generated body RGB additionally requires exact batch metadata
    // matching this packet's owner/publication/graphics sequence/racer slot.
    // A retained stopped batch must therefore keep its matching old packet.
    NativeRacerLightingPolicy50DS lightingPolicy = NativeRacerLightingPolicy50DS::unchangedHost;
    const game::NativeGcSelectedLightsCandidate* selectedLights = nullptr;
    // One live/retained source four-vertex Binder mesh per submitted racer,
    // player first. Borrowed only through submit. Alpha/RGB are source packets.
    const game::original_gameplay_binder_candidate::Mesh* gameplayBinders = nullptr;
    std::uint32_t gameplayBinderCount = 0, gameplayBinderFrame = 0;
    // Null is for non-gameplay/legacy paths: no queue consumption or effect.
    // No-request queue retains the video owner's previous coefficient/rectangle.
    const GameplayBlurFrame50DS* gameplayBlur = nullptr;
    const VehicleWindowTrackUvFrame50DS* trackUvAnimation = nullptr;
    //Actual shared manager packets, borrowed through this one transaction;
    //all5000 slots can draw. Drawn after Binder, before HUD/display blur.
    const game::OriginalGcParticleDraw50KG* gcParticles = nullptr;
    // Optional genuine single-view lap text/gleam owner bound by the method
    // below. Native gauges/countdown remain. Unsupported live states skip
    // only this layer; no empty batch counts as an original GPU submission.
    const game::OriginalRaceHud50KG* originalRaceHud = nullptr;
    const game::OriginalTimeStats50LA* originalTimeStats = nullptr;
    const game::OriginalDamage50LA* originalDamage = nullptr;
    // Explicit derived/ordinary outer-tail batch; genuine Pause scheduling required.
    const game::OriginalPauseNativeDraw50LA* originalPauseDerived = nullptr;
    const game::OriginalPositionStatsNativeOwner50LA* originalPositionStats = nullptr;
    const game::OriginalSpeedBoost50LA* originalSpeedBoost = nullptr;
    // Current successful source camera publication, including stopped racers.
    // The converted combined matrix must equal worldViewProjection exactly at
    // this viewport. Copy actual view/position before any frame GL work.
    const game::CameraViewOutput49F* sourceBodyCamera50LA = nullptr;
    const game::OriginalRacerDiffuseCamera50LA* sourceDiffuseCamera50LA = nullptr;
    std::uintptr_t sourceBodyCameraOwner50LA = 0;
    std::uint64_t sourceBodyCameraPublication50LA = 0;
    // SAME session source fog and1A6AA0/2410E0 shared threshold owners.
    // Each actual engine model owns its radius/distance/flag preparation;
    // opponent descriptors supply their corresponding existing model owners.
    const game::OriginalRaceFogFields* sourceBodyFog50LA = nullptr;
    const game::OriginalRacerModelRenderGlobals50LA* sourceBodyGlobals50LA = nullptr;
    // Indices 0/1 are primary left/right components 1/2; indices 2/3 are actual
    // optional lower left/right components 3/4, null for three-model pods.
    // Borrow only through submit, paired with each component's successful batch.
    std::array<game::OriginalRacerModelPackets50LA*, 4> sourceBodyModelPackets50LA{};
    // Actual RC numeric owner, accepted only within its private withDraw
    // borrow and the same Window's canonical material/atlas callback.
    // It lends no caller-authored OriginalGcParticleDraw receipt.
    std::shared_ptr<rc_particle_render50RC::Owner> rcParticles;
    std::shared_ptr<gb_particle_render50GB::Owner> gbParticles;
    std::shared_ptr<mc_particle_render50MC::Owner> mcParticles;
    // Optional synchronous native F8 bars/icon/text borrow. Only its actual private
    // Application factory issues this batch. No140230/199500/FIFO completion
    // or live end-flow admission follows from a successful native submission.
    const ::original_main_entry50KG::NativeF8BarPrefix50LA* nativeF8BarPrefix = nullptr;
    const ::original_main_entry50KG::NativeResultsFrame50LA* nativeResults = nullptr;
};
enum class VehicleWindowError50DS : std::uint8_t {
    none, alreadyOwned, request, assets, allocation, entryPoint, context,
    shader, limits, gl, indices, layout, framebuffer, notInitialized,
    wrongThread, notCurrent, frame, submission, presentation, cleanup, material,
    noImage, readback
};
// One attempted operation, including failures. Valid wall time is not a GPU
// duration, successful completion receipt, or physical scanout measurement.
struct VehicleWindowWallSample50DS {
    bool attempted = false, valid = false;
    std::uint64_t wallNs = 0;
};
struct VehicleWindowDiagnostic50DS {
    VehicleWindowError50DS error = VehicleWindowError50DS::none;
    const char* operation = nullptr; // Static; first failure wins.
    std::uint32_t component = game::maxVehicleDrawComponents50CM, glError = 0, cleanupGlError = 0, framebufferStatus = 0;
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
    std::uint32_t gameplayBinderDraws = 0;
    std::uint32_t gcParticleDraws = 0,gcParticleAlphaDraws = 0,gcParticleQuads = 0;
    std::uint32_t gameplayBlurDraws = 0;
    // Checked native bar commands only; require batchAccepted for publication.
    // Icon/text native commands also do not complete a source callback.
    std::uint32_t nativeF8BarQuads = 0,nativeF8SuffixQuads = 0,nativeF8SuffixPackets = 0;
    bool nativeF8BarSuffixPending = false;
    std::uint8_t gameplayBlurFixed = 0;
    bool gameplayBlurRequestConsumed = false;
    // Actual successful native swap submission or completed headless frame,
    // distinct from batch acceptance and physical display/scanout claims.
    bool gameplayDisplayHistoryAccepted = false, gameplayWindowSwapSubmitted = false;
    bool gameplayHeadlessDisplayCompleted = false;
    std::uint64_t gameplayDisplayHistoryGeneration = 0;
    // Nested components of completeGameplayDisplay, not disjoint frame phases.
    // History includes its existing error check; finish excludes its following
    // check; swap includes host validation. Headless swap remains unattempted.
    VehicleWindowWallSample50DS gameplayHistoryStageWall{}, gameplayFinishWall{}, gameplaySwapWall{};
    // Checked effect commands, included in trackDraws when track effects.
    // Source order per pass, not reconstructed original global sorting.
    std::uint32_t transparentTrackDraws = 0, transparentPodDraws = 0;
    std::uint32_t transparentPodIndices = 0, skyDraws = 0, skyIndices = 0;
    // Explicit finishSubmittedWork or source-owner retirement: glFinish
    // returned with no reported GL error.
    // This is not a measured duration, presented swap, or hardware-GPU claim.
    bool submittedWorkCompleted = false;
    bool contextWorkCompleted = false;
    std::uint64_t contextDrainGeneration = 0;
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
    std::uint32_t originalRaceHudQuads = 0, originalRaceHudLeaves = 0;
    bool originalRaceHudRejected = false;
    std::uint32_t originalTimeStatsQuads=0,originalTimeStatsLeaves=0;
    bool originalTimeStatsRejected=false;
    std::uint32_t originalPositionStatsQuads=0,originalPositionStatsLeaves=0;
    bool originalPositionStatsRejected=false;
    std::uint32_t originalDamageQuads=0,originalDamageLeaves=0;
    bool originalDamageRejected=false;
    std::uint32_t originalPauseDerivedQuads=0,originalPauseDerivedLeaves=0;
    bool originalPauseDerivedRejected=false;
    std::uint32_t originalSpeedBoostPrimitives=0,originalSpeedBoostLeaves=0;
    bool originalSpeedBoostRejected=false;
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
    // Explicit dormant PRIVATE opaque publication only, never a source/page
    // acceptance, window back-buffer copy, host swap or display receipt.
    bool originalUiPrivatePublicationAccepted = false;
    std::uint64_t originalUiPrivatePublicationGeneration = 0;
    std::uint64_t originalUiPromotedIncomingGeneration = 0;
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
    // Checked source-pair commands, including player and opponents. These are
    // not visible-fragment counts or alpha-validity claims; later failure can
    // still abort the complete frame before the existing publication point.
    std::uint32_t sourceSpecularBaseGroups50LA = 0, sourceSpecularSecondTriangles50LA = 0;
    std::uint32_t sourceSpecularSkippedGroups50LA = 0;
    // RC source-selected triangle work after ordinary frustum/chunk culling.
    // Compositions are issued GL calls, never surviving-fragment counts.
    // Wall time is CPU submit time only, measured iff the existing opt-in is on.
    std::uint32_t rcCrystalTriangles = 0, rcCrystalEmptyTriangles = 0;
    std::uint32_t rcCrystalCompositionsIssued = 0, rcCrystalFullRectangles = 0;
    std::uint64_t rcCrystalSubmissionWallUs = 0;
    bool rcCrystalWallMeasured = false;
    // Actual RC particle kernel counters; no GC source-identity claim.
    std::uint32_t rcParticleDraws = 0,rcParticleAlphaDraws = 0,rcParticleQuads = 0;
    std::uint32_t gbParticleDraws = 0,gbParticleAlphaDraws = 0,gbParticleQuads = 0;
    std::uint32_t mcParticleDraws = 0,mcParticleAlphaDraws = 0,mcParticleQuads = 0;
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

enum class OriginalUiPrivatePublicationKind50KP : std::uint8_t {
    none, byteBlendSample, immutableIncoming
};
// A separate opaque RGB copy selected for renderer readback. Completion refers
// only to that copy. Immutable lineage fields observe the original source;
// they never mint a new source submission or certify a transition schedule.
// A later sample or blend retirement does not overwrite this independent copy.
// Any later successful image publication invalidates this publication receipt.
// No default framebuffer operation or host display/swap is represented here.
class OriginalUiPrivatePublication50KP final {
public:
    OriginalUiPrivatePublicationKind50KP kind()const noexcept{return kind_;}
    std::uint64_t generation()const noexcept{return generation_;}
    std::uint64_t blendGeneration()const noexcept{return blendGeneration_;}
    std::uint64_t sampleGeneration()const noexcept{return sampleGeneration_;}
    std::uint64_t oldSnapshotGeneration()const noexcept{return oldSnapshotGeneration_;}
    std::uint64_t incomingFrameGeneration()const noexcept{return incomingFrameGeneration_;}
    std::uint64_t incomingTargetGeneration()const noexcept{return incomingTargetGeneration_;}
    // Meaningful only for byteBlendSample; immutableIncoming has no effect alpha.
    std::uint32_t alpha()const noexcept{return alpha_;}
    std::int32_t width()const noexcept{return width_;}
    std::int32_t height()const noexcept{return height_;}
    bool completed()const noexcept{return completed_;}
private:
    friend class VehicleWindowGl50DS;
    std::weak_ptr<const std::uint8_t> lifetime_;
    OriginalUiPrivatePublicationKind50KP kind_=OriginalUiPrivatePublicationKind50KP::none;
    std::uint64_t generation_=0,blendGeneration_=0,sampleGeneration_=0,oldSnapshotGeneration_=0;
    std::uint64_t incomingFrameGeneration_=0,incomingTargetGeneration_=0;
    std::uint32_t alpha_=0;
    std::int32_t width_=0,height_=0;
    bool completed_=false;
};
enum class OriginalUiWindowBackBufferCopyKind : std::uint8_t {
    none, windowBack
};
// Historical renderer-issued observation of an accepted default BACK blit.
// The copied publication records the exact prior completed PRIVATE image and
// its lineage; that completion does not cover this later window copy. Neither
// this value nor its nonwrapping sequence is a current permission to swap: the
// renderer cannot observe host swaps or intervening window writes. No window
// copy completion, swap submission, presentation event or scanout is certified.
// Retaining this value pins neither GPU pixels nor the renderer's lifetime.
class OriginalUiWindowBackBufferCopy final {
public:
    OriginalUiWindowBackBufferCopyKind kind()const noexcept{return kind_;}
    std::uint64_t sequence()const noexcept{return sequence_;}
    bool commandsAccepted()const noexcept{return commandsAccepted_;}
    const OriginalUiPrivatePublication50KP& publication()const noexcept{return publication_;}
    std::int32_t width()const noexcept{return publication_.width();}
    std::int32_t height()const noexcept{return publication_.height();}
private:
    friend class VehicleWindowGl50DS;
    std::weak_ptr<const std::uint8_t> lifetime_;
    OriginalUiPrivatePublication50KP publication_;
    std::uint64_t sequence_=0;
    OriginalUiWindowBackBufferCopyKind kind_=OriginalUiWindowBackBufferCopyKind::none;
    bool commandsAccepted_=false;
};
// Immutable full incoming RGBA ownership transferred out of the blend, not a
// new submission into the reusable ordinary source target. Its original source
// receipt remains a historical observation with the existing freshness rules.
// This raw image survives later opaque publications and blend retirement until
// explicit promoted-image release/close. Destroying the value does not release.
class OriginalUiPromotedIncoming50KP final {
public:
    std::uint64_t generation()const noexcept{return generation_;}
    const OriginalUiSourceSubmission50KN& source()const noexcept{return source_;}
private:
    friend class VehicleWindowGl50DS;
    OriginalUiSourceSubmission50KN source_;
    std::uint64_t generation_=0;
};

// Caller-owned desktop GL >=3.2. Window output requires a double-buffered,
// nonstereo, nonmultisampled window surface; headless output does not.
// Owns GPU storage; never creates/binds/releases a context or polls.
// submit never swaps. The opt-in completeGameplayDisplay uses the exact supplied
// GameHost for the real swap, with transactional completed-image history.
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
// and swap between calls are allowed for legacy paths. Opted-in gameplay uses
// completeGameplayDisplay for every race/menu swap. No other code may mutate these private
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
    // Authentic original UI only. All gameplay submission/completion entry
    // points reject this owner. Course must destroy it before race cold load.
    bool createOriginalFrontend(const OriginalFrontendWindowRequest50LA&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Accepts actual, successful 50CO batches from the exact retained 50CR
    // prepared meshes. Original scene material indices remain validated even
    // in explicit neutral mode. A matrix/size/structural rejection does not blit.
    bool submit(const game::VehicleDrawBatch50CO&, const VehicleWindowFrame50DS&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Optional cold join after actual viewport construction. Borrows the
    // concrete CPU owner through renderer.close; uploads only its real four
    // atlases. Failure keeps native gameplay/HUD usable and retains any partial
    // GL allocations for renderer.close. A repeated bind is not a retry API.
    bool bindOriginalRaceHud(const game::OriginalRaceHud50KG&,VehicleWindowDiagnostic50DS&) noexcept;
    // Strong canonical TimeStats owner retained through checked GPU cleanup.
    // Two real atlases; requested invalid frames reject the frame transaction.
    // Retains the typed presentation adapter through real GPU retirement.
    // The adapter weak-borrows the display-pinned genuine resource owner.
    bool bindOriginalPositionStats(std::shared_ptr<game::OriginalPositionStatsNativeOwner50LA>,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Retains original derived Pause resources through checked GPU cleanup.
    bool retireOriginalPauseDerived(VehicleWindowDiagnostic50DS&) noexcept;
    bool bindOriginalPauseDerived(std::shared_ptr<game::OriginalPauseNativeDraw50LA>,VehicleWindowDiagnostic50DS&) noexcept;
    // Retains the detached Damage canonical owner through checked GPU cleanup.
    // One authentic dmgradar PST; no replacement material or player identity.
    bool bindOriginalDamage(std::shared_ptr<game::OriginalDamage50LA>,VehicleWindowDiagnostic50DS&) noexcept;
    bool bindOriginalTimeStats(std::shared_ptr<game::OriginalTimeStats50LA>,VehicleWindowDiagnostic50DS&) noexcept;
    bool bindOriginalSpeedBoost(std::shared_ptr<game::OriginalSpeedBoost50LA>,VehicleWindowDiagnostic50DS&) noexcept;
    // Copy admitted shared scene fog after its real owner updates it. Requires
    // initialized/current idle renderer outside a display fade. CPU-only:
    // existing draw uniforms consume these values on the next real submit.
    // No image/publication/completion change, source recomputation or GL call.
    bool updateWorldFog(const VehicleWindowWorldFog50DS&,VehicleWindowDiagnostic50DS&) noexcept;
    // Ordered authenticated original quads, separate RGBA8/D24 transaction.
    // Does not advance gameplay or replace the retained race publication.
    bool submitOriginalUi(const OriginalUiNativeDraw50KG&, std::int32_t width,
        std::int32_t height, VehicleWindowDiagnostic50DS&) noexcept;
    // Complete sealed Rider/Options frame, fresh source RGBA, then opaque presentation.
    // Ordinary submitOriginalUi retains its existing RGB-only contract.
    bool submitOriginalUiSourceRgba(const OriginalUiSourceFrame50KM&, std::int32_t width,
        std::int32_t height, VehicleWindowDiagnostic50DS&) noexcept;
    bool originalUiSourceRgbaAvailable() const noexcept;
    bool prepareOriginalUiDirectVideo(OriginalUiDirectVideoCapability50KG&,VehicleWindowDiagnostic50DS&) noexcept;
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
    // Copies a completed current sample into the independent opaque private UI
    // target. No window copy occurs, even for a defaultBackBuffer renderer.
    // Letterbox RGB comes from the incoming source clear captured at begin.
    // It neither renders another sample nor changes source/page acceptance.
    bool publishOriginalUiByteBlendSample(const OriginalUiByteBlend50KO&,
        const OriginalUiByteBlendSample50KO&,std::int32_t width,std::int32_t height,
        OriginalUiPrivatePublication50KP&,VehicleWindowDiagnostic50DS&) noexcept;
    // Copies the ENTIRE immutable incoming image, including every border, to
    // opaque publication; then transfers its unchanged completed raw RGBA names
    // to a separate promoted owner. No alpha128 sample or ordinary source draw
    // is substituted. A successful transfer invalidates all blend/sample use;
    // explicit blend release still retires effect names and unlocks old.
    // Before success a failed copy preserves incoming ownership and the lease,
    // issues neither output, and leaves the prior private publication intact.
    // One promoted raw owner at a time; release it before another promotion.
    bool promoteOriginalUiByteBlendIncoming(const OriginalUiByteBlend50KO&,
        std::int32_t width,std::int32_t height,OriginalUiPrivatePublication50KP&,
        OriginalUiPromotedIncoming50KP&,VehicleWindowDiagnostic50DS&) noexcept;
    // Private publication acceptance and actual checked glFinish are separate.
    // Completion failure invalidates its publication authority; it never drops
    // the independently owned raw incoming image or fabricates a host receipt.
    bool completeOriginalUiPrivatePublication(const OriginalUiPrivatePublication50KP&,
        OriginalUiPrivatePublication50KP&,VehicleWindowDiagnostic50DS&) noexcept;
    // Dormant bridge: exact current completed PRIVATE publication to window
    // BACK at its recorded viewport. Requires window output and the exact live
    // context/drawable/thread. The host must separately verify live drawable
    // dimensions still equal this viewport; no live-size getter exists here.
    // No private publication, sample, source, completion barrier or swap occurs.
    // On false, do not swap: defaultFramebufferMayBeModified preserves possible
    // partial BACK writes. Private/raw ownership and completion stay unchanged.
    // Input and output must be distinct, including output.publication(). An
    // alias is rejected before clearing the output; other failures clear it.
    // Success issues only the historical command-acceptance observation above.
    bool copyOriginalUiPrivatePublicationToWindow(const OriginalUiPrivatePublication50KP&,
        OriginalUiWindowBackBufferCopy&,VehicleWindowDiagnostic50DS&) noexcept;
    // Current completed opaque publication, output viewport RGBA, alpha255.
    bool readOriginalUiPrivatePublicationRgba(const OriginalUiPrivatePublication50KP&,
        std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS&) noexcept;
    // Full original incoming640x448 RGBA, including original alpha and borders.
    // Raw completion came from begin's actual copy, independently of opaque
    // publication completion. No current ordinary source or composer is needed.
    bool readPromotedOriginalUiIncomingRgba(const OriginalUiPromotedIncoming50KP&,
        std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS&) noexcept;
    // Completes outstanding reads/copies before deletion. Failure preserves
    // unresolved names for retry/close, with the same no-name-reuse rule below.
    bool releasePromotedOriginalUiIncoming(VehicleWindowDiagnostic50DS&) noexcept;
    // Also retires partial/failed begin or sample work. Checked glFinish and
    // successful name deletion precede unlocking retained old. On failure,
    // keep the renderer/context alive and retry release/close. After ambiguous
    // deletion, no owner may create/reuse GL names until retirement succeeds.
    bool releaseOriginalUiByteBlend(VehicleWindowDiagnostic50DS&) noexcept;
    // Explicit destination-lifetime retirement, while this concrete composer
    // is still alive. Requires the live context/thread and an idle renderer;
    // rejects a different retained owner, drains all preceding GL work even
    // after a failed/unpublished attempt, then releases the generation borrow.
    // On failure the owner must stay alive for retry. A reconstructed UI
    // destination can then begin its own nonzero generation sequence safely.
    bool retireOriginalUiSourceOwner(const OriginalUiSourceOwner50KM&,
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
    // Frozen completed race image plus the bound authentic Pause frame. Exact
    // retained extent only; caller completes display through completeGameplayDisplay.
    bool submitRetainedOriginalPause(const game::OriginalPauseNativeDraw50LA&,
        std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS&) noexcept;
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
    // Window evidence only: read GL_FRONT of the actual default framebuffer
    // immediately after the caller's successful swap. No private-target fallback
    // and no presentation/scanout guarantee beyond that real application buffer.
    // Same tight bottom-first RGBA storage contract as readbackRgba. Requires
    // matching current drawable dimensions and an idle defaultBackBuffer owner.
    bool readbackWindowFrontRgba(std::uint8_t* rgba, std::size_t capacityBytes,
        std::int32_t drawableWidth, std::int32_t drawableHeight,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Pre-swap evidence only: exact current GameHost-owned default GL_BACK.
    // The caller must pair these bytes with that image's successful swap before
    // publishing them. Never read BACK after swap or call this a scanout capture.
    bool readbackWindowBackRgba(GameHost50ER&,std::uint8_t* rgba,std::size_t capacityBytes,
        std::int32_t drawableWidth,std::int32_t drawableHeight,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Explicit caller-controlled completion barrier for already submitted work,
    // e.g. bounded headless completed-frame timing. Requires an existing private
    // committed image and the exact live context/thread; calls glFinish, checks
    // errors, and sets diagnostic.submittedWorkCompleted only on success. No
    // allocation, readback, swap, image publication change, or implicit timing.
    // Normal submit/window paths never call this barrier automatically.
    bool finishSubmittedWork(VehicleWindowDiagnostic50DS&) noexcept;
    // Optional source/GPU-storage provenance of the retained accepted RA frame. Call at
    // capture selection, after excluding capture work from FPS measurement.
    // RR_RA_PIXEL_PROVENANCE=1 enables a bounded log and six fog buffer reads; no draws.
    // Reports conservative candidates, never final GPU pixel ownership.
    void logRaCapturePixelProvenance() noexcept;
    // Current GPU-completed native FireP0t publication. No caller supplies
    // span bits, generation or completion evidence; no particle activation.
    bool completedRaStaticFire(ra_static_fire_visual50LA::CompletedDrawReceipt&,
        VehicleWindowDiagnostic50DS&) const noexcept;
    // Copy requested source-span submitted bits (0/1) from the last completed
    // gameplay render. No guessed culling result, GL work, allocation, mutation
    // or new receipt. Before the first completion, returns noImage. Menus/fades
    // never publish a gameplay generation. Caller maps these exact cold span
    // indices to its retained original world-instance identities.
    bool completedTrackSpanVisibility(const std::uint32_t* sourceSpanIndices,
        std::uint8_t* submitted,std::uint32_t count,
        VehicleWindowTrackVisibilityReceipt50DS&,VehicleWindowDiagnostic50DS&) const noexcept;
    // Complete the latest successful native race/host-menu publication. The
    // exact renderer GameHost is required. Stages immutable final history,
    // completes GPU work and submits the real window swap (headless: completes
    // without swapping), then accepts history with no later fallible action.
    // Call instead of host.swapWindow for this cold opt-in. Failed work never
    // advances history. Repeated completion without a new publication rejects.
    bool completeGameplayDisplay(GameHost50ER&, GameHostDiagnostic50ER&,
        VehicleWindowDiagnostic50DS&) noexcept;
    // Original manager destruction clears all four strength bytes. Retains
    // actual displayed image and rectangle, including across a race restart.
    bool clearGameplayBlurStrengths(VehicleWindowDiagnostic50DS&) noexcept;
    bool finishInitializedContextWork(VehicleWindowDiagnostic50DS&) noexcept;
    // Failed cleanup retains unresolved ownership for a retry. If cleanup is
    // complete the instance is empty even when stale GL errors were reported.
    bool close(VehicleWindowDiagnostic50DS&) noexcept;
    bool initialized() const noexcept;
    // Inline observation keeps GameHost's lifetime guard usable by host-only
    // binaries without introducing a renderer implementation link dependency.
    bool ownsResources() const noexcept { return bool(storage_); }
    // Read-only observation of the actual private pins, not a readiness or
    // completion receipt. Same owner-thread/no-reentrancy contract as close.
    bool hasSessionDependents50LA() const noexcept { return sessionDependentPins50LA_!=0; }
private:
    friend struct ::original_main_entry50KG::Application;
    friend class ::original_main_entry50KG::ResultsRendererLease50LA;
    bool retireNativeResultsResources50LA(VehicleWindowDiagnostic50DS&)noexcept;
    // Uncalled native copied-image upload only; no Frame/draw admission.
    bool prepareNativeResultsResources50LA(const game::OriginalResults10450LA&,
        VehicleWindowDiagnostic50DS&)noexcept;
    // Actual native scratch allocation only, still uncalled by Application.
    bool prepareNativeResultsDrawResources50LA(VehicleWindowDiagnostic50DS&)noexcept;
    bool prepareNativeF8BarResources50LA(VehicleWindowDiagnostic50DS&)noexcept;
    bool prepareNativeF8SuffixResources50LA(const ::original_main_entry50KG::NativeF8BarPrefix50LA&,
        VehicleWindowDiagnostic50DS&)noexcept;
    friend class OriginalUiSharedDialogs50KG;
    bool applyMenuRasterPrelude50KG(OriginalUiSharedDialogs50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool menuRasterPreludeCurrent50KG(const OriginalUiSharedDialogs50KG&)const noexcept;
    bool sealMenuRasterSourceFrame50KG(const OriginalUiSharedDialogs50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool acceptMenuRasterSource50KG(const OriginalUiSharedDialogs50KG&)noexcept;
    bool menuRasterRetired50KG(const OriginalUiSharedDialogs50KG&)const noexcept;
    friend class OriginalUiGarageRendererLease50KG;
    friend class OriginalUiGarageDrawContribution50KG;
    bool pollOriginalRenderer245030(bool& complete,VehicleWindowDiagnostic50DS&)noexcept;
    bool completeGarageSourceTarget140A38(OriginalUiGarageDrawContribution50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool publishGarageSourceTarget140A68(OriginalUiGarageDrawContribution50KG&,std::int32_t,std::int32_t,VehicleWindowDiagnostic50DS&)noexcept;
    bool rearmGarageSourceTarget140230(OriginalUiGarageDrawContribution50KG&,VehicleWindowDiagnostic50DS&)noexcept;

    bool applyGarageFogColor244A40(const OriginalUiSourceOwner50KM&,std::uint64_t,std::uint32_t,VehicleWindowDiagnostic50DS&)noexcept;
    bool clearGarageSourceViewport50KG(const OriginalUiSourceOwner50KM&,std::uint64_t,
        bool color,std::uint32_t argb,VehicleWindowDiagnostic50DS&)noexcept;
    bool releaseGarageSourcePrelude50KG(const OriginalUiSourceOwner50KM&,std::uint64_t,
        VehicleWindowDiagnostic50DS&)noexcept;
    bool ownsGarageSourcePrelude50KG(const OriginalUiSourceOwner50KM&,std::uint64_t)const noexcept;
    bool garageUnsealedRenderQuiescent50KG(const OriginalUiGarageDrawContribution50KG&,
        std::uint64_t)const noexcept;
    friend class OriginalUiFrontend50KG;
    bool optionsViewportReservedCurrent50KG(const OriginalOptionsMutationInterlock50KG&)const noexcept;
    bool optionsViewportIdle50KG(VehicleWindowDiagnostic50DS&)const noexcept;
    bool finishOptionsViewportWork50KG(const OriginalOptionsMutationInterlock50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool prevalidateRiderTextures(const OriginalUiRiderResidencyDemand50KG&,
        const MaterialTextureGl50EB&,OriginalUiRiderTextureValidation50KG&,VehicleWindowDiagnostic50DS&) noexcept;
    bool commitRiderTextures(const OriginalUiRiderResidencyDemand50KG&,
        OriginalUiRiderTextureValidation50KG&,VehicleWindowDiagnostic50DS&) noexcept;
    friend class ::original_main_entry50KG::RaceDisplayLifetime50LA;
    // Private original-producer observation, not a new render/upload path.
    bool raSkySourceCurrent129280(const assets::Mesh48B&,
        const assets::MaterialTextureAssets50EA&,const void* actualDisplay,
        VehicleWindowDiagnostic50DS&)const noexcept;
    // Actual session owner alone admits the source reset between submissions.
    bool idleForRaceReset50LA(VehicleWindowDiagnostic50DS&) const noexcept;
    // First canonical frontend Camera only. Ordinary sealed RGBA submission
    // does not execute source26F940 or select a canonical Scene.
    bool garageFirstCameraViewportIdle50KG(VehicleWindowDiagnostic50DS&) const noexcept;
    // Private session authority: callers cannot mint a pin or authorize creation.
    const void* sessionDisplayOwner50LA_=nullptr;
    bool sessionDisplayCreate50LA_=false;
    bool sessionDisplayFrontendCreate50KG_=false;
    // Native frontend no-error domain. No source C4/frame/process history.
    bool frontendLocalizationIdle50KG(VehicleWindowDiagnostic50DS&) const noexcept;
    std::uint32_t sessionDependentPins50LA_=0;
    // Only the concrete optional GameHost adapter may supply these bindings.
    bool completeGameplayDisplayImpl(GameHost50ER&, GameHostDiagnostic50ER&,
        bool (*checkHost)(GameHost50ER&, bool, std::int32_t, std::int32_t, VehicleWindowDiagnostic50DS&) noexcept,
        bool (*swapHost)(GameHost50ER&, GameHostDiagnostic50ER&) noexcept, VehicleWindowDiagnostic50DS&) noexcept;
    friend class OriginalVideoBlockingNativeBackend;
    bool submitOriginalUiImpl(const OriginalUiNativeDraw50KG&, std::int32_t width,
        std::int32_t height, bool sourceRgba, VehicleWindowDiagnostic50DS&,
        bool* sourceWorkStarted=nullptr,bool publishSource=true,bool riderReceiptChecked=false,
        const OriginalUiDirectVideo50KG* directVideo=nullptr,bool garageBatch=false) noexcept;
    bool completeGarageCollector24C160(OriginalUiGarageDrawContribution50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool submitGarageCollectorBatch24B26C(OriginalUiGarageDrawContribution50KG&,VehicleWindowDiagnostic50DS&)noexcept;
    bool submitOriginalUiSourceFrame(const OriginalUiSourceFrame50KM&,std::int32_t width,
        std::int32_t height,bool publishSource,VehicleWindowDiagnostic50DS&) noexcept;
    bool matchesOriginalUiSource(const OriginalUiSourceSubmission50KN&)const noexcept;
    bool matchesOriginalUiByteBlend(const OriginalUiByteBlend50KO&)const noexcept;
    bool matchesOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO&)const noexcept;
    bool matchesOriginalUiPrivatePublication(const OriginalUiPrivatePublication50KP&)const noexcept;
    bool publishOriginalUiBlendImage(const OriginalUiByteBlend50KO&,
        const OriginalUiByteBlendSample50KO*,std::int32_t width,std::int32_t height,
        OriginalUiPrivatePublication50KP&,VehicleWindowDiagnostic50DS&) noexcept;
    bool readbackWindowBufferRgba(std::uint8_t*,std::size_t,std::int32_t,std::int32_t,
        bool front,const GameHost50ER* expectedHost,VehicleWindowDiagnostic50DS&) noexcept;
    struct Storage;
    std::unique_ptr<Storage> storage_;
};
}
