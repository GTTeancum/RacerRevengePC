#include "vehicle_window_gl50ds_storage.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <vector>

// Original projected pod shadows on PC (PLAN.md "Step 1 results" E):
//  caster pass   one 256x256 RGBA8 target per racer, cleared to 0 every frame,
//                1-texel border never written; every mesh of each listed
//                caster part drawn black (alpha 1) through the orthographic
//                light transform, z fixed at 0, no depth test or write.
//  receiver pass the listed static instances drawn again with the same world
//                transform as the world pass, texture projected from world P,
//                CLAMP/NEAREST, texel alpha < 0.5 discarded, output
//                (0,0,0,31/128) with SRC_ALPHA/ONE_MINUS_SRC_ALPHA
//                (dst * 97/128), depth LEQUAL, no depth write, alpha kept.
//                On receivers that take the 240C00 clipping path a triangle
//                is dropped when all three vertex normals face away from the
//                light (geometry shader).
// One path for every course: the receiver draw ranges come from whichever
// world publication this renderer owns, keyed by the same world instance.
namespace rr::platform {
namespace {
constexpr U shadowSize = game::projectedShadowTargetSize50PS;
// Caster slot -> batch component (chariot, left 1BC, right 1B8,
// bottom-left 1C4, bottom-right 1C0).
constexpr U shadowSlotComponent[5] = {0, 2, 1, 4, 3};

constexpr const char* shadowCasterVertex = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec3 rrSkinPosition50LA;
in float rrSkinBone50LA;
uniform int casterSkin;
uniform vec4 casterBones[192];
uniform mat4 casterClip;
void main() {
    vec3 world = rrPosition;
    if (casterSkin != 0) {
        int b = 3 * int(rrSkinBone50LA + 0.5);
        vec4 local = vec4(rrSkinPosition50LA, 1.0);
        world = vec3(dot(casterBones[b], local), dot(casterBones[b + 1], local), dot(casterBones[b + 2], local));
    }
    // 231DF0 orthographic light view; the original neither divides nor clips.
    vec4 p = casterClip * vec4(world, 1.0);
    gl_Position = vec4(p.xy, 0.0, 1.0);
}
)GLSL";
constexpr const char* shadowCasterFragment = R"GLSL(#version 150 core
out vec4 viewerOutput;
void main() { viewerOutput = vec4(0.0, 0.0, 0.0, 1.0); }
)GLSL";
constexpr const char* shadowReceiverVertex = R"GLSL(#version 150 core
in vec3 rrPosition;
in vec3 rrWorldNormal;
uniform mat4 worldViewProjection;
uniform mat4 shadowTexture;
uniform vec3 towardLight;
out vec2 vertexShadowUv;
out float vertexLightFacing;
void main() {
    // Same expression and matrix as the world pass for equal depth.
    gl_Position = vec4(rrPosition, 1.0) * worldViewProjection;
    vertexShadowUv = (shadowTexture * vec4(rrPosition, 1.0)).xy;
    // Zero/missing normals never drop a triangle.
    vertexLightFacing = dot(rrWorldNormal, rrWorldNormal) > 0.0 ? dot(rrWorldNormal, towardLight) : 1.0;
}
)GLSL";
constexpr const char* shadowReceiverGeometry = R"GLSL(#version 150 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec2 vertexShadowUv[];
in float vertexLightFacing[];
uniform int facingRule;
out vec2 shadowUv;
void main() {
    // ShadowReceiver clipping path (VU 0xE9..0x145): ADC only when all three
    // strip vertices face away from the light.
    if (facingRule != 0 && vertexLightFacing[0] < 0.0 && vertexLightFacing[1] < 0.0 &&
        vertexLightFacing[2] < 0.0) return;
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        shadowUv = vertexShadowUv[i];
        EmitVertex();
    }
    EndPrimitive();
}
)GLSL";
constexpr const char* shadowReceiverFragment = R"GLSL(#version 150 core
in vec2 shadowUv;
uniform sampler2D shadowImage;
uniform float shadowAlpha;
out vec4 viewerOutput;
void main() {
    // TEXA: covered texels alpha 32, empty 0; alpha test ref 1 drops the zeros.
    if (texture(shadowImage, shadowUv).a < 0.5) discard;
    viewerOutput = vec4(0.0, 0.0, 0.0, shadowAlpha);
}
)GLSL";
bool shadowTraceRequested() noexcept {
    const char* value = std::getenv("RR_SHADOW_TRACE");
    return value && value[0] == '1' && !value[1];
}
} // namespace

bool VehicleWindowGl50DS::Storage::hasShadowResources() const noexcept {
    for (const auto& t : shadowTargets) if (t.framebuffer || t.color) return true;
    for (U q : shadowQueries) if (q) return true;
    return shadowCasterProgram || shadowCasterVertexShader || shadowCasterFragmentShader ||
        shadowReceiverProgram || shadowReceiverVertexShader || shadowReceiverGeometryShader ||
        shadowReceiverFragmentShader || shadowCableVertexArray || shadowCableVertexBuffer ||
        shadowRaArray || shadowRaNormals || shadowCpuVertexArray || shadowCpuVertexBuffer || !shadowCasterLods.empty();
}

bool VehicleWindowGl50DS::Storage::buildShadowReceiverStatics() noexcept {
    shadowReceiverStatics.clear();
    const U count = request.staticInstanceCount;
    if (!count) return true;
    if (!request.staticShadowReceiving || !request.staticInstanceZone)
        return fail(*diagnostic, VehicleWindowError50DS::request, "static shadow receiver tables incomplete");
    if (count > 100000u)
        return fail(*diagnostic, VehicleWindowError50DS::limits, "static shadow receiver table outside bounds");
    struct Box { float minimum[3], maximum[3]; bool used = false; };
    std::vector<Box> boxes;
    try { boxes.resize(count); } catch (...) {
        return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate static shadow receiver boxes");
    }
    const auto add = [&](U instance, const float* p) noexcept {
        auto& b = boxes[instance];
        for (U a = 0; a < 3; ++a) {
            if (!b.used || p[a] < b.minimum[a]) b.minimum[a] = p[a];
            if (!b.used || p[a] > b.maximum[a]) b.maximum[a] = p[a];
        }
        b.used = true;
    };
    // Legacy track publication: spans carry their source world instance.
    if (request.track.mesh && request.track.spans) {
        const auto& vertices = request.track.mesh->vertices;
        for (U i = 0; i < request.track.spanCount; ++i) {
            const auto& span = request.track.spans[i];
            if (span.worldInstance == 0xffffffffu) continue;
            if (span.worldInstance >= count)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "track span world instance outside receiver table");
            if (span.vertexStart > vertices.size() || span.vertexCount > vertices.size() - span.vertexStart)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "track span vertex range outside mesh");
            for (U v = span.vertexStart; v < span.vertexStart + span.vertexCount; ++v)
                add(span.worldInstance, vertices[v].position);
        }
    }
    // RA publication: the retained surface plan carries the world instance.
    if (raWorld) {
        for (const auto& plan : raWorld->surfaceDrawPlan_) {
            if (plan.worldInstance >= count)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "RA surface world instance outside receiver table");
            if (plan.vertexStart > raWorld->vertices_.size() || plan.vertexCount > raWorld->vertices_.size() - plan.vertexStart)
                return fail(*diagnostic, VehicleWindowError50DS::assets, "RA surface vertex range outside world");
            for (std::size_t v = plan.vertexStart; v < std::size_t(plan.vertexStart) + plan.vertexCount; ++v)
                add(static_cast<U>(plan.worldInstance), raWorld->vertices_[v].worldPosition.data());
        }
    }
    U used = 0;
    for (const auto& b : boxes) used += b.used ? 1u : 0u;
    try { shadowReceiverStatics.reserve(used); } catch (...) {
        return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate static shadow receivers");
    }
    U receiving = 0;
    for (U i = 0; i < count; ++i) {
        if (!boxes[i].used) continue;
        game::ShadowReceiverStatic50PS s;
        s.instance = i;
        s.zone = request.staticInstanceZone[i];
        s.receives = request.staticShadowReceiving[i] != 0;
        float squared = 0.f;
        for (U a = 0; a < 3; ++a) {
            s.centre[a] = 0.5f * (boxes[i].minimum[a] + boxes[i].maximum[a]);
            s.halfSize[a] = 0.5f * (boxes[i].maximum[a] - boxes[i].minimum[a]);
            squared += s.halfSize[a] * s.halfSize[a];
        }
        // Stand-in: the original reads +198 (+194) and +130 from the model;
        // the port derives both from the instance's drawn geometry.
        s.radius = std::sqrt(squared);
        receiving += s.receives ? 1u : 0u;
        shadowReceiverStatics.push_back(s);
    }
    // Portals (WorldPortal48F order): zones are 1-based like the instances.
    shadowPortals.clear(); shadowZoneCount = 0;
    for (U i = 0; i < count; ++i)
        shadowZoneCount = std::max<U>(shadowZoneCount, request.staticInstanceZone[i]);
    if (request.staticPortalCount) {
        if (!request.staticPortalZones || !request.staticPortalCorners ||
            request.staticPortalCount > game::maxShadowPortals50PS)
            return fail(*diagnostic, VehicleWindowError50DS::request, "original shadow portal table incomplete");
        try { shadowPortals.resize(request.staticPortalCount); } catch (...) {
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original shadow portals");
        }
        for (U p = 0; p < request.staticPortalCount; ++p) {
            auto& portal = shadowPortals[p];
            for (U k = 0; k < 2; ++k) {
                portal.zone[k] = request.staticPortalZones[p * 2 + k];
                shadowZoneCount = std::max<U>(shadowZoneCount, portal.zone[k]);
            }
            for (U v = 0; v < 4; ++v) for (U a = 0; a < 3; ++a) {
                portal.corner[v][a] = request.staticPortalCorners[p * 12 + v * 3 + a];
                if (!std::isfinite(portal.corner[v][a]))
                    return fail(*diagnostic, VehicleWindowError50DS::request, "original shadow portal corner not finite");
            }
        }
    }
    if (shadowZoneCount > game::maxShadowZones50PS)
        return fail(*diagnostic, VehicleWindowError50DS::limits, "original shadow zone count outside bounds");
    std::fprintf(stderr, "original_shadow_receivers instances=%u with_geometry=%u receiving=%u portals=%zu zones=%u world=%s\n",
        count, used, receiving, shadowPortals.size(), shadowZoneCount, raWorld ? "ra_surface_plan" : "track_spans");
    return true;
}

bool VehicleWindowGl50DS::Storage::buildShadowCasterLods() noexcept {
    shadowCasterLods.clear();
    try { shadowCasterLods.resize(std::size_t(racerArchetypeCount) + 1u); } catch (...) {
        return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original caster LOD table");
    }
    std::size_t largest = 0;
    for (U archetype = 0; archetype <= racerArchetypeCount; ++archetype) {
        const auto selected = racerView(archetype);
        if (!selected.resources || !selected.meshes) continue;
        for (U c = 0; c < selected.resources->componentCount && c < maxComponents; ++c) {
            auto& out = shadowCasterLods[archetype][c];
            const auto* mesh = selected.meshes[c];
            const assets::Scene48D* scene = mesh && mesh->semanticScene ? mesh->semanticScene :
                (selected.scenes ? selected.scenes[c] : nullptr);
            if (!scene || scene->lods.empty() || scene->world_space || !scene->identity_matrix_palette ||
                scene->nodes.empty() || scene->nodes.size() > game::maxRigidPalette49S) continue;
            out.lodCount = static_cast<U>(scene->lods.size());
            out.nodeCount = static_cast<U>(scene->nodes.size());
            // 1B32E0 asks for LOD 2; 2316D0 stores min(2, +1B4 -> +10 - 1).
            out.lod = std::min<U>(game::projectedShadowBodyLod50PS, out.lodCount - 1u);
            try {
                for (const auto& surface : scene->lods[out.lod].surfaces) {
                    const U base = out.vertexCount;
                    if (surface.indices.size() % 3u) { out = {}; break; }
                    for (const auto& v : surface.vertices) {
                        if (v.bone >= out.nodeCount) { out = {}; break; }
                        for (U a = 0; a < 3; ++a) out.local.push_back(static_cast<float>(v.position_local[a]));
                        out.node.push_back(v.bone);
                        ++out.vertexCount;
                    }
                    if (!out.lodCount) break;
                    for (auto index : surface.indices) {
                        if (index >= surface.vertices.size()) { out = {}; break; }
                        out.indices.push_back(base + index);
                    }
                    if (!out.lodCount) break;
                }
            } catch (...) { return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original caster LOD"); }
            if (!out.lodCount || out.indices.empty()) { out = {}; continue; }
            out.indexCount = static_cast<I>(out.indices.size());
            // Static GPU stream in the skin attribute slots: node-local position + node.
            std::vector<float> packed;
            try { packed.resize(std::size_t(out.vertexCount) * 4u); } catch (...) {
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original caster LOD stream");
            }
            for (U v = 0; v < out.vertexCount; ++v) {
                for (U a = 0; a < 3; ++a) packed[v * 4 + a] = out.local[v * 3 + a];
                packed[v * 4 + 3] = static_cast<float>(out.node[v]);
            }
            gl.genVertexArrays(1, &out.vertexArray); drawGl.bindVertexArray(out.vertexArray);
            gl.genBuffers(1, &out.vertexBuffer); drawGl.bindBuffer(arrayBuffer, out.vertexBuffer);
            gl.bufferData(arrayBuffer, std::intptr_t(packed.size() * sizeof(float)), packed.data(), staticDraw);
            gl.enableVertexAttribArray(vehicleGlSkinPositionAttribute50LA);
            gl.vertexAttribPointer(vehicleGlSkinPositionAttribute50LA, 3, floatType, 0, 4 * sizeof(float), nullptr);
            gl.enableVertexAttribArray(vehicleGlSkinBoneAttribute50LA);
            gl.vertexAttribPointer(vehicleGlSkinBoneAttribute50LA, 1, floatType, 0, 4 * sizeof(float),
                reinterpret_cast<const void*>(3 * sizeof(float)));
            gl.genBuffers(1, &out.indexBuffer); drawGl.bindBuffer(elementBuffer, out.indexBuffer);
            gl.bufferData(elementBuffer, std::intptr_t(out.indices.size() * sizeof(std::uint32_t)), out.indices.data(), staticDraw);
            if (!check("create original caster LOD stream") || !out.vertexArray || !out.vertexBuffer || !out.indexBuffer)
                return fail(*diagnostic, VehicleWindowError50DS::allocation, "original caster LOD stream");
            out.ready = true;
            largest = std::max(largest, out.indices.size());
            std::fprintf(stderr, "original_shadow_caster_lod archetype=%u component=%u lod=%u lods=%u triangles=%zu\n",
                archetype, c, out.lod, out.lodCount, out.indices.size() / 3u);
        }
    }
    drawGl.bindVertexArray(0);
    // CPU-posed stream (non-indexed) for parts without a GPU palette.
    try { shadowCpuVertices.assign(largest * 3u, 0.f); } catch (...) {
        return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original CPU caster stream");
    }
    if (largest) {
        gl.genVertexArrays(1, &shadowCpuVertexArray); drawGl.bindVertexArray(shadowCpuVertexArray);
        gl.genBuffers(1, &shadowCpuVertexBuffer); drawGl.bindBuffer(arrayBuffer, shadowCpuVertexBuffer);
        gl.bufferData(arrayBuffer, std::intptr_t(shadowCpuVertices.size() * sizeof(float)), nullptr, dynamicDraw);
        gl.enableVertexAttribArray(vehicleGlPositionAttribute50CZ);
        gl.vertexAttribPointer(vehicleGlPositionAttribute50CZ, 3, floatType, 0, 3 * sizeof(float), nullptr);
        drawGl.bindVertexArray(0);
        if (!check("create original CPU caster stream") || !shadowCpuVertexArray || !shadowCpuVertexBuffer)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "original CPU caster stream");
    }
    return true;
}

// Least squares per palette entry over the part's displayed vertices:
// world = (local, 1) * A (4x3). Exact for the rigid palettes the port uses.
bool VehicleWindowGl50DS::Storage::solveShadowPalette(const game::VehicleDrawSubmission50CM& part,
    const game::VehicleDrawMesh50CM& mesh, U nodeCount) noexcept {
    if (!part.vertices || !mesh.localVertices || part.vertexCount != mesh.vertexCount ||
        !nodeCount || nodeCount > game::maxRigidPalette49S) return false;
    for (U b = 0; b < nodeCount; ++b) { shadowSolveNormal[b].fill(0.0); shadowSolveRight[b].fill(0.0); shadowSolveCount[b] = 0; }
    for (U v = 0; v < mesh.vertexCount; ++v) {
        const auto& local = mesh.localVertices[v];
        if (local.paletteIndex >= nodeCount) return false;
        const double x[4] = {local.positionLocal[0], local.positionLocal[1], local.positionLocal[2], 1.0};
        auto& n = shadowSolveNormal[local.paletteIndex];
        auto& r = shadowSolveRight[local.paletteIndex];
        for (U i = 0; i < 4; ++i) {
            for (U j = 0; j < 4; ++j) n[i * 4 + j] += x[i] * x[j];
            for (U j = 0; j < 3; ++j) r[i * 3 + j] += x[i] * double(part.vertices[v].position[j]);
        }
        ++shadowSolveCount[local.paletteIndex];
    }
    for (U b = 0; b < nodeCount; ++b) {
        auto& out = shadowSolvedPalette[b];
        if (!shadowSolveCount[b]) { out.fill(0.f); continue; } // node with no vertices: unused by LOD 0
        if (shadowSolveCount[b] < 4) return false;
        double a[4][7];
        for (U i = 0; i < 4; ++i) {
            for (U j = 0; j < 4; ++j) a[i][j] = shadowSolveNormal[b][i * 4 + j];
            for (U j = 0; j < 3; ++j) a[i][4 + j] = shadowSolveRight[b][i * 3 + j];
        }
        for (U col = 0; col < 4; ++col) {
            U pivot = col;
            for (U row = col + 1; row < 4; ++row) if (std::fabs(a[row][col]) > std::fabs(a[pivot][col])) pivot = row;
            if (!(std::fabs(a[pivot][col]) > 1e-9)) return false;
            if (pivot != col) for (U j = 0; j < 7; ++j) std::swap(a[col][j], a[pivot][j]);
            for (U row = 0; row < 4; ++row) {
                if (row == col) continue;
                const double f = a[row][col] / a[col][col];
                for (U j = col; j < 7; ++j) a[row][j] -= f * a[col][j];
            }
        }
        for (U i = 0; i < 4; ++i) for (U j = 0; j < 3; ++j) {
            out[i * 3 + j] = static_cast<float>(a[i][4 + j] / a[i][i]);
            if (!std::isfinite(out[i * 3 + j])) return false;
        }
    }
    return true;
}

bool VehicleWindowGl50DS::Storage::drawShadowCasterLod(std::uint8_t bank, const RacerView& selected, U archetype,
    U component, const game::VehicleDrawSubmission50CM& part, U lod) noexcept {
    const ShadowCasterLod50PS* casterLod = archetype < shadowCasterLods.size() && component < maxComponents ?
        &shadowCasterLods[archetype][component] : nullptr;
    const bool lodReady = casterLod && casterLod->ready && casterLod->lod == lod;
    if (lodReady && part.skin && part.skin->boneCount >= casterLod->nodeCount &&
        part.skin->boneCount * 3u <= vehicleGlSkinBoneRows50LA) {
        // GPU palette of this frame (same rows as bindRacerSkin50LA).
        const auto& skin = *part.skin;
        float rows[vehicleGlSkinBoneRows50LA * 4];
        for (U bone = 0; bone < skin.boneCount; ++bone) {
            const auto& m = skin.bones[bone].v;
            for (U axis = 0; axis < 3; ++axis) {
                float* row = rows + (bone * 3 + axis) * 4;
                row[0] = m[axis]; row[1] = m[4 + axis]; row[2] = m[8 + axis]; row[3] = m[12 + axis];
            }
        }
        gl.uniform4fv(shadowCasterBonesUniform, I(skin.boneCount * 3u), rows);
        gl.uniform1i(shadowCasterSkinUniform, 1);
        drawGl.bindVertexArray(casterLod->vertexArray);
        drawGl.bindBuffer(elementBuffer, casterLod->indexBuffer);
        drawGl.drawElementsBaseVertex(0x0004, casterLod->indexCount, 0x1405, nullptr, 0);
        ++shadowLodGpuParts;
        return check("draw original caster LOD (GPU palette)");
    }
    if (lodReady && !part.skin && selected.meshes[component] &&
        solveShadowPalette(part, *selected.meshes[component], casterLod->nodeCount) &&
        std::size_t(casterLod->indexCount) * 3u <= shadowCpuVertices.size()) {
        for (I k = 0; k < casterLod->indexCount; ++k) {
            const U v = casterLod->indices[std::size_t(k)];
            const float* p = &casterLod->local[std::size_t(v) * 3u];
            const auto& m = shadowSolvedPalette[casterLod->node[v]];
            for (U a = 0; a < 3; ++a)
                shadowCpuVertices[std::size_t(k) * 3u + a] = p[0] * m[a] + p[1] * m[3 + a] + p[2] * m[6 + a] + m[9 + a];
        }
        drawGl.bindVertexArray(shadowCpuVertexArray);
        drawGl.bindBuffer(arrayBuffer, shadowCpuVertexBuffer);
        drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(std::size_t(casterLod->indexCount) * 3u * sizeof(float)),
            shadowCpuVertices.data());
        gl.uniform1i(shadowCasterSkinUniform, 0);
        gl.drawArrays(0x0004, 0, casterLod->indexCount);
        ++shadowLodCpuParts;
        return check("draw original caster LOD (CPU palette)");
    }
    // Fallback (no caster LOD for this part, or palette not recoverable):
    // the displayed mesh, counted in the log. The caster pass runs after all
    // racers' bodies (shared inactive VBOs), so a CPU part re-uploads its
    // own current vertices first.
    if (!part.skin && part.vertices && part.vertexCount) {
        drawGl.bindBuffer(arrayBuffer, selected.resources->bank[bank].vertexBuffer[component]);
        drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(part.vertexCount) * sizeof(assets::Vertex48B), part.vertices);
        if (!check("restore caster fallback part vertices")) return false;
    }
    drawGl.bindVertexArray(selected.resources->bank[bank].vertexArray[component]);
    drawGl.bindBuffer(elementBuffer, selected.resources->indices[component].buffer);
    if (part.skin && part.skin->boneCount && part.skin->boneCount * 3u <= vehicleGlSkinBoneRows50LA) {
        const auto& skin = *part.skin;
        float rows[vehicleGlSkinBoneRows50LA * 4];
        for (U bone = 0; bone < skin.boneCount; ++bone) {
            const auto& m = skin.bones[bone].v;
            for (U axis = 0; axis < 3; ++axis) {
                float* row = rows + (bone * 3 + axis) * 4;
                row[0] = m[axis]; row[1] = m[4 + axis]; row[2] = m[8 + axis]; row[3] = m[12 + axis];
            }
        }
        gl.uniform4fv(shadowCasterBonesUniform, I(skin.boneCount * 3u), rows);
        gl.uniform1i(shadowCasterSkinUniform, 1);
    } else gl.uniform1i(shadowCasterSkinUniform, 0);
    drawGl.drawElementsBaseVertex(0x0004, I(selected.meshes[component]->indexCount), 0x1405, nullptr, 0);
    ++shadowLodFallbackParts;
    return check("draw original caster part (displayed LOD fallback)");
}

bool VehicleWindowGl50DS::Storage::createOriginalShadows() noexcept {
    // Only a gameplay world renderer owns shadows; frontend/vehicle-only skip.
    if (frontendOnly || (!trackSpanCount && !raWorld)) return true;
    shadowTrace = shadowTraceRequested();
    if (!buildShadowReceiverStatics()) return false;
    if (!loadDisplayFadeFunctions()) return false; // texture/FBO/drawArrays entry points
    if (!buildShadowCasterLods()) return false;
    // Targets: 8 x 256x256 RGBA8 = 2 MiB. Colour only: the caster pass has no
    // depth test or write (Z format 50 is cleared to 0 and never compared).
    for (U i = 0; i < game::maxProjectedShadows50PS; ++i) {
        auto& t = shadowTargets[i];
        gl.genFramebuffers(1, &t.framebuffer);
        gl.genTextures(1, &t.color);
        if (!check("allocate original shadow target") || !t.framebuffer || !t.color)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "zero original shadow target");
        gl.activeTexture(textureUnit0); gl.bindTexture(texture2D, t.color);
        if (gl.bindSampler) gl.bindSampler(0, 0);
        gl.texParameteri(texture2D, 0x2801, 0x2600); gl.texParameteri(texture2D, 0x2800, 0x2600); // NEAREST
        gl.texParameteri(texture2D, 0x2802, 0x812F); gl.texParameteri(texture2D, 0x2803, 0x812F); // CLAMP_TO_EDGE
        gl.texParameteri(texture2D, 0x813C, 0); gl.texParameteri(texture2D, 0x813D, 0);
        drawGl.bindBuffer(0x88EC, 0); // PIXEL_UNPACK_BUFFER
        gl.texImage2D(texture2D, 0, rgba8, I(shadowSize), I(shadowSize), 0, rgba, unsignedByte, nullptr);
        gl.bindFramebuffer(framebuffer, t.framebuffer);
        gl.framebufferTexture2D(framebuffer, colorAttachment, texture2D, t.color, 0);
        gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
        if (!check("attach original shadow target")) return false;
        if (gl.checkFramebufferStatus(framebuffer) != framebufferComplete)
            return fail(*diagnostic, VehicleWindowError50DS::framebuffer, "original shadow target incomplete");
        gl.disable(0x0C11); gl.viewport(0, 0, I(shadowSize), I(shadowSize));
        gl.clearColor(0.f, 0.f, 0.f, 0.f); gl.clear(colorBufferBit);
        if (!check("clear original shadow target")) return false;
    }
    gl.bindTexture(texture2D, 0);
    gl.bindFramebuffer(framebuffer, 0);
    const auto link = [&](U& program, U vertex, U geometry, U fragment, bool caster, const char* what) noexcept {
        program = shaderGl.createProgram();
        if (!check(what) || !program) return fail(*diagnostic, VehicleWindowError50DS::shader, what);
        shaderGl.attachShader(program, vertex);
        if (geometry) shaderGl.attachShader(program, geometry);
        shaderGl.attachShader(program, fragment);
        shaderGl.bindAttribLocation(program, vehicleGlPositionAttribute50CZ, "rrPosition");
        if (caster) {
            shaderGl.bindAttribLocation(program, vehicleGlSkinPositionAttribute50LA, "rrSkinPosition50LA");
            shaderGl.bindAttribLocation(program, vehicleGlSkinBoneAttribute50LA, "rrSkinBone50LA");
        } else shaderGl.bindAttribLocation(program, 3, "rrWorldNormal"); // track/RA normal stream
        shaderGl.bindFragDataLocation(program, vehicleGlColorOutput50CZ, "viewerOutput");
        shaderGl.linkProgram(program);
        I linked = 0; shaderGl.getProgramiv(program, 0x8B82, &linked);
        if (!check(what)) return false;
        if (!linked) {
            shaderGl.getProgramInfoLog(program, static_cast<I>(sizeof(diagnostic->shader.log)), nullptr, diagnostic->shader.log);
            diagnostic->shader.log[sizeof(diagnostic->shader.log) - 1] = '\0';
            check("read original shadow program log");
            return fail(*diagnostic, VehicleWindowError50DS::shader, what);
        }
        return true;
    };
    diagnostic->shader.stage = VehicleGlShaderStage50CZ::vertex;
    if (!compileTrackShader(0x8B31, shadowCasterVertex, shadowCasterVertexShader) ||
        !compileTrackShader(0x8B31, shadowReceiverVertex, shadowReceiverVertexShader)) return false;
    diagnostic->shader.stage = VehicleGlShaderStage50CZ::fragment;
    if (!compileTrackShader(0x8B30, shadowCasterFragment, shadowCasterFragmentShader) ||
        !compileTrackShader(0x8B30, shadowReceiverFragment, shadowReceiverFragmentShader)) return false;
    if (!compileTrackShader(0x8DD9, shadowReceiverGeometry, shadowReceiverGeometryShader)) return false;
    diagnostic->shader.stage = VehicleGlShaderStage50CZ::link;
    if (!link(shadowCasterProgram, shadowCasterVertexShader, 0, shadowCasterFragmentShader, true, "link original shadow caster program") ||
        !link(shadowReceiverProgram, shadowReceiverVertexShader, shadowReceiverGeometryShader, shadowReceiverFragmentShader, false,
            "link original shadow receiver program")) return false;
    shadowCasterClipUniform = shaderGl.getUniformLocation(shadowCasterProgram, "casterClip");
    shadowCasterSkinUniform = shaderGl.getUniformLocation(shadowCasterProgram, "casterSkin");
    shadowCasterBonesUniform = shaderGl.getUniformLocation(shadowCasterProgram, "casterBones");
    shadowReceiverMatrixUniform = shaderGl.getUniformLocation(shadowReceiverProgram, "worldViewProjection");
    shadowReceiverTextureUniform = shaderGl.getUniformLocation(shadowReceiverProgram, "shadowTexture");
    shadowReceiverImageUniform = shaderGl.getUniformLocation(shadowReceiverProgram, "shadowImage");
    shadowReceiverLightUniform = shaderGl.getUniformLocation(shadowReceiverProgram, "towardLight");
    shadowReceiverFacingUniform = shaderGl.getUniformLocation(shadowReceiverProgram, "facingRule");
    shadowReceiverAlphaUniform = shaderGl.getUniformLocation(shadowReceiverProgram, "shadowAlpha");
    if (!check("locate original shadow uniforms")) return false;
    if (shadowCasterClipUniform < 0 || shadowCasterSkinUniform < 0 || shadowCasterBonesUniform < 0 ||
        shadowReceiverMatrixUniform < 0 || shadowReceiverTextureUniform < 0 || shadowReceiverImageUniform < 0 ||
        shadowReceiverLightUniform < 0 || shadowReceiverFacingUniform < 0 || shadowReceiverAlphaUniform < 0)
        return fail(*diagnostic, VehicleWindowError50DS::shader, "original shadow uniform missing");
    diagnostic->shader.stage = VehicleGlShaderStage50CZ::none;
    // Cable-shadow casters: one small dynamic world-space triangle stream.
    gl.genVertexArrays(1, &shadowCableVertexArray); drawGl.bindVertexArray(shadowCableVertexArray);
    gl.genBuffers(1, &shadowCableVertexBuffer); drawGl.bindBuffer(arrayBuffer, shadowCableVertexBuffer);
    gl.bufferData(arrayBuffer, std::intptr_t(sizeof(float) * 9u * game::maxCableShadowTriangles50PS), nullptr, dynamicDraw);
    gl.enableVertexAttribArray(vehicleGlPositionAttribute50CZ);
    gl.vertexAttribPointer(vehicleGlPositionAttribute50CZ, 3, floatType, 0, 3 * sizeof(float), nullptr);
    if (!check("create original cable-shadow caster stream") || !shadowCableVertexArray || !shadowCableVertexBuffer)
        return fail(*diagnostic, VehicleWindowError50DS::allocation, "original cable-shadow caster stream");
    // RA receivers: retained world positions plus world normals (placement
    // rotation of each source normal) for the light-facing rule.
    if (raWorld && raWorldArray && raWorldVertices && raWorldIndices) {
        std::vector<float> normals;
        try { normals.assign(raWorld->vertices_.size() * 3u, 0.f); } catch (...) {
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate RA receiver normals");
        }
        for (const auto& plan : raWorld->surfaceDrawPlan_) {
            if (plan.placement >= raWorld->placements_.size() || !raWorld->placements_[plan.placement])
                return fail(*diagnostic, VehicleWindowError50DS::assets, "RA receiver placement missing");
            const auto& m = raWorld->placements_[plan.placement]->visualMatrix;
            for (std::size_t v = plan.vertexStart; v < std::size_t(plan.vertexStart) + plan.vertexCount &&
                v < raWorld->vertices_.size(); ++v) {
                const auto& n = raWorld->vertices_[v].sourceNormal;
                double w[3];
                for (U a = 0; a < 3; ++a) w[a] = n[0] * m[a] + n[1] * m[4 + a] + n[2] * m[8 + a];
                const double length = std::sqrt(w[0] * w[0] + w[1] * w[1] + w[2] * w[2]);
                for (U a = 0; a < 3; ++a)
                    normals[v * 3 + a] = length > 0.0 && std::isfinite(length) ? static_cast<float>(w[a] / length) : 0.f;
            }
        }
        gl.genBuffers(1, &shadowRaNormals); drawGl.bindBuffer(arrayBuffer, shadowRaNormals);
        gl.bufferData(arrayBuffer, std::intptr_t(normals.size() * sizeof(float)), normals.data(), staticDraw);
        gl.genVertexArrays(1, &shadowRaArray); drawGl.bindVertexArray(shadowRaArray);
        drawGl.bindBuffer(arrayBuffer, raWorldVertices);
        gl.enableVertexAttribArray(0);
        gl.vertexAttribPointer(0, 3, floatType, 0, sizeof(RaWorldVertex), reinterpret_cast<const void*>(offsetof(RaWorldVertex, position)));
        drawGl.bindBuffer(arrayBuffer, shadowRaNormals);
        gl.enableVertexAttribArray(3);
        gl.vertexAttribPointer(3, 3, floatType, 0, 3 * sizeof(float), nullptr);
        drawGl.bindBuffer(elementBuffer, raWorldIndices);
        if (!check("create RA original shadow receiver stream") || !shadowRaArray || !shadowRaNormals)
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "RA original shadow receiver stream");
    }
    drawGl.bindVertexArray(0); drawGl.bindBuffer(arrayBuffer, 0);
    if (shadowTrace) {
        const auto proc = [&](auto& pointer, const char* name) noexcept {
            pointer = reinterpret_cast<std::remove_reference_t<decltype(pointer)>>(request.procAddress(request.context, name));
            return pointer != nullptr;
        };
        if (proc(gl.genQueries, "glGenQueries") && proc(gl.deleteQueries, "glDeleteQueries") &&
            proc(gl.beginQuery, "glBeginQuery") && proc(gl.endQuery, "glEndQuery") &&
            proc(gl.getQueryObjectuiv, "glGetQueryObjectuiv")) {
            gl.genQueries(I(game::maxProjectedShadows50PS), shadowQueries.data());
            if (!check("allocate original shadow occlusion queries")) return false;
        } else std::fprintf(stderr, "original_shadow_trace queries=unavailable\n");
        try { shadowReadback.resize(std::size_t(shadowSize) * shadowSize * 4u); } catch (...) {
            return fail(*diagnostic, VehicleWindowError50DS::allocation, "allocate original shadow readback");
        }
    }
    shadowResourcesCreated = true;
    std::fprintf(stderr, "original_shadows=1 targets=%u size=%u gpu_bytes=%u receivers=%zu trace=%u\n",
        game::maxProjectedShadows50PS, shadowSize, game::maxProjectedShadows50PS * shadowSize * shadowSize * 4u,
        shadowReceiverStatics.size(), shadowTrace ? 1u : 0u);
    return check("finish original shadow resources");
}

bool VehicleWindowGl50DS::Storage::prepareOriginalShadowFrame(const VehicleWindowFrame50DS& frame,
    VehicleWindowDiagnostic50DS& d) noexcept {
    frameShadowsSet = false; frameShadowCount = 0;
    frameShadowCasterParts.fill(0); frameShadowReceiverDraws.fill(0);
    frameShadowDrawn.fill(false); frameShadowHidden.fill(false);
    frameShadowCover.fill(-1); frameShadowSamples.fill(-1);
    if (!frame.originalShadows) return true;
    if (!shadowResourcesCreated) {
        // A renderer without a static world (frontend, vehicle-only) draws none.
        static bool reported = false;
        if (!reported) { reported = true; std::fprintf(stderr, "original_shadows=0 reason=no_world_publication\n"); }
        return true;
    }
    const auto& in = *frame.originalShadows;
    if (in.shadowCount > game::maxProjectedShadows50PS || in.shadowCount > 1u + frameOpponentCount)
        return fail(d, VehicleWindowError50DS::frame, "original shadow count exceeds submitted racers");
    for (U i = 0; i < in.shadowCount; ++i) {
        const auto& s = in.shadows[i];
        if (s.racer != i || s.casterCount > game::shadowCasterSlotCount50PS ||
            s.cableTriangles > game::maxCableShadowTriangles50PS)
            return fail(d, VehicleWindowError50DS::frame, "original shadow caster list outside capacity");
        for (U c = 0; c < s.casterCount; ++c)
            if (s.casters[c].slot >= game::shadowCasterSlotCount50PS)
                return fail(d, VehicleWindowError50DS::frame, "original shadow caster slot outside capacity");
        if (!s.hasCasters()) continue;
        for (float v : s.casterClip) if (!std::isfinite(v)) return fail(d, VehicleWindowError50DS::frame, "original shadow caster transform");
        for (float v : s.texture) if (!std::isfinite(v)) return fail(d, VehicleWindowError50DS::frame, "original shadow texture transform");
        for (U v = 0; v < s.cableTriangles * 9u; ++v)
            if (!std::isfinite(s.cableVertices[v])) return fail(d, VehicleWindowError50DS::frame, "original cable-shadow vertex");
    }
    frameShadows = in;
    frameShadowCount = in.shadowCount;
    game::ShadowReceiverView50PS view;
    view.cameraZone = in.cameraZone;
    for (U a = 0; a < 3; ++a) view.cameraEye[a] = in.cameraEye[a];
    view.worldViewProjection = matrix; // already copied from this frame
    for (U i = 0; i < frameShadowCount; ++i) {
        auto& list = frameShadowReceivers[i];
        // A racer whose models the renderer hides this frame (28C6E0) has no
        // geometry to cast from; draw no receivers for it.
        frameShadowHidden[i] = i ? frameOpponentHidden[i - 1] : framePlayerHidden;
        if (frameShadowHidden[i]) { list = {}; continue; }
        game::ShadowReceiverWorld50PS world;
        world.statics = shadowReceiverStatics.data(); world.staticCount = static_cast<U>(shadowReceiverStatics.size());
        world.portals = shadowPortals.data(); world.portalCount = static_cast<U>(shadowPortals.size());
        world.zoneCount = shadowZoneCount;
        if (!game::gatherShadowReceivers50PS(frameShadows.shadows[i], world, view, list))
            return fail(d, VehicleWindowError50DS::frame, "original shadow receiver gather rejected");
        if (list.count > game::maxShadowReceivers50PS)
            return fail(d, VehicleWindowError50DS::frame, "original shadow receiver list outside capacity");
        auto& keys = frameShadowKeys[i];
        for (U k = 0; k < list.count; ++k)
            keys[k] = (std::uint64_t(list.instance[k]) << 1) | (list.facingRule[k] ? 1u : 0u);
        std::sort(keys.begin(), keys.begin() + list.count);
    }
    shadowTraceFrame = shadowTrace && (shadowFrames % 60u) == 0u;
    frameShadowsSet = true;
    return true;
}

int VehicleWindowGl50DS::Storage::shadowReceiverRule(U shadow, U instance) const noexcept {
    if (shadow >= frameShadowCount) return -1;
    const auto& list = frameShadowReceivers[shadow];
    const auto& keys = frameShadowKeys[shadow];
    const std::uint64_t key = std::uint64_t(instance) << 1;
    const auto found = std::lower_bound(keys.begin(), keys.begin() + list.count, key);
    if (found == keys.begin() + list.count || (*found >> 1) != instance) return -1;
    return int(*found & 1u);
}

bool VehicleWindowGl50DS::Storage::drawShadowCasters(std::uint8_t bank, U slot) noexcept {
    if (!frameShadowsSet || slot >= frameShadowCount || frameShadowHidden[slot]) return true;
    const auto& shadow = frameShadows.shadows[slot];
    if (!shadow.hasCasters() || !frameShadowReceivers[slot].count) return true;
    const auto& batch = slot ? frameOpponentBatches[slot - 1] : framePlayerBatch;
    const U archetype = slot ? frameOpponentArchetypes[slot - 1] : 0u;
    const auto selected = racerView(archetype);
    const auto& target = shadowTargets[slot];
    gl.bindFramebuffer(framebuffer, target.framebuffer);
    gl.viewport(0, 0, I(shadowSize), I(shadowSize));
    gl.disable(0x0C11); gl.colorMask(1, 1, 1, 1);
    gl.clearColor(0.f, 0.f, 0.f, 0.f); gl.clear(colorBufferBit);
    // preserveBorderForClamp: the outer texel ring stays clear.
    gl.enable(0x0C11); gl.scissor(1, 1, I(shadowSize) - 2, I(shadowSize) - 2);
    gl.disable(0x0B71); gl.depthMask(0); gl.disable(0x0BE2); gl.disable(0x0B44);
    gl.useProgram(shadowCasterProgram);
    gl.uniformMatrix4fv(shadowCasterClipUniform, 1, 0, shadow.casterClip);
    bool accepted = check("bind original shadow caster target");
    U parts = 0;
    for (U c = 0; accepted && c < shadow.casterCount; ++c) {
        const U casterSlot = shadow.casters[c].slot;
        if (casterSlot >= game::firstCableShadowSlot50PS) continue; // cable meshes below
        const U component = shadowSlotComponent[casterSlot];
        if (component >= selected.resources->componentCount || component >= batch.componentCount) continue;
        const auto& part = batch.component[component];
        // ProjectedShadow material: every mesh of the model at the caster LOD
        // (2316D0), regardless of its own material, as one black silhouette.
        accepted = drawShadowCasterLod(bank, selected, archetype, component, part, shadow.casters[c].lod);
        ++parts;
    }
    if (accepted && shadow.cableTriangles) {
        drawGl.bindVertexArray(shadowCableVertexArray);
        drawGl.bindBuffer(arrayBuffer, shadowCableVertexBuffer);
        drawGl.bufferSubData(arrayBuffer, 0, std::intptr_t(shadow.cableTriangles * 9u * sizeof(float)), shadow.cableVertices);
        gl.uniform1i(shadowCasterSkinUniform, 0);
        gl.drawArrays(0x0004, 0, I(shadow.cableTriangles * 3u));
        accepted = check("draw original cable-shadow casters");
        parts += 4;
    }
    frameShadowCasterParts[slot] = parts;
    frameShadowDrawn[slot] = accepted;
    if (accepted && shadowTraceFrame && !shadowReadback.empty()) {
        drawGl.bindBuffer(pixelPackBuffer, 0);
        gl.pixelStorei(0x0D05, 1);
        gl.readPixels(0, 0, I(shadowSize), I(shadowSize), rgba, unsignedByte, shadowReadback.data());
        std::int64_t covered = 0;
        for (std::size_t p = 3; p < shadowReadback.size(); p += 4) covered += shadowReadback[p] ? 1 : 0;
        frameShadowCover[slot] = covered;
        accepted = check("read back original shadow coverage");
    }
    // Back to the scene target of this transaction.
    gl.bindFramebuffer(framebuffer, coverageDemand ? coverageLive.scene[bank] : targets[bank]);
    gl.drawBuffer(colorAttachment); gl.readBuffer(colorAttachment);
    gl.viewport(0, 0, width, height);
    gl.disable(0x0C11); gl.enable(0x0B71); gl.depthMask(1); gl.depthFunc(0x0201);
    resetPodState();
    return check("restore scene target after original shadow casters") && accepted;
}

bool VehicleWindowGl50DS::Storage::drawShadowReceivers(std::uint8_t bank, U slot) noexcept {
    // One racer's receiver pass, right after its own caster (shadowCasterN then
    // shadowReceiverN in Mat_Stbl order; see commit()). Slot bounds-checked here.
    if (!frameShadowsSet || slot >= frameShadowCount || !frameShadowDrawn[slot] || !frameShadowReceivers[slot].count)
        return true;
    gl.bindFramebuffer(framebuffer, coverageDemand ? coverageLive.scene[bank] : targets[bank]);
    gl.drawBuffer(colorAttachment);
    gl.viewport(0, 0, width, height);
    coverageAa0();
    gl.useProgram(shadowReceiverProgram);
    gl.uniformMatrix4fv(shadowReceiverMatrixUniform, 1, 0, matrix);
    gl.uniform1i(shadowReceiverImageUniform, 0);
    gl.uniform1f(shadowReceiverAlphaUniform, game::projectedShadowSourceAlpha50PS);
    gl.enable(0x0BE2); gl.blendEquation(0x8006); gl.blendFunc(0x0302, 0x0303);
    gl.enable(0x0B71); gl.depthFunc(0x0203); gl.depthMask(0);
    gl.colorMask(1, 1, 1, 0); // the receiver changes RGB only
    gl.disable(0x0B44); gl.disable(0x0C11);
    gl.activeTexture(textureUnit0); if (gl.bindSampler) gl.bindSampler(0, 0);
    bool accepted = check("bind original shadow receiver state");
    for (U i = slot; accepted && i == slot; ++i) {
        const auto& shadow = frameShadows.shadows[i];
        gl.bindTexture(texture2D, shadowTargets[i].color);
        gl.uniformMatrix4fv(shadowReceiverTextureUniform, 1, 0, shadow.texture);
        // VU 47: towards the light.
        gl.uniform3fv(shadowReceiverLightUniform, 1, std::array<float, 3>{-shadow.light[0], -shadow.light[1], -shadow.light[2]}.data());
        const bool query = shadowTraceFrame && shadowQueries[i] && gl.beginQuery;
        if (query) gl.beginQuery(0x8914, shadowQueries[i]); // SAMPLES_PASSED
        int boundRule = -1;
        U draws = 0;
        // Legacy track spans: this frame's visible draws, first source pass only
        // (every mesh of the model once, as 2DED30 submits it).
        if (trackSpanCount && trackVertexArray) {
            drawGl.bindVertexArray(trackVertexArray);
            drawGl.bindBuffer(elementBuffer, trackIndexBuffer);
            for (U n = 0; accepted && n < frameTrackDrawCount; ++n) {
                const U draw = frameTrackCullingApplied ? trackVisibleDrawIds[n] : n;
                const U spanIndex = trackChunkCount ? trackChunks[draw].sourceSpan : draw;
                if (spanIndex >= trackSpanCount) { accepted = fail(*diagnostic, VehicleWindowError50DS::frame, "receiver span outside track"); break; }
                const auto& span = trackSpans[spanIndex];
                if (span.sourcePass != 1 || span.worldInstance == 0xffffffffu) continue;
                const int rule = shadowReceiverRule(i, span.worldInstance);
                if (rule < 0) continue;
                if (rule != boundRule) { gl.uniform1i(shadowReceiverFacingUniform, rule); boundRule = rule; }
                const U indexStart = trackChunkCount ? trackChunks[draw].indexStart : span.indexStart;
                const U indexCount = trackChunkCount ? trackChunks[draw].indexCount : span.indexCount;
                drawGl.drawElementsBaseVertex(0x0004, I(indexCount), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(indexStart) * sizeof(U)), 0);
                accepted = check("draw original shadow receiver track range");
                ++draws;
            }
        }
        // RA surface plan: this frame's selected model surfaces (current LOD),
        // with the same scissor the world pass used.
        if (accepted && raWorld && shadowRaArray && raWorldFramePrepared) {
            drawGl.bindVertexArray(shadowRaArray);
            drawGl.bindBuffer(elementBuffer, raWorldIndices);
            for (const auto& selected : raWorldFrameDraws) {
                if (!accepted) break;
                if (selected.localFlare) continue;
                if (selected.surface >= raWorld->surfaceDrawPlan_.size()) {
                    accepted = fail(*diagnostic, VehicleWindowError50DS::frame, "receiver surface outside RA plan"); break;
                }
                const auto& plan = raWorld->surfaceDrawPlan_[selected.surface];
                const int rule = shadowReceiverRule(i, static_cast<U>(plan.worldInstance));
                if (rule < 0) continue;
                if (rule != boundRule) { gl.uniform1i(shadowReceiverFacingUniform, rule); boundRule = rule; }
                const auto& r = selected.scissor;
                gl.enable(0x0C11); gl.scissor(r[0], r[1], r[2] - r[0], r[3] - r[1]);
                drawGl.drawElementsBaseVertex(0x0004, I(plan.indexCount), 0x1405,
                    reinterpret_cast<const void*>(std::uintptr_t(plan.indexStart) * sizeof(U)), 0);
                accepted = check("draw original shadow receiver RA surface");
                ++draws;
            }
            gl.disable(0x0C11);
        }
        if (query) gl.endQuery(0x8914);
        frameShadowReceiverDraws[i] = draws;
        if (accepted && draws && !shadowStateLogged) {
            // One draw-state record per run (state of the receiver pass itself).
            I source = 0, destination = 0, depthFunction = 0, depthWrite = -1, colorWrite[4] = {-1, -1, -1, -1};
            I filters[2] = {-1, -1}, wraps[2] = {-1, -1}, extent[2] = {-1, -1};
            gl.getIntegerv(0x80C9, &source); gl.getIntegerv(0x80C8, &destination);
            gl.getIntegerv(0x0B74, &depthFunction); gl.getIntegerv(0x0B72, &depthWrite);
            gl.getIntegerv(0x0C23, colorWrite);
            if (gl.getTexParameteriv) {
                gl.getTexParameteriv(texture2D, 0x2801, &filters[0]); gl.getTexParameteriv(texture2D, 0x2800, &filters[1]);
                gl.getTexParameteriv(texture2D, 0x2802, &wraps[0]); gl.getTexParameteriv(texture2D, 0x2803, &wraps[1]);
            }
            if (gl.getTexLevelParameteriv) {
                gl.getTexLevelParameteriv(texture2D, 0, 0x1000, &extent[0]);
                gl.getTexLevelParameteriv(texture2D, 0, 0x1001, &extent[1]);
            }
            accepted = check("observe original shadow receiver state");
            std::fprintf(stderr, "original_shadow_state blend_src_dst=0x%x,0x%x depth_func=0x%x depth_write=%d "
                "color_write=%d,%d,%d,%d alpha=%.9g alpha_discard=0.5 min_mag=0x%x,0x%x wrap_st=0x%x,0x%x "
                "target=%dx%d caster_depth_test=0 caster_border=1\n",
                unsigned(source), unsigned(destination), unsigned(depthFunction), depthWrite,
                colorWrite[0], colorWrite[1], colorWrite[2], colorWrite[3], double(game::projectedShadowSourceAlpha50PS),
                unsigned(filters[0]), unsigned(filters[1]), unsigned(wraps[0]), unsigned(wraps[1]), extent[0], extent[1]);
            shadowStateLogged = true;
        }
    }
    gl.bindTexture(texture2D, 0);
    gl.colorMask(1, 1, 1, 1); gl.disable(0x0BE2); gl.disable(0x0C11);
    gl.depthMask(1); gl.depthFunc(0x0201);
    resetPodState();
    return check("restore state after original shadow receivers") && accepted;
}

void VehicleWindowGl50DS::Storage::finishShadowReceivers() noexcept {
    // Once per frame, after every racer's caster and receiver pass.
    if (!frameShadowsSet || !frameShadowCount) return;
    if (shadowTraceFrame) logOriginalShadowFrame();
    ++shadowFrames;
}

void VehicleWindowGl50DS::Storage::logOriginalShadowFrame() noexcept {
    for (U i = 0; i < frameShadowCount; ++i) {
        if (!shadowQueries[i] || !gl.getQueryObjectuiv || !frameShadowReceiverDraws[i]) continue;
        U samples = 0;
        gl.getQueryObjectuiv(shadowQueries[i], 0x8866, &samples); // QUERY_RESULT (waits)
        frameShadowSamples[i] = samples;
    }
    const auto& f = frameShadows;
    char casters[512]{}, receivers[256]{}, draws[256]{}, cover[256]{}, samples[256]{};
    std::size_t a = 0, b = 0, c = 0, e = 0, g = 0;
    const auto put = [](char* out, std::size_t size, std::size_t& at, const char* format, auto... values) noexcept {
        if (at >= size) return;
        const int n = std::snprintf(out + at, size - at, format, values...);
        if (n > 0) at = std::min(size, at + std::size_t(n));
    };
    for (U i = 0; i < frameShadowCount; ++i) {
        const auto& s = f.shadows[i];
        put(casters, sizeof(casters), a, "%s%u:", i ? "," : "", i);
        for (U k = 0; k < s.casterCount; ++k)
            put(casters, sizeof(casters), a, "%s%u/%u", k ? "+" : "", unsigned(s.casters[k].slot), unsigned(s.casters[k].lod));
        if (s.culled) put(casters, sizeof(casters), a, "culled");
        if (frameShadowHidden[i]) put(casters, sizeof(casters), a, "hidden");
        put(receivers, sizeof(receivers), b, "%s%u:%u", i ? "," : "", i, frameShadowReceivers[i].count);
        put(draws, sizeof(draws), c, "%s%u:%u", i ? "," : "", i, frameShadowReceiverDraws[i]);
        put(cover, sizeof(cover), e, "%s%u:%lld", i ? "," : "", i, static_cast<long long>(frameShadowCover[i]));
        put(samples, sizeof(samples), g, "%s%u:%lld", i ? "," : "", i, static_cast<long long>(frameShadowSamples[i]));
    }
    std::fprintf(stderr, "original_shadow frame=%llu shadows=%u considered=%u active=%u culled=%u engine_only=%u "
        "view_skipped=%u degenerate=%u missing_slots=%u camera_zone=%u casters=[%s] receivers=[%s] "
        "receiver_draws=[%s] rt_cover=[%s] samples=[%s] lod_parts_gpu=%llu lod_parts_cpu=%llu lod_parts_fallback=%llu\n",
        static_cast<unsigned long long>(f.frame), f.shadowCount, f.racersConsidered, f.active, f.culled, f.engineOnly,
        f.viewSkipped, f.degenerate, f.missingSlots, f.cameraZone, casters, receivers, draws, cover, samples,
        static_cast<unsigned long long>(shadowLodGpuParts), static_cast<unsigned long long>(shadowLodCpuParts),
        static_cast<unsigned long long>(shadowLodFallbackParts));
    for (U i = 0; i < frameShadowCount; ++i) {
        const auto& r = frameShadowReceivers[i];
        std::fprintf(stderr, "original_shadow_gather frame=%llu racer=%u listed=%u zones_visited=%u portals_seen=%u "
            "portals_visible=%u duplicates=%u flag_rejected=%u camera_rejected=%u near_accepted=%u "
            "frustum_accepted=%u frustum_rejected=%u capped=%u\n",
            static_cast<unsigned long long>(f.frame), i, r.count, r.zonesVisited, r.portalsSeen, r.portalsVisible,
            r.duplicates, r.flagRejected, r.cameraRejected, r.nearAccepted, r.frustumAccepted, r.frustumRejected, r.capped);
    }
}

bool VehicleWindowGl50DS::Storage::deleteShadowNames(VehicleWindowDiagnostic50DS& d) noexcept {
    if (!deleteShaderName(shadowCasterProgram, true, d) || !deleteShaderName(shadowCasterVertexShader, false, d) ||
        !deleteShaderName(shadowCasterFragmentShader, false, d) || !deleteShaderName(shadowReceiverProgram, true, d) ||
        !deleteShaderName(shadowReceiverVertexShader, false, d) || !deleteShaderName(shadowReceiverGeometryShader, false, d) ||
        !deleteShaderName(shadowReceiverFragmentShader, false, d)) return false;
    if (!deleteName(shadowCableVertexArray, gl.deleteVertexArrays, d) || !deleteName(shadowCableVertexBuffer, gl.deleteBuffers, d) ||
        !deleteName(shadowRaArray, gl.deleteVertexArrays, d) || !deleteName(shadowRaNormals, gl.deleteBuffers, d)) return false;
    for (auto& lods : shadowCasterLods)
        for (auto& l : lods)
            if (!deleteName(l.vertexArray, gl.deleteVertexArrays, d) || !deleteName(l.vertexBuffer, gl.deleteBuffers, d) ||
                !deleteName(l.indexBuffer, gl.deleteBuffers, d)) return false;
    shadowCasterLods.clear();
    if (!deleteName(shadowCpuVertexArray, gl.deleteVertexArrays, d) || !deleteName(shadowCpuVertexBuffer, gl.deleteBuffers, d))
        return false;
    for (auto& t : shadowTargets)
        if (!deleteName(t.framebuffer, gl.deleteFramebuffers, d) || !deleteName(t.color, gl.deleteTextures, d)) return false;
    if (gl.deleteQueries) for (auto& q : shadowQueries) if (!deleteName(q, gl.deleteQueries, d)) return false;
    shadowReceiverStatics.clear(); shadowResourcesCreated = false; frameShadowsSet = false; frameShadowCount = 0;
    return true;
}
std::uint32_t VehicleWindowGl50DS::racerShadowLodCount50PS(std::uint32_t archetype, std::uint32_t component) const noexcept {
    if (!initialized()) return 0;
    const auto& s = *storage_;
    if (archetype >= s.shadowCasterLods.size() || component >= maxComponents) return 0;
    const auto& lod = s.shadowCasterLods[archetype][component];
    return lod.ready ? lod.lodCount : 0u;
}
} // namespace rr::platform
