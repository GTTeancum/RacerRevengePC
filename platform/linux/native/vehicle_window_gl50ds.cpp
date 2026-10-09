#include "vehicle_window_gl50ds.h"
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
    if (request.enableNativePodShadows && (!track.mesh ||
        (track.mode != VehicleWindowMode50DS::sourceVertexColors &&
         track.mode != VehicleWindowMode50DS::callerMaterialTextures)))
        return fail(d, VehicleWindowError50DS::request, "native pod shadows require actual source-color/textured track");
    if (!track.mesh)
        return (!track.maximumVertexBytes && !track.maximumIndexBytes &&
            track.mode == VehicleWindowMode50DS::unspecified && track.vertexColorScale == 1.f &&
            !track.textures && !track.spans && !track.spanCount && !track.enableFrustumCulling &&
            !track.cullingChunkTriangles && !track.sourceNormals && !track.sourceNormalCount &&
            !track.maximumNormalBytes) ||
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
            if (span.indexStart != nextIndex || span.vertexStart != nextVertex ||
                !span.indexCount || span.indexCount % 3 || !span.vertexCount ||
                span.indexCount > mesh.indices.size() - nextIndex ||
                span.vertexCount > mesh.vertices.size() - nextVertex)
                return fail(d, VehicleWindowError50DS::indices,
                    "track spans must partition source index and vertex arrays in order");
            nextIndex += span.indexCount;
            nextVertex += span.vertexCount;
            for (U n = span.indexStart; n < nextIndex; ++n)
                if (mesh.indices[n] < span.vertexStart || mesh.indices[n] >= nextVertex)
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
        if (nextIndex != mesh.indices.size() || nextVertex != mesh.vertices.size())
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
            !material.unlit || sky.spans[i].discardZeroAlpha)
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
    void standingsPanel(const VehicleWindowHostHud50DS& input, I width, I height) noexcept {
        if (!input.standingsCount) return;
        // Keep every existing HUD anchor and scale unchanged. Only this added
        // panel contracts when the viewport cannot fit it between the top race
        // readout and bottom heat/damage panel, or beside the right route cue.
        const float hudScale = scale;
        const float panelWidth = 222.f, panelHeight = 22.f + 11.f * float(input.standingsCount);
        const float top = 44.f * hudScale;
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
        scale = hudScale;
    }
    bool build(const VehicleWindowHostHud50DS& input, I width, I height) noexcept {
        count = 0; overflow = false;
        scale = width >= 960 && height >= 540 ? 2.f : 1.f;
        scale = std::min(scale, std::min(float(width) / 176.f,
            float(height) / (input.raceEnabled ? 112.f : input.lapCountValid ? 100.f : 72.f)));
        inverseWidth = 2.f / float(width); inverseHeight = 2.f / float(height);
        rectangle(0.f, 0.f, 160.f, 56.f, 0x617589);
        rectangle(1.f, 1.f, 158.f, 54.f, 0x0A111B);
        text(6.f, 5.f, "HOST HUD", 0xA8C5DE);
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
            rectangle(0.f, y, 168.f, 32.f, 0x617589);
            rectangle(1.f, y + 1.f, 166.f, 30.f, 0x0A111B);
            char laps[32]{}, place[32]{}, countdown[32]{};
            std::snprintf(laps, sizeof(laps), "LAP %u/%u", input.raceLap, input.raceLapTarget);
            std::snprintf(place, sizeof(place), "POS %u/%u", input.racePlace, input.raceRacerCount);
            std::snprintf(countdown, sizeof(countdown), "START IN %u", input.raceCountdown);
            text(6.f, y + 5.f, laps, 0xEDF3F8);
            text(90.f, y + 5.f, place, 0xEDF3F8);
            const char* status = input.raceFinished ? "FINISHED" : input.raceCountdown ? countdown :
                input.raceGo ? "GO" : input.wrongWay ? "WRONG WAY" : "RACING";
            text(6.f, y + 19.f, status, input.raceFinished || input.raceGo ? 0xA8E6B4 :
                input.wrongWay ? 0xFF6556 : 0xA8C5DE);
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
            text(x + 10.f, y + 5.f, "HOST", 0xA8C5DE);
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
    void (*activeTexture)(U) = nullptr;
    void (*bindTexture)(U, U) = nullptr;
    // Only loaded if GL >= 3.3 or ARB_sampler_objects is advertised.
    void (*bindSampler)(U, U) = nullptr;
    const unsigned char* (*getStringi)(U, U) = nullptr;
    void (*viewport)(I, I, I, I) = nullptr;
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
    };
    struct TrackChunk {
        U sourceSpan = 0, indexStart = 0, indexCount = 0;
    };
    VehicleWindowRequest50DS request{};
    VehicleGlFunctions50CV drawGl{};
    VehicleGlShaderFunctions50CZ shaderGl{};
    OwnerGl gl{};
    VehicleGlResources50CV resources{};
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
        Material* materials;
        const U* materialOffsets;
        U materialCount;
        U cableTextureName;
    };
    RacerView racerView(U archetype) const noexcept {
        if (!archetype) return {meshes, scenes, &resources, materials.get(),
            materialOffsets, materialCount, cableTextureName};
        const auto& source = racerArchetypes[archetype - 1];
        return {source.meshes, source.scenes, &source.resources, source.materials.get(),
            source.materialOffsets, source.materialCount, source.cableTextureName};
    }
    std::uint64_t texturePublication = 0;
    std::unique_ptr<TrackSpan[]> trackSpans;
    U trackSpanCount = 0;
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
    U trackVertexBuffer = 0, trackIndexBuffer = 0, trackVertexArray = 0, trackNormalBuffer = 0;
    U trackProgram = 0, trackVertexShader = 0, trackFragmentShader = 0;
    I trackMatrixUniform = -1, trackColorScaleUniform = -1;
    I trackUseTextureUniform = -1, trackImageUniform = -1;
    I trackDiscardZeroAlphaUniform = -1, trackUseSourceNormalsUniform = -1;
    I trackEffectModeUniform = -1, trackAlphaScaleUniform = -1, trackUnlitUniform = -1;
    I trackIndexCount = 0;
    NativePodShadow50HM podShadows[nativeShadowRacerCount][3]{};
    float podShadowOpponentBounds[maxVehicleWindowOpponents50DS][4]{};
    I nativeShadowEnabledUniform = -1;
    I nativeShadowOpponentCountUniform = -1;
    I nativeShadowCenterUniforms[nativeShadowCasterCount]{}, nativeShadowShapeUniforms[nativeShadowCasterCount]{};
    I nativeShadowOpponentBoundsUniforms[maxVehicleWindowOpponents50DS]{};
    U targets[2]{}, colors[2]{}, depths[2]{};
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
            request.sky.textures;
    }
    bool usesTrackShader() const noexcept {
        return request.track.mode == VehicleWindowMode50DS::sourceVertexColors ||
            request.track.mode == VehicleWindowMode50DS::callerMaterialTextures;
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
        diagnostic->trackCulledIndices = static_cast<U>(trackIndexCount) - retainedIndices;
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
        if (cableVertexBuffer || cableIndexBuffer || cableVertexArray) return true;
        if (trackVertexBuffer || trackIndexBuffer || trackVertexArray || trackNormalBuffer) return true;
        if (skyVertexBuffer || skyIndexBuffer || skyVertexArray) return true;
        for (U b = 0; b < 2; ++b) {
            if (targets[b] || colors[b] || depths[b]) return true;
            for (U c = 0; c < 3; ++c)
                if (resources.bank[b].vertexBuffer[c] || resources.bank[b].vertexArray[c]) return true;
        }
        for (const auto& index : resources.indices) if (index.buffer) return true;
        for (const auto& archetype : racerArchetypes) {
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
        if (request.output == VehicleWindowOutput50DS::defaultBackBuffer) {
            LOAD(gl, blitFramebuffer, BlitFramebuffer);
        }
        if (usesTextures()) {
            LOAD(gl, activeTexture, ActiveTexture); LOAD(gl, bindTexture, BindTexture);
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
uniform mat4 worldViewProjection;
out vec3 trackWorldNormal;
out vec3 trackWorldPosition;
out vec4 trackSourceColor;
out vec2 trackSourceUv;
void main() {
    gl_Position = vec4(rrPosition, 1.0) * worldViewProjection;
    trackWorldPosition = rrPosition;
    trackWorldNormal = rrWorldNormal;
    trackSourceColor = rrColor;
    trackSourceUv = rrUv;
}
)GLSL";
        constexpr const char* fragmentSource = R"GLSL(#version 150 core
in vec3 trackWorldPosition;
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
        trackMatrixUniform = shaderGl.getUniformLocation(trackProgram, "worldViewProjection");
        trackColorScaleUniform = shaderGl.getUniformLocation(trackProgram, "vertexColorScale");
        trackUseTextureUniform = shaderGl.getUniformLocation(trackProgram, "useTexture");
        trackImageUniform = shaderGl.getUniformLocation(trackProgram, "image");
        trackDiscardZeroAlphaUniform = shaderGl.getUniformLocation(trackProgram, "discardZeroAlpha");
        trackEffectModeUniform = shaderGl.getUniformLocation(trackProgram, "effectMode");
        trackAlphaScaleUniform = shaderGl.getUniformLocation(trackProgram, "alphaScale");
        trackUnlitUniform = shaderGl.getUniformLocation(trackProgram, "unlit");
        trackUseSourceNormalsUniform = shaderGl.getUniformLocation(trackProgram, "useSourceNormals");
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
        if (trackMatrixUniform < 0 || trackColorScaleUniform < 0 ||
            trackUseTextureUniform < 0 || trackImageUniform < 0 || trackDiscardZeroAlphaUniform < 0 ||
            trackEffectModeUniform < 0 || trackAlphaScaleUniform < 0 || trackUnlitUniform < 0 ||
            trackUseSourceNormalsUniform < 0 || nativeShadowEnabledUniform < 0 || nativeShadowOpponentCountUniform < 0)
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
        U& trackIndexBuffer, U& trackVertexArray, I& trackIndexCount) noexcept {
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
            gl.genBuffers(1, buffers[b]);
            drawGl.bindBuffer(arrayBuffer, *buffers[b]);
            gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bufferBytes[b]), inputs[b], staticDraw);
            if (!check("upload static world-space track buffer")) return false;
            if (!*buffers[b])
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero track buffer name");
            if (!verifyBuffer(arrayBuffer, bufferBytes[b], staticDraw)) return false;
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
    bool createTrackNormals(const VehicleWindowTrack50DS& track) noexcept {
        if (!track.sourceNormals) return true;
        const std::size_t bytes = std::size_t(track.sourceNormalCount) * sizeof(assets::WorldNormal50HK);
        gl.genBuffers(1, &trackNormalBuffer);
        drawGl.bindBuffer(arrayBuffer, trackNormalBuffer);
        gl.bufferData(arrayBuffer, static_cast<std::intptr_t>(bytes), track.sourceNormals, staticDraw);
        if (!check("upload authenticated track source normals")) return false;
        if (!trackNormalBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero source normal buffer name");
        if (!verifyBuffer(arrayBuffer, bytes, staticDraw)) return false;
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
    bool createTargets() noexcept {
        I maximum = 0, viewportLimits[2]{};
        gl.getIntegerv(0x84E8, &maximum); // GL_MAX_RENDERBUFFER_SIZE
        gl.getIntegerv(0x0D3A, viewportLimits); // GL_MAX_VIEWPORT_DIMS
        if (!check("query target limits")) return false;
        if (maximumWidth > maximum || maximumHeight > maximum ||
            maximumWidth > viewportLimits[0] || maximumHeight > viewportLimits[1])
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
        }
        return true;
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
    bool checkContext() noexcept {
        const auto* version = gl.getString(0x1F02); // GL_VERSION
        I major = 0, minor = 0;
        gl.getIntegerv(0x821B, &major); gl.getIntegerv(0x821C, &minor);
        if (!check("query desktop GL version")) return false;
        if (!version || std::strncmp(reinterpret_cast<const char*>(version), "OpenGL ES", 9) == 0 ||
            major < 3 || (major == 3 && minor < 2))
            return fail(*diagnostic, VehicleWindowError50DS::context, "desktop OpenGL 3.2 or newer required");
        if (!checkTextureSampling(major, minor)) return false;
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
        request.racerArchetypes = nullptr; // Cold descriptors are never retained.
        // Track CPU bytes are borrowed only for this cold synchronous call.
        // No retained pointer, including after a failed cleanup, is needed.
        request.track.mesh = nullptr;
        request.track.spans = nullptr;
        request.track.sourceNormals = nullptr;
        request.sky.mesh = nullptr;
        request.sky.spans = nullptr;
        // Mapping arrays are cold-only; retain just the 50EB owner and its
        // publication token, never pointers into caller mapping storage.
        for (auto& component : request.materials.component) component = {};
        texturePublication = input.materials.textures ? input.materials.textures->publicationToken() : 0;
        trackTexturePublication = input.track.textures ? input.track.textures->publicationToken() : 0;
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
            !createTrackMaterials(input.track) ||
            !createTrackVisibility(input.track) || !createSkyMaterials(input.sky)) return false;
        if (!loadFunctions() || !check("initial GL error state") || !checkContext()) return false;
        shader.reset(new (std::nothrow) VehicleGlShader50CZ(shaderGl));
        if (!shader) return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate shader owner");
        if (!shader->initialize(diagnostic->shader))
            return fail(*diagnostic, VehicleWindowError50DS::shader, "initialize 50CZ viewer shader");
        if (!createTrackShader() || !createGeometry(resources, meshes)) return false;
        for (U i = 0; i < racerArchetypeCount; ++i)
            if (!createGeometry(racerArchetypes[i].resources, racerArchetypes[i].meshes)) return false;
        if (
            !createStaticGeometry(input.track.mesh, trackVertexBuffer, trackIndexBuffer, trackVertexArray, trackIndexCount) ||
            !createTrackNormals(input.track) ||
            !createStaticGeometry(input.sky.mesh, skyVertexBuffer, skyIndexBuffer, skyVertexArray, skyIndexCount) ||
            !createCableGeometry() || !createHostHud() ||
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
        if (s.trackIndexCount && (!s.trackVertexBuffer || !s.trackIndexBuffer || !s.trackVertexArray))
            return fail(*s.diagnostic, VehicleWindowError50DS::assets, "static track resources are not ready");
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
        if (policy.blend == VehicleWindowBlend50DS::opaque) gl.disable(0x0BE2);
        else {
            gl.enable(0x0BE2);
            gl.blendEquation(0x8006); // FUNC_ADD; never inherit caller equations.
            gl.blendFunc(policy.fixedBlendAlpha ? 1 : 0x0302,
                policy.blend == VehicleWindowBlend50DS::additive ? 1 : 0x0303);
        }
    }
    void resetPodState() noexcept {
        gl.disable(0x0BE2); gl.enable(0x0B71); gl.depthMask(1);
        gl.useProgram(shader->program());
        const auto& u = shader->uniforms();
        gl.uniformMatrix4fv(u.worldViewProjection, 1, 0, matrix);
        gl.uniform1i(u.useTexture, 0); gl.uniform1i(u.image, 0);
        gl.uniform1i(u.effectMode, 0); gl.uniform1f(u.alphaScale, 1.f);
        gl.uniform1i(u.suppressDraw, 0); gl.uniform1i(u.useUvTransform, 0);
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
            if (s.trackProgram) {
                s.gl.useProgram(s.trackProgram);
                s.gl.uniformMatrix4fv(s.trackMatrixUniform, 1, 0, s.matrix);
                s.gl.uniform1f(s.trackColorScaleUniform, s.request.track.vertexColorScale);
                s.gl.uniform1i(s.trackUseTextureUniform, 0);
                s.gl.uniform1i(s.trackImageUniform, 0);
                s.gl.uniform1i(s.trackDiscardZeroAlphaUniform, 0);
                s.gl.uniform1i(s.trackEffectModeUniform, 0);
                s.gl.uniform1f(s.trackAlphaScaleUniform, 1.f);
                s.gl.uniform1i(s.trackUnlitUniform, 0);
                s.gl.uniform1i(s.trackUseSourceNormalsUniform, s.trackNormalBuffer ? 1 : 0);
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
                    if ((span.material.policy.blend != VehicleWindowBlend50DS::opaque) != transparent) continue;
                    s.diagnostic->trackSpan = i;
                    s.diagnostic->texture = span.material.textureIndex;
                    // Retained chunks of one span remain contiguous. Preserve
                    // its exact material policy without rebinding every chunk.
                    if (boundSpan != i) {
                        s.gl.bindTexture(texture2D, span.material.name);
                        s.gl.uniform1i(s.trackUseTextureUniform, span.material.name ? 1 : 0);
                        s.gl.uniform1i(s.trackDiscardZeroAlphaUniform, span.discardZeroAlpha ? 1 : 0);
                        s.configureBlend(span.material.policy);
                        s.gl.uniform1i(s.trackEffectModeUniform, effectMode(span.material.policy));
                        s.gl.uniform1f(s.trackAlphaScaleUniform, span.material.policy.alphaScale);
                        s.gl.uniform1i(s.trackUnlitUniform, span.material.policy.unlit ? 1 : 0);
                        s.gl.uniform1i(s.nativeShadowEnabledUniform, s.request.enableNativePodShadows &&
                            !transparent && span.material.policy.depthWrite && !span.material.policy.unlit ? 1 : 0);
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
                    if (transparent) ++s.diagnostic->transparentTrackDraws;
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
    bool applyPodMaterial(const Material& material, bool suppress, const float* overrideUv = nullptr) noexcept {
        configureBlend(material.policy);
        const auto& u = shader->uniforms();
        gl.bindTexture(texture2D, material.name);
        gl.uniform1i(u.useTexture, material.name ? 1 : 0);
        gl.uniform1i(u.effectMode, effectMode(material.policy));
        gl.uniform1f(u.alphaScale, material.policy.alphaScale);
        gl.uniform1i(u.suppressDraw, suppress ? 1 : 0);
        gl.uniform1i(u.useUvTransform, 1);
        const auto* uv = overrideUv ? overrideUv : material.uv;
        gl.uniform4f(u.uvScaleOffset, uv[0], uv[1], uv[2], uv[3]);
        return check("bind explicit vehicle material blend and UV policy");
    }
    bool uploadRacer(std::uint8_t bank, const game::VehicleDrawBatch50CO& batch,
        U archetype = 0) noexcept {
        const auto selected = racerView(archetype);
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
        if (!s.drawSky() || !s.drawTrack(false)) return false;
        return true;
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
        if (!s.drawTrack(true)) return false;
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
        s.imageCommitted = true; s.active = false;
        return true;
    }
    static void abort(void* context, std::uint8_t) noexcept {
        // Previous private target remains untouched. Never blit or swap here.
        static_cast<Storage*>(context)->active = false;
    }
    bool copyToWindow() noexcept {
        gl.bindFramebuffer(readFramebuffer, targets[published]);
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
    bool copyRgba(std::uint8_t* destination) noexcept {
        if (!check("before committed RGBA readback")) return false;
        gl.bindFramebuffer(readFramebuffer, targets[published]);
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
        gl.readPixels(0, 0, committedWidth, committedHeight, rgba, unsignedByte, destination);
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
        committedWidth = committedHeight = 0;
        adapter.reset();
        if (!hasGlResources()) { shader.reset(); d.cleanupIncomplete = false; return true; }
        // All functions were loaded before the first name could be allocated.
        if (!drainCleanupErrors(d)) {
            d.cleanupIncomplete = true;
            return fail(d, VehicleWindowError50DS::cleanup, "GL error state will not clear for cleanup");
        }
        gl.useProgram(0); drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
        gl.bindFramebuffer(framebuffer, 0); gl.bindRenderbuffer(renderbuffer, 0);
        if (usesTextures()) {
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
            for (auto& index : resources.indices)
                if (!deleteName(index.buffer, gl.deleteBuffers, d)) goto incomplete;
            for (auto& archetype : racerArchetypes) {
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
                !deleteName(trackIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            if (!deleteName(skyVertexArray, gl.deleteVertexArrays, d) ||
                !deleteName(skyVertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(skyIndexBuffer, gl.deleteBuffers, d)) goto incomplete;
            for (U b = 0; b < 2; ++b)
                if (!deleteName(targets[b], gl.deleteFramebuffers, d) ||
                    !deleteName(colors[b], gl.deleteRenderbuffers, d) ||
                    !deleteName(depths[b], gl.deleteRenderbuffers, d)) goto incomplete;
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
            std::numeric_limits<std::size_t>::max() / 16)
        return fail(d, VehicleWindowError50DS::request,
            "context callbacks, valid output/vehicle shading modes and positive target bounds required");
    if (!assets.loaded()) return fail(d, VehicleWindowError50DS::assets, "50CR assets are not loaded");
    if (!validateTrack(request, d) || !validateMaterials(request, assets, d) ||
        !validateCables(request, d) || !validateSky(request, d)) return false;
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
    if (bool(s.skyIndexCount) != bool(frame.skyWorldViewProjection))
        return fail(d, VehicleWindowError50DS::frame, "sky matrix presence must match authored sky request");
    if (frame.skyWorldViewProjection)
        for (U i = 0; i < 16; ++i)
            if (!std::isfinite(frame.skyWorldViewProjection[i]))
                return fail(d, VehicleWindowError50DS::frame, "nonfinite authored sky matrix");
    if (!s.validateMaterialUvs(frame.materialUvs, frame.materialUvCount, d) ||
        !s.validateOpponents(batch, frame, d)) return false;
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
    if (frame.opponentCount) s.framePlayerBatch = batch;
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
    std::memcpy(s.matrix, frame.worldViewProjection, sizeof(s.matrix));
    std::memcpy(s.clearRgba, frame.clearRgba, sizeof(s.clearRgba));
    s.width = frame.width; s.height = frame.height;
    s.frameCables = frame.cables;
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
    for (U i = 0; i < s.frameOpponentCount; ++i) {
        s.frameOpponentBatches[i] = {};
        s.frameOpponentCables[i] = nullptr;
        s.frameOpponentArchetypes[i] = 0;
    }
    s.frameOpponentCount = 0;
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
