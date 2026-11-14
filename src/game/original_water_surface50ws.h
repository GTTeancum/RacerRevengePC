#pragma once
// Original water surface: the VU1 microprogram `Water` (descriptor 333270,
// VU1 0x010..0x5F7) and its EE inputs (2B5710 frustum, 2B8B80 header block),
// rebuilt as plain CPU maths for every course whose SCB WaterFile is not
// `none`. Recovery: work/scratch/audit-render/water/PLAN.md sections 2-4.
// No GL, no owners, no course names. One common path.
//
// The original rasterises the flat water plane (y = WaterBaseHeight, 2B3DC0)
// through a one-pixel-per-grid-ray camera viewport and reads its depth back;
// this module intersects the same rays with the same plane directly.
#include "../assets/ai_authoring50iu.h"
#include "../assets/native_asset48b.h"
#include "original_water_readback_layout50la.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

namespace rr::game {

// ---------------------------------------------------------------------------
// WTT render keys read by 2B92F0 that the physics reader (water_authoring50jc)
// does not keep. Defaults are the reader's own (2B93C4 32.0, 2B93E0 64.0,
// Env/Froth 0x50, AlphaReference 383540 = 50, TexCoordScale 383538 = 0.1,
// NumZones 16, SeesWater 1, ShaderName "WaterDefault").
// ---------------------------------------------------------------------------
inline constexpr std::uint32_t maxWaterZones50WS = 64;
struct WaterRenderAuthoring50WS {
    float baseOpacity = 32.f, maxOpacity = 64.f;
    std::int32_t env[3]{0x50, 0x50, 0x50}, froth[3]{0x50, 0x50, 0x50};
    std::int32_t alphaReference = 50;
    float texCoordScale = .1f;
    std::int32_t zoneCount = 16;
    std::array<std::uint8_t, maxWaterZones50WS> seesWater{};
    std::array<char, 64> shaderName{};
    bool bound = false;
    WaterRenderAuthoring50WS() noexcept {
        seesWater.fill(1);
        std::memcpy(shaderName.data(), "WaterDefault", 13);
    }
    // 2B9140: Water+0x4A0 = SeesWater[cameraZone]; zone is zero-based
    // (camera->+0x38->+0x48). Out of range is the original assert, so false.
    bool seesWater48(std::uint32_t zone) const noexcept {
        return bound && zone < std::uint32_t(zoneCount) && zone < maxWaterZones50WS && seesWater[zone] != 0;
    }
};
inline bool readWaterRenderAuthoring50WS(const char* text, std::size_t bytes,
    WaterRenderAuthoring50WS& output, assets::AiAuthoringDiagnostic50IU& diagnostic) noexcept {
    using namespace assets::ai_authoring_detail50IU;
    using assets::AiAuthoringError50IU;
    diagnostic = {};
    WaterRenderAuthoring50WS staged{};
    enum class Section { other, general, froth, zone } current = Section::other;
    std::int32_t zone = -1;
    auto visit = [&](Slice section, Slice key, Slice value) noexcept {
        if (!key.n) {
            current = Section::other; zone = -1;
            if (equal(section, "General Information")) current = Section::general;
            else if (equal(section, "Wake Froth")) current = Section::froth;
            else {
                constexpr char prefix[] = "Zone ";
                const std::size_t p = sizeof(prefix) - 1u;
                if (section.n > p && equal({section.p, p, section.offset}, prefix)) {
                    std::int32_t n = 0;
                    if (!integer({section.p + p, section.n - p, section.offset + p}, n) || n < 1 ||
                        n > std::int32_t(maxWaterZones50WS))
                        return fail(diagnostic, AiAuthoringError50IU::Limit, 0x2b9b00u, section.offset);
                    current = Section::zone; zone = n - 1;
                }
            }
            return true;
        }
        auto real = [&](float& d) noexcept {
            if (!assets::parsePodPhysicsNumber49N(value.p, value.n, d))
                return fail(diagnostic, AiAuthoringError50IU::Number, 0x158b80u, value.offset);
            return true;
        };
        auto whole = [&](std::int32_t& d) noexcept {
            if (!integer(value, d)) {
                // EnvColor/AlphaReference may be authored as decimals; the
                // source getter truncates through CVT.W.S.
                float f = 0.f;
                if (!assets::parsePodPhysicsNumber49N(value.p, value.n, f) || !std::isfinite(f))
                    return fail(diagnostic, AiAuthoringError50IU::Number, 0x158b30u, value.offset);
                d = std::int32_t(f);
            }
            return true;
        };
        if (current == Section::general) {
            if (equal(key, "BaseOpacity")) return real(staged.baseOpacity);
            if (equal(key, "MaxOpacity")) return real(staged.maxOpacity);
            if (equal(key, "EnvColorRed")) return whole(staged.env[0]);
            if (equal(key, "EnvColorGreen")) return whole(staged.env[1]);
            if (equal(key, "EnvColorBlue")) return whole(staged.env[2]);
            if (equal(key, "FrothColorRed")) return whole(staged.froth[0]);
            if (equal(key, "FrothColorGreen")) return whole(staged.froth[1]);
            if (equal(key, "FrothColorBlue")) return whole(staged.froth[2]);
            if (equal(key, "NumZones")) return whole(staged.zoneCount);
            if (equal(key, "ShaderName")) {
                Slice name = value;
                if (name.n >= 2 && name.p[0] == '"' && name.p[name.n - 1] == '"') { ++name.p; name.n -= 2; }
                if (!name.n || name.n >= staged.shaderName.size())
                    return fail(diagnostic, AiAuthoringError50IU::Limit, 0x2b9300u, value.offset);
                staged.shaderName.fill(0);
                std::memcpy(staged.shaderName.data(), name.p, name.n);
            }
            return true;
        }
        if (current == Section::froth) {
            if (equal(key, "AlphaReference")) return whole(staged.alphaReference);
            if (equal(key, "TexCoordScale")) return real(staged.texCoordScale);
            return true;
        }
        if (current == Section::zone && equal(key, "SeesWater")) {
            std::int32_t v = 1;
            if (!whole(v)) return false;
            staged.seesWater[std::size_t(zone)] = v ? 1u : 0u;
        }
        return true;
    };
    if (!scan(text, bytes, 64u * 1024u, 0x2b92f0u, visit, diagnostic)) return false;
    if (staged.zoneCount < 1 || staged.zoneCount > std::int32_t(maxWaterZones50WS))
        return fail(diagnostic, AiAuthoringError50IU::Limit, 0x2b9b00u, 0);
    staged.bound = true;
    output = staged;
    return true;
}

// ---------------------------------------------------------------------------
// Grid size: factory 274570 (tables 383370/390/3B0) called from 1AA3B8 with
// a1 = Race+0x8C (0 for a one-view race) and a3 = 0 only when profile track id
// (+0x1125F & 15) is 6 and the human count ((+0x1125C >> 5) & 3) is 1, else 3.
// This is the original's own branch on its track id; keep it data-driven.
// One-player result: GC 29/2 (88 x 33), MC 85/4 (256 x 65).
// ---------------------------------------------------------------------------
struct WaterGrid50WS {
    std::uint32_t n = 0, bands = 0;
    std::uint32_t rows = 0;    // Water+0x384 = 3N+1 (one row per grid ray row)
    std::uint32_t columns = 0; // 16*bands+1 emitted columns
    bool bound = false;
};
inline WaterGrid50WS selectWaterGrid50WS(std::uint32_t race8C, std::uint32_t trackId1125F,
    std::uint32_t humans1125C) noexcept {
    const std::uint32_t index = ((trackId1125F & 15u) == 6u && (humans1125C & 3u) == 1u) ? 0u : 3u;
    const auto a = OriginalWaterReadbackLayout50LA::factoryArgumentsNumeric(race8C, index);
    if (!a || !a->constructorA2 || !a->constructorA3) return {};
    return {a->constructorA2, a->constructorA3, 3u * a->constructorA2 + 1u, 16u * a->constructorA3 + 1u, true};
}

// ---------------------------------------------------------------------------
// Per-frame inputs.
// ---------------------------------------------------------------------------
struct WaterSurfaceCamera50WS {
    float position[3]{};        // camera+0x180
    float look[3]{0, 0, 1};     // camera+0x190 (forward)
    float up[3]{0, 1, 0};       // camera+0x1A0
    float fovDegrees = 77.f;    // camera+0x1D0
    float aspect1F8 = 1.f;      // camera+0x1F8
    float nearPlane = 1.f;      // camera+0x1FC
    float farPlane = 100000.f;  // camera+0x200
};
// Height offset above WaterBaseHeight at world (x, z): the seven weighted
// layer channels of PLAN 3.3 (wake + signature channels 0..3), already in
// world units. frothA/B are the two froth-layer bytes (0..255) lerped by h.
struct WaterSurfaceSample50WS {
    float height = 0.f;
    std::uint8_t frothA = 0, frothB = 0;
};
using WaterSurfaceSampler50WS = bool (*)(const void* context, float x, float z,
    WaterSurfaceSample50WS& out) noexcept;
struct WaterSurfaceFrame50WS {
    WaterSurfaceCamera50WS camera{};
    WaterGrid50WS grid{};
    const WaterRenderAuthoring50WS* authoring = nullptr;
    float baseHeight = 0.f;      // Water+0x3C (WaterBaseHeight)
    float frothPhase35C = 0.f;   // h = provider+0x35C (FramesPerSecond4 phase)
    WaterSurfaceSampler50WS sample = nullptr;
    const void* sampleContext = nullptr;
    // A ray that misses the plane (above the horizon) is placed at
    // kFar * missDepthScale. The original reads cleared Z (d = 0) and puts the
    // point ON the far plane, then clamps Z instead of clipping (VU Z clamp).
    // GL clips at the far plane, and points exactly on it fall either side by
    // rounding, which drew alternate strips over the sky (MC streaks, 134e).
    // STAND-IN (caller default 2): misses go beyond the far plane so the GPU
    // clips them as a whole and the sky stays clear. Whether the original
    // tints the sky there (its Z test against the sky) is emulator item W-E1;
    // 1 restores on-plane placement for that comparison.
    float missDepthScale = 1.f;
};
struct WaterSurfaceCounters50WS {
    std::uint32_t strips = 0, vertices = 0, rayMisses = 0, sampleFailures = 0;
    std::uint32_t degenerateNormals = 0, frothVertices = 0;
    std::uint32_t envAlphaMin = 255, envAlphaMax = 0, frothAlphaMax = 0;
    float heightMin = 0.f, heightMax = 0.f;
};
inline constexpr std::uint32_t waterStripVertices50WS = 34; // NLOOP 34: 17 columns x 2 rows
inline std::size_t waterSurfaceVertexCapacity50WS(const WaterGrid50WS& g) noexcept {
    return g.bound && g.rows > 1 ? std::size_t(g.rows - 1u) * g.bands * 2u * waterStripVertices50WS : 0u;
}
// Analytic grid point: P (world, height applied), plus its froth bytes.
struct WaterSurfacePoint50WS {
    float p[3]{};
    std::uint8_t frothA = 0, frothB = 0;
    bool miss = false;
};

namespace water_surface_detail50ws {
struct V3 { float x = 0, y = 0, z = 0; };
inline V3 sub(V3 a, V3 b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline V3 add(V3 a, V3 b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline V3 mul(V3 a, float s) noexcept { return {a.x * s, a.y * s, a.z * s}; }
inline float dot(V3 a, V3 b) noexcept { return a.x * b.x + a.y * b.y + a.z * b.z; }
inline V3 cross(V3 a, V3 b) noexcept { return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x}; }
inline V3 normalized(V3 a, bool& degenerate) noexcept {
    const float l = std::sqrt(dot(a, a));
    degenerate = !(l > 0.f) || !std::isfinite(l);
    return degenerate ? V3{0.f, 1.f, 0.f} : mul(a, 1.f / l);
}
inline std::uint8_t byteOf(float v) noexcept { // ftoi0 then GS byte
    if (!(v > 0.f)) return 0;
    return v >= 255.f ? 255u : std::uint8_t(std::int32_t(v));
}
} // namespace water_surface_detail50ws

// Builds every strip in the original kick order: band-major, rows top to
// bottom; per strip pass A (reflection) then pass B (froth); per pass 34
// vertices zig-zag (row r, row r+1) for 17 columns. Vertex48B: world
// position, UV (S,T; the GS STQ divide is the GL perspective interpolation),
// RGBA bytes on the PS2 scale (128 = 1.0). Fog is the renderer's world fog.
// scratch is caller storage reused every frame (no per-frame allocation once
// sized). Returns false on non-finite input or an unbound grid.
inline bool buildWaterSurface50WS(const WaterSurfaceFrame50WS& f, std::vector<assets::Vertex48B>& out,
    std::vector<WaterSurfacePoint50WS>& scratch, WaterSurfaceCounters50WS& counters) noexcept {
    using namespace water_surface_detail50ws;
    counters = {};
    out.clear();
    const auto& g = f.grid;
    if (!g.bound || g.rows < 2 || !g.bands || !f.authoring || !f.authoring->bound || !f.sample) return false;
    const auto& w = *f.authoring;
    const auto& cam = f.camera;
    const V3 C{cam.position[0], cam.position[1], cam.position[2]};
    const V3 F{cam.look[0], cam.look[1], cam.look[2]};
    const V3 U{cam.up[0], cam.up[1], cam.up[2]};
    if (!std::isfinite(C.x + C.y + C.z + F.x + F.y + F.z + U.x + U.y + U.z) ||
        !(cam.nearPlane > 0.f) || !(cam.farPlane > cam.nearPlane) || !std::isfinite(cam.fovDegrees))
        return false;
    // 2B5710: R = normalize(U x F); t = tan(fov/2); hx = n t; hy = n t aspect1F8.
    bool bad = false;
    const V3 R = normalized(cross(U, F), bad);
    if (bad) return false;
    const float t = std::tan(0.5f * cam.fovDegrees * 0.017453292f);
    const float hx = cam.nearPlane * t, hy = hx * cam.aspect1F8;
    const V3 Pn = add(C, mul(F, cam.nearPlane));
    const V3 D = sub(add(Pn, mul(U, hy)), mul(R, hx));                       // +0x3B0
    const V3 A = mul(U, hy);                                                  // +0x3A0
    const float widen = float(17u * g.bands + 2u) / float(17u * g.bands);   // +0x498
    const V3 B = mul(R, 2.f * hx / float(g.bands) * widen);                  // +0x390
    const float rowStep = -2.f / float(g.rows);                              // 443EC0
    const float kFar = cam.farPlane / cam.nearPlane;                          // d = 0 (cleared Z)
    // Points for rows -1..rows and columns -1..columns (normal and froth neighbours).
    const std::uint32_t pr = g.rows + 2u, pc = g.columns + 2u;
    try { scratch.resize(std::size_t(pr) * pc); out.reserve(waterSurfaceVertexCapacity50WS(g)); }
    catch (...) { return false; }
    auto at = [&](std::int32_t r, std::int32_t c) noexcept -> WaterSurfacePoint50WS& {
        return scratch[std::size_t(r + 1) * pc + std::size_t(c + 1)];
    };
    bool firstHeight = true;
    for (std::int32_t r = -1; r <= std::int32_t(g.rows); ++r)
        for (std::int32_t c = -1; c <= std::int32_t(g.columns); ++c) {
            auto& pt = at(r, c);
            const V3 Q = add(add(D, mul(A, float(r) * rowStep)), mul(B, float(c) / 16.f));
            const float dy = Q.y - C.y;
            // Ray / plane y = WaterBaseHeight (2B5710 plane 443C00/443C10,
            // drawn by 2B3DC0, depth read back as d; VU: P0 = C + (Q-C) k).
            float k = dy != 0.f ? (f.baseHeight - C.y) / dy : -1.f;
            pt.miss = !(k > 0.f) || k > kFar || !std::isfinite(k);
            if (pt.miss) { k = kFar * f.missDepthScale; ++counters.rayMisses; }
            const V3 P0 = add(C, mul(sub(Q, C), k));
            WaterSurfaceSample50WS s{};
            if (!f.sample(f.sampleContext, P0.x, P0.z, s) || !std::isfinite(s.height)) {
                ++counters.sampleFailures; s = {};
            }
            pt.p[0] = P0.x; pt.p[1] = P0.y + s.height; pt.p[2] = P0.z;
            pt.frothA = s.frothA; pt.frothB = s.frothB;
            if (firstHeight) { counters.heightMin = counters.heightMax = s.height; firstHeight = false; }
            counters.heightMin = std::min(counters.heightMin, s.height);
            counters.heightMax = std::max(counters.heightMax, s.height);
        }
    const float base = w.baseOpacity, range = w.maxOpacity - w.baseOpacity;
    const float h = f.frothPhase35C, h1 = 1.f - h;
    auto emit = [&](std::int32_t r, std::int32_t c, bool froth) noexcept {
        const auto& pt = at(r, c);
        const V3 P{pt.p[0], pt.p[1], pt.p[2]};
        auto pv = [&](std::int32_t rr, std::int32_t cc) noexcept {
            const auto& q = at(rr, cc); return V3{q.p[0], q.p[1], q.p[2]};
        };
        bool degenerate = false;
        const V3 N = normalized(cross(sub(pv(r, c + 1), pv(r, c - 1)), sub(pv(r - 1, c), pv(r + 1, c))), degenerate);
        if (degenerate) ++counters.degenerateNormals;
        assets::Vertex48B v{};
        v.position[0] = P.x; v.position[1] = P.y; v.position[2] = P.z;
        if (!froth) {
            bool dv = false;
            const V3 V = normalized(sub(P, C), dv);
            const float nv = dot(N, V);
            const V3 Rf = sub(V, mul(N, 2.f * nv));
            const float inv = 1.f / (std::fabs(Rf.y) + 1.f);
            v.uv[0] = Rf.x * inv * .5f + .5f;
            v.uv[1] = Rf.z * inv * .5f + .5f;
            for (unsigned i = 0; i < 3; ++i) v.color[i] = std::uint8_t(std::clamp(w.env[i], 0, 255));
            v.color[3] = byteOf((1.f - std::fabs(nv)) * range + base);
            counters.envAlphaMin = std::min<std::uint32_t>(counters.envAlphaMin, v.color[3]);
            counters.envAlphaMax = std::max<std::uint32_t>(counters.envAlphaMax, v.color[3]);
        } else {
            // trunc(origin*TCS) is an integer UV shift on a repeating texture,
            // so it is omitted (provider+0x3D0/+0x3D8 kept the PS2 floats small).
            v.uv[0] = P.x * w.texCoordScale - .1f * N.x;
            v.uv[1] = P.z * w.texCoordScale - .1f * N.z;
            for (unsigned i = 0; i < 3; ++i) v.color[i] = std::uint8_t(std::clamp(w.froth[i], 0, 255));
            // Froth bytes come from one row later and one column left of the
            // vertex: the VU reads the current call's row at index j-1 (L5xx,
            // PLAN 3 table). Original behaviour, kept on purpose.
            const auto& fr = at(r + 1, c - 1);
            v.color[3] = byteOf(std::min((float(fr.frothA) * h1 + float(fr.frothB) * h) * .25f, 64.f));
            if (v.color[3]) ++counters.frothVertices;
            counters.frothAlphaMax = std::max<std::uint32_t>(counters.frothAlphaMax, v.color[3]);
        }
        out.push_back(v);
    };
    for (std::uint32_t band = 0; band < g.bands; ++band)
        for (std::uint32_t r = 0; r + 1u < g.rows; ++r)
            for (unsigned pass = 0; pass < 2; ++pass) {
                for (std::uint32_t j = 0; j < 17u; ++j) {
                    const std::int32_t c = std::int32_t(16u * band + j);
                    emit(std::int32_t(r), c, pass == 1);
                    emit(std::int32_t(r + 1u), c, pass == 1);
                }
                if (pass == 0) ++counters.strips;
            }
    counters.vertices = std::uint32_t(out.size());
    for (const auto& v : out)
        if (!std::isfinite(v.position[0] + v.position[1] + v.position[2] + v.uv[0] + v.uv[1])) return false;
    return true;
}
} // namespace rr::game
