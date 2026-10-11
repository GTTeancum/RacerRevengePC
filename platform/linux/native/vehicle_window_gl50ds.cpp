#include "vehicle_window_gl50ds.h"
#include "original_ui_rider_draw_contribution50kg.h"
#include "native_pod_shadow50hm.h"
#include "native_pod_shadow_glsl50hm.h"
#include "../../../src/game/pod_cables50fd.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
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
constexpr U nativeShadowCasterCount = 3 * nativeShadowRacerCount;
constexpr game::VehicleDrawComponent50CM components[] = {
    game::VehicleDrawComponent50CM::chariot, game::VehicleDrawComponent50CM::leftEngine,
    game::VehicleDrawComponent50CM::rightEngine
};
bool fail(VehicleWindowDiagnostic50DS& d, VehicleWindowError50DS error,
    const char* operation) noexcept {
    if (d.error == VehicleWindowError50DS::none) { d.error = error; d.operation = operation; }
    return false;
}
U componentIndex(game::VehicleDrawComponent50CM component) noexcept {
    for (U c = 0; c < 3; ++c) if (components[c] == component) return c;
    return 3;
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
    return true;
}
bool validateMaterials(const VehicleWindowRequest50DS& request,
    const assets::VehicleDrawAssets50CR& owner, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& input = request.materials;
    if (request.mode == VehicleWindowMode50DS::viewerNeutralUntextured) {
        if (input.textures)
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
    for (U c = 0; c < 3; ++c) {
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
    d.component = 3;
    d.material = d.texture = assets::unresolvedMaterialTexture50EA;
    return true;
}
bool validateTrack(const VehicleWindowRequest50DS& request, VehicleWindowDiagnostic50DS& d) noexcept {
    const auto& track = request.track;
    const auto& fog = track.worldFog;
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
            !track.maximumProjectionBytes) ||
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
            if (!validateMaterialPolicy(span.material, d)) return false;
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
        world.sourceProjectionCount || world.maximumProjectionBytes))
        return fail(d, VehicleWindowError50DS::limits,
            "dynamic world requires bounded textured topology without static culling/projection");
    const bool result = validateTrack(selected, d);
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
            !material.unlit || sky.spans[i].discardZeroAlpha || sky.spans[i].sourcePass != 1 ||
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
    // Canonical 50FD strip topology, two nine-sample cables, two triangles
    // per adjacent sample pair. No topology is generated from visual guesses.
    constexpr U offsets[] = {0, 1, 2, 2, 1, 3};
    const U trianglePair = index / 6;
    return (trianglePair / 8) * 18 + (trianglePair % 8) * 2 + offsets[index % 6];
}
bool validateCableFrame(const game::PodCableMesh50FD& mesh, VehicleWindowDiagnostic50DS& d) noexcept {
    static_assert(game::podCableVertexCount50FD == 36 && game::podCableIndexCount50FD == 96,
        "50DS fixed storage requires the admitted 50FD two-cable topology");
    for (const auto& vertex : mesh.vertices) {
        for (float value : vertex.position)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::frame, "cable vertex contains nonfinite world position");
        for (float value : vertex.uv)
            if (!std::isfinite(value))
                return fail(d, VehicleWindowError50DS::frame, "cable vertex contains nonfinite source UV");
    }
    for (U i = 0; i < game::podCableIndexCount50FD; ++i)
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
        char place[32]{}, finish[40]{};
        std::snprintf(place, sizeof(place), "PLACE %u / %u", input.resultsFinishPlace, input.raceRacerCount);
        // Finish time is the explicit sealed HOST race observation; the native
        // CC0 lap durations below retain their separate source provenance.
        const U finishCentiseconds = static_cast<U>(input.resultsFinishSeconds * 100.0 + .5);
        std::snprintf(finish, sizeof(finish), "FINISH TIME %u:%02u.%02u", finishCentiseconds / 6000u,
            (finishCentiseconds / 100u) % 60u, finishCentiseconds % 100u);
        text(x + 12.f, y + 40.f, place, 0xA8E6B4);
        text(x + 158.f, y + 40.f, finish, 0xA8E6B4);
        text(x + 12.f, y + 58.f, "STANDINGS AT YOUR FINISH", 0xA8C5DE);
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
            text(x + 300.f, rowY, row.finished ? "FINISHED" : "UNFINISHED", 0xA8C5DE);
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
            const float bestY = y + timingY + 13.f + 11.f * float(input.completedLapTimeCount);
            text(x + 12.f, bestY, "BEST", 0xA8E6B4);
            duration(bestY, input.bestLapSeconds);
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
    void standingsPanel(const VehicleWindowHostHud50DS& input, I width, I height) noexcept {
        if (!input.standingsCount) return;
        // Leave a four-cell gap below the race readout, including optional
        // clocks. Only this panel contracts when the viewport cannot fit it
        // above the bottom heat/damage panel or beside the right route cue.
        const float hudScale = scale;
        const float panelWidth = 222.f, standingsHeight = 22.f + 11.f * float(input.standingsCount);
        const float timingHeight = input.completedLapTimesValid ?
            30.f + 11.f * float(input.completedLapTimeCount) : 0.f;
        const float panelHeight = standingsHeight + timingHeight;
        const float clockHeight = input.raceClocksValid ? (input.raceFinished ? 14.f : 28.f) : 0.f;
        const float top = (44.f + clockHeight) * hudScale;
        const float availableHeight = float(height) - top - 68.f * hudScale;
        const float availableWidth = float(width) - 68.f * hudScale;
        if (availableHeight <= 0.f || availableWidth <= 0.f) return;
        scale = std::min(hudScale, std::min(availableWidth / panelWidth, availableHeight / panelHeight));
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
    bool build(const VehicleWindowHostHud50DS& input, I width, I height) noexcept {
        if (input.resultsPanelVisible) return buildResults(input, width, height);
        count = 0; overflow = false;
        const float clockHeight = input.raceClocksValid ? (input.raceFinished ? 14.f : 28.f) : 0.f;
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
        if (input.raceEnabled) {
            const float y = 72.f - float(height) / scale;
            rectangle(0.f, y, 168.f, 32.f + clockHeight, 0x617589);
            rectangle(1.f, y + 1.f, 166.f, 30.f + clockHeight, 0x0A111B);
            char laps[32]{}, place[32]{}, countdown[32]{};
            std::snprintf(laps, sizeof(laps), "LAP %u/%u", input.raceLap, input.raceLapTarget);
            std::snprintf(place, sizeof(place), "POS %u/%u", input.racePlace, input.raceRacerCount);
            std::snprintf(countdown, sizeof(countdown), "START IN %u", input.raceCountdown);
            text(6.f, y + 5.f, laps, 0xEDF3F8);
            text(90.f, y + 5.f, place, 0xEDF3F8);
            const char* status = input.raceResultsReady ? "RESULTS" : input.raceFinished ? "FINISHED" : input.raceCountdown ? countdown :
                input.raceGo ? "GO" : input.wrongWay ? "WRONG WAY" : "RACING";
            text(6.f, y + 19.f, status, input.raceFinished || input.raceGo ? 0xA8E6B4 :
                input.wrongWay ? 0xFF6556 : 0xA8C5DE);
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
                if (!input.raceFinished) {
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
        standingsPanel(input, width, height);
        return !overflow;
    }
};

// Linux desktop GL scalar signatures; resolved only through the caller's host.
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
    void (*uniform4f)(I, float, float, float, float) = nullptr;
    void (*blendFunc)(U, U) = nullptr;
    void (*blendEquation)(U) = nullptr;
    // Resolved only for source-RGBA texture-feedback fallback after capability proof.
    void (*textureBarrier)() = nullptr;
    void (*clipControl)(U,U) = nullptr;
    void (*genTextures)(I,U*) = nullptr;
    void (*deleteTextures)(I,const U*) = nullptr;
    void (*texImage2D)(U,I,I,I,I,I,U,U,const void*) = nullptr;
    void (*texParameteri)(U,U,I) = nullptr;
    void (*getTexLevelParameteriv)(U,I,U,I*) = nullptr;
    void (*framebufferTexture2D)(U,U,U,U,I) = nullptr;
    void (*activeTexture)(U) = nullptr;
    void (*bindTexture)(U, U) = nullptr;
    void (*getTexParameteriv)(U, U, I*) = nullptr;
    // Only loaded if GL >= 3.3 or ARB_sampler_objects is advertised.
    void (*bindSampler)(U, U) = nullptr;
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
    // Dormant blend-only lookup; normal renderer creation is unchanged.
    void (*drawArrays)(U,I,I) = nullptr;
    const unsigned char* (*getString)(U) = nullptr;
    void (*blitFramebuffer)(I, I, I, I, I, I, I, I, U, U) = nullptr;
};
}

struct VehicleWindowGl50DS::Storage {
    struct MaterialUv { float value[4] = {1.f, 1.f, 0.f, 0.f}; };
    struct Material {
        U textureIndex = assets::unresolvedMaterialTexture50EA, name = 0;
        VehicleWindowMaterialBinding50DS policy{};
        float uv[4] = {1.f, 1.f, 0.f, 0.f};
    };
    struct TrackSpan {
        U indexStart = 0, indexCount = 0;
        Material material{};
        bool discardZeroAlpha = false;
        U sourcePass = 1, sourcePassCount = 1, projectionPass = 0;
        bool worldFogEligible = false;
        U animation = assets::unresolvedMaterialTexture50EA;
    };
    struct TrackChunk {
        U sourceSpan = 0, indexStart = 0, indexCount = 0;
    };
    VehicleWindowRequest50DS request{};
    VehicleGlFunctions50CV drawGl{};
    VehicleGlShaderFunctions50CZ shaderGl{};
    OwnerGl gl{};
    VehicleGlResources50CV resources{};
    U racerNormalBuffers[2][3]{};
    std::unique_ptr<VehicleGlShader50CZ> shader;
    std::unique_ptr<VehicleGlBackend50CV> adapter;
    const std::thread::id thread = std::this_thread::get_id();
    const game::VehicleDrawMesh50CM* meshes[3]{};
    const assets::Scene48D* scenes[3]{};
    std::unique_ptr<Material[]> materials;
    std::unique_ptr<MaterialUv[]> opponentMaterialUvs;
    U opponentMaterialStride = 0;
    U materialOffsets[3]{}, materialCount = 0;
    struct RacerArchetype {
        const game::VehicleDrawMesh50CM* meshes[3]{};
        const assets::Scene48D* scenes[3]{};
        VehicleGlResources50CV resources{};
        U normalBuffers[2][3]{};
        std::unique_ptr<Material[]> materials;
        U materialOffsets[3]{}, materialCount = 0;
        const MaterialTextureGl50EB* textures = nullptr;
        std::uint64_t texturePublication = 0;
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
        const U (*normalBuffers)[3];
        Material* materials;
        const U* materialOffsets;
        U materialCount;
        U cableTextureName;
    };
    RacerView racerView(U archetype) const noexcept {
        if (!archetype) return {meshes, scenes, &resources, racerNormalBuffers, materials.get(),
            materialOffsets, materialCount, cableTextureName};
        const auto& source = racerArchetypes[archetype - 1];
        return {source.meshes, source.scenes, &source.resources, source.normalBuffers, source.materials.get(),
            source.materialOffsets, source.materialCount, source.cableTextureName};
    }
    std::uint64_t texturePublication = 0;
    std::unique_ptr<TrackSpan[]> trackSpans;
    U trackSpanCount = 0;
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
    std::unique_ptr<TrackChunk[]> trackChunks;
    U trackChunkCount = 0;
    std::unique_ptr<game::StaticDrawBounds50EI[]> trackDrawBounds;
    std::unique_ptr<U[]> trackVisibleDrawIds;
    U trackDrawCount = 0, frameTrackDrawCount = 0;
    bool frameTrackCullingApplied = false;
    std::uint64_t trackTexturePublication = 0;
    std::uint64_t cableTexturePublication = 0;
    U cableTextureName = 0, cableVertexBuffer = 0, cableIndexBuffer = 0, cableVertexArray = 0;
    const game::PodCableMesh50FD* frameCables = nullptr;
    game::VehicleDrawBatch50CO framePlayerBatch{}, frameOpponentBatches[maxVehicleWindowOpponents50DS]{};
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
    U menuTargets[2]{}, menuColors[2]{}, displayedTarget = 0;
    std::uint8_t menuPublished = 0;
    U uiTargets[2]{}, uiColors[2]{}, uiDepths[2]{};
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
    const OriginalUiRiderDrawContribution50KG* uiSourceFrameOwner=nullptr;
    std::uint64_t uiSourceFrameGeneration=0,uiSourceTargetGeneration=0;
    U uiProgram = 0, uiVertexShader = 0, uiFragmentShader = 0;
    U uiVertexBuffer = 0, uiIndexBuffer = 0, uiVertexArray = 0, uiParticleFogBuffer = 0, uiSourceRasterWBuffer = 0;
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
    I uiCanvasUniform = -1, uiImageUniform = -1, uiAlphaUniform = -1, uiTexturedUniform = -1;
    I uiParticleFogColorUniform = -1, uiSourceRasterUniform = -1;
    I uiSourceFogEnabledUniform = -1, uiSourceAlphaTestUniform = -1;
    I uiSourceCompositionUniform=-1,uiSourceTargetUniform=-1,uiSourceBlendUniform=-1;
    // Cold immutable atlas maxima; native blend admission is a joint texture
    // and vertex bound. Real Main atlases legitimately contain raw alpha255.
    std::uint8_t uiTextureMaximumAlpha[assets::maxMaterialTextureAssets50EA]{};
    std::uint8_t uiPublished = 0;
    std::uint64_t uiTexturePublication = 0;
    I raceWidth = 0, raceHeight = 0;
    I maximumWidth = 0, maximumHeight = 0, width = 0, height = 0, clipDistances = 0;
    I committedWidth = 0, committedHeight = 0;
    float matrix[16]{}, clearRgba[4]{};
    VehicleWindowDiagnostic50DS* diagnostic = nullptr; // Only during synchronous calls.
    std::uint8_t published = 0;
    bool initialized = false, active = false, frameSet = false, imageCommitted = false;

    bool current(VehicleWindowDiagnostic50DS& d) const noexcept {
        if (thread != std::this_thread::get_id())
            return fail(d, VehicleWindowError50DS::wrongThread, "renderer requires creating thread");
        return (request.isCurrent && request.isCurrent(request.context)) ||
            fail(d, VehicleWindowError50DS::notCurrent, "caller context/host surface is not current");
    }
    bool texturesCurrent(VehicleWindowDiagnostic50DS& d) const noexcept {
        if (request.mode == VehicleWindowMode50DS::callerMaterialTextures &&
            (!request.materials.textures || !request.materials.textures->validBorrow(
                request.context, request.isCurrent, texturePublication)))
            return fail(d, VehicleWindowError50DS::material,
                "borrowed vehicle texture publication/context/thread is no longer valid");
        for (U i = 0; i < racerArchetypeCount; ++i) {
            const auto& archetype = racerArchetypes[i];
            if (request.mode == VehicleWindowMode50DS::callerMaterialTextures &&
                (!archetype.textures || !archetype.textures->validBorrow(
                    request.context, request.isCurrent, archetype.texturePublication)))
                return fail(d, VehicleWindowError50DS::material,
                    "borrowed racer texture publication/context/thread is no longer valid");
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
            }
            if (archetype.cables.textures) {
                const auto* texture = archetype.cables.textures->asset(archetype.cables.textureIndex);
                if (!texture || texture->name != archetype.cableTextureName)
                    return fail(d, VehicleWindowError50DS::material,
                        "racer cable texture is no longer the retained publication");
            }
        }
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
        for (U i = 0; i < racerArchetypeCount; ++i)
            if (racerArchetypes[i].cables.textures) return true;
        return request.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures || request.cables.textures ||
            request.sky.textures || request.dynamicWorld.textures || request.enableOriginalUi;
    }
    bool usesTrackShader() const noexcept {
        return request.track.mode == VehicleWindowMode50DS::sourceVertexColors ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures ||
            request.dynamicWorld.mode == VehicleWindowMode50DS::callerMaterialTextures;
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
            target.sourcePass = source.sourcePass;
            target.sourcePassCount = source.sourcePassCount;
            target.projectionPass = source.projectionPass;
            // All source ranges/pass continuity were validated before creation.
            // For alpha-over pairs, fogging both colors with the same factor
            // yields fog(alpha*layer + (1-alpha)*base), without fogging alpha.
            target.worldFogEligible = source.sourcePassCount == 1 &&
                source.material.blend == VehicleWindowBlend50DS::opaque && source.material.depthWrite;
            if (source.sourcePassCount == 2) {
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
        for (U c = 0; c < 3; ++c) {
            materialOffsets[c] = total;
            total += input.component[c].count; // Already bounded to 3 * 4096.
        }
        materialCount = total;
        materials.reset(new (std::nothrow) Material[total]);
        if (!materials)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate bounded cold material table");
        for (U c = 0; c < 3; ++c) {
            for (U m = 0; m < input.component[c].count; ++m) {
                const auto& binding = input.component[c].bindings[m];
                auto& resolved = materials[materialOffsets[c] + m];
                resolved.textureIndex = binding.textureIndex;
                resolved.policy = binding;
                resolved.name = binding.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                    input.textures->texture(binding.textureIndex) : 0;
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
            if (c >= 3 || uv.materialIndex >= selected.scenes[c]->materials.size())
                return fail(d, VehicleWindowError50DS::frame, "material UV identity outside original scene");
            const U key = selected.materialOffsets[c] + uv.materialIndex;
            if (i && key <= previousKey)
                return fail(d, VehicleWindowError50DS::frame, "material UV array must be unique and source ordered");
            previousKey = key;
            const float values[] = {uv.uScale, uv.vScale, uv.uOffset, uv.vOffset};
            for (float value : values)
                if (!std::isfinite(value))
                    return fail(d, VehicleWindowError50DS::frame, "nonfinite material UV transform");
        }
        return true;
    }
    bool retainedMesh(const game::VehicleDrawBatch50CO& batch, VehicleWindowDiagnostic50DS& d,
        U archetype = 0) const noexcept {
        const auto selected = racerView(archetype);
        for (U c = 0; c < 3; ++c) {
            const auto& mesh = *selected.meshes[c];
            const auto& part = batch.component[c];
            if (part.indices != mesh.indices || part.indexCount != mesh.indexCount ||
                part.spans != mesh.spans || part.spanCount != mesh.spanCount || part.vertexCount != mesh.vertexCount)
                return fail(d, VehicleWindowError50DS::assets, "batch is not the retained prepared mesh");
        }
        return validateRacerNormals(batch, d);
    }
    bool validateRacerNormals(const game::VehicleDrawBatch50CO& batch,
        VehicleWindowDiagnostic50DS& d) const noexcept {
        for (U c = 0; c < 3; ++c) {
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
        VehicleWindowDiagnostic50DS& d) const noexcept {
        if (frame.opponentCount > maxVehicleWindowOpponents50DS ||
            bool(frame.opponentCount) != bool(frame.opponents))
            return fail(d, VehicleWindowError50DS::frame, "opponents require an explicit bounded array of at most seven");
        if (!frame.opponentCount) return true;
        const auto capacity = adapter->backend().capacity;
        game::VehicleDrawPlan50CS plan{};
        if (!game::preflightVehicleDrawBatch50CS(player, capacity, plan, d.submission))
            return fail(d, VehicleWindowError50DS::submission, "player batch preflight rejected before opponent rendering");
        if (!retainedMesh(player, d)) return false;
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
            for (U c = 0; c < 3; ++c) {
                racerCapacity.vertices[c] = selected.meshes[c]->vertexCount;
                racerCapacity.indices[c] = selected.meshes[c]->indexCount;
            }
            if (!game::preflightVehicleDrawBatch50CS(*opponent.batch, racerCapacity, plan, d.submission))
                return fail(d, VehicleWindowError50DS::submission, "opponent batch preflight rejected");
            if (!retainedMesh(*opponent.batch, d, opponent.racerArchetype) ||
                !validateMaterialUvs(opponent.materialUvs, opponent.materialUvCount, d,
                    opponent.racerArchetype)) return false;
            if (opponent.cables && (!selected.cableTextureName || !validateCableFrame(*opponent.cables, d)))
                return fail(d, VehicleWindowError50DS::frame, "opponent cables require valid geometry and the cold cable bank");
            // Each independent live 50CO stream must survive until deferred
            // effects. Reject overlapping player/opponent/component storage.
            for (U c = 0; c < 3; ++c) {
                const auto& part = opponent.batch->component[c];
                const auto start = reinterpret_cast<std::uintptr_t>(part.vertices);
                const auto bytes = std::uintptr_t(part.vertexCount) * sizeof(assets::Vertex48B);
                if (start > std::numeric_limits<std::uintptr_t>::max() - bytes)
                    return fail(d, VehicleWindowError50DS::frame, "opponent vertex range overflow");
                for (U prior = 0; prior <= i + 1; ++prior) {
                    const auto& other = prior ? *frame.opponents[prior - 1].batch : player;
                    for (U k = 0; k < (prior == i + 1 ? c : 3); ++k) {
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
    bool hasGlResources() const noexcept {
        if (shader && shader->hasResources()) return true;
        if (trackProgram || trackVertexShader || trackFragmentShader) return true;
        if (hostHudProgram || hostHudVertexShader || hostHudFragmentShader ||
            hostHudVertexBuffer || hostHudIndexBuffer || hostHudVertexArray) return true;
        if (uiProgram || uiVertexShader || uiFragmentShader ||
            uiVertexBuffer || uiIndexBuffer || uiVertexArray || uiParticleFogBuffer || uiSourceRasterWBuffer ||
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
            if (targets[b] || colors[b] || depths[b] || menuTargets[b] || menuColors[b]) return true;
            for (U c = 0; c < 3; ++c)
                if (resources.bank[b].vertexBuffer[c] || resources.bank[b].vertexArray[c] ||
                    racerNormalBuffers[b][c]) return true;
        }
        for (const auto& index : resources.indices) if (index.buffer) return true;
        for (const auto& archetype : racerArchetypes) {
            for (const auto& bank : archetype.normalBuffers)
                for (U normal : bank) if (normal) return true;
            for (const auto& bank : archetype.resources.bank)
                for (U c = 0; c < 3; ++c)
                    if (bank.vertexBuffer[c] || bank.vertexArray[c]) return true;
            for (const auto& index : archetype.resources.indices) if (index.buffer) return true;
        }
        return false;
    }
    bool check(const char* operation) noexcept {
        const U error = drawGl.getError();
        if (!error) return true;
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
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer || request.enableHostHud || request.enableOriginalUi) {
            LOAD(gl, blitFramebuffer, BlitFramebuffer);
        }
        if (request.enableOriginalUi) {
            LOAD(gl, scissor, Scissor); LOAD(gl, getTexParameteriv, GetTexParameteriv);
        }
        if (usesTextures()) {
            LOAD(gl, activeTexture, ActiveTexture); LOAD(gl, bindTexture, BindTexture);
        }
        if (request.enableOriginalUiSourceRgba) {
            LOAD(gl, genTextures, GenTextures); LOAD(gl, deleteTextures, DeleteTextures);
            LOAD(gl, texImage2D, TexImage2D); LOAD(gl, texParameteri, TexParameteri);
            LOAD(gl, getTexLevelParameteriv, GetTexLevelParameteriv);
            LOAD(gl, framebufferTexture2D, FramebufferTexture2D);
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
uniform int worldFogEnabled;
uniform vec4 worldFogCamera;
uniform vec4 worldFogRange;
out float trackFogAmount;
uniform mat4 worldViewProjection;
out vec3 trackWorldNormal;
out vec3 trackWorldPosition;
out vec4 trackSourceColor;
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
    trackSourceUv = projectionPass == 1 ? rrProjectedUv.xy :
        (projectionPass == 2 ? rrProjectedUv.zw : rrUv);
}
)GLSL";
        constexpr const char* fragmentSource = R"GLSL(#version 150 core
in vec3 trackWorldPosition;
in float trackFogAmount;
uniform int worldFogEnabled;
uniform vec4 worldFogColor;
in vec3 trackWorldNormal;
in vec4 trackSourceColor;
in vec2 trackSourceUv;
uniform float vertexColorScale;
uniform int useTexture;
uniform int discardZeroAlpha;
uniform int effectMode;
uniform float alphaScale;
uniform int unlit;
uniform int useSourceNormals;
uniform sampler2D image;
out vec4 viewerOutput;
)GLSL" RR_NATIVE_POD_SHADOW_GLSL50HM R"GLSL(
void main() {
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
        if (discardZeroAlpha != 0 && texel.a <= 0.0) discard;
        color *= texel.rgb;
        alpha *= texel.a;
    }
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
        if (worldFogEnabledUniform < 0 || worldFogCameraUniform < 0 ||
            worldFogColorUniform < 0 || worldFogRangeUniform < 0 ||
            trackMatrixUniform < 0 || trackColorScaleUniform < 0 ||
            trackUseTextureUniform < 0 || trackImageUniform < 0 || trackDiscardZeroAlphaUniform < 0 ||
            trackEffectModeUniform < 0 || trackAlphaScaleUniform < 0 || trackUnlitUniform < 0 ||
            trackUseSourceNormalsUniform < 0 || trackProjectionPassUniform < 0 || nativeShadowEnabledUniform < 0 || nativeShadowOpponentCountUniform < 0)
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
        for (U c = 0; c < 3; ++c) {
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
        diagnostic->component = 3;
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
    bool createRacerNormals(const VehicleGlResources50CV& geometry,
        U (&buffers)[2][3]) noexcept {
        // Keep the compatibility mesh and all UV/color bytes unchanged. Every
        // private VAO gets a bounded independent normal stream, initially zero.
        static constexpr assets::WorldNormal50HK absent[game::maxRigidVertices49S]{};
        for (U bank = 0; bank < 2; ++bank) for (U c = 0; c < 3; ++c) {
            VehicleWindowTrack50DS source{};
            source.sourceNormals = absent;
            source.sourceNormalCount = geometry.bank[bank].vertexCapacity[c];
            if (!createWorldNormals(source, buffers[bank][c],
                geometry.bank[bank].vertexArray[c], dynamicDraw)) return false;
        }
        return true;
    }
    bool uploadRacerNormals(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0) noexcept {
        const auto selected = racerView(archetype);
        static constexpr assets::WorldNormal50HK absent[game::maxRigidVertices49S]{};
        for (U c = 0; c < 3; ++c) {
            const auto& part = batch.component[c];
            drawGl.bindBuffer(arrayBuffer, selected.normalBuffers[bank][c]);
            drawGl.bufferSubData(arrayBuffer, 0,
                std::intptr_t(part.vertexCount) * sizeof(assets::WorldNormal50HK),
                part.sourceNormals ? part.sourceNormals : absent);
            if (!check("upload current per-node racer source normals")) return false;
        }
        return true;
    }
    bool createTrackProjection(const VehicleWindowTrack50DS& track) noexcept {
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
        U indices[game::podCableIndexCount50FD];
        for (U i = 0; i < game::podCableIndexCount50FD; ++i) indices[i] = cableIndex50DS(i);
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
        U uploaded[game::podCableIndexCount50FD];
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
    bool validateOriginalUiBank(const VehicleWindowRequest50DS& input) noexcept {
        if (!input.enableOriginalUi)
            return (!input.originalUiCpuTextures && !input.originalUiTextures) ||
                fail(*diagnostic, VehicleWindowError50DS::request, "UI banks require explicit original UI opt-in");
        const auto* cpu = input.originalUiCpuTextures;
        const auto* gpu = input.originalUiTextures;
        // No bank is needed by an explicitly untextured-only UI. A missing
        // textured resource never selects this path: each span is typed.
        if (!cpu && !gpu) return true;
        if (!cpu || !cpu->loaded() || !gpu || !gpu->validBorrow(input.context, input.isCurrent,
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
        std::memcpy(uiTextureMaximumAlpha, maximumAlphaByTexture, sizeof(uiTextureMaximumAlpha));
        return true;
    }
    bool createOriginalUi() noexcept {
        if (!request.enableOriginalUi) return true;
        constexpr const char* vertexSource = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec2 rrUv;
in vec4 rrColor;
in float rrParticleFog;
in float rrSourceRasterW;
uniform vec4 canvasToClip;
uniform int uiSourceRaster;
out vec2 uiUv;
noperspective out vec4 uiColor;
noperspective out float uiParticleFog;
void main() {
    float w = uiSourceRaster != 0 ? rrSourceRasterW : 1.0;
    gl_Position = vec4(rrPosition.xy * canvasToClip.xy + canvasToClip.zw,
        (rrPosition.z / 16777215.0) * 2.0 - 1.0, 1.0) * w;
    uiUv = rrUv; uiColor = rrColor; uiParticleFog = rrParticleFog;
}
)GLSL";
        constexpr const char* fragmentPrefix = "#version 150 core\n";
        constexpr const char* coherentFragmentPrefix =
            "#version 150 core\n"
            "#extension GL_EXT_shader_framebuffer_fetch : require\n"
            "#define RR_UI_COHERENT_FRAMEBUFFER_FETCH 1\n";
        constexpr const char* fragmentSource = R"GLSL(
in vec2 uiUv;
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
uniform int uiSourceBlend;
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
        vec4 source=uiTextured!=0 ?
            floor(clamp(texture(uiImage,uiUv)*255.0*vertexByte/128.0,0.0,255.0)) :
            floor(vertexByte);
        if (uiSourceAlphaTest!=0 && source.a<1.0) discard;
        if (uiSourceFogEnabled!=0) {
            float f=clamp(uiParticleFog*256.0,0.0,255.0);
            source.rgb=floor((source.rgb*f+uiParticleFogColor.rgb*255.0*(255.0-f))/256.0);
        }
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
            float coefficient=uiSourceBlend==3 ? destination.a : uiSourceBlend==4 ? 255.0 : source.a;
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
        uiSourceFogEnabledUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceFogEnabled");
        uiSourceAlphaTestUniform = shaderGl.getUniformLocation(uiProgram, "uiSourceAlphaTest");
        uiSourceCompositionUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceComposition");
        if (!uiSourceCoherentFramebufferFetch)
            uiSourceTargetUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceTarget");
        uiSourceBlendUniform=shaderGl.getUniformLocation(uiProgram,"uiSourceBlend");
        if (!check("locate original UI uniforms")) return false;
        if (uiCanvasUniform < 0 || uiImageUniform < 0 || uiAlphaUniform < 0 || uiTexturedUniform < 0 ||
            uiParticleFogColorUniform < 0 || uiSourceRasterUniform < 0 ||
            uiSourceFogEnabledUniform < 0 || uiSourceAlphaTestUniform < 0 ||
            uiSourceCompositionUniform < 0 || (!uiSourceCoherentFramebufferFetch && uiSourceTargetUniform < 0) ||
            uiSourceBlendUniform < 0)
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
        return !wPointer || fail(*diagnostic, VehicleWindowError50DS::layout, "source W offset mismatch");
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
        drawGl.bufferSubData(arrayBuffer, 0, sizeof(cables->vertices), cables->vertices);
        drawGl.bindBuffer(elementBuffer, cableIndexBuffer);
        gl.useProgram(shader->program());
        gl.uniformMatrix4fv(shader->uniforms().worldViewProjection, 1, 0, matrix);
        gl.activeTexture(textureUnit0);
        gl.bindTexture(texture2D, textureName);
        if (gl.bindSampler) gl.bindSampler(0, 0);
        gl.uniform1i(shader->uniforms().useTexture, 1);
        gl.uniform1i(shader->uniforms().image, 0);
        gl.uniform1i(shader->uniforms().useLighting, 0); // Energy cables stay emissive.
        if (!check("upload actual current source cable vertices and bind real texture")) return false;
        drawGl.drawElementsBaseVertex(0x0004, game::podCableIndexCount50FD, 0x1405, nullptr, 0);
        if (!check("draw source cable strips after pod")) return false;
        ++diagnostic->cableDraws;
        diagnostic->cableVertices += game::podCableVertexCount50FD;
        diagnostic->cableIndices += game::podCableIndexCount50FD;
        return true;
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
            gl.genFramebuffers(1, &targets[b]);
            gl.bindFramebuffer(framebuffer, targets[b]);
            if (!check("create unpublished framebuffer")) return false;
            if (!targets[b]) return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero framebuffer name");
            if (!createAttachment(colors[b], rgba8, colorAttachment) ||
                !createAttachment(depths[b], depth24, depthAttachment)) return false;
            gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
            const U status = gl.checkFramebufferStatus(framebuffer);
            if (!check("check unpublished framebuffer")) return false;
            if (status != framebufferComplete) {
                diagnostic->framebufferStatus = status;
                return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "incomplete unpublished framebuffer");
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
    bool checkContext() noexcept {
        const auto* version = gl.getString(0x1F02); // GL_VERSION
        I major = 0, minor = 0;
        gl.getIntegerv(0x821B, &major); gl.getIntegerv(0x821C, &minor);
        if (!check("query desktop GL version")) return false;
        if (!version || std::strncmp(reinterpret_cast<const char*>(version), "OpenGL ES", 9) == 0 ||
            major < 3 || (major == 3 && minor < 2))
            return fail(*diagnostic, VehicleWindowError50DS::context, "desktop OpenGL 3.2 or newer required");
        if (!checkTextureSampling(major, minor) || !checkOriginalUiSourceSupport(major,minor)) return false;
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
            target.textures = source.materials.textures;
            target.texturePublication = target.textures ? target.textures->publicationToken() : 0;
            target.cables = source.cables;
            target.cableTexturePublication = target.cables.textures ?
                target.cables.textures->publicationToken() : 0;
            target.cableTextureName = target.cables.textures ?
                target.cables.textures->texture(target.cables.textureIndex) : 0;
            if (target.cables.textures && !target.cableTextureName)
                return fail(*diagnostic, VehicleWindowError50DS::material,
                    "required racer cable texture disappeared during create");
            for (U c = 0; c < 3; ++c) {
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
            for (U c = 0; c < 3; ++c)
                for (U m = 0; m < source.materials.component[c].count; ++m) {
                    const auto& binding = source.materials.component[c].bindings[m];
                    auto& material = target.materials[target.materialOffsets[c] + m];
                    material.policy = binding;
                    material.textureIndex = binding.textureIndex;
                    material.name = binding.mode == VehicleWindowMaterialMode50DS::requiredTexture ?
                        target.textures->texture(binding.textureIndex) : 0;
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
        diagnostic->component = 3;
        return texturesCurrent(*diagnostic);
    }
    bool initialize(const VehicleWindowRequest50DS& input,
        const assets::VehicleDrawAssets50CR& owner) noexcept {
        request = input;
        if (input.enableOriginalUiSourceRgba && !input.enableOriginalUi)
            return fail(*diagnostic,VehicleWindowError50DS::request,"source UI requires original UI opt-in");
        if (!validateOriginalUiBank(input)) return false;
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
        trackTexturePublication = input.track.textures ? input.track.textures->publicationToken() : 0;
        dynamicWorldTexturePublication = input.dynamicWorld.textures ? input.dynamicWorld.textures->publicationToken() : 0;
        trackAnimationTexturePublication = input.trackAnimationTextures ? input.trackAnimationTextures->publicationToken() : 0;
        cableTexturePublication = input.cables.textures ? input.cables.textures->publicationToken() : 0;
        skyTexturePublication = input.sky.textures ? input.sky.textures->publicationToken() : 0;
        cableTextureName = input.cables.textures ? input.cables.textures->texture(input.cables.textureIndex) : 0;
        maximumWidth = request.maximumWidth; maximumHeight = request.maximumHeight;
        if (!current(*diagnostic) || !texturesCurrent(*diagnostic)) return false;
        if (input.cables.textures && !cableTextureName)
            return fail(*diagnostic, VehicleWindowError50DS::material, "required cable texture disappeared during create");
        for (U c = 0; c < 3; ++c) {
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
        diagnostic->component = 3;
        if (!createMaterials(input.materials) || !createRacerArchetypes(input) ||
            !createTrackMaterials(input.track) || !createTrackAnimations(input) ||
            !createDynamicWorldMaterials(input.dynamicWorld) ||
            !createTrackVisibility(input.track) || !createSkyMaterials(input.sky)) return false;
        if (!loadFunctions() || !check("initial GL error state") || !checkContext()) return false;
        shader.reset(new (std::nothrow) VehicleGlShader50CZ(shaderGl));
        if (!shader) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate shader owner");
        if (!shader->initialize(diagnostic->shader))
            return fail(*diagnostic, VehicleWindowError50DS::shader, "initialize 50CZ viewer shader");
        if (!createTrackShader() || !createGeometry(resources, meshes) ||
            !createRacerNormals(resources, racerNormalBuffers)) return false;
        for (U i = 0; i < racerArchetypeCount; ++i)
            if (!createGeometry(racerArchetypes[i].resources, racerArchetypes[i].meshes) ||
                !createRacerNormals(racerArchetypes[i].resources, racerArchetypes[i].normalBuffers)) return false;
        if (
            !createStaticGeometry(input.track.mesh, trackVertexBuffer, trackIndexBuffer, trackVertexArray, trackIndexCount) ||
            !createWorldNormals(input.track, trackNormalBuffer, trackVertexArray) ||
            !createDynamicWorldGeometry(input.dynamicWorld) ||
            !createTrackProjection(input.track) ||
            !createStaticGeometry(input.sky.mesh, skyVertexBuffer, skyIndexBuffer, skyVertexArray, skyIndexCount) ||
            !createCableGeometry() || !createHostHud() || !createOriginalUi() ||
            !createTargets()) return false;
        VehicleGlHooks50CV hooks{};
        hooks.context = this; hooks.ready = ready; hooks.materialReady = materialReady;
        hooks.begin = begin; hooks.bindMaterial = bindMaterial; hooks.commit = commit; hooks.abort = abort;
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
        if (!s.validateRacerNormals(batch, *s.diagnostic)) return false;
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
        for (U c = 0; c < 3; ++c) {
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
        if (c >= 3 || material >= s.scenes[c]->materials.size())
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
    void configureBlend(const VehicleWindowMaterialBinding50DS& policy) noexcept {
        gl.enable(0x0B71);
        gl.depthMask(policy.depthWrite ? 1 : 0);
        gl.depthFunc(policy.depthEqual ? 0x0203 : 0x0201);
        if (policy.blend == VehicleWindowBlend50DS::opaque) gl.disable(0x0BE2);
        else {
            gl.enable(0x0BE2);
            gl.blendEquation(0x8006); // FUNC_ADD; never inherit caller equations.
            gl.blendFunc(policy.fixedBlendAlpha ? 1 : 0x0302,
                policy.blend == VehicleWindowBlend50DS::additive ? 1 : 0x0303);
        }
    }
    void resetPodState() noexcept {
        // Explicitly clear the shared world shader before any pod/sky/UI path.
        if (trackProgram) {
            gl.useProgram(trackProgram);
            gl.uniform1i(worldFogEnabledUniform, 0);
        }
        gl.disable(0x0BE2); gl.enable(0x0B71); gl.depthMask(1); gl.depthFunc(0x0201);
        gl.useProgram(shader->program());
        const auto& u = shader->uniforms();
        gl.uniformMatrix4fv(u.worldViewProjection, 1, 0, matrix);
        gl.uniform1i(u.useTexture, 0); gl.uniform1i(u.image, 0);
        gl.uniform1i(u.effectMode, 0); gl.uniform1f(u.alphaScale, 1.f);
        gl.uniform1i(u.suppressDraw, 0); gl.uniform1i(u.useUvTransform, 0);
        gl.uniform1i(u.useLighting, 0);
    }
    bool drawSky() noexcept {
        if (!skyIndexCount) return true;
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
                        const U textureName = span.animation == assets::unresolvedMaterialTexture50EA ?
                            span.material.name : s.trackAnimations[span.animation].names[
                                s.trackAnimations[span.animation].selectedFrame];
                        s.gl.bindTexture(texture2D, textureName);
                        s.gl.uniform1i(s.trackUseTextureUniform, span.material.name ? 1 : 0);
                        s.gl.uniform1i(s.trackDiscardZeroAlphaUniform, span.discardZeroAlpha ? 1 : 0);
                        s.gl.uniform1i(s.worldFogEnabledUniform,
                            s.request.track.worldFog.enabled && span.worldFogEligible ? 1 : 0);
                        s.configureBlend(span.material.policy);
                        // Coplanar authored passes must survive the first pass depth.
                        // Restore the historical LESS rule for every single-pass draw.
                        s.gl.depthFunc(span.sourcePass > 1 || span.material.policy.depthEqual ? 0x0203 : 0x0201);
                        s.gl.uniform1i(s.trackProjectionPassUniform, static_cast<I>(span.projectionPass));
                        s.gl.uniform1i(s.trackEffectModeUniform, effectMode(span.material.policy));
                        s.gl.uniform1f(s.trackAlphaScaleUniform, span.material.policy.alphaScale);
                        s.gl.uniform1i(s.trackUnlitUniform,
                            span.material.policy.unlit || span.material.policy.sourcePrelit ? 1 : 0);
                        s.gl.uniform1i(s.nativeShadowEnabledUniform, s.request.enableNativePodShadows &&
                            !transparent && span.sourcePass == 1 && span.material.policy.depthWrite && !span.material.policy.unlit ? 1 : 0);
                        if (!s.check("bind explicit source-ordered track material")) return false;
                        boundSpan = i;
                    }
                    const U indexStart = s.trackChunkCount ? s.trackChunks[draw].indexStart : span.indexStart;
                    const U indexCount = s.trackChunkCount ? s.trackChunks[draw].indexCount : span.indexCount;
                    const auto offset = std::uintptr_t(indexStart) * sizeof(U);
                    s.drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(indexCount),
                        0x1405, reinterpret_cast<const void*>(offset), 0);
                    if (!s.check("draw actual track surface range")) return false;
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
        gl.useProgram(trackProgram);
        gl.uniform1i(worldFogEnabledUniform, 0);
        gl.uniformMatrix4fv(trackMatrixUniform, 1, 0, matrix);
        gl.uniform1f(trackColorScaleUniform, request.dynamicWorld.vertexColorScale);
        gl.uniform1i(trackImageUniform, 0);
        gl.uniform1i(trackUseSourceNormalsUniform, dynamicWorldNormalCount ? 1 : 0);
        gl.uniform1i(trackProjectionPassUniform, 0);
        // Static receiver/culling bounds are deliberately not reused for moving owners.
        gl.uniform1i(nativeShadowEnabledUniform, 0);
        gl.uniform1i(nativeShadowOpponentCountUniform, 0);
        drawGl.bindVertexArray(dynamicWorldArrays[bank]);
        drawGl.bindBuffer(elementBuffer, dynamicWorldIndexBuffer);
        for (U i = 0; i < dynamicWorldSpanCount; ++i) {
            const auto& span = dynamicWorldSpans[i];
            const auto& material = span.material;
            const bool deferred = span.sourcePassCount == 1 &&
                material.policy.blend != VehicleWindowBlend50DS::opaque;
            if (deferred != transparent) continue;
            diagnostic->dynamicWorldSpan = i;
            diagnostic->texture = material.textureIndex;
            configureBlend(material.policy);
            gl.depthFunc(span.sourcePass > 1 || material.policy.depthEqual ? 0x0203 : 0x0201);
            gl.bindTexture(texture2D, material.name);
            gl.uniform1i(trackUseTextureUniform, material.name ? 1 : 0);
            gl.uniform1i(trackDiscardZeroAlphaUniform, span.discardZeroAlpha ? 1 : 0);
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
    bool applyPodMaterial(const Material& material, bool suppress, const float* overrideUv = nullptr) noexcept {
        configureBlend(material.policy);
        const auto& u = shader->uniforms();
        gl.bindTexture(texture2D, material.name);
        gl.uniform1i(u.useTexture, material.name ? 1 : 0);
        gl.uniform1i(u.effectMode, effectMode(material.policy));
        gl.uniform1f(u.alphaScale, material.policy.alphaScale);
        gl.uniform1i(u.suppressDraw, suppress ? 1 : 0);
        // Retain source-alpha/additive effect appearance and explicit emissive
        // materials. Only opaque pod surfaces opt into host illumination.
        gl.uniform1i(u.useLighting, !material.policy.unlit && !material.policy.sourcePrelit &&
            material.policy.blend == VehicleWindowBlend50DS::opaque ? 1 : 0);
        gl.uniform1i(u.useUvTransform, 1);
        const auto* uv = overrideUv ? overrideUv : material.uv;
        gl.uniform4f(u.uvScaleOffset, uv[0], uv[1], uv[2], uv[3]);
        return check("bind explicit vehicle material blend and UV policy");
    }
    bool uploadRacer(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0) noexcept {
        const auto selected = racerView(archetype);
        if (!uploadRacerNormals(bank, batch, archetype)) return false;
        // Every batch was structurally preflighted and matched to these exact
        // cold buffers before begin. Only the inactive bank is ever reused.
        for (U c = 0; c < 3; ++c) {
            drawGl.bindBuffer(arrayBuffer, selected.resources->bank[bank].vertexBuffer[c]);
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
        if (!uploadRacer(bank, frameOpponentBatches[opponent], archetype)) return false;
        for (U c = 0; c < 3; ++c) {
            drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[c]);
            drawGl.bindBuffer(elementBuffer, selected.resources->indices[c].buffer);
            for (U i = 0; i < selected.meshes[c]->spanCount; ++i) {
                const auto& span = selected.meshes[c]->spans[i];
                if (selected.materials) {
                    const U material = selected.materialOffsets[c] + span.material_index;
                    if (selected.materials[material].policy.blend != VehicleWindowBlend50DS::opaque) continue;
                    if (!applyPodMaterial(selected.materials[material], false,
                        opponentMaterialUvs[opponent * opponentMaterialStride + material].value)) return false;
                }
                drawGl.drawElementsBaseVertex(0x0004, static_cast<I>(span.index_count), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(span.index_start) * sizeof(U)), 0);
                if (!check("draw actual opaque opponent source span")) return false;
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
        for (U c = 0; c < 3; ++c) {
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
        s.active = true;
        // Same-context GL ordering protects buffer/target reuse after prior
        // commands, including aborted work. No mapping or cross-context sharing.
        s.gl.bindFramebuffer(framebuffer, s.targets[bank]);
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
        if (!s.uploadDynamicWorld(bank) || !s.drawSky() || !s.drawTrack(false) ||
            !s.drawDynamicWorld(bank, false)) return false;
        return s.uploadRacerNormals(bank, s.framePlayerBatch);
    }
    static bool bindMaterial(void* context, game::VehicleDrawComponent50CM component, U material) noexcept {
        if (!materialReady(context, component, material)) return false;
        auto& s = *static_cast<Storage*>(context);
        if (s.request.mode == VehicleWindowMode50DS::callerMaterialTextures) {
            const auto& binding = s.materials[s.materialOffsets[componentIndex(component)] + material];
            // 50CV keeps its exact original submission/provenance contract. A
            // selected effect issues a discard-only first-pass draw, then the
            // retained source range is rendered once at commit after opaques.
            return s.applyPodMaterial(binding, binding.policy.blend != VehicleWindowBlend50DS::opaque);
        }
        s.gl.uniform1i(s.shader->uniforms().useTexture, 0);
        return s.check("bind explicit neutral vehicle material policy");
    }
    static bool commit(void* context, std::uint8_t bank) noexcept {
        auto& s = *static_cast<Storage*>(context);
        if (!s.active || bank != (s.published ^ 1u)) return false;
        // One transaction and one clear/sky/track pass. Other actual opaque
        // racers join the player's pending framebuffer before ANY effects.
        for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (!s.drawOpaqueOpponent(bank, i)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        // Cable/HUD failure still aborts the unpublished combined frame.
        s.resetPodState();
        if (!s.drawCables(s.frameCables, s.cableTextureName)) return false;
        for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (!s.drawCables(s.frameOpponentCables[i],
                s.racerView(s.frameOpponentArchetypes[i]).cableTextureName)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        if (!s.drawTrack(true) || !s.drawDynamicWorld(bank, true)) return false;
        // The shared inactive VBOs currently hold the last opaque opponent.
        // Restore the player's independent stream before its deferred effects.
        if (s.frameOpponentCount && s.materials && !s.uploadRacer(bank, s.framePlayerBatch)) return false;
        if (!s.drawTransparentPod(bank)) return false;
        if (s.materials) for (U i = 0; i < s.frameOpponentCount; ++i) {
            s.diagnostic->opponentIndex = i;
            if (!s.uploadRacer(bank, s.frameOpponentBatches[i], s.frameOpponentArchetypes[i]) ||
                !s.drawTransparentPod(bank, i)) return false;
        }
        s.diagnostic->opponentIndex = maxVehicleWindowOpponents50DS;
        s.resetPodState();
        if (!s.drawHostHud()) return false;
        if (!s.check("accept private vehicle frame")) return false;
        // 50CV publication is CPU-only. The fallible window copy is deliberately
        // OUTSIDE this transaction, after submitVehicleDrawBatch50CS succeeds.
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
        static_cast<Storage*>(context)->active = false;
    }
    bool copyToWindow() noexcept {
        gl.bindFramebuffer(readFramebuffer, displayedTarget);
        gl.readBuffer(colorAttachment);
        gl.bindFramebuffer(drawFramebuffer, 0);
        gl.drawBuffer(backBuffer);
        if (!check("prepare default back-buffer blit")) return false;
        // No atomicity claim for default framebuffer writes. On any later
        // error the host must not swap this potentially partial back buffer.
        diagnostic->defaultFramebufferMayBeModified = true;
        gl.blitFramebuffer(0, 0, width, height, 0, 0, width, height, colorBufferBit, 0x2600); // NEAREST
        if (!check("blit vehicle color to default back buffer")) return false;
        gl.bindFramebuffer(framebuffer, 0);
        return check("restore default framebuffer bindings");
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
            if (!d.cleanupGlError) d.cleanupGlError = error;
        }
        return false;
    }
    bool deleteName(U& name, void (*deleter)(I, const U*), VehicleWindowDiagnostic50DS& d) noexcept {
        if (!name) return true;
        deleter(1, &name);
        const U error = drawGl.getError();
        if (!error) { name = 0; return true; }
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
        if (!d.cleanupGlError) d.cleanupGlError = error;
        return false;
    }
    bool cleanup(VehicleWindowDiagnostic50DS& d) noexcept {
        if (thread != std::this_thread::get_id()) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::wrongThread, "cleanup requires creating thread");
        }
        if (hasGlResources() && !current(d)) { d.cleanupIncomplete = true; return false; }
        initialized = false; active = frameSet = imageCommitted = false;
        frameCables = nullptr; frameHostHud = false;
        committedWidth = committedHeight = raceWidth = raceHeight = 0;
        displayedTarget = 0;
        uiPrivatePublicationGeneration=0;
        adapter.reset();
        if (!hasGlResources()) { shader.reset(); d.cleanupIncomplete = false; return true; }
        // All functions were loaded before the first name could be allocated.
        if (!drainCleanupErrors(d)) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "GL error state will not clear for cleanup");
        }
        // Explicit slice1A leases add a real completion obligation. Never
        // confuse a CPU generation increment or deferred GL deletion with it.
        if (uiSourceLeaseWork || uiRetainedTarget || uiRetainedColor || uiBlendOldSnapshot ||
            uiPromotedIncomingTarget || uiPromotedIncomingColor){
            gl.finish();const U error=drawGl.getError();
            if (error){if(!d.cleanupGlError)d.cleanupGlError=error;d.cleanupIncomplete=true;
                return fail(d,VehicleWindowError50DS::cleanup,"complete source leases before cleanup");}
            uiSourceLeaseWork=false;
        }
        uiSourceReceiptLifetime.reset();uiSnapshotGeneration=0;
        uiBlendGeneration=uiBlendSampleGeneration=0;
        uiPromotedIncomingGeneration=0;
        gl.useProgram(0); drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
        gl.bindFramebuffer(framebuffer, 0); gl.bindRenderbuffer(renderbuffer, 0);
        if (usesTextures()) {
            if (request.enableOriginalUiSourceRgba) {
                gl.activeTexture(textureUnit0+1);gl.bindTexture(texture2D,0);
                if (gl.bindSampler)gl.bindSampler(1,0);
            }
            gl.activeTexture(textureUnit0); gl.bindTexture(texture2D, 0);
            if (gl.bindSampler) gl.bindSampler(0, 0);
        }
        const U unbindError = drawGl.getError();
        if (unbindError) {
            if (!d.cleanupGlError) d.cleanupGlError = unbindError;
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "unbind renderer resources before cleanup");
        }
        bool complete = true;
        if (shader && !shader->release(d.cleanupShader)) {
            if (!d.cleanupGlError) d.cleanupGlError = d.cleanupShader.glError ?
                d.cleanupShader.glError : d.cleanupShader.cleanupGlError;
            complete = false;
        }
        // Only proceed after an acknowledged clean error state. Stop at a
        // failing deletion instead of conflating it with subsequent objects.
        if (!drainCleanupErrors(d)) complete = false;
        else {
            if (!deleteUiByteBlendNames(d))goto incomplete;
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
                for (U c = 0; c < 3; ++c) {
                    if (!deleteName(bank.vertexArray[c], gl.deleteVertexArrays, d) ||
                        !deleteName(bank.vertexBuffer[c], gl.deleteBuffers, d)) goto incomplete;
                }
            }
            for (auto& bank : racerNormalBuffers)
                for (auto& normal : bank)
                    if (!deleteName(normal, gl.deleteBuffers, d)) goto incomplete;
            for (auto& index : resources.indices)
                if (!deleteName(index.buffer, gl.deleteBuffers, d)) goto incomplete;
            for (auto& archetype : racerArchetypes) {
                for (auto& bank : archetype.normalBuffers)
                    for (auto& normal : bank)
                        if (!deleteName(normal, gl.deleteBuffers, d)) goto incomplete;
                for (auto& bank : archetype.resources.bank)
                    for (U c = 0; c < 3; ++c)
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
            for (U b = 0; b < 2; ++b)
                if (!deleteName(targets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(colors[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(depths[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(menuTargets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(menuColors[b], gl.deleteRenderbuffers, d)) goto incomplete;
        }
        d.cleanupIncomplete = hasGlResources();
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
    if (storage_) return fail(d, VehicleWindowError50DS::alreadyOwned, "renderer already owns resources");
    if (!request.procAddress || !request.isCurrent ||
        (request.mode != VehicleWindowMode50DS::viewerNeutralUntextured &&
            request.mode != VehicleWindowMode50DS::callerMaterialTextures) ||
        (request.output != VehicleWindowOutput50DS::defaultBackBuffer &&
            request.output != VehicleWindowOutput50DS::headless) ||
        request.maximumWidth <= 0 || request.maximumHeight <= 0 ||
        std::uint64_t(request.maximumWidth) * std::uint64_t(request.maximumHeight) >
            std::numeric_limits<std::size_t>::max() /
                (16u + (request.enableHostHud ? 8u : 0u) + (request.enableOriginalUi ? 16u : 0u)))
        return fail(d, VehicleWindowError50DS::request,
            "context callbacks, valid output/vehicle shading modes and positive target bounds required");
    if (!assets.loaded()) return fail(d, VehicleWindowError50DS::assets, "50CR assets are not loaded");
    if (!validateTrack(request, d) || !validateMaterials(request, assets, d) ||
        !validateCables(request, d) || !validateSky(request, d) || !validateDynamicWorld(request, d) ||
        !validateTrackAnimations(request, d)) return false;
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

bool VehicleWindowGl50DS::submit(const game::VehicleDrawBatch50CO& batch,
    const VehicleWindowFrame50DS& frame, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d) || !s.texturesCurrent(d)) return false;
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
    if (!s.validateMaterialUvs(frame.materialUvs, frame.materialUvCount, d) ||
        !s.validateOpponents(batch, frame, d) || !s.validateDynamicWorldFrame(frame.dynamicWorld, d) ||
        !s.validateTrackAnimationFrames(frame, d)) return false;
    if (bool(s.request.cables.textures) != bool(frame.cables))
        return fail(d, VehicleWindowError50DS::frame, "cable frame presence must match explicit cold cable request");
    if (frame.cables && !validateCableFrame(*frame.cables, d)) return false;
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
        if (hud.raceEnabled && (!hud.raceLapTarget || hud.raceLapTarget > 999 ||
            !hud.raceLap || hud.raceLap > hud.raceLapTarget || !hud.raceRacerCount ||
            hud.raceRacerCount > 16 || !hud.racePlace || hud.racePlace > hud.raceRacerCount ||
            hud.raceCountdown > 3 ||
            unsigned(hud.raceCountdown != 0) + unsigned(hud.raceGo) + unsigned(hud.raceFinished) > 1))
            return fail(d, VehicleWindowError50DS::frame, "native race HUD counters/status outside admitted bounds");
        if (hud.raceResultsReady && (!hud.raceEnabled || !hud.raceFinished || hud.raceCountdown || hud.raceGo))
            return fail(d, VehicleWindowError50DS::frame, "host results presentation requires a sealed human finish");
        if (hud.resultsPanelVisible && (!hud.raceResultsReady || !hud.standingsCount ||
            !hud.resultsFinishPlace || hud.resultsFinishPlace != hud.racePlace ||
            hud.resultsSelectedAction >= 3 || !std::isfinite(hud.resultsFinishSeconds) ||
            hud.resultsFinishSeconds < 0.0 || hud.resultsFinishSeconds > double(maxVehicleWindowLapSeconds50DS) ||
            (!hud.resultsConfirming && hud.resultsConfirmAccept)))
            return fail(d, VehicleWindowError50DS::frame, "live results panel requires sealed finish and bounded action/time");
        if (hud.raceClocksValid) {
            const auto validSeconds = [](float value) noexcept {
                return std::isfinite(value) && value >= 0.f && value <= maxVehicleWindowLapSeconds50DS;
            };
            if (!hud.raceEnabled || !validSeconds(hud.raceElapsedSeconds) ||
                (!hud.raceFinished && !validSeconds(hud.currentLapSeconds)))
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
                if (row.racerIndex >= hud.standingsCount || seen[row.racerIndex] || row.place != i + 1 ||
                    !validHostHudName(row.displayName) || row.completedLaps > maxVehicleWindowCompletedLaps50DS ||
                    row.finished != (row.completedLaps >= hud.raceLapTarget) ||
                    (row.finished && unfinishedSeen) || (hud.raceCountdown && row.completedLaps))
                    return fail(d, VehicleWindowError50DS::frame, "named standings order/name/lap/finish state rejected");
                seen[row.racerIndex] = true;
                unfinishedSeen = unfinishedSeen || !row.finished;
                if (row.racerIndex == hud.standingsPlayerIndex &&
                    (row.place != hud.racePlace || row.finished != hud.raceFinished ||
                     std::min(row.completedLaps + 1, hud.raceLapTarget) != hud.raceLap))
                    return fail(d, VehicleWindowError50DS::frame, "named standings player differs from race HUD");
            }
        }
        if (!s.hostHudGeometry->build(hud, frame.width, frame.height))
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
    s.frameOpponentCount = frame.opponentCount;
    s.framePlayerBatch = batch;
    for (U i = 0; i < frame.opponentCount; ++i) {
        const auto& opponent = frame.opponents[i];
        s.frameOpponentBatches[i] = *opponent.batch;
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
    if (frame.skyWorldViewProjection)
        std::memcpy(s.skyMatrix, frame.skyWorldViewProjection, sizeof(s.skyMatrix));
    if (s.request.track.worldFog.enabled)
        std::memcpy(s.worldFogCamera, frame.worldFogCamera, sizeof(s.worldFogCamera));
    std::memcpy(s.matrix, frame.worldViewProjection, sizeof(s.matrix));
    std::memcpy(s.clearRgba, frame.clearRgba, sizeof(s.clearRgba));
    s.width = frame.width; s.height = frame.height;
    s.frameCables = frame.cables;
    s.frameDynamicWorld = frame.dynamicWorld;
    for (U i = 0; i < s.trackAnimationCount; ++i)
        s.trackAnimations[i].selectedFrame = frame.trackAnimationFrames[i];
    s.frameHostHud = frame.hostHud != nullptr;
    s.frameSet = true; s.diagnostic = &d;
    s.prepareTrackVisibility();
    bool accepted = s.check("before private submission") &&
        (s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    if (accepted) {
        accepted = game::submitVehicleDrawBatch50CS(batch, s.adapter->backend(), d.submission);
        if (!accepted) {
            // Structural rejection precedes adapter error reset: never copy an
            // old GL error into a new structural-rejection diagnostic.
            if (!d.glError && d.submission.error >= game::VehicleDrawSubmitError50CS::backendPreflight)
                d.glError = s.adapter->glError();
            fail(d, VehicleWindowError50DS::submission, "50CS/50CV submission rejected");
        } else {
            d.batchAccepted = true;
            if (s.request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
                accepted = s.copyToWindow();
                if (!accepted && d.error == VehicleWindowError50DS::gl)
                    d.error = VehicleWindowError50DS::presentation;
            }
        }
    }
    s.frameSet = false; s.frameCables = nullptr; s.frameHostHud = false; s.diagnostic = nullptr;
    s.framePlayerBatch = {};
    s.frameDynamicWorld = {};
    for (U i = 0; i < s.frameOpponentCount; ++i) {
        s.frameOpponentBatches[i] = {};
        s.frameOpponentCables[i] = nullptr;
        s.frameOpponentArchetypes[i] = 0;
    }
    s.frameOpponentCount = 0;
    return accepted;
}

bool VehicleWindowGl50DS::bindOriginalUiTextures(const assets::MaterialTextureAssets50EA& cpu,
    const MaterialTextureGl50EB& gpu, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableOriginalUi || s.diagnostic)
        return fail(d, VehicleWindowError50DS::frame, "UI bank replacement requires idle original UI renderer");
    auto candidate = s.request;
    candidate.originalUiCpuTextures = &cpu;
    candidate.originalUiTextures = &gpu;
    s.diagnostic = &d;
    const bool accepted = s.validateOriginalUiBank(candidate);
    s.diagnostic = nullptr;
    if (!accepted) return false;
    s.request.originalUiCpuTextures = &cpu;
    s.request.originalUiTextures = &gpu;
    s.uiTexturePublication = gpu.publicationToken();
    return true;
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
bool VehicleWindowGl50DS::submitOriginalUiSourceRgba(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,VehicleWindowDiagnostic50DS& d) noexcept {
    return submitOriginalUiSourceFrame(frame,width,height,true,d);
}
bool VehicleWindowGl50DS::submitOriginalUiSourceFrame(const OriginalUiSourceFrame50KM& frame,
    std::int32_t width,std::int32_t height,bool publishSource,VehicleWindowDiagnostic50DS& d) noexcept {
    d={};
    if (!originalUiSourceRgbaAvailable())return fail(d,VehicleWindowError50DS::context,"source UI composition is unavailable");
    auto& s=*storage_;
    if (!frame.native_ || !frame.owner_ || !frame.frameGeneration_ ||
        (s.uiSourceFrameOwner && frame.owner_!=s.uiSourceFrameOwner) ||
        (frame.owner_==s.uiSourceFrameOwner && frame.frameGeneration_<=s.uiSourceFrameGeneration) ||
        s.uiSourceTargetGeneration==std::numeric_limits<std::uint64_t>::max() ||
        (publishSource && s.uiSourcePublicationGeneration==std::numeric_limits<std::uint64_t>::max()))
        return fail(d,VehicleWindowError50DS::frame,"unsealed or stale complete Rider frame");
    if (!frame.owner_->beginSourceSubmission(frame.frameGeneration_))
        return fail(d,VehicleWindowError50DS::frame,"Rider source binding-history receipt rejected");
    bool sourceWorkStarted=false;
    const auto sourceTargetBefore=s.uiSourceTargetGeneration;
    const bool accepted=submitOriginalUiImpl(*frame.native_,width,height,true,d,&sourceWorkStarted,publishSource);
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
    if(accepted){s.gl.finish();accepted=s.check("complete exact source target work");}
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
    // Authenticated Mat_Stbl first-match token3217 is ui/textures/icon_x.
    // Read the real completed drain's owned fields, including inactive FIX and
    // clamp limits. Effect TEX1=0 is separate from this ordinary sampler state.
    return terminal.canonicalToken==3217 && terminal.pass==1 &&
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
        return fail(d,VehicleWindowError50DS::material,"byte-blend requires actual terminal icon_x/pass1 and exact inherited GS state");
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
bool VehicleWindowGl50DS::retireOriginalUiSourceOwner(
    const OriginalUiRiderDrawContribution50KG& owner,VehicleWindowDiagnostic50DS& d) noexcept {
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
bool VehicleWindowGl50DS::submitOriginalUiImpl(const OriginalUiNativeDraw50KG& native,
    std::int32_t width,std::int32_t height,bool sourceRgba,VehicleWindowDiagnostic50DS& d,
    bool* sourceWorkStarted,bool publishSource) noexcept {
    d = {};
    if(sourceWorkStarted)*sourceWorkStarted=false;
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_; const auto& input = native.draw;
    d.originalUiCoherentFramebufferFetch=sourceRgba && s.uiSourceCoherentFramebufferFetch;
    if (!s.current(d)) return false;
    if (s.active || s.frameSet || !s.request.enableOriginalUi || !s.uiProgram ||
        !s.uiVertexBuffer || !s.uiIndexBuffer || !s.uiVertexArray || !s.uiParticleFogBuffer || !s.uiSourceRasterWBuffer ||
        !s.uiTargets[0] || !s.uiTargets[1] ||
        width <= 0 || height <= 0 || width > s.maximumWidth || height > s.maximumHeight ||
        input.canvasWidth != 640.f || input.canvasHeight != 448.f ||
        !input.quadCount || input.quadCount > (sourceRgba?originalUiMaximumSourceQuads50KM:originalUiMaximumQuads50KG) || !input.vertices ||
        !input.spanCount || input.spanCount > originalUiMaximumSpans50KG || !input.spans ||
        !native.states || native.stateCount != input.spanCount ||
        (sourceRgba ? native.opaquePresentationAlphaOnly : !native.opaquePresentationAlphaOnly))
        return fail(d, VehicleWindowError50DS::frame, "original UI requires explicit native policy and bounded 640x448 draw");
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
    U next = 0;
    bool anyFog = false, anySourceRaster = false;
    for (U i = 0; i < input.spanCount; ++i) {
        const auto& span = input.spans[i]; const auto& state = native.states[i];
        const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
        const bool specular=span.materialMode==OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
        const bool sourceRaster = span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuse ||
            span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuseCompressed || specular;
        const bool specularBlend=span.blend==OriginalUiBlend50KG::destinationAlphaAdditive ||
            span.blend==OriginalUiBlend50KG::fixed255Additive;
        const bool shadowBlend=span.blend==OriginalUiBlend50KG::destinationSourceAlphaAttenuation;
        const bool sourceSpecialBlend=specularBlend||shadowBlend;
        if (shadowBlend && (!sourceRgba || span.materialMode!=OriginalUiMaterialMode50KG::sourceDiffuseCompressed ||
            state.depth!=OriginalUiDepth50KG::greaterEqual || !state.depthWrite ||
            !state.sourceFogEnabled || !state.sourceAlphaTestGreaterZero))
            return fail(d,VehicleWindowError50DS::material,"shadow requires actual typed source-RGBA policy");
        if ((specular && !sourceRgba) || specularBlend!=specular ||
            (specular && (state.depth!=OriginalUiDepth50KG::greaterEqual || state.depthWrite ||
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

    s.diagnostic = &d;
    bool accepted = s.check("before original UI submission") &&
        ((sourceRgba && !publishSource) || s.request.output == VehicleWindowOutput50DS::headless || s.checkDefaultFramebuffer());
    const auto pending = static_cast<std::uint8_t>(s.uiPublished ^ 1u);
    if (accepted) {
        // Sampler zero exposes the immutable published texture parameters.
        // Cache only this submission's actual GL image observations. Compare
        // every span's expectations independently: same-image conflicts reject.
        // Zero-map spans have no sampled image or texture parameters to query.
        struct SamplerObservation { U image; I values[6]; };
        SamplerObservation observed[originalUiMaximumSpans50KG];
        U observedCount = 0;
        constexpr U parameters[] = {0x2801, 0x2800, 0x2802, 0x2803, 0x813C, 0x813D};
        s.gl.activeTexture(textureUnit0); if (s.gl.bindSampler) s.gl.bindSampler(0, 0);
        for (U i = 0; accepted && i < input.spanCount; ++i) {
            if (input.spans[i].materialMode == OriginalUiMaterialMode50KG::untexturedVertexColor) continue;
            const auto& sampler = native.states[i].sampler;
            const U image = input.textures->texture(input.spans[i].textureIndex);
            U observation = 0;
            while (observation < observedCount && observed[observation].image != image) ++observation;
            const bool firstObservation = observation == observedCount;
            // At most one new entry per span; spanCount was bounded above.
            auto& actual = observed[observation];
            if (firstObservation) {
                actual.image = image;
                s.gl.bindTexture(texture2D, image);
            }
            const I expected[] = {
                sampler.minification == MaterialTextureFilter50EB::nearest ? 0x2600 : 0x2601,
                sampler.magnification == MaterialTextureFilter50EB::nearest ? 0x2600 : 0x2601,
                sampler.s == MaterialTextureWrap50EB::repeat ? 0x2901 : 0x812F,
                sampler.t == MaterialTextureWrap50EB::repeat ? 0x2901 : 0x812F, 0, 0};
            for (U p = 0; accepted && p < 6; ++p) {
                if (firstObservation) {
                    actual.values[p] = -1;
                    s.gl.getTexParameteriv(texture2D, parameters[p], &actual.values[p]);
                    accepted = s.check("verify per-span original UI sampler");
                }
                if (accepted && actual.values[p] != expected[p])
                    accepted = fail(d, VehicleWindowError50DS::material,
                        "UI span source sampler differs from immutable bank texture parameters");
            }
            if (firstObservation && accepted) ++observedCount;
        }
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
        s.gl.clear(colorBufferBit | depthBufferBit);
        if(sourceRgba){++s.uiSourceTargetGeneration;s.uiSourceAcceptedTargetGeneration=0;
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
        s.gl.enable(0x0B71); s.gl.useProgram(s.uiProgram);
        s.gl.uniform4f(s.uiCanvasUniform,float(2.0*scale/rasterWidth),float(-2.0*scale/rasterHeight),
            float(2.0*ox/rasterWidth-1.0),float(1.0-2.0*oy/rasterHeight));
        s.gl.uniform1i(s.uiSourceCompositionUniform,sourceRgba?1:0);
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
        s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(input.quadCount) * 4 * sizeof(assets::Vertex48B), input.vertices);
        if (anyFog) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiParticleFogBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(native.particleFogCount) * sizeof(float), native.particleFog);
        }
        if (anySourceRaster) {
            s.drawGl.bindBuffer(arrayBuffer, s.uiSourceRasterWBuffer);
            s.drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(native.sourceRasterWCount) * sizeof(float), native.sourceRasterW);
        }
        s.drawGl.bindBuffer(elementBuffer, sourceRgba ? s.uiSourceIndexBuffer : s.uiIndexBuffer);
        if (sourceRgba)
            s.drawGl.bufferSubData(elementBuffer, 0, std::intptr_t(sourceIndexCount) * sizeof(U),
                s.uiSourceIndices->indices);
        accepted = s.check("prepare authenticated original UI frame");
        // No material representative has bound before this point. Read-only
        // preflight/context/window failures remain retryable with the same
        // composer generation. A later partial source pass failure is unknown.
        if(sourceRgba&&accepted&&input.spanCount&&sourceWorkStarted)*sourceWorkStarted=true;
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
                span.blend==OriginalUiBlend50KG::fixed255Additive?4:5;
            s.gl.uniform1i(s.uiSourceBlendUniform,sourceBlend);
            const bool particle = span.materialMode == OriginalUiMaterialMode50KG::mainParticle;
            const bool sourceRaster = span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuse ||
                span.materialMode == OriginalUiMaterialMode50KG::sourceDiffuseCompressed ||
                span.materialMode == OriginalUiMaterialMode50KG::sourceSpecularMapCompressed;
            const bool textured = span.materialMode == OriginalUiMaterialMode50KG::texturedModulate || particle || sourceRaster;
            s.gl.uniform1i(s.uiTexturedUniform, textured ? 1 : 0);
            s.gl.uniform1i(s.uiSourceRasterUniform, sourceRaster ? 1 : 0);
            s.gl.uniform1i(s.uiSourceFogEnabledUniform, (particle || state.sourceFogEnabled) ? 1 : 0);
            s.gl.uniform1i(s.uiSourceAlphaTestUniform, (particle || state.sourceAlphaTestGreaterZero) ? 1 : 0);
            s.gl.bindTexture(texture2D, textured ? input.textures->texture(span.textureIndex) : 0);
            if(sourceRgba){
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
                if(accepted){d.originalUiDraws+=sourceDraws;d.originalUiSourceTriangles+=sourceTriangles;
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
            d.originalUiSourceAccepted=true;
            if(publishSource)accepted=s.publishUiSource(input.clearRgba,width,height);
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
    s.diagnostic = nullptr;
    return accepted;
}

bool VehicleWindowGl50DS::submitHostMenu(const VehicleWindowHostMenu50DS& input,
    std::int32_t width, std::int32_t height, VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
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

bool VehicleWindowGl50DS::finishSubmittedWork(VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!initialized()) return fail(d, VehicleWindowError50DS::notInitialized, "renderer not initialized");
    auto& s = *storage_;
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
    s.diagnostic = nullptr;
    return result;
}

bool VehicleWindowGl50DS::close(VehicleWindowDiagnostic50DS& d) noexcept {
    d = {};
    if (!storage_) return true;
    const bool result = storage_->cleanup(d);
    if (!d.cleanupIncomplete) storage_.reset();
    return result;
}
bool VehicleWindowGl50DS::initialized() const noexcept { return storage_ && storage_->initialized; }
bool VehicleWindowGl50DS::ownsResources() const noexcept { return bool(storage_); }
}
