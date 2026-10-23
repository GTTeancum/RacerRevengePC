#include "original_ui_garage_draw_contribution50kg.h"
#include "../../../tests/native/original_native_collector_fifo50la.h"
#include "original_ui_garage_canonical_submission50kg.h"
struct __GLsync; // OpenGL opaque sync type, never a native-dereferenced pointer.
#include "../../../tests/native/ra_static_fire_visual50la.h"
#include "vehicle_window_gl50ds.h"
#include "original_ui_coverage_bounds50kg.h"
#include "original_ui_coverage_batch50kg.h"
#include "original_ui_coverage_disjoint50kg.h"
#include "../../../tests/native/rc_crystal50rc.h"
#include "../../../tests/native/tb_original_world50lm.h"
#include "../../../tests/native/rc_particle_visual50rc.h"
#include "../../../tests/native/rc_particle_render50rc.h"
#include "../../../tests/native/gb_particle_visual50gb.h"
#include "../../../tests/native/mc_particle_visual50mc.h"
#include "../../../tests/native/gb_particle_render50gb.h"
#include "../../../tests/native/mc_particle_render50mc.h"
#include "../../../tests/native/ra_cold_aggregate50la.h"
#include "../../../tests/native/ra_static_fog_numeric50la.h"
#include "native_camera_view50dx.h"
#include "../../../src/game/original_racer_specular50la.h"
#include "../../../src/game/original_racer_diffuse50la.h"
#include "../../../src/game/original_racer_model_packets50la.h"
#include "../../../src/game/original_camera_start_effects.h"
#include "original_gameplay_blur_native_pass.h"
#include "original_video_blocking_native.h"
#include "original_ui_rider_draw_contribution50kg.h"
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
#include "native_pod_shadow_glsl50hm.h"
#include "../../../src/game/pod_cables50fd.h"
#include "../../../src/game/original_gameplay_binder_candidate.h"
#include <algorithm>
#include <chrono>
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
namespace {
using U = std::uint32_t;
using I = std::int32_t;
constexpr U arrayBuffer = 0x8892, elementBuffer = 0x8893;
constexpr U bufferSize = 0x8764, bufferUsage = 0x8765, dynamicDraw = 0x88E8, staticDraw = 0x88E4;
constexpr U framebuffer = 0x8D40, readFramebuffer = 0x8CA8, drawFramebuffer = 0x8CA9;
constexpr U framebufferComplete = 0x8CD5, backBuffer = 0x0405;
constexpr U renderbuffer = 0x8D41, colorAttachment = 0x8CE0, depthAttachment = 0x8D00;
constexpr U rgba8 = 0x8058, depth24 = 0x81A6, unsignedByte = 0x1401, floatType = 0x1406;
constexpr U rgba = 0x1908, pixelPackBuffer = 0x88EB;
constexpr U colorBufferBit = 0x00004000, depthBufferBit = 0x00000100;
constexpr U texture2D = 0x0DE1, textureUnit0 = 0x84C0;
// Keep the CPU capacity tied to the deliberately unrolled GLSL contract.
static_assert(maxVehicleWindowOpponents50DS == 7, "update explicit shadow caster slots when racer capacity changes");
constexpr U nativeShadowRacerCount = 1 + maxVehicleWindowOpponents50DS;
// Existing native shadow approximation retains its three primary-model casters.
// Optional models are rendered normally; no additional shadow parity is claimed.
constexpr U nativeShadowCasterCount = 3 * nativeShadowRacerCount;
constexpr U maxComponents = game::maxVehicleDrawComponents50CM;
// Bounded body slots: primary left/right, then actual optional lower left/right.
// Body slot N maps only to component N + 1; chariot has no source body slot.
constexpr U sourceBodyCount50LA = 4;
static_assert(maxComponents == 5, "source body slots require the exact five-component topology");
constexpr game::VehicleDrawComponent50CM components[] = {
    game::VehicleDrawComponent50CM::chariot, game::VehicleDrawComponent50CM::leftEngine,
    game::VehicleDrawComponent50CM::rightEngine, game::VehicleDrawComponent50CM::bottomLeftEngine,
    game::VehicleDrawComponent50CM::bottomRightEngine
};
bool fail(VehicleWindowDiagnostic50DS& d, VehicleWindowError50DS error,
    const char* operation) noexcept {
    if (d.error == VehicleWindowError50DS::none) { d.error = error; d.operation = operation; }
    return false;
}
// Local observation only: no GL calls, logger, test dependency or simulation clock.
class GameplayDisplayWallScope50DS final {
    using Clock = std::chrono::steady_clock;
    VehicleWindowWallSample50DS* sample_ = nullptr;
    Clock::time_point before_{};
public:
    GameplayDisplayWallScope50DS(bool enabled,VehicleWindowWallSample50DS& sample) noexcept {
        if(enabled){sample_=&sample;sample.attempted=true;before_=Clock::now();}
    }
    ~GameplayDisplayWallScope50DS() noexcept {
        if(!sample_)return;
        const auto after=Clock::now();
        if(after<before_)return;
        // Bound before integer conversion, including implementations with a
        // wider steady-clock duration. An invalid sample never changes success.
        const auto ns=std::chrono::duration<long double,std::nano>(after-before_).count();
        if(!(ns>=0 && ns<18446744073709551616.0L))return;
        sample_->wallNs=static_cast<std::uint64_t>(ns);sample_->valid=true;
    }
    GameplayDisplayWallScope50DS(const GameplayDisplayWallScope50DS&)=delete;
    GameplayDisplayWallScope50DS& operator=(const GameplayDisplayWallScope50DS&)=delete;
};
U componentIndex(game::VehicleDrawComponent50CM component) noexcept {
    for (U c = 0; c < maxComponents; ++c) if (components[c] == component) return c;
    return maxComponents;
}
bool sourceBodyComponent50LA(U component, U componentCount) noexcept {
    // The generic draw interface also accepts four components. Source body
    // admission requires a complete three-model pod or actual five-model pod.
    return (componentCount == 3 || componentCount == 5) &&
        component >= 1 && component <= sourceBodyCount50LA && component < componentCount;
}
U trackSpanChunkCount(U indexCount, bool textured, U maximumTriangles) noexcept {
    if (!textured || !maximumTriangles) return 1;
    const U triangles = indexCount / 3;
    return triangles / maximumTriangles + (triangles % maximumTriangles != 0);
}
bool validateMaterialPolicy(const VehicleWindowMaterialBinding50DS& material,
    VehicleWindowDiagnostic50DS& d) noexcept {
    if ((material.blend != VehicleWindowBlend50DS::opaque &&
        material.blend != VehicleWindowBlend50DS::sourceAlpha &&
        material.blend != VehicleWindowBlend50DS::additive) ||
        !std::isfinite(material.alphaScale) || material.alphaScale < 0.f || material.alphaScale > 8.f ||
        (material.fixedBlendAlpha && material.blend != VehicleWindowBlend50DS::additive) ||
        (material.blend == VehicleWindowBlend50DS::opaque && material.alphaScale != 1.f))
        return fail(d, VehicleWindowError50DS::material, "invalid explicit blend/alpha material policy");
    if (material.sourceDiffuseCompressed50LA &&
        (material.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
         material.blend != VehicleWindowBlend50DS::opaque || material.unlit ||
         material.sourcePrelit || material.fixedBlendAlpha || material.alphaScale != 1.f))
        return fail(d, VehicleWindowError50DS::material,
            "source compressed diffuse requires a lit opaque base-map texture policy");
    if (material.sourceSpecularPair50LA) {
        if (!material.sourceDiffuseCompressed50LA || material.sourceSingleDiffuseRgba50LA || material.sourceBaseAlphaTest50LA || !material.depthWrite || !material.depthEqual ||
            material.specularTextureIndex50LA == assets::unresolvedMaterialTexture50EA)
            return fail(d, VehicleWindowError50DS::material,
                "source specular pair requires source diffuse, real map 2 and inclusive writing base depth");
    } else if (material.sourceSingleDiffuseRgba50LA) {
        if (!material.sourceDiffuseCompressed50LA || !material.depthWrite || !material.depthEqual ||
            material.specularTextureIndex50LA != assets::unresolvedMaterialTexture50EA ||
            material.sourceSpecularFog50LA || material.sourceSpecularAlphaTest50LA)
            return fail(d,VehicleWindowError50DS::material,"single diffuse requires its own source RGBA depth contract without map 2");
    } else if (material.specularTextureIndex50LA != assets::unresolvedMaterialTexture50EA ||
        material.sourceBaseFog50LA || material.sourceBaseAlphaTest50LA || material.sourceSpecularFog50LA || material.sourceSpecularAlphaTest50LA)
        return fail(d, VehicleWindowError50DS::material, "unpaired material must not carry specular state");
    return true;
}
bool validateMaterials(const VehicleWindowRequest50DS& request,
    const assets::VehicleDrawAssets50CR& owner, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& input = request.materials;
    if (input.componentCount != owner.componentCount())
        return fail(d, VehicleWindowError50DS::material, "material component count must match actual assets");
    if (request.mode == VehicleWindowMode50DS::viewerNeutralUntextured) {
        if (input.textures || input.specularTextures50LA)
            return fail(d, VehicleWindowError50DS::request, "neutral mode must not supply a texture bank");
        for (const auto& component : input.component)
            if (component.bindings || component.count)
                return fail(d, VehicleWindowError50DS::request, "neutral mode must not supply material bindings");
        return true;
    }
    if (!input.textures || !input.textures->validBorrow(request.context, request.isCurrent,
        input.textures->publicationToken()))
        return fail(d, VehicleWindowError50DS::material,
            "published texture bank with exact current context/callback/thread required");
    if (input.specularTextures50LA && !input.specularTextures50LA->validBorrow(
        request.context, request.isCurrent, input.specularTextures50LA->publicationToken()))
        return fail(d, VehicleWindowError50DS::material,
            "map-2 publication requires the same current context, callback and thread");
    bool anyPair = false;
    for (U c = 0; c < owner.componentCount(); ++c) {
        d.component = c;
        d.material = d.texture = assets::unresolvedMaterialTexture50EA;
        const auto* scene = owner.scene(components[c]);
        const auto& component = input.component[c];
        if (!scene || scene->materials.empty() || scene->materials.size() > maxVehicleWindowMaterials50DS ||
            component.count != scene->materials.size() || !component.bindings)
            return fail(d, VehicleWindowError50DS::material,
                "one explicit binding per original scene material within 4096-entry cap required");
        for (U m = 0; m < component.count; ++m) {
            d.material = m;
            const auto& binding = component.bindings[m];
            d.texture = binding.textureIndex;
            if (!validateMaterialPolicy(binding, d)) return false;
            if (binding.sourceDiffuseCompressed50LA &&
                !sourceBodyComponent50LA(c, owner.componentCount()))
                return fail(d, VehicleWindowError50DS::material,
                    "source compressed diffuse requires actual engine components in a three/five-model pod");
            if (binding.sourceSpecularPair50LA) {
                if (!sourceBodyComponent50LA(c, owner.componentCount()))
                    return fail(d, VehicleWindowError50DS::material,
                        "source specular pair requires actual engine components in a three/five-model pod");
                anyPair = true;
                const auto* map2 = input.specularTextures50LA ?
                    input.specularTextures50LA->asset(binding.specularTextureIndex50LA) : nullptr;
                if (!map2 || !map2->name)
                    return fail(d, VehicleWindowError50DS::material, "source pair has no actual published map 2");
            }
            if (binding.mode == VehicleWindowMaterialMode50DS::untextured) {
                if (binding.textureIndex != assets::unresolvedMaterialTexture50EA)
                    return fail(d, VehicleWindowError50DS::material,
                        "explicit untextured material must not name a texture");
            } else if (binding.mode == VehicleWindowMaterialMode50DS::requiredTexture) {
                const auto* texture = input.textures->asset(binding.textureIndex);
                if (!texture || !texture->name)
                    return fail(d, VehicleWindowError50DS::material, "required material texture is not published");
            } else {
                return fail(d, VehicleWindowError50DS::material, "material mode must be explicitly selected");
            }
        }
    }
    if (anyPair != bool(input.specularTextures50LA))
        return fail(d, VehicleWindowError50DS::material, "map-2 bank presence differs from source pair table");
    d.component = maxComponents;
    d.material = d.texture = assets::unresolvedMaterialTexture50EA;
    return true;
}
// This checks live canonical state without touching create-only span arrays.
bool currentRaStaticFire(const VehicleWindowTrack50DS& track,VehicleWindowDiagnostic50DS& d) noexcept {
    if(!track.raStaticFire)return true;
    if(!track.textures||track.textures->raStaticFireOwner()!=track.raStaticFire.get())
        return fail(d,VehicleWindowError50DS::material,"FireP0t exact image publication lost");
    constexpr std::int32_t fields[]={1,0,255,255,0,0,1,0,1,128,0,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,0,1,6,0,0,0,1,2,0};
    constexpr std::int32_t sampler[]={0,0,1,5,0,-75};
    for(std::size_t i=0;i<4;++i){
        assets::OriginalStandardMaterialMapBindingView50LA map;
        assets::OriginalStandardMaterialDiagnostic50LA md;
        if(!track.raStaticFire->currentMap(i,map,md))
            return fail(d,VehicleWindowError50DS::material,"FireP0t canonical map/collector expired");
        if(map.contextAfter249610.fields.size()!=sizeof(fields)/sizeof(fields[0]))
            return fail(d,VehicleWindowError50DS::material,"FireP0t unsupported canonical field extent");
        for(std::size_t j=0;j<map.contextAfter249610.fields.size();++j)
            if(map.contextAfter249610.fields[j].value!=fields[j])
                return fail(d,VehicleWindowError50DS::material,"FireP0t unsupported canonical raster state");
        for(std::size_t j=0;j<6;++j)if(map.contextAfter249610.sampler[j].value!=sampler[j])
            return fail(d,VehicleWindowError50DS::material,"FireP0t canonical sampler changed");
    }
    return true;
}
bool validateRaStaticFire(const VehicleWindowTrack50DS& track,VehicleWindowDiagnostic50DS& d) noexcept {
    if(!track.raStaticFire){
        if(track.textures&&track.textures->raStaticFireOwner())
            return fail(d,VehicleWindowError50DS::material,"FireP0t image bank requires its typed track publication");
        return true;
    }
    if(track.mesh!=track.raStaticFire->mesh()||!track.mesh||
       track.mode!=VehicleWindowMode50DS::callerMaterialTextures||track.vertexColorScale!=1.f||
       track.spanCount!=4||!track.spans||track.sourceNormals||track.sourceNormalCount||
       track.sourceProjectionUvs||track.sourceProjectionCount)
        return fail(d,VehicleWindowError50DS::material,"FireP0t requires exact source mesh/four spans/authored RGB and UV");
    if(!currentRaStaticFire(track,d))return false;
    for(std::size_t i=0;i<4;++i){
        const auto* source=track.raStaticFire->span(i);const auto& span=track.spans[i];const auto& m=span.material;
        if(!source||source->vertexStart!=span.vertexStart||source->vertexCount!=span.vertexCount||
           source->indexStart!=span.indexStart||source->indexCount!=span.indexCount||
           m.mode!=VehicleWindowMaterialMode50DS::requiredTexture||m.textureIndex!=0||
           m.blend!=VehicleWindowBlend50DS::opaque||!m.depthWrite||!m.depthEqual||m.alphaScale!=1.f||
           m.fixedBlendAlpha||m.unlit||!m.sourcePrelit||m.sourceDiffuseCompressed50LA||m.sourceSpecularPair50LA||
           span.sourcePass!=1||span.sourcePassCount!=1||span.projectionPass||span.discardZeroAlpha||
           !span.sourceAlphaTest||span.sourceAlphaReference||span.sourceUvAnimation||
           !span.sourceFogKnown||!span.sourceFogEnabled)
            return fail(d,VehicleWindowError50DS::material,"FireP0t source span or exact canonical lowering differs");
    }
    return true;
}
bool validateTbWorld(const VehicleWindowRequest50DS& request,VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& track=request.track;if(!track.tbWorld)return true;
    const auto& owner=*track.tbWorld;const auto source=owner.sourceOwner();
    const auto* spans=owner.spans();const auto* selections=owner.selections();
    const auto* normals=owner.normals();const auto* projected=owner.projectionUvs();const auto* bank=owner.bank();
    if(!owner.loaded()||!source||!source->ready()||request.raStaticWorld||track.rcCrystal||
       track.raStaticFire||track.rcParticleWindow||track.gbParticleWindow||track.mcParticleWindow||track.mode!=VehicleWindowMode50DS::callerMaterialTextures||
       track.mesh!=owner.mesh()||!track.mesh||track.mesh->vertices.size()!=151225u||
       track.mesh->indices.size()!=286893u||!spans||spans->size()!=1376u||!selections||
       !normals||normals->size()!=track.mesh->vertices.size()||track.sourceNormals!=normals->data()||
       track.sourceNormalCount!=normals->size()||!projected||projected->size()!=track.mesh->vertices.size()||
       track.sourceProjectionUvs!=projected->data()||track.sourceProjectionCount!=projected->size()||
       !track.spans||track.spanCount!=spans->size()||track.vertexColorScale!=255.f/128.f||!bank||!track.textures||
       !track.textures->matchesSourceBank(*bank,track.textures->publicationToken())||track.textures->textureCount()!=103u)
        return fail(d,VehicleWindowError50DS::request,"TB retained world/source publication differs");
    std::size_t crowd=0;
    for(std::size_t i=0;i<spans->size();++i){const auto& a=(*spans)[i];const auto& b=track.spans[i];
        if(a.materialSelection>=selections->size()||a.vertexStart!=b.vertexStart||a.vertexCount!=b.vertexCount||
           a.indexStart!=b.indexStart||a.indexCount!=b.indexCount||a.textureIndex!=b.material.textureIndex||
           a.sourcePass!=b.sourcePass||a.sourcePassCount!=b.sourcePassCount||a.projectionPass!=b.projectionPass||
           b.rcCrystalPass||b.material.mode!=VehicleWindowMaterialMode50DS::requiredTexture)
            return fail(d,VehicleWindowError50DS::material,"TB ordered source span association differs");
        if(pod_materials50ed_fixture::detail::normalized((*selections)[a.materialSelection].materialName)=="CROWD"){
            if(b.nativeCoverage!=NativeTrackCoverage50DS::fourSampleAa1)
                return fail(d,VehicleWindowError50DS::material,"TB original CROWD AA1 lowering missing");
            ++crowd;
        }
    }
    if(crowd!=33u)return fail(d,VehicleWindowError50DS::material,"TB original CROWD span count differs");
    for(U i=0;i<103u;++i){const auto* cpu=bank->texture(i);const auto* gpu=track.textures->asset(i);
        if(!cpu||!gpu||!gpu->name||cpu->identity!=gpu->identity)
            return fail(d,VehicleWindowError50DS::material,"TB source image identity differs");
    }
    return true;
}
bool validateTrack(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    if(!validateRaStaticFire(request.track,d)||!validateTbWorld(request,d))return false;
    const auto& track = request.track;
    const auto& fog = track.worldFog;
    if(track.rcCrystal){
        const std::uint64_t gpu=track.mesh?std::uint64_t(track.mesh->vertices.size())*sizeof(std::array<float,2>):0;
        if(!track.rcCrystal->validate(track)||!gpu||gpu>track.maximumRcProjectionBytes||
           gpu>std::uint64_t(std::numeric_limits<std::intptr_t>::max())||
           rc_crystal50RC::Owner::uvCount*sizeof(std::array<float,2>)>track.maximumRcStagingBytes)
            return fail(d,VehicleWindowError50DS::request,"RC crystal source association/projection budget rejected");
    }else if(track.maximumRcProjectionBytes||track.maximumRcStagingBytes)
        return fail(d,VehicleWindowError50DS::request,"RC projection budgets require typed source owner");
    if (fog.enabled) {
        if (!track.mesh || track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
            !std::isfinite(fog.start) || !std::isfinite(fog.end) ||
            fog.start < 0.f || fog.end <= fog.start)
            return fail(d, VehicleWindowError50DS::request, "world fog requires textured world and finite ordered range");
        for (float value : fog.color)
            if (!std::isfinite(value) || value < 0.f || value > 1.f)
                return fail(d, VehicleWindowError50DS::request, "world fog color outside finite [0,1]");
    }
    if (request.enableNativePodShadows && (!track.mesh ||
        (track.mode != VehicleWindowMode50DS::sourceVertexColors &&
         track.mode != VehicleWindowMode50DS::callerMaterialTextures)))
        return fail(d, VehicleWindowError50DS::request, "native pod shadows require actual source-color/textured track");
    if (!track.mesh)
        return (!track.maximumVertexBytes && !track.maximumIndexBytes &&
            track.mode == VehicleWindowMode50DS::unspecified && track.vertexColorScale == 1.f &&
            !track.textures && !track.spans && !track.spanCount && !track.enableFrustumCulling &&
            !track.cullingChunkTriangles && !track.sourceNormals && !track.sourceNormalCount &&
            !track.maximumNormalBytes && !track.sourceProjectionUvs && !track.sourceProjectionCount &&
            !track.maximumProjectionBytes && !track.rcCrystal && !track.tbWorld && !track.maximumRcProjectionBytes && !track.maximumRcStagingBytes && !track.rcParticleWindow && !track.gbParticleWindow && !track.mcParticleWindow) ||
            fail(d, VehicleWindowError50DS::request, "track mode/budgets/culling require an actual visual mesh");
    if ((track.mode != VehicleWindowMode50DS::viewerNeutralUntextured &&
            track.mode != VehicleWindowMode50DS::sourceVertexColors &&
            track.mode != VehicleWindowMode50DS::callerMaterialTextures) ||
        !track.maximumVertexBytes || !track.maximumIndexBytes)
        return fail(d, VehicleWindowError50DS::request, "track requires explicit shading mode and byte budgets");
    if (track.cullingChunkTriangles &&
        (!track.enableFrustumCulling || track.mode != VehicleWindowMode50DS::callerMaterialTextures))
        return fail(d, VehicleWindowError50DS::request,
            "track triangle chunks require textured track mode and enabled frustum culling");
    if (!std::isfinite(track.vertexColorScale) || track.vertexColorScale <= 0.f ||
        track.vertexColorScale > 8.f ||
        (track.mode == VehicleWindowMode50DS::viewerNeutralUntextured && track.vertexColorScale != 1.f))
        return fail(d, VehicleWindowError50DS::request, "track color scale requires source RGB mode and finite (0,8]");
    const auto& mesh = *track.mesh;
    if (mesh.vertices.empty() || mesh.vertices.size() > assets::max_mesh_vertices ||
        mesh.indices.empty() || mesh.indices.size() > assets::max_mesh_indices || mesh.indices.size() % 3 ||
        mesh.indices.size() > static_cast<std::size_t>(std::numeric_limits<I>::max()))
        return fail(d, VehicleWindowError50DS::assets, "track must be a bounded nonempty Mesh48B triangle list");
    const std::uint64_t vertexBytes = std::uint64_t(mesh.vertices.size()) * sizeof(assets::Vertex48B);
    const std::uint64_t indexBytes = std::uint64_t(mesh.indices.size()) * sizeof(U);
    if (vertexBytes > track.maximumVertexBytes || indexBytes > track.maximumIndexBytes ||
        vertexBytes > std::numeric_limits<std::size_t>::max() ||
        indexBytes > std::numeric_limits<std::size_t>::max() ||
        vertexBytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()) ||
        indexBytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
        return fail(d, VehicleWindowError50DS::limits, "track exceeds explicit vertex/index byte budgets or GL size limits");
    if (track.sourceNormals) {
        const std::uint64_t normalBytes = std::uint64_t(track.sourceNormalCount) * sizeof(assets::WorldNormal50HK);
        if (track.mode == VehicleWindowMode50DS::viewerNeutralUntextured ||
            track.sourceNormalCount != mesh.vertices.size() || !track.maximumNormalBytes ||
            normalBytes > track.maximumNormalBytes ||
            normalBytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
            return fail(d, VehicleWindowError50DS::limits, "source normals require source RGB mode, exact vertex count and byte budget");
        for (U n = 0; n < track.sourceNormalCount; ++n) {
            const auto& v = track.sourceNormals[n].xyz;
            const double length = std::hypot(v[0], v[1], v[2]);
            if (!std::isfinite(length) || (length != 0.0 && std::abs(length - 1.0) > 0.0001))
                return fail(d, VehicleWindowError50DS::assets, "source normals must be finite unit vectors or explicit zero");
        }
    } else if (track.sourceNormalCount || track.maximumNormalBytes) {
        return fail(d, VehicleWindowError50DS::request, "normal count/budget require an explicit source stream");
    }
    if (track.sourceProjectionUvs) {
        const std::uint64_t bytes = std::uint64_t(track.sourceProjectionCount) * sizeof(assets::TrackProjectedUv);
        if (track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
            track.sourceProjectionCount != mesh.vertices.size() || !track.maximumProjectionBytes ||
            bytes > track.maximumProjectionBytes ||
            bytes > std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
            return fail(d, VehicleWindowError50DS::limits, "projected UV stream requires exact vertex count and explicit byte budget");
        for (U v = 0; v < track.sourceProjectionCount; ++v)
            for (float uv : track.sourceProjectionUvs[v].uv)
                if (!std::isfinite(uv))
                    return fail(d, VehicleWindowError50DS::assets, "nonfinite authored projected UV");
    } else if (track.sourceProjectionCount || track.maximumProjectionBytes) {
        return fail(d, VehicleWindowError50DS::request, "projected UV count/budget require a source stream");
    }
    // Mesh48B can be built or changed after parsing. Recheck all geometry
    // before allocating even the renderer owner; do not trust parser history.
    for (const auto& vertex : mesh.vertices) {
        for (float value : vertex.position)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::assets, "track contains nonfinite world position");
        for (float value : vertex.uv)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::assets, "track contains nonfinite UV");
    }
    for (U index : mesh.indices)
        if (index >= mesh.vertices.size())
            return fail(d, VehicleWindowError50DS::indices, "track index outside vertex array");
    if (track.mode == VehicleWindowMode50DS::callerMaterialTextures) {
        if(track.textures&&track.textures->raStaticWorldOwner())
            return fail(d,VehicleWindowError50DS::material,
                "full RA image publication requires pending typed world renderer/light join");
        if (!track.textures || !track.textures->validBorrow(request.context, request.isCurrent,
            track.textures->publicationToken()) || !track.spans || !track.spanCount ||
            track.spanCount > maxVehicleWindowTrackSpans50DS)
            return fail(d, VehicleWindowError50DS::material,
                "track textures require current published bank and 1..65536 explicit source spans");
        U nextIndex = 0, nextVertex = 0;
        for (U i = 0; i < track.spanCount; ++i) {
            d.trackSpan = i;
            const auto& span = track.spans[i];
            d.texture = span.material.textureIndex;
            if (span.nativeCoverage != NativeTrackCoverage50DS::aa0 &&
                span.nativeCoverage != NativeTrackCoverage50DS::fourSampleAa1)
                return fail(d, VehicleWindowError50DS::material, "unknown native track coverage policy");
            if (!validateMaterialPolicy(span.material, d)) return false;
            if(span.rcCrystalPass && (!track.rcCrystal || span.rcCrystalPass>2))
                return fail(d,VehicleWindowError50DS::material,"RC crystal pass lacks actual typed source owner");
            if (span.material.sourceDiffuseCompressed50LA)
                return fail(d, VehicleWindowError50DS::material,
                    "track, dynamic-world and sky spans cannot select racer source diffuse");
            if (!span.sourcePass || !span.sourcePassCount || span.sourcePassCount > 2 ||
                span.sourcePass > span.sourcePassCount || span.projectionPass > 2 ||
                (span.projectionPass && (!track.sourceProjectionUvs ||
                    span.projectionPass != span.sourcePass)))
                return fail(d, VehicleWindowError50DS::material, "invalid original track pass/projection binding");
            if (span.sourcePass == 1) {
                if ((i && track.spans[i-1].sourcePass != track.spans[i-1].sourcePassCount) ||
                    span.indexStart != nextIndex || span.vertexStart != nextVertex ||
                    !span.indexCount || span.indexCount % 3 || !span.vertexCount ||
                    span.indexCount > mesh.indices.size() - nextIndex ||
                    span.vertexCount > mesh.vertices.size() - nextVertex)
                    return fail(d, VehicleWindowError50DS::indices,
                        "first material passes must partition source mesh arrays in order");
                nextIndex += span.indexCount;
                nextVertex += span.vertexCount;
            } else {
                if (!i) return fail(d, VehicleWindowError50DS::indices, "material continuation without first pass");
                const auto& prior = track.spans[i-1];
                if (span.sourcePass != prior.sourcePass+1 || span.sourcePassCount != prior.sourcePassCount ||
                    span.indexStart != prior.indexStart || span.indexCount != prior.indexCount ||
                    span.vertexStart != prior.vertexStart || span.vertexCount != prior.vertexCount)
                    return fail(d, VehicleWindowError50DS::indices, "material passes must repeat the exact preceding source surface");
            }
            for (U n = span.indexStart; n < span.indexStart+span.indexCount; ++n)
                if (mesh.indices[n] < span.vertexStart || mesh.indices[n] >= span.vertexStart+span.vertexCount)
                    return fail(d, VehicleWindowError50DS::indices,
                        "track span index escapes its declared source vertex range");
            if (span.sourceUvPolicy != NativeTrackUvPolicy50DS::legacySinglePass &&
                span.sourceUvPolicy != NativeTrackUvPolicy50DS::scLavaTwoPass)
                return fail(d,VehicleWindowError50DS::material,"unknown native track UV policy");
            const bool scLava=span.sourceUvPolicy==NativeTrackUvPolicy50DS::scLavaTwoPass;
            if ((span.discardZeroAlpha && span.sourceAlphaTest) ||
                (!scLava && span.sourceUvAnimation && (span.sourcePass != 1 || span.sourcePassCount != 1 ||
                    span.projectionPass || span.material.mode != VehicleWindowMaterialMode50DS::requiredTexture)))
                return fail(d,VehicleWindowError50DS::material,
                    "source UV service requires one textured unprojected pass and one alpha-test mode");
            if(scLava) {
                // Both policies are checked before any GL allocation/publication.
                const U first=span.sourcePass==1?i:i-1;
                if(first+1>=track.spanCount ||
                    !validScLavaUvPair50DS(track.spans[first],track.spans[first+1]))
                    return fail(d,VehicleWindowError50DS::material,"SC lava exact adjacent two-pass UV/material pair differs");
            }
            if (span.material.mode == VehicleWindowMaterialMode50DS::untextured) {
                if (span.material.textureIndex != assets::unresolvedMaterialTexture50EA || span.discardZeroAlpha)
                    return fail(d, VehicleWindowError50DS::material,
                        "untextured track span must retain unresolved texture sentinel");
            } else if (span.material.mode == VehicleWindowMaterialMode50DS::requiredTexture) {
                const auto* texture = track.textures->asset(span.material.textureIndex);
                if (!texture || !texture->name)
                    return fail(d, VehicleWindowError50DS::material,
                        "required track span texture is not published");
            } else {
                return fail(d, VehicleWindowError50DS::material, "track span shading must be explicit");
            }
        }
        if (track.spans[track.spanCount-1].sourcePass != track.spans[track.spanCount-1].sourcePassCount ||
            nextIndex != mesh.indices.size() || nextVertex != mesh.vertices.size())
            return fail(d, VehicleWindowError50DS::indices, "track spans must cover the complete source mesh");
        if (track.cullingChunkTriangles) {
            // Keep the complete original source validation above. Bound the
            // separate candidate table before renderer allocation or GL work.
            std::uint64_t chunks = 0;
            for (U i = 0; i < track.spanCount; ++i) {
                const auto& span = track.spans[i];
                chunks += trackSpanChunkCount(span.indexCount,
                    span.material.mode == VehicleWindowMaterialMode50DS::requiredTexture,
                    track.cullingChunkTriangles);
                if (chunks > maxVehicleWindowTrackChunks50DS) {
                    d.trackSpan = i;
                    d.texture = span.material.textureIndex;
                    return fail(d, VehicleWindowError50DS::limits,
                        "track triangle chunks exceed bounded 65536-candidate capacity");
                }
            }
        }
        d.trackSpan = d.texture = assets::unresolvedMaterialTexture50EA;
    } else if (track.textures || track.spans || track.spanCount) {
        return fail(d, VehicleWindowError50DS::request,
            "neutral/source-color track must not supply texture bank or material spans");
    }
    // Serialized/display bounds are unused. Optional host culling derives its
    // own bounds from the actual indexed world-space geometry after validation.
    return true;
}
bool validateTrackAnimations(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    if (!request.trackAnimationCount)
        return (!request.trackAnimations && !request.trackAnimationTextures) ||
            fail(d, VehicleWindowError50DS::request, "animation bank/descriptors require a nonzero count");
    if (request.trackAnimationCount > maxVehicleWindowTrackAnimations50DS ||
        !request.trackAnimations || !request.trackAnimationTextures ||
        request.track.mode != VehicleWindowMode50DS::callerMaterialTextures ||
        !request.trackAnimationTextures->validBorrow(request.context, request.isCurrent,
            request.trackAnimationTextures->publicationToken()))
        return fail(d, VehicleWindowError50DS::material, "track animation requires a bounded published source frame bank");
    for (U i = 0; i < request.trackAnimationCount; ++i) {
        const auto& animation = request.trackAnimations[i];
        if (!animation.frameCount || animation.frameCount > maxVehicleWindowAnimationFrames50DS ||
            animation.firstTextureIndex > request.trackAnimationTextures->textureCount() ||
            animation.frameCount > request.trackAnimationTextures->textureCount()-animation.firstTextureIndex)
            return fail(d, VehicleWindowError50DS::limits, "track animation frame range escapes actual published bank");
        for (U previous = 0; previous < i; ++previous)
            if (request.trackAnimations[previous].sourceSpanIndex == animation.sourceSpanIndex)
                return fail(d, VehicleWindowError50DS::material, "duplicate placed animation span association");
        if (animation.sourceSpanIndex >= request.track.spanCount)
            return fail(d, VehicleWindowError50DS::material, "animated source span outside original track table");
        const auto& span = request.track.spans[animation.sourceSpanIndex];
        if(span.rcCrystalPass)
            return fail(d,VehicleWindowError50DS::material,"RC crystal retains its actual static images; ATM override rejected");
        if(span.sourceUvPolicy==NativeTrackUvPolicy50DS::scLavaTwoPass)
            return fail(d,VehicleWindowError50DS::material,"SC lava retains its static original image; ATM override rejected");
        if (span.material.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
            span.material.textureIndex != animation.sourceTextureIndex)
            return fail(d, VehicleWindowError50DS::material, "animated source span texture identity mismatch");
        for (U frame = 0; frame < animation.frameCount; ++frame) {
            const auto* image = request.trackAnimationTextures->asset(animation.firstTextureIndex+frame);
            if (!image || !image->name)
                return fail(d, VehicleWindowError50DS::material, "original animation frame is not published");
        }
    }
    return true;
}
bool validateDynamicWorld(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    auto selected = request;
    selected.track = request.dynamicWorld;
    selected.enableNativePodShadows = false;
    const auto& world = selected.track;
    if (world.mesh && (world.mode != VehicleWindowMode50DS::callerMaterialTextures ||
        world.mesh->vertices.size() > maxVehicleWindowDynamicVertices50DS ||
        world.mesh->indices.size() > maxVehicleWindowDynamicIndices50DS ||
        world.spanCount > maxVehicleWindowDynamicSpans50DS || world.enableFrustumCulling ||
        world.cullingChunkTriangles || world.sourceProjectionUvs ||
        world.sourceProjectionCount || world.maximumProjectionBytes || world.rcCrystal || world.tbWorld ||
        world.maximumRcProjectionBytes || world.maximumRcStagingBytes || world.rcParticleWindow || world.gbParticleWindow || world.mcParticleWindow))
        return fail(d, VehicleWindowError50DS::limits,
            "dynamic world requires bounded textured topology without static culling/projection");
    const bool result = validateTrack(selected, d);
    if(result)for(U i=0;i<world.spanCount;++i)if(world.spans[i].sourceUvAnimation||world.spans[i].rcCrystalPass)
        return fail(d,VehicleWindowError50DS::material,"dynamic UV/crystal controller requires a separate owner binding");
    d.dynamicWorldSpan = d.trackSpan;
    d.trackSpan = assets::unresolvedMaterialTexture50EA;
    return result;
}
bool validateSky(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& sky = request.sky;
    if (!sky.mesh)
        return (!sky.maximumVertexBytes && !sky.maximumIndexBytes && !sky.textures &&
            !sky.spans && !sky.spanCount) ||
            fail(d, VehicleWindowError50DS::request, "sky fields require an actual authored sky mesh");
    VehicleWindowRequest50DS check = request;
    check.track = {};
    check.track.mesh = sky.mesh;
    check.track.maximumVertexBytes = sky.maximumVertexBytes;
    check.track.maximumIndexBytes = sky.maximumIndexBytes;
    check.track.mode = VehicleWindowMode50DS::callerMaterialTextures;
    check.track.textures = sky.textures;
    check.track.spans = sky.spans;
    check.track.spanCount = sky.spanCount;
    if (!validateTrack(check, d)) return false;
    for (U i = 0; i < sky.spanCount; ++i) {
        const auto& material = sky.spans[i].material;
        if (material.blend != VehicleWindowBlend50DS::opaque || material.depthWrite ||
            !material.unlit || sky.spans[i].discardZeroAlpha || sky.spans[i].sourceAlphaTest ||
            sky.spans[i].sourceUvAnimation || sky.spans[i].sourcePass != 1 ||
            sky.spans[i].sourcePassCount != 1 || sky.spans[i].projectionPass)
            return fail(d, VehicleWindowError50DS::material,
                "sky requires explicit unlit opaque non-depth-writing materials");
    }
    return true;
}
bool validateCables(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& cables = request.cables;
    if (!cables.textures)
        return cables.textureIndex == assets::unresolvedMaterialTexture50EA ||
            fail(d, VehicleWindowError50DS::request, "cable texture index requires an actual texture bank");
    d.texture = cables.textureIndex;
    if (!cables.textures->validBorrow(request.context, request.isCurrent,
        cables.textures->publicationToken()))
        return fail(d, VehicleWindowError50DS::material,
            "cables require an exact current published texture bank");
    const auto* texture = cables.textures->asset(cables.textureIndex);
    if (!texture || !texture->name)
        return fail(d, VehicleWindowError50DS::material, "required actual cable texture is not published");
    d.texture = assets::unresolvedMaterialTexture50EA;
    return true;
}
U cableIndex50DS(U index) noexcept {
    // Canonical 50FD strip topology, up to four nine-sample cables, two triangles
    // per adjacent sample pair. No topology is generated from visual guesses.
    constexpr U offsets[] = {0, 1, 2, 2, 1, 3};
    const U trianglePair = index / 6;
    return (trianglePair / 8) * 18 + (trianglePair % 8) * 2 + offsets[index % 6];
}
bool validateCableFrame(const game::PodCableMesh50FD& mesh, VehicleWindowDiagnostic50DS& d) noexcept {
    static_assert(game::maxPodCableVertexCount50FD == 72 && game::maxPodCableIndexCount50FD == 192,
        "50DS fixed storage requires the admitted 50FD four-cable capacity");
    if (mesh.cableCount != 2 && mesh.cableCount != 4)
        return fail(d, VehicleWindowError50DS::frame, "cable count is outside the admitted source topology");
    const U vertexCount = mesh.cableCount * game::podCableVerticesPerCable50FD;
    const U indexCount = mesh.cableCount * game::podCableIndicesPerCable50FD;
    for (U i = 0; i < vertexCount; ++i) {
        const auto& vertex = mesh.vertices[i];
        for (float value : vertex.position)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::frame, "cable vertex contains nonfinite world position");
        for (float value : vertex.uv)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::frame, "cable vertex contains nonfinite source UV");
    }
    for (U i = 0; i < indexCount; ++i)
        if (mesh.indices[i] != cableIndex50DS(i))
            return fail(d, VehicleWindowError50DS::indices, "cable topology differs from canonical source strips");
    return true;
}

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
constexpr U hostHudMaximumQuads = 8192;
static_assert(maxVehicleWindowStandings50DS <= 9, "standings rank glyph requires a single-digit field");
constexpr std::uint64_t hudGlyph(U a, U b, U c, U d, U e, U f, U g) noexcept {
    return std::uint64_t(a) | (std::uint64_t(b) << 5) | (std::uint64_t(c) << 10) |
        (std::uint64_t(d) << 15) | (std::uint64_t(e) << 20) |
        (std::uint64_t(f) << 25) | (std::uint64_t(g) << 30);
}
std::uint64_t hostHudGlyph(char letter) noexcept {
    if (letter >= 'a' && letter <= 'z') letter = char(letter - 'a' + 'A');
    switch (letter) {
    case 'A': return hudGlyph(14,17,17,31,17,17,17);
    case 'B': return hudGlyph(30,17,17,30,17,17,30);
    case 'C': return hudGlyph(14,17,16,16,16,17,14);
    case 'F': return hudGlyph(31,16,16,30,16,16,16);
    case '/': return hudGlyph(1,1,2,4,8,16,16);
    case ':': return hudGlyph(0,4,4,0,4,4,0);
    case '.': return hudGlyph(0,0,0,0,0,4,4);
    case 'D': return hudGlyph(30,17,17,17,17,17,30);
    case 'E': return hudGlyph(31,16,16,30,16,16,31);
    case 'G': return hudGlyph(14,17,16,23,17,17,14);
    case 'H': return hudGlyph(17,17,17,31,17,17,17);
    case 'I': return hudGlyph(31,4,4,4,4,4,31);
    case 'J': return hudGlyph(7,2,2,2,18,18,12);
    case 'K': return hudGlyph(17,18,20,24,20,18,17);
    case 'L': return hudGlyph(16,16,16,16,16,16,31);
    case 'M': return hudGlyph(17,27,21,21,17,17,17);
    case 'N': return hudGlyph(17,25,25,21,19,19,17);
    case 'O': return hudGlyph(14,17,17,17,17,17,14);
    case 'P': return hudGlyph(30,17,17,30,16,16,16);
    case 'Q': return hudGlyph(14,17,17,17,21,18,13);
    case 'R': return hudGlyph(30,17,17,30,20,18,17);
    case 'S': return hudGlyph(15,16,16,14,1,1,30);
    case 'T': return hudGlyph(31,4,4,4,4,4,4);
    case 'U': return hudGlyph(17,17,17,17,17,17,14);
    case 'V': return hudGlyph(17,17,17,17,17,10,4);
    case 'W': return hudGlyph(17,17,17,21,21,21,10);
    case 'X': return hudGlyph(17,17,10,4,10,17,17);
    case 'Y': return hudGlyph(17,17,10,4,4,4,4);
    case 'Z': return hudGlyph(31,1,2,4,8,16,31);
    case '(': return hudGlyph(2,4,8,8,8,4,2);
    case ')': return hudGlyph(8,4,2,2,2,4,8);
    case '0': return hudGlyph(14,17,17,17,17,17,14);
    case '1': return hudGlyph(4,12,4,4,4,4,14);
    case '2': return hudGlyph(14,17,1,2,4,8,31);
    case '3': return hudGlyph(30,1,1,14,1,1,30);
    case '4': return hudGlyph(18,18,18,31,2,2,2);
    case '5': return hudGlyph(31,16,16,30,1,1,30);
    case '6': return hudGlyph(14,16,16,30,17,17,14);
    case '7': return hudGlyph(31,1,2,4,8,8,8);
    case '8': return hudGlyph(14,17,17,14,17,17,14);
    case '9': return hudGlyph(14,17,17,15,1,1,14);
    default: return 0; // The only other admitted character is a space.
    }
}
bool validHostHudName(const char (&name)[maxVehicleWindowRacerName50DS + 1]) noexcept {
    bool visible = false;
    for (U i = 0; i <= maxVehicleWindowRacerName50DS; ++i) {
        const char letter = name[i];
        if (!letter) return i != 0 && visible;
        if (i == maxVehicleWindowRacerName50DS) return false;
        if (letter == ' ') continue;
        if (!((letter >= 'a' && letter <= 'z') || (letter >= 'A' && letter <= 'Z') ||
              (letter >= '0' && letter <= '9') || letter == '(' || letter == ')')) return false;
        visible = true;
    }
    return false;
}
struct HostHudGeometry {
    assets::Vertex48B vertices[hostHudMaximumQuads * 4]{};
    U count = 0;
    float scale = 1.f, inverseWidth = 1.f, inverseHeight = 1.f;
    bool overflow = false;

    void quad(const float (&positions)[4][2], U rgb) noexcept {
        if (count + 4 > hostHudMaximumQuads * 4) { overflow = true; return; }
        for (U i = 0; i < 4; ++i) {
            auto& vertex = vertices[count++];
            vertex.position[0] = (8.f + positions[i][0]) * scale * inverseWidth - 1.f;
            vertex.position[1] = (64.f - positions[i][1]) * scale * inverseHeight - 1.f;
            vertex.position[2] = vertex.uv[0] = vertex.uv[1] = 0.f;
            vertex.color[0] = static_cast<std::uint8_t>(rgb >> 16);
            vertex.color[1] = static_cast<std::uint8_t>(rgb >> 8);
            vertex.color[2] = static_cast<std::uint8_t>(rgb);
            vertex.color[3] = 255;
        }
    }
    void rectangle(float x, float y, float w, float h, U rgb) noexcept {
        if (w <= 0.f || h <= 0.f) return;
        const float positions[4][2] = {{x,y+h}, {x+w,y+h}, {x+w,y}, {x,y}};
        quad(positions, rgb);
    }
    void text(float x, float y, const char* label, U rgb) noexcept {
        for (; *label; ++label, x += 6.f) {
            const auto glyph = hostHudGlyph(*label);
            for (U row = 0; row < 7; ++row) {
                const U pixels = static_cast<U>((glyph >> (row * 5)) & 31);
                for (U column = 0; column < 5;) {
                    if (!(pixels & (16u >> column))) { ++column; continue; }
                    const U first = column++;
                    while (column < 5 && (pixels & (16u >> column))) ++column;
                    rectangle(x + static_cast<float>(first), y + static_cast<float>(row),
                        static_cast<float>(column - first), 1.f, rgb);
                }
            }
        }
    }
    void bar(float y, float value, U rgb) noexcept {
        rectangle(44.f, y - 1.f, 110.f, 9.f, 0x697A8B);
        rectangle(45.f, y, 108.f, 7.f, 0x1D2935);
        rectangle(45.f, y, 108.f * value, 7.f, rgb);
    }
    bool buildMenu(const VehicleWindowHostMenu50DS& input, I width, I height) noexcept {
        count = 0; overflow = false;
        constexpr float panelWidth = 312.f;
        const float panelHeight = 62.f + 15.f * float(input.rowCount);
        scale = std::min(2.f, std::min(float(width) / (panelWidth + 24.f),
            float(height) / (panelHeight + 24.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        const float x = (float(width) / scale - panelWidth) * .5f - 8.f;
        const float y = 64.f - (float(height) / scale + panelHeight) * .5f;
        rectangle(x, y, panelWidth, panelHeight, 0x617589);
        rectangle(x + 1.f, y + 1.f, panelWidth - 2.f, panelHeight - 2.f, 0x0A111B);
        text(x + 12.f, y + 10.f, input.title, 0x5DDFFF);
        text(x + 12.f, y + 23.f, input.subtitle, 0xA8C5DE);
        for (U i = 0; i < input.rowCount; ++i) {
            const float rowY = y + 41.f + 15.f * float(i);
            const bool selected = i == input.selectedRow;
            if (selected) rectangle(x + 7.f, rowY - 3.f, panelWidth - 14.f, 13.f, 0x173547);
            text(x + 12.f, rowY, input.rows[i], selected ? 0x5DDFFF : 0xEDF3F8);
        }
        text(x + 12.f, y + panelHeight - 12.f, input.footer, 0xA8C5DE);
        return !overflow;
    }
    bool buildResults(const VehicleWindowHostHud50DS& input, I width, I height) noexcept {
        count = 0; overflow = false;
        constexpr float panelWidth = 380.f;
        const float timingY = 79.f + 11.f * float(input.standingsCount);
        const float actionY = timingY + (input.completedLapTimesValid ?
            27.f + 11.f * float(input.completedLapTimeCount) : 20.f);
        const float panelHeight = actionY + 70.f;
        scale = std::min(2.f, std::min(float(width) / (panelWidth + 24.f),
            float(height) / (panelHeight + 24.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        const float x = (float(width) / scale - panelWidth) * .5f - 8.f;
        const float y = 64.f - (float(height) / scale + panelHeight) * .5f;
        rectangle(x, y, panelWidth, panelHeight, 0x617589);
        rectangle(x + 1.f, y + 1.f, panelWidth - 2.f, panelHeight - 2.f, 0x0A111B);
        text(x + 12.f, y + 10.f, "RACE RESULTS", 0x5DDFFF);
        for (U i = 0; i < input.standingsCount; ++i)
            if (input.standings[i].racerIndex == input.standingsPlayerIndex)
                text(x + 12.f, y + 24.f, input.standings[i].displayName, 0xEDF3F8);
        if (input.raceFinished) {
            char place[32]{}, finish[40]{};
            std::snprintf(place, sizeof(place), "PLACE %u / %u", input.resultsFinishPlace, input.raceRacerCount);
            // Explicit sealed finish only. Native CC0 durations below retain
            // their separate provenance; neither elapsed time nor rank can
            // manufacture a finish for a retired, unfinished racer.
            const U finishCentiseconds = static_cast<U>(input.resultsFinishSeconds * 100.0 + .5);
            std::snprintf(finish, sizeof(finish), "FINISH TIME %u:%02u.%02u", finishCentiseconds / 6000u,
                (finishCentiseconds / 100u) % 60u, finishCentiseconds % 100u);
            text(x + 12.f, y + 40.f, place, 0xA8E6B4);
            text(x + 158.f, y + 40.f, finish, 0xA8E6B4);
        } else {
            text(x + 12.f, y + 40.f, "DNF", 0xFFBF55);
            text(x + 158.f, y + 40.f, "NO FINISH TIME", 0xFFBF55);
        }
        text(x + 12.f, y + 58.f, "STANDINGS AT RACE END", 0xA8C5DE);
        text(x + 232.f, y + 58.f, "LAPS", 0xA8C5DE);
        for (U i = 0; i < input.standingsCount; ++i) {
            const auto& row = input.standings[i];
            const float rowY = y + 72.f + 11.f * float(i);
            const bool player = row.racerIndex == input.standingsPlayerIndex;
            if (player) rectangle(x + 7.f, rowY - 2.f, panelWidth - 14.f, 11.f, 0x173547);
            const U color = player ? 0x5DDFFF : 0xEDF3F8;
            char rank[4]{}, laps[12]{};
            std::snprintf(rank, sizeof(rank), "%u", row.place);
            std::snprintf(laps, sizeof(laps), "%u/%u", row.completedLaps, input.raceLapTarget);
            text(x + 12.f, rowY, rank, color);
            text(x + 30.f, rowY, row.displayName, color);
            text(x + 232.f, rowY, laps, color);
            text(x + 300.f, rowY, row.finished ? "FINISHED" : row.retirementCD4 ? "DNF" : "UNFINISHED",
                !row.finished && row.retirementCD4 ? 0xFFBF55 : 0xA8C5DE);
        }
        if (input.completedLapTimesValid) {
            text(x + 12.f, y + timingY, "COMPLETED LAP TIMES", 0xA8C5DE);
            const auto duration = [&](float rowY, float seconds) noexcept {
                const U centiseconds = static_cast<U>(double(seconds) * 100.0 + .5);
                char value[20]{};
                std::snprintf(value, sizeof(value), "%u:%02u.%02u", centiseconds / 6000u,
                    (centiseconds / 100u) % 60u, centiseconds % 100u);
                text(x + 78.f, rowY, value, 0xEDF3F8);
            };
            for (U i = 0; i < input.completedLapTimeCount; ++i) {
                const char label[] = {'L','A','P',' ',char('1' + i),'\0'};
                const float rowY = y + timingY + 13.f + 11.f * float(i);
                text(x + 12.f, rowY, label, 0xEDF3F8);
                duration(rowY, input.completedLapSeconds[i]);
            }
            if (input.completedLapTimeCount) {
                const float bestY = y + timingY + 13.f + 11.f * float(input.completedLapTimeCount);
                text(x + 12.f, bestY, "BEST", 0xA8E6B4);
                duration(bestY, input.bestLapSeconds);
            } else text(x + 12.f, y + timingY + 13.f, "NO COMPLETED LAPS", 0xEDF3F8);
        } else text(x + 12.f, y + timingY, "LAP TIMES UNAVAILABLE", 0xA8C5DE);
        const char* actions[] = {"RESTART RACE", "MAIN MENU", "EXIT"};
        if (input.resultsConfirming) {
            text(x + 12.f, y + actionY, "LEAVE THESE RESULTS", 0xFFBF55);
            const char* choices[] = {"CANCEL", actions[input.resultsSelectedAction]};
            for (U i = 0; i < 2; ++i) {
                const float rowY = y + actionY + 16.f + 16.f * float(i);
                const bool selected = i == unsigned(input.resultsConfirmAccept);
                if (selected) rectangle(x + 7.f, rowY - 3.f, panelWidth - 14.f, 13.f, 0x173547);
                text(x + 12.f, rowY, choices[i], selected ? 0x5DDFFF : 0xEDF3F8);
            }
        } else for (U i = 0; i < 3; ++i) {
            const float rowY = y + actionY + 16.f * float(i);
            const bool selected = i == input.resultsSelectedAction;
            if (selected) rectangle(x + 7.f, rowY - 3.f, panelWidth - 14.f, 13.f, 0x173547);
            text(x + 12.f, rowY, actions[i], selected ? 0x5DDFFF : 0xEDF3F8);
        }
        text(x + 12.f, y + panelHeight - 12.f, input.resultsConfirming ?
            "UP/DOWN SELECT  ENTER CONFIRM  ESC CANCEL" :
            "UP/DOWN SELECT  ENTER CONFIRM  ESC PAUSE", 0xA8C5DE);
        return !overflow;
    }
    void standingsPanel(const VehicleWindowHostHud50DS& input, I width, I height,
        float reservedTop=0.f,float maximumScale=0.f) noexcept {
        if (!input.standingsCount) return;
        // Leave a four-cell gap below the race readout, including optional
        // clocks. Only this panel contracts when the viewport cannot fit it
        // above the bottom heat/damage panel or beside the right route cue.
        const float hudScale = scale;
        const float panelWidth = 222.f, standingsHeight = 22.f + 11.f * float(input.standingsCount);
        const float timingHeight = input.completedLapTimesValid ?
            30.f + 11.f * float(input.completedLapTimeCount) : 0.f;
        const float panelHeight = standingsHeight + timingHeight;
        const float clockHeight = input.raceClocksValid ?
            (input.raceFinished || input.raceRetirementCD4 ? 14.f : 28.f) : 0.f;
        const float top = reservedTop>0.f?reservedTop:(44.f + clockHeight) * hudScale;
        const float availableHeight = float(height) - top - 68.f * hudScale;
        const float availableWidth = float(width) - 68.f * hudScale;
        if (availableHeight <= 0.f || availableWidth <= 0.f) return;
        scale = std::min(maximumScale>0.f?maximumScale:hudScale,
            std::min(availableWidth / panelWidth, availableHeight / panelHeight));
        const float x = 8.f * hudScale / scale - 8.f;
        const float y = 64.f - (float(height) - top) / scale;
        rectangle(x, y, panelWidth, panelHeight, 0x617589);
        rectangle(x + 1.f, y + 1.f, panelWidth - 2.f, panelHeight - 2.f, 0x0A111B);
        text(x + 6.f, y + 5.f, "STANDINGS", 0xA8C5DE);
        text(x + 168.f, y + 5.f, "DONE", 0xA8C5DE);
        for (U i = 0; i < input.standingsCount; ++i) {
            const auto& row = input.standings[i];
            const float rowY = y + 19.f + 11.f * float(i);
            const bool player = row.racerIndex == input.standingsPlayerIndex;
            if (player) rectangle(x + 3.f, rowY - 2.f, panelWidth - 6.f, 11.f, 0x173547);
            const U color = player ? 0x5DDFFF : 0xEDF3F8;
            const char place[2] = {char('0' + row.place), '\0'};
            char completed[4]{};
            std::snprintf(completed, sizeof(completed), "%u", row.completedLaps);
            text(x + 6.f, rowY, place, color);
            text(x + 18.f, rowY, row.displayName, color);
            text(x + 168.f, rowY, completed, color);
            if (row.finished) text(x + 198.f, rowY, "FIN", 0xA8E6B4);
            else if (row.retirementCD4) text(x + 198.f, rowY, "DNF", 0xFFBF55);
        }
        if (input.completedLapTimesValid) {
            const float timingY = y + standingsHeight;
            text(x + 6.f, timingY + 4.f, "COMPLETED LAP TIMES", 0xA8C5DE);
            if (!input.completedLapTimeCount) {
                text(x + 6.f, timingY + 18.f, "NO COMPLETED LAPS", 0xEDF3F8);
            } else {
                const auto duration = [&](float rowY, float seconds, U color) noexcept {
                    // Round binary32 storage back to its published centisecond;
                    // flooring can otherwise drop a displayed hundredth.
                    const U centiseconds = static_cast<U>(double(seconds) * 100.0 + 0.5);
                    char value[20]{};
                    std::snprintf(value, sizeof(value), "%u:%02u.%02u", centiseconds / 6000u,
                        (centiseconds / 100u) % 60u, centiseconds % 100u);
                    text(x + 66.f, rowY, value, color);
                };
                for (U i = 0; i < input.completedLapTimeCount; ++i) {
                    const float rowY = timingY + 18.f + 11.f * float(i);
                    const char label[] = {'L', 'A', 'P', ' ', char('1' + i), '\0'};
                    text(x + 6.f, rowY, label, 0xEDF3F8);
                    duration(rowY, input.completedLapSeconds[i], 0xEDF3F8);
                }
                const float bestY = timingY + 18.f + 11.f * float(input.completedLapTimeCount);
                text(x + 6.f, bestY, "BEST", 0xA8E6B4);
                duration(bestY, input.bestLapSeconds, 0xA8E6B4);
            }
        }
        scale = hudScale;
    }
    bool build(const VehicleWindowHostHud50DS& input, I width, I height,
        float admittedOriginalBottom=0.f) noexcept {
        if (input.resultsPanelVisible) return buildResults(input, width, height);
        count = 0; overflow = false;
        const float clockHeight = input.raceClocksValid ?
            (input.raceFinished || input.raceRetirementCD4 ? 14.f : 28.f) : 0.f;
        scale = width >= 960 && height >= 540 ? 2.f : 1.f;
        scale = std::min(scale, std::min(float(width) / 176.f,
            float(height) / (input.raceEnabled ? 112.f + clockHeight : input.lapCountValid ? 100.f : 72.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        rectangle(0.f, 0.f, 160.f, 56.f, 0x617589);
        rectangle(1.f, 1.f, 158.f, 54.f, 0x0A111B);
        text(6.f, 5.f, "POD STATUS", 0xA8C5DE);
        text(104.f, 5.f, input.overheated ? "OVERHEAT" : input.boostActive ? "BOOST" : "IDLE",
            input.overheated ? 0xFF6556 : input.boostActive ? 0x5DDFFF : 0xA8E6B4);
        text(6.f, 19.f, "HEAT", 0xEDF3F8);
        text(6.f, 31.f, "L DMG", 0xEDF3F8);
        text(6.f, 43.f, "R DMG", 0xEDF3F8);
        bar(19.f, input.heat01, input.overheated ? 0xFF6556 : 0xFFBF55);
        bar(31.f, input.leftDamage01, 0xEF7767);
        bar(43.f, input.rightDamage01, 0xEF7767);
        float standingsTop=0.f,standingsScale=0.f;bool omitStandings=false;
        if (input.raceEnabled&&admittedOriginalBottom>0.f) {
            // This bound comes only from this renderer's accepted TimeStats
            // quad copy. Original coordinates and source draw stay untouched.
            const float hudScale=scale;
            const float top=admittedOriginalBottom+8.f*hudScale;
            const float panelHeight=input.standingsCount?
                22.f+11.f*float(input.standingsCount)+(input.completedLapTimesValid?
                    30.f+11.f*float(input.completedLapTimeCount):0.f):0.f;
            const float availableHeight=float(height)-top-68.f*hudScale;
            const float availableWidth=float(width)-68.f*hudScale;
            if(availableHeight<=0.f||availableWidth<=0.f)omitStandings=true;
            else {
                // Fit the compact status and standings together above heat/damage;
                // leave the same right-side clearance used for NEXT.
                scale=std::min(hudScale,std::min(availableWidth/(input.standingsCount?222.f:168.f),
                    availableHeight/(32.f+(input.standingsCount?4.f+panelHeight:0.f))));
                const float x=8.f*hudScale/scale-8.f;
                const float y=64.f-(float(height)-top)/scale;
                rectangle(x,y,168.f,32.f,0x617589);
                rectangle(x+1.f,y+1.f,166.f,30.f,0x0A111B);
                char place[32]{},countdown[32]{};
                std::snprintf(place,sizeof(place),"POS %u/%u",input.racePlace,input.raceRacerCount);
                std::snprintf(countdown,sizeof(countdown),"START IN %u",input.raceCountdown);
                text(x+6.f,y+5.f,place,0xEDF3F8);
                const char* status=input.raceResultsReady?"RESULTS":input.raceFinished?"FINISHED":
                    input.raceRetirementCD4?"DNF":input.raceCountdown?countdown:
                    input.raceGo?"GO":input.wrongWay?"WRONG WAY":"RACING";
                text(x+6.f,y+19.f,status,input.raceFinished||input.raceGo?0xA8E6B4:
                    input.raceRetirementCD4?0xFFBF55:input.wrongWay?0xFF6556:0xA8C5DE);
                standingsTop=top+36.f*scale;standingsScale=scale;
                scale=hudScale;
            }
        } else if (input.raceEnabled) {
            const float y = 72.f - float(height) / scale;
            rectangle(0.f, y, 168.f, 32.f + clockHeight, 0x617589);
            rectangle(1.f, y + 1.f, 166.f, 30.f + clockHeight, 0x0A111B);
            char laps[32]{}, place[32]{}, countdown[32]{};
            std::snprintf(laps, sizeof(laps), "LAP %u/%u", input.raceLap, input.raceLapTarget);
            std::snprintf(place, sizeof(place), "POS %u/%u", input.racePlace, input.raceRacerCount);
            std::snprintf(countdown, sizeof(countdown), "START IN %u", input.raceCountdown);
            text(6.f, y + 5.f, laps, 0xEDF3F8);
            text(90.f, y + 5.f, place, 0xEDF3F8);
            const char* status = input.raceResultsReady ? "RESULTS" : input.raceFinished ? "FINISHED" :
                input.raceRetirementCD4 ? "DNF" : input.raceCountdown ? countdown :
                input.raceGo ? "GO" : input.wrongWay ? "WRONG WAY" : "RACING";
            text(6.f, y + 19.f, status, input.raceFinished || input.raceGo ? 0xA8E6B4 :
                input.raceRetirementCD4 ? 0xFFBF55 : input.wrongWay ? 0xFF6556 : 0xA8C5DE);
            if (input.raceClocksValid) {
                const auto clock = [&](float rowY, float seconds) noexcept {
                    // These live fields are not CC0's quantized completed laps.
                    // Truncate for presentation; never advance a source clock.
                    const U centiseconds = static_cast<U>(double(seconds) * 100.0);
                    char value[20]{};
                    std::snprintf(value, sizeof(value), "%u:%02u.%02u", centiseconds / 6000u,
                        (centiseconds / 100u) % 60u, centiseconds % 100u);
                    text(66.f, rowY, value, 0xEDF3F8);
                };
                text(6.f, y + 33.f, "RACE", 0xA8C5DE);
                clock(y + 33.f, input.raceElapsedSeconds);
                if (!input.raceFinished && !input.raceRetirementCD4) {
                    text(6.f, y + 47.f, "LAP TIME", 0xA8C5DE);
                    clock(y + 47.f, input.currentLapSeconds);
                }
            }
        } else if (input.lapCountValid) {
            // Completed count plus optional caller-supplied direction warning.
            // Same fixed batch, top-left opposite NEXT; at most 143 quads.
            const float y = 72.f - float(height) / scale;
            rectangle(0.f, y, 94.f, 18.f, 0x617589);
            rectangle(1.f, y + 1.f, 92.f, 16.f, 0x0A111B);
            text(6.f, y + 5.f, input.wrongWay ? "WRONG WAY" : "HOST LAPS",
                input.wrongWay ? 0xFF6556 : 0xA8C5DE);
            char digits[4]{};
            U length = 0;
            if (input.completedLaps >= 100)
                digits[length++] = char('0' + input.completedLaps / 100);
            if (input.completedLaps >= 10)
                digits[length++] = char('0' + (input.completedLaps / 10) % 10);
            digits[length] = char('0' + input.completedLaps % 10);
            text(68.f, y + 5.f, digits, 0xEDF3F8);
        }
        if (input.routeValid) {
            // Same screen-space batch. Anchor a 44x60-cell panel eight cells
            // from the top/right; route bearing never alters the scene/camera.
            const float x = float(width) / scale - 60.f;
            const float y = 72.f - float(height) / scale;
            rectangle(x, y, 44.f, 60.f, 0x617589);
            rectangle(x + 1.f, y + 1.f, 42.f, 58.f, 0x0A111B);
            text(x + 10.f, y + 15.f, "NEXT", 0xEDF3F8);
            const float cx = x + 22.f, cy = y + 41.f;
            const float dx = input.routeX, dy = -input.routeY;
            const float px = -dy, py = dx;
            const float shaft[4][2] = {
                {cx - 7.f*dx - 2.f*px, cy - 7.f*dy - 2.f*py},
                {cx + 3.f*dx - 2.f*px, cy + 3.f*dy - 2.f*py},
                {cx + 3.f*dx + 2.f*px, cy + 3.f*dy + 2.f*py},
                {cx - 7.f*dx + 2.f*px, cy - 7.f*dy + 2.f*py}};
            // A repeated corner makes one indexed quad a single arrowhead triangle.
            const float head[4][2] = {
                {cx + 2.f*dx - 7.f*px, cy + 2.f*dy - 7.f*py},
                {cx + 13.f*dx, cy + 13.f*dy},
                {cx + 2.f*dx + 7.f*px, cy + 2.f*dy + 7.f*py},
                {cx + 2.f*dx + 7.f*px, cy + 2.f*dy + 7.f*py}};
            quad(shaft, 0x5DDFFF);
            quad(head, 0x5DDFFF);
        }
        if(!omitStandings)standingsPanel(input, width, height,standingsTop,standingsScale);
        return !overflow;
    }
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
}

struct VehicleWindowGl50DS::Storage {
    std::shared_ptr<ra_static_fire_visual50LA::Owner> raStaticFireOwner;
    const VehicleWindowGl50DS* raStaticFireRenderer=nullptr;
    std::uint64_t raStaticFirePublicationSerial=0;
    void retireRaStaticFirePublication() noexcept {
        if(raStaticFireOwner){
            raStaticFireOwner->retireRenderer(raStaticFireRenderer);
            raStaticFireOwner.reset();raStaticFireRenderer=nullptr;raStaticFirePublicationSerial=0;
        }
    }
    std::shared_ptr<rc_particle_visual50RC::Window> rcParticleWindow;
    const VehicleWindowGl50DS* rcParticleRenderer=nullptr;
    std::uint64_t rcParticleAttempt=0,rcParticlePendingAttempt=0,rcParticleCompletedBefore=0;
    bool cancelRcParticleAttempt() noexcept {
        if(!rcParticleWindow||!rcParticlePendingAttempt)return true;
        if(rcParticleWindow->renderer_!=rcParticleRenderer||
           rcParticleWindow->lastAttempt_!=rcParticlePendingAttempt)return false;
        if(rcParticleWindow->attemptActive_&&
           !rcParticleWindow->failRenderAttempt(rcParticleRenderer,rcParticlePendingAttempt))return false;
        rcParticlePendingAttempt=0;return true;
    }
    bool beginRcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!rcParticleWindow)return true;
        if(!cancelRcParticleAttempt())return fail(d,VehicleWindowError50DS::frame,"RC prior particle draw attempt could not retire");
        if(rcParticleAttempt==std::numeric_limits<std::uint64_t>::max()){
            if(!rcParticleWindow->invalidateRenderPublication(rcParticleRenderer))d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::limits,"RC particle draw generation exhausted");
        }
        const auto attempt=++rcParticleAttempt;
        if(!rcParticleWindow->beginRenderAttempt(rcParticleRenderer,attempt))
            return fail(d,VehicleWindowError50DS::frame,"RC particle draw attempt owner rejected");
        rcParticlePendingAttempt=attempt;rcParticleCompletedBefore=trackVisibilityCompletedGeneration;return true;
    }
    bool completeRcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!rcParticleWindow||!rcParticlePendingAttempt)return true;
        if(active||frameSet||diagnostic||!current(d)||!texturesCurrent(d)||
           !trackVisibilityCompletedValid||trackVisibilityCompletedGeneration<=rcParticleCompletedBefore)
            return fail(d,VehicleWindowError50DS::frame,"RC particle completion requires the same idle current completed draw");
        std::array<bool,4> submitted{};
        for(std::size_t i=0;i<submitted.size();++i){
            if(rcParticleWindow->spans_[i].empty())return fail(d,VehicleWindowError50DS::frame,"RC source instance has no authenticated spans");
            submitted[i]=true;
            for(const auto span:rcParticleWindow->spans_[i]){
                if(span>=trackSpanCount)return fail(d,VehicleWindowError50DS::frame,"RC source instance span exceeds retained renderer table");
                submitted[i]=submitted[i]&&trackSpans[span].submittedCompletedGameplay;
            }
        }
        if(!rcParticleWindow->recordCompleted(rcParticleRenderer,rcParticlePendingAttempt,
            trackVisibilityCompletedGeneration,submitted))
            return fail(d,VehicleWindowError50DS::frame,"RC actual particle instance completion publication rejected");
        rcParticlePendingAttempt=0;return true;
    }
    void retireRcParticlePublication() noexcept {
        if(rcParticleWindow){
            if(!cancelRcParticleAttempt())std::terminate();
            rcParticleWindow->retireRenderer(rcParticleRenderer);
            rcParticleWindow.reset();rcParticleRenderer=nullptr;
        }
    }
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
    bool cancelGbParticleAttempt() noexcept {
        if(!gbParticleWindow||!gbParticlePendingAttempt)return true;
        if(gbParticleWindow->renderer_!=gbParticleRenderer||
           gbParticleWindow->lastAttempt_!=gbParticlePendingAttempt)return false;
        if(gbParticleWindow->attemptActive_&&
           !gbParticleWindow->failRenderAttempt(gbParticleRenderer,gbParticlePendingAttempt))return false;
        gbParticlePendingAttempt=0;return true;
    }
    bool beginGbParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!gbParticleWindow)return true;
        if(!cancelGbParticleAttempt())return fail(d,VehicleWindowError50DS::frame,"GB prior particle draw attempt could not retire");
        if(gbParticleAttempt==std::numeric_limits<std::uint64_t>::max()){
            if(!gbParticleWindow->invalidateRenderPublication(gbParticleRenderer))d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::limits,"GB particle draw generation exhausted");
        }
        const auto attempt=++gbParticleAttempt;
        if(!gbParticleWindow->beginRenderAttempt(gbParticleRenderer,attempt))
            return fail(d,VehicleWindowError50DS::frame,"GB particle draw attempt owner rejected");
        gbParticlePendingAttempt=attempt;gbParticleCompletedBefore=trackVisibilityCompletedGeneration;return true;
    }
    bool completeGbParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!gbParticleWindow||!gbParticlePendingAttempt)return true;
        if(active||frameSet||diagnostic||!current(d)||!texturesCurrent(d)||
           !trackVisibilityCompletedValid||trackVisibilityCompletedGeneration<=gbParticleCompletedBefore)
            return fail(d,VehicleWindowError50DS::frame,"GB particle completion requires the same idle current completed draw");
        std::array<bool,gb_particle_attachments50GB::placementCount> submitted{};
        for(std::size_t i=0;i<submitted.size();++i){
            const auto association=gbParticleWindow->representative_[i];
            const auto count=gbParticleWindow->attachments_->spanCount(association);
            const auto* spans=gbParticleWindow->attachments_->spanIndices(association);
            if(!count||!spans)return fail(d,VehicleWindowError50DS::frame,"GB source instance has no authenticated spans");
            submitted[i]=true;
            for(std::size_t n=0;n<count;++n){const auto span=spans[n];
                if(span>=trackSpanCount)return fail(d,VehicleWindowError50DS::frame,"GB source instance span exceeds retained renderer table");
                submitted[i]=submitted[i]&&trackSpans[span].submittedCompletedGameplay;
            }
        }
        if(!gbParticleWindow->recordCompleted(gbParticleRenderer,gbParticlePendingAttempt,
            trackVisibilityCompletedGeneration,submitted))
            return fail(d,VehicleWindowError50DS::frame,"GB actual particle instance completion publication rejected");
        gbParticlePendingAttempt=0;return true;
    }
    void retireGbParticlePublication() noexcept {
        if(gbParticleWindow){
            if(!cancelGbParticleAttempt())std::terminate();
            gbParticleWindow->retireRenderer(gbParticleRenderer);
            gbParticleWindow.reset();gbParticleRenderer=nullptr;
        }
    }
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
    bool cancelMcParticleAttempt() noexcept {
        if(!mcParticleWindow||!mcParticlePendingAttempt)return true;
        if(mcParticleWindow->renderer_!=mcParticleRenderer||
           mcParticleWindow->lastAttempt_!=mcParticlePendingAttempt)return false;
        if(mcParticleWindow->attemptActive_&&
           !mcParticleWindow->failRenderAttempt(mcParticleRenderer,mcParticlePendingAttempt))return false;
        mcParticlePendingAttempt=0;return true;
    }
    bool beginMcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!mcParticleWindow)return true;
        if(!cancelMcParticleAttempt())return fail(d,VehicleWindowError50DS::frame,"MC prior particle draw attempt could not retire");
        if(mcParticleAttempt==std::numeric_limits<std::uint64_t>::max()){
            if(!mcParticleWindow->invalidateRenderPublication(mcParticleRenderer))d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::limits,"MC particle draw generation exhausted");
        }
        const auto attempt=++mcParticleAttempt;
        if(!mcParticleWindow->beginRenderAttempt(mcParticleRenderer,attempt))
            return fail(d,VehicleWindowError50DS::frame,"MC particle draw attempt owner rejected");
        mcParticlePendingAttempt=attempt;mcParticleCompletedBefore=trackVisibilityCompletedGeneration;return true;
    }
    bool completeMcParticleAttempt(VehicleWindowDiagnostic50DS& d) noexcept {
        if(!mcParticleWindow||!mcParticlePendingAttempt)return true;
        if(active||frameSet||diagnostic||!current(d)||!texturesCurrent(d)||
           !trackVisibilityCompletedValid||trackVisibilityCompletedGeneration<=mcParticleCompletedBefore)
            return fail(d,VehicleWindowError50DS::frame,"MC particle completion requires the same idle current completed draw");
        std::array<bool,mc_particle_attachments50MC::placementCount> submitted{};
        for(std::size_t i=0;i<submitted.size();++i){
            const auto association=mcParticleWindow->representative_[i];
            const auto count=mcParticleWindow->attachments_->spanCount(association);
            const auto* spans=mcParticleWindow->attachments_->spanIndices(association);
            if(!count||!spans)return fail(d,VehicleWindowError50DS::frame,"MC source instance has no authenticated spans");
            submitted[i]=true;
            for(std::size_t n=0;n<count;++n){const auto span=spans[n];
                if(span>=trackSpanCount)return fail(d,VehicleWindowError50DS::frame,"MC source instance span exceeds retained renderer table");
                submitted[i]=submitted[i]&&trackSpans[span].submittedCompletedGameplay;
            }
        }
        if(!mcParticleWindow->recordCompleted(mcParticleRenderer,mcParticlePendingAttempt,
            trackVisibilityCompletedGeneration,submitted))
            return fail(d,VehicleWindowError50DS::frame,"MC actual particle instance completion publication rejected");
        mcParticlePendingAttempt=0;return true;
    }
    void retireMcParticlePublication() noexcept {
        if(mcParticleWindow){
            if(!cancelMcParticleAttempt())std::terminate();
            mcParticleWindow->retireRenderer(mcParticleRenderer);
            mcParticleWindow.reset();mcParticleRenderer=nullptr;
        }
    }
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
    static bool podMaterialUsesNormals(const VehicleWindowMaterialBinding50DS& policy) noexcept {
        return !policy.unlit && !policy.sourcePrelit && !policy.sourceDiffuseCompressed50LA &&
            policy.blend == VehicleWindowBlend50DS::opaque;
    }
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
    RacerView racerView(U archetype) const noexcept {
        if (!archetype) return {meshes, scenes, &resources, racerNormalBuffers, materials.get(),
            materialOffsets, materialSummary, materialCount, cableTextureName, sourceSpecularGeometry50LA.data()};
        const auto& source = racerArchetypes[archetype - 1];
        return {source.meshes, source.scenes, &source.resources, source.normalBuffers, source.materials.get(),
            source.materialOffsets, source.materialSummary, source.materialCount, source.cableTextureName,
            source.specularGeometry50LA.data()};
    }
    std::uint64_t texturePublication = 0;
    std::unique_ptr<TrackSpan[]> trackSpans;
    U trackSpanCount = 0;
    U trackVisibilityPendingTarget = 0;
    std::uint64_t trackVisibilityCompletedGeneration = 0;
    bool trackVisibilityCompletedValid = false;
    void acceptCompletedTrackVisibility() noexcept {
        // Race banks are distinct from host-menu, UI and blocking-fade targets.
        // Only a fully successful gameplay submit sets the pending target.
        if(!trackVisibilityPendingTarget)return;
        const bool actualGameplay=trackVisibilityPendingTarget==displayedTarget &&
            displayedTarget==targets[published];
        trackVisibilityPendingTarget=0;
        if(!actualGameplay)return;
        if(trackVisibilityCompletedGeneration==std::numeric_limits<std::uint64_t>::max()) {
            trackVisibilityCompletedValid=false;
            if(raStaticFireOwner)raStaticFireOwner->nativeCompletionLive_=false;
            return;
        }
        for(U i=0;i<trackSpanCount;++i)
            trackSpans[i].submittedCompletedGameplay=trackSpans[i].submittedGameplayCandidate;
        ++trackVisibilityCompletedGeneration;trackVisibilityCompletedValid=true;
        if(raStaticFireOwner){
            // Exact cold admission proved source-order spans0..3 and retained
            // their actual worldInstance identities. Read only issued draw bits.
            if(trackSpanCount!=4){trackVisibilityCompletedValid=false;raStaticFireOwner->nativeCompletionLive_=false;return;}
            std::array<bool,4> submitted{};
            for(U i=0;i<4;++i)submitted[i]=trackSpans[i].submittedCompletedGameplay;
            if(!raStaticFireOwner->recordNativeCompleted(raStaticFireRenderer,
                raStaticFirePublicationSerial,trackVisibilityCompletedGeneration,submitted)){
                trackVisibilityCompletedValid=false;raStaticFireOwner->nativeCompletionLive_=false;
            }
        }
    }
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
        const OriginalVideoBlockingNativeBackend* fadeOwner=nullptr) const noexcept {
        if(displayFadeOwner && displayFadeOwner!=fadeOwner)
            return fail(d,VehicleWindowError50DS::frame,"blocking display fade owns renderer until retirement");
        if (thread != std::this_thread::get_id())
            return fail(d, VehicleWindowError50DS::wrongThread, "renderer requires creating thread");
        return (request.isCurrent && request.isCurrent(request.context)) ||
            fail(d, VehicleWindowError50DS::notCurrent, "caller context/host surface is not current");
    }
    std::uint64_t gameplayBinderPublication=0;
    const game::original_gameplay_binder_candidate::Mesh* frameGameplayBinders=nullptr;
    U frameGameplayBinderCount=0,frameGameplayBinderImage=0;
    bool texturesCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if(!currentRaStaticFire(request.track,d))return false;
        if(rcParticleWindow&&(!rcParticleWindow->ready()||
           !rcParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,rcParticlePublication)||
           !rcParticleWindow->gpu_.matchesSourceBank(rcParticleWindow->bank_,rcParticlePublication)))
            return fail(d,VehicleWindowError50DS::material,"RC particle actual atlas publication/context changed");
        if(gbParticleWindow&&(!gbParticleWindow->ready()||
           !gbParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,gbParticlePublication)||
           !gbParticleWindow->gpu_.matchesSourceBank(gbParticleWindow->bank_,gbParticlePublication)))
            return fail(d,VehicleWindowError50DS::material,"GB particle actual atlas publication/context changed");
        if(mcParticleWindow&&(!mcParticleWindow->ready()||
           !mcParticleWindow->gpu_.validBorrow(request.context,request.isCurrent,mcParticlePublication)||
           !mcParticleWindow->gpu_.matchesSourceBank(mcParticleWindow->bank_,mcParticlePublication)))
            return fail(d,VehicleWindowError50DS::material,"MC particle actual atlas publication/context changed");
        if(request.gcParticleTextures&&!request.gcParticleTextures->validBorrow(
            request.context,request.isCurrent,gcParticlePublication))
            return fail(d,VehicleWindowError50DS::material,"GC particle atlas publication/context changed");
        if(request.gameplayBinderTextures&&!request.gameplayBinderTextures->validBorrow(
            request.context,request.isCurrent,gameplayBinderPublication))
            return fail(d,VehicleWindowError50DS::material,"Binder immutable texture publication expired");
        if (request.mode == VehicleWindowMode50DS::callerMaterialTextures &&
            (!request.materials.textures || !request.materials.textures->validBorrow(
                request.context, request.isCurrent, texturePublication)))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed vehicle texture publication/context/thread is no longer valid");
        if (request.materials.specularTextures50LA && !request.materials.specularTextures50LA->validBorrow(
            request.context, request.isCurrent, specularPublication50LA))
            return fail(d, VehicleWindowError50DS::material, "player map-2 publication/context/thread changed");
        for (U m = 0; m < materialCount; ++m) {
            const auto& material = materials[m];
            if (!material.policy.sourceSpecularPair50LA) continue;
            const auto* base = request.materials.textures->asset(material.textureIndex);
            const auto* map2 = request.materials.specularTextures50LA->asset(material.policy.specularTextureIndex50LA);
            if (!base || base->name != material.name || base->identity != material.textureIdentity50LA ||
                !map2 || map2->name != material.specularName50LA || map2->identity != material.specularIdentity50LA)
                return fail(d, VehicleWindowError50DS::material, "player source pair texture identity changed");
        }
        for (U i = 0; i < racerArchetypeCount; ++i) {
            const auto& archetype = racerArchetypes[i];
            if (request.mode == VehicleWindowMode50DS::callerMaterialTextures &&
                (!archetype.textures || !archetype.textures->validBorrow(
                    request.context, request.isCurrent, archetype.texturePublication)))
                return fail(d, VehicleWindowError50DS::material,
                    "borrowed racer texture publication/context/thread is no longer valid");
            if (archetype.specularTextures50LA && !archetype.specularTextures50LA->validBorrow(
                request.context, request.isCurrent, archetype.specularPublication50LA))
                return fail(d, VehicleWindowError50DS::material, "racer map-2 publication/context/thread changed");
            if (archetype.cables.textures && !archetype.cables.textures->validBorrow(
                request.context, request.isCurrent, archetype.cableTexturePublication))
                return fail(d, VehicleWindowError50DS::material,
                    "borrowed racer cable publication/context/thread is no longer valid");
            for (U m = 0; m < archetype.materialCount; ++m) {
                const auto& material = archetype.materials[m];
                if (!material.name) continue;
                const auto* texture = archetype.textures->asset(material.textureIndex);
                if (!texture || texture->name != material.name)
                    return fail(d, VehicleWindowError50DS::material,
                        "racer material texture is no longer the retained publication");
                if (material.policy.sourceSpecularPair50LA) {
                    const auto* map2 = archetype.specularTextures50LA->asset(material.policy.specularTextureIndex50LA);
                    if (texture->identity != material.textureIdentity50LA || !map2 ||
                        map2->name != material.specularName50LA || map2->identity != material.specularIdentity50LA)
                        return fail(d, VehicleWindowError50DS::material, "racer source pair texture identity changed");
                }
            }
            if (archetype.cables.textures) {
                const auto* texture = archetype.cables.textures->asset(archetype.cables.textureIndex);
                if (!texture || texture->name != archetype.cableTextureName)
                    return fail(d, VehicleWindowError50DS::material,
                        "racer cable texture is no longer the retained publication");
            }
        }
        if(!raWorldCurrent(d))return false;
        if (request.track.mode == VehicleWindowMode50DS::callerMaterialTextures &&
            (!request.track.textures || !request.track.textures->validBorrow(
                request.context, request.isCurrent, trackTexturePublication)))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed track texture publication/context/thread is no longer valid");
        if (request.trackAnimationTextures && !request.trackAnimationTextures->validBorrow(
            request.context, request.isCurrent, trackAnimationTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed animation texture publication/context/thread is no longer valid");
        if (request.dynamicWorld.textures && !request.dynamicWorld.textures->validBorrow(
            request.context, request.isCurrent, dynamicWorldTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed dynamic-world texture publication/context/thread is no longer valid");
        if (request.sky.textures && !request.sky.textures->validBorrow(
            request.context, request.isCurrent, skyTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed sky texture publication/context/thread is no longer valid");
        if (request.cables.textures && !request.cables.textures->validBorrow(
            request.context, request.isCurrent, cableTexturePublication))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed cable texture publication/context/thread is no longer valid");
        return true;
    }
    bool usesTextures() const noexcept {
        if(request.raStaticWorld)return true;
        for (U i = 0; i < racerArchetypeCount; ++i)
            if (racerArchetypes[i].cables.textures) return true;
        return request.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures || request.cables.textures ||
            request.sky.textures || request.dynamicWorld.textures || request.enableOriginalUi ||
            request.enableOriginalGameplayBlur || request.gcParticleTextures || request.track.rcParticleWindow || request.track.gbParticleWindow || request.track.mcParticleWindow || (hasRaceHudResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources());
    }
    bool usesTrackShader() const noexcept {
        return request.track.mode == VehicleWindowMode50DS::sourceVertexColors ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.dynamicWorld.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.gameplayBinderTextures != nullptr;
    }
    bool createTrackMaterials(const VehicleWindowTrack50DS& input) noexcept {
        if (input.mode != VehicleWindowMode50DS::callerMaterialTextures) return true;
        trackSpans.reset(new (std::nothrow) TrackSpan[input.spanCount]);
        if (!trackSpans)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded source-ordered track span table");
        trackSpanCount = input.spanCount;
        for (U i = 0; i < trackSpanCount; ++i) {
            const auto& source = input.spans[i];
            auto& target = trackSpans[i];
            target.indexStart = source.indexStart;
            target.indexCount = source.indexCount;
            target.discardZeroAlpha = source.discardZeroAlpha;
            target.nativeCoverage = source.nativeCoverage;
            coverageDemand = coverageDemand || source.nativeCoverage == NativeTrackCoverage50DS::fourSampleAa1;
            target.sourceAlphaTest = source.sourceAlphaTest;
            target.sourceAlphaReference = source.sourceAlphaReference;
            target.sourceUvAnimation = source.sourceUvAnimation;
            target.allowPodShadow = source.allowPodShadow;
            target.sourcePass = source.sourcePass;
            target.sourcePassCount = source.sourcePassCount;
            target.projectionPass = source.projectionPass;
            target.rcCrystalPass = source.rcCrystalPass;
            // All source ranges/pass continuity were validated before creation.
            // For alpha-over pairs, fogging both colors with the same factor
            // yields fog(alpha*layer + (1-alpha)*base), without fogging alpha.
            target.worldFogEligible = source.sourceFogKnown ? source.sourceFogEnabled :
                (source.sourcePassCount == 1 && source.material.blend == VehicleWindowBlend50DS::opaque && source.material.depthWrite);
            if (source.sourcePassCount == 2 && !source.sourceFogKnown) {
                const U first = i - (source.sourcePass - 1);
                const auto& base = input.spans[first];
                const auto& layer = input.spans[first + 1];
                target.worldFogEligible = base.projectionPass == 1 && layer.projectionPass == 2 &&
                    base.sourcePass == 1 && layer.sourcePass == 2 &&
                    base.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                    layer.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                    base.material.blend == VehicleWindowBlend50DS::opaque && base.material.depthWrite &&
                    layer.material.blend == VehicleWindowBlend50DS::sourceAlpha &&
                    !base.material.fixedBlendAlpha && !layer.material.fixedBlendAlpha &&
                    base.material.unlit && layer.material.unlit &&
                    !base.discardZeroAlpha && !layer.discardZeroAlpha;
            }
            target.material.textureIndex = source.material.textureIndex;
            target.material.policy = source.material;
            target.material.name = source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                input.textures->texture(source.material.textureIndex) : 0;
            if (source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                !target.material.name) {
                diagnostic->trackSpan = i;
                diagnostic->texture = source.material.textureIndex;
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required track texture disappeared during cold preparation");
            }
        }
        return true;
    }
    bool createTrackAnimations(const VehicleWindowRequest50DS& input) noexcept {
        if (!input.trackAnimationCount) return true;
        trackAnimations.reset(new (std::nothrow) TrackAnimation[input.trackAnimationCount]);
        if (!trackAnimations) return fail(*diagnostic, VehicleWindowError50DS::allocation,
            "allocate bounded cold original track-animation table");
        trackAnimationCount = input.trackAnimationCount;
        for (U i = 0; i < trackAnimationCount; ++i) {
            const auto& source = input.trackAnimations[i]; auto& target = trackAnimations[i];
            target.sourceTextureIndex = source.sourceTextureIndex;
            target.firstTextureIndex = source.firstTextureIndex; target.frameCount = source.frameCount;
            for (U f = 0; f < target.frameCount; ++f) {
                target.names[f] = input.trackAnimationTextures->texture(target.firstTextureIndex+f);
                if (!target.names[f]) return fail(*diagnostic, VehicleWindowError50DS::material,
                    "original animation frame disappeared during cold preparation");
            }
            trackSpans[source.sourceSpanIndex].animation = i;
        }
        return true;
    }
    bool validateTrackAnimationFrames(const VehicleWindowFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (frame.trackAnimationFrameCount != trackAnimationCount ||
            bool(frame.trackAnimationFrames) != bool(trackAnimationCount))
            return fail(d, VehicleWindowError50DS::frame, "one explicit current frame per original track animation required");
        for (U i = 0; i < trackAnimationCount; ++i)
            if (frame.trackAnimationFrames[i] >= trackAnimations[i].frameCount)
                return fail(d, VehicleWindowError50DS::frame, "current track animation frame outside original stored extent");
        return true;
    }
    bool createDynamicWorldMaterials(const VehicleWindowTrack50DS& input) noexcept {
        if (input.mode != VehicleWindowMode50DS::callerMaterialTextures) return true;
        dynamicWorldSpans.reset(new (std::nothrow) TrackSpan[input.spanCount]);
        if (!dynamicWorldSpans)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded source-ordered dynamic-world span table");
        dynamicWorldSpanCount = input.spanCount;
        for (U i = 0; i < dynamicWorldSpanCount; ++i) {
            const auto& source = input.spans[i];
            auto& target = dynamicWorldSpans[i];
            target.indexStart = source.indexStart;
            target.indexCount = source.indexCount;
            target.discardZeroAlpha = source.discardZeroAlpha;
            target.sourceAlphaTest = source.sourceAlphaTest;
            target.sourceAlphaReference = source.sourceAlphaReference;
            target.sourceUvAnimation = source.sourceUvAnimation;
            target.allowPodShadow = source.allowPodShadow;
            target.sourcePass = source.sourcePass;
            target.sourcePassCount = source.sourcePassCount;
            target.projectionPass = source.projectionPass;
            target.material.textureIndex = source.material.textureIndex;
            target.material.policy = source.material;
            target.material.name = source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                input.textures->texture(source.material.textureIndex) : 0;
            if (source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture &&
                !target.material.name) {
                diagnostic->dynamicWorldSpan = i;
                diagnostic->texture = source.material.textureIndex;
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required dynamic-world texture disappeared during cold preparation");
            }
        }
        return true;
    }
    bool createSkyMaterials(const VehicleWindowSky50DS& input) noexcept {
        if (!input.mesh) return true;
        skySpans.reset(new (std::nothrow) TrackSpan[input.spanCount]);
        if (!skySpans) return fail(*diagnostic, VehicleWindowError50DS::allocation,
            "allocate bounded authored sky span table");
        skySpanCount = input.spanCount;
        for (U i = 0; i < skySpanCount; ++i) {
            const auto& source = input.spans[i];
            auto& target = skySpans[i];
            target.indexStart = source.indexStart;
            target.indexCount = source.indexCount;
            target.material.textureIndex = source.material.textureIndex;
            target.material.policy = source.material;
            target.material.name = source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                input.textures->texture(source.material.textureIndex) : 0;
            if (source.material.mode == VehicleWindowMaterialMode50DS::requiredTexture && !target.material.name)
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required sky texture disappeared during cold preparation");
        }
        return true;
    }
    bool createTrackVisibility(const VehicleWindowTrack50DS& input) noexcept {
        if (!input.mesh) return true;
        trackDrawCount = trackSpanCount ? trackSpanCount : 1;
        trackCandidateIndexCount = trackSpanCount ? 0 : static_cast<U>(input.mesh->indices.size());
        for (U i = 0; i < trackSpanCount; ++i) trackCandidateIndexCount += trackSpans[i].indexCount;
        if (!input.enableFrustumCulling) return true;
        if (input.cullingChunkTriangles) {
            // Validation already bounded the total, with the original material
            // and complete source-span partition requirements left unchanged.
            for (U i = 0; i < trackSpanCount; ++i)
                trackChunkCount += trackSpanChunkCount(trackSpans[i].indexCount,
                    trackSpans[i].material.name != 0, input.cullingChunkTriangles);
            trackChunks.reset(new (std::nothrow) TrackChunk[trackChunkCount]);
            if (!trackChunks)
                return fail(*diagnostic, VehicleWindowError50DS::allocation,
                    "allocate bounded source-ordered track triangle chunks");
            U nextChunk = 0;
            for (U i = 0; i < trackSpanCount; ++i) {
                const auto& span = trackSpans[i];
                U start = span.indexStart, remaining = span.indexCount;
                while (remaining) {
                    // Bound before multiplication, so even UINT32_MAX as the
                    // requested triangle ceiling cannot overflow an index count.
                    const U count = span.material.name ?
                        std::min(remaining / 3, input.cullingChunkTriangles) * 3 : remaining;
                    trackChunks[nextChunk++] = {i, start, count};
                    start += count;
                    remaining -= count;
                }
            }
            trackDrawCount = trackChunkCount;
        }
        // The validated candidates have at most 65536 draws. Allocate the complete
        // possible retained list once; submit never grows or allocates storage.
        trackDrawBounds.reset(new (std::nothrow) game::StaticDrawBounds50EI[trackDrawCount]);
        trackVisibleDrawIds.reset(new (std::nothrow) U[trackDrawCount]);
        if (!trackDrawBounds || !trackVisibleDrawIds)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded static track bounds and retained draw IDs");
        const auto& mesh = *input.mesh;
        for (U i = 0; i < trackDrawCount; ++i) {
            auto& draw = trackDrawBounds[i];
            draw.drawId = i;
            const U start = trackChunkCount ? trackChunks[i].indexStart :
                (trackSpanCount ? trackSpans[i].indexStart : 0);
            const U count = trackChunkCount ? trackChunks[i].indexCount :
                (trackSpanCount ? trackSpans[i].indexCount : static_cast<U>(mesh.indices.size()));
            // Index ranges are nonempty and validated before renderer ownership
            // or allocation. Ignore unreferenced vertices and untrusted mesh
            // display bounds; no root or model transform is applied a second time.
            const auto& first = mesh.vertices[mesh.indices[start]];
            for (U axis = 0; axis < 3; ++axis)
                draw.worldBounds.minimum[axis] = draw.worldBounds.maximum[axis] = first.position[axis];
            for (U n = start + 1; n < start + count; ++n) {
                const auto& vertex = mesh.vertices[mesh.indices[n]];
                for (U axis = 0; axis < 3; ++axis) {
                    draw.worldBounds.minimum[axis] = std::min(draw.worldBounds.minimum[axis], vertex.position[axis]);
                    draw.worldBounds.maximum[axis] = std::max(draw.worldBounds.maximum[axis], vertex.position[axis]);
                }
            }
        }
        return true;
    }
    void prepareTrackVisibility() noexcept {
        // Start each frame at the complete list. On helper failure its output
        // prefix is discarded by leaving this mode/count unchanged.
        frameTrackDrawCount = trackDrawCount;
        frameTrackCullingApplied = false;
        diagnostic->trackCandidateDraws = diagnostic->trackRetainedDraws = trackDrawCount;
        if (!request.track.enableFrustumCulling || !trackDrawCount) return;
        game::DrawVisibilityInput50EI input{};
        input.draws = trackDrawBounds.get(); input.drawCount = trackDrawCount;
        input.worldViewProjection = matrix; // Same copied bytes as both GL_FALSE uploads.
        input.clipDepth = game::DrawClipDepth50EI::negativeOneToOne;
        input.uncertainPolicy = game::DrawUncertainPolicy50EI::retain;
        const game::DrawVisibilityBuffer50EI buffer{trackVisibleDrawIds.get(), trackDrawCount};
        game::DrawVisibilityResult50EI result{};
        if (!game::collectVisibleStaticDraws50EI(input, buffer, result, diagnostic->trackVisibility)) {
            diagnostic->trackCullingFallback = true;
            return;
        }
        frameTrackDrawCount = result.retained;
        frameTrackCullingApplied = true;
        diagnostic->trackCullingApplied = true;
        diagnostic->trackRetainedDraws = result.retained;
        diagnostic->trackCulledDraws = result.culled;
        diagnostic->trackUncertainRetainedDraws = result.uncertainRetained;
        U retainedIndices = 0;
        for (U i = 0; i < result.retained; ++i) {
            const U draw = trackVisibleDrawIds[i];
            retainedIndices += trackChunkCount ? trackChunks[draw].indexCount :
                (trackSpanCount ? trackSpans[draw].indexCount : static_cast<U>(trackIndexCount));
        }
        diagnostic->trackCulledIndices = trackCandidateIndexCount - retainedIndices;
    }
    bool createMaterials(const VehicleWindowMaterials50DS& input) noexcept {
        if (request.mode != VehicleWindowMode50DS::callerMaterialTextures) return true;
        U total = 0;
        for (U c = 0; c < resources.componentCount; ++c) {
            materialOffsets[c] = total;
            total += input.component[c].count; // Already bounded to 5 * 4096.
        }
        materialCount = total;
        materials.reset(new (std::nothrow) Material[total]);
        if (!materials)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded cold material table");
        for (U c = 0; c < resources.componentCount; ++c) {
            for (U m = 0; m < input.component[c].count; ++m) {
                const auto& binding = input.component[c].bindings[m];
                auto& resolved = materials[materialOffsets[c] + m];
                resolved.textureIndex = binding.textureIndex;
                resolved.policy = binding;
                materialSummary[c].sourceDiffuse |= binding.sourceDiffuseCompressed50LA;
                materialSummary[c].sourcePair |= binding.sourceSpecularPair50LA;
                materialSummary[c].normalDependent |= podMaterialUsesNormals(binding);
                materialSummary[c].hasDeferredEffects |= binding.blend != VehicleWindowBlend50DS::opaque;
                resolved.name = binding.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                    input.textures->texture(binding.textureIndex) : 0;
                if (binding.sourceSpecularPair50LA) {
                    const auto* base = input.textures->asset(binding.textureIndex);
                    const auto* map2 = input.specularTextures50LA->asset(binding.specularTextureIndex50LA);
                    if (!base || !base->name || !map2 || !map2->name)
                        return fail(*diagnostic, VehicleWindowError50DS::material, "source pair disappeared during create");
                    resolved.textureIdentity50LA = base->identity;
                    resolved.specularName50LA = map2->name; resolved.specularIdentity50LA = map2->identity;
                    hasSpecularPairs50LA = true;
                }
                if (binding.mode == VehicleWindowMaterialMode50DS::requiredTexture && !resolved.name) {
                    diagnostic->component = c;
                    diagnostic->material = m;
                    diagnostic->texture = binding.textureIndex;
                    return fail(*diagnostic, VehicleWindowError50DS::material,
                        "required texture disappeared during cold material preparation");
                }
            }
        }
        return true;
    }
    bool validateMaterialUvs(const VehicleWindowMaterialUv50DS* uvs, U count,
        VehicleWindowDiagnostic50DS& d, U archetype = 0) const noexcept {
        const auto selected = racerView(archetype);
        if ((count && !uvs) || count > selected.materialCount || (!count && uvs))
            return fail(d, VehicleWindowError50DS::frame, "material UV array requires valid bounded entries");
        U previousKey = 0;
        for (U i = 0; i < count; ++i) {
            const auto& uv = uvs[i];
            const U c = componentIndex(uv.component);
            if (c >= selected.resources->componentCount || uv.materialIndex >= selected.scenes[c]->materials.size())
                return fail(d, VehicleWindowError50DS::frame, "material UV identity outside original scene");
            const U key = selected.materialOffsets[c] + uv.materialIndex;
            if (i && key <= previousKey)
                return fail(d, VehicleWindowError50DS::frame, "material UV array must be unique and source ordered");
            previousKey = key;
            if (selected.materials[key].policy.sourceSpecularPair50LA &&
                (uv.uScale != 1.f || uv.vScale != 1.f || uv.uOffset != 0.f || uv.vOffset != 0.f))
                return fail(d, VehicleWindowError50DS::material, "source pair requires its actual unmodified base UV");
            const float values[] = {uv.uScale, uv.vScale, uv.uOffset, uv.vOffset};
            for (float value : values)
                if (!std::isfinite(value))
                    return fail(d, VehicleWindowError50DS::frame, "nonfinite material UV transform");
        }
        return true;
    }
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
        U archetype = 0, RacerNormalReceipt* normalReceipt = nullptr) const noexcept {
        const auto selected = racerView(archetype);
        if (batch.componentCount != selected.resources->componentCount)
            return fail(d, VehicleWindowError50DS::assets, "batch component count differs from actual assets");
        for (U c = 0; c < batch.componentCount; ++c) {
            const auto& mesh = *selected.meshes[c];
            const auto& part = batch.component[c];
            if (part.indices != mesh.indices || part.indexCount != mesh.indexCount ||
                part.spans != mesh.spans || part.spanCount != mesh.spanCount || part.vertexCount != mesh.vertexCount)
                return fail(d, VehicleWindowError50DS::assets, "batch is not the retained prepared mesh");
            // Full semantic material identity, including unused entries, is
            // retained with the generated RGB. A subset mask must not relabel
            // imported RGB as source color or route generated RGB to legacy.
            const auto count = static_cast<U>(selected.scenes[c]->materials.size());
            const auto& summary = selected.materialSummary[c];
            const bool sourceDiffuse = summary.sourceDiffuse, sourcePair = summary.sourcePair;
            if (part.sourceSpecularGeometry50LA != sourcePair)
                return fail(d, VehicleWindowError50DS::frame, "source pair geometry differs from actual cold body policy");
            if (!sourceDiffuse) {
                if (part.sourceColorMaterialModes50LA || part.sourceColorMaterialCount50LA ||
                    part.sourceColorOwner50LA || part.sourceColorPublication50LA ||
                    part.sourceColorGraphicsSequence50LA || part.sourceColorRacerSlot50LA)
                    return fail(d, VehicleWindowError50DS::frame,
                        "legacy racer component must not carry source-color metadata");
                continue;
            }
            d.component = c;
            if (!sourceBodyComponent50LA(c, batch.componentCount) ||
                !part.sourceColorMaterialModes50LA || part.sourceColorMaterialCount50LA != count ||
                !part.sourceColorOwner50LA || !part.sourceColorPublication50LA ||
                !part.sourceColorGraphicsSequence50LA)
                return fail(d, VehicleWindowError50DS::frame,
                    "source diffuse requires the actual engine mask and light identity in a three/five-model pod");
            for (U m = 0; m < count; ++m) {
                d.material = m;
                const auto& binding = selected.materials[selected.materialOffsets[c] + m].policy;
                if (part.sourceColorMaterialModes50LA[m] !=
                    (binding.sourceDiffuseCompressed50LA ? 1u : 0u))
                    return fail(d, VehicleWindowError50DS::material,
                        "source-color material mask differs from the retained actual material policy");
            }
            for (U i = 0; i < part.spanCount; ++i) {
                const auto& span = part.spans[i];
                if (!span.index_count) continue;
                d.material = span.material_index;
                if (span.material_index >= count ||
                    part.sourceColorMaterialModes50LA[span.material_index] !=
                    (selected.materials[selected.materialOffsets[c] + span.material_index].
                        policy.sourceDiffuseCompressed50LA ? 1u : 0u))
                    return fail(d, VehicleWindowError50DS::material,
                        "source-color selected span differs from its retained material policy");
            }
        }
        d.component = maxComponents;
        d.material = assets::unresolvedMaterialTexture50EA;
        if (normalReceipt && normalReceipt->matches(batch)) return true;
        if (normalReceipt) normalReceipt->completedBatch = nullptr;
        if (!validateRacerNormals(batch, d)) return false;
        if (normalReceipt) normalReceipt->accept(batch);
        return true;
    }
    bool validateRacerNormals(const game::VehicleDrawBatch50CO& batch,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (!game::validVehicleDrawComponentCount50CM(batch.componentCount))
            return fail(d, VehicleWindowError50DS::frame, "invalid active racer component count");
        for (U c = 0; c < batch.componentCount; ++c) {
            const auto& part = batch.component[c];
            if (bool(part.sourceNormals) != bool(part.sourceNormalCount) ||
                (part.sourceNormals && part.sourceNormalCount != part.vertexCount))
                return fail(d, VehicleWindowError50DS::frame,
                    "racer normal stream must match its exact live vertex count");
            for (U n = 0; n < part.sourceNormalCount; ++n) {
                const auto& normal = part.sourceNormals[n].xyz;
                const double length = std::hypot(normal[0], normal[1], normal[2]);
                if (!std::isfinite(length) || (length != 0.0 && std::abs(length - 1.0) > 0.001))
                    return fail(d, VehicleWindowError50DS::frame,
                        "racer normals must be finite unit vectors or explicit zero");
            }
        }
        return true;
    }
    bool validateOpponents(const game::VehicleDrawBatch50CO& player, const VehicleWindowFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d,
        RacerNormalReceipt (&normalReceipts)[1 + maxVehicleWindowOpponents50DS]) const noexcept {
        if (frame.opponentCount > maxVehicleWindowOpponents50DS ||
            bool(frame.opponentCount) != bool(frame.opponents))
            return fail(d, VehicleWindowError50DS::frame, "opponents require an explicit bounded array of at most seven");
        if (!frame.opponentCount) return true;
        const auto capacity = adapter->backend().capacity;
        game::VehicleDrawPlan50CS plan{};
        if (!game::preflightVehicleDrawBatch50CS(player, capacity, plan, d.submission))
            return fail(d, VehicleWindowError50DS::submission, "player batch preflight rejected before opponent rendering");
        if (!retainedMesh(player, d, 0, &normalReceipts[0])) return false;
        for (U i = 0; i < frame.opponentCount; ++i) {
            d.opponentIndex = i;
            const auto& opponent = frame.opponents[i];
            if (!opponent.batch)
                return fail(d, VehicleWindowError50DS::frame, "opponent requires an actual successful batch");
            if (opponent.racerArchetype > racerArchetypeCount)
                return fail(d, VehicleWindowError50DS::frame, "opponent selects an unprepared racer archetype");
            const auto selected = racerView(opponent.racerArchetype);
            game::VehicleDrawCapacity50CS racerCapacity{};
            racerCapacity.commands = game::maxVehicleDrawCommands50CS;
            for (U c = 0; c < selected.resources->componentCount; ++c) {
                racerCapacity.vertices[c] = selected.meshes[c]->vertexCount;
                racerCapacity.indices[c] = selected.meshes[c]->indexCount;
            }
            if (!game::preflightVehicleDrawBatch50CS(*opponent.batch, racerCapacity, plan, d.submission))
                return fail(d, VehicleWindowError50DS::submission, "opponent batch preflight rejected");
            if (!retainedMesh(*opponent.batch, d, opponent.racerArchetype, &normalReceipts[i + 1]) ||
                !validateMaterialUvs(opponent.materialUvs, opponent.materialUvCount, d,
                    opponent.racerArchetype)) return false;
            if (opponent.cables && (!selected.cableTextureName || !validateCableFrame(*opponent.cables, d)))
                return fail(d, VehicleWindowError50DS::frame, "opponent cables require valid geometry and the cold cable bank");
            // Each independent live 50CO stream must survive until deferred
            // effects. Reject overlapping player/opponent/component storage.
            for (U c = 0; c < selected.resources->componentCount; ++c) {
                const auto& part = opponent.batch->component[c];
                const auto start = reinterpret_cast<std::uintptr_t>(part.vertices);
                const auto bytes = std::uintptr_t(part.vertexCount) * sizeof(assets::Vertex48B);
                if (start > std::numeric_limits<std::uintptr_t>::max() - bytes)
                    return fail(d, VehicleWindowError50DS::frame, "opponent vertex range overflow");
                for (U prior = 0; prior <= i + 1; ++prior) {
                    const auto& other = prior ? *frame.opponents[prior - 1].batch : player;
                    for (U k = 0; k < (prior == i + 1 ? c : other.componentCount); ++k) {
                        const auto begin = reinterpret_cast<std::uintptr_t>(other.component[k].vertices);
                        const auto size = std::uintptr_t(other.component[k].vertexCount) * sizeof(assets::Vertex48B);
                        if (begin > std::numeric_limits<std::uintptr_t>::max() - size ||
                            (start < begin + size && begin < start + bytes))
                            return fail(d, VehicleWindowError50DS::frame, "racers require disjoint live world-space vertex arrays");
                    }
                }
                for (U v = 0; v < part.vertexCount; ++v) {
                    for (float value : part.vertices[v].position)
                        if (!std::isfinite(value))
                            return fail(d, VehicleWindowError50DS::frame, "opponent contains nonfinite world-space position");
                    for (float value : part.vertices[v].uv)
                        if (!std::isfinite(value))
                            return fail(d, VehicleWindowError50DS::frame, "opponent contains nonfinite source UV");
                }
            }
        }
        d.opponentIndex = maxVehicleWindowOpponents50DS;
        return true;
    }
    bool copyNativeGcLights(NativeRacerLightingPolicy50DS policy,
        const game::NativeGcSelectedLightsCandidate* input, U expectedRacerSlot,
        const game::VehicleDrawBatch50CO& batch, U archetype,
        std::uintptr_t& fleetOwner, game::NativeGcSelectedLightsCandidate& output,
        VehicleWindowDiagnostic50DS& d, RacerNormalReceipt& normalReceipt) const noexcept {
        output = {};
        if (archetype > racerArchetypeCount)
            return fail(d, VehicleWindowError50DS::frame,
                "native GC packet requires the actual prepared racer archetype");
        if (!retainedMesh(batch, d, archetype, &normalReceipt)) return false;
        if (policy == NativeRacerLightingPolicy50DS::unchangedHost) {
            if (input)
                return fail(d, VehicleWindowError50DS::frame,
                    "unchanged host lighting requires no selected-light packet");
            for (U c = 0; c < batch.componentCount; ++c)
                if (batch.component[c].sourceColorMaterialModes50LA)
                    return fail(d, VehicleWindowError50DS::frame,
                        "unchanged host lighting cannot consume source-generated racer colors");
            return true;
        }
        if (policy != NativeRacerLightingPolicy50DS::nativeGcSelectedLights || !input ||
            request.mode != VehicleWindowMode50DS::callerMaterialTextures)
            return fail(d, VehicleWindowError50DS::frame,
                "native GC lighting requires explicit textured policy and a selected-light packet");
        // Copy once, then validate/use that copy only. A retained stopped racer
        // may legitimately have an older graphics sequence than another racer;
        // runtime must retain it with the exact geometry batch it describes.
        const game::NativeGcSelectedLightsCandidate packet = *input;
        if (!packet.ready() || packet.componentCount() != batch.componentCount || !packet.graphicsSequence() || packet.racerSlot() != expectedRacerSlot ||
            !packet.publication() || !packet.ownerIdentity() ||
            (fleetOwner && fleetOwner != packet.ownerIdentity()))
            return fail(d, VehicleWindowError50DS::frame,
                "native GC packet requires ready matching racer and shared owner identity");
        const auto selected = racerView(archetype);
        if (!selected.materials)
            return fail(d, VehicleWindowError50DS::material,
                "native GC lighting requires the actual retained material bank");
        for (U c = 0; c < batch.componentCount; ++c) {
            d.component = c;
            // Inspect actual nonempty draw spans only, not unused material
            // table entries. A requested lit opaque untextured surface cannot
            // silently fall back to the fixed-gray host light. Existing unlit,
            // source-prelit and transparent/additive paths remain excluded.
            const auto& geometry = batch.component[c];
            if (geometry.sourceColorMaterialModes50LA &&
                (geometry.sourceColorOwner50LA != packet.ownerIdentity() ||
                 geometry.sourceColorPublication50LA != packet.publication() ||
                 geometry.sourceColorGraphicsSequence50LA != packet.graphicsSequence() ||
                 geometry.sourceColorRacerSlot50LA != packet.racerSlot()))
                return fail(d, VehicleWindowError50DS::frame,
                    "source-generated colors require their exact selected-light publication");
            for (U i = 0; i < geometry.spanCount; ++i) {
                const auto& span = geometry.spans[i];
                if (!span.index_count) continue;
                if (span.material_index >= selected.scenes[c]->materials.size())
                    return fail(d, VehicleWindowError50DS::material,
                        "native GC draw span material is outside its actual racer scene");
                const auto& material = selected.materials[selected.materialOffsets[c] + span.material_index];
                const auto& binding = material.policy;
                if (binding.unlit || binding.sourcePrelit ||
                    binding.blend != VehicleWindowBlend50DS::opaque) continue;
                if (binding.mode != VehicleWindowMaterialMode50DS::requiredTexture ||
                    !material.name || material.textureIndex == assets::unresolvedMaterialTexture50EA) {
                    d.material = span.material_index;
                    d.texture = material.textureIndex;
                    return fail(d, VehicleWindowError50DS::material,
                        "native GC lit opaque draw requires its real retained texture; no host-light fallback");
                }
            }
            const auto* light = packet.component(c);
            if (!light)
                return fail(d, VehicleWindowError50DS::frame,
                    "native GC packet is missing the actual component selection");
            for (U row = 0; row < 4; ++row) {
                for (U lane = 0; lane < 4; ++lane) {
                    const float direction = light->directions[row][lane];
                    const float diffuse = light->diffuse[row][lane];
                    const float specular = light->specular[row][lane];
                    const auto rejectMatrixLane = [&](const char* operation) noexcept {
                        if (d.error == VehicleWindowError50DS::none) {
                            // Failure-only record; %a preserves every finite
                            // binary32 value without changing diagnostic ABI.
                            std::fprintf(stderr,
                                "native selected-light matrix rejection racer=%u component=%u row=%u lane=%u direction=%a diffuse=%a specular=%a\n",
                                static_cast<unsigned>(expectedRacerSlot), static_cast<unsigned>(c),
                                static_cast<unsigned>(row), static_cast<unsigned>(lane),
                                static_cast<double>(direction), static_cast<double>(diffuse),
                                static_cast<double>(specular));
                        }
                        return fail(d, VehicleWindowError50DS::frame, operation);
                    };
                    if (!std::isfinite(direction) || !std::isfinite(diffuse) ||
                        !std::isfinite(specular))
                        return rejectMatrixLane("native selected-light matrix contains a nonfinite lane");
                    if (std::abs(direction) > 1.f)
                        return rejectMatrixLane("native selected-light direction exceeds the admitted unit bound");
                    // These are selected floating-point light coefficients,
                    // not packed color bytes. Light::Init halves authored RGB,
                    // but 1498A0 copies zone sun overrides without halving and
                    // spatial selection divides local RGB by attenuation.
                    // 149A50 folds negative DIFFUSE into sun/ambient only;
                    // signed specular survives unchanged (TA negative spots).
                    // Preserve those values: the native shader consumes only
                    // diffuse RGB / 128, not this retained specular matrix.
                    if (diffuse < 0.f)
                        return rejectMatrixLane("native selected-light diffuse lane was not negative-light folded");
                    if ((row == 3 || lane == 3) && direction != 0.f)
                        return rejectMatrixLane("native GC selected direction fourth row and column must be zero");
                    if ((lane == 3 && diffuse != (row == 3 ? 64.f : 0.f)) ||
                        ((row == 3 || lane == 3) && specular != 0.f))
                        return rejectMatrixLane("native GC selected color rows do not match admitted source layout");
                }
            }
        }
        fleetOwner = packet.ownerIdentity();
        output = packet;
        d.component = maxComponents;
        return true;
    }
    bool hasDisplayFadeNames() const noexcept {
        return displayFadeSourceTarget || displayFadeSourceColor ||
            displayFadeTargets[0] || displayFadeTargets[1] || displayFadeColors[0] || displayFadeColors[1] ||
            displayFadeProgram || displayFadeVertexShader || displayFadeFragmentShader || displayFadeVertexArray;
    }
    bool loadDisplayFadeFunctions() noexcept {
        // Dormant on normal create/submit, independent of every UI opt-in.
        // All required pointers exist before the first name is allocated.
        return load(gl.activeTexture,"glActiveTexture") && load(gl.bindTexture,"glBindTexture") &&
            load(gl.genTextures,"glGenTextures") && load(gl.deleteTextures,"glDeleteTextures") &&
            load(gl.texImage2D,"glTexImage2D") && load(gl.texParameteri,"glTexParameteri") &&
            load(gl.getTexLevelParameteriv,"glGetTexLevelParameteriv") &&
            load(gl.framebufferTexture2D,"glFramebufferTexture2D") &&
            load(gl.blitFramebuffer,"glBlitFramebuffer") && load(gl.drawArrays,"glDrawArrays");
    }
    bool createDisplayFadeTarget(U& target,U& color,I w,I h) noexcept {
        gl.genFramebuffers(1,&target);
        if(!check("allocate display-fade framebuffer") || !target)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero display-fade framebuffer");
        gl.genTextures(1,&color);
        if(!check("allocate display-fade texture") || !color)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero display-fade texture");
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,color);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,w,h,0,rgba,unsignedByte,nullptr);
        if(!check("allocate actual-size display-fade RGBA8 storage"))return false;
        I actualW=0,actualH=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&actualW);
        gl.getTexLevelParameteriv(texture2D,0,0x1001,&actualH);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if(!check("verify display-fade texture storage"))return false;
        if(actualW!=w || actualH!=h || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"display-fade texture storage mismatch");
        gl.bindFramebuffer(framebuffer,target);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,color,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify display-fade framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete display-fade framebuffer");}
        return true;
    }
    bool createDisplayFadeProgram() noexcept {
        static const char* vertex=R"GLSL(#version 150 core
void main() {
    vec2 p=gl_VertexID==0 ? vec2(-1.0,-1.0) :
           gl_VertexID==1 ? vec2(3.0,-1.0) : vec2(-1.0,3.0);
    gl_Position=vec4(p,0.0,1.0);
}
)GLSL";
        static const char* fragment=R"GLSL(#version 150 core
uniform sampler2D immutableRaceImage;
uniform vec4 rgbaqBytes;
out vec4 viewerOutput;
int signedFloor128(int value) {
    return value>=0 ? value/128 : -((-value+127)/128);
}
void main() {
    // Source PRIM0x46 is an untextured full-size sprite. ALPHA0x8000000044
    // supplies (Cs-Cd)*As/128+Cd. RGBAQ low bytes are used verbatim, including
    // source packing's signed-lane OR effects; Q is irrelevant to this sprite.
    // Integer byte policy avoids normalized-GL-alpha255 interpolation.
    ivec4 source=ivec4(rgbaqBytes);
    ivec3 destination=ivec3(floor(texelFetch(immutableRaceImage,
        ivec2(gl_FragCoord.xy),0).rgb*255.0+0.5));
    ivec3 product=(source.rgb-destination)*source.a;
    ivec3 rgb=destination+ivec3(signedFloor128(product.r),
        signedFloor128(product.g),signedFloor128(product.b));
    viewerOutput=vec4(vec3(clamp(rgb,ivec3(0),ivec3(255))),float(source.a))/255.0;
}
)GLSL";
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compileTrackShader(0x8B31,vertex,displayFadeVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compileTrackShader(0x8B30,fragment,displayFadeFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        displayFadeProgram=shaderGl.createProgram();
        if(!check("create display-fade program") || !displayFadeProgram)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"zero display-fade program");
        shaderGl.attachShader(displayFadeProgram,displayFadeVertexShader);
        shaderGl.attachShader(displayFadeProgram,displayFadeFragmentShader);
        shaderGl.bindFragDataLocation(displayFadeProgram,vehicleGlColorOutput50CZ,"viewerOutput");
        shaderGl.linkProgram(displayFadeProgram);
        I linked=0;shaderGl.getProgramiv(displayFadeProgram,0x8B82,&linked);
        if(!check("link display-fade program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(displayFadeProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read display-fade program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"display-fade program link failed");
        }
        displayFadeImageUniform=shaderGl.getUniformLocation(displayFadeProgram,"immutableRaceImage");
        displayFadeColorUniform=shaderGl.getUniformLocation(displayFadeProgram,"rgbaqBytes");
        if(!check("locate display-fade uniforms"))return false;
        if(displayFadeImageUniform<0 || displayFadeColorUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing display-fade uniform");
        gl.genVertexArrays(1,&displayFadeVertexArray);
        if(!check("allocate display-fade vertex array") || !displayFadeVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero display-fade vertex array");
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;return true;
    }
    bool restoreDisplayFadeBindings(I w,I h) noexcept {
        // The existing native renderer forbids unrelated foreign GL state.
        // Restore its normal binding/depth baseline, never clear source depth.
        gl.useProgram(0);drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,0);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.bindFramebuffer(framebuffer,0);gl.viewport(0,0,w,h);
        gl.disable(0x0BE2);gl.disable(0x0C11);gl.enable(0x0B71);
        gl.depthMask(1);gl.depthFunc(0x0201);gl.depthRange(0,1);gl.clearDepth(1);
        gl.colorMask(1,1,1,1);
        return check("restore native display-fade renderer state without clearing depth");
    }
    bool deleteDisplayFadeNames(VehicleWindowDiagnostic50DS& d) noexcept {
        // Last complete publication may point at an effect surface. Call only
        // after a newer race publication or after close has invalidated it.
        displayFadeDeletionPending=true;
        if(!deleteShaderName(displayFadeProgram,true,d) ||
           !deleteShaderName(displayFadeVertexShader,false,d) ||
           !deleteShaderName(displayFadeFragmentShader,false,d) ||
           !deleteName(displayFadeVertexArray,gl.deleteVertexArrays,d))return false;
        for(U i=0;i<2;++i)
            if(!deleteName(displayFadeTargets[i],gl.deleteFramebuffers,d) ||
               !deleteName(displayFadeColors[i],gl.deleteTextures,d))return false;
        if(!deleteName(displayFadeSourceTarget,gl.deleteFramebuffers,d) ||
           !deleteName(displayFadeSourceColor,gl.deleteTextures,d))return false;
        displayFadeDeletionPending=false;return true;
    }
    bool hasGlResources() const noexcept {
        if(garagePollFence50KG)return true;
        for(U name:garageSamplers50KG)if(name)return true;
        if(hasRaWorldResources())return true;
        if(hasCoverageResources()||uiCoverage.owns())return true;
        if(hasRaceHudResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources())return true;
        if(hasDisplayFadeNames())return true;
        if (hasGameplayBlurResources()) return true;
        if (shader && shader->hasResources()) return true;
        if (trackProgram || trackVertexShader || trackFragmentShader) return true;
        if (hostHudProgram || hostHudVertexShader || hostHudFragmentShader ||
            hostHudVertexBuffer || hostHudIndexBuffer || hostHudVertexArray) return true;
        if (uiProgram || uiVertexShader || uiFragmentShader ||
            uiVertexBuffer || uiIndexBuffer || uiVertexArray || uiParticleFogBuffer || uiSourceRasterWBuffer || uiSourcePacketStqBuffer ||
            uiSourceIndexBuffer) return true;
        for (U b = 0; b < 2; ++b)
            if (uiTargets[b] || uiColors[b] || uiDepths[b]) return true;
        if (uiSourceTarget || uiSourceColor || uiSourceDepth || uiRetainedTarget || uiRetainedColor) return true;
        if (uiBlendIncomingTarget || uiBlendIncomingColor || uiBlendEffectTarget || uiBlendEffectColor ||
            uiBlendProgram || uiBlendVertexShader || uiBlendFragmentShader || uiBlendVertexArray) return true;
        if (uiPromotedIncomingTarget || uiPromotedIncomingColor) return true;
        if (cableVertexBuffer || cableIndexBuffer || cableVertexArray) return true;
        if (trackVertexBuffer || trackIndexBuffer || trackVertexArray || trackNormalBuffer || trackProjectionBuffer) return true;
        if (dynamicWorldIndexBuffer) return true;
        for (U b = 0; b < 2; ++b)
            if (dynamicWorldVertexBuffers[b] || dynamicWorldNormalBuffers[b] || dynamicWorldArrays[b]) return true;
        if (skyVertexBuffer || skyIndexBuffer || skyVertexArray) return true;
        for (U b = 0; b < 2; ++b) {
            if (sourcePairColors50LA[b]) return true;
            for (const auto& body : sourceSpecularGeometry50LA)
                if (body.buffers[b] || body.fogBuffers[b]) return true;
        }
        for (U b = 0; b < 2; ++b) {
            if (targets[b] || colors[b] || depths[b] || menuTargets[b] || menuColors[b]) return true;
            for (U c = 0; c < maxComponents; ++c)
                if (resources.bank[b].vertexBuffer[c] || resources.bank[b].vertexArray[c] ||
                    racerNormalBuffers[b][c]) return true;
        }
        for (const auto& index : resources.indices) if (index.buffer) return true;
        for (const auto& archetype : racerArchetypes) {
            for (const auto& body : archetype.specularGeometry50LA) {
                for (U name : body.buffers) if (name) return true;
                for (U name : body.fogBuffers) if (name) return true;
            }
            for (const auto& bank : archetype.normalBuffers)
                for (U normal : bank) if (normal) return true;
            for (const auto& bank : archetype.resources.bank)
                for (U c = 0; c < maxComponents; ++c)
                    if (bank.vertexBuffer[c] || bank.vertexArray[c]) return true;
            for (const auto& index : archetype.resources.indices) if (index.buffer) return true;
        }
        return false;
    }
    bool check(const char* operation) noexcept {
        const U error = drawGl.getError();
        if (!error) return true;
        nativeGlFaultObserved50KG=true;
        if (!diagnostic->glError) diagnostic->glError = error;
        return fail(*diagnostic, VehicleWindowError50DS::gl, operation);
    }
    template<class F> bool load(F& pointer, const char* name) noexcept {
        pointer = reinterpret_cast<F>(request.procAddress(request.context, name));
        return pointer || fail(*diagnostic, VehicleWindowError50DS::entryPoint, name);
    }
    bool loadFunctions() noexcept {
#define LOAD(group, member, suffix) if (!load(group.member, "gl" #suffix)) return false
        LOAD(drawGl, bindBuffer, BindBuffer); LOAD(drawGl, bufferSubData, BufferSubData);
        LOAD(drawGl, bindVertexArray, BindVertexArray);
        LOAD(drawGl, drawElementsBaseVertex, DrawElementsBaseVertex);
        LOAD(drawGl, isBuffer, IsBuffer); LOAD(drawGl, isVertexArray, IsVertexArray);
        LOAD(drawGl, getError, GetError);
        LOAD(shaderGl, createShader, CreateShader); LOAD(shaderGl, shaderSource, ShaderSource);
        LOAD(shaderGl, compileShader, CompileShader); LOAD(shaderGl, getShaderiv, GetShaderiv);
        LOAD(shaderGl, getShaderInfoLog, GetShaderInfoLog); LOAD(shaderGl, deleteShader, DeleteShader);
        LOAD(shaderGl, createProgram, CreateProgram); LOAD(shaderGl, attachShader, AttachShader);
        LOAD(shaderGl, bindAttribLocation, BindAttribLocation);
        LOAD(shaderGl, bindFragDataLocation, BindFragDataLocation);
        LOAD(shaderGl, linkProgram, LinkProgram); LOAD(shaderGl, getProgramiv, GetProgramiv);
        LOAD(shaderGl, getProgramInfoLog, GetProgramInfoLog);
        LOAD(shaderGl, getUniformLocation, GetUniformLocation);
        LOAD(shaderGl, deleteProgram, DeleteProgram); shaderGl.getError = drawGl.getError;
        LOAD(gl, genBuffers, GenBuffers); LOAD(gl, deleteBuffers, DeleteBuffers);
        LOAD(gl, bufferData, BufferData); LOAD(gl, getBufferParameteri64v, GetBufferParameteri64v);
        LOAD(gl, getBufferParameteriv, GetBufferParameteriv); LOAD(gl, getBufferSubData, GetBufferSubData);
        LOAD(gl, genVertexArrays, GenVertexArrays); LOAD(gl, deleteVertexArrays, DeleteVertexArrays);
        LOAD(gl, enableVertexAttribArray, EnableVertexAttribArray);
        LOAD(gl, vertexAttribPointer, VertexAttribPointer); LOAD(gl, getVertexAttribiv, GetVertexAttribiv);
        LOAD(gl, getVertexAttribPointerv, GetVertexAttribPointerv);
        LOAD(gl, genFramebuffers, GenFramebuffers); LOAD(gl, deleteFramebuffers, DeleteFramebuffers);
        LOAD(gl, bindFramebuffer, BindFramebuffer); LOAD(gl, checkFramebufferStatus, CheckFramebufferStatus);
        LOAD(gl, framebufferRenderbuffer, FramebufferRenderbuffer);
        LOAD(gl, genRenderbuffers, GenRenderbuffers); LOAD(gl, deleteRenderbuffers, DeleteRenderbuffers);
        LOAD(gl, bindRenderbuffer, BindRenderbuffer); LOAD(gl, renderbufferStorage, RenderbufferStorage);
        LOAD(gl, getRenderbufferParameteriv, GetRenderbufferParameteriv);
        LOAD(gl, getIntegerv, GetIntegerv); LOAD(gl, useProgram, UseProgram);
        LOAD(gl, uniformMatrix4fv, UniformMatrix4fv); LOAD(gl, uniform1i, Uniform1i);
        LOAD(gl, uniform1f, Uniform1f); LOAD(gl, uniform4f, Uniform4f);
        LOAD(gl, blendFunc, BlendFunc); LOAD(gl, blendEquation, BlendEquation);
        LOAD(gl, viewport, Viewport); LOAD(gl, enable, Enable); LOAD(gl, disable, Disable);
        LOAD(gl, depthFunc, DepthFunc); LOAD(gl, depthMask, DepthMask);
        LOAD(gl, depthRange, DepthRange); LOAD(gl, clearDepth, ClearDepth);
        LOAD(gl, colorMask, ColorMask); LOAD(gl, polygonMode, PolygonMode);
        LOAD(gl, clearColor, ClearColor); LOAD(gl, clear, Clear);
        LOAD(gl, drawBuffer, DrawBuffer); LOAD(gl, readBuffer, ReadBuffer);
        LOAD(gl, pixelStorei, PixelStorei); LOAD(gl, readPixels, ReadPixels);
        LOAD(gl, finish, Finish);
        LOAD(gl, getString, GetString);
        // Host pause menus copy the retained race image between private FBOs
        // in headless mode too. Resolve before exposing either blit path.
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer || request.enableHostHud || request.enableOriginalUi || request.enableOriginalGameplayBlur) {
            LOAD(gl, blitFramebuffer, BlitFramebuffer);
        }
        if (request.enableOriginalUi || request.gcParticleTextures || request.track.rcParticleWindow || request.track.gbParticleWindow || request.track.mcParticleWindow) {
            LOAD(gl, scissor, Scissor); LOAD(gl, getTexParameteriv, GetTexParameteriv);
        }
        if (usesTextures()) {
            LOAD(gl, activeTexture, ActiveTexture); LOAD(gl, bindTexture, BindTexture);
        }
        if (request.enableOriginalUiSourceRgba || request.enableOriginalGameplayBlur || hasSpecularPairs50LA) {
            LOAD(gl, genTextures, GenTextures); LOAD(gl, deleteTextures, DeleteTextures);
            LOAD(gl, texImage2D, TexImage2D); LOAD(gl, texParameteri, TexParameteri);
            LOAD(gl, getTexLevelParameteriv, GetTexLevelParameteriv);
            LOAD(gl, framebufferTexture2D, FramebufferTexture2D);
        }
        if (request.enableOriginalGameplayBlur) {
            LOAD(gl, drawArrays, DrawArrays); LOAD(gl, uniform2i, Uniform2i); LOAD(gl, uniform4i, Uniform4i);
        }
#undef LOAD
        return true;
    }
    bool compileTrackShader(U type, const char* source, U& name) noexcept {
        name = shaderGl.createShader(type);
        if (!check("create source-color track shader")) return false;
        if (!name) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero track shader name");
        shaderGl.shaderSource(name, 1, &source, nullptr);
        shaderGl.compileShader(name);
        I compiled = 0;
        shaderGl.getShaderiv(name, 0x8B81, &compiled); // GL_COMPILE_STATUS
        if (!check("compile source-color track shader")) return false;
        if (compiled) return true;
        shaderGl.getShaderInfoLog(name, static_cast<I>(sizeof(diagnostic->shader.log)),
            nullptr, diagnostic->shader.log);
        diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
        check("read source-color track shader log");
        return fail(*diagnostic, VehicleWindowError50DS::shader, "source-color track shader compilation failed");
    }
    bool createTrackShader() noexcept {
        if (!usesTrackShader()) return true;
        // Unchanged positions/UV/RGB/index stream, optional authenticated
        // normals in a separate VBO. No mesh welding or fabricated detail.
        // Missing normals preserve the prior derivative-flat display policy.
        // This explicit host illumination is not original-game shader parity.
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec2 rrUv;
in vec4 rrColor;
in vec3 rrWorldNormal;
in vec4 rrProjectedUv;
uniform int projectionPass;
uniform int rcCrystalPass;
uniform vec4 trackUvTranslation;
uniform int worldFogEnabled;
uniform vec4 worldFogCamera;
uniform vec4 worldFogRange;
out float trackFogAmount;
uniform mat4 worldViewProjection;
out vec3 trackWorldNormal;
out vec3 trackWorldPosition;
out vec4 trackSourceColor;
noperspective out vec4 rcSourceColor;
out vec2 trackSourceUv;
void main() {
    gl_Position = vec4(rrPosition, 1.0) * worldViewProjection;
    trackWorldPosition = rrPosition;
    // Native radial world distance per vertex. No clip-W/perspective scale.
    trackFogAmount = worldFogEnabled != 0 ? clamp(
        (length(rrPosition - worldFogCamera.xyz) - worldFogRange.x) /
        (worldFogRange.y - worldFogRange.x), 0.0, 1.0) : 0.0;
    trackWorldNormal = rrWorldNormal;
    trackSourceColor = rrColor;
    rcSourceColor = rrColor;
    trackSourceUv = rcCrystalPass == 2 ? rrProjectedUv.xy : projectionPass == 1 ? rrProjectedUv.xy :
        (projectionPass == 2 ? rrProjectedUv.zw : rrUv);
    trackSourceUv += trackUvTranslation.xy;
}
)GLSL";
        constexpr const char* fragmentSource = R"GLSL(#version 150 core
in vec3 trackWorldPosition;
in float trackFogAmount;
uniform int worldFogEnabled;
uniform vec4 worldFogColor;
in vec3 trackWorldNormal;
in vec4 trackSourceColor;
noperspective in vec4 rcSourceColor;
in vec2 trackSourceUv;
uniform int rcCrystalPass;
uniform int rcCrystalPayload;
uniform vec4 rcCrystalAmbient;
out float rcCenterDepth;
uniform float vertexColorScale;
uniform int useTexture;
uniform int discardZeroAlpha;
uniform float sourceAlphaReference;
uniform int effectMode;
uniform float alphaScale;
uniform int unlit;
uniform int useSourceNormals;
uniform sampler2D image;
out vec4 viewerOutput;
)GLSL" RR_NATIVE_POD_SHADOW_GLSL50HM R"GLSL(
void main() {
    if(rcCrystalPass!=0){
        vec4 vertexByte=rcCrystalPass==2?rcCrystalAmbient:clamp(rcSourceColor*255.0,0.0,255.0);
        vec4 source=floor(clamp(texture(image,trackSourceUv)*255.0*vertexByte/128.0,0.0,255.0));
        if(discardZeroAlpha==2&&source.a<=sourceAlphaReference)discard;
        // Explicit existing native radial-fog presentation, not GS raster parity.
        if(worldFogEnabled!=0)source.rgb=floor(mix(source.rgb,worldFogColor.rgb*255.0,trackFogAmount));
        rcCenterDepth=gl_FragCoord.z;
        viewerOutput=rcCrystalPayload!=0?source:vec4(0.0);
        return;
    }
    vec3 plane = cross(dFdx(trackWorldPosition), dFdy(trackWorldPosition));
    float squaredLength = dot(plane, plane);
    float illumination = 0.45;
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 lightDirection = normalize(vec3(0.35, 0.85, 0.30));
    if (squaredLength > 0.0) {
        normal = plane * inversesqrt(squaredLength);
        illumination += 0.55 * abs(dot(normal, lightDirection));
    }
    if (useSourceNormals != 0) {
        float authoredLength = dot(trackWorldNormal, trackWorldNormal);
        if (authoredLength > 0.000001) {
            normal = trackWorldNormal * inversesqrt(authoredLength);
        } else if (!gl_FrontFacing) {
            // Correct derivative orientation only for the explicit zero-normal
            // fallback. Authored normal signs do not follow the camera.
            normal = -normal;
        }
        float skyAmbient = 0.32 + 0.16 * max(normal.y, 0.0);
        illumination = skyAmbient + 0.52 * max(dot(normal, lightDirection), 0.0);
    }
    vec3 color = trackSourceColor.rgb * vertexColorScale * (unlit != 0 ? 1.0 : illumination);
    float alpha = trackSourceColor.a;
    if (useTexture != 0) {
        vec4 texel = texture(image, trackSourceUv);
        if (discardZeroAlpha == 1 && texel.a <= 0.0) discard;
        color *= texel.rgb;
        alpha *= texel.a;
    }
    // Declared native GS-unit alpha policy. Original bytes remain unchanged;
    // native interpolation/raster precision is not a PS2 fragment-parity claim.
    float sourceAlphaByte = floor(clamp(alpha * (255.0 * 255.0 / 128.0), 0.0, 255.0));
    if (discardZeroAlpha == 2 && sourceAlphaByte <= sourceAlphaReference) discard;
    color *= rrNativePodShadow(trackWorldPosition, normal);
    if (effectMode == 2) color *= alphaScale;
    if (worldFogEnabled != 0) color = mix(color, worldFogColor.rgb, trackFogAmount);
    viewerOutput = vec4(color, effectMode == 1 ? clamp(alpha * alphaScale, 0.0, 1.0) : 1.0);
}
)GLSL";
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
        if (!compileTrackShader(0x8B31, vertexSource, trackVertexShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
        if (!compileTrackShader(0x8B30, fragmentSource, trackFragmentShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
        trackProgram = shaderGl.createProgram();
        if (!check("create source-color track program")) return false;
        if (!trackProgram) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero track program name");
        shaderGl.attachShader(trackProgram, trackVertexShader);
        shaderGl.attachShader(trackProgram, trackFragmentShader);
        shaderGl.bindAttribLocation(trackProgram, vehicleGlPositionAttribute50CZ, "rrPosition");
        shaderGl.bindAttribLocation(trackProgram, vehicleGlUvAttribute50CZ, "rrUv");
        shaderGl.bindAttribLocation(trackProgram, vehicleGlColorAttribute50CZ, "rrColor");
        shaderGl.bindAttribLocation(trackProgram, 3, "rrWorldNormal");
        shaderGl.bindAttribLocation(trackProgram, 4, "rrProjectedUv");
        shaderGl.bindFragDataLocation(trackProgram, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.bindFragDataLocation(trackProgram,1,"rcCenterDepth");
        shaderGl.linkProgram(trackProgram);
        I linked = 0;
        shaderGl.getProgramiv(trackProgram, 0x8B82, &linked); // GL_LINK_STATUS
        if (!check("link source-color track program")) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(trackProgram, static_cast<I>(sizeof(diagnostic->shader.log)),
                nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
            check("read source-color track link log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "source-color track program link failed");
        }
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::uniforms;
        rcCrystalPassUniform=shaderGl.getUniformLocation(trackProgram,"rcCrystalPass");
        rcCrystalPayloadUniform=shaderGl.getUniformLocation(trackProgram,"rcCrystalPayload");
        rcCrystalAmbientUniform=shaderGl.getUniformLocation(trackProgram,"rcCrystalAmbient");
        worldFogEnabledUniform = shaderGl.getUniformLocation(trackProgram, "worldFogEnabled");
        worldFogCameraUniform = shaderGl.getUniformLocation(trackProgram, "worldFogCamera");
        worldFogColorUniform = shaderGl.getUniformLocation(trackProgram, "worldFogColor");
        worldFogRangeUniform = shaderGl.getUniformLocation(trackProgram, "worldFogRange");
        trackMatrixUniform = shaderGl.getUniformLocation(trackProgram, "worldViewProjection");
        trackColorScaleUniform = shaderGl.getUniformLocation(trackProgram, "vertexColorScale");
        trackUseTextureUniform = shaderGl.getUniformLocation(trackProgram, "useTexture");
        trackImageUniform = shaderGl.getUniformLocation(trackProgram, "image");
        trackDiscardZeroAlphaUniform = shaderGl.getUniformLocation(trackProgram, "discardZeroAlpha");
        trackEffectModeUniform = shaderGl.getUniformLocation(trackProgram, "effectMode");
        trackAlphaScaleUniform = shaderGl.getUniformLocation(trackProgram, "alphaScale");
        trackUnlitUniform = shaderGl.getUniformLocation(trackProgram, "unlit");
        trackUseSourceNormalsUniform = shaderGl.getUniformLocation(trackProgram, "useSourceNormals");
        trackProjectionPassUniform = shaderGl.getUniformLocation(trackProgram, "projectionPass");
        trackUvTranslationUniform = shaderGl.getUniformLocation(trackProgram, "trackUvTranslation");
        trackAlphaReferenceUniform = shaderGl.getUniformLocation(trackProgram, "sourceAlphaReference");
        nativeShadowEnabledUniform = shaderGl.getUniformLocation(trackProgram, "nativePodShadowEnabled");
        nativeShadowOpponentCountUniform = shaderGl.getUniformLocation(trackProgram, "nativePodShadowOpponentCount");
        for (U c = 0; c < nativeShadowCasterCount; ++c) {
            char center[40]{}, shape[40]{};
            std::snprintf(center, sizeof(center), "nativePodShadowCenter[%u]", c);
            std::snprintf(shape, sizeof(shape), "nativePodShadowShape[%u]", c);
            nativeShadowCenterUniforms[c] = shaderGl.getUniformLocation(trackProgram, center);
            nativeShadowShapeUniforms[c] = shaderGl.getUniformLocation(trackProgram, shape);
            if (nativeShadowCenterUniforms[c] < 0 || nativeShadowShapeUniforms[c] < 0)
                return fail(*diagnostic, VehicleWindowError50DS::shader, "native shadow caster uniform missing");
        }
        for (U i = 0; i < maxVehicleWindowOpponents50DS; ++i) {
            char bounds[48]{};
            std::snprintf(bounds, sizeof(bounds), "nativePodShadowOpponentBounds[%u]", i);
            nativeShadowOpponentBoundsUniforms[i] = shaderGl.getUniformLocation(trackProgram, bounds);
            if (nativeShadowOpponentBoundsUniforms[i] < 0)
                return fail(*diagnostic, VehicleWindowError50DS::shader, "native opponent shadow bounds uniform missing");
        }
        if (!check("query source-color track uniforms")) return false;
        if (rcCrystalPassUniform<0||rcCrystalPayloadUniform<0||rcCrystalAmbientUniform<0||
            worldFogEnabledUniform < 0 || worldFogCameraUniform < 0 ||
            worldFogColorUniform < 0 || worldFogRangeUniform < 0 ||
            trackMatrixUniform < 0 || trackColorScaleUniform < 0 ||
            trackUseTextureUniform < 0 || trackImageUniform < 0 || trackDiscardZeroAlphaUniform < 0 ||
            trackEffectModeUniform < 0 || trackAlphaScaleUniform < 0 || trackUnlitUniform < 0 ||
            trackUseSourceNormalsUniform < 0 || trackProjectionPassUniform < 0 || trackUvTranslationUniform < 0 || trackAlphaReferenceUniform < 0 || nativeShadowEnabledUniform < 0 || nativeShadowOpponentCountUniform < 0)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "source-color track uniform missing");
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
        return true;
    }
    bool verifyBuffer(U target, std::size_t bytes, U usage) noexcept {
        std::int64_t actualBytes = 0;
        I actualUsage = 0;
        gl.getBufferParameteri64v(target, bufferSize, &actualBytes);
        gl.getBufferParameteriv(target, bufferUsage, &actualUsage);
        if (!check("verify buffer allocation")) return false;
        return (actualBytes == static_cast<std::int64_t>(bytes) && actualUsage == static_cast<I>(usage)) ||
            fail(*diagnostic, VehicleWindowError50DS::allocation, "buffer size/usage mismatch");
    }
    bool verifyLayout(U vertexBuffer, U indices) noexcept {
        constexpr I sizes[] = {3, 2, 4};
        constexpr U types[] = {floatType, floatType, unsignedByte};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        // enabled, size, stride, type, normalized, VBO association, integer.
        for (U a = 0; a < 3; ++a) {
            const I expected[] = {1, sizes[a], 24, static_cast<I>(types[a]), a == 2 ? 1 : 0,
                static_cast<I>(vertexBuffer), 0};
            for (U p = 0; p < 7; ++p) {
                I value = 0;
                gl.getVertexAttribiv(a, properties[p], &value);
                if (!check("glGetVertexAttribiv")) return false;
                if (value != expected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                    "vertex attribute layout mismatch");
            }
            void* pointer = nullptr;
            gl.getVertexAttribPointerv(a, 0x8645, &pointer);
            if (!check("glGetVertexAttribPointerv")) return false;
            if (reinterpret_cast<std::uintptr_t>(pointer) != offsets[a])
                return fail(*diagnostic, VehicleWindowError50DS::layout, "vertex attribute offset mismatch");
        }
        I element = 0;
        gl.getIntegerv(0x8895, &element);
        if (!check("verify VAO element buffer")) return false;
        return static_cast<U>(element) == indices ||
            fail(*diagnostic, VehicleWindowError50DS::layout, "VAO element association mismatch");
    }
    bool createGeometry(VehicleGlResources50CV& resources,
        const game::VehicleDrawMesh50CM* const* meshes) noexcept {
        for (U c = 0; c < resources.componentCount; ++c) {
            diagnostic->component = c;
            const auto& mesh = *meshes[c];
            auto& index = resources.indices[c];
            index.data = mesh.indices; index.count = mesh.indexCount;
            const std::size_t indexBytes = std::size_t(index.count) * sizeof(U);
            gl.genBuffers(1, &index.buffer);
            // ARRAY_BUFFER can initialize an EBO without changing VAO state.
            drawGl.bindBuffer(arrayBuffer, index.buffer);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(indexBytes), index.data, staticDraw);
            if (!check("create immutable index buffer")) return false;
            if (!index.buffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero index buffer name");
            if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
            // Cold, exact GPU-side contents check; only 1 KiB stack scratch.
            U words[256];
            for (U start = 0; start < index.count;) {
                const U count = std::min<U>(256, index.count - start);
                gl.getBufferSubData(arrayBuffer, std::intptr_t(start) * sizeof(U),
                    std::intptr_t(count) * sizeof(U), words);
                if (!check("verify retained index bytes")) return false;
                if (std::memcmp(words, index.data + start, std::size_t(count) * sizeof(U)))
                    return fail(*diagnostic, VehicleWindowError50DS::indices, "GPU index contents mismatch");
                start += count;
            }
            for (auto& bank : resources.bank) {
                bank.vertexCapacity[c] = mesh.vertexCount;
                gl.genBuffers(1, &bank.vertexBuffer[c]);
                drawGl.bindBuffer(arrayBuffer, bank.vertexBuffer[c]);
                const std::size_t bytes = std::size_t(mesh.vertexCount) * sizeof(assets::Vertex48B);
                gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), nullptr, dynamicDraw);
                if (!check("allocate inactive vertex bank")) return false;
                if (!bank.vertexBuffer[c]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero vertex buffer name");
                if (!verifyBuffer(arrayBuffer, bytes, dynamicDraw)) return false;
                gl.genVertexArrays(1, &bank.vertexArray[c]);
                drawGl.bindVertexArray(bank.vertexArray[c]);
                drawGl.bindBuffer(elementBuffer, index.buffer);
                constexpr I sizes[] = {3, 2, 4};
                constexpr std::uintptr_t offsets[] = {0, 12, 20};
                for (U a = 0; a < 3; ++a) {
                    gl.enableVertexAttribArray(a);
                    gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                        a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
                }
                if (!check("create Vertex48B VAO")) return false;
                if (!bank.vertexArray[c]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero vertex array name");
                if (!verifyLayout(bank.vertexBuffer[c], index.buffer)) return false;
            }
        }
        diagnostic->component = maxComponents;
        return true;
    }
    bool createStaticGeometry(const assets::Mesh48B* mesh, U& trackVertexBuffer,
        U& trackIndexBuffer, U& trackVertexArray, I& trackIndexCount,
        U vertexUsage = staticDraw, bool reuseIndices = false) noexcept {
        if (!mesh) return true;
        const std::size_t vertexBytes = mesh->vertices.size() * sizeof(assets::Vertex48B);
        const std::size_t indexBytes = mesh->indices.size() * sizeof(U);
        // The complete input was validated before any renderer/GL allocation.
        // Use ARRAY_BUFFER for both uploads so no other VAO's EBO is changed.
        U* const buffers[] = {&trackVertexBuffer, &trackIndexBuffer};
        const void* const inputs[] = {mesh->vertices.data(), mesh->indices.data()};
        const std::size_t bufferBytes[] = {vertexBytes, indexBytes};
        std::uint8_t scratch[1024];
        for (U b = 0; b < 2; ++b) {
            if (b == 1 && reuseIndices) continue;
            const U usage = b == 0 ? vertexUsage : staticDraw;
            gl.genBuffers(1, buffers[b]);
            drawGl.bindBuffer(arrayBuffer, *buffers[b]);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bufferBytes[b]), inputs[b], usage);
            if (!check("upload static world-space track buffer")) return false;
            if (!*buffers[b])
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero track buffer name");
            if (!verifyBuffer(arrayBuffer, bufferBytes[b], usage)) return false;
            const auto* source = static_cast<const std::uint8_t*>(inputs[b]);
            for (std::size_t offset = 0; offset < bufferBytes[b];) {
                const std::size_t count = std::min(sizeof(scratch), bufferBytes[b] - offset);
                gl.getBufferSubData(arrayBuffer, static_cast<std::intptr_t>(offset),
                    static_cast<std::intptr_t>(count), scratch);
                if (!check("verify uploaded track bytes")) return false;
                if (std::memcmp(scratch, source + offset, count))
                    return fail(*diagnostic, b == 0 ? VehicleWindowError50DS::assets :
                        VehicleWindowError50DS::indices, "GPU track contents mismatch");
                offset += count;
            }
        }
        gl.genVertexArrays(1, &trackVertexArray);
        drawGl.bindVertexArray(trackVertexArray);
        drawGl.bindBuffer(arrayBuffer, trackVertexBuffer);
        drawGl.bindBuffer(elementBuffer, trackIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create static track Vertex48B VAO")) return false;
        if (!trackVertexArray)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero track vertex array name");
        if (!verifyLayout(trackVertexBuffer, trackIndexBuffer)) return false;
        trackIndexCount = static_cast<I>(mesh->indices.size());
        return true;
    }
    bool createWorldNormals(const VehicleWindowTrack50DS& track, U& trackNormalBuffer,
        U trackVertexArray, U usage = staticDraw) noexcept {
        if (!track.sourceNormals) return true;
        const std::size_t bytes = std::size_t(track.sourceNormalCount) * sizeof(assets::WorldNormal50HK);
        gl.genBuffers(1, &trackNormalBuffer);
        drawGl.bindBuffer(arrayBuffer, trackNormalBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), track.sourceNormals, usage);
        if (!check("upload authenticated track source normals")) return false;
        if (!trackNormalBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero source normal buffer name");
        if (!verifyBuffer(arrayBuffer, bytes, usage)) return false;
        std::uint8_t scratch[1024];
        const auto* source = reinterpret_cast<const std::uint8_t*>(track.sourceNormals);
        for (std::size_t offset = 0; offset < bytes;) {
            const auto count = std::min(sizeof(scratch), bytes - offset);
            gl.getBufferSubData(arrayBuffer, static_cast<std::intptr_t>(offset),
                static_cast<std::intptr_t>(count), scratch);
            if (!check("verify authenticated track normal bytes")) return false;
            if (std::memcmp(scratch, source + offset, count))
                return fail(*diagnostic, VehicleWindowError50DS::assets, "GPU track source normal mismatch");
            offset += count;
        }
        drawGl.bindVertexArray(trackVertexArray);
        gl.enableVertexAttribArray(3);
        gl.vertexAttribPointer(3, 3, floatType, 0, sizeof(assets::WorldNormal50HK), nullptr);
        if (!check("bind authenticated track normal attribute")) return false;
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        const I expected[] = {1, 3, sizeof(assets::WorldNormal50HK), static_cast<I>(floatType), 0,
            static_cast<I>(trackNormalBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(3, properties[p], &value);
            if (!check("verify authenticated track normal attribute")) return false;
            if (value != expected[p])
                return fail(*diagnostic, VehicleWindowError50DS::layout, "source normal attribute layout mismatch");
        }
        void* pointer = nullptr; gl.getVertexAttribPointerv(3, 0x8645, &pointer);
        if (!check("verify authenticated normal offset")) return false;
        return pointer == nullptr || fail(*diagnostic, VehicleWindowError50DS::layout, "source normal attribute offset mismatch");
    }
    // Preserve original PSG MSCNT group boundaries rather than pairing whole
    // surfaces or individual base/map-2 triangles. Each strip's triangle order
    // is checked against BOTH the semantic indices and immutable flattened EBO.
    bool createSpecularGroups50LA(U archetype, U body, SpecularGeometry50LA& out) noexcept {
        const auto selected = racerView(archetype);
        if (!selected.materials) return true;
        if (body >= sourceBodyCount50LA)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "source pair requires an engine body slot");
        const U c = body + 1;
        // Three-model pods have no lower scenes, meshes or material offsets.
        // Leave their two absent body slots empty before indexing any of them.
        if (c >= selected.resources->componentCount) return true;
        const auto& scene = *selected.scenes[c];
        const auto& mesh = *selected.meshes[c];
        if(scene.materials.size()>maxVehicleWindowMaterials50DS)return false;
        std::array<std::uint8_t,maxVehicleWindowMaterials50DS> singleModes{};
        for (U m = 0; m < scene.materials.size(); ++m) {
            const auto& policy=selected.materials[selected.materialOffsets[c]+m].policy;
            out.enabled = out.enabled || policy.sourceSpecularPair50LA;
            singleModes[m]=policy.sourceSingleDiffuseRgba50LA?1u:0u;
            out.singleDiffuse=out.singleDiffuse||policy.sourceSingleDiffuseRgba50LA;
        }
        if(out.singleDiffuse){
            if(!game::original_racer_diffuse50LA::prepareCold(mesh,singleModes.data(),scene.materials.size(),out.diffuseGeometry))
                return fail(*diagnostic,VehicleWindowError50DS::assets,"single diffuse original group/quantized palette inputs rejected");
            hasSingleDiffuse50LA=true;
        }
        if (!out.enabled && !out.singleDiffuse) return true;
        if (!sourceBodyComponent50LA(c, selected.resources->componentCount))
            return fail(*diagnostic, VehicleWindowError50DS::assets,
                "source pair requires an actual engine in a three/five-model pod");
        if (mesh.semanticScene != &scene || scene.lods.empty() ||
            scene.lods[0].surfaces.size() != mesh.spanCount || mesh.spanCount > game::maxVehicleDrawSpans50CM)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "source pair requires exact semantic LOD0 spans");
        const U capacity = mesh.vertexCount / 3; // Every original group owns at least three vertices.
        out.groups.reset(new (std::nothrow) SpecularGroup50LA[capacity]);
        if (!out.groups) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded original body group table");
        for (U spanIndex = 0; spanIndex < mesh.spanCount; ++spanIndex) {
            const auto& span = mesh.spans[spanIndex];
            const auto& spanPolicy=selected.materials[selected.materialOffsets[c]+span.material_index].policy;
            if (!spanPolicy.sourceSpecularPair50LA && !spanPolicy.sourceSingleDiffuseRgba50LA) continue;
            const auto& surface = scene.lods[0].surfaces[spanIndex];
            if (surface.material_index != span.material_index || surface.vertices.size() != span.vertex_count ||
                surface.indices.size() != span.index_count || surface.strips.empty() || !surface.groups ||
                surface.groups > capacity || span.index_start > mesh.indexCount ||
                span.index_count > mesh.indexCount - span.index_start || span.vertex_start > mesh.vertexCount ||
                span.vertex_count > mesh.vertexCount - span.vertex_start)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "source body surface/group extent differs");
            out.spanFirst[spanIndex] = out.groupCount;
            U index = 0, vertexBase = 0, stripIndex = 0;
            for (U group = 0; group < surface.groups; ++group) {
                if (out.groupCount >= capacity || stripIndex >= surface.strips.size())
                    return fail(*diagnostic, VehicleWindowError50DS::assets, "source body group count escapes topology");
                const U firstIndex = index;
                U nextVertex = 0;
                while (stripIndex < surface.strips.size() && surface.strips[stripIndex].group == group) {
                    const auto& strip = surface.strips[stripIndex++];
                    if (strip.start != nextVertex || strip.end < strip.start + 3 || strip.end > 16 ||
                        strip.vertex_start != vertexBase + strip.start || strip.vertex_start > surface.vertices.size() ||
                        strip.end - strip.start > surface.vertices.size() - strip.vertex_start)
                        return fail(*diagnostic, VehicleWindowError50DS::assets, "source body strip order/3..16 vertex group differs");
                    for (U v = strip.start + 2; v < strip.end; ++v) {
                        U a = vertexBase + v - 2, b = vertexBase + v - 1;
                        if ((v - strip.start) & 1u) std::swap(a, b);
                        const U triangle[] = {a, b, vertexBase + v};
                        for (U expected : triangle) {
                            if (index >= surface.indices.size() || surface.indices[index] != expected ||
                                mesh.indices[span.index_start + index] != span.vertex_start + expected)
                                return fail(*diagnostic, VehicleWindowError50DS::indices, "source body group triangle order differs from retained EBO");
                            ++index;
                        }
                    }
                    nextVertex = strip.end;
                }
                if (nextVertex < 3 || index == firstIndex)
                    return fail(*diagnostic, VehicleWindowError50DS::assets, "empty or missing original body group");
                out.groups[out.groupCount++] = {span.index_start + firstIndex, index - firstIndex};
                ++out.spanCount[spanIndex];
                vertexBase += nextVertex;
            }
            if (stripIndex != surface.strips.size() || vertexBase != surface.vertices.size() || index != span.index_count)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "source body groups do not partition the retained surface");
        }
        return true;
    }
    bool createDiffuseFogBuffers50LA(U archetype,U body,SpecularGeometry50LA& target) noexcept {
        if(!target.singleDiffuse)return true;
        const auto selected=racerView(archetype);const U c=body+1;
        const auto bytes=std::size_t(selected.meshes[c]->vertexCount)*sizeof(float);
        constexpr U attribute=vehicleGlSourceFogAttribute50LA;
        for(U bank=0;bank<2;++bank){
            gl.genBuffers(1,&target.fogBuffers[bank]);drawGl.bindBuffer(arrayBuffer,target.fogBuffers[bank]);
            gl.bufferData(arrayBuffer,static_cast<std::intptr_t>(bytes),nullptr,dynamicDraw);
            if(!check("allocate single diffuse fog stream")||!target.fogBuffers[bank]||
               !verifyBuffer(arrayBuffer,bytes,dynamicDraw))return false;
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            gl.enableVertexAttribArray(attribute);gl.vertexAttribPointer(attribute,1,floatType,0,sizeof(float),nullptr);
            constexpr U properties[]={0x8622,0x8623,0x8624,0x8625,0x886A,0x889F,0x88FD};
            const I expected[]={1,1,sizeof(float),static_cast<I>(floatType),0,static_cast<I>(target.fogBuffers[bank]),0};
            for(U p=0;p<7;++p){I actual=0;gl.getVertexAttribiv(attribute,properties[p],&actual);
                if(!check("verify single diffuse fog attribute")||actual!=expected[p])return false;}
            void* offset=nullptr;gl.getVertexAttribPointerv(attribute,0x8645,&offset);
            if(!check("verify single diffuse fog offset")||offset)return false;
        }return true;
    }
    bool createSpecularUvBuffers50LA(U archetype, U body, SpecularGeometry50LA& target) noexcept {
        if(!createDiffuseFogBuffers50LA(archetype,body,target))return false;
        if (!target.enabled) return true;
        if (body >= sourceBodyCount50LA)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "map-2 UVs require an engine body slot");
        const auto selected = racerView(archetype);
        const U c = body + 1;
        if (!sourceBodyComponent50LA(c, selected.resources->componentCount))
            return fail(*diagnostic, VehicleWindowError50DS::assets,
                "map-2 UVs require an actual engine in a three/five-model pod");
        constexpr U attribute = vehicleGlSpecularUvAttribute50LA;
        const std::size_t bytes = std::size_t(selected.meshes[c]->vertexCount) * sizeof(std::array<float, 2>);
        for (U bank = 0; bank < 2; ++bank) {
            gl.genBuffers(1, &target.buffers[bank]);
            drawGl.bindBuffer(arrayBuffer, target.buffers[bank]);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), nullptr, dynamicDraw);
            if (!check("allocate private bounded map-2 UV stream") || !target.buffers[bank])
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "map-2 UV buffer allocation failed");
            if (!verifyBuffer(arrayBuffer, bytes, dynamicDraw)) return false;
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            gl.enableVertexAttribArray(attribute);
            gl.vertexAttribPointer(attribute, 2, floatType, 0, sizeof(std::array<float, 2>), nullptr);
            if (!check("bind source map-2 UV attribute")) return false;
            constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
            const I expected[] = {1, 2, static_cast<I>(sizeof(std::array<float, 2>)),
                static_cast<I>(floatType), 0, static_cast<I>(target.buffers[bank]), 0};
            for (U p = 0; p < 7; ++p) {
                I actual = 0; gl.getVertexAttribiv(attribute, properties[p], &actual);
                if (!check("verify source map-2 UV layout")) return false;
                if (actual != expected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout, "map-2 UV layout differs");
            }
            void* offset = nullptr; gl.getVertexAttribPointerv(attribute, 0x8645, &offset);
            if (!check("verify source map-2 UV offset")) return false;
            if (offset) return fail(*diagnostic, VehicleWindowError50DS::layout, "map-2 UV offset differs");
        }
        return true;
    }
    bool prepareSpecularFrame50LA(const game::VehicleDrawBatch50CO& player,
        const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d,
        RacerNormalReceipt (&normalReceipts)[1 + maxVehicleWindowOpponents50DS]) noexcept {
        if (!hasSpecularPairs50LA && !hasSingleDiffuse50LA) {
            for (U i = 0; i < frame.opponentCount; ++i)
                for (auto* model : frame.opponents[i].sourceBodyModelPackets50LA)
                    if (model) return fail(d, VehicleWindowError50DS::frame,
                        "source opponent model view owner supplied without paired body capability");
            for (auto* model : frame.sourceBodyModelPackets50LA)
                if (model) return fail(d, VehicleWindowError50DS::frame,
                    "source player model view owner supplied without paired body capability");
            if(request.track.rcCrystal)return frame.sourceBodyCamera50LA&&frame.sourceBodyCameraOwner50LA&&
                frame.sourceBodyCameraPublication50LA&&!frame.sourceBodyFog50LA&&!frame.sourceBodyGlobals50LA ||
                fail(d,VehicleWindowError50DS::frame,"RC crystal requires its actual source camera publication");
            return (!frame.sourceBodyCamera50LA && !frame.sourceBodyCameraOwner50LA &&
                !frame.sourceBodyCameraPublication50LA && !frame.sourceBodyFog50LA &&
                !frame.sourceBodyGlobals50LA) ||
                fail(d, VehicleWindowError50DS::frame, "source body camera supplied without paired body capability");
        }
        if (!frame.sourceBodyCamera50LA || !frame.sourceBodyCameraOwner50LA ||
            !frame.sourceBodyCameraPublication50LA || !frame.sourceBodyFog50LA ||
            !frame.sourceBodyGlobals50LA || (hasSpecularPairs50LA && (!specularWorkspace50LA || !frameSpecularUvs50LA)))
            return fail(d, VehicleWindowError50DS::frame, "source body pair requires actual current camera publication");
        // A current source view is independent of the retained/stopped lighting
        // publication. Its exact combined matrix must produce this same raster.
        NativeCameraViewContract50DX contract{};
        contract.provenance = NativeCameraViewProvenance50DX::currentCore49FForOutput;
        contract.width = frame.width; contract.height = frame.height;
        contract.clipDepth = NativeCameraClipDepth50DX::negativeOneToOne;
        NativeCameraMatrix50DX converted{};
        NativeCameraViewError50DX error{};
        const game::CameraViewOutput49F camera = *frame.sourceBodyCamera50LA;
        if (!convertNativeCameraView50DX(camera, contract, converted, error) ||
            std::memcmp(converted.worldViewProjection, frame.worldViewProjection, sizeof(converted.worldViewProjection)))
            return fail(d, VehicleWindowError50DS::frame, "source body camera differs from actual frame world-to-clip matrix");
        if(hasSingleDiffuse50LA){
            if(!frame.sourceDiffuseCamera50LA||!diffuseWorkspace50LA||!frameDiffuseFog50LA||
               !frame.sourceDiffuseCamera50LA->read(frameDiffuseCamera50LA)||
               frameDiffuseCamera50LA.owner!=frame.sourceBodyCameraOwner50LA||
               frameDiffuseCamera50LA.publication!=frame.sourceBodyCameraPublication50LA||
               std::memcmp(frameDiffuseCamera50LA.camera.combined.data(),camera.combined.data(),sizeof(float)*16)||
               frameDiffuseCamera50LA.fog.color30!=frame.sourceBodyFog50LA->color30||
               frameDiffuseCamera50LA.fog.start34!=frame.sourceBodyFog50LA->start34||
               frameDiffuseCamera50LA.fog.end38!=frame.sourceBodyFog50LA->end38)
                return fail(d,VehicleWindowError50DS::frame,"single diffuse current private camera/guard/fog publication rejected");
            frameDiffuseEndpoint50LA=frame.sourceDiffuseCamera50LA;
        }
        game::OriginalRacerSpecularRequest50LA actual{};
        actual.view = camera.view;
        actual.cameraPosition = {camera.position.x, camera.position.y, camera.position.z};
        for (U slot = 0; slot <= frame.opponentCount; ++slot) {
            const U archetype = slot ? frame.opponents[slot - 1].racerArchetype : 0;
            const auto selected = racerView(archetype);
            const auto& batch = slot ? *frame.opponents[slot - 1].batch : player;
            if (!retainedMesh(batch, d, archetype, &normalReceipts[slot])) return false;
            const auto& models = slot ? frame.opponents[slot - 1].sourceBodyModelPackets50LA :
                frame.sourceBodyModelPackets50LA;
            // These four fixed slots preserve each component's own model,
            // semantic normals/root/bounds and stopped-geometry preparation.
            for (U bodySlot = 0; bodySlot < sourceBodyCount50LA; ++bodySlot) {
                frameSpecularSecond50LA[slot][bodySlot] = false;
                auto* model = models[bodySlot];
                frameDiffuseSelected50LA[slot][bodySlot]=true;
                if (!selected.specular[bodySlot].enabled && !selected.specular[bodySlot].singleDiffuse) {
                    if (model) return fail(d, VehicleWindowError50DS::frame,
                        "source model view owner supplied without paired body");
                    continue;
                }
                const U c = bodySlot + 1;
                if (!sourceBodyComponent50LA(c, batch.componentCount))
                    return fail(d, VehicleWindowError50DS::frame,
                        "source body slot has no actual engine in this three/five-model batch");
                game::OriginalRacerModelDistanceFlags50LA modelView{};
                const auto& body = batch.component[c];
                if (!model || !body.sourceSpecularGeometry50LA ||
                    !model->prepareDistanceFlags241280(body.sourceAggregateBounds50LA,
                        body.sourcePosition50LA, actual.cameraPosition, frame.sourceBodyFog50LA->start34,
                        *frame.sourceBodyGlobals50LA, modelView).okay())
                    return fail(d, VehicleWindowError50DS::frame,
                        "actual model radius distance and view flags rejected");
                const U uvSlot = slot * sourceBodyCount50LA + bodySlot;
                if(selected.specular[bodySlot].singleDiffuse){
                    game::OriginalRacerDiffuseClip50LA clip;
                    if(body.sourceGeometryModel50LA!=model||
                       !model->prepareDiffuseSelection240C00(*selected.meshes[c]->semanticScene,
                        body.sourceGeometryRevision50LA,body.sourceRootWorld50LA,body.sourceAggregateBounds50LA,
                        frameDiffuseCamera50LA.camera.combined,frameDiffuseCamera50LA.guardCombined,clip))
                        return fail(d,VehicleWindowError50DS::frame,"single diffuse actual null-observer selection rejected");
                    frameDiffuseSelected50LA[slot][bodySlot]=clip.selected;
                    auto* fog=frameDiffuseFog50LA.get()+uvSlot*game::maxRigidVertices49S;
                    if(clip.selected){
                        if(!game::original_racer_diffuse50LA::prepareFrame(selected.specular[bodySlot].diffuseGeometry,
                            *selected.meshes[c],body,*model,modelView,clip,frameDiffuseCamera50LA,*diffuseWorkspace50LA,
                            fog,game::maxRigidVertices49S))
                            return fail(d,VehicleWindowError50DS::frame,"single diffuse original palette fog operands rejected");
                    }else std::fill_n(fog,body.vertexCount,255.f);
                }
                if(!selected.specular[bodySlot].enabled)continue;
                game::OriginalRacerSpecularResult50LA result{};
                if (!game::prepareOriginalRacerSpecular50LA(*selected.meshes[c], body, actual, modelView,
                    frameSpecularUvs50LA.get() + uvSlot * game::maxRigidVertices49S, game::maxRigidVertices49S,
                    *specularWorkspace50LA, result))
                    return fail(d, VehicleWindowError50DS::frame, "actual current camera and retained body specular projection rejected");
                frameSpecularSecond50LA[slot][bodySlot] = result.secondEnabled;
            }
        }
        frameSpecularCamera50LA = actual;
        frameSpecularCameraOwner50LA = frame.sourceBodyCameraOwner50LA;
        frameSpecularCameraPublication50LA = frame.sourceBodyCameraPublication50LA;
        return true;
    }
    bool uploadSpecularUvs50LA(std::uint8_t bank, U slot, U archetype) noexcept {
        const auto selected = racerView(archetype);
        for (U bodySlot = 0; bodySlot < sourceBodyCount50LA; ++bodySlot) {
            const auto& body = selected.specular[bodySlot];
            if (!body.enabled && !body.singleDiffuse) continue;
            const U c = bodySlot + 1, uvSlot = slot * sourceBodyCount50LA + bodySlot;
            if(body.singleDiffuse){
                game::OriginalRacerDiffuseCameraSnapshot50LA current;
                if(!frameDiffuseEndpoint50LA||!frameDiffuseEndpoint50LA->read(current)||
                   current.owner!=frameDiffuseCamera50LA.owner||current.revision!=frameDiffuseCamera50LA.revision||
                   current.generation!=frameDiffuseCamera50LA.generation||current.publication!=frameDiffuseCamera50LA.publication||
                   current.frame!=frameDiffuseCamera50LA.frame)
                    return fail(*diagnostic,VehicleWindowError50DS::frame,"single diffuse camera publication changed before upload");
                drawGl.bindBuffer(arrayBuffer,body.fogBuffers[bank]);
                drawGl.bufferSubData(arrayBuffer,0,std::intptr_t(selected.meshes[c]->vertexCount)*sizeof(float),
                    frameDiffuseFog50LA.get()+uvSlot*game::maxRigidVertices49S);
                if(!check("upload actual current single diffuse fog bytes"))return false;
            }
            if(!body.enabled)continue;
            if (!sourceBodyComponent50LA(c, selected.resources->componentCount))
                return fail(*diagnostic, VehicleWindowError50DS::assets,
                    "source map-2 upload has no actual engine in this three/five-model pod");
            drawGl.bindBuffer(arrayBuffer, body.buffers[bank]);
            // CPU preparation still admits every current-camera UV. The same
            // original distance flag prevents all map-2 draws for this body;
            // other passes' results do not depend on this attribute. Every
            // map-2-enabled body uploads its complete stream, even after a skip,
            // bank switch, shared-archetype draw or failed prior submission.
            if (frameSpecularSecond50LA[slot][bodySlot])
                drawGl.bufferSubData(arrayBuffer, 0,
                    std::intptr_t(selected.meshes[c]->vertexCount) * sizeof(std::array<float, 2>),
                    frameSpecularUvs50LA.get() + uvSlot * game::maxRigidVertices49S);
            if (!check("upload source map-2 UVs for this current camera and actual racer body")) return false;
        }
        return true;
    }
    bool createRacerNormals(const VehicleGlResources50CV& geometry,
        U (&buffers)[2][maxComponents]) noexcept {
        // Keep the compatibility mesh and all UV/color bytes unchanged. Every
        // private VAO gets a bounded independent normal stream, initially zero.
        static constexpr assets::WorldNormal50HK absent[game::maxRigidVertices49S]{};
        for (U bank = 0; bank < 2; ++bank) for (U c = 0; c < geometry.componentCount; ++c) {
            VehicleWindowTrack50DS source{};
            source.sourceNormals = absent;
            source.sourceNormalCount = geometry.bank[bank].vertexCapacity[c];
            if (!createWorldNormals(source, buffers[bank][c],
                geometry.bank[bank].vertexArray[c], dynamicDraw)) return false;
        }
        return true;
    }
    bool uploadRacerNormals(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0, bool effectsOnly = false) noexcept {
        const auto selected = racerView(archetype);
        static constexpr assets::WorldNormal50HK absent[game::maxRigidVertices49S]{};
        for (U c = 0; c < batch.componentCount; ++c) {
            const auto& part = batch.component[c];
            drawGl.bindBuffer(arrayBuffer, selected.normalBuffers[bank][c]);
            // A complete cold bank with no normal-dependent lighting keeps its
            // verified zero-filled attribute. CPU normals still feed source
            // colors/specular and validation; absent tables stay conservative.
            // Deferred draws select nonopaque materials, making the normal
            // varying output-independent. Preserve the conservative null-table
            // route, every bind/check, and all ordinary opaque uploads.
            if (!selected.materials || (!effectsOnly && selected.materialSummary[c].normalDependent))
                drawGl.bufferSubData(arrayBuffer, 0,
                    std::intptr_t(part.vertexCount) * sizeof(assets::WorldNormal50HK),
                    part.sourceNormals ? part.sourceNormals : absent);
            if (!check("upload current per-node racer source normals")) return false;
        }
        return true;
    }
    bool createTrackProjection(const VehicleWindowTrack50DS& track) noexcept {
        if(track.rcCrystal)return createRcCrystalProjection(track);
        if (!track.sourceProjectionUvs) return true;
        const std::size_t bytes = std::size_t(track.sourceProjectionCount) * sizeof(assets::TrackProjectedUv);
        gl.genBuffers(1, &trackProjectionBuffer);
        drawGl.bindBuffer(arrayBuffer, trackProjectionBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), track.sourceProjectionUvs, staticDraw);
        if (!check("upload authenticated track source projection UVs")) return false;
        if (!trackProjectionBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero source projection UV buffer name");
        if (!verifyBuffer(arrayBuffer, bytes, staticDraw)) return false;
        std::uint8_t scratch[1024];
        const auto* source = reinterpret_cast<const std::uint8_t*>(track.sourceProjectionUvs);
        for (std::size_t offset = 0; offset < bytes;) {
            const auto count = std::min(sizeof(scratch), bytes - offset);
            gl.getBufferSubData(arrayBuffer, static_cast<std::intptr_t>(offset),
                static_cast<std::intptr_t>(count), scratch);
            if (!check("verify authenticated track projection UV bytes")) return false;
            if (std::memcmp(scratch, source + offset, count))
                return fail(*diagnostic, VehicleWindowError50DS::assets, "GPU track source projection UV mismatch");
            offset += count;
        }
        drawGl.bindVertexArray(trackVertexArray);
        gl.enableVertexAttribArray(4);
        gl.vertexAttribPointer(4, 4, floatType, 0, sizeof(assets::TrackProjectedUv), nullptr);
        if (!check("bind authenticated track projection UV attribute")) return false;
        constexpr U properties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        const I expected[] = {1, 4, sizeof(assets::TrackProjectedUv), static_cast<I>(floatType), 0,
            static_cast<I>(trackProjectionBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(4, properties[p], &value);
            if (!check("verify authenticated track projection UV attribute")) return false;
            if (value != expected[p])
                return fail(*diagnostic, VehicleWindowError50DS::layout, "source projection UV attribute layout mismatch");
        }
        void* pointer = nullptr; gl.getVertexAttribPointerv(4, 0x8645, &pointer);
        if (!check("verify authenticated projection UV offset")) return false;
        return pointer == nullptr || fail(*diagnostic, VehicleWindowError50DS::layout, "source projection UV attribute offset mismatch");
    }
    bool createDynamicWorldGeometry(const VehicleWindowTrack50DS& world) noexcept {
        if (!world.mesh) return true;
        dynamicWorldVertexCount = static_cast<U>(world.mesh->vertices.size());
        dynamicWorldNormalCount = world.sourceNormalCount;
        dynamicWorldSourceVertices.reset(new (std::nothrow) assets::Vertex48B[dynamicWorldVertexCount]);
        if (!dynamicWorldSourceVertices)
            return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded dynamic-world source UV/color provenance");
        std::memcpy(dynamicWorldSourceVertices.get(), world.mesh->vertices.data(),
            std::size_t(dynamicWorldVertexCount) * sizeof(assets::Vertex48B));
        for (U b = 0; b < 2; ++b)
            if (!createStaticGeometry(world.mesh, dynamicWorldVertexBuffers[b], dynamicWorldIndexBuffer,
                    dynamicWorldArrays[b], dynamicWorldIndexCount, dynamicDraw, b != 0) ||
                !createWorldNormals(world, dynamicWorldNormalBuffers[b], dynamicWorldArrays[b], dynamicDraw))
                return false;
        return true;
    }
    bool validateDynamicWorldFrame(const VehicleWindowDynamicWorldFrame50DS& frame,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (frame.vertexCount != dynamicWorldVertexCount ||
            bool(frame.vertices) != bool(dynamicWorldVertexCount) ||
            frame.sourceNormalCount != dynamicWorldNormalCount ||
            bool(frame.sourceNormals) != bool(dynamicWorldNormalCount))
            return fail(d, VehicleWindowError50DS::frame,
                "dynamic-world streams must exactly match cold vertex/normal counts");
        for (U v = 0; v < frame.vertexCount; ++v) {
            const auto& vertex = frame.vertices[v];
            for (float value : vertex.position)
                if (!std::isfinite(value))
                    return fail(d, VehicleWindowError50DS::frame, "nonfinite dynamic-world position");
            const auto& source = dynamicWorldSourceVertices[v];
            if (std::memcmp(vertex.uv, source.uv, sizeof(source.uv)) ||
                std::memcmp(vertex.color, source.color, sizeof(source.color)))
                return fail(d, VehicleWindowError50DS::frame,
                    "dynamic-world UV/color must retain exact cold source bytes");
        }
        for (U n = 0; n < frame.sourceNormalCount; ++n) {
            const auto& normal = frame.sourceNormals[n].xyz;
            const double length = std::hypot(normal[0], normal[1], normal[2]);
            if (!std::isfinite(length) || (length != 0.0 && std::abs(length - 1.0) > 0.0001))
                return fail(d, VehicleWindowError50DS::frame,
                    "dynamic-world normals must be finite unit vectors or explicit zero");
        }
        return true;
    }
    bool uploadDynamicWorld(U bank) noexcept {
        if (!dynamicWorldVertexCount) return true;
        drawGl.bindBuffer(arrayBuffer, dynamicWorldVertexBuffers[bank]);
        drawGl.bufferSubData(arrayBuffer, 0,
            std::intptr_t(dynamicWorldVertexCount) * sizeof(assets::Vertex48B), frameDynamicWorld.vertices);
        if (!check("upload current source-animated dynamic-world vertices")) return false;
        diagnostic->dynamicWorldVerticesUploaded = dynamicWorldVertexCount;
        if (dynamicWorldNormalCount) {
            drawGl.bindBuffer(arrayBuffer, dynamicWorldNormalBuffers[bank]);
            drawGl.bufferSubData(arrayBuffer, 0,
                std::intptr_t(dynamicWorldNormalCount) * sizeof(assets::WorldNormal50HK), frameDynamicWorld.sourceNormals);
            if (!check("upload current source-animated dynamic-world normals")) return false;
            diagnostic->dynamicWorldNormalsUploaded = dynamicWorldNormalCount;
        }
        return true;
    }
    bool createCableGeometry() noexcept {
        bool needed = request.cables.textures != nullptr;
        for (U i = 0; i < racerArchetypeCount; ++i)
            needed = needed || racerArchetypes[i].cables.textures;
        if (!needed) return true;
        constexpr std::size_t vertexBytes = sizeof(game::PodCableMesh50FD::vertices);
        constexpr std::size_t indexBytes = sizeof(game::PodCableMesh50FD::indices);
        U indices[game::maxPodCableIndexCount50FD];
        for (U i = 0; i < game::maxPodCableIndexCount50FD; ++i) indices[i] = cableIndex50DS(i);
        gl.genBuffers(1, &cableVertexBuffer);
        drawGl.bindBuffer(arrayBuffer, cableVertexBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(vertexBytes), nullptr, dynamicDraw);
        if (!check("allocate fixed dynamic cable vertices")) return false;
        if (!cableVertexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero cable vertex buffer");
        if (!verifyBuffer(arrayBuffer, vertexBytes, dynamicDraw)) return false;
        gl.genBuffers(1, &cableIndexBuffer);
        drawGl.bindBuffer(arrayBuffer, cableIndexBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(indexBytes), indices, staticDraw);
        if (!check("upload canonical source cable indices")) return false;
        if (!cableIndexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero cable index buffer");
        if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
        U uploaded[game::maxPodCableIndexCount50FD];
        gl.getBufferSubData(arrayBuffer, 0, static_cast<std::intptr_t>(indexBytes), uploaded);
        if (!check("verify canonical cable index bytes")) return false;
        if (std::memcmp(indices, uploaded, indexBytes))
            return fail(*diagnostic, VehicleWindowError50DS::indices, "GPU cable index contents mismatch");
        gl.genVertexArrays(1, &cableVertexArray);
        drawGl.bindVertexArray(cableVertexArray);
        drawGl.bindBuffer(arrayBuffer, cableVertexBuffer);
        drawGl.bindBuffer(elementBuffer, cableIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create fixed cable Vertex48B layout")) return false;
        if (!cableVertexArray)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero cable vertex array");
        return verifyLayout(cableVertexBuffer, cableIndexBuffer);
    }
    bool compileHostHudShader(U type, const char* source, U& name) noexcept {
        name = shaderGl.createShader(type);
        if (!check("create host HUD shader")) return false;
        if (!name) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero host HUD shader name");
        shaderGl.shaderSource(name, 1, &source, nullptr);
        shaderGl.compileShader(name);
        I compiled = 0;
        shaderGl.getShaderiv(name, 0x8B81, &compiled);
        if (!check("compile host HUD shader")) return false;
        if (compiled) return true;
        shaderGl.getShaderInfoLog(name, static_cast<I>(sizeof(diagnostic->shader.log)),
            nullptr, diagnostic->shader.log);
        diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
        check("read host HUD shader log");
        return fail(*diagnostic, VehicleWindowError50DS::shader, "host HUD shader compilation failed");
    }
    bool validateOriginalUiBank(const VehicleWindowRequest50DS& input,std::uint8_t* stagedAlpha=nullptr) noexcept {
        if (!input.enableOriginalUi)
            return (!input.originalUiCpuTextures && !input.originalUiTextures) ||
                fail(*diagnostic, VehicleWindowError50DS::request, "UI banks require explicit original UI opt-in");
        const auto* cpu = input.originalUiCpuTextures;
        const auto* gpu = input.originalUiTextures;
        // No bank is needed by an explicitly untextured-only UI. A missing
        // textured resource never selects this path: each span is typed.
        if (!cpu && !gpu) return true;
        if (!cpu || !cpu->loaded() || !gpu || gpu->riderSparse() || !gpu->validBorrow(input.context, input.isCurrent,
            gpu->publicationToken()) || !cpu->textureCount() ||
            cpu->textureCount() > assets::maxMaterialTextureAssets50EA || cpu->textureCount() != gpu->textureCount())
            return fail(*diagnostic, VehicleWindowError50DS::material, "UI requires matching immutable CPU/GPU publication");
        // Cold publication scan. Stage maxima so a rejected destination cannot
        // corrupt the currently retained bank's blend-admission bounds.
        std::uint8_t maximumAlphaByTexture[assets::maxMaterialTextureAssets50EA]{};
        for (U i = 0; i < cpu->textureCount(); ++i) {
            const auto* source = cpu->texture(i); const auto* image = gpu->asset(i);
            if (!source || !image || !image->name || image->identity != source->identity)
                return fail(*diagnostic, VehicleWindowError50DS::material, "UI CPU/GPU texture identity mismatch");
            std::uint8_t maximumAlpha = 0;
            for (std::size_t a = 3; a < source->texture.rgba.size(); a += 4)
                maximumAlpha = std::max(maximumAlpha, source->texture.rgba[a]);
            maximumAlphaByTexture[i] = maximumAlpha;
        }
        std::memcpy(stagedAlpha?stagedAlpha:uiTextureMaximumAlpha, maximumAlphaByTexture, sizeof(uiTextureMaximumAlpha));
        return true;
    }
    bool createOriginalUi() noexcept {
        if (!request.enableOriginalUi && !request.gcParticleTextures && !request.track.rcParticleWindow && !request.track.gbParticleWindow && !request.track.mcParticleWindow && !raceHudPreparing && !timeStatsPreparing && !positionStatsPreparing && !damagePreparing && !pauseDerivedPreparing && !speedBoostPreparing) return true;
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec2 rrUv;
in vec4 rrColor;
in float rrParticleFog;
in float rrSourceRasterW;
in vec3 rrSourceStq;
uniform vec4 canvasToClip;
uniform int uiSourceRaster;
uniform int gcParticleMode;
out vec2 uiUv;
noperspective out vec3 uiSourceStq;
noperspective out vec4 uiColor;
noperspective out float uiParticleFog;
void main() {
    float w = uiSourceRaster != 0 ? rrSourceRasterW : 1.0;
    // Source screen Z is reversed. Gameplay uses core49F depth[0,1],
    // shared with the already drawn opaque world, not the UI reversed target.
    float z=gcParticleMode!=0 ? (8388591.0-rrPosition.z)/8388590.0 : rrPosition.z/16777215.0;
    gl_Position = vec4(rrPosition.xy * canvasToClip.xy + canvasToClip.zw,
        z * 2.0 - 1.0, 1.0) * w;
    uiUv = rrUv; uiColor = rrColor; uiParticleFog = rrParticleFog;
    // Modes0/1 retain the existing semantic-UV path. Mode2 transports the
    // actual packet lanes screen-affinely, independently of projected W.
    uiSourceStq = uiSourceRaster == 2 ? rrSourceStq : vec3(0.0,0.0,1.0);
}
)GLSL";
        constexpr const char* fragmentPrefix = "#version 150 core\n";
        constexpr const char* coherentFragmentPrefix =
            "#version 150 core\n"
            "#extension GL_EXT_shader_framebuffer_fetch : require\n"
            "#define RR_UI_COHERENT_FRAMEBUFFER_FETCH 1\n";
        constexpr const char* fragmentSource = R"GLSL(
in vec2 uiUv;
noperspective in vec3 uiSourceStq;
uniform int uiSourceRaster;
noperspective in vec4 uiColor;
noperspective in float uiParticleFog;
uniform sampler2D uiImage;
uniform float uiAlphaScale;
uniform int uiTextured;
uniform int uiSourceFogEnabled;
uniform int uiSourceAlphaTest;
uniform vec4 uiParticleFogColor;
#ifndef RR_UI_COHERENT_FRAMEBUFFER_FETCH
uniform sampler2D uiSourceTarget;
#endif
uniform int uiSourceComposition;
uniform int uiCoveragePayload;
uniform int uiSourceBlend;
uniform int gcParticleMode;
#ifdef RR_UI_COHERENT_FRAMEBUFFER_FETCH
inout vec4 viewerOutput;
#else
out vec4 viewerOutput;
#endif
void main() {
    if (uiSourceComposition != 0) {
        // Explicit native byte representation. Native GL sampling/interpolation
        // remains separately documented; no PS2 rasterizer/device is executed.
        vec4 vertexByte=clamp(uiColor*255.0,0.0,255.0);
        vec2 sampleUv=uiUv;
        // Q was separately rounded by the original packet/clipper. Do not
        // replace this division with semantic UV perspective interpolation.
        if(uiSourceRaster==2)sampleUv=uiSourceStq.xy/uiSourceStq.z;
        vec4 source=uiTextured!=0 ?
            floor(clamp(texture(uiImage,sampleUv)*255.0*vertexByte/128.0,0.0,255.0)) :
            floor(vertexByte);
        if (uiSourceAlphaTest!=0 && source.a<1.0) discard;
        if (uiSourceFogEnabled!=0) {
            float f=clamp(uiParticleFog*256.0,0.0,255.0);
            source.rgb=floor((source.rgb*f+uiParticleFogColor.rgb*255.0*(255.0-f))/256.0);
        }
        // Float scratch stores exact byte payload, including A=0. Negative A
        // is reserved for uncovered samples by the private raster clear.
        if(uiCoveragePayload!=0){viewerOutput=source;return;}
        vec3 result=source.rgb;
        if (uiSourceBlend!=0) {
            // Opaque spans never enter this destination-fetch branch.
#ifdef RR_UI_COHERENT_FRAMEBUFFER_FETCH
            // Coherent EXT fetch observes prior writes in API primitive order,
            // including overlapping triangles within this indexed span draw.
            vec4 destination=floor(viewerOutput*255.0+0.5);
#else
            // ARB_texture_barrier: feedback uses one actual triangle per draw,
            // one same-pixel read/write per texel, with a barrier before each.
            vec4 destination=floor(texelFetch(uiSourceTarget,ivec2(gl_FragCoord.xy),0)*255.0+0.5);
#endif
            float coefficient=uiSourceBlend==3 ? destination.a : uiSourceBlend==4 ? 255.0 :
                uiSourceBlend==6 ? 64.0 : source.a;
            // Exact shadow ALPHA A=2 selects zero, B=1 selects destination.
            // Floor the signed product before adding Cd, as for other ALPHA
            // modes; never replace the original shadow texture or source A.
            vec3 difference=uiSourceBlend==5 ? -destination.rgb :
                uiSourceBlend==1 ? source.rgb-destination.rgb : source.rgb;
            result=floor(difference*coefficient/128.0)+destination.rgb;
        }
        // ALPHA changes RGB only. Every surviving fragment replaces stored A;
        // it never accumulates source-over alpha or clamps a FIX255 factor to1.
        viewerOutput=vec4(clamp(result,0.0,255.0),source.a)/255.0;
        return;
    }
    if (uiTextured != 0) {
        vec4 raw = texture(uiImage, uiUv) * uiColor;
        // MODULATE clamps before blend, not after addition to destination RGB.
        viewerOutput = vec4(clamp(raw.rgb * (255.0 / 128.0), 0.0, 1.0),
            clamp(raw.a * uiAlphaScale, 0.0, 1.0));
        // Actual DefaultMT ATE/GREATER/AREF0/KEEP. Source MODULATE alpha
        // is byte-quantized before the test: values below one byte fail.
        if (uiSourceAlphaTest != 0 && raw.a * (255.0 * 255.0 / 128.0) < 1.0) discard;
        if (uiSourceFogEnabled != 0) {
            // Source F is a byte, with both fog weights divided by256.
            // Fog changes RGB only; A/128 remains the blend coefficient.
            viewerOutput.rgb = viewerOutput.rgb * uiParticleFog +
                uiParticleFogColor.rgb * (255.0 / 256.0 - uiParticleFog);
        }
        if(gcParticleMode!=0) {
            //Source MODULATE A is byte-quantized/clamped before both ALPHA
            //and independent destination-A replacement. Never store A/128
            //into a framebuffer whose source byte representation is A/255.
            float sourceAlphaByte=floor(clamp(raw.a*(255.0*255.0/128.0),0.0,255.0));
            if(gcParticleMode==3) {
                viewerOutput=vec4(0.0,0.0,0.0,sourceAlphaByte/255.0);
            } else if(gcParticleMode==2) {
                //ONE/ONE permits genuine glow coefficients above1 without
                //SRC_ALPHA clamp. Positive fixed-target additive saturation
                //is unchanged if premultiplied source exceeds framebuffer1.
                viewerOutput.rgb*=sourceAlphaByte/128.0;
                viewerOutput.a=1.0;
            } else viewerOutput.a=sourceAlphaByte/128.0;
        }
    } else {
        // TME=0 uses raw interpolated vertex color without texture modulation.
        // Nonopaque A/128 uses one 255/128 scale; RGB retains its raw /255.
        viewerOutput = vec4(uiColor.rgb, clamp(uiColor.a * uiAlphaScale, 0.0, 1.0));
    }
}
)GLSL";
        const auto compile = [&](U type, const char* source, U& name, const char* prefix=nullptr) noexcept {
            name = shaderGl.createShader(type);
            if (!check("create original UI shader")) return false;
            if (!name) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero original UI shader");
            const char* sources[] = {prefix, source};
            shaderGl.shaderSource(name, prefix ? 2 : 1, prefix ? sources : &source, nullptr);
            shaderGl.compileShader(name);
            I compiled = 0; shaderGl.getShaderiv(name, 0x8B81, &compiled);
            if (!check("compile original UI shader")) return false;
            if (compiled) return true;
            shaderGl.getShaderInfoLog(name, static_cast<I>(sizeof(diagnostic->shader.log)), nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1] = '\0';
            check("read original UI shader log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "original UI shader compilation failed");
        };
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
        if (!compile(0x8B31, vertexSource, uiVertexShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
        if (!compile(0x8B30, fragmentSource, uiFragmentShader,
            uiSourceCoherentFramebufferFetch ? coherentFragmentPrefix : fragmentPrefix)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
        uiProgram = shaderGl.createProgram();
        if (!check("create original UI program")) return false;
        if (!uiProgram) return fail(*diagnostic, VehicleWindowError50DS::shader, "zero original UI program");
        shaderGl.attachShader(uiProgram, uiVertexShader); shaderGl.attachShader(uiProgram, uiFragmentShader);
        shaderGl.bindAttribLocation(uiProgram, 0, "rrPosition");
        shaderGl.bindAttribLocation(uiProgram, 1, "rrUv");
        shaderGl.bindAttribLocation(uiProgram, 2, "rrColor");
        shaderGl.bindAttribLocation(uiProgram, 3, "rrParticleFog");
        shaderGl.bindAttribLocation(uiProgram, 4, "rrSourceRasterW");
        shaderGl.bindAttribLocation(uiProgram, 5, "rrSourceStq");
        shaderGl.bindFragDataLocation(uiProgram, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.linkProgram(uiProgram);
        I linked = 0; shaderGl.getProgramiv(uiProgram, 0x8B82, &linked);
        if (!check("link original UI program")) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(uiProgram, static_cast<I>(sizeof(diagnostic->shader.log)), nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1] = '\0';
            check("read original UI program log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "original UI program link failed");
        }
        uiCanvasUniform = shaderGl.getUniformLocation(uiProgram, "canvasToClip");
        uiImageUniform = shaderGl.getUniformLocation(uiProgram, "uiImage");
        uiAlphaUniform = shaderGl.getUniformLocation(uiProgram, "uiAlphaScale");
        uiTexturedUniform = shaderGl.getUniformLocation(uiProgram, "uiTextured");
        uiParticleFogColorUniform = shaderGl.getUniformLocation(uiProgram, "uiParticleFogColor");
        uiSourceRasterUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceRaster");
        gcParticleModeUniform = shaderGl.getUniformLocation(uiProgram, "gcParticleMode");
        uiSourceFogEnabledUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceFogEnabled");
        uiSourceAlphaTestUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceAlphaTest");
        uiSourceCompositionUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceComposition");
        if (!uiSourceCoherentFramebufferFetch)
            uiSourceTargetUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceTarget");
        uiSourceBlendUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceBlend");
        uiCoveragePayloadUniform=shaderGl.getUniformLocation(uiProgram,"uiCoveragePayload");
        if (!check("locate original UI uniforms")) return false;
        if (uiCanvasUniform < 0 || uiImageUniform < 0 || uiAlphaUniform < 0 || uiTexturedUniform < 0 ||
            uiParticleFogColorUniform < 0 || uiSourceRasterUniform < 0 || gcParticleModeUniform < 0 ||
            uiSourceFogEnabledUniform < 0 || uiSourceAlphaTestUniform < 0 ||
            uiSourceCompositionUniform < 0 || (!uiSourceCoherentFramebufferFetch && uiSourceTargetUniform < 0) ||
            uiSourceBlendUniform < 0 || uiCoveragePayloadUniform < 0)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "original UI uniform missing");
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
        const U uiCapacity=request.enableOriginalUiSourceRgba?originalUiMaximumSourceQuads50KM:originalUiMaximumQuads50KG;
        const std::intptr_t vertexBytes = uiCapacity * 4 * sizeof(assets::Vertex48B);
        gl.genBuffers(1, &uiVertexBuffer); drawGl.bindBuffer(arrayBuffer, uiVertexBuffer);
        gl.bufferData(arrayBuffer, vertexBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI vertices")) return false;
        if (!uiVertexBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI vertex buffer");
        if (!verifyBuffer(arrayBuffer, vertexBytes, dynamicDraw)) return false;
        const std::intptr_t fogBytes = uiCapacity * 4 * sizeof(float);
        gl.genBuffers(1, &uiParticleFogBuffer); drawGl.bindBuffer(arrayBuffer, uiParticleFogBuffer);
        gl.bufferData(arrayBuffer, fogBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI particle fog")) return false;
        if (!uiParticleFogBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI fog buffer");
        if (!verifyBuffer(arrayBuffer, fogBytes, dynamicDraw)) return false;
        gl.genBuffers(1, &uiSourceRasterWBuffer); drawGl.bindBuffer(arrayBuffer, uiSourceRasterWBuffer);
        gl.bufferData(arrayBuffer, fogBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI source W")) return false;
        if (!uiSourceRasterWBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI source W buffer");
        if (!verifyBuffer(arrayBuffer, fogBytes, dynamicDraw)) return false;
        const std::intptr_t stqBytes = uiCapacity * 4 * sizeof(std::array<float,3>);
        gl.genBuffers(1, &uiSourcePacketStqBuffer); drawGl.bindBuffer(arrayBuffer, uiSourcePacketStqBuffer);
        gl.bufferData(arrayBuffer, stqBytes, nullptr, dynamicDraw);
        if (!check("allocate bounded original UI packet STQ")) return false;
        if (!uiSourcePacketStqBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI packet STQ buffer");
        if (!verifyBuffer(arrayBuffer, stqBytes, dynamicDraw)) return false;
        // Heap allocation is cold-only; no large transient frame stack.
        std::unique_ptr<U[]> indices(new (std::nothrow) U[uiCapacity * 6]);
        if (!indices) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original UI quad indices");
        constexpr U corners[] = {0, 1, 2, 0, 2, 3};
        for (U i = 0; i < uiCapacity * 6; ++i) indices[i] = (i / 6) * 4 + corners[i % 6];
        const std::intptr_t indexBytes = uiCapacity * 6 * sizeof(U);
        gl.genBuffers(1, &uiIndexBuffer); drawGl.bindBuffer(arrayBuffer, uiIndexBuffer);
        gl.bufferData(arrayBuffer, indexBytes, indices.get(), staticDraw);
        if (!check("upload original UI quad indices")) return false;
        if (!uiIndexBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI index buffer");
        if (!verifyBuffer(arrayBuffer, indexBytes, staticDraw)) return false;
        U uploaded[256];
        for (U i = 0; i < uiCapacity * 6; i += 256) {
            const auto bytes = std::min<U>(256, uiCapacity * 6 - i) * sizeof(U);
            gl.getBufferSubData(arrayBuffer, std::intptr_t(i) * sizeof(U), bytes, uploaded);
            if (!check("verify original UI index bytes")) return false;
            if (std::memcmp(indices.get() + i, uploaded, bytes))
                return fail(*diagnostic, VehicleWindowError50DS::indices, "original UI GPU index mismatch");
        }
        if (request.enableOriginalUiSourceRgba) {
            uiSourceIndices.reset(new (std::nothrow) UiSourceIndexStream);
            if (!uiSourceIndices) return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "allocate bounded original UI source indices and span ranges");
            gl.genBuffers(1, &uiSourceIndexBuffer); drawGl.bindBuffer(arrayBuffer, uiSourceIndexBuffer);
            gl.bufferData(arrayBuffer, indexBytes, nullptr, dynamicDraw);
            if (!check("allocate bounded original UI source index buffer")) return false;
            if (!uiSourceIndexBuffer) return fail(*diagnostic, VehicleWindowError50DS::allocation,
                "zero UI source index buffer");
            if (!verifyBuffer(arrayBuffer, indexBytes, dynamicDraw)) return false;
        }
        gl.genVertexArrays(1, &uiVertexArray); drawGl.bindVertexArray(uiVertexArray);
        drawGl.bindBuffer(arrayBuffer, uiVertexBuffer); drawGl.bindBuffer(elementBuffer, uiIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create original UI Vertex48B layout")) return false;
        if (!uiVertexArray) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI vertex array");
        if (!verifyLayout(uiVertexBuffer, uiIndexBuffer)) return false;
        drawGl.bindBuffer(arrayBuffer, uiParticleFogBuffer);
        gl.enableVertexAttribArray(3);
        gl.vertexAttribPointer(3, 1, floatType, 0, sizeof(float), nullptr);
        if (!check("create original UI particle fog layout")) return false;
        constexpr U fogProperties[] = {0x8622, 0x8623, 0x8624, 0x8625, 0x886A, 0x889F, 0x88FD};
        const I fogExpected[] = {1, 1, sizeof(float), static_cast<I>(floatType), 0,
            static_cast<I>(uiParticleFogBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(3, fogProperties[p], &value);
            if (!check("verify original UI particle fog attribute")) return false;
            if (value != fogExpected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                "particle fog attribute mismatch");
        }
        void* fogPointer = nullptr; gl.getVertexAttribPointerv(3, 0x8645, &fogPointer);
        if (!check("verify original UI particle fog offset")) return false;
        if (fogPointer) return fail(*diagnostic, VehicleWindowError50DS::layout, "particle fog offset mismatch");
        drawGl.bindBuffer(arrayBuffer, uiSourceRasterWBuffer);
        gl.enableVertexAttribArray(4);
        gl.vertexAttribPointer(4, 1, floatType, 0, sizeof(float), nullptr);
        if (!check("create original UI source W layout")) return false;
        const I wExpected[] = {1, 1, sizeof(float), static_cast<I>(floatType), 0,
            static_cast<I>(uiSourceRasterWBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(4, fogProperties[p], &value);
            if (!check("verify original UI source W attribute")) return false;
            if (value != wExpected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                "source W attribute mismatch");
        }
        void* wPointer = nullptr; gl.getVertexAttribPointerv(4, 0x8645, &wPointer);
        if (!check("verify original UI source W offset")) return false;
        if (wPointer) return fail(*diagnostic, VehicleWindowError50DS::layout, "source W offset mismatch");
        drawGl.bindBuffer(arrayBuffer, uiSourcePacketStqBuffer);
        gl.enableVertexAttribArray(5);
        gl.vertexAttribPointer(5, 3, floatType, 0, sizeof(std::array<float,3>), nullptr);
        if (!check("create original UI packet STQ layout")) return false;
        const I stqExpected[] = {1, 3, sizeof(std::array<float,3>), static_cast<I>(floatType), 0,
            static_cast<I>(uiSourcePacketStqBuffer), 0};
        for (U p = 0; p < 7; ++p) {
            I value = 0; gl.getVertexAttribiv(5, fogProperties[p], &value);
            if (!check("verify original UI packet STQ attribute")) return false;
            if (value != stqExpected[p]) return fail(*diagnostic, VehicleWindowError50DS::layout,
                "source packet STQ attribute mismatch");
        }
        void* stqPointer = nullptr; gl.getVertexAttribPointerv(5, 0x8645, &stqPointer);
        if (!check("verify original UI packet STQ offset")) return false;
        return !stqPointer || fail(*diagnostic, VehicleWindowError50DS::layout, "source packet STQ offset mismatch");
    }
    bool createHostHud() noexcept {
        if (!request.enableHostHud) return true;
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec4 rrColor;
out vec4 hudColor;
void main() { gl_Position = vec4(rrPosition, 1.0); hudColor = rrColor; }
)GLSL";
        constexpr const char* fragmentSource = R"GLSL(#version 150 core
in vec4 hudColor;
out vec4 viewerOutput;
void main() { viewerOutput = hudColor; }
)GLSL";
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
        if (!compileHostHudShader(0x8B31, vertexSource, hostHudVertexShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
        if (!compileHostHudShader(0x8B30, fragmentSource, hostHudFragmentShader)) return false;
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
        hostHudProgram = shaderGl.createProgram();
        if (!check("create host HUD program")) return false;
        if (!hostHudProgram)
            return fail(*diagnostic, VehicleWindowError50DS::shader, "zero host HUD program name");
        shaderGl.attachShader(hostHudProgram, hostHudVertexShader);
        shaderGl.attachShader(hostHudProgram, hostHudFragmentShader);
        shaderGl.bindAttribLocation(hostHudProgram, vehicleGlPositionAttribute50CZ, "rrPosition");
        shaderGl.bindAttribLocation(hostHudProgram, vehicleGlColorAttribute50CZ, "rrColor");
        shaderGl.bindFragDataLocation(hostHudProgram, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.linkProgram(hostHudProgram);
        I linked = 0;
        shaderGl.getProgramiv(hostHudProgram, 0x8B82, &linked);
        if (!check("link host HUD program")) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(hostHudProgram, static_cast<I>(sizeof(diagnostic->shader.log)),
                nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
            check("read host HUD program log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, "host HUD program link failed");
        }
        diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
        hostHudGeometry.reset(new (std::nothrow) HostHudGeometry);
        if (!hostHudGeometry)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded cold host HUD vertices");
        gl.genBuffers(1, &hostHudVertexBuffer);
        drawGl.bindBuffer(arrayBuffer, hostHudVertexBuffer);
        gl.bufferData(arrayBuffer, sizeof(hostHudGeometry->vertices), nullptr, dynamicDraw);
        if (!check("allocate fixed host HUD vertex buffer")) return false;
        if (!hostHudVertexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host HUD vertex buffer");
        if (!verifyBuffer(arrayBuffer, sizeof(hostHudGeometry->vertices), dynamicDraw)) return false;
        U indices[hostHudMaximumQuads * 6];
        constexpr U corners[] = {0, 1, 2, 0, 2, 3};
        for (U i = 0; i < hostHudMaximumQuads * 6; ++i)
            indices[i] = (i / 6) * 4 + corners[i % 6];
        gl.genBuffers(1, &hostHudIndexBuffer);
        drawGl.bindBuffer(arrayBuffer, hostHudIndexBuffer);
        gl.bufferData(arrayBuffer, sizeof(indices), indices, staticDraw);
        if (!check("upload immutable host HUD quad indices")) return false;
        if (!hostHudIndexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host HUD index buffer");
        if (!verifyBuffer(arrayBuffer, sizeof(indices), staticDraw)) return false;
        U uploaded[256];
        for (U i = 0; i < hostHudMaximumQuads * 6; i += 256) {
            const auto bytes = std::min<U>(256, hostHudMaximumQuads * 6 - i) * sizeof(U);
            gl.getBufferSubData(arrayBuffer, std::intptr_t(i) * sizeof(U), bytes, uploaded);
            if (!check("verify host HUD index bytes")) return false;
            if (std::memcmp(indices + i, uploaded, bytes))
                return fail(*diagnostic, VehicleWindowError50DS::indices, "GPU host HUD index contents mismatch");
        }
        gl.genVertexArrays(1, &hostHudVertexArray);
        drawGl.bindVertexArray(hostHudVertexArray);
        drawGl.bindBuffer(arrayBuffer, hostHudVertexBuffer);
        drawGl.bindBuffer(elementBuffer, hostHudIndexBuffer);
        constexpr I sizes[] = {3, 2, 4};
        constexpr std::uintptr_t offsets[] = {0, 12, 20};
        for (U a = 0; a < 3; ++a) {
            gl.enableVertexAttribArray(a);
            gl.vertexAttribPointer(a, sizes[a], a == 2 ? unsignedByte : floatType,
                a == 2 ? 1 : 0, 24, reinterpret_cast<const void*>(offsets[a]));
        }
        if (!check("create host HUD Vertex48B layout")) return false;
        if (!hostHudVertexArray)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host HUD vertex array");
        return verifyLayout(hostHudVertexBuffer, hostHudIndexBuffer);
    }
    bool drawHostHud() noexcept {
        if (!frameHostHud) return true;
        resetNativeGcLighting();
        // Same pending color target; depth is neither tested nor modified.
        // No texture access, blending, framebuffer change, or heap allocation.
        gl.disable(0x0B71); gl.depthMask(0);
        gl.useProgram(hostHudProgram);
        drawGl.bindVertexArray(hostHudVertexArray);
        drawGl.bindBuffer(arrayBuffer, hostHudVertexBuffer);
        drawGl.bufferSubData(arrayBuffer, 0,
            std::intptr_t(hostHudGeometry->count) * sizeof(assets::Vertex48B), hostHudGeometry->vertices);
        drawGl.bindBuffer(elementBuffer, hostHudIndexBuffer);
        bool success = check("upload bounded live host HUD geometry");
        if (success) {
            const U indices = (hostHudGeometry->count / 4) * 6;
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(indices), 0x1405, nullptr, 0);
            success = check("draw host HUD before private publication");
            if (success) {
                diagnostic->hostHudDraws = 1;
                diagnostic->hostHudVertices = hostHudGeometry->count;
                diagnostic->hostHudIndices = indices;
            }
        }
        // Restore the renderer's own state on success and failure. The next
        // begin also establishes this state, including its world matrix.
        gl.enable(0x0B71); gl.depthMask(1);
        gl.useProgram(shader->program());
        drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
        const bool restored = check("restore world rendering state after host HUD");
        return success && restored;
    }
    bool drawCables(const game::PodCableMesh50FD* cables, U textureName) noexcept {
        if (!cables) return true;
        // The source-generated mesh was preflighted before any frame GL work.
        // Same-context ordering protects this small fixed VBO across frames;
        // old committed images are renderbuffer pixels, not references to it.
        drawGl.bindVertexArray(cableVertexArray);
        drawGl.bindBuffer(arrayBuffer, cableVertexBuffer);
        const U vertexCount = cables->cableCount * game::podCableVerticesPerCable50FD;
        const U indexCount = cables->cableCount * game::podCableIndicesPerCable50FD;
        drawGl.bufferSubData(arrayBuffer, 0, vertexCount * sizeof(cables->vertices[0]), cables->vertices);
        drawGl.bindBuffer(elementBuffer, cableIndexBuffer);
        gl.useProgram(shader->program());
        gl.uniformMatrix4fv(shader->uniforms().worldViewProjection, 1, 0, matrix);
        gl.activeTexture(textureUnit0);
        gl.bindTexture(texture2D, textureName);
        if (gl.bindSampler) gl.bindSampler(0, 0);
        gl.uniform1i(shader->uniforms().useTexture, 1);
        gl.uniform1i(shader->uniforms().image, 0);
        gl.uniform1i(shader->uniforms().useLighting, 0); // Energy cables stay emissive.
        resetNativeGcLighting();
        if (!check("upload actual current source cable vertices and bind real texture")) return false;
        drawGl.drawElementsBaseVertex(0x0004, indexCount, 0x1405, nullptr, 0);
        if (!check("draw source cable strips after pod")) return false;
        ++diagnostic->cableDraws;
        diagnostic->cableVertices += vertexCount;
        diagnostic->cableIndices += indexCount;
        return true;
    }
    bool drawGameplayBinders() noexcept {
        if(!frameGameplayBinderCount)return true;
        // Native semantic Diffuse material: packetRGBA and original ATM pixels.
        // Existing native world shader handles fog/clipping; exact source
        // raster quantization, framebuffer-alpha and group ordering remain open.
        const auto* bank=request.gameplayBinderTextures;
        if(!bank||!trackProgram||!cableVertexArray)
            return fail(*diagnostic,VehicleWindowError50DS::material,"Binder shader, pixel bank or quad buffers missing");
        const U texture=bank->texture(frameGameplayBinderImage);if(!texture)return false;
        resetPodState();
        gl.useProgram(trackProgram);gl.uniform1i(rcCrystalPassUniform,0);gl.uniform1i(rcCrystalPayloadUniform,0);
        gl.uniformMatrix4fv(trackMatrixUniform,1,0,matrix);
        gl.uniform1f(trackColorScaleUniform,255.f/128.f);
        gl.uniform1i(trackUseTextureUniform,1);gl.uniform1i(trackImageUniform,0);
        gl.uniform1i(trackDiscardZeroAlphaUniform,1); // DefaultMT: ATE1,ATST6,AREF0.
        gl.uniform1i(trackEffectModeUniform,1);
        gl.uniform1f(trackAlphaScaleUniform,(255.f/128.f)*(255.f/128.f));
        gl.uniform1i(trackUnlitUniform,1); // Diffuse ambient is already source-packed on CPU.
        gl.uniform1i(trackUseSourceNormalsUniform,0);gl.uniform1i(trackProjectionPassUniform,0);
        gl.uniform4f(trackUvTranslationUniform,0.f,0.f,0.f,0.f);
        gl.uniform1i(nativeShadowEnabledUniform,0);gl.uniform1i(nativeShadowOpponentCountUniform,0);
        const auto& fog=request.track.worldFog;
        // Authored fog is on by default; retain the explicit scene-fog
        // diagnostic opt-out without dropping the actual Binder geometry.
        gl.uniform1i(worldFogEnabledUniform,fog.enabled?1:0);
        gl.uniform4f(worldFogCameraUniform,worldFogCamera[0],worldFogCamera[1],worldFogCamera[2],0.f);
        gl.uniform4f(worldFogColorUniform,fog.color[0],fog.color[1],fog.color[2],1.f);
        gl.uniform4f(worldFogRangeUniform,fog.start,fog.end,0.f,0.f);
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,texture);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.enable(0x0BE2);gl.blendEquation(0x8006);gl.blendFunc(0x0302,0x0303);
        gl.enable(0x0B71);gl.depthFunc(0x0203);gl.depthMask(0);gl.disable(0x0B44);
        drawGl.bindVertexArray(cableVertexArray);drawGl.bindBuffer(arrayBuffer,cableVertexBuffer);
        drawGl.bindBuffer(elementBuffer,cableIndexBuffer);
        for(U i=0;i<frameGameplayBinderCount;++i){
            drawGl.bufferSubData(arrayBuffer,0,sizeof(frameGameplayBinders[i].vertices),frameGameplayBinders[i].vertices.data());
            // The first canonical cable segment has the SAME source quad
            // triangles0,1,2 /2,1,3; its cold36-vertex storage is ample for4.
            drawGl.drawElementsBaseVertex(0x0004,6,0x1405,nullptr,0);
            if(!check("draw original gameplay Binder quad"))return false;
            ++diagnostic->gameplayBinderDraws;
        }
        resetPodState();return check("restore native world state after Binder");
    }
    bool createAttachment(U& name, U format, U attachment) noexcept {
        gl.genRenderbuffers(1, &name);
        gl.bindRenderbuffer(renderbuffer, name);
        gl.renderbufferStorage(renderbuffer, format, maximumWidth, maximumHeight);
        if (!check("allocate private staging renderbuffer")) return false;
        if (!name) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero renderbuffer name");
        I actualWidth = 0, actualHeight = 0, actualFormat = 0, actualSamples = 0;
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D42, &actualWidth);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D43, &actualHeight);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8D44, &actualFormat);
        gl.getRenderbufferParameteriv(renderbuffer, 0x8CAB, &actualSamples);
        if (!check("verify private staging renderbuffer")) return false;
        if (actualWidth != maximumWidth || actualHeight != maximumHeight || actualFormat != static_cast<I>(format) || actualSamples)
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "renderbuffer storage mismatch");
        gl.framebufferRenderbuffer(framebuffer, attachment, renderbuffer, name);
        return check("attach private staging renderbuffer");
    }
    bool ensureUiSourceReceiptLifetime(VehicleWindowDiagnostic50DS& d) noexcept {
        if (uiSourceReceiptLifetime)return true;
        try {uiSourceReceiptLifetime=std::make_shared<const std::uint8_t>(0);return true;}
        catch (...) {return fail(d,VehicleWindowError50DS::allocation,"allocate source receipt lifetime");}
    }
    bool createRetainedUiSourceTarget() noexcept {
        gl.genFramebuffers(1,&uiRetainedTarget);gl.bindFramebuffer(framebuffer,uiRetainedTarget);
        gl.genTextures(1,&uiRetainedColor);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,uiRetainedColor);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,640,448,0,0x1908,0x1401,nullptr);
        if (!check("allocate distinct retained source RGBA8 texture"))return false;
        if (!uiRetainedTarget || !uiRetainedColor || uiRetainedTarget==uiSourceTarget || uiRetainedColor==uiSourceColor)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"retained source names must be distinct");
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if (!check("verify retained source texture"))return false;
        if (w!=640 || h!=448 || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"retained source texture storage mismatch");
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,uiRetainedColor,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);gl.bindTexture(texture2D,0);
        if (!check("complete retained source framebuffer"))return false;
        if (status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete retained source framebuffer");}
        return true;
    }
    bool hasUiByteBlendNames()const noexcept {
        return uiBlendIncomingTarget || uiBlendIncomingColor || uiBlendEffectTarget || uiBlendEffectColor ||
            uiBlendProgram || uiBlendVertexShader || uiBlendFragmentShader || uiBlendVertexArray;
    }
    bool createUiByteBlendTarget(U& target,U& color) noexcept {
        gl.genFramebuffers(1,&target);
        if(!check("allocate byte-blend framebuffer name"))return false;
        if(!target || target==uiSourceTarget || target==uiRetainedTarget ||
            (&target==&uiBlendEffectTarget && target==uiBlendIncomingTarget))
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"byte-blend framebuffer must be distinct");
        gl.genTextures(1,&color);
        if(!check("allocate byte-blend texture name"))return false;
        if(!color || color==uiSourceColor || color==uiRetainedColor ||
            (&color==&uiBlendEffectColor && color==uiBlendIncomingColor))
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"byte-blend texture must be distinct");
        gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,color);
        if(gl.bindSampler)gl.bindSampler(0,0);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0);
        gl.texImage2D(texture2D,0,rgba8,640,448,0,rgba,unsignedByte,nullptr);
        if(!check("allocate immutable-pair byte-blend RGBA8 storage"))return false;
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if(!check("verify byte-blend RGBA8 storage"))return false;
        if(w!=640 || h!=448 || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"byte-blend storage must be640x448 RGBA8");
        gl.bindFramebuffer(framebuffer,target);
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,color,0);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        if(!check("verify byte-blend framebuffer"))return false;
        if(status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete byte-blend framebuffer");}
        return true;
    }
    bool createUiByteBlendProgram() noexcept {
        // No new loader, shader, VAO or allocation on the normal UI path.
        if(!load(gl.drawArrays,"glDrawArrays"))return false;
        static const char* vertexSource=R"GLSL(#version 150 core
void main() {
    vec2 p=gl_VertexID==0 ? vec2(-1.0,-1.0) :
           gl_VertexID==1 ? vec2(3.0,-1.0) : vec2(-1.0,3.0);
    gl_Position=vec4(p,0.0,1.0);
}
)GLSL";
        static const char* fragmentSource=R"GLSL(#version 150 core
uniform sampler2D oldImage;
uniform sampler2D incomingImage;
uniform int sourceAlpha;
out vec4 viewerOutput;
int signedFloor128(int value) {
    return value>=0 ? value/128 : -((-value+127)/128);
}
void main() {
    // Both source textures have identical lower-left GL orientation. Source
    // top-down639x447 coverage excludes GL row0 and column639. No UV mix,
    // rescaling, filtering, gamma conversion or preceding-effect feedback.
    ivec2 q=ivec2(gl_FragCoord.xy);
    ivec4 oldByte=ivec4(floor(texelFetch(oldImage,q,0)*255.0+0.5));
    if(q.x==639 || q.y==0) {
        viewerOutput=vec4(oldByte)/255.0;
        return;
    }
    ivec3 incomingByte=ivec3(floor(texelFetch(incomingImage,q,0).rgb*255.0+0.5));
    ivec3 product=(incomingByte-oldByte.rgb)*sourceAlpha;
    ivec3 result=oldByte.rgb+ivec3(signedFloor128(product.r),
        signedFloor128(product.g),signedFloor128(product.b));
    // Alpha is the raw source byte, not alpha/128. Endpoints still draw.
    viewerOutput=vec4(vec3(result),float(sourceAlpha))/255.0;
}
)GLSL";
        const auto compile=[&](U type,const char* source,U& name) noexcept {
            name=shaderGl.createShader(type);
            if(!check("create byte-blend shader"))return false;
            if(!name)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero byte-blend shader");
            shaderGl.shaderSource(name,1,&source,nullptr);shaderGl.compileShader(name);
            I compiled=0;shaderGl.getShaderiv(name,0x8B81,&compiled);
            if(!check("compile byte-blend shader"))return false;
            if(compiled)return true;
            shaderGl.getShaderInfoLog(name,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read byte-blend shader log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"byte-blend shader compilation failed");
        };
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::vertex;
        if(!compile(0x8B31,vertexSource,uiBlendVertexShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::fragment;
        if(!compile(0x8B30,fragmentSource,uiBlendFragmentShader))return false;
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::link;
        uiBlendProgram=shaderGl.createProgram();
        if(!check("create byte-blend program"))return false;
        if(!uiBlendProgram)return fail(*diagnostic,VehicleWindowError50DS::shader,"zero byte-blend program");
        shaderGl.attachShader(uiBlendProgram,uiBlendVertexShader);shaderGl.attachShader(uiBlendProgram,uiBlendFragmentShader);
        shaderGl.bindFragDataLocation(uiBlendProgram,vehicleGlColorOutput50CZ,"viewerOutput");
        shaderGl.linkProgram(uiBlendProgram);
        I linked=0;shaderGl.getProgramiv(uiBlendProgram,0x8B82,&linked);
        if(!check("link byte-blend program"))return false;
        if(!linked){
            shaderGl.getProgramInfoLog(uiBlendProgram,static_cast<I>(sizeof(diagnostic->shader.log)),nullptr,diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log)-1]='\0';
            check("read byte-blend program log");
            return fail(*diagnostic,VehicleWindowError50DS::shader,"byte-blend program link failed");
        }
        uiBlendOldUniform=shaderGl.getUniformLocation(uiBlendProgram,"oldImage");
        uiBlendIncomingUniform=shaderGl.getUniformLocation(uiBlendProgram,"incomingImage");
        uiBlendAlphaUniform=shaderGl.getUniformLocation(uiBlendProgram,"sourceAlpha");
        if(!check("locate byte-blend uniforms"))return false;
        if(uiBlendOldUniform<0 || uiBlendIncomingUniform<0 || uiBlendAlphaUniform<0)
            return fail(*diagnostic,VehicleWindowError50DS::shader,"missing byte-blend uniform");
        gl.genVertexArrays(1,&uiBlendVertexArray);
        if(!check("allocate byte-blend vertex array"))return false;
        if(!uiBlendVertexArray)return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero byte-blend vertex array");
        diagnostic->shader.stage=VehicleGlShaderStage50CZ::none;return true;
    }
    bool restoreUiByteBlendBindings() noexcept {
        // Explicit renderer baseline, not restoration of arbitrary foreign GL
        // state. No EBO is touched and no source depth clear is performed here.
        gl.useProgram(0);drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
        for(U unit=0;unit<2;++unit){gl.activeTexture(textureUnit0+unit);gl.bindTexture(texture2D,0);
            if(gl.bindSampler)gl.bindSampler(unit,0);}
        gl.activeTexture(textureUnit0);gl.bindFramebuffer(framebuffer,0);
        gl.disable(0x0BE2);gl.disable(0x0C11);gl.enable(0x0B71);
        gl.depthMask(1);gl.depthFunc(0x0201);gl.clearDepth(1.0);gl.colorMask(1,1,1,1);
        return check("restore byte-blend renderer bindings without depth clear");
    }
    bool deleteUiByteBlendNames(VehicleWindowDiagnostic50DS& d) noexcept {
        return deleteShaderName(uiBlendProgram,true,d) &&
            deleteShaderName(uiBlendVertexShader,false,d) && deleteShaderName(uiBlendFragmentShader,false,d) &&
            deleteName(uiBlendVertexArray,gl.deleteVertexArrays,d) &&
            deleteName(uiBlendEffectTarget,gl.deleteFramebuffers,d) && deleteName(uiBlendEffectColor,gl.deleteTextures,d) &&
            deleteName(uiBlendIncomingTarget,gl.deleteFramebuffers,d) && deleteName(uiBlendIncomingColor,gl.deleteTextures,d);
    }
    bool copyUiSourceToOpaque(U sourceTarget,const float* clear,I outputWidth,I outputHeight) noexcept {
        const auto pending=static_cast<std::uint8_t>(uiPublished^1u);
        // Only this independent private presentation surface gets alpha1.
        gl.disable(0x0C11);gl.disable(0x0BE2);gl.disable(0x8DB9);gl.disable(0x0BD0);
        gl.colorMask(1,1,1,1);
        gl.bindFramebuffer(framebuffer,uiTargets[pending]);gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        gl.viewport(0,0,outputWidth,outputHeight);gl.clearColor(clear[0],clear[1],clear[2],1.f);gl.clear(colorBufferBit);
        const double fit=std::min(double(outputWidth)/640.0,double(outputHeight)/448.0);
        const I rw=std::max<I>(1,static_cast<I>(std::floor(640.0*fit)));
        const I rh=std::max<I>(1,static_cast<I>(std::floor(448.0*fit)));
        const I x=(outputWidth-rw)/2,y=(outputHeight-rh)/2;
        gl.bindFramebuffer(readFramebuffer,sourceTarget);gl.readBuffer(colorAttachment);
        gl.blitFramebuffer(0,0,640,448,x,y,x+rw,y+rh,colorBufferBit,0x2600);
        gl.bindFramebuffer(framebuffer,uiTargets[pending]);
        gl.colorMask(0,0,0,1);gl.clear(colorBufferBit);gl.colorMask(1,1,1,1);
        const bool copied=check("copy source UI RGB to separate opaque presentation");
        // Preserve the previous combined path's post-copy baseline, including
        // a failed opaque copy. Source acceptance/history is already separate.
        // Headless historically leaves this private UI FBO bound; window
        // success reaches framebuffer0 through copyToWindow below.
        gl.disable(0x0C11);gl.disable(0x0BE2);gl.depthMask(1);gl.depthFunc(0x0201);
        gl.colorMask(1,1,1,1);gl.clearDepth(1.0);
        drawGl.bindVertexArray(0);drawGl.bindBuffer(arrayBuffer,0);
        const bool restored=check("finish source publication baseline");
        return copied && restored;
    }
    bool publishUiSource(const float* clear,I outputWidth,I outputHeight) noexcept {
        const auto pending=static_cast<std::uint8_t>(uiPublished^1u);
        if(!copyUiSourceToOpaque(uiSourceTarget,clear,outputWidth,outputHeight))return false;
        uiPrivatePublicationGeneration=0;
        uiPublished=pending;displayedTarget=uiTargets[pending];
        width=committedWidth=outputWidth;height=committedHeight=outputHeight;
        imageCommitted=true;diagnostic->originalUiAccepted=true;
        // Private publication may survive a failed window copy, as before.
        // Such a partial window operation must not mint a retained publication.
        uiSourcePublishedTargetGeneration=0;
        bool accepted=true;
        if (request.output==VehicleWindowOutput50DS::defaultBackBuffer){
            accepted=copyToWindow();
            if (!accepted && diagnostic->error==VehicleWindowError50DS::gl)
                diagnostic->error=VehicleWindowError50DS::presentation;
        }
        if (accepted){
            uiSourcePublishedTargetGeneration=uiSourceTargetGeneration;
            diagnostic->originalUiPublicationGeneration=++uiSourcePublicationGeneration;
        }
        return accepted;
    }
    bool createOriginalUiSourceTarget() noexcept {
        if (!request.enableOriginalUiSourceRgba)return true;
        I textureLimit=0;gl.getIntegerv(0x0D33,&textureLimit);
        if (!check("query source UI texture limits"))return false;
        if (textureLimit<640)return fail(*diagnostic,VehicleWindowError50DS::limits,"source UI texture dimensions unavailable");
        gl.genFramebuffers(1,&uiSourceTarget);gl.bindFramebuffer(framebuffer,uiSourceTarget);
        gl.genTextures(1,&uiSourceColor);gl.activeTexture(textureUnit0);gl.bindTexture(texture2D,uiSourceColor);
        gl.texParameteri(texture2D,0x2801,0x2600);gl.texParameteri(texture2D,0x2800,0x2600);
        gl.texParameteri(texture2D,0x2802,0x812F);gl.texParameteri(texture2D,0x2803,0x812F);
        gl.texParameteri(texture2D,0x813C,0);gl.texParameteri(texture2D,0x813D,0);
        drawGl.bindBuffer(0x88EC,0); // Null data is allocation, never an inherited unpack-PBO offset.
        gl.texImage2D(texture2D,0,rgba8,640,448,0,0x1908,0x1401,nullptr);
        if (!check("allocate fixed source UI RGBA8 texture") || !uiSourceTarget || !uiSourceColor)
            return fail(*diagnostic,VehicleWindowError50DS::allocation,"source UI framebuffer/texture allocation");
        I w=0,h=0,format=0;
        gl.getTexLevelParameteriv(texture2D,0,0x1000,&w);gl.getTexLevelParameteriv(texture2D,0,0x1001,&h);
        gl.getTexLevelParameteriv(texture2D,0,0x1003,&format);
        if (!check("verify source UI texture") || w!=640 || h!=448 || format!=static_cast<I>(rgba8))
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"source UI texture storage mismatch");
        gl.framebufferTexture2D(framebuffer,colorAttachment,texture2D,uiSourceColor,0);
        gl.genRenderbuffers(1,&uiSourceDepth);gl.bindRenderbuffer(renderbuffer,uiSourceDepth);
        gl.renderbufferStorage(renderbuffer,depth24,640,448);
        I samples=0;gl.getRenderbufferParameteriv(renderbuffer,0x8D42,&w);
        gl.getRenderbufferParameteriv(renderbuffer,0x8D43,&h);gl.getRenderbufferParameteriv(renderbuffer,0x8D44,&format);
        gl.getRenderbufferParameteriv(renderbuffer,0x8CAB,&samples);
        if (!check("allocate source UI depth") || !uiSourceDepth || w!=640 || h!=448 ||
            format!=static_cast<I>(depth24) || samples)
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"source UI depth storage mismatch");
        gl.framebufferRenderbuffer(framebuffer,depthAttachment,renderbuffer,uiSourceDepth);
        gl.drawBuffer(colorAttachment);gl.readBuffer(colorAttachment);
        const U status=gl.checkFramebufferStatus(framebuffer);
        gl.bindTexture(texture2D,0);
        if (!check("complete source UI framebuffer"))return false;
        if (status!=framebufferComplete){diagnostic->framebufferStatus=status;
            return fail(*diagnostic,VehicleWindowError50DS::framebuffer,"incomplete source UI framebuffer");}
        return true;
    }
    bool createSpecularColorAttachment50LA(U& name) noexcept {
        // Gameplay banks keep their existing transaction/depth owners. Only
        // authenticated body-pair capability changes color storage to RGBA8
        // textures; UI targets, receipts and source-alpha owners are unrelated.
        I maximum = 0; gl.getIntegerv(0x0D33, &maximum);
        if (!check("query gameplay pair texture dimensions")) return false;
        if (maximumWidth > maximum || maximumHeight > maximum)
            return fail(*diagnostic, VehicleWindowError50DS::limits, "gameplay pair texture exceeds context limit");
        gl.genTextures(1, &name); gl.activeTexture(textureUnit0); gl.bindTexture(texture2D, name);
        if (gl.bindSampler) gl.bindSampler(0, 0);
        gl.texParameteri(texture2D, 0x2801, 0x2600); gl.texParameteri(texture2D, 0x2800, 0x2600);
        gl.texParameteri(texture2D, 0x2802, 0x812F); gl.texParameteri(texture2D, 0x2803, 0x812F);
        gl.texParameteri(texture2D, 0x813C, 0); gl.texParameteri(texture2D, 0x813D, 0);
        drawGl.bindBuffer(0x88EC, 0); // No inherited unpack-PBO offset.
        gl.texImage2D(texture2D, 0, rgba8, maximumWidth, maximumHeight, 0, rgba, unsignedByte, nullptr);
        if (!check("allocate private gameplay source-pair RGBA8 color") || !name)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "gameplay source-pair texture allocation failed");
        I w = 0, h = 0, format = 0;
        gl.getTexLevelParameteriv(texture2D, 0, 0x1000, &w); gl.getTexLevelParameteriv(texture2D, 0, 0x1001, &h);
        gl.getTexLevelParameteriv(texture2D, 0, 0x1003, &format);
        if (!check("verify private gameplay source-pair texture")) return false;
        if (w != maximumWidth || h != maximumHeight || format != static_cast<I>(rgba8))
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "gameplay source-pair texture storage mismatch");
        gl.framebufferTexture2D(framebuffer, colorAttachment, texture2D, name, 0);
        gl.bindTexture(texture2D, 0);
        return check("attach private gameplay source-pair color");
    }
    bool createTargets() noexcept {
        I maximum = 0, viewportLimits[2]{};
        gl.getIntegerv(0x84E8, &maximum); // GL_MAX_RENDERBUFFER_SIZE
        gl.getIntegerv(0x0D3A, viewportLimits); // GL_MAX_VIEWPORT_DIMS
        if (!check("query target limits")) return false;
        if (maximumWidth > maximum || maximumHeight > maximum ||
            maximumWidth > viewportLimits[0] || maximumHeight > viewportLimits[1] ||
            (request.enableOriginalUiSourceRgba && (maximum<640 || viewportLimits[0]<640 || viewportLimits[1]<448)))
            return fail(*diagnostic, VehicleWindowError50DS::limits, "explicit resolution exceeds GL limits");
        for (U b = 0; b < 2; ++b) {
            if(!frontendOnly){
            gl.genFramebuffers(1, &targets[b]);
            gl.bindFramebuffer(framebuffer, targets[b]);
            if (!check("create unpublished framebuffer")) return false;
            if (!targets[b]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero framebuffer name");
            if (!(hasSpecularPairs50LA ? createSpecularColorAttachment50LA(sourcePairColors50LA[b]) :
                    createAttachment(colors[b], rgba8, colorAttachment)) ||
                !createAttachment(depths[b], depth24, depthAttachment)) return false;
            gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
            const U status = gl.checkFramebufferStatus(framebuffer);
            if (!check("check unpublished framebuffer")) return false;
            if (status != framebufferComplete) {
                diagnostic->framebufferStatus = status;
                return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete unpublished framebuffer");
            }
            }
            if (request.enableOriginalUi) {
                gl.genFramebuffers(1, &uiTargets[b]);
                gl.bindFramebuffer(framebuffer, uiTargets[b]);
                if (!check("create private original UI framebuffer")) return false;
                if (!uiTargets[b]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero UI framebuffer");
                if (!createAttachment(uiColors[b], rgba8, colorAttachment) ||
                    !createAttachment(uiDepths[b], depth24, depthAttachment)) return false;
                gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
                const U uiStatus = gl.checkFramebufferStatus(framebuffer);
                if (!check("check private original UI framebuffer")) return false;
                if (uiStatus != framebufferComplete) {
                    diagnostic->framebufferStatus = uiStatus;
                    return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete original UI framebuffer");
                }
            }
            if (request.enableHostHud) {
                gl.genFramebuffers(1, &menuTargets[b]);
                gl.bindFramebuffer(framebuffer, menuTargets[b]);
                if (!check("create private host menu framebuffer")) return false;
                if (!menuTargets[b])
                    return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero host menu framebuffer");
                if (!createAttachment(menuColors[b], rgba8, colorAttachment)) return false;
                gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
                const U menuStatus = gl.checkFramebufferStatus(framebuffer);
                if (!check("check private host menu framebuffer")) return false;
                if (menuStatus != framebufferComplete) {
                    diagnostic->framebufferStatus = menuStatus;
                    return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete host menu framebuffer");
                }
            }
        }
        return createOriginalUiSourceTarget();
    }
    bool checkDefaultFramebuffer() noexcept {
        gl.bindFramebuffer(framebuffer, 0);
        gl.drawBuffer(backBuffer); gl.readBuffer(backBuffer);
        const U status = gl.checkFramebufferStatus(framebuffer);
        if (!check("select default window back buffer")) return false;
        if (status != framebufferComplete) {
            diagnostic->framebufferStatus = status;
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer,
                "default window framebuffer is incomplete");
        }
        return true;
    }
    bool checkTextureSampling(I major, I minor) noexcept {
        if (!usesTextures()) return true;
        I profile = 0, imageUnits = 0;
        gl.getIntegerv(0x9126, &profile); // GL_CONTEXT_PROFILE_MASK
        gl.getIntegerv(0x8872, &imageUnits); // GL_MAX_TEXTURE_IMAGE_UNITS
        if (!check("query caller-material texture support")) return false;
        if (!(profile & 1) || imageUnits < 1)
            return fail(*diagnostic, VehicleWindowError50DS::context,
                "caller material textures require desktop core GL and texture unit zero");
        bool samplerObjects = major > 3 || (major == 3 && minor >= 3);
        if (!samplerObjects) {
            // GL 3.2 has no core sampler objects; its optional ARB extension
            // does. A nonnull resolver result alone is not capability evidence.
            if (!load(gl.getStringi, "glGetStringi")) return false;
            I count = 0;
            gl.getIntegerv(0x821D, &count); // GL_NUM_EXTENSIONS
            if (!check("query GL 3.2 sampler extension count")) return false;
            if (count < 0 || count > 65536)
                return fail(*diagnostic, VehicleWindowError50DS::limits, "invalid bounded GL extension count");
            for (I i = 0; i < count; ++i) {
                const auto* extension = gl.getStringi(0x1F03, static_cast<U>(i));
                if (!check("query GL 3.2 sampler extension")) return false;
                if (!extension)
                    return fail(*diagnostic, VehicleWindowError50DS::context, "null GL extension name");
                if (!std::strcmp(reinterpret_cast<const char*>(extension), "GL_ARB_sampler_objects")) {
                    samplerObjects = true;
                    break;
                }
            }
        }
        return !samplerObjects || load(gl.bindSampler, "glBindSampler");
    }
    U garageSampler50KG(const MaterialTextureSampler50EB& sampler)const noexcept {
        const auto filter=[](MaterialTextureFilter50EB v){return v==MaterialTextureFilter50EB::nearest||v==MaterialTextureFilter50EB::linear;};
        const auto wrap=[](MaterialTextureWrap50EB v){return v==MaterialTextureWrap50EB::repeat||v==MaterialTextureWrap50EB::clampToEdge;};
        if(!garageSamplersReady50KG||!filter(sampler.minification)||sampler.minification!=sampler.magnification||
           !wrap(sampler.s)||!wrap(sampler.t))return 0;
        const U index=(sampler.minification==MaterialTextureFilter50EB::linear?4u:0u)|
            (sampler.s==MaterialTextureWrap50EB::clampToEdge?2u:0u)|(sampler.t==MaterialTextureWrap50EB::clampToEdge?1u:0u);
        return garageSamplers50KG[index];
    }
    bool ensureGarageSamplers50KG()noexcept {
        if(garageSamplersReady50KG){
            for(U name:garageSamplers50KG)if(!name)return fail(*diagnostic,VehicleWindowError50DS::material,"Garage sampler owner lost a published name");
            return true;
        }
        // A partial creation owns its names until real cleanup, never replay it.
        for(U name:garageSamplers50KG)if(name)return fail(*diagnostic,VehicleWindowError50DS::material,"partial Garage sampler creation retained for cleanup");
        // bindSampler exists only after the unchanged advertised capability check.
        // Ordinary startup and all non-Garage draws retain their original requirements.
        if(!gl.bindSampler)return fail(*diagnostic,VehicleWindowError50DS::context,"Garage per-material sampling requires advertised sampler objects");
        if(!load(gl.genSamplers,"glGenSamplers")||!load(gl.deleteSamplers,"glDeleteSamplers")||
           !load(gl.samplerParameteri,"glSamplerParameteri")||!load(gl.getSamplerParameteriv,"glGetSamplerParameteriv"))return false;
        constexpr U parameters[]={0x2801,0x2800,0x2802,0x2803};
        for(U i=0;i<8;++i){
            gl.genSamplers(1,&garageSamplers50KG[i]);
            if(!check("create bounded Garage sampler object"))return false;
            if(!garageSamplers50KG[i])return fail(*diagnostic,VehicleWindowError50DS::allocation,"zero Garage sampler name");
            const I values[]={(i&4u)?0x2601:0x2600,(i&4u)?0x2601:0x2600,
                (i&2u)?0x812F:0x2901,(i&1u)?0x812F:0x2901};
            for(U j=0;j<4;++j)gl.samplerParameteri(garageSamplers50KG[i],parameters[j],values[j]);
            if(!check("configure immutable Garage sampler object"))return false;
            for(U j=0;j<4;++j){
                I actual=-1;gl.getSamplerParameteriv(garageSamplers50KG[i],parameters[j],&actual);
                if(!check("verify immutable Garage sampler object"))return false;
                if(actual!=values[j])return fail(*diagnostic,VehicleWindowError50DS::material,"Garage sampler parameter publication mismatch");
            }
        }
        garageSamplersReady50KG=true;return true;
    }
    bool checkOriginalUiSourceSupport(I major,I minor) noexcept {
        if (!request.enableOriginalUiSourceRgba)return true;
        I units=0;gl.getIntegerv(0x8872,&units);
        if (!check("query source UI composition texture units"))return false;
        bool supported=major>4 || (major==4 && minor>=5);
        bool clipControl=supported;
        bool coherentFetch=false;
        if (!load(gl.getStringi,"glGetStringi"))return false;
        I count=0;gl.getIntegerv(0x821D,&count);
        if (!check("query source UI extension count"))return false;
        if (count<0 || count>65536)return fail(*diagnostic,VehicleWindowError50DS::limits,"invalid extension count");
        for (I i=0;i<count;++i) {
            const auto* extension=gl.getStringi(0x1F03,static_cast<U>(i));
            if (!check("query source UI fetch, texture barrier and clip capabilities"))return false;
            if (!extension)return fail(*diagnostic,VehicleWindowError50DS::context,"null extension name");
            const auto* name=reinterpret_cast<const char*>(extension);
            if (!std::strcmp(name,"GL_ARB_texture_barrier"))supported=true;
            if (!std::strcmp(name,"GL_ARB_clip_control"))clipControl=true;
            // The non_coherent extension alone cannot order overlapping triangles.
            if (!std::strcmp(name,"GL_EXT_shader_framebuffer_fetch"))coherentFetch=true;
        }
        if (units<(coherentFetch?1:2))return fail(*diagnostic,VehicleWindowError50DS::context,
            "source UI has insufficient fragment texture units");
        if (!coherentFetch && !supported)return fail(*diagnostic,VehicleWindowError50DS::context,
            "source UI requires coherent EXT_shader_framebuffer_fetch, GL4.5 or ARB_texture_barrier");
        if ((!coherentFetch && !load(gl.textureBarrier,"glTextureBarrier")) ||
            (clipControl && !load(gl.clipControl,"glClipControl")))return false;
        uiSourceCoherentFramebufferFetch=coherentFetch;
        return true;
    }
    bool checkSpecularPairSupport50LA(I major, I minor) noexcept {
        if (!hasSpecularPairs50LA) return true;
        I units = 0; gl.getIntegerv(0x8872, &units);
        if (!check("query body-pair fragment texture units")) return false;
        if (units < 2) return fail(*diagnostic, VehicleWindowError50DS::context, "body pairs need two fragment texture units");
        bool coreOrArb = major > 4 || (major == 4 && minor >= 5), nv = false;
        if (!load(gl.getStringi, "glGetStringi")) return false;
        I count = 0; gl.getIntegerv(0x821D, &count);
        if (!check("query body-pair texture-barrier extensions")) return false;
        if (count < 0 || count > 65536)
            return fail(*diagnostic, VehicleWindowError50DS::limits, "invalid bounded body-pair extension count");
        for (I i = 0; i < count; ++i) {
            const auto* extension = gl.getStringi(0x1F03, static_cast<U>(i));
            if (!check("read body-pair texture-barrier capability")) return false;
            if (!extension) return fail(*diagnostic, VehicleWindowError50DS::context, "null body-pair extension name");
            const auto* name = reinterpret_cast<const char*>(extension);
            if (!std::strcmp(name, "GL_ARB_texture_barrier")) coreOrArb = true;
            if (!std::strcmp(name, "GL_NV_texture_barrier")) nv = true;
        }
        if (!coreOrArb && !nv)
            return fail(*diagnostic, VehicleWindowError50DS::context, "body pairs require GL4.5, ARB or NV texture barrier");
        return load(gl.textureBarrier, coreOrArb ? "glTextureBarrier" : "glTextureBarrierNV");
    }
    bool checkContext() noexcept {
        const auto* version = gl.getString(0x1F02); // GL_VERSION
        I major = 0, minor = 0;
        gl.getIntegerv(0x821B, &major); gl.getIntegerv(0x821C, &minor);
        if (!check("query desktop GL version")) return false;
        if (!version || std::strncmp(reinterpret_cast<const char*>(version), "OpenGL ES", 9) == 0 ||
            major < 3 || (major == 3 && minor < 2))
            return fail(*diagnostic, VehicleWindowError50DS::context, "desktop OpenGL 3.2 or newer required");
        if (!checkCoverageSupport(major, minor)) return false;
        if (!checkTextureSampling(major, minor) || !checkOriginalUiSourceSupport(major,minor) ||
            !checkSpecularPairSupport50LA(major, minor)) return false;
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
            if (!checkDefaultFramebuffer()) return false;
            I doubleBuffered = 0, stereo = 0, sampleBuffers = 0, samples = 0;
            gl.getIntegerv(0x0C32, &doubleBuffered); // GL_DOUBLEBUFFER
            gl.getIntegerv(0x0C33, &stereo); // GL_STEREO
            gl.getIntegerv(0x80A8, &sampleBuffers); gl.getIntegerv(0x80A9, &samples);
            if (!check("query default window framebuffer format")) return false;
            if (!doubleBuffered || stereo || sampleBuffers || samples)
                return fail(*diagnostic, VehicleWindowError50DS::context,
                    "nonstereo double-buffered nonmultisampled window required");
        }
        gl.getIntegerv(0x0D32, &clipDistances); // GL_MAX_CLIP_DISTANCES
        if (!check("query GL clip-distance limit")) return false;
        // GL 3.2 guarantees at least eight. Keep state work bounded even if a
        // broken resolver or driver returns an implausible capability count.
        if (clipDistances < 8 || clipDistances > 64)
            return fail(*diagnostic, VehicleWindowError50DS::limits, "invalid GL clip-distance limit");
        return true;
    }
    bool createRacerArchetypes(const VehicleWindowRequest50DS& input) noexcept {
        racerArchetypeCount = input.racerArchetypeCount;
        opponentMaterialStride = materialCount;
        for (U a = 0; a < racerArchetypeCount; ++a) {
            const auto& source = input.racerArchetypes[a];
            auto& target = racerArchetypes[a];
            target.resources.componentCount = source.assets->componentCount();
            target.textures = source.materials.textures;
            target.texturePublication = target.textures ? target.textures->publicationToken() : 0;
            target.specularTextures50LA = source.materials.specularTextures50LA;
            target.specularPublication50LA = target.specularTextures50LA ? target.specularTextures50LA->publicationToken() : 0;
            target.cables = source.cables;
            target.cableTexturePublication = target.cables.textures ?
                target.cables.textures->publicationToken() : 0;
            target.cableTextureName = target.cables.textures ?
                target.cables.textures->texture(target.cables.textureIndex) : 0;
            if (target.cables.textures && !target.cableTextureName)
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required racer cable texture disappeared during create");
            for (U c = 0; c < target.resources.componentCount; ++c) {
                diagnostic->component = c;
                const auto* mesh = target.meshes[c] = source.assets->preparedMesh(components[c]);
                const auto* scene = target.scenes[c] = source.assets->scene(components[c]);
                if (!mesh || !scene || !mesh->indices || !mesh->spans ||
                    !mesh->vertexCount || !mesh->indexCount || !mesh->spanCount ||
                    mesh->vertexCount > game::maxRigidVertices49S ||
                    mesh->indexCount > assets::max_mesh_indices ||
                    mesh->spanCount > game::maxVehicleDrawSpans50CM ||
                    std::uint64_t(mesh->vertexCount) * sizeof(assets::Vertex48B) >
                        std::uint64_t(std::numeric_limits<std::intptr_t>::max()) ||
                    std::uint64_t(mesh->indexCount) * sizeof(U) >
                        std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
                    return fail(*diagnostic, VehicleWindowError50DS::assets,
                        "invalid prepared racer 50CR mesh");
                for (U i = 0; i < mesh->spanCount; ++i)
                    if (mesh->spans[i].material_index >= scene->materials.size())
                        return fail(*diagnostic, VehicleWindowError50DS::assets,
                            "racer original material index outside loaded scene");
                target.materialOffsets[c] = target.materialCount;
                target.materialCount += source.materials.component[c].count;
            }
            if (request.mode != VehicleWindowMode50DS::callerMaterialTextures) continue;
            target.materials.reset(new (std::nothrow) Material[target.materialCount]);
            if (!target.materials)
                return fail(*diagnostic, VehicleWindowError50DS::allocation,
                    "allocate bounded cold racer material table");
            for (U c = 0; c < target.resources.componentCount; ++c)
                for (U m = 0; m < source.materials.component[c].count; ++m) {
                    const auto& binding = source.materials.component[c].bindings[m];
                    auto& material = target.materials[target.materialOffsets[c] + m];
                    material.policy = binding;
                    target.materialSummary[c].sourceDiffuse |= binding.sourceDiffuseCompressed50LA;
                    target.materialSummary[c].sourcePair |= binding.sourceSpecularPair50LA;
                    target.materialSummary[c].normalDependent |= podMaterialUsesNormals(binding);
                    target.materialSummary[c].hasDeferredEffects |= binding.blend != VehicleWindowBlend50DS::opaque;
                    material.textureIndex = binding.textureIndex;
                    material.name = binding.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                        target.textures->texture(binding.textureIndex) : 0;
                    if (binding.sourceSpecularPair50LA) {
                        const auto* base = target.textures->asset(binding.textureIndex);
                        const auto* map2 = target.specularTextures50LA->asset(binding.specularTextureIndex50LA);
                        if (!base || !base->name || !map2 || !map2->name)
                            return fail(*diagnostic, VehicleWindowError50DS::material, "racer source pair disappeared during create");
                        material.textureIdentity50LA = base->identity;
                        material.specularName50LA = map2->name; material.specularIdentity50LA = map2->identity;
                        hasSpecularPairs50LA = true;
                    }
                    if (binding.mode == VehicleWindowMaterialMode50DS::requiredTexture && !material.name)
                        return fail(*diagnostic, VehicleWindowError50DS::material,
                            "required racer material texture disappeared during create");
                }
            opponentMaterialStride = std::max(opponentMaterialStride, target.materialCount);
        }
        if (opponentMaterialStride) {
            opponentMaterialUvs.reset(new (std::nothrow)
                MaterialUv[opponentMaterialStride * maxVehicleWindowOpponents50DS]);
            if (!opponentMaterialUvs)
                return fail(*diagnostic, VehicleWindowError50DS::allocation,
                    "allocate bounded per-racer UV transform storage");
        }
        diagnostic->component = maxComponents;
        return texturesCurrent(*diagnostic);
    }
    bool initializeOriginalFrontend(const VehicleWindowRequest50DS& input) noexcept {
        request=input;frontendOnly=true;
        maximumWidth=input.maximumWidth;maximumHeight=input.maximumHeight;
        if(!validateOriginalUiBank(input))return false;
        uiTexturePublication=input.originalUiTextures->publicationToken();
        if(!current(*diagnostic)||!texturesCurrent(*diagnostic))return false;
        if(!loadFunctions()||!check("initial frontend GL error state")||!checkContext())return false;
        if(!createOriginalUi()||!createTargets())return false;
        if(request.output==VehicleWindowOutput50DS::defaultBackBuffer)gl.bindFramebuffer(framebuffer,0);
        if(!check("finish dedicated original frontend creation"))return false;
        initialized=true;
        diagnostic->originalUiCoherentFramebufferFetch=uiSourceCoherentFramebufferFetch;
        return true;
    }
    bool initialize(const VehicleWindowRequest50DS& input,
        const assets::VehicleDrawAssets50CR& owner) noexcept {
        request = input;
        // Coverage is currently admitted only for static track spans. Never
        // silently consume a requested dynamic policy without binding it.
        for (U i=0;i<input.dynamicWorld.spanCount;++i)
            if(input.dynamicWorld.spans[i].nativeCoverage!=NativeTrackCoverage50DS::aa0)
                return fail(*diagnostic,VehicleWindowError50DS::material,"dynamic coverage is not admitted");
        resources.componentCount = owner.componentCount();
        if (input.enableOriginalUiSourceRgba && !input.enableOriginalUi)
            return fail(*diagnostic,VehicleWindowError50DS::request,"source UI requires original UI opt-in");
        if (!validateOriginalUiBank(input) || !validateGcParticleBank(input) || !validateRcParticleBank(input) || !validateGbParticleBank(input) || !validateMcParticleBank(input)) return false;
        uiTexturePublication = input.originalUiTextures ? input.originalUiTextures->publicationToken() : 0;
        request.racerArchetypes = nullptr; // Cold descriptors are never retained.
        request.trackAnimations = nullptr;
        // Track CPU bytes are borrowed only for this cold synchronous call.
        // No retained pointer, including after a failed cleanup, is needed.
        request.track.mesh = nullptr;
        request.track.spans = nullptr;
        request.track.sourceNormals = nullptr;
        request.track.sourceProjectionUvs = nullptr;
        request.dynamicWorld.mesh = nullptr;
        request.dynamicWorld.spans = nullptr;
        request.dynamicWorld.sourceNormals = nullptr;
        request.dynamicWorld.sourceProjectionUvs = nullptr;
        request.sky.mesh = nullptr;
        request.sky.spans = nullptr;
        // Mapping arrays are cold-only; retain just the 50EB owner and its
        // publication token, never pointers into caller mapping storage.
        for (auto& component : request.materials.component) component = {};
        texturePublication = input.materials.textures ? input.materials.textures->publicationToken() : 0;
        specularPublication50LA = input.materials.specularTextures50LA ? input.materials.specularTextures50LA->publicationToken() : 0;
        trackTexturePublication = input.track.textures ? input.track.textures->publicationToken() : 0;
        dynamicWorldTexturePublication = input.dynamicWorld.textures ? input.dynamicWorld.textures->publicationToken() : 0;
        trackAnimationTexturePublication = input.trackAnimationTextures ? input.trackAnimationTextures->publicationToken() : 0;
        cableTexturePublication = input.cables.textures ? input.cables.textures->publicationToken() : 0;
        gameplayBinderPublication=input.gameplayBinderTextures?input.gameplayBinderTextures->publicationToken():0;
        skyTexturePublication = input.sky.textures ? input.sky.textures->publicationToken() : 0;
        cableTextureName = input.cables.textures ? input.cables.textures->texture(input.cables.textureIndex) : 0;
        maximumWidth = request.maximumWidth; maximumHeight = request.maximumHeight;
        if (!current(*diagnostic) || !texturesCurrent(*diagnostic)) return false;
        if (input.cables.textures && !cableTextureName)
            return fail(*diagnostic, VehicleWindowError50DS::material, "required cable texture disappeared during create");
        for (U c = 0; c < resources.componentCount; ++c) {
            diagnostic->component = c;
            meshes[c] = owner.preparedMesh(components[c]); scenes[c] = owner.scene(components[c]);
            if (!meshes[c] || !scenes[c] || !meshes[c]->indices || !meshes[c]->spans ||
                !meshes[c]->vertexCount || !meshes[c]->indexCount || !meshes[c]->spanCount ||
                meshes[c]->vertexCount > game::maxRigidVertices49S ||
                meshes[c]->indexCount > assets::max_mesh_indices ||
                meshes[c]->spanCount > game::maxVehicleDrawSpans50CM ||
                std::uint64_t(meshes[c]->vertexCount) * sizeof(assets::Vertex48B) >
                    std::uint64_t(std::numeric_limits<std::intptr_t>::max()) ||
                std::uint64_t(meshes[c]->indexCount) * sizeof(U) >
                    std::uint64_t(std::numeric_limits<std::intptr_t>::max()))
                return fail(*diagnostic, VehicleWindowError50DS::assets, "invalid prepared 50CR mesh");
            for (U i = 0; i < meshes[c]->spanCount; ++i)
                if (meshes[c]->spans[i].material_index >= scenes[c]->materials.size())
                    return fail(*diagnostic, VehicleWindowError50DS::assets,
                        "original material index outside loaded scene");
        }
        diagnostic->component = maxComponents;
        if (!createMaterials(input.materials) || !createRacerArchetypes(input) ||
            !createTrackMaterials(input.track) || !createTrackAnimations(input) ||
            !createDynamicWorldMaterials(input.dynamicWorld) ||
            !createTrackVisibility(input.track) || !createSkyMaterials(input.sky)) return false;
        for (U body = 0; body < sourceBodyCount50LA; ++body)
            if (!createSpecularGroups50LA(0, body, sourceSpecularGeometry50LA[body])) return false;
        for (U i = 0; i < racerArchetypeCount; ++i)
            for (U body = 0; body < sourceBodyCount50LA; ++body)
                if (!createSpecularGroups50LA(i + 1, body, racerArchetypes[i].specularGeometry50LA[body])) return false;
        if (hasSpecularPairs50LA) {
            frameSpecularUvs50LA.reset(new (std::nothrow) std::array<float, 2>[
                (1 + maxVehicleWindowOpponents50DS) * sourceBodyCount50LA * game::maxRigidVertices49S]);
            specularWorkspace50LA.reset(new (std::nothrow) game::OriginalRacerSpecularWorkspace50LA);
            if (!frameSpecularUvs50LA || !specularWorkspace50LA)
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded current-camera specular workspace");
        }
        if(hasSingleDiffuse50LA){
            frameDiffuseFog50LA.reset(new(std::nothrow) float[(1+maxVehicleWindowOpponents50DS)*sourceBodyCount50LA*game::maxRigidVertices49S]);
            diffuseWorkspace50LA.reset(new(std::nothrow) game::original_racer_diffuse50LA::Workspace);
            if(!frameDiffuseFog50LA||!diffuseWorkspace50LA)return fail(*diagnostic,VehicleWindowError50DS::allocation,"allocate bounded single diffuse fog workspace");
        }
        coverageDemand = coverageDemand || bool(request.raStaticWorld);
        if (!loadFunctions() || !check("initial GL error state") || !checkContext()) return false;
        shader.reset(new (std::nothrow) VehicleGlShader50CZ(shaderGl));
        if (!shader) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate shader owner");
        if (!shader->initialize(diagnostic->shader, coverageDemand && hasSpecularPairs50LA))
            return fail(*diagnostic, VehicleWindowError50DS::shader, "initialize 50CZ viewer shader");
        if (!createTrackShader() || !createGeometry(resources, meshes) ||
            !createRacerNormals(resources, racerNormalBuffers)) return false;
        for (U body = 0; body < sourceBodyCount50LA; ++body)
            if (!createSpecularUvBuffers50LA(0, body, sourceSpecularGeometry50LA[body])) return false;
        for (U i = 0; i < racerArchetypeCount; ++i) {
            if (!createGeometry(racerArchetypes[i].resources, racerArchetypes[i].meshes) ||
                !createRacerNormals(racerArchetypes[i].resources, racerArchetypes[i].normalBuffers)) return false;
            for (U body = 0; body < sourceBodyCount50LA; ++body)
                if (!createSpecularUvBuffers50LA(i + 1, body, racerArchetypes[i].specularGeometry50LA[body])) return false;
        }
        if (
            !createStaticGeometry(input.track.mesh, trackVertexBuffer, trackIndexBuffer, trackVertexArray, trackIndexCount) ||
            !createWorldNormals(input.track, trackNormalBuffer, trackVertexArray) ||
            !createDynamicWorldGeometry(input.dynamicWorld) ||
            !createTrackProjection(input.track) ||
            !createStaticGeometry(input.sky.mesh, skyVertexBuffer, skyIndexBuffer, skyVertexArray, skyIndexCount) ||
            !createCableGeometry() || !createHostHud() || !createOriginalUi() ||
            !createTargets() || !createGameplayBlur() || !createCoverageProgram() || !createRaWorld()) return false;
        VehicleGlHooks50CV hooks{};
        hooks.context = this; hooks.ready = ready; hooks.materialReady = materialReady;
        if(request.enableGameplayDisplayWallTiming)hooks.timingObserver=observePlayerTiming;
        hooks.begin = begin; hooks.bindMaterial = bindMaterial; hooks.commit = commit; hooks.abort = abort;
        if (hasSpecularPairs50LA || hasSingleDiffuse50LA) hooks.drawSpan = drawSpan50LA;
        adapter.reset(new (std::nothrow) VehicleGlBackend50CV(drawGl, resources, hooks, published));
        if (!adapter) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate 50CV adapter");
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer)
            gl.bindFramebuffer(framebuffer, 0);
        if (!check("finish cold renderer creation")) return false;
        initialized = true;
        diagnostic->originalUiCoherentFramebufferFetch=uiSourceCoherentFramebufferFetch;
        return true;
    }
    // Only use a retained, preflighted batch and its own cold material bank.
    // The live world-space payload is the same one this transaction will draw.
    bool buildRacerPodShadows(const game::VehicleDrawBatch50CO& batch, U archetype,
        NativePodShadow50HM (&output)[3]) noexcept {
        const auto selected = racerView(archetype);
        for (U c = 0; c < 3; ++c) {
            const auto& part = batch.component[c];
            std::uint64_t opaque = 0;
            for (U span = 0; span < part.spanCount; ++span) {
                bool casts = true;
                if (request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
                    const auto& policy = selected.materials[selected.materialOffsets[c] +
                        part.spans[span].material_index].policy;
                    casts = policy.blend == VehicleWindowBlend50DS::opaque && policy.depthWrite;
                }
                if (casts) opaque |= std::uint64_t{1} << span;
            }
            if (!buildNativePodShadow50HM(part, opaque, output[c])) {
                diagnostic->component = c;
                return fail(*diagnostic, VehicleWindowError50DS::assets, "native pod shadow source geometry rejected");
            }
        }
        return true;
    }
    static bool ready(void* context, const VehicleGlResources50CV& resources, std::uint8_t bank,
        const game::VehicleDrawBatch50CO& batch, const game::VehicleDrawPlan50CS&) noexcept {
        auto& s = *static_cast<Storage*>(context);
        // Check exact current context/drawable before even querying GL errors.
        // Cold-verified private object storage remains unchanged by contract.
        if (!s.current(*s.diagnostic) || !s.texturesCurrent(*s.diagnostic)) return false;
        if (!s.initialized || s.active || !s.frameSet || bank != (s.published ^ 1u) ||
            !s.shader->program() || !s.targets[bank]) return false;
        if (s.coverageDemand && (!s.coverageLive.scene[bank] ||
            s.coverageLive.width != s.width || s.coverageLive.height != s.height))
            return fail(*s.diagnostic, VehicleWindowError50DS::frame, "exact native coverage extent is not prepared");
        if (!s.retainedMesh(batch, *s.diagnostic)) return false;
        if (s.trackIndexCount && (!s.trackVertexBuffer || !s.trackIndexBuffer || !s.trackVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "static track resources are not ready");
        for (U i = 0; i < s.trackAnimationCount; ++i) {
            const auto& animation = s.trackAnimations[i];
            const auto selected = animation.selectedFrame;
            if (selected >= animation.frameCount)
                return fail(*s.diagnostic, VehicleWindowError50DS::frame, "track animation frame is not prepared");
            const auto* image = s.request.trackAnimationTextures->asset(animation.firstTextureIndex+selected);
            if (!image || image->name != animation.names[selected])
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "selected original animation frame no longer matches retained publication");
        }
        if (s.dynamicWorldIndexCount && (!s.dynamicWorldIndexBuffer ||
            !s.dynamicWorldVertexBuffers[bank] || !s.dynamicWorldArrays[bank] ||
            (s.dynamicWorldNormalCount && !s.dynamicWorldNormalBuffers[bank])))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "dynamic-world resources are not ready");
        for (U i = 0; i < s.dynamicWorldSpanCount; ++i) {
            const auto& material = s.dynamicWorldSpans[i].material;
            if (!material.name) continue;
            const auto* texture = s.request.dynamicWorld.textures->asset(material.textureIndex);
            if (!texture || texture->name != material.name) {
                s.diagnostic->dynamicWorldSpan = i;
                s.diagnostic->texture = material.textureIndex;
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "required dynamic-world texture is no longer the retained publication");
            }
        }
        if (s.usesTrackShader() && !s.trackProgram)
            return fail(*s.diagnostic, VehicleWindowError50DS::shader, "source-color track program is not ready");
        if (s.frameHostHud && (!s.request.enableHostHud || !s.hostHudGeometry ||
            !s.hostHudProgram || !s.hostHudVertexBuffer || !s.hostHudIndexBuffer || !s.hostHudVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::frame, "host HUD resources are not ready");
        if (s.request.cables.textures) {
            const auto* texture = s.request.cables.textures->asset(s.request.cables.textureIndex);
            if (!s.frameCables || !s.cableVertexBuffer || !s.cableIndexBuffer || !s.cableVertexArray ||
                !texture || !texture->name || texture->name != s.cableTextureName)
                return fail(*s.diagnostic, VehicleWindowError50DS::material, "source cable resources are not ready");
        }
        for (U i = 0; i < s.trackSpanCount; ++i) {
            const auto& material = s.trackSpans[i].material;
            if (!material.name) continue; // Explicit source-color-only surface.
            const auto* texture = s.request.track.textures->asset(material.textureIndex);
            if (!texture || texture->name != material.name) {
                s.diagnostic->trackSpan = i;
                s.diagnostic->texture = material.textureIndex;
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "required track span texture is no longer the retained publication");
            }
        }
        if (s.skyIndexCount && (!s.skyVertexBuffer || !s.skyIndexBuffer || !s.skyVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "authored sky resources are not ready");
        for (U i = 0; i < s.skySpanCount; ++i) {
            const auto& material = s.skySpans[i].material;
            if (!material.name) continue;
            const auto* texture = s.request.sky.textures->asset(material.textureIndex);
            if (!texture || texture->name != material.name)
                return fail(*s.diagnostic, VehicleWindowError50DS::material,
                    "required sky texture is no longer the retained publication");
        }
        for (U c = 0; c < resources.componentCount; ++c) {
            const auto& mesh = *s.meshes[c];
            const auto& b = batch.component[c];
            if (b.indices != mesh.indices || b.indexCount != mesh.indexCount ||
                b.spans != mesh.spans || b.spanCount != mesh.spanCount || b.vertexCount != mesh.vertexCount ||
                resources.indices[c].buffer != s.resources.indices[c].buffer)
                return fail(*s.diagnostic, VehicleWindowError50DS::assets,
                    "batch is not the retained prepared mesh");
        }
        for (auto& racer : s.podShadows)
            for (auto& shadow : racer) shadow = {};
        if (s.request.enableNativePodShadows) {
            if (!s.buildRacerPodShadows(batch, 0, s.podShadows[0])) return false;
            for (U i = 0; i < s.frameOpponentCount; ++i) {
                s.diagnostic->opponentIndex = i;
                if (!s.buildRacerPodShadows(s.frameOpponentBatches[i], s.frameOpponentArchetypes[i],
                    s.podShadows[i + 1])) return false;
                if (!buildNativePodShadowReceiverBounds50HM(s.podShadows[i + 1], s.podShadowOpponentBounds[i]))
                    return fail(*s.diagnostic, VehicleWindowError50DS::assets,
                        "native opponent shadow receiver bounds rejected");
            }
            s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        }
        return s.check("before vehicle preflight");
    }
    static bool materialReady(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        auto& s = *static_cast<Storage*>(context);
        const U c = componentIndex(component);
        if (c >= s.resources.componentCount || material >= s.scenes[c]->materials.size())
            return fail(*s.diagnostic, VehicleWindowError50DS::assets,
                "material is outside loaded component scene");
        if (s.request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
            const auto& binding = s.materials[s.materialOffsets[c] + material];
            if (binding.name) {
                const auto* texture = s.request.materials.textures->asset(binding.textureIndex);
                if (!texture || texture->name != binding.name) {
                    s.diagnostic->component = c;
                    s.diagnostic->material = material;
                    s.diagnostic->texture = binding.textureIndex;
                    return fail(*s.diagnostic, VehicleWindowError50DS::material,
                        "required material texture is no longer the retained publication");
                }
            }
        }
        return true;
    }
    static I effectMode(const VehicleWindowMaterialBinding50DS& policy) noexcept {
        return policy.blend == VehicleWindowBlend50DS::opaque ? 0 : (policy.fixedBlendAlpha ? 2 : 1);
    }
    void configureBlend(const VehicleWindowMaterialBinding50DS& policy, bool layeredDepth = false) noexcept {
        gl.enable(0x0B71);
        gl.depthMask(policy.depthWrite ? 1 : 0);
        gl.depthFunc(layeredDepth || policy.depthEqual ? 0x0203 : 0x0201);
        if (policy.blend == VehicleWindowBlend50DS::opaque) gl.disable(0x0BE2);
        else {
            gl.enable(0x0BE2);
            gl.blendEquation(0x8006); // FUNC_ADD; never inherit caller equations.
            gl.blendFunc(policy.fixedBlendAlpha ? 1 : 0x0302,
                policy.blend == VehicleWindowBlend50DS::additive ? 1 : 0x0303);
        }
    }
    void unbindSpecularFeedback50LA() noexcept {
        if (!hasSpecularPairs50LA) return;
        gl.activeTexture(textureUnit0 + 1); gl.bindTexture(texture2D, 0);
        if (gl.bindSampler) gl.bindSampler(1, 0);
        gl.activeTexture(textureUnit0);
    }
    void resetNativeGcLighting() noexcept {
        coverageAa0();
        // This capability owns no gameplay shader or coefficients to clear.
        // The UI path immediately binds its own complete shader/state below.
        if(frontendOnly)return;
        // Both lighting selectors and coefficients are explicitly cleared at
        // renderer transitions, including cable/sky/track and an aborted racer
        // frame followed by UI. Source body MODULATE must never leak to them.
        gl.useProgram(shader->program());
        const auto& u = shader->uniforms();
        gl.uniform1i(u.useNativeGcSelectedLights, 0);
        gl.uniform1i(u.useSourceDiffuseCompressed50LA, 0);
        gl.uniform1i(u.sourcePairPass50LA, 0); gl.uniform1i(u.sourcePairFog50LA, 0);
        gl.uniform1i(u.sourcePairAlphaTest50LA, 0);
        gl.uniform1i(u.sourceSingleDiffuseRgba50LA,0);
        gl.uniform1i(u.sourceSingleFogEnabled50LA,0);
        gl.uniform1i(u.sourceSingleAlphaTest50LA,0);
        gl.uniform1i(u.sourcePairDestination50LA, hasSpecularPairs50LA ? 1 : 0);
        unbindSpecularFeedback50LA();
        gl.uniform4f(u.nativeGcAmbient, 0.f, 0.f, 0.f, 0.f);
        for (U i = 0; i < 3; ++i) {
            gl.uniform4f(u.nativeGcDiffuse[i], 0.f, 0.f, 0.f, 0.f);
            gl.uniform4f(u.nativeGcDirection[i], 0.f, 0.f, 0.f, 0.f);
        }
    }
    void resetPodState() noexcept {
        // Explicitly clear the shared world shader before any pod/sky/UI path.
        if (trackProgram) {
            gl.useProgram(trackProgram);
            gl.uniform1i(worldFogEnabledUniform, 0);
            gl.uniform1i(rcCrystalPassUniform,0);gl.uniform1i(rcCrystalPayloadUniform,0);
        }
        gl.disable(0x0BE2); gl.enable(0x0B71); gl.depthMask(1); gl.depthFunc(0x0201);
        gl.useProgram(shader->program());
        const auto& u = shader->uniforms();
        gl.uniformMatrix4fv(u.worldViewProjection, 1, 0, matrix);
        gl.uniform1i(u.useTexture, 0); gl.uniform1i(u.image, 0);
        gl.uniform1i(u.effectMode, 0); gl.uniform1f(u.alphaScale, 1.f);
        gl.uniform1i(u.suppressDraw, 0); gl.uniform1i(u.useUvTransform, 0);
        gl.uniform1i(u.useLighting, 0);
        resetNativeGcLighting();
    }
    bool drawSky() noexcept {
        if (!skyIndexCount || !frameSkyEnabled) return true;
        // Genuine source sky, separate caller camera transform, color only.
        // Nothing here scales geometry or manufactures missing faces.
        resetPodState();
        gl.disable(0x0B71); gl.depthMask(0);
        gl.uniformMatrix4fv(shader->uniforms().worldViewProjection, 1, 0, skyMatrix);
        gl.uniform1i(shader->uniforms().effectMode, 3); // Unlit source RGB, opaque.
        drawGl.bindVertexArray(skyVertexArray);
        drawGl.bindBuffer(elementBuffer, skyIndexBuffer);
        for (U i = 0; i < skySpanCount; ++i) {
            const auto& span = skySpans[i];
            gl.bindTexture(texture2D, span.material.name);
            gl.uniform1i(shader->uniforms().useTexture, span.material.name ? 1 : 0);
            if (!check("bind actual authored sky tile")) return false;
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.indexCount), 0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(span.indexStart) * sizeof(U)), 0);
            if (!check("draw actual authored sky tile")) return false;
            ++diagnostic->skyDraws; diagnostic->skyIndices += span.indexCount;
        }
        resetPodState();
        return check("restore world transform and depth after sky");
    }
    bool drawTrack(bool transparent) noexcept {
        auto& s = *this;
        resetNativeGcLighting();
        if (s.frameTrackDrawCount && (!transparent || s.trackSpanCount)) {
            // The entire draw is still unpublished and abortable. The following
            // vehicle draws retain this exact target, matrix, and written depth.
            if (s.request.track.mode == VehicleWindowMode50DS::sourceVertexColors ||
                s.request.track.mode == VehicleWindowMode50DS::callerMaterialTextures) {
                s.gl.useProgram(s.trackProgram);
                const auto& fog = s.request.track.worldFog;
                s.gl.uniform1i(s.worldFogEnabledUniform, 0);
                s.gl.uniform4f(s.worldFogCameraUniform, s.worldFogCamera[0], s.worldFogCamera[1], s.worldFogCamera[2], 0.f);
                s.gl.uniform4f(s.worldFogColorUniform, fog.color[0], fog.color[1], fog.color[2], 1.f);
                s.gl.uniform4f(s.worldFogRangeUniform, fog.start, fog.end, 0.f, 0.f);
                s.gl.uniformMatrix4fv(s.trackMatrixUniform, 1, 0, s.matrix);
                s.gl.uniform1f(s.trackColorScaleUniform, s.request.track.vertexColorScale);
                s.gl.uniform1i(s.trackUseTextureUniform, 0);
                s.gl.uniform1i(s.trackImageUniform, 0);
                s.gl.uniform1i(s.trackDiscardZeroAlphaUniform, 0);
                s.gl.uniform1i(s.trackEffectModeUniform, 0);
                s.gl.uniform1f(s.trackAlphaScaleUniform, 1.f);
                s.gl.uniform1i(s.trackUnlitUniform, 0);
                s.gl.uniform1i(s.trackUseSourceNormalsUniform, s.trackNormalBuffer ? 1 : 0);
                s.gl.uniform1i(s.trackProjectionPassUniform, 0);
                s.gl.uniform1i(s.rcCrystalPassUniform,0);s.gl.uniform1i(s.rcCrystalPayloadUniform,0);
                s.gl.uniform4f(s.rcCrystalAmbientUniform,s.rcCrystalAmbient[0],s.rcCrystalAmbient[1],s.rcCrystalAmbient[2],s.rcCrystalAmbient[3]);
                s.gl.uniform4f(s.trackUvTranslationUniform,0.f,0.f,0.f,0.f);
                s.gl.uniform1i(s.nativeShadowEnabledUniform,
                    s.request.enableNativePodShadows && !transparent ? 1 : 0);
                s.gl.uniform1i(s.nativeShadowOpponentCountUniform,
                    s.request.enableNativePodShadows && !transparent ? static_cast<I>(s.frameOpponentCount) : 0);
                // One upload for the opaque track pass. Inactive slots are
                // excluded by the explicit count, including after a roster
                // shrinks; alpha/unlit/depth policy still disables all shadows.
                if (s.request.enableNativePodShadows && !transparent) {
                    for (U racer = 0; racer <= s.frameOpponentCount; ++racer) {
                        for (U c = 0; c < 3; ++c) {
                            const U caster = 3 * racer + c;
                            const auto& center = s.podShadows[racer][c].centerAndReach;
                            const auto& shape = s.podShadows[racer][c].shapeAndOpacity;
                            s.gl.uniform4f(s.nativeShadowCenterUniforms[caster], center[0], center[1], center[2], center[3]);
                            s.gl.uniform4f(s.nativeShadowShapeUniforms[caster], shape[0], shape[1], shape[2], shape[3]);
                        }
                    }
                    for (U i = 0; i < s.frameOpponentCount; ++i) {
                        const auto& bounds = s.podShadowOpponentBounds[i];
                        s.gl.uniform4f(s.nativeShadowOpponentBoundsUniforms[i], bounds[0], bounds[1], bounds[2], bounds[3]);
                    }
                }
            }
            s.drawGl.bindVertexArray(s.trackVertexArray);
            s.drawGl.bindBuffer(elementBuffer, s.trackIndexBuffer);
            if (!s.check("bind static track geometry")) return false;
            if (s.trackSpanCount) {
                // Stable source order within each explicit pass. No distance
                // sorting, index rebasing or reconstructed global-sort claim.
                U boundSpan = assets::unresolvedMaterialTexture50EA;
                // Only this uninterrupted track pass owns these material bindings.
                // Do not infer state from pass setup, another program, or a prior frame.
                I trackIntegerValues[8]{};
                U trackTextureName = 0;
                std::uint8_t trackAlphaReference = 0;
                unsigned char trackAlphaScaleBytes[sizeof(float)]{};
                bool trackMaterialKnown = false;
                // Raster state belongs only to this uninterrupted track pass.
                // Opaque tuples canonicalize blend factors that are not applied.
                struct TrackRasterState {
                    bool depthWrite = false, blendEnabled = false;
                    U depthFunction = 0, sourceFactor = 0, destinationFactor = 0;
                    bool same(const TrackRasterState& other) const noexcept {
                        return depthWrite == other.depthWrite && blendEnabled == other.blendEnabled &&
                            depthFunction == other.depthFunction && sourceFactor == other.sourceFactor &&
                            destinationFactor == other.destinationFactor;
                    }
                };
                TrackRasterState trackRasterState{};
                bool trackRasterKnown = false;
                for (U n = 0; n < s.frameTrackDrawCount; ++n) {
                    const U draw = s.frameTrackCullingApplied ? s.trackVisibleDrawIds[n] : n;
                    const U i = s.trackChunkCount ? s.trackChunks[draw].sourceSpan : draw;
                    const auto& span = s.trackSpans[i];
                    // Layered materials stay adjacent to their first source pass.
                    // Their alpha layer is not postponed into a separate world pass.
                    const bool deferredAlpha = span.sourcePassCount == 1 &&
                        span.material.policy.blend != VehicleWindowBlend50DS::opaque;
                    if (deferredAlpha != transparent) continue;
                    s.diagnostic->trackSpan = i;
                    s.diagnostic->texture = span.material.textureIndex;
                    // Retained chunks of one span remain contiguous. Preserve
                    // its exact material policy without rebinding every chunk.
                    if (boundSpan != i) {
                        // The public UV callback has no GL-state purity contract.
                        // Emit every value on its span and invalidate for the next span,
                        // even though the current MA controller callback is CPU-only.
                        const bool reuseMaterial = trackMaterialKnown && !span.sourceUvAnimation;
                        const auto integerUniform = [&](U slot, I location, I value) {
                            if (!reuseMaterial || trackIntegerValues[slot] != value)
                                s.gl.uniform1i(location, value);
                            trackIntegerValues[slot] = value;
                        };
                        const U textureName = span.animation == assets::unresolvedMaterialTexture50EA ?
                            span.material.name : s.trackAnimations[span.animation].names[
                                s.trackAnimations[span.animation].selectedFrame];
                        if (!reuseMaterial || trackTextureName != textureName)
                            s.gl.bindTexture(texture2D, textureName);
                        integerUniform(0, s.trackUseTextureUniform, span.material.name ? 1 : 0);
                        integerUniform(1, s.trackDiscardZeroAlphaUniform, span.sourceAlphaTest ? 2 : (span.discardZeroAlpha ? 1 : 0));
                        if (!reuseMaterial || trackAlphaReference != span.sourceAlphaReference)
                            s.gl.uniform1f(s.trackAlphaReferenceUniform,static_cast<float>(span.sourceAlphaReference));
                        integerUniform(2, s.worldFogEnabledUniform,
                            s.request.track.worldFog.enabled && span.worldFogEligible ? 1 : 0);
                        // Establish the final authored depth rule once, before drawing.
                        // Coplanar later passes use LEQUAL; single passes retain policy.
                        const auto& rasterPolicy = span.material.policy;
                        const bool blendEnabled = !span.rcCrystalPass && rasterPolicy.blend != VehicleWindowBlend50DS::opaque;
                        const TrackRasterState nextRasterState{
                            rasterPolicy.depthWrite, blendEnabled,
                            (span.sourcePass > 1 || rasterPolicy.depthEqual) ? U{0x0203} : U{0x0201},
                            blendEnabled ? (rasterPolicy.fixedBlendAlpha ? U{1} : U{0x0302}) : U{0},
                            blendEnabled ? (rasterPolicy.blend == VehicleWindowBlend50DS::additive ?
                                U{1} : U{0x0303}) : U{0}};
                        // The UV callback below has no GL-state purity contract:
                        // force this span and leave the next span unknown.
                        if (span.sourceUvAnimation) trackRasterKnown = false;
                        if (!trackRasterKnown || !trackRasterState.same(nextRasterState)){
                            if(span.rcCrystalPass){s.gl.disable(0x0BE2);s.gl.depthMask(0);s.gl.depthFunc(0x0203);}
                            else s.configureBlend(span.material.policy, span.sourcePass > 1);
                        }
                        integerUniform(7,s.rcCrystalPassUniform,span.rcCrystalPass);
                        integerUniform(3, s.trackProjectionPassUniform, static_cast<I>(span.projectionPass));
                        float uOffset=0.f,vOffset=0.f;
                        if(span.sourceUvAnimation && (!s.frameTrackUv.service ||
                            !s.frameTrackUv.service(s.frameTrackUv.owner,i,uOffset,vOffset) ||
                            !std::isfinite(uOffset) || !std::isfinite(vOffset)))
                            return fail(*s.diagnostic,VehicleWindowError50DS::frame,
                                "source track UV controller rejected selected material service");
                        // Reuse only the exact zero tuple of consecutive unanimated spans.
                        if (!reuseMaterial)
                            s.gl.uniform4f(s.trackUvTranslationUniform,uOffset,vOffset,0.f,0.f);
                        integerUniform(4, s.trackEffectModeUniform, effectMode(span.material.policy));
                        // Byte identity preserves signed zero without changing the value.
                        if (!reuseMaterial || std::memcmp(trackAlphaScaleBytes,
                            &span.material.policy.alphaScale, sizeof(float)) != 0)
                            s.gl.uniform1f(s.trackAlphaScaleUniform, span.material.policy.alphaScale);
                        integerUniform(5, s.trackUnlitUniform,
                            span.material.policy.unlit || span.material.policy.sourcePrelit ? 1 : 0);
                        integerUniform(6, s.nativeShadowEnabledUniform, s.request.enableNativePodShadows &&
                            !transparent && span.allowPodShadow && span.sourcePass == 1 && span.material.policy.depthWrite && !span.material.policy.unlit ? 1 : 0);
                        if (!s.check("bind explicit source-ordered track material")) return false;
                        trackRasterState = nextRasterState;
                        trackRasterKnown = !span.sourceUvAnimation;
                        trackTextureName = textureName;
                        trackAlphaReference = span.sourceAlphaReference;
                        std::memcpy(trackAlphaScaleBytes, &span.material.policy.alphaScale, sizeof(float));
                        trackMaterialKnown = !span.sourceUvAnimation;
                        boundSpan = i;
                    }
                    // Coverage binding mutates neither depth nor blend. Keep it
                    // independent of the root-owned pass-local tuple cache.
                    s.bindTrackCoverage(span.nativeCoverage);
                    const U indexStart = s.trackChunkCount ? s.trackChunks[draw].indexStart : span.indexStart;
                    const U indexCount = s.trackChunkCount ? s.trackChunks[draw].indexCount : span.indexCount;
                    const auto offset = std::uintptr_t(indexStart) * sizeof(U);
                    if(span.rcCrystalPass){
                        if(!s.drawRcCrystalRange(s.activeFrameBank,indexStart,indexCount,span.rcCrystalPass,trackTextureName))return false;
                        // Bridge restores the exact source program/material/raster tuple.
                    }else s.drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(indexCount),
                        0x1405, reinterpret_cast<const void*>(offset), 0);
                    if (!s.check("draw actual track surface range")) return false;
                    if(indexCount)s.trackSpans[i].submittedGameplayCandidate=true;
                    ++s.diagnostic->trackDraws;
                    if (span.material.policy.blend != VehicleWindowBlend50DS::opaque) ++s.diagnostic->transparentTrackDraws;
                    if (span.material.name) ++s.diagnostic->trackTexturedDraws;
                    else ++s.diagnostic->trackUntexturedDraws;
                    s.diagnostic->trackIndices += indexCount;
                }
                s.diagnostic->trackSpan = s.diagnostic->texture = assets::unresolvedMaterialTexture50EA;
            } else {
                s.drawGl.drawElementsBaseVertex(0x0004, s.trackIndexCount, 0x1405, nullptr, 0);
                if (!s.check("draw world-space track before vehicle")) return false;
                s.diagnostic->trackDraws = s.diagnostic->trackUntexturedDraws = 1;
                s.diagnostic->trackIndices = static_cast<U>(s.trackIndexCount);
            }
        }
        resetPodState();
        return s.check("restore opaque vehicle state after track pass");
    }
    bool drawDynamicWorld(U bank, bool transparent) noexcept {
        if (!dynamicWorldSpanCount) return true;
        resetNativeGcLighting();
        gl.useProgram(trackProgram);
        gl.uniform1i(rcCrystalPassUniform,0);gl.uniform1i(rcCrystalPayloadUniform,0);
        gl.uniform1i(worldFogEnabledUniform, 0);
        gl.uniformMatrix4fv(trackMatrixUniform, 1, 0, matrix);
        gl.uniform1f(trackColorScaleUniform, request.dynamicWorld.vertexColorScale);
        gl.uniform1i(trackImageUniform, 0);
        gl.uniform1i(trackUseSourceNormalsUniform, dynamicWorldNormalCount ? 1 : 0);
        gl.uniform1i(trackProjectionPassUniform, 0);
        gl.uniform4f(trackUvTranslationUniform,0.f,0.f,0.f,0.f);
        // Static receiver/culling bounds are deliberately not reused for moving owners.
        gl.uniform1i(nativeShadowEnabledUniform, 0);
        gl.uniform1i(nativeShadowOpponentCountUniform, 0);
        drawGl.bindVertexArray(dynamicWorldArrays[bank]);
        drawGl.bindBuffer(elementBuffer, dynamicWorldIndexBuffer);
        // Local to this pass: the first submitted span always establishes state,
        // including texture zero. No call in the loop switches texture units,
        // binds/deletes textures, or changes the texture-use uniform elsewhere.
        U boundTexture = 0;
        bool textureBound = false;
        // Cache exactly configureBlend's inputs for this owned loop only.
        // No intervening call changes blend/depth state or mutates policies.
        const VehicleWindowMaterialBinding50DS* boundPolicy = nullptr;
        bool boundLayeredDepth = false;
        for (U i = 0; i < dynamicWorldSpanCount; ++i) {
            const auto& span = dynamicWorldSpans[i];
            const auto& material = span.material;
            const bool deferred = span.sourcePassCount == 1 &&
                material.policy.blend != VehicleWindowBlend50DS::opaque;
            if (deferred != transparent) continue;
            diagnostic->dynamicWorldSpan = i;
            diagnostic->texture = material.textureIndex;
            const auto& policy = material.policy;
            const bool layeredDepth = span.sourcePass > 1;
            if (!boundPolicy || boundPolicy->blend != policy.blend ||
                boundPolicy->depthWrite != policy.depthWrite ||
                boundPolicy->depthEqual != policy.depthEqual ||
                boundPolicy->fixedBlendAlpha != policy.fixedBlendAlpha ||
                boundLayeredDepth != layeredDepth) {
                configureBlend(policy, layeredDepth);
                boundPolicy = &policy;
                boundLayeredDepth = layeredDepth;
            }
            if (!textureBound || boundTexture != material.name) {
                gl.bindTexture(texture2D, material.name);
                gl.uniform1i(trackUseTextureUniform, material.name ? 1 : 0);
                boundTexture = material.name;
                textureBound = true;
            }
            gl.uniform1i(trackDiscardZeroAlphaUniform, span.sourceAlphaTest ? 2 : (span.discardZeroAlpha ? 1 : 0));
            gl.uniform1f(trackAlphaReferenceUniform,static_cast<float>(span.sourceAlphaReference));
            gl.uniform1i(trackEffectModeUniform, effectMode(material.policy));
            gl.uniform1f(trackAlphaScaleUniform, material.policy.alphaScale);
            gl.uniform1i(trackUnlitUniform,
                material.policy.unlit || material.policy.sourcePrelit ? 1 : 0);
            if (!check("bind authenticated dynamic-world material")) return false;
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.indexCount), 0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(span.indexStart) * sizeof(U)), 0);
            if (!check("draw current original dynamic-world source surface")) return false;
            ++diagnostic->dynamicWorldDraws;
            diagnostic->dynamicWorldIndices += span.indexCount;
        }
        diagnostic->dynamicWorldSpan = diagnostic->texture = assets::unresolvedMaterialTexture50EA;
        resetPodState();
        return check("restore pod state after dynamic-world pass");
    }
    bool applyPodMaterial(const Material& material, bool suppress, const float* overrideUv = nullptr,
        const game::original_racer_lighting50KM::LightMatrices* selectedLights = nullptr) noexcept {
        const bool sourceDiffuse = material.policy.sourceDiffuseCompressed50LA;
        if (sourceDiffuse && (!selectedLights || suppress || !material.name))
            return fail(*diagnostic, VehicleWindowError50DS::material,
                "source diffuse requires the authenticated unsuppressed textured body draw");
        configureBlend(material.policy);
        const auto& u = shader->uniforms();
        unbindSpecularFeedback50LA();
        gl.uniform1i(u.sourcePairPass50LA, 0); gl.uniform1i(u.sourcePairFog50LA, 0);
        gl.uniform1i(u.sourcePairAlphaTest50LA, 0);
        gl.uniform1i(u.sourceSingleDiffuseRgba50LA,0);
        gl.uniform1i(u.sourceSingleFogEnabled50LA,0);
        gl.uniform1i(u.sourceSingleAlphaTest50LA,0);
        gl.bindTexture(texture2D, material.name);
        gl.uniform1i(u.useTexture, material.name ? 1 : 0);
        gl.uniform1i(u.effectMode, effectMode(material.policy));
        gl.uniform1f(u.alphaScale, material.policy.alphaScale);
        gl.uniform1i(u.suppressDraw, suppress ? 1 : 0);
        gl.uniform1i(u.useSourceDiffuseCompressed50LA, sourceDiffuse ? 1 : 0);
        if(material.policy.sourceSingleDiffuseRgba50LA){
            gl.uniform1i(u.sourceSingleDiffuseRgba50LA,1);
            gl.uniform1i(u.sourceSingleFogEnabled50LA,material.policy.sourceBaseFog50LA?1:0);
            gl.uniform1i(u.sourceSingleAlphaTest50LA,material.policy.sourceBaseAlphaTest50LA?1:0);
            const auto color=frameDiffuseCamera50LA.fog.color30;
            // Source color30 is AARRGGBB; 13EB90 reverses it for GS FOGCOL.
            gl.uniform4f(u.sourceSingleFogColor50LA,float((color>>16)&255u),float((color>>8)&255u),float(color&255u),0.f);
        }
        // Retain source-alpha/additive effect appearance and explicit emissive
        // materials. Generated source body RGB has already been lit on CPU;
        // only the remaining opaque pod surfaces use host illumination.
        const bool hostLit = podMaterialUsesNormals(material.policy);
        gl.uniform1i(u.useLighting, hostLit ? 1 : 0);
        const bool nativeGc = selectedLights && hostLit && !suppress && material.name &&
            material.policy.mode == VehicleWindowMaterialMode50DS::requiredTexture;
        gl.uniform1i(u.useNativeGcSelectedLights, nativeGc ? 1 : 0);
        if (nativeGc) {
            // Source byte MODULATE neutral 128 is the explicit native RGB
            // coefficient convention. No exposure scalar or specular term.
            constexpr float coefficient = 1.f / 128.f;
            const auto& ambient = selectedLights->diffuse[3];
            gl.uniform4f(u.nativeGcAmbient, ambient[0] * coefficient,
                ambient[1] * coefficient, ambient[2] * coefficient, 0.f);
            for (U i = 0; i < 3; ++i) {
                const auto& diffuse = selectedLights->diffuse[i];
                gl.uniform4f(u.nativeGcDiffuse[i], diffuse[0] * coefficient,
                    diffuse[1] * coefficient, diffuse[2] * coefficient, 0.f);
                gl.uniform4f(u.nativeGcDirection[i], selectedLights->directions[0][i],
                    selectedLights->directions[1][i], selectedLights->directions[2][i], 0.f);
            }
        }
        gl.uniform1i(u.useUvTransform, 1);
        const auto* uv = overrideUv ? overrideUv : material.uv;
        gl.uniform4f(u.uvScaleOffset, uv[0], uv[1], uv[2], uv[3]);
        return check("bind explicit vehicle material blend and UV policy");
    }
    bool drawSpecularSpan50LA(std::uint8_t bank, U slot, U archetype, U component,
        U ordinal, const assets::SceneSpan48D& span) noexcept {
        const auto selected = racerView(archetype);
        const Material* material = selected.materials ?
            &selected.materials[selected.materialOffsets[component] + span.material_index] : nullptr;
        if(component>=1&&component<=sourceBodyCount50LA&&selected.specular[component-1].singleDiffuse&&
           !frameDiffuseSelected50LA[slot][component-1])return true;
        if (!material || !material->policy.sourceSpecularPair50LA) {
            VehicleGlTimingScope50CV timing(request.enableGameplayDisplayWallTiming&&slot==0&&archetype==0?
                observePlayerTiming:nullptr,this,VehicleGlTimingPhase50CV::unpairedSpan,component);
            drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
            return check("draw unchanged original unpaired source span");
        }
        if (!sourceBodyComponent50LA(component, selected.resources->componentCount))
            return fail(*diagnostic, VehicleWindowError50DS::assets,
                "body pair requires an actual engine in a three/five-model pod");
        const U bodySlot = component - 1;
        const auto& groups = selected.specular[bodySlot];
        if (!groups.enabled || ordinal >= game::maxVehicleDrawSpans50CM ||
            !groups.spanCount[ordinal] || !sourcePairColors50LA[bank] || !gl.textureBarrier)
            return fail(*diagnostic, VehicleWindowError50DS::assets, "body pair lost its exact original group resources");
        const auto& u = shader->uniforms();
        bool accepted = true;
        bool firstCoverageSetup = true;
        const U first = groups.spanFirst[ordinal], end = first + groups.spanCount[ordinal];
        const bool drawSecond = frameSpecularSecond50LA[slot][bodySlot];
        VehicleGlTimingScope50CV timing(request.enableGameplayDisplayWallTiming&&slot==0&&archetype==0?
            observePlayerTiming:nullptr,this,drawSecond?VehicleGlTimingPhase50CV::pairedSpan:
            VehicleGlTimingPhase50CV::baseOnlySpan,component);
        const bool sameFog = material->policy.sourceBaseFog50LA == material->policy.sourceSpecularFog50LA;
        // No draw in this owned span changes these states. Establish them
        // once; the first existing base-bind check still audits every call.
        gl.disable(0x0BE2); gl.enable(0x0B71); gl.depthFunc(0x0203);
        gl.colorMask(1, 1, 1, 1);
        // The shader consults this test only in pass 2. Its material policy
        // therefore stays invariant even while complete base groups draw.
        // Both callers bind this material first; applyPodMaterial has already
        // established zero and checked it. No intervening command writes this
        // uniform. Retain the unconditional reset below, including failures.
        if (material->policy.sourceSpecularAlphaTest50LA)
            gl.uniform1i(u.sourcePairAlphaTest50LA, 1);
        // Establish sampler 0 on unit 1 once. Group setup preserves it;
        // only single-sample later bases need to unbind feedback.
        // Coverage bridge tails retain their checked feedback unbind.
        unbindSpecularFeedback50LA();
        if (!drawSecond) {
            // The source distance gate removes every map-2 command in this
            // span. Cold group admission proved that these groups partition
            // this exact contiguous EBO range in order. With one unchanged
            // base state and no feedback reads, issue those same triangles as
            // one native draw. Keep source-group accounting, not host-call
            // counts, in the diagnostics. This never combines paired passes.
            gl.bindTexture(texture2D, material->name);
            gl.uniform1i(u.sourcePairPass50LA, 1);
            gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceBaseFog50LA ? 1 : 0);
            gl.depthMask(1);
            accepted = check("bind original distance-skipped source base span");
            if (accepted) {
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
                accepted = check("draw ordered distance-skipped source base span");
                if (accepted) {
                    diagnostic->sourceSpecularBaseGroups50LA += groups.spanCount[ordinal];
                    diagnostic->sourceSpecularSkippedGroups50LA += groups.spanCount[ordinal];
                }
            }
        }
        for (U g = first; drawSecond && accepted && g < end; ++g) {
            const auto& group = groups.groups[g];
            // Original single32CC28/multi32D670 issue the ENTIRE MSCNT group
            // base before single32CD18/multi32D750 issue that group's map 2.
            // Several source strips can share the same group. Keep them joined.
            // Map 2 changes these states, so restore them for every paired
            // group. On the source distance-skip path only base draws follow;
            // their texture/uniform/depth state and unbound feedback persist.
            if (drawSecond || g == first) {
                if (g != first && !coverageDemand) {
                    gl.activeTexture(textureUnit0 + 1); gl.bindTexture(texture2D, 0);
                    gl.activeTexture(textureUnit0);
                }
                gl.bindTexture(texture2D, material->name);
                gl.uniform1i(u.sourcePairPass50LA, 1);
                // Equal pass policies need one write per span. No command
                // inside the group loop otherwise changes this uniform.
                if (g == first || !sameFog)
                    gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceBaseFog50LA ? 1 : 0);
                gl.depthMask(1);
            }
            accepted = check("bind original source-pair base group");
            if (accepted) {
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(group.indexCount), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(group.indexStart) * sizeof(U)), 0);
                accepted = check("draw complete original source-pair base group");
                if (accepted) ++diagnostic->sourceSpecularBaseGroups50LA;
            }
            // Original distance130/1000 skip retains the complete base draw.
            if (!accepted) continue;
            if (!drawSecond) {
                ++diagnostic->sourceSpecularSkippedGroups50LA;
                continue;
            }
            gl.bindTexture(texture2D, material->specularName50LA);
            gl.uniform1i(u.sourcePairPass50LA, 2);
            if (!sameFog)
                gl.uniform1i(u.sourcePairFog50LA, material->policy.sourceSpecularFog50LA ? 1 : 0);
            gl.depthMask(0); // Same linked position shader/VAO and LEQUAL coverage.
            // Coverage entry and checked bridge tails retain unit 1 unbound
            // and unit 0 current, including groups with no visible triangles.
            if (!coverageDemand) {
                gl.activeTexture(textureUnit0 + 1);
                // The span's initial unbind selected sampler 0 on unit 1;
                // no intervening command changes sampler-object bindings.
                gl.bindTexture(texture2D, sourcePairColors50LA[bank]); gl.activeTexture(textureUnit0);
            }
            accepted = check("bind original source-pair second group");
            // The unchanged single-sample route reads one actual triangle's
            // own pixel once. A barrier before EACH draw observes overlapping
            // prior second triangles, including their independent source-A store.
            // Surviving map-2 samples were covered by the identical base group;
            // the optional GREATER0/KEEP test can only narrow that coverage.
            // Source sampling/clipping/depth remain the admitted native policy.
            if (accepted) {
                if (coverageDemand) {
                    // No callback or matrix/vertex mutation occurs inside
                    // this synchronous group. End the scratch scope on failure
                    // as well as success before another source owner can run.
                    beginRacerRasterOperandGroup(slot,component,group.indexStart,group.indexCount);
                    accepted = drawCoveragePairGroup(bank, slot, component,
                        group.indexStart, group.indexCount, material->specularName50LA, firstCoverageSetup);
                    endRacerRasterOperandGroup();
                } else {
                    for (U i = group.indexStart; i < group.indexStart + group.indexCount; i += 3) {
                        gl.textureBarrier();
                        drawGl.drawElementsBaseVertex(0x0004, 3, 0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(i) * sizeof(U)), 0);
                    }
                }
                // The AA1 route checks each scratch/copy/composition operation.
                // Cold source topology bounds one MSCNT group to 3..16
                // vertices, hence at most 14 ordered triangles. Poll errors
                // once for that bounded command batch, not once per triangle.
                // On the unchanged route no intervening call consumes GL errors; reject before the
                // next group and restore/unbind below, without publication.
                const bool commandsOk = check("draw ordered source map-2 group with current destination alpha");
                accepted = accepted && commandsOk;
                if (accepted) diagnostic->sourceSpecularSecondTriangles50LA += group.indexCount / 3;
            }
        }
        // Unbind the target even after failure, before any base/unrelated draw
        // or abort. Publication still belongs solely to the existing bank owner.
        unbindSpecularFeedback50LA();
        gl.bindTexture(texture2D, material->name);
        gl.uniform1i(u.sourcePairPass50LA, 0); gl.uniform1i(u.sourcePairFog50LA, 0);
        gl.uniform1i(u.sourcePairAlphaTest50LA, 0);
        gl.uniform1i(u.sourceSingleDiffuseRgba50LA,0);
        gl.uniform1i(u.sourceSingleFogEnabled50LA,0);
        gl.uniform1i(u.sourceSingleAlphaTest50LA,0); gl.depthMask(1);
        const bool restored = check("restore state after original body source pair");
        return accepted && restored;
    }
    static bool drawSpan50LA(void* context, game::VehicleDrawComponent50CM component,
        U ordinal, const assets::SceneSpan48D& span, I baseVertex) noexcept {
        auto& s = *static_cast<Storage*>(context);
        const U c = componentIndex(component);
        if (!s.active || c >= s.resources.componentCount || baseVertex)
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "invalid owned body-pair span draw");
        return s.drawSpecularSpan50LA(static_cast<std::uint8_t>(s.published ^ 1u), 0, 0, c, ordinal, span);
    }
    bool uploadRacer(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0, bool effectsOnly = false) noexcept {
        const auto selected = racerView(archetype);
        if (!uploadRacerNormals(bank, batch, archetype, effectsOnly)) return false;
        // Every batch was structurally preflighted and matched to these exact
        // cold buffers before begin. Only the inactive bank is ever reused.
        for (U c = 0; c < selected.resources->componentCount; ++c) {
            drawGl.bindBuffer(arrayBuffer, selected.resources->bank[bank].vertexBuffer[c]);
            // No deferred span consumes an all-opaque component. Upload the
            // complete current stream for every possible deferred consumer;
            // never assume submitted UV/color bytes equal the cold template.
            if (!effectsOnly || !selected.materials || selected.materialSummary[c].hasDeferredEffects)
                drawGl.bufferSubData(arrayBuffer, 0,
                    std::intptr_t(batch.component[c].vertexCount) * sizeof(assets::Vertex48B), batch.component[c].vertices);
            if (!check("upload independent world-space racer into inactive buffers")) return false;
        }
        return true;
    }
    bool drawOpaqueOpponent(std::uint8_t bank, U opponent) noexcept {
        resetPodState();
        const U archetype = frameOpponentArchetypes[opponent];
        const auto selected = racerView(archetype);
        if (!uploadRacer(bank, frameOpponentBatches[opponent], archetype) ||
            !uploadSpecularUvs50LA(bank, opponent + 1, archetype)) return false;
        for (U c = 0; c < selected.resources->componentCount; ++c) {
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            drawGl.bindBuffer(elementBuffer, selected.resources->indices[c].buffer);
            for (U i = 0; i < selected.meshes[c]->spanCount; ++i) {
                const auto& span = selected.meshes[c]->spans[i];
                if (selected.materials) {
                    const U material = selected.materialOffsets[c] + span.material_index;
                    if (selected.materials[material].policy.blend != VehicleWindowBlend50DS::opaque) continue;
                    if (!applyPodMaterial(selected.materials[material], false,
                        opponentMaterialUvs[opponent * opponentMaterialStride + material].value,
                        frameOpponentLights[opponent]->component(c))) return false;
                }
                if (!drawSpecularSpan50LA(bank, opponent + 1, archetype, c, i, span)) return false;
                ++diagnostic->opponentOpaqueDraws;
                diagnostic->opponentOpaqueIndices += span.index_count;
            }
        }
        ++diagnostic->opponentRacersDrawn;
        resetPodState();
        return check("restore state after opaque opponent");
    }
    bool drawTransparentPod(std::uint8_t bank, U opponent = maxVehicleWindowOpponents50DS) noexcept {
        const auto selected = racerView(opponent < maxVehicleWindowOpponents50DS ?
            frameOpponentArchetypes[opponent] : 0);
        if (!selected.materials) return true;
        resetPodState();
        const U slot=opponent<maxVehicleWindowOpponents50DS?opponent+1u:0u;
        for (U c = 0; c < selected.resources->componentCount; ++c) {
            // Source selected191 suppresses the complete model, including its
            // deferred flame/effect materials, not just the opaque body maps.
            if(c>0&&c<=sourceBodyCount50LA&&selected.specular[c-1].singleDiffuse&&
               !frameDiffuseSelected50LA[slot][c-1])continue;
            bool bound = false;
            for (U i = 0; i < selected.meshes[c]->spanCount; ++i) {
                const auto& span = selected.meshes[c]->spans[i];
                const auto& material = selected.materials[selected.materialOffsets[c] + span.material_index];
                if (material.policy.blend == VehicleWindowBlend50DS::opaque) continue;
                if (!bound) {
                    drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
                    drawGl.bindBuffer(elementBuffer, selected.resources->indices[c].buffer);
                    bound = true;
                }
                const float* uv = opponent < maxVehicleWindowOpponents50DS ?
                    opponentMaterialUvs[opponent * opponentMaterialStride +
                        selected.materialOffsets[c] + span.material_index].value : nullptr;
                if (!applyPodMaterial(material, false, uv)) return false;
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
                if (!check("draw deferred source pod effect span")) return false;
                ++diagnostic->transparentPodDraws;
                diagnostic->transparentPodIndices += span.index_count;
            }
        }
        resetPodState();
        return check("restore opaque state after deferred pod effects");
    }
    static bool begin(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        auto stageStart=s.rendererStageNow();
        s.active = true;s.activeFrameBank=bank;
        s.coveragePairRuns=s.coveragePairTriangles=s.coveragePairCoalescedTriangles=0;
        s.coveragePairEmptyTriangles=s.coveragePairFullSingletons=0;
        // Same-context GL ordering protects buffer/target reuse after prior
        // commands, including aborted work. No mapping or cross-context sharing.
        s.gl.bindFramebuffer(framebuffer, s.coverageDemand ? s.coverageLive.scene[bank] : s.targets[bank]);
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        s.gl.viewport(0, 0, s.width, s.height);
        // Solid, two-sided, opaque, LESS depth. This is diagnostic viewer state,
        // deliberately independent of original-game texture/material behavior.
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F};
        for (U capability : disabled) s.gl.disable(capability);
        for (I i = 0; i < s.clipDistances; ++i) s.gl.disable(0x3000 + static_cast<U>(i));
        s.gl.enable(0x0B71); s.gl.depthFunc(0x0201); s.gl.depthMask(1);
        s.gl.depthRange(0.0, 1.0); s.gl.clearDepth(1.0);
        s.gl.colorMask(1, 1, 1, 1); s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.useProgram(s.shader->program());
        const auto& uniforms = s.shader->uniforms();
        s.gl.uniformMatrix4fv(uniforms.worldViewProjection, 1, 0, s.matrix);
        s.gl.uniform1i(uniforms.useTexture, 0); s.gl.uniform1i(uniforms.image, 0);
        s.gl.uniform1i(uniforms.effectMode, 0); s.gl.uniform1f(uniforms.alphaScale, 1.f);
        s.gl.uniform1i(uniforms.suppressDraw, 0); s.gl.uniform1i(uniforms.useUvTransform, 0);
        s.gl.uniform1i(uniforms.useLighting, 0);
        s.resetNativeGcLighting();
        if (s.usesTextures()) {
            s.gl.activeTexture(textureUnit0);
            s.gl.bindTexture(texture2D, 0);
            // A sampler object would override the immutable 50EB parameters.
            // Select the texture object's already-authored sampling policy.
            if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        }
        s.gl.clearColor(s.clearRgba[0], s.clearRgba[1], s.clearRgba[2], s.clearRgba[3]);
        s.gl.clear(colorBufferBit | depthBufferBit);
        if (!s.check("begin explicit private frame")) return false;
        s.traceRaStage("target_setup",stageStart);
        if (!s.uploadRcCrystalUvs() || !s.uploadDynamicWorld(bank) || !s.drawSky())return false;
        s.traceRaStage("sky",stageStart);
        if(!s.drawRaWorld(bank))return false;
        s.traceRaStage("ra_world",stageStart);
        if(!s.drawTrack(false)||!s.drawDynamicWorld(bank,false))return false;
        s.traceRaStage("legacy_world",stageStart);
        const bool accepted=s.uploadRacerNormals(bank, s.framePlayerBatch) && s.uploadSpecularUvs50LA(bank, 0, 0);
        if(accepted){
            s.traceRaStage("player_stream_prepare",stageStart);
            s.rendererPlayerStageStart=s.rendererStageNow();
        }
        return accepted;
    }
    static bool bindMaterial(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        if (!materialReady(context, component, material)) return false;
        auto& s = *static_cast<Storage*>(context);
        if (s.request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
            const auto& binding = s.materials[s.materialOffsets[componentIndex(component)] + material];
            // 50CV keeps its exact original submission/provenance contract. A
            // selected effect issues a discard-only first-pass draw, then the
            // retained source range is rendered once at commit after opaques.
            return s.applyPodMaterial(binding, binding.policy.blend != VehicleWindowBlend50DS::opaque,
                nullptr, s.framePlayerLights->component(componentIndex(component)));
        }
        s.gl.uniform1i(s.shader->uniforms().useTexture, 0);
        s.gl.uniform1i(s.shader->uniforms().useNativeGcSelectedLights, 0);
        s.gl.uniform1i(s.shader->uniforms().useSourceDiffuseCompressed50LA, 0);
        return s.check("bind explicit neutral vehicle material policy");
    }
    struct PlayerTimingCounter {std::uint64_t calls=0,wallNs=0;};
    struct PlayerTiming {
        PlayerTimingCounter value[game::maxVehicleDrawComponents50CM][static_cast<unsigned>(VehicleGlTimingPhase50CV::count)]{};
        bool invalid=false;
    } playerTiming;
    static void observePlayerTiming(void* context,VehicleGlTimingPhase50CV phase,U component,
        std::uint64_t ns,bool valid) noexcept {
        auto& s=*static_cast<Storage*>(context);const auto p=static_cast<unsigned>(phase);
        if(!s.request.enableGameplayDisplayWallTiming)return;
        if(!valid||component>=game::maxVehicleDrawComponents50CM||p>=static_cast<unsigned>(VehicleGlTimingPhase50CV::count)){
            s.playerTiming.invalid=true;return;
        }
        auto& counter=s.playerTiming.value[component][p];
        if(counter.calls==std::numeric_limits<std::uint64_t>::max()||
           ns>std::numeric_limits<std::uint64_t>::max()-counter.wallNs){s.playerTiming.invalid=true;return;}
        ++counter.calls;counter.wallNs+=ns;
    }
    void reportPlayerTiming(bool accepted) const noexcept {
        if(!request.enableGameplayDisplayWallTiming)return;
        std::fprintf(stderr,"player_submit_timing accepted=%u invalid=%u components=%u order=upload,geometry_bind,material_bind,span_draw,unpaired,base_only,paired units=calls:wall_ns",
            accepted?1u:0u,playerTiming.invalid?1u:0u,resources.componentCount);
        for(U c=0;c<resources.componentCount&&c<game::maxVehicleDrawComponents50CM;++c){
            std::fprintf(stderr," c%u=",c);
            for(unsigned p=0;p<static_cast<unsigned>(VehicleGlTimingPhase50CV::count);++p){
                const auto& counter=playerTiming.value[c][p];
                std::fprintf(stderr,"%s%llu:%llu",p?",":"",static_cast<unsigned long long>(counter.calls),
                    static_cast<unsigned long long>(counter.wallNs));
            }
        }
        std::fprintf(stderr,"\n");
    }
    // Diagnostic-only stamps. Default gameplay takes no clock samples here.
    std::chrono::steady_clock::time_point rendererPlayerStageStart{};
    std::chrono::steady_clock::time_point rendererStageNow() const noexcept {
        return request.enableGameplayDisplayWallTiming?std::chrono::steady_clock::now():
            std::chrono::steady_clock::time_point{};
    }
    void traceRaStage(const char* stage,std::chrono::steady_clock::time_point& start,bool nested=true) noexcept {
        if(!request.enableGameplayDisplayWallTiming)return;
        const auto now=std::chrono::steady_clock::now();
        const auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(now-start).count();
        if(request.raStaticWorld)
            std::fprintf(stderr,"ra_renderer_stage name=%s cpu_submission_wall_us=%lld\n",stage,static_cast<long long>(elapsed));
        else
            std::fprintf(stderr,"renderer_stage name=%s nested_backend=%u submission_wall_us=%lld\n",
                stage,nested?1u:0u,static_cast<long long>(elapsed));
        // Do not charge this diagnostic write to the following sibling stage.
        start=std::chrono::steady_clock::now();
    }
    static bool commit(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        if (!s.active || bank != (s.published ^ 1u)) return false;
        s.traceRaStage("player_draw",s.rendererPlayerStageStart);
        auto stageStart=s.rendererStageNow();
        // One transaction and one clear/sky/track pass. Other actual opaque
        // racers join the player's pending framebuffer before ANY effects.
        for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (!s.drawOpaqueOpponent(bank, i)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.traceRaStage("opaque_opponents",stageStart);
        if(s.request.raStaticWorld&&s.request.enableGameplayDisplayWallTiming)
            std::fprintf(stderr,"ra_pair_batches scope=player_and_opaque_opponents runs=%llu triangles=%llu coalesced_triangles=%llu saved_bridges=%llu empty_triangles=%llu full_singletons=%llu\n",
                static_cast<unsigned long long>(s.coveragePairRuns),
                static_cast<unsigned long long>(s.coveragePairTriangles),
                static_cast<unsigned long long>(s.coveragePairCoalescedTriangles),
                static_cast<unsigned long long>(s.coveragePairTriangles-s.coveragePairRuns),
                static_cast<unsigned long long>(s.coveragePairEmptyTriangles),
                static_cast<unsigned long long>(s.coveragePairFullSingletons));
        // Cable/HUD failure still aborts the unpublished combined frame.
        s.resetPodState();
        if (!s.drawCables(s.frameCables, s.cableTextureName)) return false;
        for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (!s.drawCables(s.frameOpponentCables[i],
                s.racerView(s.frameOpponentArchetypes[i]).cableTextureName)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.traceRaStage("cables",stageStart);
        if (!s.drawTrack(true) || !s.drawDynamicWorld(bank, true)) return false;
        s.traceRaStage("transparent_world",stageStart);
        // The shared inactive VBOs currently hold the last opaque opponent.
        // Restore the player's independent stream before its deferred effects.
        if (s.frameOpponentCount && s.materials && !s.uploadRacer(bank, s.framePlayerBatch, 0, true)) return false;
        if (!s.drawTransparentPod(bank)) return false;
        if (s.materials) for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (!s.uploadRacer(bank, s.frameOpponentBatches[i], s.frameOpponentArchetypes[i], true) ||
                !s.drawTransparentPod(bank, i)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.resetPodState();
        s.traceRaStage("transparent_racers",stageStart);
        if (!s.drawGameplayBinders() || !s.drawGcParticles()) return false;
        s.traceRaStage("binders_particles",stageStart);
        if (s.hasSpecularPairs50LA || s.hasSingleDiffuse50LA || s.request.track.rcCrystal) {
            // All body/crystal Ad consumers have completed. Current Binder/GC
            // materials use source alpha and DATE0; the original HUD's private
            // scratch proves its own clear-before-Ad coverage. No later current
            // gameplay consumer needs these body/particle alpha bytes.
            s.unbindSpecularFeedback50LA();
            s.gl.disable(0x0C11); s.gl.colorMask(0, 0, 0, 1);
            s.gl.clearColor(0.f, 0.f, 0.f, 1.f); s.gl.clear(colorBufferBit);
            s.gl.colorMask(1, 1, 1, 1);
            if (!s.check("restore opaque presentation alpha after all body/crystal and particle consumers")) return false;
        }
        if (!s.resolveCoverage(bank)) return false;
        s.traceRaStage("coverage_resolve",stageStart);
        // 1AAE38/48/58/68 attach C8/CC/D0/C4 through141960 tail append;
        // 141D70 traverses firstChild18/next14. Preserve that HUD sibling order.
        // These explicit native joins do not claim recovered parent traversal.
        if (!s.drawHostHud() || !s.drawRaceHud(bank) || !s.drawDamage(bank) || !s.drawPositionStats(bank) || !s.drawTimeStats(bank) || !s.drawSpeedBoost(bank) || !s.drawGameplayBlur(bank) || !s.drawPauseDerived(bank)) return false;
        s.traceRaStage("hud_blur",stageStart);
        if (!s.check("accept private vehicle frame")) return false;
        // 50CV publication is CPU-only. The fallible window copy is deliberately
        // OUTSIDE this transaction, after submitVehicleDrawBatch50CS succeeds.
        s.retainedRaceDisplayCompleted=false;
        s.retainedRaceContainsPause=s.pauseDerived&&s.pauseDerived->frame;
        s.retainedRaceCameraPublication=s.pendingRaceCameraPublication;
        s.published = bank;
        s.committedWidth = s.width; s.committedHeight = s.height;
        s.raceWidth = s.width; s.raceHeight = s.height;
        s.displayedTarget = s.targets[bank];
        s.uiPrivatePublicationGeneration=0;
        s.uiSourcePublishedTargetGeneration=0;
        s.imageCommitted = true; s.active = false;
        return true;
    }
    static void abort(void* context, std::uint8_t) noexcept {
        // Previous private target remains untouched. Never blit or swap here.
        auto& s = *static_cast<Storage*>(context);
        s.unbindSpecularFeedback50LA();
        s.coverageAa0();
        s.active = false;
    }
    bool copyToWindow() noexcept { return copyToWindow(width,height); }
    bool copyToWindow(I copyWidth,I copyHeight) noexcept {
        // An unsuccessful or superseded copy must not authorize a later swap.
        if(request.enableOriginalGameplayBlur)gameplayAwaitingTarget=0;
        gl.bindFramebuffer(readFramebuffer, displayedTarget);
        gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer, 0);
        gl.drawBuffer(backBuffer);
        if (!check("prepare default back-buffer blit")) return false;
        // No atomicity claim for default framebuffer writes. On any later
        // error the host must not swap this potentially partial back buffer.
        diagnostic->defaultFramebufferMayBeModified = true;
        gl.blitFramebuffer(0, 0, copyWidth, copyHeight, 0, 0, copyWidth, copyHeight, colorBufferBit, 0x2600); // NEAREST
        if (!check("blit vehicle color to default back buffer")) return false;
        gl.bindFramebuffer(framebuffer, 0);
        const bool accepted=check("restore default framebuffer bindings");
        if(accepted)noteGameplayDisplayPending(displayedTarget,copyWidth,copyHeight);
        return accepted;
    }
    bool copyRgba(std::uint8_t* destination,U target=0,I readWidth=0,I readHeight=0) noexcept {
        if (!target){target=displayedTarget;readWidth=committedWidth;readHeight=committedHeight;}
        if (!check("before committed RGBA readback")) return false;
        gl.bindFramebuffer(readFramebuffer,target);
        gl.readBuffer(colorAttachment);
        // A caller pointer must never be interpreted as an offset into a PBO.
        // Explicit pack state keeps all writes inside the validated tight extent.
        drawGl.bindBuffer(pixelPackBuffer, 0);
        gl.pixelStorei(0x0D05, 1); // GL_PACK_ALIGNMENT
        gl.pixelStorei(0x0D02, 0); // GL_PACK_ROW_LENGTH
        gl.pixelStorei(0x0D03, 0); // GL_PACK_SKIP_ROWS
        gl.pixelStorei(0x0D04, 0); // GL_PACK_SKIP_PIXELS
        gl.pixelStorei(0x0D00, 0); // GL_PACK_SWAP_BYTES
        gl.pixelStorei(0x0D01, 0); // GL_PACK_LSB_FIRST
        gl.pixelStorei(0x806C, 0); // GL_PACK_IMAGE_HEIGHT
        gl.pixelStorei(0x806B, 0); // GL_PACK_SKIP_IMAGES
        if (!check("configure committed RGBA readback")) return false;
        const U status = gl.checkFramebufferStatus(readFramebuffer);
        if (!check("check committed readback framebuffer")) return false;
        if (status != framebufferComplete) {
            diagnostic->framebufferStatus = status;
            return fail(*diagnostic, VehicleWindowError50DS::readback,
                "committed readback framebuffer is incomplete");
        }
        diagnostic->readbackBufferMayBeModified = true;
        gl.readPixels(0,0,readWidth,readHeight,rgba,unsignedByte,destination);
        return check("read committed viewport RGBA bytes");
    }
    bool drainCleanupErrors(VehicleWindowDiagnostic50DS& d) noexcept {
        for (unsigned i = 0; i < 32; ++i) {
            const U error = drawGl.getError();
            if (!error) return true;
            nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = error;
        }
        return false;
    }
    bool deleteName(U& name, void (*deleter)(I, const U*), VehicleWindowDiagnostic50DS& d) noexcept {
        if (!name) return true;
        deleter(1, &name);
        const U error = drawGl.getError();
        if (!error) { name = 0; return true; }
        nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = error;
        // Retain the name conservatively until deletion is acknowledged. The
        // host may not create/reuse GL objects between unsuccessful close calls.
        return false;
    }
    bool deleteShaderName(U& name, bool program, VehicleWindowDiagnostic50DS& d) noexcept {
        if (!name) return true;
        if (program) shaderGl.deleteProgram(name);
        else shaderGl.deleteShader(name);
        const U error = drawGl.getError();
        if (!error) { name = 0; return true; }
        nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = error;
        return false;
    }
    bool cleanup(VehicleWindowDiagnostic50DS& d) noexcept {
        if(garagePreludeOwner50KG){d.cleanupIncomplete=true;
            return fail(d,VehicleWindowError50DS::cleanup,"Garage source-target prelude must retire its actual owner before renderer cleanup");}
        if (thread != std::this_thread::get_id()) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::wrongThread, "cleanup requires creating thread");
        }
        if(rcParticleWindow && (rcParticleWindow->material_->active || active || frameSet || diagnostic)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"RC renderer close requires ended synchronous draw/material borrowers");
        }
        if(!cancelRcParticleAttempt()){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"RC pending draw attempt could not retire before cleanup");
        }
        if(rcParticleWindow&&!rcParticleWindow->invalidateRenderPublication(rcParticleRenderer)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"RC prior particle completion could not invalidate");
        }
        if(gbParticleWindow && (gbParticleWindow->material_->active || active || frameSet || diagnostic)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"GB renderer close requires ended synchronous draw/material borrowers");
        }
        if(!cancelGbParticleAttempt()){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"GB pending draw attempt could not retire before cleanup");
        }
        if(gbParticleWindow&&!gbParticleWindow->invalidateRenderPublication(gbParticleRenderer)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"GB prior particle completion could not invalidate");
        }
        if(mcParticleWindow && (mcParticleWindow->material_->active || active || frameSet || diagnostic)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"MC renderer close requires ended synchronous draw/material borrowers");
        }
        if(!cancelMcParticleAttempt()){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"MC pending draw attempt could not retire before cleanup");
        }
        if(mcParticleWindow&&!mcParticleWindow->invalidateRenderPublication(mcParticleRenderer)){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"MC prior particle completion could not invalidate");
        }
        if (hasGlResources() && !current(d)) { d.cleanupIncomplete = true; return false; }
        if(raStaticFireOwner)raStaticFireOwner->nativeCompletionLive_=false;
        initialized = false; active = frameSet = imageCommitted = false;
        frameCables = nullptr; frameHostHud = false;frameGcParticles=nullptr;
        committedWidth = committedHeight = raceWidth = raceHeight = 0;
        retainedRaceDisplayCompleted=retainedRaceContainsPause=false;
        pendingRaceCameraPublication=retainedRaceCameraPublication=0;
        displayedTarget = 0;
        trackVisibilityPendingTarget=0;trackVisibilityCompletedValid=false;
        gameplayHistoryValid=gameplayHistoryStaged=frameGameplayBlurEnabled=false;
        gameplayAwaitingTarget=0;frameGameplayBlur={};
        game::clearGameplayBlurVideoStrengths(gameplayBlurVideo);
        uiPrivatePublicationGeneration=0;
        adapter.reset();
        if (!hasGlResources()) { shader.reset(); retireRaWorldPublication(); retireRaStaticFirePublication(); retireRcParticlePublication();retireGbParticlePublication();retireMcParticlePublication(); d.cleanupIncomplete = false; return true; }
        // All functions were loaded before the first name could be allocated.
        if (!drainCleanupErrors(d)) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "GL error state will not clear for cleanup");
        }
        // A failed nonblocking Garage observation can retain its one native
        // fence. Query actual ownership before deletion, including a prior
        // delete whose diagnostic outcome was uncertain. No replayed draw.
        if(garagePollFence50KG){
            if(!gl.isSync||!gl.deleteSync){d.cleanupIncomplete=true;
                return fail(d,VehicleWindowError50DS::cleanup,"Garage poll fence lacks its exact GL release owner");}
            const auto alive=gl.isSync(garagePollFence50KG);
            U error=drawGl.getError();
            if(error){d.cleanupIncomplete=true;d.cleanupGlError=error;return false;}
            if(alive){gl.deleteSync(garagePollFence50KG);error=drawGl.getError();
                if(error){d.cleanupIncomplete=true;d.cleanupGlError=error;return false;}}
            garagePollFence50KG=nullptr;
        }
        // Explicit source/display leases add a real completion obligation. Never
        // confuse a CPU generation increment or deferred GL deletion with it.
        if (riderTextureBorrow || raWorld || raStaticFireOwner || rcParticleWindow || gbParticleWindow || mcParticleWindow || hasDisplayFadeNames() || uiCoverage.owns() || uiSourceLeaseWork || uiRetainedTarget || uiRetainedColor || uiBlendOldSnapshot ||
            uiPromotedIncomingTarget || uiPromotedIncomingColor || hasGameplayBlurResources() || (hasRaceHudResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources())){
            gl.finish();const U error=drawGl.getError();
            if (error){nativeGlFaultObserved50KG=true;if(!d.cleanupGlError)d.cleanupGlError=error;d.cleanupIncomplete=true;
                return fail(d,VehicleWindowError50DS::cleanup,"complete source leases before cleanup");}
            uiSourceLeaseWork=false;
        }
        uiSourceReceiptLifetime.reset();uiSnapshotGeneration=0;
        uiBlendGeneration=uiBlendSampleGeneration=0;
        uiPromotedIncomingGeneration=0;
        gl.useProgram(0); drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
        gl.bindFramebuffer(framebuffer, 0); gl.bindRenderbuffer(renderbuffer, 0);
        if(uiCoverage.owns()){unbindUiCoverageInputs();gl.disable(0x809D);}
        if (coverageDemand) {
            coverageAa0();
            gl.activeTexture(textureUnit0+2);gl.bindTexture(coverageTexture,0);
            if(gl.bindSampler)gl.bindSampler(2,0);
            gl.activeTexture(textureUnit0);gl.bindTexture(coverageTexture,0);
        }
        if (usesTextures() || hasDisplayFadeNames()) {
            if (request.enableOriginalUiSourceRgba || request.enableOriginalGameplayBlur || (hasRaceHudResources() || hasTimeStatsResources() || hasPositionStatsResources() || hasDamageResources() || hasPauseDerivedResources() || hasSpeedBoostResources()) || hasSpecularPairs50LA || raWorld) {
                gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
                if (gl.bindSampler)gl.bindSampler(1,0);
            }
            gl.activeTexture(textureUnit0); gl.bindTexture(texture2D, 0);
            if (gl.bindSampler) gl.bindSampler(0, 0);
        }
        const U unbindError = drawGl.getError();
        if (unbindError) {
            nativeGlFaultObserved50KG=true;
            if (!d.cleanupGlError) d.cleanupGlError = unbindError;
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "unbind renderer resources before cleanup");
        }
        bool complete = true;
        if(!deleteSpeedBoostNames(d))goto incomplete;
        if(!deletePauseDerivedNames(d))goto incomplete;
        if(!deleteDamageNames(d))goto incomplete;
        if(!deletePositionStatsNames(d))goto incomplete;
        if(!deleteTimeStatsNames(d))goto incomplete;
        if(!deleteRaceHudNames(d))goto incomplete;
        if(!deleteGameplayBlurNames(d))goto incomplete;
        if (shader && !shader->release(d.cleanupShader)) {
            if (!d.cleanupGlError) d.cleanupGlError = d.cleanupShader.glError ?
                d.cleanupShader.glError : d.cleanupShader.cleanupGlError;
            complete = false;
        }
        // Only proceed after an acknowledged clean error state. Stop at a
        // failing deletion instead of conflating it with subsequent objects.
        if (!drainCleanupErrors(d)) complete = false;
        else {
            if(!deleteDisplayFadeNames(d))goto incomplete;
            if (!deleteUiByteBlendNames(d) || !deleteUiCoverage(d))goto incomplete;
            garageSamplersReady50KG=false;
            for(auto& name:garageSamplers50KG)if(!deleteName(name,gl.deleteSamplers,d))goto incomplete;
            uiBlendOldSnapshot=uiBlendIncomingSource=0;
            if (!deleteName(uiPromotedIncomingTarget,gl.deleteFramebuffers,d) ||
                !deleteName(uiPromotedIncomingColor,gl.deleteTextures,d))goto incomplete;
            if (!deleteShaderName(uiProgram, true, d) ||
                !deleteShaderName(uiVertexShader, false, d) ||
                !deleteShaderName(uiFragmentShader, false, d) ||
                !deleteName(uiVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(uiVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiParticleFogBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiSourceRasterWBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiSourcePacketStqBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiSourceIndexBuffer, gl.deleteBuffers, d) ||
                !deleteName(uiIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(uiTargets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(uiColors[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(uiDepths[b], gl.deleteRenderbuffers, d)) goto incomplete;
            if (!deleteName(uiRetainedTarget,gl.deleteFramebuffers,d) ||
                !deleteName(uiRetainedColor,gl.deleteTextures,d))goto incomplete;
            if (!deleteName(uiSourceTarget,gl.deleteFramebuffers,d) ||
                !deleteName(uiSourceColor,gl.deleteTextures,d) ||
                !deleteName(uiSourceDepth,gl.deleteRenderbuffers,d))goto incomplete;
            if (!deleteShaderName(trackProgram, true, d) ||
                !deleteShaderName(trackVertexShader, false, d) ||
                !deleteShaderName(trackFragmentShader, false, d)) goto incomplete;
            if (!deleteShaderName(hostHudProgram, true, d) ||
                !deleteShaderName(hostHudVertexShader, false, d) ||
                !deleteShaderName(hostHudFragmentShader, false, d)) goto incomplete;
            for (auto& bank : resources.bank) {
                for (U c = 0; c < maxComponents; ++c) {
                    if (!deleteName(bank.vertexArray[c], gl.deleteVertexArrays, d) ||
                        !deleteName(bank.vertexBuffer[c], gl.deleteBuffers, d)) goto incomplete;
                }
            }
            for (auto& body : sourceSpecularGeometry50LA) {
                for (auto& buffer : body.buffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
                for (auto& buffer : body.fogBuffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
            }
            for (auto& bank : racerNormalBuffers)
                for (auto& normal : bank)
                    if (!deleteName(normal, gl.deleteBuffers, d)) goto incomplete;
            for (auto& index : resources.indices)
                if (!deleteName(index.buffer, gl.deleteBuffers, d)) goto incomplete;
            for (auto& archetype : racerArchetypes) {
                for (auto& body : archetype.specularGeometry50LA) {
                    for (auto& buffer : body.buffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
                    for (auto& buffer : body.fogBuffers)if (!deleteName(buffer, gl.deleteBuffers, d)) goto incomplete;
                }
                for (auto& bank : archetype.normalBuffers)
                    for (auto& normal : bank)
                        if (!deleteName(normal, gl.deleteBuffers, d)) goto incomplete;
                for (auto& bank : archetype.resources.bank)
                    for (U c = 0; c < maxComponents; ++c)
                        if (!deleteName(bank.vertexArray[c], gl.deleteVertexArrays, d) ||
                            !deleteName(bank.vertexBuffer[c], gl.deleteBuffers, d)) goto incomplete;
                for (auto& index : archetype.resources.indices)
                    if (!deleteName(index.buffer, gl.deleteBuffers, d)) goto incomplete;
            }
            if (!deleteName(cableVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(cableVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(cableIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(hostHudVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(hostHudVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(hostHudIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(trackVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(trackVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(trackNormalBuffer, gl.deleteBuffers, d) ||
                !deleteName(trackProjectionBuffer, gl.deleteBuffers, d) ||
                !deleteName(trackIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(dynamicWorldArrays[b], gl.deleteVertexArrays, d) ||
                    !deleteName(dynamicWorldVertexBuffers[b], gl.deleteBuffers, d) ||
                    !deleteName(dynamicWorldNormalBuffers[b], gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(dynamicWorldIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(skyVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(skyVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(skyIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteRaWorldResources(d) || !deleteCoverageResources(d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(targets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(colors[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(sourcePairColors50LA[b], gl.deleteTextures, d) ||
                    !deleteName(depths[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(menuTargets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(menuColors[b], gl.deleteRenderbuffers, d)) goto incomplete;
        }
        d.cleanupIncomplete = hasGlResources();
        if(!d.cleanupIncomplete){retireRaStaticFirePublication();retireRaWorldPublication();retireRcParticlePublication();retireGbParticlePublication();retireMcParticlePublication();}
        if (d.cleanupIncomplete || !complete)
            return fail(d, VehicleWindowError50DS::cleanup, "renderer cleanup incomplete");
        shader.reset();
        if (d.cleanupGlError)
            return fail(d, VehicleWindowError50DS::cleanup, "cleanup completed after GL errors");
        return true;
    incomplete:
        d.cleanupIncomplete = true;
        return fail(d, VehicleWindowError50DS::cleanup, "GL resource deletion failed");
    }
};

VehicleWindowGl50DS::VehicleWindowGl50DS() noexcept = default;
VehicleWindowGl50DS::~VehicleWindowGl50DS() noexcept {
    VehicleWindowDiagnostic50DS d{};
    close(d);
    if (d.cleanupIncomplete) std::terminate();
}

bool VehicleWindowGl50DS::create(const VehicleWindowRequest50DS& request,
    const assets::VehicleDrawAssets50CR& assets, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if(sessionDisplayFrontendCreate50KG_||(sessionDisplayOwner50LA_&&!sessionDisplayCreate50LA_))
        return fail(d,VehicleWindowError50DS::request,"session renderer creation requires its actual display owner");
    if(request.gameplayBinderTextures){
        // Binder uses the shared cable quad buffers and the requested
        // world material shader. RA has its own authenticated world geometry,
        // so it must not supply a fabricated legacy track mesh here.
        if(!request.cables.textures||(!request.raStaticWorld&&
            (request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures||!request.track.mesh))||
            request.gameplayBinderTextures->textureCount()!=10||
            !request.gameplayBinderTextures->validBorrow(request.context,request.isCurrent,
                request.gameplayBinderTextures->publicationToken()))
            return fail(d,VehicleWindowError50DS::request,"Binder needs exact cold bank, world material program and shared geometry storage");
        for(U i=0;i<10;++i)if(!request.gameplayBinderTextures->texture(i))
            return fail(d,VehicleWindowError50DS::material,"Binder stored frame missing");
    }

    if (storage_) return fail(d, VehicleWindowError50DS::alreadyOwned, "renderer already owns resources");
    if (!request.procAddress || !request.isCurrent ||
        (request.mode != VehicleWindowMode50DS::viewerNeutralUntextured &&
            request.mode != VehicleWindowMode50DS::callerMaterialTextures) ||
        (request.output != VehicleWindowOutput50DS::defaultBackBuffer &&
            request.output != VehicleWindowOutput50DS::headless) ||
        request.maximumWidth <= 0 || request.maximumHeight <= 0 ||
        std::uint64_t(request.maximumWidth) * std::uint64_t(request.maximumHeight) >
            std::numeric_limits<std::size_t>::max() /
                (16u + (request.enableHostHud ? 8u : 0u) + (request.enableOriginalUi ? 16u : 0u) +
                 (request.enableOriginalGameplayBlur ? 12u : 0u)))
        return fail(d, VehicleWindowError50DS::request,
            "context callbacks, valid output/vehicle shading modes and positive target bounds required");
    if (!assets.loaded()) return fail(d, VehicleWindowError50DS::assets, "50CR assets are not loaded");
    if (!validateTrack(request, d) || !validateMaterials(request, assets, d) ||
        !validateCables(request, d) || !validateSky(request, d) || !validateDynamicWorld(request, d) ||
        !validateTrackAnimations(request, d)) return false;
    if(request.track.rcParticleWindow && (request.raStaticWorld || request.track.raStaticFire || request.track.gbParticleWindow || request.track.mcParticleWindow ||
        request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
        !request.track.rcParticleWindow->validateTrack(request.track)))
        return fail(d,VehicleWindowError50DS::material,"RC particle completion requires its exact actual world/image/span owner");
    if(request.track.gbParticleWindow && (request.track.mcParticleWindow || request.raStaticWorld || request.track.raStaticFire || request.track.rcParticleWindow || request.track.rcCrystal || request.track.tbWorld ||
        request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
        !request.track.gbParticleWindow->validateTrack(request.track)))
        return fail(d,VehicleWindowError50DS::material,"GB particle completion requires its exact actual world/image/span owner");
    if(request.track.mcParticleWindow && (request.track.gbParticleWindow || request.raStaticWorld || request.track.raStaticFire || request.track.rcParticleWindow || request.track.rcCrystal || request.track.tbWorld ||
        request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
        !request.track.mcParticleWindow->validateTrack(request.track)))
        return fail(d,VehicleWindowError50DS::material,"MC particle completion requires its exact actual world/image/span owner");
    if (request.racerArchetypeCount > maxVehicleWindowRacerArchetypes50DS ||
        bool(request.racerArchetypeCount) != bool(request.racerArchetypes))
        return fail(d, VehicleWindowError50DS::request, "racer archetypes require an explicit bounded cold array");
    for (U i = 0; i < request.racerArchetypeCount; ++i) {
        const auto& archetype = request.racerArchetypes[i];
        if (!archetype.assets || !archetype.assets->loaded())
            return fail(d, VehicleWindowError50DS::assets, "racer archetype assets are not loaded");
        auto selected = request;
        selected.materials = archetype.materials;
        selected.cables = archetype.cables;
        if (!validateMaterials(selected, *archetype.assets, d) || !validateCables(selected, d)) return false;
    }
    std::unique_ptr<Storage> next(new (std::nothrow) Storage);
    if (!next) return fail(d, VehicleWindowError50DS::allocation, "allocate caller-context renderer owner");
    if(request.raStaticWorld){
        if(request.track.mesh || request.track.raStaticFire || !request.raStaticWorldTextures ||
           !request.raStaticWorld->world_ || request.raStaticWorld->terminal_ || request.raStaticWorld->closed_ ||
           request.raStaticWorldTextures->raStaticWorldOwner()!=request.raStaticWorld->world_.get() ||
           !request.raStaticWorld->world_->publishRenderer(this,request.raStaticWorldTextures))
            return fail(d,VehicleWindowError50DS::material,"actual complete RA aggregate/image publication required without legacy track");
        next->raWorld=request.raStaticWorld->world_;next->raWorldRenderer=this;
        next->raWorldImagePublication=request.raStaticWorldTextures->publicationToken();
    }else if(request.raStaticWorldTextures)
        return fail(d,VehicleWindowError50DS::material,"RA image bank requires actual aggregate owner");
    if(request.track.raStaticFire){
        if(!request.track.raStaticFire->publishRenderer(this,request.track.textures))
            return fail(d,VehicleWindowError50DS::material,"FireP0t renderer publication already leased");
        next->raStaticFireOwner=request.track.raStaticFire;next->raStaticFireRenderer=this;
        next->raStaticFirePublicationSerial=request.track.raStaticFire->rendererPublicationSerial_;
    }
    if(request.track.rcParticleWindow){
        if(!request.track.rcParticleWindow->attachRenderer(this))
            return fail(d,VehicleWindowError50DS::material,"RC particle Window already belongs to a renderer");
        next->rcParticleWindow=request.track.rcParticleWindow;next->rcParticleRenderer=this;
        next->rcParticleAttempt=request.track.rcParticleWindow->lastAttempt_;
    }
    if(request.track.gbParticleWindow){
        if(!request.track.gbParticleWindow->attachRenderer(this))
            return fail(d,VehicleWindowError50DS::material,"GB particle Window already belongs to a renderer");
        next->gbParticleWindow=request.track.gbParticleWindow;next->gbParticleRenderer=this;
        next->gbParticleAttempt=request.track.gbParticleWindow->lastAttempt_;
    }
    if(request.track.mcParticleWindow){
        if(!request.track.mcParticleWindow->attachRenderer(this))
            return fail(d,VehicleWindowError50DS::material,"MC particle Window already belongs to a renderer");
        next->mcParticleWindow=request.track.mcParticleWindow;next->mcParticleRenderer=this;
        next->mcParticleAttempt=request.track.mcParticleWindow->lastAttempt_;
    }
    next->diagnostic = &d;
    const bool result = next->initialize(request, assets);
    next->diagnostic = nullptr;
    if (!result) {
        next->cleanup(d);
        if (d.cleanupIncomplete) storage_ = std::move(next);
        return false;
    }
    storage_ = std::move(next);
    return true;
}

bool VehicleWindowGl50DS::createOriginalFrontend(const OriginalFrontendWindowRequest50LA& input,
    VehicleWindowDiagnostic50DS& d) noexcept {
    if(sessionDisplayCreate50LA_||(sessionDisplayOwner50LA_&&!sessionDisplayFrontendCreate50KG_)){
        d={};return fail(d,VehicleWindowError50DS::request,
            "frontend renderer creation requires its actual frontend display scope");}
    d={};
    if(storage_)return fail(d,VehicleWindowError50DS::alreadyOwned,"renderer already owns resources");
    if(!input.procAddress||!input.isCurrent||!input.cpuTextures||!input.textures||
        (input.output!=VehicleWindowOutput50DS::defaultBackBuffer&&input.output!=VehicleWindowOutput50DS::headless)||
        input.maximumWidth<=0||input.maximumHeight<=0||
        std::uint64_t(input.maximumWidth)*std::uint64_t(input.maximumHeight)>
            std::numeric_limits<std::size_t>::max()/16u)
        return fail(d,VehicleWindowError50DS::request,"frontend requires exact context, authenticated UI bank and bounded output");
    VehicleWindowRequest50DS request;
    request.context=input.context;request.procAddress=input.procAddress;request.isCurrent=input.isCurrent;
    request.maximumWidth=input.maximumWidth;request.maximumHeight=input.maximumHeight;request.output=input.output;
    request.enableOriginalUi=true;request.enableOriginalUiSourceRgba=input.enableSourceRgba;
    request.originalUiCpuTextures=input.cpuTextures;request.originalUiTextures=input.textures;
    std::unique_ptr<Storage> next(new(std::nothrow) Storage);
    if(!next)return fail(d,VehicleWindowError50DS::allocation,"allocate dedicated frontend renderer owner");
    next->diagnostic=&d;
    const bool result=next->initializeOriginalFrontend(request);
    next->diagnostic=nullptr;
    if(!result){
        next->cleanup(d);
        if(d.cleanupIncomplete)storage_=std::move(next);
        return false;
    }
    storage_=std::move(next);return true;
}

bool VehicleWindowGl50DS::updateWorldFog(const VehicleWindowWorldFog50DS& fog,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"world-fog update requires initialized renderer");
    auto& s=*storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"world-fog update requires idle renderer");
    if(!std::isfinite(fog.start) || !std::isfinite(fog.end) || fog.start<0.f || fog.end<=fog.start)
        return fail(d,VehicleWindowError50DS::request,"world-fog update requires finite ordered nonnegative range");
    for(float value:fog.color)
        if(!std::isfinite(value) || value<0.f || value>1.f)
            return fail(d,VehicleWindowError50DS::request,"world-fog update color outside finite [0,1]");
    if(fog.enabled && (s.request.track.mode!=VehicleWindowMode50DS::callerMaterialTextures ||
       s.trackIndexCount<=0 || !s.trackProgram || !s.trackVertexArray || !s.trackSpanCount))
        return fail(d,VehicleWindowError50DS::request,"world-fog enable requires actual retained textured track");
    s.request.track.worldFog=fog;
    return true;
}

bool VehicleWindowGl50DS::bindOriginalSpeedBoost(std::shared_ptr<game::OriginalSpeedBoost50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original SpeedBoost binding requires idle gameplay renderer");
    if(s.speedBoost||s.speedBoostUiCreationFailed||s.timeStatsUiCreationFailed||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.raceHudUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original SpeedBoost already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareSpeedBoostResources(owner);
    if(!accepted&&s.hasSpeedBoostResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteSpeedBoostNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasSpeedBoostResources())s.speedBoost.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original SpeedBoost optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalTimeStats(std::shared_ptr<game::OriginalTimeStats50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original TimeStats binding requires idle gameplay renderer");
    if(s.timeStats||s.timeStatsUiCreationFailed||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original TimeStats already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareTimeStatsResources(owner);
    if(!accepted&&s.hasTimeStatsResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteTimeStatsNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasTimeStatsResources())s.timeStats.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original TimeStats optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalDamage(std::shared_ptr<game::OriginalDamage50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original Damage binding requires idle gameplay renderer");
    if(s.damage||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.positionStatsUiCreationFailed||s.timeStatsUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original Damage already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareDamageResources(owner);
    if(!accepted&&s.hasDamageResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteDamageNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasDamageResources())s.damage.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original Damage optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::retireOriginalPauseDerived(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!storage_||!storage_->pauseDerived)return true;
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Pause retirement requires its live renderer");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"Pause retirement requires idle renderer");
    s.diagnostic=&d;
    bool okay=s.check("before original Pause binding retirement");
    if(okay){s.gl.finish();okay=s.check("drain original Pause binding work");}
    if(okay)okay=s.deletePauseDerivedNames(d);
    s.diagnostic=nullptr;
    return okay;
}
bool VehicleWindowGl50DS::bindOriginalPauseDerived(std::shared_ptr<game::OriginalPauseNativeDraw50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original PauseDerived binding requires idle gameplay renderer");
    if(s.pauseDerived||s.pauseDerivedUiCreationFailed||s.damageUiCreationFailed||s.positionStatsUiCreationFailed||s.timeStatsUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original PauseDerived already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.preparePauseDerivedResources(owner);
    if(!accepted&&s.hasPauseDerivedResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deletePauseDerivedNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasPauseDerivedResources())s.pauseDerived.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original PauseDerived optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalPositionStats(std::shared_ptr<game::OriginalPositionStatsNativeOwner50LA> owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original PositionStats binding requires idle gameplay renderer");
    if(s.positionStats||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.timeStatsUiCreationFailed||s.raceHudUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original PositionStats already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.preparePositionStatsResources(owner);
    if(!accepted&&s.hasPositionStatsResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deletePositionStatsNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasPositionStatsResources())s.positionStats.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original PositionStats optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::bindOriginalRaceHud(const game::OriginalRaceHud50KG& owner,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"renderer not initialized");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"original HUD binding requires idle gameplay renderer");
    if(s.raceHud||s.raceHudUiCreationFailed||s.positionStatsUiCreationFailed||s.damageUiCreationFailed||s.pauseDerivedUiCreationFailed||s.timeStatsUiCreationFailed||s.speedBoostUiCreationFailed)
        return fail(d,VehicleWindowError50DS::request,"original HUD already bound or late UI initialization failed");
    s.diagnostic=&d;const bool accepted=s.prepareRaceHudResources(owner);
    if(!accepted&&s.hasRaceHudResources()){
        // Optional fallback is safe only after outstanding new HUD work/names
        // have retired. An ambiguous deletion must not permit later allocation.
        s.gl.finish();const bool drained=s.check("retire failed optional HUD initialization");
        VehicleWindowDiagnostic50DS cleanup;
        if(!drained||!s.deleteRaceHudNames(cleanup))d.cleanupIncomplete=true;
        if(cleanup.cleanupGlError&&!d.cleanupGlError)d.cleanupGlError=cleanup.cleanupGlError;
    }
    if(!accepted&&!s.hasRaceHudResources())s.raceHud.reset();
    s.diagnostic=nullptr;
    if(!accepted&&d.error==VehicleWindowError50DS::none)
        fail(d,VehicleWindowError50DS::material,"original HUD optional cold join rejected");
    return accepted;
}
bool VehicleWindowGl50DS::submit(const game::VehicleDrawBatch50CO& batch,
    const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    Storage::RcFrameObservation rcObservation(s,d);
    auto submissionStageStart=s.rendererStageNow();
    if(s.request.enableGameplayDisplayWallTiming)s.playerTiming={};
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    // Only this real serial gameplay draw entry advances the native attempt.
    // Fixed updates and completion/readback methods never manufacture one.
    if(s.rcParticleWindow&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"RC particle draw entry requires the idle creating thread");
    if((s.gbParticleWindow||s.mcParticleWindow)&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"GB particle transaction requires idle creating thread");
    Storage::RcParticleAttemptScope rcParticleAttemptScope{s,d};
    Storage::GbParticleAttemptScope gbParticleAttemptScope{s,d};
    Storage::McParticleAttemptScope mcParticleAttemptScope{s,d};
    if(!s.beginRcParticleAttempt(d)||!s.beginGbParticleAttempt(d)||!s.beginMcParticleAttempt(d))return false;
    if (!s.current(d)) return false;
    // Any new gameplay attempt supersedes an uncompleted candidate. Failure
    // anywhere below cannot authorize its partial draws through a later finish.
    s.trackVisibilityPendingTarget=0;
    for(U i=0;i<s.trackSpanCount;++i)s.trackSpans[i].submittedGameplayCandidate=false;
    if(!s.texturesCurrent(d))return false;
    struct TrackUvBorrowReset {
        VehicleWindowTrackUvFrame50DS& borrowed;
        ~TrackUvBorrowReset() { borrowed={}; }
    } trackUvBorrowReset{s.frameTrackUv};
    if(!s.validateGameplayBlurFrame(frame.gameplayBlur,d) || !s.validateGcParticleFrame(frame,d) ||
       !s.validateRcParticleFrame(frame,d) || !s.validateGbParticleFrame(frame,d) || !s.validateMcParticleFrame(frame,d))return false;
    for(U i=0;i<s.trackSpanCount;++i)if(s.trackSpans[i].sourceUvAnimation &&
        (!frame.trackUvAnimation || !frame.trackUvAnimation->owner || !frame.trackUvAnimation->service))
        return fail(d,VehicleWindowError50DS::frame,"animated static material has no real frame controller");
    // A cold bank may remain resident in Main. Only active race frames supply
    // meshes; absence never requests an animation clock or a lighting owner.
    if((frame.gameplayBinderCount&&!s.request.gameplayBinderTextures)||
        bool(frame.gameplayBinders)!=bool(frame.gameplayBinderCount))
        return fail(d,VehicleWindowError50DS::frame,"Binder cold/frame presence differs");
    if(frame.gameplayBinderCount){
        if(frame.gameplayBinderCount<frame.opponentCount+1u||frame.gameplayBinderCount>2u*(frame.opponentCount+1u)||frame.gameplayBinderCount>16u||frame.gameplayBinderFrame>=10u)
            return fail(d,VehicleWindowError50DS::frame,"Binder racer/frame extent differs");
        constexpr float uv[4][2]={{0,0},{0,1},{1,0},{1,1}};
        for(U i=0;i<frame.gameplayBinderCount;++i)for(U v=0;v<4;++v){
            const auto& vertex=frame.gameplayBinders[i].vertices[v];
            for(float x:vertex.position)if(!std::isfinite(x))return fail(d,VehicleWindowError50DS::frame,"Nonfinite Binder position");
            for(U c=0;c<2;++c)if(vertex.uv[c]!=uv[v][c])return fail(d,VehicleWindowError50DS::frame,"Binder source UV order differs");
            if(vertex.color[3]!=64)return fail(d,VehicleWindowError50DS::frame,"Binder source global ambient alpha differs");
        }
    }
    if (frame.width <= 0 || frame.height <= 0 ||
        frame.width > s.maximumWidth || frame.height > s.maximumHeight ||
        !frame.worldViewProjection || !frame.clearRgba)
        return fail(d, VehicleWindowError50DS::frame,
            "explicit matrix/color and nonzero viewport within cold bounds required");
    for (U i = 0; i < 16; ++i)
        if (!std::isfinite(frame.worldViewProjection[i]))
            return fail(d, VehicleWindowError50DS::frame, "nonfinite world-to-clip matrix");
    for (U i = 0; i < 4; ++i)
        if (!std::isfinite(frame.clearRgba[i]) || frame.clearRgba[i] < 0 || frame.clearRgba[i] > 1)
            return fail(d, VehicleWindowError50DS::frame, "clear color outside finite [0,1]");
    if(!s.prepareRaWorldFrame(frame,d))return false;
    if (s.request.track.worldFog.enabled) {
        if (!frame.worldFogCamera)
            return fail(d, VehicleWindowError50DS::frame, "world fog requires explicit camera position");
        for (U i = 0; i < 3; ++i)
            if (!std::isfinite(frame.worldFogCamera[i]))
                return fail(d, VehicleWindowError50DS::frame, "nonfinite world fog camera");
    }
    if (bool(s.skyIndexCount) != bool(frame.skyWorldViewProjection))
        return fail(d, VehicleWindowError50DS::frame, "sky matrix presence must match authored sky request");
    if (frame.skyWorldViewProjection)
        for (U i = 0; i < 16; ++i)
            if (!std::isfinite(frame.skyWorldViewProjection[i]))
                return fail(d, VehicleWindowError50DS::frame, "nonfinite authored sky matrix");
    // All selected-light validation and value copying precedes the first GL
    // call. Missing/invalid requested packets never fall back to generic light.
    game::NativeGcSelectedLightsCandidate playerLights{};
    game::NativeGcSelectedLightsCandidate opponentLights[maxVehicleWindowOpponents50DS]{};
    {
        // These owned validators neither invoke callbacks nor mutate the live
        // vertex/normal streams. Reuse only a completed, identity-matched scan;
        // ready() still performs its original scan after currentness callbacks.
        Storage::RacerNormalReceipt normalReceipts[1 + maxVehicleWindowOpponents50DS]{};
        if (!s.validateMaterialUvs(frame.materialUvs, frame.materialUvCount, d) ||
            !s.validateOpponents(batch, frame, d, normalReceipts) ||
            !s.validateDynamicWorldFrame(frame.dynamicWorld, d) ||
            !s.validateTrackAnimationFrames(frame, d)) return false;
        std::uintptr_t fleetLightOwner = 0;
        if (!s.copyNativeGcLights(frame.lightingPolicy, frame.selectedLights, 0,
            batch, 0, fleetLightOwner, playerLights, d, normalReceipts[0])) return false;
        for (U i = 0; i < frame.opponentCount; ++i) {
            d.opponentIndex = i;
            const auto& opponent = frame.opponents[i];
            if (!s.copyNativeGcLights(opponent.lightingPolicy, opponent.selectedLights, i + 1,
                *opponent.batch, opponent.racerArchetype, fleetLightOwner, opponentLights[i], d,
                normalReceipts[i + 1])) return false;
        }
        d.opponentIndex = maxVehicleWindowOpponents50DS;
        if (!s.prepareSpecularFrame50LA(batch, frame, d, normalReceipts) || !s.prepareRcCrystalFrame(frame,d)) return false;
    }
    if (bool(s.request.cables.textures) != bool(frame.cables))
        return fail(d, VehicleWindowError50DS::frame, "cable frame presence must match explicit cold cable request");
    if (frame.cables && !validateCableFrame(*frame.cables, d)) return false;
    d.originalSpeedBoostRejected=!s.prepareSpeedBoostFrame(frame.originalSpeedBoost);
    if(d.originalSpeedBoostRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original Speed/Boost frame rejected");
    d.originalTimeStatsRejected=!s.prepareTimeStatsFrame(frame.originalTimeStats);
    if(d.originalTimeStatsRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original TimeStats frame rejected");
    d.originalPositionStatsRejected=!s.preparePositionStatsFrame(frame.originalPositionStats);
    if(d.originalPositionStatsRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original PositionStats frame rejected");
    d.originalPauseDerivedRejected=!s.preparePauseDerivedFrame(frame.originalPauseDerived);
    if(d.originalPauseDerivedRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original Pause derived frame rejected");
    d.originalDamageRejected=!s.prepareDamageFrame(frame.originalDamage);
    if(d.originalDamageRejected)
        return fail(d,VehicleWindowError50DS::material,"requested original Damage frame rejected");
    float admittedOriginalBottom=0.f;
    if(s.timeStats&&s.timeStats->frame){
        // Match drawTimeStats' actual640x448 aspect transform, including
        // letterboxing. Use admitted copied geometry, never a caller flag.
        const double sourceScale=std::min(double(frame.width)/640.0,double(frame.height)/448.0);
        const double oy=(frame.height-448.0*sourceScale)*.5;
        float bottom=0.f;
        for(U i=0;i<s.timeStats->quads*4;++i)
            bottom=std::max(bottom,s.timeStats->vertices[i].position[1]);
        admittedOriginalBottom=static_cast<float>(oy+double(bottom)*sourceScale);
    }
    if (frame.hostHud) {
        if (!s.request.enableHostHud || !s.hostHudGeometry)
            return fail(d, VehicleWindowError50DS::frame, "host HUD frame requires explicit cold enableHostHud");
        const VehicleWindowHostHud50DS hud = *frame.hostHud;
        const float values[] = {hud.heat01, hud.leftDamage01, hud.rightDamage01};
        for (float value : values)
            if (!std::isfinite(value) || value < 0.f || value > 1.f)
                return fail(d, VehicleWindowError50DS::frame, "host HUD live values must be finite [0,1]");
        if (hud.routeValid &&
            (!std::isfinite(hud.routeX) || !std::isfinite(hud.routeY) ||
             std::abs(hud.routeX) > 1.f || std::abs(hud.routeY) > 1.f ||
             std::abs(hud.routeX * hud.routeX + hud.routeY * hud.routeY - 1.f) > 0.001f))
            return fail(d, VehicleWindowError50DS::frame,
                "host HUD enabled route bearing must be finite normalized screen-right/ahead");
        if (hud.lapCountValid && hud.completedLaps > maxVehicleWindowCompletedLaps50DS)
            return fail(d, VehicleWindowError50DS::frame,
                "host HUD enabled completed-lap count must be in [0,999]");
        // Native 1AFD80 installs 0x40A66666 (5.2 seconds) for the ordinary
        // course start. The existing one-way HUD adapter displays ceil of
        // the real remaining time, so its admitted maximum is 6, not the
        // earlier HOST-only three-second start. This changes no race clock.
        constexpr U maxRaceCountdown = 6;
        if (hud.raceEnabled && (!hud.raceLapTarget || hud.raceLapTarget > 999 ||
            !hud.raceLap || hud.raceLap > hud.raceLapTarget || !hud.raceRacerCount ||
            hud.raceRacerCount > 16 || !hud.racePlace || hud.racePlace > hud.raceRacerCount ||
            hud.raceCountdown > maxRaceCountdown ||
            unsigned(hud.raceCountdown != 0) + unsigned(hud.raceGo) + unsigned(hud.raceFinished) +
                unsigned(!hud.raceFinished && hud.raceRetirementCD4) > 1)) {
            // Failure-only payload; keep the public diagnostic ABI intact.
            if (d.error == VehicleWindowError50DS::none)
                std::fprintf(stderr,
                    "native race HUD rejection lap=%u target=%u place=%u racers=%u countdown=%u go=%u finished=%u retirementCD4=%u\n",
                    static_cast<unsigned>(hud.raceLap), static_cast<unsigned>(hud.raceLapTarget),
                    static_cast<unsigned>(hud.racePlace), static_cast<unsigned>(hud.raceRacerCount),
                    static_cast<unsigned>(hud.raceCountdown), static_cast<unsigned>(hud.raceGo),
                    static_cast<unsigned>(hud.raceFinished), static_cast<unsigned>(hud.raceRetirementCD4));
            return fail(d, VehicleWindowError50DS::frame, "native race HUD counters/status outside admitted bounds");
        }
        if (hud.raceResultsReady && (!hud.raceEnabled || (!hud.raceFinished && !hud.raceRetirementCD4) ||
            hud.raceCountdown || hud.raceGo))
            return fail(d, VehicleWindowError50DS::frame, "host results presentation requires a sealed finish or retirement");
        if (hud.resultsPanelVisible && (!hud.raceResultsReady || !hud.standingsCount ||
            hud.resultsSelectedAction >= 3 ||
            (!hud.resultsConfirming && hud.resultsConfirmAccept)))
            return fail(d, VehicleWindowError50DS::frame, "live results panel requires a sealed outcome and bounded action");
        if (hud.resultsPanelVisible && hud.raceFinished &&
            (!hud.resultsFinishPlace || hud.resultsFinishPlace > hud.raceRacerCount ||
             (!hud.standingsSourceRanks && hud.resultsFinishPlace != hud.racePlace) ||
             !std::isfinite(hud.resultsFinishSeconds) || hud.resultsFinishSeconds < 0.0 ||
             hud.resultsFinishSeconds > double(maxVehicleWindowLapSeconds50DS)))
            return fail(d, VehicleWindowError50DS::frame, "finished results require an explicit bounded finish place/time");
        if (hud.resultsPanelVisible && !hud.raceFinished &&
            (hud.resultsFinishPlace || hud.resultsFinishSeconds != 0.0))
            return fail(d, VehicleWindowError50DS::frame, "DNF results cannot supply a finish place or finish time");
        if (hud.raceClocksValid) {
            const auto validSeconds = [](float value) noexcept {
                return std::isfinite(value) && value >= 0.f && value <= maxVehicleWindowLapSeconds50DS;
            };
            if (!hud.raceEnabled || !validSeconds(hud.raceElapsedSeconds) ||
                (!hud.raceFinished && !hud.raceRetirementCD4 && !validSeconds(hud.currentLapSeconds)))
                return fail(d, VehicleWindowError50DS::frame, "native race clocks outside admitted presentation bounds");
        }
        if (hud.completedLapTimesValid) {
            if (!hud.raceEnabled || !hud.lapCountValid || !hud.standingsCount ||
                hud.raceLapTarget > maxVehicleWindowLapTimes50DS ||
                hud.completedLapTimeCount != std::min(hud.completedLaps, hud.raceLapTarget))
                return fail(d, VehicleWindowError50DS::frame, "completed lap times require the bounded committed race");
            const auto validSeconds = [](float value) noexcept {
                return std::isfinite(value) && value >= 0.f && value <= maxVehicleWindowLapSeconds50DS;
            };
            if (hud.completedLapTimeCount && !validSeconds(hud.bestLapSeconds))
                return fail(d, VehicleWindowError50DS::frame, "native best lap seconds outside admitted bounds");
            for (U i = 0; i < hud.completedLapTimeCount; ++i)
                if (!validSeconds(hud.completedLapSeconds[i]))
                    return fail(d, VehicleWindowError50DS::frame, "native completed lap seconds outside admitted bounds");
        }
        if (hud.standingsCount) {
            if (!hud.raceEnabled || hud.standingsCount > maxVehicleWindowStandings50DS ||
                hud.standingsCount != hud.raceRacerCount || hud.standingsPlayerIndex >= hud.standingsCount)
                return fail(d, VehicleWindowError50DS::frame, "named standings require the complete bounded race field");
            bool seen[maxVehicleWindowStandings50DS]{}, unfinishedSeen = false;
            for (U i = 0; i < hud.standingsCount; ++i) {
                const auto& row = hud.standings[i];
                const bool rankValid = hud.standingsSourceRanks ?
                    (row.place >= 1 && row.place <= hud.standingsCount &&
                     (!i || hud.standings[i - 1].place <= row.place)) : row.place == i + 1;
                if (row.racerIndex >= hud.standingsCount || seen[row.racerIndex] || !rankValid ||
                    !validHostHudName(row.displayName) || row.completedLaps > maxVehicleWindowCompletedLaps50DS ||
                    row.finished != (row.completedLaps >= hud.raceLapTarget) ||
                    (!hud.standingsSourceRanks && row.finished && unfinishedSeen) || (hud.raceCountdown && row.completedLaps))
                    return fail(d, VehicleWindowError50DS::frame, "named standings order/name/lap/finish state rejected");
                seen[row.racerIndex] = true;
                unfinishedSeen = unfinishedSeen || !row.finished;
                if (row.racerIndex == hud.standingsPlayerIndex &&
                    (row.place != hud.racePlace || row.finished != hud.raceFinished ||
                     row.retirementCD4 != hud.raceRetirementCD4 ||
                     std::min(row.completedLaps + 1, hud.raceLapTarget) != hud.raceLap))
                    return fail(d, VehicleWindowError50DS::frame, "named standings player differs from race HUD");
            }
        }
        if (!s.hostHudGeometry->build(hud, frame.width, frame.height,admittedOriginalBottom))
            return fail(d, VehicleWindowError50DS::limits, "host HUD exceeds fixed glyph geometry capacity");
    }
    // Reset every cold material to identity so absent overrides never retain
    // a previous frame's animation. Validation above precedes all GL work.
    for (U i = 0; i < s.materialCount; ++i) {
        auto& uv = s.materials[i].uv;
        uv[0] = uv[1] = 1.f; uv[2] = uv[3] = 0.f;
    }
    for (U i = 0; i < frame.materialUvCount; ++i) {
        const auto& input = frame.materialUvs[i];
        auto& uv = s.materials[s.materialOffsets[componentIndex(input.component)] + input.materialIndex].uv;
        uv[0] = input.uScale; uv[1] = input.vScale;
        uv[2] = input.uOffset; uv[3] = input.vOffset;
    }
    // Snapshot bounded opponent descriptors and UV values before any GL work.
    // Their independent source vertex/cable arrays remain borrowed only through
    // this synchronous transaction; no per-frame allocation or index upload.
    // Reuse the admitted light values above instead of copying every component
    // matrix again. Reset on every exit, including a failed backend transaction.
    struct FrameLightBorrowReset {
        Storage& state;
        U opponentCount;
        ~FrameLightBorrowReset() noexcept {
            state.framePlayerLights = nullptr;
            for (U i = 0; i < opponentCount; ++i) state.frameOpponentLights[i] = nullptr;
        }
    } frameLightBorrowReset{s, frame.opponentCount};
    s.frameOpponentCount = frame.opponentCount;
    s.framePlayerBatch = batch;
    s.framePlayerLights = &playerLights;
    for (U i = 0; i < frame.opponentCount; ++i) {
        const auto& opponent = frame.opponents[i];
        s.frameOpponentBatches[i] = *opponent.batch;
        s.frameOpponentLights[i] = &opponentLights[i];
        s.frameOpponentCables[i] = opponent.cables;
        s.frameOpponentArchetypes[i] = opponent.racerArchetype;
        const auto selected = s.racerView(opponent.racerArchetype);
        for (U m = 0; m < selected.materialCount; ++m)
            s.opponentMaterialUvs[i * s.opponentMaterialStride + m] = {};
        for (U u = 0; u < opponent.materialUvCount; ++u) {
            const auto& input = opponent.materialUvs[u];
            auto& uv = s.opponentMaterialUvs[i * s.opponentMaterialStride +
                selected.materialOffsets[componentIndex(input.component)] + input.materialIndex].value;
            uv[0] = input.uScale; uv[1] = input.vScale;
            uv[2] = input.uOffset; uv[3] = input.vOffset;
        }
    }
    s.frameTrackUv=frame.trackUvAnimation?*frame.trackUvAnimation:VehicleWindowTrackUvFrame50DS{};
    s.frameSkyEnabled=frame.skyEnabled;
    if (frame.skyWorldViewProjection)
        std::memcpy(s.skyMatrix, frame.skyWorldViewProjection, sizeof(s.skyMatrix));
    if (s.request.track.worldFog.enabled)
        std::memcpy(s.worldFogCamera, frame.worldFogCamera, sizeof(s.worldFogCamera));
    std::memcpy(s.matrix, frame.worldViewProjection, sizeof(s.matrix));
    std::memcpy(s.clearRgba, frame.clearRgba, sizeof(s.clearRgba));
    s.pendingRaceCameraPublication=frame.sourceBodyCameraPublication50LA;
    s.width = frame.width; s.height = frame.height;
    s.frameCables = frame.cables;
    s.frameGameplayBinders=frame.gameplayBinders;s.frameGameplayBinderCount=frame.gameplayBinderCount;
    s.frameGameplayBinderImage=frame.gameplayBinderFrame;
    s.frameGcParticles=frame.mcParticles?frame.mcParticles->rendererDraw():frame.gbParticles?frame.gbParticles->rendererDraw():(frame.rcParticles?frame.rcParticles->rendererDraw():frame.gcParticles);
    s.frameRcParticles=bool(frame.rcParticles);s.frameGbParticles=bool(frame.gbParticles);s.frameMcParticles=bool(frame.mcParticles);
    s.frameDynamicWorld = frame.dynamicWorld;
    for (U i = 0; i < s.trackAnimationCount; ++i)
        s.trackAnimations[i].selectedFrame = frame.trackAnimationFrames[i];
    s.frameHostHud = frame.hostHud != nullptr;
    // Prepare authentic geometry before any gameplay target writes. A bounded
    // rejection leaves the existing native HUD and world frame unchanged.
    d.originalRaceHudRejected=!s.prepareRaceHudFrame(frame.originalRaceHud);
    if(d.originalRaceHudRejected&&s.raceHud)s.raceHud->frame=false;
    s.frameGameplayBlurEnabled=frame.gameplayBlur!=nullptr;
    s.frameGameplayBlur=frame.gameplayBlur?*frame.gameplayBlur:GameplayBlurFrame50DS{};
    s.frameSet = true; s.diagnostic = &d;
    s.prepareTrackVisibility();
    s.traceRaStage("validation_prepare",submissionStageStart,false);
    bool accepted = s.check("before private submission") &&
        (s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    if (accepted) accepted = s.prepareCoverageExtent();
    s.traceRaStage("target_preflight",submissionStageStart,false);
    if (accepted) {
        accepted = game::submitVehicleDrawBatch50CS(batch, s.adapter->backend(), d.submission);
        s.traceRaStage("backend_transaction",submissionStageStart,false);
        if (!accepted) {
            // Structural rejection precedes adapter error reset: never copy an
            // old GL error into a new structural-rejection diagnostic.
            if (!d.glError && d.submission.error >= game::VehicleDrawSubmitError50CS::backendPreflight)
                d.glError = s.adapter->glError();
            std::fprintf(stderr,"vehicle_submission_failure stage=%u component=%u span=%u gl=%u\n",
                unsigned(d.submission.error),d.submission.component,d.submission.span,s.adapter->glError());
            // Keep the first concrete world/effect rejection from callbacks;
            // the outer submission status must not erase its cause.
            if(d.error==VehicleWindowError50DS::none)
                fail(d, VehicleWindowError50DS::submission, "50CS/50CV submission rejected");
        } else {
            d.batchAccepted = true;
            if(s.request.output==VehicleWindowOutput50DS::headless)
                s.noteGameplayDisplayPending(s.displayedTarget,s.committedWidth,s.committedHeight);
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                auto copyStageStart=s.rendererStageNow();
                accepted = s.copyToWindow();
                s.traceRaStage("copy_to_window",copyStageStart,false);
                if (!accepted && d.error == VehicleWindowError50DS::gl)
                    d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    s.frameSet = false; s.frameCables = nullptr; s.frameHostHud = false; s.diagnostic = nullptr;
    if(s.raceHud)s.raceHud->frame=false;
    if(s.timeStats)s.timeStats->frame=false;
    if(s.positionStats)s.positionStats->frame=false;
    if(s.damage)s.damage->frame=false;
    if(s.pauseDerived)s.pauseDerived->frame=false;
    if(s.speedBoost)s.speedBoost->frame=false;
    s.frameGameplayBlurEnabled=false;s.frameGameplayBlur={};
    s.frameGameplayBinders=nullptr;s.frameGameplayBinderCount=0;s.frameGameplayBinderImage=0;
    s.frameGcParticles=nullptr;s.frameRcParticles=false;s.frameGbParticles=false;s.frameMcParticles=false;
    s.framePlayerBatch = {};
    s.frameDynamicWorld = {};
    for (U i = 0; i < s.frameOpponentCount; ++i) {
        s.frameOpponentBatches[i] = {};
        s.frameOpponentCables[i] = nullptr;
        s.frameOpponentArchetypes[i] = 0;
    }
    s.frameOpponentCount = 0;
    if(accepted && s.displayedTarget==s.targets[s.published])
        s.trackVisibilityPendingTarget=s.displayedTarget;
    if(s.rcParticleWindow){
        if(accepted&&!s.trackVisibilityPendingTarget)
            accepted=fail(d,VehicleWindowError50DS::frame,"RC accepted draw has no actual pending gameplay target");
        rcParticleAttemptScope.retainPending=accepted;
    }
    if(s.gbParticleWindow){
        if(accepted&&!s.trackVisibilityPendingTarget)
            accepted=fail(d,VehicleWindowError50DS::frame,"GB accepted draw has no actual pending gameplay target");
        gbParticleAttemptScope.retainPending=accepted;
    }
    if(s.mcParticleWindow){
        if(accepted&&!s.trackVisibilityPendingTarget)
            accepted=fail(d,VehicleWindowError50DS::frame,"MC accepted draw has no actual pending gameplay target");
        mcParticleAttemptScope.retainPending=accepted;
    }
    s.reportPlayerTiming(accepted);
    return accepted;
}

bool VehicleWindowGl50DS::bindOriginalUiTextures(const assets::MaterialTextureAssets50EA& cpu,
    const MaterialTextureGl50EB& gpu, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableOriginalUi || s.diagnostic ||
        s.uiTextureEpoch==std::numeric_limits<std::uint64_t>::max() || (s.riderTextureBorrow&&s.uiSourceFrameOwner))
        return fail(d, VehicleWindowError50DS::frame, "UI bank replacement requires idle original UI renderer and retired Rider source");
    auto candidate = s.request;
    candidate.originalUiCpuTextures = &cpu;
    candidate.originalUiTextures = &gpu;
    s.diagnostic = &d;
    std::uint8_t stagedAlpha[assets::maxMaterialTextureAssets50EA]{};
    const bool accepted = s.validateOriginalUiBank(candidate,stagedAlpha);
    s.diagnostic = nullptr;
    if (!accepted) return false;
    if(s.riderTextureBorrow&&!s.riderTextureBorrow->releaseRiderBorrow(&s))
        return fail(d,VehicleWindowError50DS::material,"Rider renderer borrower retirement mismatch");
    s.riderTextureBorrow=nullptr;
    std::memcpy(s.uiTextureMaximumAlpha,stagedAlpha,sizeof(stagedAlpha));++s.uiTextureEpoch;
    s.request.originalUiCpuTextures = &cpu;
    s.request.originalUiTextures = &gpu;
    s.uiTexturePublication = gpu.publicationToken();
    return true;
}

bool VehicleWindowGl50DS::prevalidateRiderTextures(const OriginalUiRiderResidencyDemand50KG& demand,
    const MaterialTextureGl50EB& gpu,OriginalUiRiderTextureValidation50KG& out,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};out={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Rider validation requires actual initialized renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic||!s.request.enableOriginalUi||!demand.session_||!demand.serial_||
       demand.consumed_||!demand.bank_||!gpu.riderSparse()||gpu.riderSession()!=demand.session_||
       gpu.riderBank()!=demand.bank_.get()||gpu.riderDemandSerial()!=demand.serial_||
       !gpu.validBorrow(s.request.context,s.request.isCurrent,gpu.publicationToken()))
        return fail(d,VehicleWindowError50DS::material,"private pending Rider demand and exact sparse publication required");
    const auto images=demand.bank_->images();const auto* cpu=images?images->textures():nullptr;
    if(!cpu||!cpu->loaded()||!cpu->textureCount()||cpu->textureCount()>assets::maxMaterialTextureAssets50EA||cpu->textureCount()!=gpu.textureCount())
        return fail(d,VehicleWindowError50DS::material,"Rider logical CPU/GPU bank extent mismatch");
    OriginalUiRiderTextureValidation50KG next;
    for(U i=0;i<cpu->textureCount();++i){const auto* source=cpu->texture(i);const auto* image=gpu.asset(i);
        if(!source||bool(image)!=demand.includes(i)||(image&&(!image->name||image->identity!=source->identity)))
            return fail(d,VehicleWindowError50DS::material,"Rider closure residency/CPU identity mismatch");
        std::uint8_t maximum=0;for(std::size_t a=3;a<source->texture.rgba.size();a+=4)maximum=std::max(maximum,source->texture.rgba[a]);
        next.maximumAlpha_[i]=maximum;
    }
    if(demand.requiresCoverage_){
        if(!s.request.enableOriginalUiSourceRgba||s.uiBlendOldSnapshot||s.displayFadeOwner)
            return fail(d,VehicleWindowError50DS::frame,"coverage prevalidation requires idle source UI without active display lease");
        s.diagnostic=&d;const bool prepared=s.prepareUiCoverage("cold_prevalidation");s.diagnostic=nullptr;
        if(!prepared)return false;
    }
    next.requiresCoverage_=demand.requiresCoverage_;
    next.renderer_=this;next.session_=demand.session_;next.bank_=demand.bank_.get();next.gpu_=&gpu;
    next.serial_=demand.serial_;next.token_=gpu.publicationToken();next.rendererEpoch_=s.uiTextureEpoch;next.ready_=true;
    out=next;return true;
}
bool VehicleWindowGl50DS::commitRiderTextures(const OriginalUiRiderResidencyDemand50KG& demand,
    OriginalUiRiderTextureValidation50KG& receipt,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Rider commit requires initialized renderer");
    auto& s=*storage_;const auto* gpu=receipt.gpu_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic||s.uiSourceFrameOwner||!s.request.enableOriginalUi||
       !receipt.ready_||receipt.renderer_!=this||receipt.rendererEpoch_!=s.uiTextureEpoch||
       s.uiTextureEpoch==std::numeric_limits<std::uint64_t>::max()||!demand.consumed_||
       receipt.session_!=demand.session_||receipt.serial_!=demand.serial_||receipt.bank_!=demand.bank_.get()||
       receipt.requiresCoverage_!=demand.requiresCoverage_||(receipt.requiresCoverage_&&!s.uiCoverage.ready)||
       !gpu||!gpu->riderSparse()||gpu->riderSession()!=demand.session_||gpu->riderBank()!=demand.bank_.get()||
       gpu->riderDemandSerial()!=demand.serial_||!gpu->validBorrow(s.request.context,s.request.isCurrent,receipt.token_))
        return fail(d,VehicleWindowError50DS::material,"stale Rider prevalidation or unretired outgoing source");
    const auto images=demand.bank_->images();const auto* cpu=images?images->textures():nullptr;
    if(!cpu||cpu->textureCount()!=gpu->textureCount())return fail(d,VehicleWindowError50DS::material,"Rider commit bank changed");
    for(U i=0;i<cpu->textureCount();++i)if(bool(gpu->asset(i))!=demand.includes(i))
        return fail(d,VehicleWindowError50DS::material,"Rider demand changed after renderer prevalidation");
    if(!gpu->acquireRiderBorrow(&s))return fail(d,VehicleWindowError50DS::material,"Rider candidate renderer lease rejected");
    if(s.riderTextureBorrow&&!s.riderTextureBorrow->releaseRiderBorrow(&s)){
        gpu->releaseRiderBorrow(&s);return fail(d,VehicleWindowError50DS::material,"Rider outgoing renderer lease mismatch");}
    s.riderTextureBorrow=gpu;s.request.originalUiCpuTextures=cpu;s.request.originalUiTextures=gpu;
    s.uiTexturePublication=receipt.token_;++s.uiTextureEpoch;
    std::memcpy(s.uiTextureMaximumAlpha,receipt.maximumAlpha_.data(),sizeof(s.uiTextureMaximumAlpha));
    receipt.ready_=false;return true;
}

bool VehicleWindowGl50DS::submitOriginalUi(const OriginalUiDraw50KG&,
    std::int32_t, std::int32_t, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    return fail(d, VehicleWindowError50DS::material,
        "draw-only original UI lacks explicit depth and alpha-consumer ownership; use native descriptor");
}

bool VehicleWindowGl50DS::submitOriginalUi(const OriginalUiNativeDraw50KG& native,
    std::int32_t width, std::int32_t height, VehicleWindowDiagnostic50DS& d) noexcept {
    return submitOriginalUiImpl(native,width,height,false,d);
}
bool VehicleWindowGl50DS::originalUiSourceRgbaAvailable() const noexcept {
    return initialized() && storage_->request.enableOriginalUiSourceRgba &&
        (storage_->uiSourceCoherentFramebufferFetch || storage_->gl.textureBarrier) &&
        storage_->uiSourceTarget && storage_->uiSourceColor && storage_->uiSourceDepth &&
        storage_->uiSourceIndexBuffer && storage_->uiSourceIndices;
}
bool VehicleWindowGl50DS::prepareOriginalUiDirectVideo(OriginalUiDirectVideoCapability50KG& out,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};out={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"direct video requires actual source RGBA renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic||!s.uiProgram||!s.uiVertexArray||!s.uiVertexBuffer)
        return fail(d,VehicleWindowError50DS::frame,"direct video capability requires idle actual UI draw owner");
    // Dedicated frontend creation does not load gameplay-blur/display-fade
    // entry points. Resolve this actual direct sprite dependency explicitly,
    // using the same checked loader before minting any renderer capability.
    s.diagnostic=&d;
    const bool directDrawReady=s.gl.drawArrays||s.load(s.gl.drawArrays,"glDrawArrays");
    s.diagnostic=nullptr;
    if(!directDrawReady)return false;
    try {if(!s.uiDirectVideoLifetime)s.uiDirectVideoLifetime=std::make_shared<const std::uint8_t>(0);}
    catch(...){return fail(d,VehicleWindowError50DS::frame,"direct video capability allocation failed");}
    out.lifetime_=s.uiDirectVideoLifetime;out.renderer_=this;return true;
}
bool VehicleWindowGl50DS::submitOriginalUiSourceRgba(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d) noexcept {
    return submitOriginalUiSourceFrame(frame,width,height,true,d);
}
bool VehicleWindowGl50DS::submitOriginalUiSourceFrame(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,bool publishSource,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if(s.garagePreludeOwner50KG&&(frame.owner_!=s.garagePreludeOwner50KG||
       frame.frameGeneration_!=s.garagePreludePublication50KG||s.garagePreludeFailed50KG))
        return fail(d,VehicleWindowError50DS::frame,"source frame differs from actual Garage viewport prelude");
    if (!frame.native_ || !frame.owner_ || !frame.frameGeneration_ ||
        (s.uiSourceFrameOwner && frame.owner_!=s.uiSourceFrameOwner) ||
        (frame.owner_==s.uiSourceFrameOwner && frame.frameGeneration_<=s.uiSourceFrameGeneration) ||
        s.uiSourceTargetGeneration==std::numeric_limits<std::uint64_t>::max() ||
        (publishSource && s.uiSourcePublicationGeneration==std::numeric_limits<std::uint64_t>::max()))
        return fail(d,VehicleWindowError50DS::frame,"unsealed or stale complete original UI frame");
    if(frame.directVideoPresent_&&(!validOriginalUiDirectVideo50KG(frame.directVideo_)||
       frame.directVideoCapability_.renderer_!=this||!frame.directVideoCapability_.valid()||
       frame.directVideoCapability_.lifetime_.lock()!=s.uiDirectVideoLifetime))
        return fail(d,VehicleWindowError50DS::frame,"direct video tuple or live renderer capability rejected");
    const auto* riderGpu=frame.native_->draw.textures;
    const bool riderSparse=riderGpu&&riderGpu->riderSparse();
    if(riderSparse&&(frame.riderSession_!=riderGpu->riderSession()||frame.riderTextures_!=riderGpu||
       frame.riderResidencySerial_!=riderGpu->riderDemandSerial()||s.riderTextureBorrow!=riderGpu))
        return fail(d,VehicleWindowError50DS::material,"Rider complete-frame receipt session/publication/serial mismatch");
    if(frame.native_->draw.spanCount>originalUiMaximumSpans50KG||
       (frame.native_->draw.spanCount&&!frame.native_->draw.spans))
        return fail(d,VehicleWindowError50DS::frame,"bounded source span array is missing");
    bool hasGaragePrelit=false;
    for(std::uint32_t i=0;i<frame.native_->draw.spanCount;++i)
        if(frame.native_->draw.spans[i].materialMode==OriginalUiMaterialMode50KG::sourcePrelitCompressed)hasGaragePrelit=true;
    if(hasGaragePrelit&&!frame.garageCanonical_)
        return fail(d,VehicleWindowError50DS::material,"Garage PreLit requires its actual canonical pass transaction");
    if(frame.garageCanonical_&&!frame.garageCanonical_->preflight(frame,frame.owner_,frame.frameGeneration_,frame.native_,*this))
        return fail(d,VehicleWindowError50DS::material,"Garage canonical handles, source image bank or effective state rejected");
    if (!frame.owner_->beginSourceSubmission(frame.frameGeneration_,*this))
        return fail(d,VehicleWindowError50DS::frame,"original UI source binding-history receipt rejected");
    if(frame.garageCanonical_&&!frame.garageCanonical_->begin(frame,frame.owner_,frame.frameGeneration_,frame.native_,*this)){
        frame.owner_->finishSourceSubmission(frame.frameGeneration_,false,false);
        return fail(d,VehicleWindowError50DS::material,"Garage canonical submission changed before target work");
    }
    bool sourceWorkStarted=false;
    const auto sourceTargetBefore=s.uiSourceTargetGeneration;
    const auto submitDiagnosticStart=Storage::UiCoverageClock::now();
    const bool accepted=submitOriginalUiImpl(*frame.native_,width,height,true,d,&sourceWorkStarted,publishSource,riderSparse,frame.directVideoPresent_?&frame.directVideo_:nullptr);
    const auto submitDiagnosticNs=Storage::uiCoverageNs(submitDiagnosticStart);
    if(s.uiCoverageSourceDiagnostic&&submitDiagnosticNs>=100000000ull&&s.uiCoverageSlowTotalReports<8){
        ++s.uiCoverageSlowTotalReports;
        std::fprintf(stderr,"ui_coverage_slow_submit frame=%llu target_before=%llu target=%llu total_wall_ns=%llu accepted=%u source_accepted=%u scope=cpu_submit_including_diagnostic_logging_no_added_wait\n",
            (unsigned long long)frame.frameGeneration_,(unsigned long long)sourceTargetBefore,
            (unsigned long long)s.uiSourceTargetGeneration,(unsigned long long)submitDiagnosticNs,
            unsigned(accepted),unsigned(d.originalUiSourceAccepted));
    }
    const bool canonicalHistoryAccepted=!frame.garageCanonical_||frame.garageCanonical_->finish(
        frame.frameGeneration_,d.originalUiSourceAccepted,sourceWorkStarted);
    frame.owner_->finishSourceSubmission(frame.frameGeneration_,d.originalUiSourceAccepted,sourceWorkStarted);
    d.originalUiSourceFrameGeneration=frame.frameGeneration_;
    d.originalUiSourceTargetGeneration=s.uiSourceTargetGeneration;
    // Keep the attempted owner borrowed after any potentially partial source
    // work. Only explicit successful retirement may release that lifetime.
    if (sourceWorkStarted || d.originalUiSourceAccepted || s.uiSourceTargetGeneration!=sourceTargetBefore)
        s.uiSourceFrameOwner=frame.owner_;
    // Only the actual successful complete source drain seals these values.
    // A later opaque copy failure does not undo a completed material bind.
    if (d.originalUiSourceAccepted){
        s.uiSourceFrameGeneration=frame.frameGeneration_;
        s.uiSourceAcceptedTargetGeneration=s.uiSourceTargetGeneration;
        s.uiSourceAcceptedTexturePublication=frame.native_->draw.texturePublication;
        s.uiSourceTerminal=frame.terminal_;
        for(unsigned i=0;i<4;++i)s.uiSourceClear[i]=frame.native_->draw.clearRgba[i];
    }
    if(!canonicalHistoryAccepted)
        return fail(d,VehicleWindowError50DS::material,"Garage canonical history indeterminate after actual source work");
    return accepted;
}
bool VehicleWindowGl50DS::matchesOriginalUiSource(const OriginalUiSourceSubmission50KN& receipt)const noexcept {
    if (!storage_)return false;const auto& s=*storage_;
    const auto lifetime=receipt.lifetime_.lock();
    return lifetime && lifetime==s.uiSourceReceiptLifetime && receipt.targetGeneration_ &&
        receipt.targetGeneration_==s.uiSourceTargetGeneration &&
        receipt.targetGeneration_==s.uiSourceAcceptedTargetGeneration &&
        receipt.frameGeneration_==s.uiSourceFrameGeneration &&
        receipt.texturePublication_==s.uiSourceAcceptedTexturePublication;
}
bool VehicleWindowGl50DS::originalUiSourceSubmission(OriginalUiSourceSubmission50KN& receipt,
    VehicleWindowDiagnostic50DS& d) noexcept {
    receipt={};d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !s.uiSourceFrameOwner || !s.uiSourceFrameGeneration ||
        !s.uiSourceAcceptedTargetGeneration || s.uiSourceAcceptedTargetGeneration!=s.uiSourceTargetGeneration)
        return fail(d,VehicleWindowError50DS::frame,"no current accepted source drain for receipt");
    if(!s.ensureUiSourceReceiptLifetime(d))return false;
    receipt.lifetime_=s.uiSourceReceiptLifetime;receipt.frameGeneration_=s.uiSourceFrameGeneration;
    receipt.targetGeneration_=s.uiSourceAcceptedTargetGeneration;receipt.texturePublication_=s.uiSourceAcceptedTexturePublication;
    receipt.terminal_=s.uiSourceTerminal;s.uiSourceLeaseWork=true;
    d.originalUiSourceAccepted=true;d.originalUiSourceFrameGeneration=receipt.frameGeneration_;
    d.originalUiSourceTargetGeneration=receipt.targetGeneration_;return true;
}
bool VehicleWindowGl50DS::submitOriginalUiSourceUnpublished(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,OriginalUiSourceSubmission50KN& receipt,
    VehicleWindowDiagnostic50DS& d) noexcept {
    receipt={};d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if(!s.current(d) || !s.ensureUiSourceReceiptLifetime(d))return false;
    s.uiSourceLeaseWork=true;
    if(!submitOriginalUiSourceFrame(frame,width,height,false,d))return false;
    VehicleWindowDiagnostic50DS receiptDiagnostic;
    if(!originalUiSourceSubmission(receipt,receiptDiagnostic)){d=receiptDiagnostic;return false;}
    return true;
}
bool VehicleWindowGl50DS::completeOriginalUiSource(const OriginalUiSourceSubmission50KN& input,
    OriginalUiSourceSubmission50KN& completed,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(&input==&completed)return fail(d,VehicleWindowError50DS::request,"source completion input and output must be distinct");
    completed={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiSource(input))
        return fail(d,VehicleWindowError50DS::frame,"source completion requires the exact current accepted target");
    s.diagnostic=&d;bool accepted=s.check("before explicit source completion");
    if(accepted){const auto finishStart=Storage::UiCoverageClock::now();
        s.gl.finish();accepted=s.check("complete exact source target work");
        if(s.uiCoverageSourceDiagnostic&&(s.uiCoverageCompletionReports<8||!accepted)){++s.uiCoverageCompletionReports;
            std::fprintf(stderr,"ui_coverage_completion target=%llu finish_wall_ns=%llu accepted=%u scope=existing_completion_wait\n",
                static_cast<unsigned long long>(s.uiSourceTargetGeneration),static_cast<unsigned long long>(Storage::uiCoverageNs(finishStart)),unsigned(accepted));}
    }
    s.diagnostic=nullptr;
    if(accepted){completed=input;completed.completed_=true;d.submittedWorkCompleted=true;
        d.originalUiSourceAccepted=true;d.originalUiSourceFrameGeneration=input.frameGeneration_;
        d.originalUiSourceTargetGeneration=input.targetGeneration_;}
    return accepted;
}
bool VehicleWindowGl50DS::publishOriginalUiSource(const OriginalUiSourceSubmission50KN& source,
    std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !source.completed_ || !matchesOriginalUiSource(source) ||
        width<=0 || height<=0 || width>s.maximumWidth || height>s.maximumHeight ||
        s.uiSourcePublicationGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"source publication requires exact completed target and bounded viewport");
    s.diagnostic=&d;
    bool accepted=s.check("before separate source publication") &&
        (s.request.output==VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    if(accepted)accepted=s.publishUiSource(s.uiSourceClear,width,height);
    s.diagnostic=nullptr;
    d.originalUiSourceAccepted=true;d.originalUiSourceFrameGeneration=source.frameGeneration_;
    d.originalUiSourceTargetGeneration=source.targetGeneration_;
    return accepted;
}
bool VehicleWindowGl50DS::retainPublishedOriginalUiSource(OriginalUiRetainedSource50KN& retained,
    VehicleWindowDiagnostic50DS& d) noexcept {
    retained={};d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || s.uiRetainedTarget || s.uiRetainedColor ||
        !s.uiSourcePublishedTargetGeneration || s.uiSourcePublishedTargetGeneration!=s.uiSourceAcceptedTargetGeneration ||
        s.uiSourcePublishedTargetGeneration!=s.uiSourceTargetGeneration ||
        !s.imageCommitted || s.displayedTarget!=s.uiTargets[s.uiPublished] ||
        s.uiSnapshotSerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"retain requires the current successful source publication and no prior snapshot");
    OriginalUiSourceSubmission50KN source;
    if(!originalUiSourceSubmission(source,d))return false;
    s.diagnostic=&d;bool accepted=s.check("before retaining renderer-published source RGBA");
    if(accepted)accepted=s.createRetainedUiSourceTarget();
    if(accepted){
        // Blit all four raw channels before any later ordinary source clear.
        // Scissor must not crop the source rightmost column or bottom row.
        ++s.uiSnapshotSerial;s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
        s.gl.bindFramebuffer(readFramebuffer,s.uiSourceTarget);s.gl.readBuffer(colorAttachment);
        s.gl.bindFramebuffer(drawFramebuffer,s.uiRetainedTarget);s.gl.drawBuffer(colorAttachment);
        s.gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit,0x2600);
        accepted=s.check("copy immutable retained source RGBA");
    }
    if(accepted){s.gl.finish();accepted=s.check("complete retained source RGBA copy");}
    // Failed attempts keep every unresolved name; release/close owns retirement.
    // No partial snapshot or fabricated completion receipt is published.
    if(accepted){
        s.gl.bindFramebuffer(framebuffer,0);accepted=s.check("finish retained source copy bindings");
    }
    if(accepted){
        s.uiSnapshotGeneration=s.uiSnapshotSerial;s.uiRetainedSourceTargetGeneration=source.targetGeneration_;
        s.uiRetainedPublicationGeneration=s.uiSourcePublicationGeneration;
        source.completed_=true;retained.source_=source;retained.snapshotGeneration_=s.uiSnapshotGeneration;
        retained.publicationGeneration_=s.uiRetainedPublicationGeneration;
        d.submittedWorkCompleted=true;d.originalUiSnapshotGeneration=s.uiSnapshotGeneration;
        d.originalUiPublicationGeneration=s.uiRetainedPublicationGeneration;
    }
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::readRetainedOriginalUiSourceRgba(const OriginalUiRetainedSource50KN& retained,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    const auto lifetime=retained.source_.lifetime_.lock();
    if(s.active || s.frameSet || s.diagnostic || !lifetime || lifetime!=s.uiSourceReceiptLifetime ||
        !retained.snapshotGeneration_ || retained.snapshotGeneration_!=s.uiSnapshotGeneration ||
        retained.publicationGeneration_!=s.uiRetainedPublicationGeneration ||
        retained.source_.targetGeneration_!=s.uiRetainedSourceTargetGeneration ||
        !s.uiRetainedTarget || !s.uiRetainedColor)
        return fail(d,VehicleWindowError50DS::frame,"retained source readback requires its exact live immutable snapshot");
    d.readbackWidth=640;d.readbackHeight=448;d.readbackBytes=std::size_t(640)*448*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"retained RGBA readback requires full640x448 destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.uiRetainedTarget,640,448);s.diagnostic=nullptr;
    d.originalUiSnapshotGeneration=retained.snapshotGeneration_;return accepted;
}
bool VehicleWindowGl50DS::releaseRetainedOriginalUiSource(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || s.uiBlendOldSnapshot)
        return fail(d,VehicleWindowError50DS::frame,"retained source release requires idle renderer without a blend borrow");
    if(!s.uiRetainedTarget && !s.uiRetainedColor)return true;
    s.diagnostic=&d;
    bool accepted=s.check("before retained source retirement");
    if(accepted){s.gl.finish();accepted=s.check("complete retained source work before deletion");}
    if(accepted){
        d.submittedWorkCompleted=true;s.uiSnapshotGeneration=0;
        s.gl.bindFramebuffer(framebuffer,0);s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,0);
        accepted=s.check("unbind retained source before deletion");
        if(accepted)accepted=s.deleteName(s.uiRetainedTarget,s.gl.deleteFramebuffers,d) &&
            s.deleteName(s.uiRetainedColor,s.gl.deleteTextures,d);
        if(!accepted && d.error==VehicleWindowError50DS::none)
            fail(d,VehicleWindowError50DS::cleanup,"retained source deletion requires retry");
        if(accepted){s.uiRetainedSourceTargetGeneration=0;s.uiRetainedPublicationGeneration=0;}
    }
    s.diagnostic=nullptr;return accepted;
}
namespace {
bool exactUiByteBlendTerminal(const OriginalUiSourceTerminal50KN& terminal) noexcept {
    using F=assets::OriginalUiGsField50KG;
    const auto value=[&](F field) noexcept {return terminal.gs[static_cast<std::size_t>(field)];};
    // Authenticated Mat_Stbl first-match tokens3216/3217 are icon_tri/icon_x.
    // Locked Rider appearance draws only triangle (promptMask42C=2); both
    // prompts inherit the same ScreenMT state. Preserve every GS check below.
    // Read the real completed drain's owned fields, including inactive FIX and
    // clamp limits. Effect TEX1=0 is separate from this ordinary sampler state.
    return (terminal.canonicalToken==3216 || terminal.canonicalToken==3217) && terminal.pass==1 &&
        terminal.registryGeneration && terminal.bankGeneration && terminal.enabled==1 &&
        terminal.textureCount==1 && terminal.textureCoordinateIndex==0 &&
        value(F::environmentContext0or1)==0 &&
        value(F::inputColorA)==0 && value(F::inputColorB)==1 && value(F::inputAlphaC)==0 &&
        value(F::inputColorD)==1 && value(F::fixedAlphaValue)==128 &&
        value(F::wrapModeHorizontal)==1 && value(F::wrapModeVertical)==1 &&
        value(F::minClampParameterInUDirection)==0 && value(F::maxClampParameterInUDirection)==0 &&
        value(F::minClampParameterInVDirection)==0 && value(F::maxClampParameterInVDirection)==0 &&
        value(F::colorClampingMethod)==1 && value(F::alphaBlendingControlInUnitsOfPixels)==0 &&
        value(F::alphaCorrectionValue)==0;
}
}
bool VehicleWindowGl50DS::matchesOriginalUiByteBlend(const OriginalUiByteBlend50KO& blend)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    const auto oldLifetime=blend.old_.source_.lifetime_.lock();
    const auto incomingLifetime=blend.incoming_.lifetime_.lock();
    return oldLifetime && oldLifetime==s.uiSourceReceiptLifetime && incomingLifetime==oldLifetime &&
        !s.uiBlendPoisoned && blend.generation_ && blend.generation_==s.uiBlendGeneration &&
        blend.old_.snapshotGeneration_==s.uiBlendOldSnapshot && s.uiBlendOldSnapshot==s.uiSnapshotGeneration &&
        blend.old_.publicationGeneration_==s.uiRetainedPublicationGeneration &&
        blend.old_.source_.targetGeneration_==s.uiRetainedSourceTargetGeneration &&
        blend.incoming_.targetGeneration_==s.uiBlendIncomingSource &&
        s.uiRetainedTarget && s.uiRetainedColor && s.uiBlendIncomingTarget && s.uiBlendIncomingColor &&
        s.uiBlendEffectTarget && s.uiBlendEffectColor && s.uiBlendProgram && s.uiBlendVertexArray;
}
bool VehicleWindowGl50DS::matchesOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO& sample)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    const auto lifetime=sample.lifetime_.lock();
    return lifetime && lifetime==s.uiSourceReceiptLifetime && !s.uiBlendPoisoned &&
        sample.blendGeneration_ && sample.blendGeneration_==s.uiBlendGeneration &&
        sample.sampleGeneration_ && sample.sampleGeneration_==s.uiBlendSampleGeneration &&
        sample.alpha_==s.uiBlendAlpha && s.uiBlendOldSnapshot &&
        s.uiBlendOldSnapshot==s.uiSnapshotGeneration && s.uiBlendEffectTarget && s.uiBlendEffectColor;
}
bool VehicleWindowGl50DS::beginOriginalUiByteBlend(const OriginalUiRetainedSource50KN& oldSource,
    const OriginalUiSourceSubmission50KN& incoming,OriginalUiByteBlend50KO& blend,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(&oldSource==&blend.old_ || &incoming==&blend.incoming_ || &incoming==&blend.old_.source_)
        return fail(d,VehicleWindowError50DS::request,"byte-blend input and output storage must be distinct");
    blend={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    const auto oldLifetime=oldSource.source_.lifetime_.lock();
    if(s.active || s.frameSet || s.diagnostic || s.uiBlendOldSnapshot || s.hasUiByteBlendNames() ||
        !oldLifetime || oldLifetime!=s.uiSourceReceiptLifetime || !oldSource.source_.completed_ ||
        !oldSource.snapshotGeneration_ || oldSource.snapshotGeneration_!=s.uiSnapshotGeneration ||
        oldSource.publicationGeneration_!=s.uiRetainedPublicationGeneration ||
        oldSource.source_.targetGeneration_!=s.uiRetainedSourceTargetGeneration ||
        !s.uiRetainedTarget || !s.uiRetainedColor || !incoming.completed_ || !matchesOriginalUiSource(incoming) ||
        incoming.targetGeneration_<=oldSource.source_.targetGeneration_ ||
        s.uiBlendSerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"byte-blend requires live retained old and newer exact completed incoming");
    if(!exactUiByteBlendTerminal(incoming.terminal_))
        return fail(d,VehicleWindowError50DS::material,"byte-blend requires actual terminal icon_tri/icon_x pass1 and exact inherited GS state");
    s.diagnostic=&d;
    bool accepted=s.check("before immutable byte-blend pair creation");
    if(accepted){
        // This borrow is installed BEFORE any fallible allocation/copy. It
        // survives failure and cannot be replaced until explicit retirement.
        s.uiBlendOldSnapshot=oldSource.snapshotGeneration_;s.uiBlendPoisoned=true;s.uiSourceLeaseWork=true;
        accepted=s.createUiByteBlendTarget(s.uiBlendIncomingTarget,s.uiBlendIncomingColor) &&
            s.createUiByteBlendTarget(s.uiBlendEffectTarget,s.uiBlendEffectColor) && s.createUiByteBlendProgram();
        if(accepted){
            s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
            s.gl.bindFramebuffer(readFramebuffer,s.uiSourceTarget);s.gl.readBuffer(colorAttachment);
            s.gl.bindFramebuffer(drawFramebuffer,s.uiBlendIncomingTarget);s.gl.drawBuffer(colorAttachment);
            s.gl.blitFramebuffer(0,0,640,448,0,0,640,448,colorBufferBit,0x2600);
            accepted=s.check("copy actual completed incoming into private immutable byte-blend image");
        }
        if(accepted){s.gl.finish();accepted=s.check("complete immutable incoming byte-blend copy");}
        const bool restored=s.restoreUiByteBlendBindings();accepted=accepted && restored;
    }
    if(accepted){
        s.uiBlendIncomingSource=incoming.targetGeneration_;s.uiBlendGeneration=++s.uiBlendSerial;
        s.uiBlendSampleGeneration=0;s.uiBlendPoisoned=false;
        for(unsigned i=0;i<4;++i)s.uiBlendClear[i]=s.uiSourceClear[i];
        blend.old_=oldSource;blend.incoming_=incoming;blend.generation_=s.uiBlendGeneration;
        d.submittedWorkCompleted=true;d.originalUiByteBlendGeneration=s.uiBlendGeneration;
    }
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::renderOriginalUiByteBlendSample(const OriginalUiByteBlend50KO& blend,
    std::uint32_t alpha,OriginalUiByteBlendSample50KO& sample,VehicleWindowDiagnostic50DS& d) noexcept {
    sample={};d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || alpha>OriginalUiByteBlendContract50KO::maximumAlpha ||
        !matchesOriginalUiByteBlend(blend) || s.uiBlendSampleSerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"byte-blend sample requires exact live immutable pair and alpha0..128");
    // Invalidate previous sample authority before any attempted write. Failed
    // samples poison the pair until real retirement; never publish partial E.
    s.uiBlendSampleGeneration=0;s.diagnostic=&d;
    bool accepted=s.check("before byte-blend sample");
    if(accepted){
        const auto generation=++s.uiBlendSampleSerial;
        s.gl.bindFramebuffer(framebuffer,s.uiBlendEffectTarget);s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
        if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E);
        s.gl.viewport(0,0,640,448);
        constexpr U disabled[]={0x0BE2,0x0B44,0x0C11,0x0B90,0x8037,0x0B20,
            0x8F9D,0x0BD0,0x809D,0x80A0,0x809E,0x809F,0x8E51,0x8C89,0x8DB9,0x864F,0x0BF2,0x0B71};
        for(U capability:disabled)s.gl.disable(capability);
        for(I i=0;i<s.clipDistances;++i)s.gl.disable(0x3000+static_cast<U>(i));
        s.gl.colorMask(1,1,1,1);s.gl.depthMask(0);s.gl.polygonMode(0x0408,0x1B02);
        s.resetNativeGcLighting();
        s.gl.useProgram(s.uiBlendProgram);s.drawGl.bindVertexArray(s.uiBlendVertexArray);
        s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,s.uiRetainedColor);
        if(s.gl.bindSampler)s.gl.bindSampler(0,0);
        s.gl.activeTexture(textureUnit0+1);s.gl.bindTexture(texture2D,s.uiBlendIncomingColor);
        if(s.gl.bindSampler)s.gl.bindSampler(1,0);
        s.gl.uniform1i(s.uiBlendOldUniform,0);s.gl.uniform1i(s.uiBlendIncomingUniform,1);
        s.gl.uniform1i(s.uiBlendAlphaUniform,static_cast<I>(alpha));
        accepted=s.check("bind immutable inputs and exact byte-blend state");
        if(accepted){s.gl.drawArrays(0x0004,0,3);accepted=s.check("draw documented byte-blend sample");}
        const bool restored=s.restoreUiByteBlendBindings();accepted=accepted && restored;
        if(accepted){
            s.uiBlendSampleGeneration=generation;s.uiBlendAlpha=alpha;
            sample.lifetime_=s.uiSourceReceiptLifetime;sample.blendGeneration_=s.uiBlendGeneration;
            sample.sampleGeneration_=generation;sample.alpha_=alpha;
            d.originalUiByteBlendAccepted=true;d.originalUiByteBlendGeneration=s.uiBlendGeneration;
            d.originalUiByteBlendSampleGeneration=generation;
        }
    }
    if(!accepted)s.uiBlendPoisoned=true;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::completeOriginalUiByteBlendSample(const OriginalUiByteBlendSample50KO& input,
    OriginalUiByteBlendSample50KO& completed,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(&input==&completed)return fail(d,VehicleWindowError50DS::request,"byte-blend completion input and output must be distinct");
    completed={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiByteBlendSample(input))
        return fail(d,VehicleWindowError50DS::frame,"byte-blend completion requires exact accepted sample");
    s.diagnostic=&d;bool accepted=s.check("before byte-blend sample completion");
    if(accepted){s.gl.finish();accepted=s.check("complete byte-blend sample GPU work");}
    if(accepted){
        completed=input;completed.completed_=true;d.submittedWorkCompleted=true;d.originalUiByteBlendAccepted=true;
        d.originalUiByteBlendGeneration=input.blendGeneration_;d.originalUiByteBlendSampleGeneration=input.sampleGeneration_;
    }else{s.uiBlendPoisoned=true;s.uiBlendSampleGeneration=0;}
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::readOriginalUiByteBlendSampleRgba(const OriginalUiByteBlendSample50KO& sample,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !sample.completed_ || !matchesOriginalUiByteBlendSample(sample))
        return fail(d,VehicleWindowError50DS::frame,"byte-blend readback requires exact current completed sample");
    d.readbackWidth=640;d.readbackHeight=448;d.readbackBytes=std::size_t(640)*448*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"byte-blend readback requires full640x448 RGBA destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.uiBlendEffectTarget,640,448);s.diagnostic=nullptr;
    d.originalUiByteBlendGeneration=sample.blendGeneration_;d.originalUiByteBlendSampleGeneration=sample.sampleGeneration_;
    return accepted;
}
bool VehicleWindowGl50DS::matchesOriginalUiPrivatePublication(const OriginalUiPrivatePublication50KP& publication)const noexcept {
    if(!storage_)return false;const auto& s=*storage_;
    const auto lifetime=publication.lifetime_.lock();
    return lifetime && lifetime==s.uiSourceReceiptLifetime && publication.generation_ &&
        publication.generation_==s.uiPrivatePublicationGeneration && s.imageCommitted &&
        s.displayedTarget==s.uiTargets[s.uiPublished] &&
        publication.width_==s.committedWidth && publication.height_==s.committedHeight;
}
bool VehicleWindowGl50DS::publishOriginalUiBlendImage(const OriginalUiByteBlend50KO& blend,
    const OriginalUiByteBlendSample50KO* sample,std::int32_t width,std::int32_t height,
    OriginalUiPrivatePublication50KP& publication,VehicleWindowDiagnostic50DS& d) noexcept {
    publication={};d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiByteBlend(blend) ||
        width<=0 || height<=0 || width>s.maximumWidth || height>s.maximumHeight ||
        s.uiPrivatePublicationSerial==std::numeric_limits<std::uint64_t>::max() ||
        (sample && (!sample->completed_ || !matchesOriginalUiByteBlendSample(*sample))) ||
        (!sample && (s.uiPromotedIncomingTarget || s.uiPromotedIncomingColor)))
        return fail(d,VehicleWindowError50DS::frame,"private publication requires live immutable pair, completed sample or unowned promotion, and bounded viewport");
    const auto pending=static_cast<std::uint8_t>(s.uiPublished^1u);
    s.diagnostic=&d;
    bool accepted=s.check("before private immutable-image publication");
    if(accepted)accepted=s.copyUiSourceToOpaque(sample?s.uiBlendEffectTarget:s.uiBlendIncomingTarget,
        s.uiBlendClear,width,height);
    // The pending surface is independent of the last committed image. Failed
    // copies neither transfer incoming ownership nor invalidate that prior
    // image. Inputs remain immutable and an explicit retry can overwrite the
    // whole pending surface. No default framebuffer is checked or written.
    if(accepted){
        s.uiPublished=pending;s.displayedTarget=s.uiTargets[pending];
        s.width=s.committedWidth=width;s.height=s.committedHeight=height;s.imageCommitted=true;
        s.uiSourcePublishedTargetGeneration=0;
        s.uiPrivatePublicationGeneration=++s.uiPrivatePublicationSerial;
        publication.lifetime_=s.uiSourceReceiptLifetime;
        publication.kind_=sample?OriginalUiPrivatePublicationKind50KP::byteBlendSample:
            OriginalUiPrivatePublicationKind50KP::immutableIncoming;
        publication.generation_=s.uiPrivatePublicationGeneration;publication.blendGeneration_=blend.generation_;
        publication.oldSnapshotGeneration_=blend.old_.snapshotGeneration_;
        publication.incomingFrameGeneration_=blend.incoming_.frameGeneration_;
        publication.incomingTargetGeneration_=blend.incoming_.targetGeneration_;
        publication.width_=width;publication.height_=height;
        if(sample){publication.sampleGeneration_=sample->sampleGeneration_;publication.alpha_=sample->alpha_;}
        d.originalUiPrivatePublicationAccepted=true;d.originalUiPrivatePublicationGeneration=publication.generation_;
        d.originalUiByteBlendGeneration=publication.blendGeneration_;
        d.originalUiByteBlendSampleGeneration=publication.sampleGeneration_;
    }
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::publishOriginalUiByteBlendSample(const OriginalUiByteBlend50KO& blend,
    const OriginalUiByteBlendSample50KO& sample,std::int32_t width,std::int32_t height,
    OriginalUiPrivatePublication50KP& publication,VehicleWindowDiagnostic50DS& d) noexcept {
    return publishOriginalUiBlendImage(blend,&sample,width,height,publication,d);
}
bool VehicleWindowGl50DS::promoteOriginalUiByteBlendIncoming(const OriginalUiByteBlend50KO& blend,
    std::int32_t width,std::int32_t height,OriginalUiPrivatePublication50KP& publication,
    OriginalUiPromotedIncoming50KP& incoming,VehicleWindowDiagnostic50DS& d) noexcept {
    incoming={};
    if(!publishOriginalUiBlendImage(blend,nullptr,width,height,publication,d))return false;
    auto& s=*storage_;
    // Nothing fallible follows the successful private copy: this is a CPU
    // ownership move of the already completed immutable incoming names, not a
    // new GPU copy, source submission, completion barrier or host presentation.
    s.uiPromotedIncomingTarget=s.uiBlendIncomingTarget;s.uiBlendIncomingTarget=0;
    s.uiPromotedIncomingColor=s.uiBlendIncomingColor;s.uiBlendIncomingColor=0;
    s.uiPromotedIncomingGeneration=blend.generation_;
    incoming.source_=blend.incoming_;incoming.generation_=s.uiPromotedIncomingGeneration;
    // No sample or promotion retry is legal after transfer. The old borrow and
    // remaining effect objects stay owned until explicit blend retirement.
    s.uiBlendGeneration=s.uiBlendSampleGeneration=0;s.uiBlendPoisoned=true;
    d.originalUiPromotedIncomingGeneration=incoming.generation_;return true;
}
bool VehicleWindowGl50DS::completeOriginalUiPrivatePublication(const OriginalUiPrivatePublication50KP& input,
    OriginalUiPrivatePublication50KP& completed,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(&input==&completed)return fail(d,VehicleWindowError50DS::request,"publication completion input and output must be distinct");
    completed={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !matchesOriginalUiPrivatePublication(input))
        return fail(d,VehicleWindowError50DS::frame,"publication completion requires exact current private image");
    s.diagnostic=&d;bool accepted=s.check("before private publication completion");
    if(accepted){s.gl.finish();accepted=s.check("complete private opaque publication GPU work");}
    if(accepted){
        completed=input;completed.completed_=true;d.submittedWorkCompleted=true;
        d.originalUiPrivatePublicationAccepted=true;d.originalUiPrivatePublicationGeneration=input.generation_;
        d.originalUiByteBlendGeneration=input.blendGeneration_;
        d.originalUiByteBlendSampleGeneration=input.sampleGeneration_;
    }else s.uiPrivatePublicationGeneration=0;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::copyOriginalUiPrivatePublicationToWindow(const OriginalUiPrivatePublication50KP& publication,
    OriginalUiWindowBackBufferCopy& copied,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(&publication==&copied.publication_)
        return fail(d,VehicleWindowError50DS::request,"window copy input and output must be distinct");
    copied={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic ||
        s.request.output!=VehicleWindowOutput50DS::defaultBackBuffer ||
        !publication.completed_ || !matchesOriginalUiPrivatePublication(publication) ||
        publication.width_<=0 || publication.height_<=0 ||
        publication.width_>s.maximumWidth || publication.height_>s.maximumHeight ||
        s.uiWindowBackBufferCopySerial==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,
            "window copy requires idle window renderer, exact completed private image, bounded viewport and copy sequence");
    s.diagnostic=&d;
    bool accepted=s.check("before private publication window copy") && s.checkDefaultFramebuffer();
    if(accepted){
        // Working frame dimensions may differ after a failed later submit.
        // Copy only the authenticated selected image's exact recorded extent,
        // without changing working dimensions or any private image ownership.
        // The helper checks restoration of framebuffer0 before it succeeds.
        accepted=s.copyToWindow(publication.width_,publication.height_);
        if(!accepted && d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::presentation;
    }
    if(accepted){
        copied.lifetime_=s.uiSourceReceiptLifetime;copied.publication_=publication;
        copied.sequence_=++s.uiWindowBackBufferCopySerial;
        copied.kind_=OriginalUiWindowBackBufferCopyKind::windowBack;copied.commandsAccepted_=true;
    }
    // Failed blits preserve the selected private image and its completion.
    // The helper retains possible partial BACK modification in d; no receipt
    // or completion fence is issued and this renderer never swaps the window.
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::readOriginalUiPrivatePublicationRgba(const OriginalUiPrivatePublication50KP& publication,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic || !publication.completed_ || !matchesOriginalUiPrivatePublication(publication))
        return fail(d,VehicleWindowError50DS::frame,"private readback requires exact current completed publication");
    d.readbackWidth=publication.width_;d.readbackHeight=publication.height_;
    d.readbackBytes=std::size_t(publication.width_)*std::size_t(publication.height_)*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"private publication readback requires full viewport RGBA destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.displayedTarget,publication.width_,publication.height_);
    s.diagnostic=nullptr;d.originalUiPrivatePublicationGeneration=publication.generation_;
    d.originalUiByteBlendGeneration=publication.blendGeneration_;
    d.originalUiByteBlendSampleGeneration=publication.sampleGeneration_;return accepted;
}
bool VehicleWindowGl50DS::readPromotedOriginalUiIncomingRgba(const OriginalUiPromotedIncoming50KP& incoming,
    std::uint8_t* rgba,std::size_t capacityBytes,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    const auto lifetime=incoming.source_.lifetime_.lock();
    if(s.active || s.frameSet || s.diagnostic || !lifetime || lifetime!=s.uiSourceReceiptLifetime ||
        !incoming.generation_ || incoming.generation_!=s.uiPromotedIncomingGeneration ||
        !s.uiPromotedIncomingTarget || !s.uiPromotedIncomingColor)
        return fail(d,VehicleWindowError50DS::frame,"promoted incoming readback requires its exact immutable raw owner");
    d.readbackWidth=640;d.readbackHeight=448;d.readbackBytes=std::size_t(640)*448*4;
    if(!rgba || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"promoted incoming readback requires full640x448 RGBA destination");
    s.diagnostic=&d;const bool accepted=s.copyRgba(rgba,s.uiPromotedIncomingTarget,640,448);s.diagnostic=nullptr;
    d.originalUiPromotedIncomingGeneration=incoming.generation_;return accepted;
}
bool VehicleWindowGl50DS::releasePromotedOriginalUiIncoming(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"promoted incoming retirement requires idle renderer");
    if(!s.uiPromotedIncomingTarget && !s.uiPromotedIncomingColor)return true;
    s.diagnostic=&d;bool accepted=s.check("before promoted incoming retirement");
    if(accepted){s.gl.finish();accepted=s.check("complete promoted incoming reads before retirement");}
    if(accepted){
        d.submittedWorkCompleted=true;s.uiPromotedIncomingGeneration=0;
        accepted=s.restoreUiByteBlendBindings();
        if(accepted)accepted=s.deleteName(s.uiPromotedIncomingTarget,s.gl.deleteFramebuffers,d) &&
            s.deleteName(s.uiPromotedIncomingColor,s.gl.deleteTextures,d);
        if(!accepted && d.error==VehicleWindowError50DS::none)
            fail(d,VehicleWindowError50DS::cleanup,"promoted incoming name retirement requires retry");
    }
    if(!accepted)d.cleanupIncomplete=true;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::releaseOriginalUiByteBlend(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;if(!s.current(d))return false;
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"byte-blend retirement requires idle renderer");
    if(!s.uiBlendOldSnapshot && !s.hasUiByteBlendNames())return true;
    s.diagnostic=&d;bool accepted=s.check("before byte-blend retirement");
    if(accepted){s.gl.finish();accepted=s.check("complete all byte-blend work before retirement");}
    if(accepted){
        d.submittedWorkCompleted=true;s.uiBlendGeneration=s.uiBlendSampleGeneration=0;s.uiBlendPoisoned=true;
        accepted=s.restoreUiByteBlendBindings();
        if(accepted)accepted=s.deleteUiByteBlendNames(d);
        if(!accepted && d.error==VehicleWindowError50DS::none)
            fail(d,VehicleWindowError50DS::cleanup,"byte-blend name retirement requires retry");
        if(accepted){s.uiBlendOldSnapshot=s.uiBlendIncomingSource=0;s.uiBlendPoisoned=false;}
    }
    if(!accepted)d.cleanupIncomplete=true;
    s.diagnostic=nullptr;return accepted;
}
bool VehicleWindowGl50DS::applyGarageFogColor244A40(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication,std::uint32_t argb,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!originalUiSourceRgbaAvailable()||!ownsGarageSourcePrelude50KG(owner,publication))
        return fail(d,VehicleWindowError50DS::context,"Garage Fog command requires exact live source-target owner");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.nativeGlFaultObserved50KG||!s.uiProgram||s.uiParticleFogColorUniform<0)
        return fail(d,VehicleWindowError50DS::frame,"Garage Fog command requires current idle source renderer");
    s.diagnostic=&d;s.gl.useProgram(s.uiProgram);
    //Native equivalent of the single13EBBC GSFOGCOL write. Apply the actual
    //retained source-RGBA shader state in command order; do not draw/present.
    s.gl.uniform4f(s.uiParticleFogColorUniform,float((argb>>16)&255u)/255.f,
        float((argb>>8)&255u)/255.f,float(argb&255u)/255.f,1.f);
    const bool accepted=s.check("apply actual Garage Fog GSFOGCOL command");
    s.diagnostic=nullptr;if(!accepted)s.garagePreludeFailed50KG=true;return accepted;
}
bool VehicleWindowGl50DS::clearGarageSourceViewport50KG(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication,bool color,std::uint32_t argb,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};
    if(!publication||!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"Garage source clear requires actual source target and publication");
    auto& s=*storage_;if(!s.current(d))return false;
    if(!s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.nativeGlFaultObserved50KG||
       s.garagePreludeFailed50KG||(s.garagePreludeOwner50KG&&
       (s.garagePreludeOwner50KG!=&owner||s.garagePreludePublication50KG!=publication))||
       (s.uiSourceFrameOwner&&s.uiSourceFrameOwner!=&owner))
        return fail(d,VehicleWindowError50DS::frame,"Garage source clear requires idle exact source-target ownership");
    const bool first=!s.garagePreludeOwner50KG;
    if(first&&s.uiSourceTargetGeneration==std::numeric_limits<std::uint64_t>::max())
        return fail(d,VehicleWindowError50DS::frame,"Garage source target generation exhausted");
    s.garagePreludeOwner50KG=&owner;s.garagePreludePublication50KG=publication;
    if(first)++s.uiSourceTargetGeneration;
    s.uiSourceAcceptedTargetGeneration=0;s.uiSourcePublishedTargetGeneration=0;
    s.diagnostic=&d;
    //Actual full640x448 source viewport.2458C0's sprite has ATE=NEVER,
    //AFAIL=ZB_ONLY,Z=0. This real D24 clear has no color/image-publication effect.
    s.gl.bindFramebuffer(framebuffer,s.uiSourceTarget);s.gl.viewport(0,0,640,448);
    s.gl.disable(0x0C11);s.gl.depthMask(1);s.gl.clearDepth(0.0);
    U mask=depthBufferBit;
    if(color){
        s.gl.colorMask(1,1,1,1);
        //25C0A0 emits alpha128 regardless of the incoming ARGB high byte.
        s.gl.clearColor(float((argb>>16)&255u)/255.f,float((argb>>8)&255u)/255.f,
            float(argb&255u)/255.f,128.f/255.f);mask|=colorBufferBit;
    }
    s.gl.clear(mask);
    const bool accepted=s.check("apply actual Garage full-viewport source clear");
    if(accepted&&color){s.uiSourceClear[0]=float((argb>>16)&255u)/255.f;
        s.uiSourceClear[1]=float((argb>>8)&255u)/255.f;s.uiSourceClear[2]=float(argb&255u)/255.f;
        s.uiSourceClear[3]=1.f;} //Actual clear provenance for later opaque target copy.
    s.diagnostic=nullptr;s.garagePreludeFailed50KG=!accepted;
    //No source frame, source alpha receipt, image acceptance or timing commit.
    return accepted;
}
bool VehicleWindowGl50DS::ownsGarageSourcePrelude50KG(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication)const noexcept {
    return storage_&&storage_->garagePreludeOwner50KG==&owner&&storage_->garagePreludePublication50KG==publication;
}
bool VehicleWindowGl50DS::garageUnsealedRenderQuiescent50KG(
    const OriginalUiGarageDrawContribution50KG& draw,std::uint64_t publication)const noexcept {
    if(!storage_||!publication||draw.publication_!=publication||
       !draw.frontendTextureRendererCurrent(*this)||!originalUiSourceRgbaAvailable())return false;
    const auto& s=*storage_;VehicleWindowDiagnostic50DS observed;
    if(!s.current(observed)||!s.frontendOnly||s.active||s.frameSet||s.diagnostic||
       s.garagePollFence50KG||s.nativeGlFaultObserved50KG||s.garagePreludeFailed50KG||
       s.uiSourceFrameOwner||s.uiSourceFrameGeneration||s.garageCompletedPublication50KG||
       s.garageCompletedTarget50KG||s.garagePublishedPublication50KG)return false;
    // A prior prelude may not have started, or may already be truly released
    // on this native retry. A live one must be this exact Draw/publication.
    // Poll fences and observed GL faults are independent owners/history and
    // cannot be discharged by this read-only observation or inferred finish.
    return s.garagePreludeOwner50KG?
        s.garagePreludeOwner50KG==&draw&&s.garagePreludePublication50KG==publication:
        s.garagePreludePublication50KG==0;
}
bool VehicleWindowGl50DS::releaseGarageSourcePrelude50KG(const OriginalUiSourceOwner50KM& owner,
    std::uint64_t publication,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!storage_)return false;auto& s=*storage_;
    if(!s.garagePreludeOwner50KG)return true;
    if(s.garagePreludeOwner50KG!=&owner||s.garagePreludePublication50KG!=publication||
       s.active||s.frameSet||s.diagnostic||!s.current(d))return false;
    //Retire only this native source-target work. Failed GL completion retains
    //the exact owner; it cannot be hidden by dropping the SourceOwner pointer.
    s.diagnostic=&d;bool okay=s.check("before Garage source prelude retirement");
    if(okay){s.gl.finish();okay=s.check("retire actual Garage source-target work");}
    s.diagnostic=nullptr;
    if(okay){s.garagePreludeOwner50KG=nullptr;s.garagePreludePublication50KG=0;s.garagePreludeFailed50KG=false;
        s.garageCompletedPublication50KG=0;s.garageCompletedTarget50KG=0;s.garagePublishedPublication50KG=0;}
    return okay;
}
bool VehicleWindowGl50DS::pollOriginalRenderer245030(bool& complete,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};complete=false;
    if(!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"Garage poll requires source RGBA renderer");
    auto& s=*storage_;if(!s.current(d))return false;
    if(!s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.garagePollFence50KG||s.nativeGlFaultObserved50KG)
        return fail(d,VehicleWindowError50DS::frame,"Garage poll requires idle current frontend without a failed poll");
    s.diagnostic=&d;
    const bool loaded=(s.gl.fenceSync||s.load(s.gl.fenceSync,"glFenceSync"))&&
        (s.gl.getSynciv||s.load(s.gl.getSynciv,"glGetSynciv"))&&
        (s.gl.isSync||s.load(s.gl.isSync,"glIsSync"))&&
        (s.gl.deleteSync||s.load(s.gl.deleteSync,"glDeleteSync"));
    if(!loaded||!s.check("before Garage nonblocking renderer poll")){s.diagnostic=nullptr;return false;}
    // Observe the real context's preceding native work. This inserts no draw,
    // flush, wait, source timing sample or completion receipt. The sync object
    // is a native observation marker, not a reconstruction of guest DMA pools.
    s.garagePollFence50KG=s.gl.fenceSync(0x9117u,0u); //SYNC_GPU_COMMANDS_COMPLETE
    if(!s.check("create Garage native poll marker")||!s.garagePollFence50KG){
        if(d.error==VehicleWindowError50DS::none)fail(d,VehicleWindowError50DS::gl,"Garage native poll marker is null");
        s.diagnostic=nullptr;return false;
    }
    I length=0,status=0;s.gl.getSynciv(s.garagePollFence50KG,0x9114u,1,&length,&status); //SYNC_STATUS
    if(!s.check("observe Garage native completion without waiting")||length!=1||
       (status!=0x9118&&status!=0x9119)){
        if(d.error==VehicleWindowError50DS::none)fail(d,VehicleWindowError50DS::gl,"Garage native poll status is unsupported");
        s.diagnostic=nullptr;return false;
    }
    s.gl.deleteSync(s.garagePollFence50KG);
    if(!s.check("release Garage native poll marker")){s.diagnostic=nullptr;return false;}
    s.garagePollFence50KG=nullptr;s.diagnostic=nullptr;
    complete=status==0x9119;return true;
}
bool VehicleWindowGl50DS::completeGarageSourceTarget140A38(
    OriginalUiGarageDrawContribution50KG& draw,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};
    if(!originalUiSourceRgbaAvailable()||!draw.identityCurrent()||
       draw.phase_!=decltype(draw.phase_)::finalPass||draw.sourcePC_!=0x140A38||
       !draw.finalPassReturned140844_||!draw.rendererEndReturned2442CC_||
       !draw.terminalQueue105990Current()||draw.targetCompleted140A38_||
       !ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"Garage target requires the actual complete Root render and retired FIFO");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.garageCompletedPublication50KG||!s.uiSourceTarget||!s.uiSourceTargetGeneration)
        return fail(d,VehicleWindowError50DS::frame,"Garage target completion requires the same idle actual context");
    s.diagnostic=&d;s.gl.finish();const bool okay=s.check("complete actual Garage Root render target");
    if(okay){s.garageCompletedPublication50KG=draw.publication_;s.garageCompletedTarget50KG=s.uiSourceTargetGeneration;}
    else s.garagePreludeFailed50KG=true;
    d.submittedWorkCompleted=okay;s.diagnostic=nullptr;return okay;
}
bool VehicleWindowGl50DS::publishGarageSourceTarget140A68(
    OriginalUiGarageDrawContribution50KG& draw,std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!originalUiSourceRgbaAvailable()||!draw.issuer_||!draw.renderReturned140230Current(*draw.issuer_)||
       !draw.targetPublishAttempted_||draw.targetPublished140A68_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"Garage publication requires this actual completed Root target");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.garageCompletedPublication50KG!=draw.publication_||s.garageCompletedTarget50KG!=s.uiSourceTargetGeneration||
       s.garagePublishedPublication50KG||width<=0||height<=0)
        return fail(d,VehicleWindowError50DS::frame,"Garage target publication is stale or already attempted");
    s.diagnostic=&d;
    //Copy the genuine completed source-RGBA target. Do not replay any packets,
    //make an empty SourceFrame, or manufacture a generic byte-blend terminal.
    const bool okay=s.publishUiSource(s.uiSourceClear,width,height);
    if(okay)s.garagePublishedPublication50KG=draw.publication_;
    else s.garagePreludeFailed50KG=true;
    s.diagnostic=nullptr;return okay;
}
bool VehicleWindowGl50DS::rearmGarageSourceTarget140230(
    OriginalUiGarageDrawContribution50KG& draw,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!storage_||!draw.issuer_||!draw.renderReturned140230Current(*draw.issuer_)||
       !draw.targetPublished140A68_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"Garage rearm requires its published completed target");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG||
       s.garageCompletedPublication50KG!=draw.publication_||s.garagePublishedPublication50KG!=draw.publication_||
       s.garageCompletedTarget50KG!=s.uiSourceTargetGeneration||!draw.drainGpu_||
       s.request.originalUiTextures!=draw.drainGpu_.get()||s.uiTexturePublication!=draw.drainGpu_->publicationToken())
        return fail(d,VehicleWindowError50DS::frame,"Garage prior target or actual bank lifetime changed before rearm");
    //Consume only this native invocation's target lease. The published image
    //and its backing survive; next source clear mints the actual next target
    //generation. No GL bank, source history, FIFO epoch or Camera is reset.
    s.garagePreludeOwner50KG=nullptr;s.garagePreludePublication50KG=0;
    s.garageCompletedPublication50KG=0;s.garageCompletedTarget50KG=0;s.garagePublishedPublication50KG=0;
    return true;
}
bool VehicleWindowGl50DS::retireOriginalUiSourceOwner(
    const OriginalUiSourceOwner50KM& owner,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if (!originalUiSourceRgbaAvailable())
        return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if (!s.current(d))return false;
    if (s.active || s.frameSet || s.diagnostic ||
        (s.uiSourceFrameOwner && s.uiSourceFrameOwner!=&owner))
        return fail(d,VehicleWindowError50DS::frame,"source UI retirement requires idle matching live owner");
    s.diagnostic=&d;
    bool accepted=s.check("before explicit source UI owner retirement");
    if (accepted){
        s.gl.finish();
        accepted=s.check("drain prior GL work before source UI owner retirement");
    }
    d.submittedWorkCompleted=accepted;
    if (accepted){s.uiSourceFrameOwner=nullptr;s.uiSourceFrameGeneration=0;
        s.uiSourceAcceptedTargetGeneration=0;s.uiSourcePublishedTargetGeneration=0;}
    s.diagnostic=nullptr;
    return accepted;
}
bool VehicleWindowGl50DS::completeGarageCollector24C160(OriginalUiGarageDrawContribution50KG& draw,
    VehicleWindowDiagnostic50DS& d)noexcept {
    d={};if(!initialized()||!draw.identityCurrent()||draw.phase_!=decltype(draw.phase_)::collectorDrain24B7B0||
       draw.sourcePC_!=0x105990u||!draw.drainContextCaptured_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"actual completed Garage leaf traversal required");
    const auto fifo=draw.fifoForBackendDrain();
    if(!fifo||!fifo->draining_||fifo->epoch04_!=draw.drainEpoch_||draw.drainCursor_!=draw.drainOrder_.size()||
       draw.drainOrder_.size()!=fifo->records_.size()||draw.drainBatchPending_||draw.drainBatchFailed_||
       (!fifo->records_.empty()&&!draw.drainBatchSubmitted_))return fail(d,VehicleWindowError50DS::frame,"actual FIFO epoch and all accepted leaves");
    auto& s=*storage_;if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG)return false;
    if(!s.gl.garageFlush24C160){s.diagnostic=&d;const bool loaded=s.load(s.gl.garageFlush24C160,"glFlush");s.diagnostic=nullptr;
        if(!loaded)return false;}
    //All retained primitive bytes have been copied to actual GL-owned buffers.
    //Only now perform source105990 and24B954. A later real command error
    //retains this completed prefix/epoch, never reconstructs discarded records.
    ::original_main_entry50KG::NativeCollectorFifo50LA::CompletedLowering receipt(*fifo);
    if(!fifo->commitActualDrain(receipt))return fail(d,VehicleWindowError50DS::frame,"actual105990 FIFO release rejected");
    draw.sourcePC_=0x24C160;
    //24C274/278 emits GS TEXFLUSH(1). Native image writes and draws use the
    //same current ordered GL context, which owns texture coherence; submit
    //that real command stream without a fabricated guest packet or GPU wait.
    s.diagnostic=&d;s.gl.garageFlush24C160();const bool okay=s.check("actual Garage ordered texture command tail");s.diagnostic=nullptr;
    if(!okay)s.garagePreludeFailed50KG=true;
    return okay;
}
bool VehicleWindowGl50DS::submitGarageCollectorBatch24B26C(
    OriginalUiGarageDrawContribution50KG& draw,VehicleWindowDiagnostic50DS& d)noexcept {
    d={};
    if(!initialized()||!draw.identityCurrent()||
       draw.phase_!=decltype(draw.phase_)::collectorDrain24B7B0||draw.sourcePC_!=0x24B26Cu||
       !draw.drainBatchPending_||draw.drainBatchSubmitted_||draw.drainBatchFailed_||
       !draw.drainBatchCanonical_||!ownsGarageSourcePrelude50KG(draw,draw.publication_))
        return fail(d,VehicleWindowError50DS::frame,"actual retained Garage collector batch required");
    if(draw.drainParticleGeometry_&&!draw.drainParticleRecordCurrent())
        return fail(d,VehicleWindowError50DS::frame,"actual pinned particle record required");
    auto& s=*storage_;
    if(!s.current(d)||s.active||s.frameSet||s.diagnostic||s.garagePreludeFailed50KG)
        return fail(d,VehicleWindowError50DS::material,"actual Garage batch binding transaction rejected");
    // One-time native resource preparation, before this real canonical transaction.
    // No frame creates additional sampler names once this bounded owner is published.
    s.diagnostic=&d;const bool samplers=s.ensureGarageSamplers50KG();s.diagnostic=nullptr;
    if(!samplers)return false;
    if(!draw.drainBatchCanonical_->beginGarageBatch(draw,*this))
        return fail(d,VehicleWindowError50DS::material,"actual Garage batch binding transaction rejected");
    bool started=false;
    bool accepted=false;
    if(draw.drainModelClippedEmptyCurrent()||draw.drainParticleClippedEmptyCurrent()){
        //The exact queued numeric owner processed every source triangle and
        //returned clipped-empty. GPU image upload and canonical bindings are
        //real; no raster command exists, so do not manufacture a one-quad draw.
        const auto& input=draw.drainBatchNative_.draw;
        accepted=input.quadCount==0&&input.spanCount==0&&input.textures==s.request.originalUiTextures&&
            input.texturePublication==s.uiTexturePublication&&input.textures&&
            input.textures->validBorrow(s.request.context,s.request.isCurrent,s.uiTexturePublication);
    }else accepted=submitOriginalUiImpl(draw.drainBatchNative_,640,448,true,d,&started,false,false,nullptr,true);
    const bool committed=draw.drainBatchCanonical_->finish(draw.publication_,accepted,started);
    //No SourceOwner begin/finish, SourceFrame acceptance, publication or swap.
    //A failure retains the real FIFO records and blocks replay of this batch.
    draw.drainBatchPending_=false;draw.drainBatchSubmitted_=accepted&&committed;
    draw.drainBatchFailed_=!draw.drainBatchSubmitted_;
    if(draw.drainBatchFailed_)s.garagePreludeFailed50KG=true;
    if(draw.drainBatchFailed_){
        std::fprintf(stderr,"original_garage_native_batch_rejected publication=%llu epoch=%u cursor=%zu record=%zu accepted=%u started=%u committed=%u renderer_error=%u operation=%.256s gl_error=%u cleanup_gl_error=%u batch_quads=%u batch_spans=%u ui_draws=%u ui_quads=%u ui_triangles=%u prelude_failed=%u\n",
            static_cast<unsigned long long>(draw.publication_),draw.drainEpoch_,draw.drainCursor_,draw.drainActiveRecordOrdinal_,
            accepted?1u:0u,started?1u:0u,committed?1u:0u,static_cast<unsigned>(d.error),d.operation?d.operation:"",
            d.glError,d.cleanupGlError,draw.drainBatchNative_.draw.quadCount,draw.drainBatchNative_.draw.spanCount,
            d.originalUiDraws,d.originalUiQuads,d.originalUiSourceTriangles,s.garagePreludeFailed50KG?1u:0u);
    }
    d.originalUiSourceAccepted=false;d.originalUiAccepted=false;
    return draw.drainBatchSubmitted_;
}

bool VehicleWindowGl50DS::submitOriginalUiImpl(const OriginalUiNativeDraw50KG& native,
    std::int32_t width,std::int32_t height,bool sourceRgba,VehicleWindowDiagnostic50DS& d,
    bool* sourceWorkStarted,bool publishSource,bool riderReceiptChecked,const OriginalUiDirectVideo50KG* directVideo,bool garageBatch) noexcept {
    d = {};
    if(sourceWorkStarted)*sourceWorkStarted=false;
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_; const auto& input = native.draw;
    if(garageBatch&&(!sourceRgba||publishSource||directVideo||!s.garagePreludeOwner50KG||s.garagePreludeFailed50KG))
        return fail(d,VehicleWindowError50DS::frame,"Garage batch requires retained unaccepted source target");
    if(s.garagePreludeOwner50KG&&!sourceRgba)
        return fail(d,VehicleWindowError50DS::frame,"ordinary UI draw cannot interleave with retained Garage source work");
    const auto submitStageStart=Storage::UiCoverageClock::now();
    std::uint64_t preDrawDiagnosticNs=0,drawDiagnosticNs=0,resolveDiagnosticNs=0,publishDiagnosticNs=0;
    U diagnosticPrefixDraws=0,diagnosticMixedDraws=0,diagnosticEmptyAa1=0;
    bool diagnosticPromoted=false;std::uint64_t diagnosticPromotionNs=0;
    if(input.textures&&input.textures->riderSparse()&&(!sourceRgba||!riderReceiptChecked||s.riderTextureBorrow!=input.textures))
        return fail(d,VehicleWindowError50DS::material,"sparse Rider draw requires actual sealed complete-frame residency receipt");
    d.originalUiCoherentFramebufferFetch=sourceRgba && s.uiSourceCoherentFramebufferFetch;
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableOriginalUi || !s.uiProgram ||
        !s.uiVertexBuffer || !s.uiIndexBuffer || !s.uiVertexArray || !s.uiParticleFogBuffer || !s.uiSourceRasterWBuffer || !s.uiSourcePacketStqBuffer ||
        !s.uiTargets[0] || !s.uiTargets[1] ||
        width <= 0 || height <= 0 || width > s.maximumWidth || height > s.maximumHeight ||
        input.canvasWidth != 640.f || input.canvasHeight != 448.f ||
        !input.quadCount || input.quadCount > (sourceRgba?originalUiMaximumSourceQuads50KM:originalUiMaximumQuads50KG) || !input.vertices ||
        !input.spanCount || input.spanCount > originalUiMaximumSpans50KG || !input.spans ||
        !native.states || native.stateCount != input.spanCount ||
        (sourceRgba ? native.opaquePresentationAlphaOnly : !native.opaquePresentationAlphaOnly))
        return fail(d, VehicleWindowError50DS::frame, "original UI requires explicit native policy and bounded 640x448 draw");
    if(directVideo&&(!sourceRgba||!validOriginalUiDirectVideo50KG(*directVideo)))
        return fail(d,VehicleWindowError50DS::frame,"unsupported direct video source command");
    if (sourceRgba && (!originalUiSourceRgbaAvailable() || input.retainRaceImage))
        return fail(d,VehicleWindowError50DS::frame,"source UI requires fresh native source target, never retained race alpha");
    if (input.textures != s.request.originalUiTextures || input.texturePublication != s.uiTexturePublication ||
        (input.textures && !input.textures->validBorrow(s.request.context, s.request.isCurrent, s.uiTexturePublication)) ||
        (!input.textures && input.texturePublication != 0))
        return fail(d, VehicleWindowError50DS::material, "original UI texture publication/context/thread changed");
    for (float value : input.clearRgba)
        if (!std::isfinite(value) || value < 0.f || value > 1.f)
            return fail(d, VehicleWindowError50DS::frame, "invalid original UI clear color");
    if (input.clearRgba[3] != 1.f)
        return fail(d, VehicleWindowError50DS::frame, "original UI presentation alpha must be opaque");
    if (input.retainRaceImage && (s.raceWidth <= 0 || s.raceHeight <= 0 || !s.gl.blitFramebuffer))
        return fail(d, VehicleWindowError50DS::noImage, "requested original UI race backdrop is unavailable");
    for (U i = 0; i < input.quadCount * 4; ++i) {
        const auto& v = input.vertices[i];
        for (float value : v.position) if (!std::isfinite(value))
            return fail(d, VehicleWindowError50DS::frame, "nonfinite original UI position");
        for (float value : v.uv) if (!std::isfinite(value))
            return fail(d, VehicleWindowError50DS::frame, "nonfinite original UI UV");
        if (v.position[2] < 0.f || v.position[2] > 16777215.f ||
            std::floor(v.position[2]) != v.position[2] || v.color[3] > 128)
            return fail(d, VehicleWindowError50DS::frame, "UI requires original 24-bit packet depth and raw alpha <=128");
    }
    if ((native.sourcePacketStq == nullptr) != (native.sourcePacketStqCount == 0) ||
        (native.sourcePacketStq && native.sourcePacketStqCount != input.quadCount * 4) ||
        (native.sourcePacketStq && !sourceRgba))
        return fail(d, VehicleWindowError50DS::frame, "invalid or non-source packet STQ stream");
    U next = 0;
    bool anyFog = false, anySourceRaster = false, anyCoverage = false, anySourcePacketStq = false;
    for (U i = 0; i < input.spanCount; ++i) {
        const auto& span = input.spans[i]; const auto& state = native.states[i];
        if(state.coverage!=OriginalUiCoverage50KG::aa0&&state.coverage!=OriginalUiCoverage50KG::fourSampleAa1)
            return fail(d,VehicleWindowError50DS::material,"unknown original UI coverage policy");
        if(state.coverage==OriginalUiCoverage50KG::fourSampleAa1&&!sourceRgba)
            return fail(d,VehicleWindowError50DS::material,"UI AA1 requires source-RGBA sample composition");
        if(state.coverage==OriginalUiCoverage50KG::fourSampleAa1&&
            (span.materialMode!=OriginalUiMaterialMode50KG::sourceDiffuseCompressed||
             span.blend!=OriginalUiBlend50KG::sourceAlpha||state.sourceAlphaTestGreaterZero))
            return fail(d,VehicleWindowError50DS::material,"UI AA1 requires original compressed Diffuse source-alpha without alpha test");
        anyCoverage=anyCoverage||state.coverage==OriginalUiCoverage50KG::fourSampleAa1;
        const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
        const bool specular=span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
        const bool sourceRaster = span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuse ||
            span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuseCompressed || specular ||
            span.materialMode == OriginalUiMaterialMode50KG::sourcePrelitCompressed;
        const bool fixed64Specular=span.blend==OriginalUiBlend50KG::fixed64Additive;
        const bool specularBlend=span.blend==OriginalUiBlend50KG::destinationAlphaAdditive ||
            span.blend==OriginalUiBlend50KG::fixed255Additive || fixed64Specular;
        const bool shadowBlend=span.blend==OriginalUiBlend50KG::destinationSourceAlphaAttenuation;
        const bool sourceSpecialBlend=specularBlend||shadowBlend;
        if (shadowBlend && (!sourceRgba || span.materialMode!=OriginalUiMaterialMode50KG::sourceDiffuseCompressed ||
            state.depth!=OriginalUiDepth50KG::greaterEqual || !state.depthWrite ||
            !state.sourceFogEnabled || !state.sourceAlphaTestGreaterZero))
            return fail(d,VehicleWindowError50DS::material,"shadow requires actual typed source-RGBA policy");
        if ((specular && !sourceRgba) || specularBlend!=specular ||
            (specular && (state.depth!=OriginalUiDepth50KG::greaterEqual || state.depthWrite!=fixed64Specular ||
                !state.sourceFogEnabled || !state.sourceAlphaTestGreaterZero)))
            return fail(d,VehicleWindowError50DS::material,"specular requires actual typed source-RGBA policy");
        const bool textured = span.materialMode == OriginalUiMaterialMode50KG::texturedModulate || particle || sourceRaster;
        const bool untextured = span.materialMode == OriginalUiMaterialMode50KG::untexturedVertexColor;
        const float blendAlphaScale = textured ? (255.f / 128.f) * (255.f / 128.f) : 255.f / 128.f;
        if (!span.quadCount || span.firstQuad != next || span.quadCount > input.quadCount - next ||
            (!textured && !untextured) || span.colorScale != (textured ? 255.f / 128.f : 1.f) ||
            (span.blend != OriginalUiBlend50KG::opaque && span.blend != OriginalUiBlend50KG::sourceAlpha &&
                span.blend != OriginalUiBlend50KG::additive && !sourceSpecialBlend) ||
            span.alphaScale != (span.blend == OriginalUiBlend50KG::opaque || sourceSpecialBlend ? 1.f : blendAlphaScale) ||
            (state.depth != OriginalUiDepth50KG::never && state.depth != OriginalUiDepth50KG::always &&
                state.depth != OriginalUiDepth50KG::greaterEqual && state.depth != OriginalUiDepth50KG::greater))
            return fail(d, VehicleWindowError50DS::material, "invalid ordered original UI material/depth policy");
        if (!sourceRaster && (state.sourceFogEnabled || state.sourceAlphaTestGreaterZero))
            return fail(d, VehicleWindowError50DS::material, "source Diffuse flags on another material identity");
        if (state.sourcePacketStq && (!sourceRgba || !sourceRaster || !native.sourcePacketStq))
            return fail(d, VehicleWindowError50DS::material, "raw packet STQ requires sealed source raster ownership");
        if (span.materialMode == OriginalUiMaterialMode50KG::sourcePrelitCompressed &&
            (!sourceRgba || !state.sourcePacketStq))
            return fail(d, VehicleWindowError50DS::material, "original PreLit requires its raw packet STQ source route");
        anySourcePacketStq = anySourcePacketStq || state.sourcePacketStq;
        anyFog = anyFog || particle || (sourceRaster && state.sourceFogEnabled);
        anySourceRaster = anySourceRaster || sourceRaster;
        if (particle) {
            if (span.blend == OriginalUiBlend50KG::opaque || state.depth != OriginalUiDepth50KG::greaterEqual ||
                state.depthWrite || state.sampler.minification != MaterialTextureFilter50EB::linear ||
                state.sampler.magnification != MaterialTextureFilter50EB::linear ||
                state.sampler.s != MaterialTextureWrap50EB::repeat || state.sampler.t != MaterialTextureWrap50EB::repeat)
                return fail(d, VehicleWindowError50DS::material, "invalid actual Main particle policy");
        }
        if (untextured) {
            if (span.textureIndex != assets::unresolvedMaterialTexture50EA ||
                state.sampler.minification != MaterialTextureFilter50EB::unspecified ||
                state.sampler.magnification != MaterialTextureFilter50EB::unspecified ||
                state.sampler.s != MaterialTextureWrap50EB::unspecified ||
                state.sampler.t != MaterialTextureWrap50EB::unspecified)
                return fail(d, VehicleWindowError50DS::material,
                    "untextured UI requires no texture index or sampler");
            next += span.quadCount;
            continue;
        }
        const auto filter = [](MaterialTextureFilter50EB v) noexcept {
            return v == MaterialTextureFilter50EB::nearest || v == MaterialTextureFilter50EB::linear;
        };
        const auto wrap = [](MaterialTextureWrap50EB v) noexcept {
            return v == MaterialTextureWrap50EB::repeat || v == MaterialTextureWrap50EB::clampToEdge;
        };
        if (!filter(state.sampler.minification) || !filter(state.sampler.magnification) ||
            state.sampler.minification != state.sampler.magnification ||
            !wrap(state.sampler.s) || !wrap(state.sampler.t))
            return fail(d, VehicleWindowError50DS::material, "UI requires explicit admitted matching min/mag source sampler");
        const auto* texture = input.textures ? input.textures->asset(span.textureIndex) : nullptr;
        if (!texture || !texture->name)
            return fail(d, VehicleWindowError50DS::material, "original UI span lacks its real published texture");
        if(input.textures->riderSparse()){
            const auto* cpu=s.request.originalUiCpuTextures?s.request.originalUiCpuTextures->texture(span.textureIndex):nullptr;
            if(!cpu||cpu->identity!=texture->identity||!input.textures->riderSamplerMatches(span.textureIndex,state.sampler))
                return fail(d,VehicleWindowError50DS::material,"Rider actual textured span lacks exact resident identity/sampler");
        }
        if (span.textureIndex >= assets::maxMaterialTextureAssets50EA)
            return fail(d, VehicleWindowError50DS::material, "UI texture alpha metadata index out of bounds");
        if (!sourceRgba && span.blend != OriginalUiBlend50KG::opaque) {
            const U maximumAlpha = s.uiTextureMaximumAlpha[span.textureIndex];
            const U firstVertex = span.firstQuad * 4, endVertex = (span.firstQuad + span.quadCount) * 4;
            for (U v = firstVertex; v < endVertex; ++v)
                if (U(input.vertices[v].color[3]) * maximumAlpha > 128u * 128u)
                    return fail(d, VehicleWindowError50DS::material,
                        "source texture times vertex alpha exceeds native blend factor domain");
        }
        next += span.quadCount;
    }
    if (next != input.quadCount)
        return fail(d, VehicleWindowError50DS::frame, "original UI spans must partition all quads exactly");
    if (anyFog && (!native.particleFog || native.particleFogCount != input.quadCount * 4))
        return fail(d, VehicleWindowError50DS::frame, "fog-enabled materials require the parallel source fog stream");
    if ((native.particleFog == nullptr) != (native.particleFogCount == 0) ||
        (native.particleFog && native.particleFogCount != input.quadCount * 4))
        return fail(d, VehicleWindowError50DS::frame, "invalid particle fog stream bounds");
    for (U i = 0; i < native.particleFogCount; ++i)
        if (!std::isfinite(native.particleFog[i]) || native.particleFog[i] < 0.f ||
            native.particleFog[i] > 255.f / 256.f)
            return fail(d, VehicleWindowError50DS::frame, "invalid source particle fog coefficient");
    if ((native.sourceRasterW == nullptr) != (native.sourceRasterWCount == 0) ||
        (native.sourceRasterW && native.sourceRasterWCount != input.quadCount * 4) ||
        (anySourceRaster && !native.sourceRasterW))
        return fail(d, VehicleWindowError50DS::frame, "invalid or missing source perspective W stream");
    for (U i = 0; i < native.sourcePacketStqCount; ++i) {
        const auto& stq = native.sourcePacketStq[i];
        if (!std::isfinite(stq[0]) || !std::isfinite(stq[1]) || !std::isfinite(stq[2]) || stq[2] <= 0.f ||
            !std::isfinite(stq[0] / stq[2]) || !std::isfinite(stq[1] / stq[2]))
            return fail(d, VehicleWindowError50DS::frame, "invalid original packet STQ sampling range");
    }
    for (U i = 0; i < native.sourceRasterWCount; ++i)
        if (!std::isfinite(native.sourceRasterW[i]) || native.sourceRasterW[i] <= 0.f)
            return fail(d, VehicleWindowError50DS::frame, "source perspective W must be finite and positive");
    for (U i = 0; i < input.spanCount; ++i) {
        const auto& span = input.spans[i]; const auto& state = native.states[i];
        const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
        if (!particle && !state.sourceFogEnabled) continue;
        for (U v = span.firstQuad * 4; v < (span.firstQuad + span.quadCount) * 4; ++v) {
            const float f = native.particleFog[v];
            if ((particle && f > 254.f / 256.f) || (!particle && std::floor(f * 256.f) != f * 256.f) ||
                (span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed && f!=255.f/256.f))
                return fail(d, VehicleWindowError50DS::frame, "fog stream does not match source program byte domain");
        }
    }
    for (float value : native.particleFogColor)
        if (!std::isfinite(value) || value < 0.f || value > 1.f)
            return fail(d, VehicleWindowError50DS::frame, "invalid authored particle fog color");

    U sourceIndexCount = 0;
    if (sourceRgba) {
        // Keep exactly the old source triangle sequence and equal-XY skip.
        // No projection, Z/W, clipping or raster bounds enter this decision.
        // Validated spans partition at most the cold source quad capacity;
        // each quad writes at most six indices, with no frame allocation.
        constexpr U corners[2][3] = {{0, 1, 2}, {0, 2, 3}};
        for (U i = 0; i < input.spanCount; ++i) {
            const auto& span = input.spans[i];
            auto& range = s.uiSourceIndices->spans[i];
            range.firstIndex = sourceIndexCount;
            for (U q = span.firstQuad; q < span.firstQuad + span.quadCount; ++q) {
                const auto* v = &input.vertices[q * 4];
                const auto samePointXY = [&](U a, U b) {
                    return v[a].position[0] == v[b].position[0] &&
                        v[a].position[1] == v[b].position[1];
                };
                for (const auto& c : corners) {
                    if (samePointXY(c[0], c[1]) || samePointXY(c[1], c[2]) ||
                        samePointXY(c[0], c[2])) continue;
                    for (U corner : c) s.uiSourceIndices->indices[sourceIndexCount++] = q * 4 + corner;
                }
            }
            range.indexCount = sourceIndexCount - range.firstIndex;
        }
    }

    // A descriptor does not imply a covered AA1 sample. Prove the prefix
    // boundary from exactly the same retained triples and conservative XY/W
    // domains as the mixed renderer, never from alpha, pose or rider identity.
    U firstCoveredAa1Span=input.spanCount;
    if(sourceRgba&&anyCoverage){
        for(U si=0;si<input.spanCount&&firstCoveredAa1Span==input.spanCount;++si){
            if(native.states[si].coverage!=OriginalUiCoverage50KG::fourSampleAa1)continue;
            const auto& range=s.uiSourceIndices->spans[si];
            for(U index=range.firstIndex;index<range.firstIndex+range.indexCount;index+=3){
                const U ia=s.uiSourceIndices->indices[index],ib=s.uiSourceIndices->indices[index+1],ic=s.uiSourceIndices->indices[index+2];
                OriginalUiCoverageBounds50KG region;
                if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
                   !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],region))
                    return fail(d,VehicleWindowError50DS::frame,"invalid original UI coverage triangle bounds");
                // Admitted AA1 is sourceDiffuseCompressed and has a validated
                // W stream. Extreme finite W must never be classified empty.
                if(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic]))region={0,0,640,448};
                if(!region.empty()){firstCoveredAa1Span=si;break;}
            }
        }
    }

    s.diagnostic = &d;
    bool accepted = s.check("before original UI submission") &&
        ((sourceRgba && !publishSource) || s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    const auto pending = static_cast<std::uint8_t>(s.uiPublished ^ 1u);
    if (accepted) {
        // Ordinary spans still use immutable image parameters through sampler0.
        // Only an authenticated canonical Garage batch uses the Window-owned
        // per-policy sampler; image base/max levels remain separately immutable.
        // Cache this submission's exact image+sampler observations, never readiness.
        struct SamplerObservation { U image, sampler; I values[6]; };
        SamplerObservation observed[originalUiMaximumSpans50KG];
        U observedCount = 0;
        constexpr U parameters[] = {0x2801, 0x2800, 0x2802, 0x2803, 0x813C, 0x813D};
        s.gl.activeTexture(textureUnit0); if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        for (U i = 0; accepted && i < input.spanCount; ++i) {
            if (input.spans[i].materialMode == OriginalUiMaterialMode50KG::untexturedVertexColor) continue;
            const auto& sampler = native.states[i].sampler;
            const U image = input.textures->texture(input.spans[i].textureIndex);
            const U samplerName=garageBatch?s.garageSampler50KG(sampler):0;
            if(garageBatch&&!samplerName){accepted=fail(d,VehicleWindowError50DS::material,"actual Garage span has no owned effective sampler");break;}
            U observation = 0;
            while (observation < observedCount && (observed[observation].image != image||observed[observation].sampler!=samplerName)) ++observation;
            const bool firstObservation = observation == observedCount;
            // At most one new entry per span; spanCount was bounded above.
            auto& actual = observed[observation];
            if (firstObservation) {
                actual.image = image;actual.sampler=samplerName;
                s.gl.bindTexture(texture2D, image);
                if(garageBatch){
                    s.gl.bindSampler(0,samplerName);I bound=0;s.gl.getIntegerv(0x8919,&bound);
                    accepted=s.check("verify actual Garage sampler binding");
                    if(accepted&&static_cast<U>(bound)!=samplerName)
                        accepted=fail(d,VehicleWindowError50DS::material,"Garage sampler binding differs from its retained owner");
                }
            }
            const I expected[] = {
                sampler.minification == MaterialTextureFilter50EB::nearest ? 0x2600 : 0x2601,
                sampler.magnification == MaterialTextureFilter50EB::nearest ? 0x2600 : 0x2601,
                sampler.s == MaterialTextureWrap50EB::repeat ? 0x2901 : 0x812F,
                sampler.t == MaterialTextureWrap50EB::repeat ? 0x2901 : 0x812F, 0, 0};
            for (U p = 0; accepted && p < 6; ++p) {
                if (firstObservation) {
                    actual.values[p] = -1;
                    if(garageBatch&&p<4)s.gl.getSamplerParameteriv(samplerName,parameters[p],&actual.values[p]);
                    else s.gl.getTexParameteriv(texture2D, parameters[p], &actual.values[p]);
                    accepted = s.check("verify per-span original UI sampler");
                }
                if (accepted && actual.values[p] != expected[p])
                    accepted = fail(d, VehicleWindowError50DS::material,
                        garageBatch?"Garage span sampler/image parameters differ from actual retained owners":
                            "UI span source sampler differs from immutable bank texture parameters");
            }
            if (firstObservation && accepted) ++observedCount;
        }
        if(garageBatch){s.gl.bindSampler(0,0);const bool restored=s.check("restore sampler0 after Garage binding preflight");accepted=accepted&&restored;}
    }
    // Observation only, after authenticated frame/index/sampler preflight.
    // It cannot grant readiness, modify native diagnostics, or alter drawing.
    const char* disjointTrace=std::getenv("RR_TRACE_UI_COVERAGE_DISJOINT");
    if(accepted&&sourceRgba&&anyCoverage&&!s.uiCoverageDisjointTraced&&
       disjointTrace&&std::strcmp(disjointTrace,"1")==0){
        s.uiCoverageDisjointTraced=true;
        const auto traceStart=Storage::UiCoverageClock::now();
        struct Totals {std::uint64_t current=0,proposed=0,area=0,unionArea=0,rectangleArea=0,
            overlap=0,cap=0,empty=0,full=0,wFallback=0,xyFallback=0,invalid=0,comparisons=0;
            U maxGroup=0,rows=0,truncated=0,nonemptySpans=0;} total;
        for(U si=0;si<input.spanCount;++si){
            const auto& span=input.spans[si];const auto& state=native.states[si];
            const auto& range=s.uiSourceIndices->spans[si];
            const bool opaque=originalUiCoverageOpaqueBatch50KG(span,state);
            const bool raster=span.materialMode==OriginalUiMaterialMode50KG::sourceDiffuse||
                span.materialMode==OriginalUiMaterialMode50KG::sourceDiffuseCompressed||
                span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed||
                span.materialMode==OriginalUiMaterialMode50KG::sourcePrelitCompressed;
            OriginalUiCoverageDisjoint50KG measure;OriginalUiCoverageBounds50KG whole;
            std::uint64_t full=0,wFallback=0,xyFallback=0,invalid=0;
            for(U index=range.firstIndex;index<range.firstIndex+range.indexCount;index+=3){
                const U ia=s.uiSourceIndices->indices[index],ib=s.uiSourceIndices->indices[index+1],ic=s.uiSourceIndices->indices[index+2];
                OriginalUiCoverageBounds50KG r;
                if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
                   !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],r)){++invalid;continue;}
                bool extremeXY=false;
                for(U vertex:{ia,ib,ic})for(U axis=0;axis<2;++axis)
                    extremeXY=extremeXY||std::fabs(input.vertices[vertex].position[axis])>1048576.f;
                if(extremeXY)++xyFallback;
                if(raster&&(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
                   !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic]))){r={0,0,640,448};++wFallback;}
                if(r.x==0&&r.y==0&&r.width==640&&r.height==448)++full;
                if(!measure.add(r)){++invalid;continue;}
                if(!r.empty()){
                    if(whole.empty())whole=r;
                    else {const I right=std::max(whole.x+whole.width,r.x+r.width),top=std::max(whole.y+whole.height,r.y+r.height);
                        whole.x=std::min(whole.x,r.x);whole.y=std::min(whole.y,r.y);
                        whole.width=right-whole.x;whole.height=top-whole.y;}
                }
            }
            measure.finish();
            const std::uint64_t current=opaque?std::uint64_t(!whole.empty()):measure.nonempty;
            const std::uint64_t proposed=opaque?current:measure.groups;
            const std::uint64_t area=opaque?std::uint64_t(whole.width)*whole.height:measure.rectangleArea;
            const std::uint64_t unionArea=opaque?area:measure.unionArea;
            total.current+=current;total.proposed+=proposed;total.area+=area;total.unionArea+=unionArea;
            total.rectangleArea+=measure.rectangleArea;total.overlap+=opaque?0:measure.overlapFlushes;
            total.cap+=opaque?0:measure.capFlushes;total.empty+=measure.empty;total.full+=full;
            total.wFallback+=wFallback;total.xyFallback+=xyFallback;total.invalid+=invalid;
            total.comparisons+=measure.comparisons;total.maxGroup=std::max(total.maxGroup,opaque?0u:measure.maxGroup);
            if(current||invalid){++total.nonemptySpans;
                if(total.rows<256){++total.rows;
                    std::fprintf(stderr,"ui_coverage_disjoint_span span=%u cap=64 coverage=%u blend=%u depth=%u depth_write=%u material=%u opaque_unchanged=%u triangles=%u current=%llu proposed=%llu rectangle_area=%llu current_area=%llu proposed_union_area=%llu overlap_flushes=%llu cap_flushes=%llu empty=%llu full=%llu xy_fallback=%llu w_fallback=%llu invalid=%llu max_group=%u\n",
                        si,unsigned(state.coverage),unsigned(span.blend),unsigned(state.depth),unsigned(state.depthWrite),unsigned(span.materialMode),unsigned(opaque),range.indexCount/3,
                        (unsigned long long)current,(unsigned long long)proposed,(unsigned long long)measure.rectangleArea,(unsigned long long)area,(unsigned long long)unionArea,
                        (unsigned long long)(opaque?0:measure.overlapFlushes),(unsigned long long)(opaque?0:measure.capFlushes),(unsigned long long)measure.empty,
                        (unsigned long long)full,(unsigned long long)xyFallback,(unsigned long long)wFallback,(unsigned long long)invalid,opaque?0u:measure.maxGroup);
                }else ++total.truncated;
            }
        }
        std::fprintf(stderr,"ui_coverage_disjoint_total cap=64 spans=%u nonempty_spans=%u logged_rows=%u truncated_rows=%u current=%llu proposed=%llu rectangle_area=%llu current_area=%llu proposed_union_area=%llu overlap_flushes=%llu cap_flushes=%llu empty=%llu full=%llu xy_fallback=%llu w_fallback=%llu invalid=%llu comparisons=%llu max_group=%u analysis_wall_ns=%llu scope=diagnostic_hypothetical_all_mixed_baseline\n",
            input.spanCount,total.nonemptySpans,total.rows,total.truncated,(unsigned long long)total.current,(unsigned long long)total.proposed,
            (unsigned long long)total.rectangleArea,(unsigned long long)total.area,(unsigned long long)total.unionArea,(unsigned long long)total.overlap,
            (unsigned long long)total.cap,(unsigned long long)total.empty,(unsigned long long)total.full,(unsigned long long)total.xyFallback,
            (unsigned long long)total.wFallback,(unsigned long long)total.invalid,(unsigned long long)total.comparisons,total.maxGroup,
            (unsigned long long)Storage::uiCoverageNs(traceStart));
    }
    if(accepted&&anyCoverage){
        accepted=s.prepareUiCoverage("active_submit");
    }
    if (accepted) {
        s.gl.bindFramebuffer(framebuffer,sourceRgba?s.uiSourceTarget:s.uiTargets[pending]);
        if(sourceRgba){
            s.gl.disable(0x0BF2); // No inherited color-logic operation on raw source bytes.
            if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E); // Conventional lower-left/-1..1.
        }
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        const I rasterWidth=sourceRgba?640:width,rasterHeight=sourceRgba?448:height;
        s.gl.viewport(0,0,rasterWidth,rasterHeight);
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F};
        for (U capability : disabled) s.gl.disable(capability);
        for (I i = 0; i < s.clipDistances; ++i) s.gl.disable(0x3000 + static_cast<U>(i));
        s.gl.colorMask(1, 1, 1, 1); s.gl.depthMask(1);
        s.gl.depthRange(0.0, 1.0); s.gl.clearDepth(0.0);
        s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.clearColor(input.clearRgba[0],input.clearRgba[1],input.clearRgba[2],sourceRgba?128.f/255.f:1.f);
        //Actual mid-Camera24B7B0 must preserve all preceding callback draws.
        //Only a fresh complete-frame invocation owns this old clear path.
        if(!garageBatch)s.gl.clear(colorBufferBit | depthBufferBit);
        if(sourceRgba&&!garageBatch){s.uiCoverageSourceDiagnostic=anyCoverage;++s.uiSourceTargetGeneration;s.uiSourceAcceptedTargetGeneration=0;
            s.uiSourcePublishedTargetGeneration=0;}

        if (input.retainRaceImage) {
            // Preserve the race aspect ratio too; blit bounds are whole pixels.
            const double fit = std::min(double(width) / s.raceWidth, double(height) / s.raceHeight);
            const I rw = std::max<I>(1, static_cast<I>(std::floor(s.raceWidth * fit)));
            const I rh = std::max<I>(1, static_cast<I>(std::floor(s.raceHeight * fit)));
            const I x = (width - rw) / 2, y = (height - rh) / 2;
            s.gl.bindFramebuffer(readFramebuffer, s.targets[s.published]); s.gl.readBuffer(colorAttachment);
            s.gl.blitFramebuffer(0, 0, s.raceWidth, s.raceHeight, x, y, x + rw, y + rh, colorBufferBit, 0x2600);
            s.gl.bindFramebuffer(framebuffer, s.uiTargets[pending]);
            // Blit ignores color masks. Reset only presentation alpha afterward.
            s.gl.colorMask(0, 0, 0, 1); s.gl.clear(colorBufferBit);
        }
        s.gl.colorMask(1,1,1,sourceRgba?1:0);
        const double scale = std::min(double(rasterWidth)/640.0,double(rasterHeight)/448.0);
        const double ox=(rasterWidth-640.0*scale)*0.5,oy=(rasterHeight-448.0*scale)*0.5;
        // One scale for both axes. Raster scissor encloses fractional boundaries;
        // shader geometry is never rounded, stretched, or half-pixel adjusted.
        const I sx = static_cast<I>(std::floor(ox)), sy = static_cast<I>(std::floor(oy));
        const I ex = std::min<I>(rasterWidth, static_cast<I>(std::ceil(ox + 640.0 * scale)));
        const I ey = std::min<I>(rasterHeight, static_cast<I>(std::ceil(oy + 448.0 * scale)));
        s.gl.scissor(sx, sy, ex - sx, ey - sy); s.gl.enable(0x0C11);
        s.resetNativeGcLighting();
        s.gl.enable(0x0B71); s.gl.useProgram(s.uiProgram);
        s.gl.uniform4f(s.uiCanvasUniform,float(2.0*scale/rasterWidth),float(-2.0*scale/rasterHeight),
            float(2.0*ox/rasterWidth-1.0),float(1.0-2.0*oy/rasterHeight));
        s.gl.uniform1i(s.gcParticleModeUniform,0);
        s.gl.uniform1i(s.uiSourceCompositionUniform,sourceRgba?1:0);
        s.gl.uniform1i(s.uiCoveragePayloadUniform,0);
        if(!s.uiSourceCoherentFramebufferFetch)s.gl.uniform1i(s.uiSourceTargetUniform,1);
        if(sourceRgba && !s.uiSourceCoherentFramebufferFetch){
            s.gl.activeTexture(textureUnit0+1);if(s.gl.bindSampler)s.gl.bindSampler(1,0);
            s.gl.bindTexture(texture2D,s.uiSourceColor);
        }
        s.gl.activeTexture(textureUnit0); if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        s.gl.uniform1i(s.uiImageUniform, 0);
        s.gl.uniform4f(s.uiParticleFogColorUniform, native.particleFogColor[0],
            native.particleFogColor[1], native.particleFogColor[2], 1.f);
        s.drawGl.bindVertexArray(s.uiVertexArray); s.drawGl.bindBuffer(arrayBuffer, s.uiVertexBuffer);
        if(directVideo){
            // TEST=31001: alpha NEVER/FB_ONLY, depth ALWAYS; source writes
            // RGBA but not Z. ALPHA=(Cd-0)*As/128+Cs with Cs=As=0 is exactly
            // zero, independent of destination. This is an actual queued sprite
            // draw, not the frame clear and not a fabricated material pass.
            const assets::Vertex48B vertices[4]={
                {{0,0,0},{0,0},{0,0,0,0}},{{0,448,0},{0,0},{0,0,0,0}},
                {{640,0,0},{0,0},{0,0,0,0}},{{640,448,0},{0,0},{0,0,0,0}}};
            s.gl.disable(0x0BE2);s.gl.depthFunc(0x0207);s.gl.depthMask(0);
            s.gl.uniform1i(s.uiTexturedUniform,0);s.gl.uniform1i(s.uiSourceRasterUniform,0);
            s.gl.uniform1i(s.uiSourceFogEnabledUniform,0);s.gl.uniform1i(s.uiSourceAlphaTestUniform,0);
            s.gl.uniform1i(s.uiSourceBlendUniform,0);s.gl.uniform1f(s.uiAlphaUniform,1.f);
            s.gl.bindTexture(texture2D,0);
            if(!s.uiSourceCoherentFramebufferFetch){s.gl.activeTexture(textureUnit0+1);
                s.gl.bindTexture(texture2D,0);s.gl.activeTexture(textureUnit0);}
            s.drawGl.bufferSubData(arrayBuffer,0,sizeof(vertices),vertices);
            if(sourceWorkStarted)*sourceWorkStarted=true;
            s.gl.drawArrays(0x0005,0,4);
            accepted=s.check("source direct video 25E610 before material drain");
            if(accepted){++d.originalUiDraws;d.originalUiSourceTriangles+=2;}
            // Subsequent spans establish their own depth/blend/material state;
            // all source vertex/index/auxiliary streams are restored below.
            // Fallback destination sampling is rebound by each source span;
            // zero-source direct output must not sample the attached target.
        }
        s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(input.quadCount) * 4 * sizeof(assets::Vertex48B), input.vertices);
        if (anyFog) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiParticleFogBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(native.particleFogCount) * sizeof(float), native.particleFog);
        }
        if (anySourceRaster) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiSourceRasterWBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(native.sourceRasterWCount) * sizeof(float), native.sourceRasterW);
        }
        if (anySourcePacketStq) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiSourcePacketStqBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0,
                std::intptr_t(native.sourcePacketStqCount) * sizeof(std::array<float,3>), native.sourcePacketStq);
        }
        s.drawGl.bindBuffer(elementBuffer, sourceRgba ? s.uiSourceIndexBuffer : s.uiIndexBuffer);
        if (sourceRgba)
            s.drawGl.bufferSubData(elementBuffer, 0, std::intptr_t(sourceIndexCount) * sizeof(U),
                s.uiSourceIndices->indices);
        accepted = s.check("prepare authenticated original UI frame") && accepted;
        // No material representative has bound before this point. Read-only
        // preflight/context/window failures remain retryable with the same
        // composer generation. A later partial source pass failure is unknown.
        if(sourceRgba&&accepted&&input.spanCount&&sourceWorkStarted)*sourceWorkStarted=true;
        preDrawDiagnosticNs=Storage::uiCoverageNs(submitStageStart);
        const auto coverageDrawStart=Storage::UiCoverageClock::now();
        bool coverageMixed=false;U coveragePrefixDraws=0,coverageMixedDraws=0,coverageEmptyAa1Spans=0;
        std::uint64_t coveragePromotionNs=0;
        for (U i = 0; accepted && i < input.spanCount; ++i) {
            const auto& span = input.spans[i]; const auto& state = native.states[i];
            const U depth = state.depth == OriginalUiDepth50KG::never ? 0x0200 :
                state.depth == OriginalUiDepth50KG::always ? 0x0207 :
                state.depth == OriginalUiDepth50KG::greaterEqual ? 0x0206 : 0x0204;
            s.gl.depthFunc(depth); s.gl.depthMask(state.depthWrite ? 1 : 0);
            if (sourceRgba || span.blend == OriginalUiBlend50KG::opaque) s.gl.disable(0x0BE2);
            else {
                s.gl.enable(0x0BE2); s.gl.blendEquation(0x8006);
                s.gl.blendFunc(0x0302, span.blend == OriginalUiBlend50KG::additive ? 1 : 0x0303);
            }
            s.gl.uniform1f(s.uiAlphaUniform, span.alphaScale);
            const I sourceBlend=span.blend==OriginalUiBlend50KG::opaque?0:
                span.blend==OriginalUiBlend50KG::sourceAlpha?1:
                span.blend==OriginalUiBlend50KG::additive?2:
                span.blend==OriginalUiBlend50KG::destinationAlphaAdditive?3:
                span.blend==OriginalUiBlend50KG::fixed255Additive?4:
                span.blend==OriginalUiBlend50KG::fixed64Additive?6:5;
            s.gl.uniform1i(s.uiSourceBlendUniform,sourceBlend);
            const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
            const bool sourceRaster = span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuse ||
                span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuseCompressed ||
                span.materialMode == OriginalUiMaterialMode50KG::sourceSpecularMapCompressed ||
                span.materialMode == OriginalUiMaterialMode50KG::sourcePrelitCompressed;
            const bool textured = span.materialMode == OriginalUiMaterialMode50KG::texturedModulate || particle || sourceRaster;
            s.gl.uniform1i(s.uiTexturedUniform, textured ? 1 : 0);
            s.gl.uniform1i(s.uiSourceRasterUniform, sourceRaster ? (state.sourcePacketStq ? 2 : 1) : 0);
            s.gl.uniform1i(s.uiSourceFogEnabledUniform, (particle || state.sourceFogEnabled) ? 1 : 0);
            s.gl.uniform1i(s.uiSourceAlphaTestUniform, (particle || state.sourceAlphaTestGreaterZero) ? 1 : 0);
            s.gl.bindTexture(texture2D, textured ? input.textures->texture(span.textureIndex) : 0);
            const U sourceSampler=garageBatch&&textured?s.garageSampler50KG(state.sampler):0;
            if(garageBatch)s.gl.bindSampler(0,sourceSampler);
            if(sourceRgba&&anyCoverage&&!coverageMixed&&state.coverage==OriginalUiCoverage50KG::fourSampleAa1){
                if(i<firstCoveredAa1Span){
                    // All retained bounds in this AA1 span were proven empty.
                    // Keep original retained-triangle/quad accounting intact.
                    ++coverageEmptyAa1Spans;
                    d.originalUiSourceTriangles+=s.uiSourceIndices->spans[i].indexCount/3;
                    d.originalUiQuads+=span.quadCount;continue;
                }
                const auto promotionStart=Storage::UiCoverageClock::now();
                accepted=s.promoteUiCoveragePrefix();coveragePromotionNs=Storage::uiCoverageNs(promotionStart);
                if(!s.uiCoveragePromotionReported){s.uiCoveragePromotionReported=true;
                    std::fprintf(stderr,"ui_coverage_first_promotion target=%llu span=%u spans=%u prefix_draws=%u promotion_wall_ns=%llu accepted=%u scope=cpu_blit_call_no_added_wait\n",
                        (unsigned long long)s.uiSourceTargetGeneration,i,input.spanCount,coveragePrefixDraws,
                        (unsigned long long)coveragePromotionNs,unsigned(accepted));}
                if(!accepted)break; // Existing partial-work receipt forbids publication.
                coverageMixed=true; // Never collapse again, even for later AA0.
                s.gl.bindTexture(texture2D,textured?input.textures->texture(span.textureIndex):0);
            }
            if(sourceRgba&&coverageMixed){
                const auto& range=s.uiSourceIndices->spans[i];U draws=0;
                const U image=textured?input.textures->texture(span.textureIndex):0;
                const bool opaqueBatch=originalUiCoverageOpaqueBatch50KG(span,state);
                OriginalUiCoverageBounds50KG spanRegion;
                accepted=s.beginUiCoverageSpan(state.coverage==OriginalUiCoverage50KG::fourSampleAa1,sourceBlend);
                for(U index=range.firstIndex;accepted&&index<range.firstIndex+range.indexCount;index+=3){
                    const U ia=s.uiSourceIndices->indices[index],ib=s.uiSourceIndices->indices[index+1],
                        ic=s.uiSourceIndices->indices[index+2];
                    OriginalUiCoverageBounds50KG region;
                    if(ia>=input.quadCount*4||ib>=input.quadCount*4||ic>=input.quadCount*4||
                        !originalUiCoverageBounds50KG(input.vertices[ia],input.vertices[ib],input.vertices[ic],region)){
                        accepted=fail(d,VehicleWindowError50DS::frame,"invalid original UI coverage triangle bounds");break;
                    }
                    // Extreme positive W may under/overflow clipXY*W in the
                    // source shader. Preserve the full domain rather than infer
                    // a narrow XY bound; geometry/coverage policy is unchanged.
                    if(sourceRaster){
                        if(!originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ia])||
                           !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ib])||
                           !originalUiCoverageRasterWBounded50KG(native.sourceRasterW[ic]))region={0,0,640,448};
                    }
                    // Only a clipped-empty conservative region omits GPU work.
                    // Source order, indices, coverage, ALPHA and depth stay intact.
                    if(region.empty())continue;
                    if(opaqueBatch){
                        if(spanRegion.empty())spanRegion=region;
                        else {const I right=std::max(spanRegion.x+spanRegion.width,region.x+region.width);
                            const I top=std::max(spanRegion.y+spanRegion.height,region.y+region.height);
                            spanRegion.x=std::min(spanRegion.x,region.x);spanRegion.y=std::min(spanRegion.y,region.y);
                            spanRegion.width=right-spanRegion.x;spanRegion.height=top-spanRegion.y;}
                    }else{
                        accepted=s.drawUiCoverageRange(state.coverage==OriginalUiCoverage50KG::fourSampleAa1,
                            index,3,image,depth,state.depthWrite,sourceBlend,false,region,sourceSampler);if(accepted)++draws;
                    }
                }
                if(accepted&&opaqueBatch&&!spanRegion.empty()){
                    // Uniform opaque AA0 GE/GREATER writing span: source-order
                    // center-depth reduction selects the same surviving payload
                    // for comparison against every retained destination sample.
                    accepted=s.drawUiCoverageRange(false,range.firstIndex,range.indexCount,image,depth,
                        true,0,true,spanRegion,sourceSampler);if(accepted)++draws;
                }
                const bool spanRestored=s.endUiCoverageSpan(image,depth,state.depthWrite);
                accepted=accepted&&spanRestored;
                if(accepted){coverageMixedDraws+=draws;d.originalUiDraws+=draws;d.originalUiSourceTriangles+=range.indexCount/3;d.originalUiQuads+=span.quadCount;}
            }else if(sourceRgba){
                // Coherent fetch reads the framebuffer directly. Fallback
                // opaque writes unbind destination RGBA; only fallback feedback
                // requires one real triangle and texture barrier per draw.
                const bool feedback=span.blend!=OriginalUiBlend50KG::opaque;
                if(!s.uiSourceCoherentFramebufferFetch){
                    s.gl.activeTexture(textureUnit0+1);
                    s.gl.bindTexture(texture2D,feedback?s.uiSourceColor:0);
                    s.gl.activeTexture(textureUnit0);
                }
                const auto& range=s.uiSourceIndices->spans[i];
                const U sourceTriangles=range.indexCount/3;
                U sourceDraws=0;
                if(s.uiSourceCoherentFramebufferFetch || !feedback){
                    // Only retained original triples enter the batched draw;
                    // discarded equal-XY halves are never submitted to GL.
                    if(range.indexCount){
                        s.drawGl.drawElementsBaseVertex(0x0004,static_cast<I>(range.indexCount),0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(range.firstIndex)*sizeof(U)),0);
                        sourceDraws=1;
                    }
                }else{
                    for(U index=range.firstIndex;index<range.firstIndex+range.indexCount;index+=3){
                        s.gl.textureBarrier();++d.originalUiTextureBarriers;
                        s.drawGl.drawElementsBaseVertex(0x0004,3,0x1405,
                            reinterpret_cast<const void*>(std::uintptr_t(index)*sizeof(U)),0);
                        ++sourceDraws;
                    }
                }
                // GL errors remain sticky until this span boundary. A failed
                // span publishes no successful counters; partial source work
                // still poisons its pending history through the existing receipt.
                accepted=s.check("draw ordered source-RGBA span");
                if(accepted){coveragePrefixDraws+=sourceDraws;d.originalUiDraws+=sourceDraws;d.originalUiSourceTriangles+=sourceTriangles;
                    d.originalUiQuads+=span.quadCount;}
            }else{
                s.drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.quadCount * 6), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.firstQuad) * 6 * sizeof(U)), 0);
                accepted = s.check("draw ordered original UI span");
                if (accepted) { ++d.originalUiDraws; d.originalUiQuads += span.quadCount; }
            }
        }
        if(sourceRgba && !s.uiSourceCoherentFramebufferFetch){
            s.gl.activeTexture(textureUnit0+1);s.gl.bindTexture(texture2D,0);
            if(s.gl.bindSampler)s.gl.bindSampler(1,0);
            s.gl.activeTexture(textureUnit0);
        }
        if(anyCoverage){
            const auto drawNs=Storage::uiCoverageNs(coverageDrawStart);const auto resolveStart=Storage::UiCoverageClock::now();
            if(accepted&&coverageMixed)accepted=s.resolveUiCoverage();
            const auto resolveNs=Storage::uiCoverageNs(resolveStart);
            drawDiagnosticNs=drawNs;resolveDiagnosticNs=resolveNs;
            diagnosticPrefixDraws=coveragePrefixDraws;diagnosticMixedDraws=coverageMixedDraws;
            diagnosticEmptyAa1=coverageEmptyAa1Spans;diagnosticPromoted=coverageMixed;diagnosticPromotionNs=coveragePromotionNs;
            s.unbindUiCoverageInputs();s.gl.disable(0x809D);
            if(s.uiCoverageDrawReports<8||!accepted){++s.uiCoverageDrawReports;
                std::fprintf(stderr,"ui_coverage_active target=%llu source_draws=%u triangles=%u ordered_draw_wall_ns=%llu resolve_wall_ns=%llu accepted=%u prefix_draws=%u mixed_draws=%u empty_aa1_spans=%u promoted=%u coherent_fetch=%u promotion_wall_ns=%llu scope=cpu_submission_no_gpu_finish\n",
                    static_cast<unsigned long long>(s.uiSourceTargetGeneration),d.originalUiDraws,d.originalUiSourceTriangles,
                    static_cast<unsigned long long>(drawNs),static_cast<unsigned long long>(resolveNs),unsigned(accepted),
                    coveragePrefixDraws,coverageMixedDraws,coverageEmptyAa1Spans,unsigned(coverageMixed),unsigned(s.uiSourceCoherentFramebufferFetch),
                    static_cast<unsigned long long>(coveragePromotionNs));}
        }
        // Restore renderer-owned baseline state even on draw failure. The next
        // world begin supplies its own program, matrix, viewport and geometry.
        s.gl.disable(0x0C11); s.gl.disable(0x0BE2); s.gl.depthMask(1); s.gl.depthFunc(0x0201);
        s.gl.colorMask(1, 1, 1, 1); s.gl.clearDepth(1.0);
        // Element binding belongs to the VAO. Restore its immutable RGB bank
        // while the UI VAO is still current, including after source failures.
        if(sourceRgba)s.drawGl.bindBuffer(elementBuffer,s.uiIndexBuffer);
        s.drawGl.bindVertexArray(0); s.drawGl.bindBuffer(arrayBuffer, 0);
        const bool restored = s.check("finish private original UI frame");
        accepted = accepted && restored;
        if (accepted && sourceRgba){
            d.originalUiSourceAccepted=!garageBatch;
            if(publishSource){const auto publishStart=Storage::UiCoverageClock::now();
                accepted=s.publishUiSource(input.clearRgba,width,height);
                publishDiagnosticNs=Storage::uiCoverageNs(publishStart);
                if(anyCoverage&&(s.uiCoveragePublishReports<8||!accepted)){++s.uiCoveragePublishReports;
                    std::fprintf(stderr,"ui_coverage_publication target=%llu publish_wall_ns=%llu accepted=%u scope=existing_source_and_window_copy_no_gpu_finish\n",
                        static_cast<unsigned long long>(s.uiSourceTargetGeneration),static_cast<unsigned long long>(Storage::uiCoverageNs(publishStart)),unsigned(accepted));}
            }
        }else if (accepted) {
            s.uiPrivatePublicationGeneration=0;
            s.uiSourcePublishedTargetGeneration=0;
            s.uiPublished = pending; s.displayedTarget = s.uiTargets[pending];
            s.width = s.committedWidth = width; s.height = s.committedHeight = height;
            s.imageCommitted = true; d.originalUiAccepted = true;
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                accepted = s.copyToWindow();
                if (!accepted && d.error == VehicleWindowError50DS::gl) d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    // Also restore after rejected preflight, which never entered the draw body.
    // This precedes the sole return and never clears batch/GL failure history.
    if(garageBatch){s.gl.bindSampler(0,0);const bool restored=s.check("restore sampler0 after Garage submission");accepted=accepted&&restored;}
    const auto stageTotalNs=Storage::uiCoverageNs(submitStageStart);
    if(anyCoverage&&stageTotalNs>=100000000ull&&s.uiCoverageSlowReports<8){++s.uiCoverageSlowReports;
        std::fprintf(stderr,"ui_coverage_slow_stages target=%llu total_wall_ns=%llu pre_draw_wall_ns=%llu ordered_draw_wall_ns=%llu resolve_wall_ns=%llu publication_wall_ns=%llu first_aa1_span=%u spans=%u prefix_draws=%u mixed_draws=%u empty_aa1_spans=%u promoted=%u promotion_wall_ns=%llu accepted=%u scope=cpu_stage_times_no_added_wait\n",
            (unsigned long long)s.uiSourceTargetGeneration,(unsigned long long)stageTotalNs,
            (unsigned long long)preDrawDiagnosticNs,(unsigned long long)drawDiagnosticNs,
            (unsigned long long)resolveDiagnosticNs,(unsigned long long)publishDiagnosticNs,
            firstCoveredAa1Span,input.spanCount,diagnosticPrefixDraws,diagnosticMixedDraws,diagnosticEmptyAa1,
            unsigned(diagnosticPromoted),(unsigned long long)diagnosticPromotionNs,unsigned(accepted));
    }
    s.diagnostic = nullptr;
    return accepted;
}

bool VehicleWindowGl50DS::submitHostMenu(const VehicleWindowHostMenu50DS& input,
    std::int32_t width, std::int32_t height, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableHostHud || !s.hostHudGeometry ||
        !s.hostHudProgram || !s.hostHudVertexBuffer || !s.hostHudIndexBuffer || !s.hostHudVertexArray ||
        !s.menuTargets[0] || !s.menuTargets[1] ||
        width <= 0 || height <= 0 || width > s.maximumWidth || height > s.maximumHeight ||
        !input.rowCount || input.rowCount > maxVehicleWindowMenuRows50DS || input.selectedRow >= input.rowCount)
        return fail(d, VehicleWindowError50DS::frame, "host menu requires idle opt-in renderer and bounded viewport/rows");
    if (input.retainRaceImage && s.raceWidth > 0 && s.raceHeight > 0 && !s.gl.blitFramebuffer)
        return fail(d, VehicleWindowError50DS::entryPoint, "retained host menu backdrop requires glBlitFramebuffer");
    const auto validText = [](const char* value) noexcept {
        if (!value) return false;
        for (U i = 0; i <= maxVehicleWindowMenuText50DS; ++i) {
            if (!value[i]) return true;
            if (i == maxVehicleWindowMenuText50DS || (value[i] != ' ' && !hostHudGlyph(value[i]))) return false;
        }
        return false;
    };
    if (!validText(input.title) || !validText(input.subtitle) || !validText(input.footer))
        return fail(d, VehicleWindowError50DS::frame, "host menu text is not bounded supported ASCII");
    for (U i = 0; i < input.rowCount; ++i)
        if (!validText(input.rows[i]))
            return fail(d, VehicleWindowError50DS::frame, "host menu row is not bounded supported ASCII");
    if (!s.hostHudGeometry->buildMenu(input, width, height))
        return fail(d, VehicleWindowError50DS::frame, "host menu exceeds cold glyph capacity");

    s.diagnostic = &d;
    bool accepted = s.check("before private host menu submission") &&
        (s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    const auto pending = static_cast<std::uint8_t>(s.menuPublished ^ 1u);
    if (accepted) {
        // Separate color-only targets leave the native adapter's publication
        // bank, both GPU vertex banks, and its last complete race image intact.
        s.gl.bindFramebuffer(framebuffer, s.menuTargets[pending]);
        s.gl.drawBuffer(colorAttachment); s.gl.readBuffer(colorAttachment);
        s.gl.viewport(0, 0, width, height);
        constexpr U disabled[] = {0x0BE2, 0x0B44, 0x0C11, 0x0B90, 0x8037, 0x0B20,
            0x8F9D, 0x0BD0, 0x809D, 0x80A0, 0x809E, 0x8C89, 0x8DB9, 0x864F, 0x0B71};
        for (U capability : disabled) s.gl.disable(capability);
        for (I i = 0; i < s.clipDistances; ++i) s.gl.disable(0x3000 + static_cast<U>(i));
        s.gl.colorMask(1, 1, 1, 1); s.gl.polygonMode(0x0408, 0x1B02);
        s.gl.clearColor(.015f, .025f, .045f, 1.f); s.gl.clear(colorBufferBit);
        if (input.retainRaceImage && s.raceWidth > 0 && s.raceHeight > 0) {
            s.gl.bindFramebuffer(readFramebuffer, s.targets[s.published]);
            s.gl.readBuffer(colorAttachment);
            s.gl.blitFramebuffer(0, 0, s.raceWidth, s.raceHeight,
                0, 0, width, height, colorBufferBit, 0x2600);
        }
        accepted = s.check("prepare independent host menu backdrop");
        s.frameHostHud = true;
        if (accepted) accepted = s.drawHostHud() && s.check("accept private host menu frame");
        s.frameHostHud = false;
        if (accepted) {
            s.menuPublished = pending; s.displayedTarget = s.menuTargets[pending];
            s.uiPrivatePublicationGeneration=0;
            s.uiSourcePublishedTargetGeneration=0;
            s.width = s.committedWidth = width; s.height = s.committedHeight = height;
            s.imageCommitted = true; d.hostMenuAccepted = true;
            if(s.request.output==VehicleWindowOutput50DS::headless)
                s.noteGameplayDisplayPending(s.displayedTarget,width,height);
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                accepted = s.copyToWindow();
                if (!accepted && d.error == VehicleWindowError50DS::gl)
                    d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    s.diagnostic = nullptr;
    return accepted;
}

bool VehicleWindowGl50DS::submitRetainedOriginalPause(
    const game::OriginalPauseNativeDraw50LA& owner,I width,I height,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"Pause requires initialized renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.frontendOnly||s.active||s.frameSet||s.diagnostic||s.uiBlendOldSnapshot||s.hasUiByteBlendNames()||
       s.pauseDerivedUiCreationFailed||!s.request.enableOriginalGameplayBlur||
       !s.retainedRaceDisplayCompleted||s.retainedRaceContainsPause||!s.imageCommitted||
       !s.retainedRaceCameraPublication||s.gameplayAwaitingTarget||
       width<=0||height<=0||width!=s.raceWidth||height!=s.raceHeight||
       width>s.maximumWidth||height>s.maximumHeight||!s.targets[s.published]||
       !s.menuTargets[0]||!s.menuTargets[1]||!s.gl.blitFramebuffer||
       !owner.frame()||owner.frame()->cameraPublication!=s.retainedRaceCameraPublication)
        return fail(d,VehicleWindowError50DS::frame,"Pause requires completed unchanged race extent and camera publication");
    if(!s.preparePauseDerivedFrame(&owner)||!s.pauseDerived||!s.pauseDerived->frame){
        d.originalPauseDerivedRejected=true;
        return fail(d,VehicleWindowError50DS::frame,"authentic bound Pause frame rejected");
    }
    s.diagnostic=&d;
    const auto oldWidth=s.width,oldHeight=s.height;
    s.width=width;s.height=height;
    const auto pending=static_cast<std::uint8_t>(s.menuPublished^1u);
    bool accepted=s.check("before retained original Pause")&&
        (s.request.output==VehicleWindowOutput50DS::headless||s.checkDefaultFramebuffer());
    if(accepted){
        // Normalize raster state without touching world/material/flame preparation.
        constexpr U disabled[]={0x0BE2,0x0B44,0x0C11,0x0B90,0x8037,0x0B20,
            0x8F9D,0x0BD0,0x809D,0x80A0,0x809E,0x8C89,0x8DB9,0x864F};
        for(U capability:disabled)s.gl.disable(capability);
        for(I i=0;i<s.clipDistances;++i)s.gl.disable(0x3000+static_cast<U>(i));
        s.gl.polygonMode(0x0408,0x1B02);
        accepted=s.drawPauseDerivedTargets(s.targets[s.published],s.menuTargets[pending])&&
            s.check("accept retained original Pause");
    }
    s.pauseDerived->frame=false;
    if(accepted){
        s.menuPublished=pending;s.displayedTarget=s.menuTargets[pending];
        s.uiPrivatePublicationGeneration=0;s.uiSourcePublishedTargetGeneration=0;
        s.committedWidth=width;s.committedHeight=height;s.imageCommitted=true;
        if(s.request.output==VehicleWindowOutput50DS::headless)
            s.noteGameplayDisplayPending(s.displayedTarget,width,height);
        else {
            accepted=s.copyToWindow();
            if(!accepted&&d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::presentation;
        }
    }else{s.width=oldWidth;s.height=oldHeight;}
    s.diagnostic=nullptr;return accepted;
}

bool VehicleWindowGl50DS::readbackRgba(std::uint8_t* destination, std::size_t capacityBytes,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (!s.imageCommitted)
        return fail(d, VehicleWindowError50DS::noImage, "no privately committed viewport to read");
    d.readbackWidth = s.committedWidth; d.readbackHeight = s.committedHeight;
    // Cold bounds already prove this product fits size_t; pending/failed frames
    // cannot replace these dimensions. Still validate before any readback GL call.
    d.readbackBytes = std::size_t(s.committedWidth) * std::size_t(s.committedHeight) * 4;
    if (!destination || capacityBytes < d.readbackBytes)
        return fail(d, VehicleWindowError50DS::readback, "RGBA destination is null or too small for committed viewport");
    s.diagnostic = &d;
    const bool result = s.copyRgba(destination);
    if (!result && d.error == VehicleWindowError50DS::gl) d.error = VehicleWindowError50DS::readback;
    s.diagnostic = nullptr;
    return result;
}

bool VehicleWindowGl50DS::readbackWindowFrontRgba(std::uint8_t* destination,
    std::size_t capacityBytes,I width,I height,VehicleWindowDiagnostic50DS& d) noexcept {
    return readbackWindowBufferRgba(destination,capacityBytes,width,height,true,nullptr,d);
}

bool VehicleWindowGl50DS::readbackWindowBackRgba(GameHost50ER& host,std::uint8_t* destination,
    std::size_t capacityBytes,I width,I height,VehicleWindowDiagnostic50DS& d) noexcept {
    return readbackWindowBufferRgba(destination,capacityBytes,width,height,false,&host,d);
}

bool VehicleWindowGl50DS::readbackWindowBufferRgba(std::uint8_t* destination,
    std::size_t capacityBytes,I width,I height,bool front,const GameHost50ER* expectedHost,
    VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"window readback requires initialized renderer");
    auto& s=*storage_;
    if(!s.current(d))return false;
    if((expectedHost && s.request.context!=static_cast<const void*>(expectedHost)) ||
        s.request.output!=VehicleWindowOutput50DS::defaultBackBuffer || s.active || s.frameSet || s.diagnostic ||
        !s.imageCommitted || width<=0 || height<=0 || width!=s.committedWidth || height!=s.committedHeight ||
        std::uint64_t(width)*std::uint64_t(height)>std::numeric_limits<std::size_t>::max()/4u)
        return fail(d,VehicleWindowError50DS::readback,"window-buffer readback requires idle real window owner and matching committed drawable");
    d.readbackWidth=width;d.readbackHeight=height;
    d.readbackBytes=std::size_t(width)*std::size_t(height)*4u; // Existing cold capacity bounds.
    if(!destination || capacityBytes<d.readbackBytes)
        return fail(d,VehicleWindowError50DS::readback,"window RGBA destination is null or too small");
    s.diagnostic=&d;
    bool accepted=s.check("before actual window framebuffer readback");
    I previousRead=0,defaultRead=0,previousPackBuffer=0;
    constexpr U packNames[]={0x0D05,0x0D02,0x0D03,0x0D04,0x0D00,0x0D01,0x806C,0x806B};
    I previousPack[8]{};
    bool restore=false;
    if(accepted){
        s.gl.getIntegerv(0x8CAA,&previousRead); // GL_READ_FRAMEBUFFER_BINDING
        s.gl.getIntegerv(0x88ED,&previousPackBuffer); // GL_PIXEL_PACK_BUFFER_BINDING
        for(unsigned i=0;i<8;++i)s.gl.getIntegerv(packNames[i],&previousPack[i]);
        accepted=s.check("query prior window readback binding");
    }
    if(accepted){
        s.gl.bindFramebuffer(readFramebuffer,0);s.gl.getIntegerv(0x0C02,&defaultRead);
        restore=true;accepted=s.check("query default framebuffer read buffer");
    }
    if(accepted){
        s.gl.readBuffer(front?0x0404:0x0405); // GL_FRONT after swap / GL_BACK strictly before swap.
        s.drawGl.bindBuffer(pixelPackBuffer,0);
        s.gl.pixelStorei(0x0D05,1);s.gl.pixelStorei(0x0D02,0);
        s.gl.pixelStorei(0x0D03,0);s.gl.pixelStorei(0x0D04,0);
        s.gl.pixelStorei(0x0D00,0);s.gl.pixelStorei(0x0D01,0);
        s.gl.pixelStorei(0x806C,0);s.gl.pixelStorei(0x806B,0);
        accepted=s.check("configure actual window RGBA readback");
    }
    if(accepted){
        const U status=s.gl.checkFramebufferStatus(readFramebuffer);
        accepted=s.check("check actual window readback framebuffer");
        if(accepted&&status!=framebufferComplete){d.framebufferStatus=status;
            accepted=fail(d,VehicleWindowError50DS::readback,"actual window framebuffer is incomplete");}
    }
    if(accepted){d.readbackBufferMayBeModified=true;
        s.gl.readPixels(0,0,width,height,rgba,unsignedByte,destination);
        accepted=s.check(front?"read actual swapped window front-buffer RGBA bytes":
            "read actual pre-swap window back-buffer RGBA bytes");}
    if(restore){
        s.gl.readBuffer(static_cast<U>(defaultRead));s.gl.bindFramebuffer(readFramebuffer,static_cast<U>(previousRead));
        for(unsigned i=0;i<8;++i)s.gl.pixelStorei(packNames[i],previousPack[i]);
        s.drawGl.bindBuffer(pixelPackBuffer,static_cast<U>(previousPackBuffer));
        const bool restored=s.check("restore window readback binding");accepted=accepted&&restored;
    }
    if(!accepted&&d.error==VehicleWindowError50DS::gl)d.error=VehicleWindowError50DS::readback;
    s.diagnostic=nullptr;return accepted;
}

bool VehicleWindowGl50DS::finishSubmittedWork(VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if(s.rcParticleWindow&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"RC completion requires the idle creating thread");
    if((s.gbParticleWindow||s.mcParticleWindow)&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"GB particle transaction requires idle creating thread");
    Storage::RcParticleAttemptScope rcParticleAttemptScope{s,d};
    Storage::GbParticleAttemptScope gbParticleAttemptScope{s,d};
    Storage::McParticleAttemptScope mcParticleAttemptScope{s,d};
    if (!s.current(d)) return false;
    if (!s.imageCommitted)
        return fail(d, VehicleWindowError50DS::noImage, "no privately committed frame to finish");
    s.diagnostic = &d;
    bool result = s.check("before explicit submitted-work completion");
    if (result) {
        s.gl.finish();
        result = s.check("complete prior submitted GL work");
    }
    d.submittedWorkCompleted = result;
    if(result)s.acceptCompletedTrackVisibility();
    else s.trackVisibilityPendingTarget=0;
    s.diagnostic = nullptr;
    if(result)result=s.completeRcParticleAttempt(d);
    if(result)result=s.completeGbParticleAttempt(d);
    if(result)result=s.completeMcParticleAttempt(d);
    return result;
}

bool VehicleWindowGl50DS::completedRaStaticFire(
    ra_static_fire_visual50LA::CompletedDrawReceipt& out,VehicleWindowDiagnostic50DS& d) const noexcept {
    out={};d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"FireP0t completion requires live renderer");
    const auto& s=*storage_;
    if(!s.raStaticFireOwner||s.raStaticFireRenderer!=this||
       s.raStaticFireOwner->rendererPublication_!=this||
       s.raStaticFirePublicationSerial!=s.raStaticFireOwner->rendererPublicationSerial_)
        return fail(d,VehicleWindowError50DS::material,"FireP0t publication identity differs");
    // Reuse the real idle/current/GPU-completion boundary. These outputs are
    // private temporaries, never caller-supplied claims of submitted geometry.
    const std::uint32_t indices[4]={0,1,2,3};std::uint8_t submitted[4]{};
    VehicleWindowTrackVisibilityReceipt50DS receipt;
    if(!completedTrackSpanVisibility(indices,submitted,4,receipt,d)||!s.texturesCurrent(d))return false;
    if(!s.raStaticFireOwner->nativeCompletionLive_||receipt.sourceSpanCount!=4||receipt.completedGameplayGeneration!=s.raStaticFireOwner->nativeCompletedGeneration_)
        return fail(d,VehicleWindowError50DS::frame,"FireP0t native completed generation differs");
    for(U i=0;i<4;++i)if(bool(submitted[i])!=s.raStaticFireOwner->nativeSubmitted_[i])
        return fail(d,VehicleWindowError50DS::frame,"FireP0t completed span evidence differs");
    out.owner_=s.raStaticFireOwner;out.publication_=s.raStaticFirePublicationSerial;
    out.generation_=receipt.completedGameplayGeneration;return true;
}

bool VehicleWindowGl50DS::completedTrackSpanVisibility(const std::uint32_t* indices,
    std::uint8_t* submitted,std::uint32_t count,VehicleWindowTrackVisibilityReceipt50DS& receipt,
    VehicleWindowDiagnostic50DS& d) const noexcept {
    d={};receipt={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"track visibility requires initialized renderer");
    const auto& s=*storage_;if(!s.current(d))return false;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"track visibility requires idle renderer");
    if(!s.trackVisibilityCompletedValid)
        return fail(d,VehicleWindowError50DS::noImage,"no completed gameplay track visibility receipt");
    if(count>s.trackSpanCount || (count && (!indices || !submitted)))
        return fail(d,VehicleWindowError50DS::frame,"track visibility output or count is invalid");
    for(U i=0;i<count;++i)if(indices[i]>=s.trackSpanCount)
        return fail(d,VehicleWindowError50DS::frame,"track visibility source span index is outside the cold table");
    for(U i=0;i<count;++i)submitted[i]=s.trackSpans[indices[i]].submittedCompletedGameplay?1u:0u;
    receipt.completedGameplayGeneration=s.trackVisibilityCompletedGeneration;
    receipt.sourceSpanCount=s.trackSpanCount;return true;
}

namespace {
bool sameDisplayFadeSample(const game::OriginalVideoBlockingSample& a,
    const game::OriginalVideoBlockingSample& b) noexcept {
    return a.direction==b.direction && a.ordinal==b.ordinal && a.gsRgbaq==b.gsRgbaq &&
        a.alpha==b.alpha && a.sourceCopyPC==b.sourceCopyPC;
}
}
OriginalVideoBlockingNativeBackend::~OriginalVideoBlockingNativeBackend() noexcept {
    // Teardown is cancellation, never successful completion of the fade.
    if(!cancelAndRetire())std::terminate();
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::failure(std::uint32_t pc) const noexcept {
    return {diagnostic_.error==VehicleWindowError50DS::noImage?
        game::OriginalCameraStartStatus::rendererNoImage:game::OriginalCameraStartStatus::videoBackend,pc};
}
bool OriginalVideoBlockingNativeBackend::ownersCurrent() noexcept {
    if(!renderer_.initialized())return fail(diagnostic_,VehicleWindowError50DS::notInitialized,"display fade requires live renderer");
    auto& s=*renderer_.storage_;
    if(s.frontendOnly)return fail(diagnostic_,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay fade capability");
    if(!s.current(diagnostic_,this))return false;
    if(s.request.context!=&host_ || ((s.request.output==VehicleWindowOutput50DS::defaultBackBuffer)!=window_))
        return fail(diagnostic_,VehicleWindowError50DS::context,"display fade requires renderer's exact current GameHost50ER");
    if(s.active || s.frameSet || s.diagnostic)
        return fail(diagnostic_,VehicleWindowError50DS::frame,"display fade requires idle non-reentrant renderer");
    return checkHost_(host_,window_,width_,height_,diagnostic_);
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::begin(
    game::OriginalVideoFadeDirection direction,const game::OriginalVideoFadeState& video) {
    using S=game::OriginalCameraStartStatus;
    const bool async=use_==OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0;
    const auto pc=async?0x25e610u:(direction==game::OriginalVideoFadeDirection::out260850?0x260850u:0x261630u);
    if(phase_==OriginalVideoNativePhase::failed)return failure(pc);
    diagnostic_={};hostDiagnostic_={};
    if(phase_==OriginalVideoNativePhase::sourceReady && direction==direction_)
        return ownersCurrent()?game::OriginalCameraStartResult{S::complete,pc}:failure(pc);
    if((phase_!=OriginalVideoNativePhase::idle && phase_!=OriginalVideoNativePhase::retired) ||
       (use_!=OriginalVideoNativeUse::blockingSourceLoops && !async) ||
       (direction!=game::OriginalVideoFadeDirection::out260850 && direction!=game::OriginalVideoFadeDirection::in261630) ||
       (async?((video.flags404Low6&7u)!=(direction==game::OriginalVideoFadeDirection::out260850?5u:6u)):
           bool(video.flags404Low6&4u)) ||
       !(video.flags404Low6&(direction==game::OriginalVideoFadeDirection::out260850?1u:2u)) ||
       (direction==game::OriginalVideoFadeDirection::out260850 && (video.flags404Low6&32u)))
        return {S::sourcePath,pc};
    if(phase_==OriginalVideoNativePhase::retired)width_=height_=0;
    if(!ownersCurrent())return failure(pc);
    auto& s=*renderer_.storage_;
    if(!s.imageCommitted || !s.raceWidth || !s.raceHeight ||
       s.displayedTarget!=s.targets[s.published] || s.committedWidth!=s.raceWidth || s.committedHeight!=s.raceHeight){
        fail(diagnostic_,VehicleWindowError50DS::noImage,"display fade requires current real race image, never UI or invented cold image");
        return failure(pc);
    }
    width_=s.raceWidth;height_=s.raceHeight;
    if(!ownersCurrent()){width_=height_=0;return failure(pc);}
    s.diagnostic=&diagnostic_;
    bool accepted=s.check("before native display-fade begin") && s.loadDisplayFadeFunctions();
    if(!accepted){s.diagnostic=nullptr;width_=height_=0;return failure(pc);}
    // Install the exclusive borrow before every fallible allocation/copy.
    // Failed work retains it; only real retirement/cancellation unlocks it.
    direction_=direction;nextOrdinal_=windowSwaps_=headlessCompletions_=0;last_={};pending_={};pendingBank_=0;
    s.displayFadeOwner=this;phase_=OriginalVideoNativePhase::failed;
    s.gl.finish();accepted=s.check("complete race source before display-fade snapshot");
    if(accepted && !s.deleteDisplayFadeNames(diagnostic_))
        accepted=fail(diagnostic_,VehicleWindowError50DS::cleanup,"display-fade cached name deletion remains pending");
    I textureLimit=0,textureUnits=0;
    if(accepted){
        s.gl.getIntegerv(0x0D33,&textureLimit);s.gl.getIntegerv(0x8872,&textureUnits);
        accepted=s.check("query native display-fade texture limits");
        if(accepted && (width_>textureLimit || height_>textureLimit || textureUnits<1))
            accepted=fail(diagnostic_,VehicleWindowError50DS::limits,"display-fade texture dimensions or unit unavailable");
    }
    if(accepted)accepted=s.createDisplayFadeTarget(s.displayFadeSourceTarget,s.displayFadeSourceColor,width_,height_) &&
        s.createDisplayFadeTarget(s.displayFadeTargets[0],s.displayFadeColors[0],width_,height_) &&
        s.createDisplayFadeTarget(s.displayFadeTargets[1],s.displayFadeColors[1],width_,height_) && s.createDisplayFadeProgram();
    if(accepted){
        s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
        s.gl.bindFramebuffer(readFramebuffer,s.targets[s.published]);s.gl.readBuffer(colorAttachment);
        s.gl.bindFramebuffer(drawFramebuffer,s.displayFadeSourceTarget);s.gl.drawBuffer(colorAttachment);
        s.gl.blitFramebuffer(0,0,width_,height_,0,0,width_,height_,colorBufferBit,0x2600);
        accepted=s.check("snapshot actual committed race into distinct immutable display-fade source");
    }
    const bool restored=s.restoreDisplayFadeBindings(width_,height_);accepted=accepted && restored;
    if(accepted){s.gl.finish();accepted=s.check("complete immutable display-fade source snapshot");}
    if(accepted){diagnostic_.submittedWorkCompleted=true;phase_=OriginalVideoNativePhase::sourceReady;}
    s.diagnostic=nullptr;
    return accepted?game::OriginalCameraStartResult{S::complete,pc}:failure(pc);
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::copyDrawAndComplete(
    const game::OriginalVideoBlockingSample& sample) {
    using S=game::OriginalCameraStartStatus;
    const bool async=use_==OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0;
    const auto pc=async?0x25e610u:(direction_==game::OriginalVideoFadeDirection::out260850?0x262c70u:0x262d80u);
    if(phase_==OriginalVideoNativePhase::failed)return failure(pc);
    diagnostic_={};hostDiagnostic_={};
    if(sample.direction!=direction_ || sample.sourceCopyPC!=(async?0u:pc) ||
       (phase_!=OriginalVideoNativePhase::sourceReady && phase_!=OriginalVideoNativePhase::presentationPending))
        return {S::owner,pc};
    // An acknowledged exact retry never recopies, redraws, or swaps twice.
    if(nextOrdinal_ && sameDisplayFadeSample(sample,last_))return {S::complete,pc};
    if(sample.ordinal!=nextOrdinal_ || nextOrdinal_==std::numeric_limits<std::uint64_t>::max() ||
       (phase_==OriginalVideoNativePhase::presentationPending && !sameDisplayFadeSample(sample,pending_)))
        return {S::owner,pc};
    if(!ownersCurrent())return failure(pc);
    auto& s=*renderer_.storage_;
    if(s.displayFadeOwner!=this)return {S::owner,pc};
    if(phase_==OriginalVideoNativePhase::sourceReady){
        pending_=sample;pendingBank_=static_cast<std::uint8_t>(nextOrdinal_&1u);
        s.diagnostic=&diagnostic_;
        bool accepted=s.check("before display-fade source recopy");
        if(accepted){
            // Blocking262C70/262D80 or explicit native async retained-image
            // recopy EVERY sample. Async sourceCopyPC=0 makes no blocking-PC
            // attribution. Never sample an effect or previous sample.
            s.gl.disable(0x0C11);s.gl.disable(0x8DB9);
            s.gl.bindFramebuffer(readFramebuffer,s.displayFadeSourceTarget);s.gl.readBuffer(colorAttachment);
            s.gl.bindFramebuffer(drawFramebuffer,s.displayFadeTargets[pendingBank_]);s.gl.drawBuffer(colorAttachment);
            s.gl.blitFramebuffer(0,0,width_,height_,0,0,width_,height_,colorBufferBit,0x2600);
            accepted=s.check("recopy immutable race source for display-fade sample");
        }
        if(accepted){
            s.gl.bindFramebuffer(framebuffer,s.displayFadeTargets[pendingBank_]);
            s.gl.drawBuffer(colorAttachment);s.gl.readBuffer(colorAttachment);
            if(s.gl.clipControl)s.gl.clipControl(0x8CA1,0x935E);
            s.gl.viewport(0,0,width_,height_);
            constexpr U disabled[]={0x0BE2,0x0B44,0x0C11,0x0B90,0x8037,0x0B20,
                0x8F9D,0x0BD0,0x809D,0x80A0,0x809E,0x809F,0x8E51,0x8C89,0x8DB9,0x864F,0x0BF2,0x0B71};
            for(U capability:disabled)s.gl.disable(capability);
            for(I i=0;i<s.clipDistances;++i)s.gl.disable(0x3000+static_cast<U>(i));
            s.gl.colorMask(1,1,1,1);s.gl.depthMask(0);s.gl.polygonMode(0x0408,0x1B02);
            s.gl.useProgram(s.displayFadeProgram);s.drawGl.bindVertexArray(s.displayFadeVertexArray);
            s.gl.activeTexture(textureUnit0);s.gl.bindTexture(texture2D,s.displayFadeSourceColor);
            if(s.gl.bindSampler)s.gl.bindSampler(0,0);
            s.gl.uniform1i(s.displayFadeImageUniform,0);
            s.gl.uniform4f(s.displayFadeColorUniform,float(sample.gsRgbaq&255u),float((sample.gsRgbaq>>8)&255u),
                float((sample.gsRgbaq>>16)&255u),float((sample.gsRgbaq>>24)&255u));
            accepted=s.check("bind immutable race source and actual packed RGBAQ bytes");
            if(accepted){s.gl.drawArrays(0x0004,0,3);accepted=s.check("draw native full-race display-fade sample");}
        }
        // Exact final effect image, staged before its real finish/swap.
        if(accepted)accepted=s.stageGameplayDisplayedHistory(s.displayFadeTargets[pendingBank_],width_,height_);
        const bool restored=s.restoreDisplayFadeBindings(width_,height_);accepted=accepted && restored;
        if(accepted){s.gl.finish();accepted=s.check("complete private display-fade sample");}
        if(accepted && window_){
            accepted=s.checkDefaultFramebuffer();
            if(accepted){
                s.gl.bindFramebuffer(readFramebuffer,s.displayFadeTargets[pendingBank_]);s.gl.readBuffer(colorAttachment);
                s.gl.bindFramebuffer(drawFramebuffer,0);s.gl.drawBuffer(backBuffer);
                accepted=s.check("bind completed display-fade sample for window copy");
            }
            if(accepted){
                diagnostic_.defaultFramebufferMayBeModified=true;
                s.gl.blitFramebuffer(0,0,width_,height_,0,0,width_,height_,colorBufferBit,0x2600);
                accepted=s.check("copy completed display-fade sample to real window back buffer");
            }
            const bool windowRestored=s.restoreDisplayFadeBindings(width_,height_);accepted=accepted && windowRestored;
            if(accepted){s.gl.finish();accepted=s.check("complete display-fade back-buffer copy before swap");}
        }
        s.diagnostic=nullptr;
        if(!accepted){phase_=OriginalVideoNativePhase::failed;return failure(pc);}
        diagnostic_.submittedWorkCompleted=true;phase_=OriginalVideoNativePhase::presentationPending;
    }
    if(window_){
        // Actual50DQ swap returns false only before SDL_GL_SwapWindow is
        // called. A false result therefore leaves this SAME sample pending;
        // it may retry the swap, without repeating draw or back-buffer copy.
        if(!swapHost_(host_,hostDiagnostic_)){
            fail(diagnostic_,VehicleWindowError50DS::presentation,"GameHost50ER display-fade swap not performed");
            return failure(pc);
        }
        ++windowSwaps_;
    }else ++headlessCompletions_;
    // CPU-only acceptance after actual display completion, never another swap.
    s.acceptGameplayDisplayedHistory();
    // No fallible operation follows a successful swap. Publish only a fully
    // completed image; alternating surfaces preserve the previous committed
    // sample across every failed write. Source race banks remain untouched.
    s.displayedTarget=s.displayFadeTargets[pendingBank_];s.imageCommitted=true;
    s.width=s.committedWidth=width_;s.height=s.committedHeight=height_;
    s.uiPrivatePublicationGeneration=0;s.uiSourcePublishedTargetGeneration=0;
    last_=sample;++nextOrdinal_;phase_=OriginalVideoNativePhase::sourceReady;
    return {S::complete,pc};
}
bool OriginalVideoBlockingNativeBackend::retire(bool cancelled) noexcept {
    if(!renderer_.storage_ || renderer_.storage_->displayFadeOwner!=this){
        if(phase_==OriginalVideoNativePhase::idle || phase_==OriginalVideoNativePhase::retired ||
           phase_==OriginalVideoNativePhase::cancelled)return true;
        return fail(diagnostic_,VehicleWindowError50DS::frame,"display-fade retirement lost its actual renderer owner");
    }
    auto& s=*renderer_.storage_;
    // Cancellation must remain possible after a resize. It changes no window
    // or private publication; it only drains work and restores native state.
    if(!renderer_.initialized() || !s.current(diagnostic_,this) || s.active || s.frameSet || s.diagnostic)return false;
    s.diagnostic=&diagnostic_;
    bool accepted=s.check("before display-fade retirement");
    if(accepted)accepted=s.restoreDisplayFadeBindings(width_,height_);
    if(accepted){s.gl.finish();accepted=s.check("complete display-fade state restoration and retirement");}
    if(accepted && s.displayFadeDeletionPending && !s.deleteDisplayFadeNames(diagnostic_))
        accepted=fail(diagnostic_,VehicleWindowError50DS::cleanup,"display-fade retirement still owns unresolved deleted names");
    s.diagnostic=nullptr;
    if(!accepted)return false;
    diagnostic_.submittedWorkCompleted=true;s.displayFadeOwner=nullptr;
    phase_=cancelled?OriginalVideoNativePhase::cancelled:OriginalVideoNativePhase::retired;
    return true;
}
game::OriginalCameraStartResult OriginalVideoBlockingNativeBackend::restoreAndComplete(
    game::OriginalVideoFadeDirection direction) {
    using S=game::OriginalCameraStartStatus;
    const auto pc=use_==OriginalVideoNativeUse::asyncFrozenUnderlay25B5D0?0x25e610u:
        (direction_==game::OriginalVideoFadeDirection::out260850?0x261594u:0x261df0u);
    if(direction!=direction_)return {S::owner,pc};
    if(phase_==OriginalVideoNativePhase::retired)return {S::complete,pc};
    // Failed/unpublished samples must never let the numeric owner publish
    // final alpha/flags. Only the separate cancellation method can retire it.
    if(phase_==OriginalVideoNativePhase::failed)return failure(pc);
    if(phase_!=OriginalVideoNativePhase::sourceReady && phase_!=OriginalVideoNativePhase::restoring)
        return {S::owner,pc};
    diagnostic_={};phase_=OriginalVideoNativePhase::restoring;
    return retire(false)?game::OriginalCameraStartResult{S::complete,pc}:failure(pc);
}
bool OriginalVideoBlockingNativeBackend::cancelAndRetire() noexcept {
    diagnostic_={};return retire(true);
}

bool VehicleWindowGl50DS::idleForRaceReset50LA(VehicleWindowDiagnostic50DS& d) const noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"race reset requires initialized renderer");
    const auto& s=*storage_;
    if(!s.current(d))return false;
    if(s.active||s.frameSet||s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"race reset requires idle native submission ownership");
    return true;
}

bool VehicleWindowGl50DS::garageFirstCameraViewportIdle50KG(VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!idleForRaceReset50LA(d))return false;
    const auto& s=*storage_;
    // Sealed Main/Rider/Career RGBA frames own pixels, not source Scene0.
    // They do not execute26F940 and must not invalidate canonical selection.
    // Gameplay uses a different creation path and is not admitted here.
    if(!s.frontendOnly||s.nativeGlFaultObserved50KG||s.frameSet)
        return fail(d,VehicleWindowError50DS::frame,"Garage first Camera requires untouched native frontend scene selection");
    return true;
}

bool VehicleWindowGl50DS::frontendLocalizationIdle50KG(VehicleWindowDiagnostic50DS& d) const noexcept {
    if(!idleForRaceReset50LA(d))return false;
    const auto& s=*storage_;
    if(!s.frontendOnly||s.nativeGlFaultObserved50KG)
        return fail(d,VehicleWindowError50DS::frame,"localization requires the retained no-error frontend renderer");
    // Do not accept closed/replaced UI banks merely because the GL context is
    // still current. Their real context/publication borrows must remain valid.
    const auto* cpu=s.request.originalUiCpuTextures;
    const auto* gpu=s.request.originalUiTextures;
    if(!cpu||!cpu->loaded()||!gpu||!cpu->textureCount()||cpu->textureCount()!=gpu->textureCount()||
       !gpu->validBorrow(s.request.context,s.request.isCurrent,s.uiTexturePublication))
        return fail(d,VehicleWindowError50DS::material,"localization requires the exact live UI bank publication");
    if(gpu->riderSparse()){
        const auto* bank=gpu->riderBank();const auto images=bank?bank->images():nullptr;
        if(s.riderTextureBorrow!=gpu||!gpu->riderSession()||!images||images->textures()!=cpu)
            return fail(d,VehicleWindowError50DS::material,"localization requires the actual retained sparse Rider bank");
        for(U i=0;i<cpu->textureCount();++i){
            const auto* source=cpu->texture(i);const auto* image=gpu->asset(i);
            if(!source||(image&&(!image->name||image->identity!=source->identity)))
                return fail(d,VehicleWindowError50DS::material,"localization sparse UI identity changed");
        }
    }else if(!gpu->matchesSourceBank(*cpu,s.uiTexturePublication))
        return fail(d,VehicleWindowError50DS::material,"localization CPU/UI upload identity changed");
    return true;
}

bool VehicleWindowGl50DS::finishInitializedContextWork(VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (s.contextDrainGeneration == std::numeric_limits<std::uint64_t>::max())
        return fail(d, VehicleWindowError50DS::submission, "context drain generation exhausted");
    s.diagnostic = &d;
    bool result = s.check("before explicit initialized-context completion");
    if (result) {
        s.gl.finish();
        result = s.check("complete prior initialized-context GL work");
    }
    if (result) {
        ++s.contextDrainGeneration;
        d.contextWorkCompleted = true;
        d.contextDrainGeneration = s.contextDrainGeneration;
    }
    s.diagnostic = nullptr;
    return result;
}

bool VehicleWindowGl50DS::completeGameplayDisplayImpl(GameHost50ER& host,
    GameHostDiagnostic50ER& hostDiagnostic,
    bool (*checkHost)(GameHost50ER&,bool,I,I,VehicleWindowDiagnostic50DS&) noexcept,
    bool (*swapHost)(GameHost50ER&,GameHostDiagnostic50ER&) noexcept,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"gameplay display requires initialized renderer");
    auto& s=*storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(s.rcParticleWindow&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"RC display completion requires the idle creating thread");
    if((s.gbParticleWindow||s.mcParticleWindow)&&(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic))
        return fail(d,VehicleWindowError50DS::frame,"GB particle transaction requires idle creating thread");
    Storage::RcParticleAttemptScope rcParticleAttemptScope{s,d};
    Storage::GbParticleAttemptScope gbParticleAttemptScope{s,d};
    Storage::McParticleAttemptScope mcParticleAttemptScope{s,d};
    if(!s.current(d))return false;
    const bool window=s.request.output==VehicleWindowOutput50DS::defaultBackBuffer;
    if(!s.request.enableOriginalGameplayBlur || s.request.context!=static_cast<void*>(&host) ||
        !checkHost || !swapHost)
        return fail(d,VehicleWindowError50DS::context,"gameplay display requires opted-in renderer's exact current GameHost");
    if(s.active || s.frameSet || s.diagnostic || !s.imageCommitted ||
        !s.gameplayAwaitingTarget || s.gameplayAwaitingTarget!=s.displayedTarget ||
        s.gameplayAwaitingWidth!=s.committedWidth || s.gameplayAwaitingHeight!=s.committedHeight ||
        (s.displayedTarget!=s.targets[s.published] && s.displayedTarget!=s.menuTargets[s.menuPublished]))
        return fail(d,VehicleWindowError50DS::frame,"gameplay display needs new successful race or host-menu publication");
    if(!checkHost(host,window,s.committedWidth,s.committedHeight,d))return false;
    s.diagnostic=&d;
    const auto stageHistory=[&]() noexcept {
        GameplayDisplayWallScope50DS timing(s.request.enableGameplayDisplayWallTiming,d.gameplayHistoryStageWall);
        return s.stageGameplayDisplayedHistory(s.displayedTarget,s.committedWidth,s.committedHeight);
    };
    bool accepted=s.check("before actual gameplay display completion") && stageHistory();
    // All fallible GPU work precedes the real swap. The staged image is an
    // independent copy, so no subsequent frame can mutate accepted history.
    s.gl.bindFramebuffer(framebuffer,0);
    const bool restored=s.check("restore framebuffer before gameplay display completion");
    accepted=accepted && restored;
    if(accepted){
        {
            GameplayDisplayWallScope50DS timing(s.request.enableGameplayDisplayWallTiming,d.gameplayFinishWall);
            s.gl.finish();
        }
        accepted=s.check("complete final gameplay image and staged history before display");
    }
    d.submittedWorkCompleted=accepted;
    s.diagnostic=nullptr;
    if(!accepted){s.trackVisibilityPendingTarget=0;return false;}
    const auto swapWindow=[&]() noexcept {
        GameplayDisplayWallScope50DS timing(s.request.enableGameplayDisplayWallTiming,d.gameplaySwapWall);
        return swapHost(host,hostDiagnostic);
    };
    if(window && !swapWindow()) {
        s.trackVisibilityPendingTarget=0;
        return fail(d,VehicleWindowError50DS::presentation,"GameHost gameplay window swap was not submitted");
    }
    if(s.displayedTarget==s.targets[s.published])s.retainedRaceDisplayCompleted=true;
    s.acceptGameplayDisplayedHistory();
    s.acceptCompletedTrackVisibility();
    if(!s.completeRcParticleAttempt(d)||!s.completeGbParticleAttempt(d)||!s.completeMcParticleAttempt(d))return false;
    d.gameplayDisplayHistoryAccepted=true;
    d.gameplayDisplayHistoryGeneration=s.gameplayHistoryGeneration;
    d.gameplayWindowSwapSubmitted=window;d.gameplayHeadlessDisplayCompleted=!window;
    return true;
}

bool VehicleWindowGl50DS::clearGameplayBlurStrengths(VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if(!initialized())return fail(d,VehicleWindowError50DS::notInitialized,"gameplay blur clear requires initialized renderer");
    auto& s=*storage_;
    if(s.frontendOnly)return fail(d,VehicleWindowError50DS::request,"dedicated frontend owns no gameplay capability");
    if(!s.current(d))return false;
    if(!s.request.enableOriginalGameplayBlur || s.active || s.frameSet || s.diagnostic)
        return fail(d,VehicleWindowError50DS::frame,"gameplay blur clear requires idle opted-in renderer");
    game::clearGameplayBlurVideoStrengths(s.gameplayBlurVideo);
    return true;
}

void VehicleWindowGl50DS::logRaCapturePixelProvenance() noexcept {
    if(!initialized())return;
    auto& s=*storage_;
    if(s.thread!=std::this_thread::get_id()||s.active||s.frameSet||s.diagnostic||
       !s.imageCommitted||!s.raWorld||!s.raWorldFramePrepared)return;
    VehicleWindowDiagnostic50DS observation;
    if(!s.current(observation)||!s.raWorldFrameCurrent(observation))return;
    s.traceRaPixelProvenance();
}

bool VehicleWindowGl50DS::close(VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if(sessionDependentPins50LA_){
        d.cleanupIncomplete=true;
        return fail(d,VehicleWindowError50DS::request,"dependent primitive retirement must precede renderer close");
    }
    if (!storage_) return true;
    storage_->uiDirectVideoLifetime.reset();
    const bool result = storage_->cleanup(d);
    if (!d.cleanupIncomplete) {
        if(storage_->riderTextureBorrow&&!storage_->riderTextureBorrow->releaseRiderBorrow(storage_.get())){
            d.cleanupIncomplete=true;return fail(d,VehicleWindowError50DS::cleanup,"Rider renderer close borrower mismatch");}
        storage_->riderTextureBorrow=nullptr;storage_.reset();
    }
    return result;
}
bool VehicleWindowGl50DS::initialized() const noexcept { return storage_ && storage_->initialized; }
}
