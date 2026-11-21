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
#include <cfenv>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>
#if defined(_M_X64) || defined(_M_IX86) || defined(__x86_64__) || defined(__i386__)
#include <xmmintrin.h>
#define RR_WATER_SURFACE_MXCSR50WS 1
#endif

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
    // The point's 4-neighbour normal, computed once per frame for every emitted
    // point (rows 0..rows-1, columns 0..columns-1); the emit reads it for both
    // passes and both strips that share the point.
    bool degenerate = false;
    float n[3]{0.f, 1.f, 0.f};
};

// Host worker threads for the per-point water work (measurement-neutral host
// execution, no game memory). The calling thread runs part 0, worker k runs part
// k+1. Each part is a fixed contiguous range (deterministic partition), writes
// only its own disjoint slots and its own counters; the caller merges counters in
// part order. Workers take the caller's floating-point state (MXCSR: rounding,
// flush/denormal modes; fenv rounding) for every job, so every operation rounds
// as on the calling thread: the output is bit-identical to the one-thread build.
// RR_WATER_WORKERS=0..3 caps the worker count (0 = one-thread build).
class WaterSurfaceWorkers50WS final {
public:
    static constexpr std::size_t maximumWorkers = 3, maximumParts = maximumWorkers + 1u;
    using Job = void (*)(void* context, std::size_t part, std::size_t parts) noexcept;
    static WaterSurfaceWorkers50WS& shared() noexcept { static WaterSurfaceWorkers50WS pool; return pool; }
    std::size_t parts() const noexcept { return workerCount_ + 1u; }
    // Runs job(context, part, parts()) for every part, returns after all finished.
    void run(Job job, void* context) noexcept {
        if (!workerCount_) { job(context, 0, 1); return; }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            job_ = job; context_ = context; pending_ = workerCount_;
#ifdef RR_WATER_SURFACE_MXCSR50WS
            mxcsr_ = _mm_getcsr();
#endif
            rounding_ = std::fegetround();
            ++generation_;
        }
        wake_.notify_all();
        job(context, 0, parts());
        std::unique_lock<std::mutex> lock(mutex_);
        done_.wait(lock, [this] { return pending_ == 0; });
        job_ = nullptr; context_ = nullptr;
    }
    WaterSurfaceWorkers50WS(const WaterSurfaceWorkers50WS&) = delete;
    WaterSurfaceWorkers50WS& operator=(const WaterSurfaceWorkers50WS&) = delete;
    ~WaterSurfaceWorkers50WS() {
        { std::lock_guard<std::mutex> lock(mutex_); stop_ = true; }
        wake_.notify_all();
        for (std::size_t i = 0; i < workerCount_; ++i) if (threads_[i].joinable()) threads_[i].join();
    }
private:
    WaterSurfaceWorkers50WS() noexcept {
        const unsigned hardware = std::thread::hardware_concurrency();
        std::size_t wanted = hardware > 1u ? std::min<std::size_t>(maximumWorkers, hardware - 1u) : 0u;
        if (const char* e = std::getenv("RR_WATER_WORKERS"); e && e[0] >= '0' && e[0] <= '9' && !e[1])
            wanted = std::min<std::size_t>(wanted, std::size_t(e[0] - '0'));
        try {
            for (std::size_t i = 0; i < wanted; ++i) { threads_[i] = std::thread(&WaterSurfaceWorkers50WS::loop, this, i); ++workerCount_; }
        } catch (...) {} // fewer workers (or none): the same partition rule over parts()
    }
    void loop(std::size_t index) noexcept {
        std::uint64_t seen = 0;
        for (;;) {
            Job job = nullptr; void* context = nullptr; std::size_t parts = 1;
            unsigned mxcsr = 0; int rounding = FE_TONEAREST;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                wake_.wait(lock, [&] { return stop_ || generation_ != seen; });
                if (stop_) return;
                seen = generation_; job = job_; context = context_; parts = workerCount_ + 1u;
                mxcsr = mxcsr_; rounding = rounding_;
            }
#ifdef RR_WATER_SURFACE_MXCSR50WS
            _mm_setcsr(mxcsr);
#else
            (void)mxcsr;
#endif
            std::fesetround(rounding);
            if (job) job(context, index + 1u, parts);
            { std::lock_guard<std::mutex> lock(mutex_); if (pending_ && --pending_ == 0) done_.notify_one(); }
        }
    }
    std::array<std::thread, maximumWorkers> threads_{};
    std::size_t workerCount_ = 0, pending_ = 0;
    std::mutex mutex_;
    std::condition_variable wake_, done_;
    std::uint64_t generation_ = 0;
    Job job_ = nullptr; void* context_ = nullptr;
    unsigned mxcsr_ = 0; int rounding_ = FE_TONEAREST;
    bool stop_ = false;
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

namespace water_surface_detail50ws {
// Fixed contiguous partition of [0, total) into `parts` ranges (deterministic).
inline void partRange(std::size_t total, std::size_t part, std::size_t parts, std::size_t& begin, std::size_t& end) noexcept {
    begin = parts ? total * part / parts : 0u; end = parts ? total * (part + 1u) / parts : 0u;
}
struct SamplePart {   // phase 1 counters of one part
    std::uint32_t rayMisses = 0, sampleFailures = 0;
    bool any = false; float heightMin = 0.f, heightMax = 0.f;
};
struct EmitPart {     // phase 3 counters of one part
    std::uint32_t degenerateNormals = 0, frothVertices = 0;
    std::uint32_t envAlphaMin = 255, envAlphaMax = 0, frothAlphaMax = 0;
    bool nonFinite = false, outOfRange = false;
};
struct Build {
    const WaterSurfaceFrame50WS* f = nullptr;
    WaterSurfacePoint50WS* points = nullptr; std::size_t pointCount = 0;
    assets::Vertex48B* out = nullptr; std::size_t outCount = 0;
    std::uint32_t pr = 0, pc = 0, rows = 0, columns = 0, bands = 0;
    V3 C{}, D{}, A{}, B{};
    float rowStep = 0.f, kFar = 0.f, base = 0.f, range = 0.f, h = 0.f, h1 = 0.f;
    std::array<SamplePart, WaterSurfaceWorkers50WS::maximumParts> sample{};
    std::array<EmitPart, WaterSurfaceWorkers50WS::maximumParts> emit{};
    std::array<bool, WaterSurfaceWorkers50WS::maximumParts> partOutOfRange{};
    // Bounds-checked slot of grid point (r, c), r in -1..rows, c in -1..columns.
    WaterSurfacePoint50WS* at(std::int32_t r, std::int32_t c) const noexcept {
        if (r < -1 || c < -1 || r > std::int32_t(rows) || c > std::int32_t(columns)) return nullptr;
        const std::size_t i = std::size_t(r + 1) * pc + std::size_t(c + 1);
        return i < pointCount ? points + i : nullptr;
    }
};
// Phase 1: every grid point's ray/plane hit and height sample (the original's
// per-point maths, unchanged). Part p owns point rows [begin, end) of the pr rows.
inline void samplePart(void* context, std::size_t part, std::size_t parts) noexcept {
    auto& b = *static_cast<Build*>(context);
    if (part >= b.sample.size()) return;
    const auto& f = *b.f; auto& k0 = b.sample[part]; k0 = {};
    std::size_t begin = 0, end = 0; partRange(b.pr, part, parts, begin, end);
    for (std::size_t ri = begin; ri < end; ++ri) {
        const std::int32_t r = std::int32_t(ri) - 1;
        for (std::int32_t c = -1; c <= std::int32_t(b.columns); ++c) {
            auto* slot = b.at(r, c);
            if (!slot) { b.partOutOfRange[part] = true; return; }
            auto& pt = *slot;
            const V3 Q = add(add(b.D, mul(b.A, float(r) * b.rowStep)), mul(b.B, float(c) / 16.f));
            const float dy = Q.y - b.C.y;
            // Ray / plane y = WaterBaseHeight (2B5710 plane 443C00/443C10,
            // drawn by 2B3DC0, depth read back as d; VU: P0 = C + (Q-C) k).
            float k = dy != 0.f ? (f.baseHeight - b.C.y) / dy : -1.f;
            pt.miss = !(k > 0.f) || k > b.kFar || !std::isfinite(k);
            if (pt.miss) { k = b.kFar * f.missDepthScale; ++k0.rayMisses; }
            const V3 P0 = add(b.C, mul(sub(Q, b.C), k));
            WaterSurfaceSample50WS s{};
            if (!f.sample(f.sampleContext, P0.x, P0.z, s) || !std::isfinite(s.height)) {
                ++k0.sampleFailures; s = {};
            }
            pt.p[0] = P0.x; pt.p[1] = P0.y + s.height; pt.p[2] = P0.z;
            pt.frothA = s.frothA; pt.frothB = s.frothB;
            if (!k0.any) { k0.heightMin = k0.heightMax = s.height; k0.any = true; }
            k0.heightMin = std::min(k0.heightMin, s.height);
            k0.heightMax = std::max(k0.heightMax, s.height);
        }
    }
}
// Phase 2: the 4-neighbour normal of every emitted point, once (it was
// recomputed for each pass and each strip sharing the point; same expression).
// Part p owns emitted rows [begin, end) of rows 0..rows-1; it reads neighbour
// positions written in phase 1 and writes only its own points' n/degenerate.
inline void normalPart(void* context, std::size_t part, std::size_t parts) noexcept {
    auto& b = *static_cast<Build*>(context);
    if (part >= b.partOutOfRange.size()) return;
    std::size_t begin = 0, end = 0; partRange(b.rows, part, parts, begin, end);
    for (std::size_t ri = begin; ri < end; ++ri) {
        const std::int32_t r = std::int32_t(ri);
        for (std::int32_t c = 0; c < std::int32_t(b.columns); ++c) {
            auto* self = b.at(r, c);
            const auto* e = b.at(r, c + 1); const auto* w = b.at(r, c - 1);
            const auto* u = b.at(r - 1, c); const auto* d = b.at(r + 1, c);
            if (!self || !e || !w || !u || !d) { b.partOutOfRange[part] = true; return; }
            const auto pv = [](const WaterSurfacePoint50WS& q) noexcept { return V3{q.p[0], q.p[1], q.p[2]}; };
            bool degenerate = false;
            const V3 N = normalized(cross(sub(pv(*e), pv(*w)), sub(pv(*u), pv(*d))), degenerate);
            self->degenerate = degenerate; self->n[0] = N.x; self->n[1] = N.y; self->n[2] = N.z;
        }
    }
}
// One emitted vertex (the original emit body; the normal comes from phase 2).
inline assets::Vertex48B emitVertex(const Build& b, const WaterSurfacePoint50WS& pt,
    const WaterSurfacePoint50WS& fr, bool froth, EmitPart& k) noexcept {
    const auto& w = *b.f->authoring;
    const V3 P{pt.p[0], pt.p[1], pt.p[2]};
    const V3 N{pt.n[0], pt.n[1], pt.n[2]};
    if (pt.degenerate) ++k.degenerateNormals;
    assets::Vertex48B v{};
    v.position[0] = P.x; v.position[1] = P.y; v.position[2] = P.z;
    if (!froth) {
        bool dv = false;
        const V3 V = normalized(sub(P, b.C), dv);
        const float nv = dot(N, V);
        const V3 Rf = sub(V, mul(N, 2.f * nv));
        const float inv = 1.f / (std::fabs(Rf.y) + 1.f);
        v.uv[0] = Rf.x * inv * .5f + .5f;
        v.uv[1] = Rf.z * inv * .5f + .5f;
        for (unsigned i = 0; i < 3; ++i) v.color[i] = std::uint8_t(std::clamp(w.env[i], 0, 255));
        v.color[3] = byteOf((1.f - std::fabs(nv)) * b.range + b.base);
        k.envAlphaMin = std::min<std::uint32_t>(k.envAlphaMin, v.color[3]);
        k.envAlphaMax = std::max<std::uint32_t>(k.envAlphaMax, v.color[3]);
    } else {
        // trunc(origin*TCS) is an integer UV shift on a repeating texture,
        // so it is omitted (provider+0x3D0/+0x3D8 kept the PS2 floats small).
        v.uv[0] = P.x * w.texCoordScale - .1f * N.x;
        v.uv[1] = P.z * w.texCoordScale - .1f * N.z;
        for (unsigned i = 0; i < 3; ++i) v.color[i] = std::uint8_t(std::clamp(w.froth[i], 0, 255));
        // Froth bytes come from one row later and one column left of the
        // vertex: the VU reads the current call's row at index j-1 (L5xx,
        // PLAN 3 table). Original behaviour, kept on purpose.
        v.color[3] = byteOf(std::min((float(fr.frothA) * b.h1 + float(fr.frothB) * b.h) * .25f, 64.f));
        if (v.color[3]) ++k.frothVertices;
        k.frothAlphaMax = std::max<std::uint32_t>(k.frothAlphaMax, v.color[3]);
    }
    if (!std::isfinite(v.position[0] + v.position[1] + v.position[2] + v.uv[0] + v.uv[1])) k.nonFinite = true;
    return v;
}
// Phase 3: strips in kick order (band-major, rows top to bottom). Strip s owns
// output slots [s*68, s*68+68): pass A 34 then pass B 34, zig-zag (r, r+1) per
// column, exactly the order the one-thread build pushed them. Part p owns strips
// [begin, end); every write is checked against the output size first.
inline void emitPart(void* context, std::size_t part, std::size_t parts) noexcept {
    auto& b = *static_cast<Build*>(context);
    if (part >= b.emit.size()) return;
    auto& k = b.emit[part]; k = {};
    const std::size_t stripRows = b.rows - 1u, strips = std::size_t(b.bands) * stripRows;
    constexpr std::size_t perStrip = 2u * waterStripVertices50WS;
    std::size_t begin = 0, end = 0; partRange(strips, part, parts, begin, end);
    for (std::size_t s = begin; s < end; ++s) {
        const std::uint32_t band = std::uint32_t(s / stripRows), r = std::uint32_t(s % stripRows);
        const std::size_t base = s * perStrip;
        if (base + perStrip > b.outCount) { k.outOfRange = true; return; }
        for (unsigned pass = 0; pass < 2; ++pass)
            for (std::uint32_t j = 0; j < 17u; ++j) {
                const std::int32_t c = std::int32_t(16u * band + j);
                const auto* top = b.at(std::int32_t(r), c); const auto* bottom = b.at(std::int32_t(r + 1u), c);
                const auto* topFroth = b.at(std::int32_t(r + 1u), c - 1); const auto* bottomFroth = b.at(std::int32_t(r + 2u), c - 1);
                if (!top || !bottom || (pass == 1 && (!topFroth || !bottomFroth))) { k.outOfRange = true; return; }
                const std::size_t slot = base + pass * waterStripVertices50WS + 2u * j;
                b.out[slot] = emitVertex(b, *top, pass == 1 ? *topFroth : *top, pass == 1, k);
                b.out[slot + 1u] = emitVertex(b, *bottom, pass == 1 ? *bottomFroth : *bottom, pass == 1, k);
            }
    }
}
inline void runPhase(bool parallel, WaterSurfaceWorkers50WS::Job job, Build& b) noexcept {
    if (parallel) WaterSurfaceWorkers50WS::shared().run(job, &b);
    else job(&b, 0, 1);
}
} // namespace water_surface_detail50ws

// Builds every strip in the original kick order: band-major, rows top to
// bottom; per strip pass A (reflection) then pass B (froth); per pass 34
// vertices zig-zag (row r, row r+1) for 17 columns. Vertex48B: world
// position, UV (S,T; the GS STQ divide is the GL perspective interpolation),
// RGBA bytes on the PS2 scale (128 = 1.0). Fog is the renderer's world fog.
// scratch is caller storage reused every frame (no per-frame allocation once
// sized). Returns false on non-finite input or an unbound grid.
// parallel: the per-point work runs on WaterSurfaceWorkers50WS (bit-identical to
// parallel=false, see the class). f.sample must be re-entrant and read-only (the
// one water sampler only reads the completed water runtime).
inline bool buildWaterSurface50WS(const WaterSurfaceFrame50WS& f, std::vector<assets::Vertex48B>& out,
    std::vector<WaterSurfacePoint50WS>& scratch, WaterSurfaceCounters50WS& counters, bool parallel = true) noexcept {
    using namespace water_surface_detail50ws;
    counters = {};
    out.clear();
    const auto& g = f.grid;
    if (!g.bound || g.rows < 2 || !g.bands || !f.authoring || !f.authoring->bound || !f.sample) return false;
    if (g.columns != 16u * g.bands + 1u) return false; // emitted columns 0..16*bands must be grid columns
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
    Build b{};
    b.f = &f; b.C = C;
    b.D = sub(add(Pn, mul(U, hy)), mul(R, hx));                              // +0x3B0
    b.A = mul(U, hy);                                                         // +0x3A0
    const float widen = float(17u * g.bands + 2u) / float(17u * g.bands);   // +0x498
    b.B = mul(R, 2.f * hx / float(g.bands) * widen);                         // +0x390
    b.rowStep = -2.f / float(g.rows);                                        // 443EC0
    b.kFar = cam.farPlane / cam.nearPlane;                                    // d = 0 (cleared Z)
    // Points for rows -1..rows and columns -1..columns (normal and froth neighbours).
    b.pr = g.rows + 2u; b.pc = g.columns + 2u; b.rows = g.rows; b.columns = g.columns; b.bands = g.bands;
    const std::size_t capacity = waterSurfaceVertexCapacity50WS(g);
    try { scratch.resize(std::size_t(b.pr) * b.pc); out.resize(capacity); }
    catch (...) { out.clear(); return false; }
    b.points = scratch.data(); b.pointCount = scratch.size();
    b.out = out.data(); b.outCount = out.size();
    b.base = w.baseOpacity; b.range = w.maxOpacity - w.baseOpacity;
    b.h = f.frothPhase35C; b.h1 = 1.f - b.h;
    runPhase(parallel, &samplePart, b);
    runPhase(parallel, &normalPart, b);
    runPhase(parallel, &emitPart, b);
    // Merge the part counters in part order (sums; min/max as the one-thread
    // build's running min/max over the same points in the same order).
    bool firstHeight = true, failed = false;
    for (const bool o : b.partOutOfRange) failed = failed || o;
    for (const auto& k : b.sample) {
        counters.rayMisses += k.rayMisses; counters.sampleFailures += k.sampleFailures;
        if (!k.any) continue;
        if (firstHeight) { counters.heightMin = k.heightMin; counters.heightMax = k.heightMax; firstHeight = false; }
        counters.heightMin = std::min(counters.heightMin, k.heightMin);
        counters.heightMax = std::max(counters.heightMax, k.heightMax);
    }
    for (const auto& k : b.emit) {
        counters.degenerateNormals += k.degenerateNormals; counters.frothVertices += k.frothVertices;
        counters.envAlphaMin = std::min(counters.envAlphaMin, k.envAlphaMin);
        counters.envAlphaMax = std::max(counters.envAlphaMax, k.envAlphaMax);
        counters.frothAlphaMax = std::max(counters.frothAlphaMax, k.frothAlphaMax);
        failed = failed || k.outOfRange || k.nonFinite;
    }
    counters.strips = g.bands * (g.rows - 1u);
    counters.vertices = std::uint32_t(out.size());
    return !failed;
}
} // namespace rr::game
