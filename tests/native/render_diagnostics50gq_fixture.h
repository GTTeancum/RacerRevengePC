#pragma once
// Opt-in development-HOST observations only. No arithmetic mode changes,
// canonical state writes, logging, heap storage, synchronization, or workers.
#include "phase_timing50gk_fixture.h"
#include "../../src/game/vehicle_frame49w.h"
#include "../../src/game/vehicle_draw50cm.h"
#include <cstring>
#include <limits>
#if defined(__SSE__) || (defined(_MSC_VER) && (defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 1)))
#include <xmmintrin.h>
#define RR_RENDER_DIAGNOSTICS50GQ_HAS_MXCSR 1
#else
#define RR_RENDER_DIAGNOSTICS50GQ_HAS_MXCSR 0
#endif

namespace render_diagnostics50GQ {
enum class Subphase : std::uint8_t {
    DrawBank, VehicleProjection, HostCamera, SourceCables, HostHud,
    DriverSubmit, Census, OpponentPreparation,
    // Nested inside OpponentPreparation; profiling rows only, never summed with it.
    OpponentDrawBatch, OpponentLights, OpponentCables, OpponentBinder, OpponentFlame, Count
};
constexpr std::size_t subphaseCount = static_cast<std::size_t>(Subphase::Count);

inline const char* subphaseName(Subphase phase) noexcept {
    switch (phase) {
    case Subphase::DrawBank: return "draw_bank";
    case Subphase::VehicleProjection: return "vehicle_projection";
    case Subphase::HostCamera: return "host_camera";
    case Subphase::SourceCables: return "source_cables";
    case Subphase::HostHud: return "host_hud";
    case Subphase::DriverSubmit: return "driver_submit";
    case Subphase::Census: return "census";
    case Subphase::OpponentPreparation: return "opponent_preparation";
    case Subphase::OpponentDrawBatch: return "opponent_draw_batch";
    case Subphase::OpponentLights: return "opponent_lights";
    case Subphase::OpponentCables: return "opponent_cables";
    case Subphase::OpponentBinder: return "opponent_binder";
    case Subphase::OpponentFlame: return "opponent_flame";
    default: return "invalid";
    }
}

enum class Binary32Class : std::uint8_t { Zero, Subnormal, Normal, Infinity, Nan };

// A supplied-bits seam avoids both floating-point arithmetic and reliance on
// fpclassify/isfinite behavior under the caller's MXCSR DAZ/FTZ configuration.
inline Binary32Class classifyBinary32Bits(std::uint32_t bits) noexcept {
    const auto exponent = bits & 0x7f800000u;
    const auto fraction = bits & 0x007fffffu;
    if (!exponent) return fraction ? Binary32Class::Subnormal : Binary32Class::Zero;
    if (exponent == 0x7f800000u)
        return fraction ? Binary32Class::Nan : Binary32Class::Infinity;
    return Binary32Class::Normal;
}

inline std::uint32_t binary32Bits(const float& value) noexcept {
    static_assert(sizeof(float) == sizeof(std::uint32_t) &&
        std::numeric_limits<float>::is_iec559 &&
        std::numeric_limits<float>::digits == 24, "IEEE binary32 required");
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}

enum class MatrixSource : std::uint8_t { None, Local, ReadyWorld, DirtyWorld };
inline const char* matrixSourceName(MatrixSource source) noexcept {
    switch (source) {
    case MatrixSource::Local: return "local";
    case MatrixSource::ReadyWorld: return "ready_world";
    case MatrixSource::DirtyWorld: return "dirty_world";
    default: return "none";
    }
}

struct SceneSample {
    std::uint64_t tick = 0;
    std::uint32_t nodes = 0, words = 0, dirtyNodes = 0;
    std::uint32_t subnormalWords = 0, localSubnormalWords = 0;
    std::uint32_t readyWorldSubnormalWords = 0, dirtyWorldSubnormalWords = 0;
    std::uint32_t nonfiniteWords = 0;
    // Only the first subnormal in deterministic node/local/world/lane order.
    // Indices/bits are available only when firstSource != None.
    MatrixSource firstSource = MatrixSource::None;
    std::uint32_t firstNode = 0, firstLane = 0, firstBits = 0;
    // Read-only observation. No _mm_setcsr/fenv write is performed anywhere.
    std::uint32_t mxcsr = 0;
    bool mxcsrAvailable = false, valid = false;
};

// Caller supplies stable, initialized nodes. Bounded to the native scene limit;
// only declared float fields are examined, never padding or other object bytes.
// Includes all 16 words, including opaque/fourth lanes. Dirty worlds can be old
// caches; neither their presence nor a subnormal proves arithmetic consumption.
inline SceneSample censusSceneNodes(const rr::game::SceneNode49P* nodes,
    std::uint32_t count, std::uint64_t tick = 0) noexcept {
    SceneSample sample{};
    sample.tick = tick;
    if (!nodes || !count || count > rr::game::maxSceneNodes49P) return sample;
    sample.nodes = count;
#if RR_RENDER_DIAGNOSTICS50GQ_HAS_MXCSR
    sample.mxcsr = static_cast<std::uint32_t>(_mm_getcsr());
    sample.mxcsrAvailable = true;
#endif
    for (std::uint32_t node = 0; node < count; ++node) {
        const bool ready = (nodes[node].sceneFlags190 & 1u) != 0;
        if (!ready) ++sample.dirtyNodes;
        for (unsigned matrix = 0; matrix < 2; ++matrix) {
            const auto& values = matrix ? nodes[node].world100 : nodes[node].localC0;
            const MatrixSource source = !matrix ? MatrixSource::Local :
                (ready ? MatrixSource::ReadyWorld : MatrixSource::DirtyWorld);
            for (std::uint32_t lane = 0; lane < 16; ++lane) {
                ++sample.words;
                const auto bits = binary32Bits(values.v[lane]);
                const auto kind = classifyBinary32Bits(bits);
                if (kind == Binary32Class::Infinity || kind == Binary32Class::Nan)
                    ++sample.nonfiniteWords;
                if (kind != Binary32Class::Subnormal) continue;
                ++sample.subnormalWords;
                if (!matrix) ++sample.localSubnormalWords;
                else if (ready) ++sample.readyWorldSubnormalWords;
                else ++sample.dirtyWorldSubnormalWords;
                if (sample.firstSource == MatrixSource::None) {
                    sample.firstSource = source;
                    sample.firstNode = node;
                    sample.firstLane = lane;
                    sample.firstBits = bits;
                }
            }
        }
    }
    sample.valid = true;
    return sample;
}

struct Interval {
    // Configure once before any scopes; use one owning thread. Reset only when
    // no scopes are live. This collector has no shared/global mutable state.
    explicit Interval(bool enable = false) noexcept : enabled(enable) {}
    bool enabled = false;
    phase_timing50GK::Totals totals[subphaseCount]{};
    SceneSample latestScene{}, sceneAtMaxWall[subphaseCount]{};
    std::uint64_t sceneSamples = 0, sceneFramesWithSubnormal = 0;
    std::uint64_t sceneFramesWithNonfinite = 0;
    std::uint32_t maxSubnormalWords = 0;
    bool overflow = false;
    // Attached to the shared draw workspace only around opponent batches.
    rr::game::VehicleDrawTiming50CM opponentDrawTiming{};

    void observeScene(const rr::game::VehicleFrameState49W& state,
        std::uint64_t tick = 0) noexcept {
        if (!enabled) return;
        latestScene = censusSceneNodes(state.nodes,
            rr::game::vehicleFrameNodeCount49W, tick);
        phase_timing50GK::detail::add(sceneSamples, 1, overflow);
        if (latestScene.subnormalWords)
            phase_timing50GK::detail::add(sceneFramesWithSubnormal, 1, overflow);
        if (latestScene.nonfiniteWords)
            phase_timing50GK::detail::add(sceneFramesWithNonfinite, 1, overflow);
        if (latestScene.subnormalWords > maxSubnormalWords)
            maxSubnormalWords = latestScene.subnormalWords;
    }

    void resetInterval() noexcept {
        opponentDrawTiming = {};
        for (auto& total : totals) total = phase_timing50GK::Totals{};
        for (auto& scene : sceneAtMaxWall) scene = SceneSample{};
        latestScene = SceneSample{};
        sceneSamples = sceneFramesWithSubnormal = sceneFramesWithNonfinite = 0;
        maxSubnormalWords = 0;
        overflow = false;
    }
};

class Scope final {
public:
    Scope(Interval& interval, Subphase phase) noexcept {
        const auto index = static_cast<std::size_t>(phase);
        if (interval.enabled && index < subphaseCount) {
            interval_ = &interval;
            index_ = index;
            before_ = phase_timing50GK::readClocks();
        }
    }
    ~Scope() noexcept { finish(); }
    Scope(const Scope&) = delete;
    Scope& operator=(const Scope&) = delete;
    Scope(Scope&&) = delete;
    Scope& operator=(Scope&&) = delete;

    // Keep latestScene unchanged between this scope's start/finish, except for
    // Census itself, which intentionally associates its just-observed scene.
    // Pair the scene with the same maximum-wall call as Totals' CPU sample.
    void finish() noexcept {
        if (!interval_) return;
        const auto after = phase_timing50GK::readClocks();
        auto& total = interval_->totals[index_];
        const bool validWall = before_.wallValid && after.wallValid &&
            after.wallNs >= before_.wallNs;
        if (validWall && (!total.wallCalls ||
            after.wallNs - before_.wallNs > total.maxWallNs))
            interval_->sceneAtMaxWall[index_] = interval_->latestScene;
        total.record(before_, after);
        interval_ = nullptr;
    }

private:
    Interval* interval_ = nullptr;
    std::size_t index_ = 0;
    phase_timing50GK::Sample before_{};
};
}

#undef RR_RENDER_DIAGNOSTICS50GQ_HAS_MXCSR
