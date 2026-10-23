#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace rr::platform {

// Isolated numerical component, NOT a mixer/voice/playback admission owner.
// The retained Main/Application owner must supply genuinely routed per-core
// samples and apply source controls in order at exact native sample boundaries.
// No core summation, voice allocation, group policy, clock-from-UI, or device I/O.
// Single producer only; callers must not race processing, controls or diagnostics.
// See this proposal's MODEL.md for deliberate arithmetic and phase limitations.
class NativeSourceReverb50KG final {
public:
    static constexpr std::uint32_t nativeRate = 48000;
    static constexpr std::size_t coreCount = 2;
    static constexpr std::size_t workAreaBytesPerCore = 98368;
    static constexpr std::size_t workWordsPerCore = workAreaBytesPerCore / 2;
    static constexpr std::size_t firTaps = 39;
    // Same maximum duration as AudioMixer50HD::mixStereo16, not a new voice cap.
    static constexpr std::size_t maximumBlockFrames = nativeRate;

    struct Stereo {
        std::int16_t left = 0;
        std::int16_t right = 0;
    };
    struct Frame {
        // Already summed/saturated, source-routed PCM16 for each real core.
        // This class never duplicates one core's input into the other core.
        std::array<Stereo, coreCount> core{};
    };
    struct CoreStartup {
        std::uint32_t sourceMode = 0x107; // mode 7 plus actual work-area clear
        std::int32_t delay = 48;
        std::int32_t feedback = 24;
        std::uint16_t leftDepth = 0x7fff;
        std::uint16_t rightDepth = 0x7fff;
    };
    struct Startup {
        std::uint32_t outputRate = nativeRate;
        std::uint64_t firstFrame = 0;
        std::array<CoreStartup, coreCount> core{};
    };
    struct RawMaster {
        // Signed Q15 bit patterns, not percentages or normalized float gains.
        // The original absolute 25-percent command supplies 4095, not 8192.
        std::uint16_t left = 0x7fff;
        std::uint16_t right = 0x7fff;
    };
    struct Parameters {
        // Native signed-16 sample coordinates. Actual LIBSD address publishing
        // shifts its raw halfword by 2 to SPU2 word units; not raw array indices.
        std::array<std::uint32_t, 2> allPassDelayWords{};
        std::int32_t reflectionQ15 = 0;
        std::array<std::int32_t, 4> combQ15{};
        std::int32_t wallQ15 = 0;
        std::array<std::int32_t, 2> allPassQ15{};
        std::array<std::int32_t, 2> inputQ15{};
        std::array<std::uint32_t, 2> sameDestinationWords{};
        std::array<std::uint32_t, 2> sameSourceWords{};
        std::array<std::uint32_t, 2> crossDestinationWords{};
        std::array<std::uint32_t, 2> crossSourceWords{};
        std::array<std::array<std::uint32_t, 2>, 4> combSourceWords{};
        std::array<std::array<std::uint32_t, 2>, 2> allPassDestinationWords{};
    };
    struct Observation {
        std::uint64_t nextFrame = 0;
        std::uint64_t effectTicks = 0;
        std::uint32_t workCursorWord = 0;
        std::uint32_t firNextWrite = 0;
        std::uint8_t nextHalfRatePhase = 0;
        RawMaster master{};
    };

    NativeSourceReverb50KG() noexcept = default;
    NativeSourceReverb50KG(const NativeSourceReverb50KG&) = delete;
    NativeSourceReverb50KG& operator=(const NativeSourceReverb50KG&) = delete;
    NativeSourceReverb50KG(NativeSourceReverb50KG&&) = delete;
    NativeSourceReverb50KG& operator=(NativeSourceReverb50KG&&) = delete;

    // Cold, once per real audio lifetime. Entire configuration is validated
    // before mutation. Unsupported rate/mode/depth/delay/feedback and repeat
    // initialization reject without replacing or clearing an existing history.
    bool initialize(const Startup&) noexcept;
    bool configured() const noexcept { return configured_; }
    std::uint64_t nextFrame() const noexcept { return nextFrame_; }
    static const Parameters& startupParameters() noexcept;

    // These are immediate sample-boundary numerical operations, not command
    // queues/receipts. Caller owns original command ordering and authorization.
    // A master change never resets the work RAM, FIR histories, phase or clock.
    bool setRawMasters(const std::array<RawMaster, coreCount>&,
                       std::uint64_t atFrame) noexcept;
    // Only for a reached source clear operation. Bits 0/1 select actual cores.
    // Zeroes real work RAM only; preserves FIR history, cursor, phase and master.
    // A startup clear also initializes the newly constructed FIR histories.
    bool clearSourceWorkAreas(std::uint8_t coreMask,
                              std::uint64_t atFrame) noexcept;

    // Input/output extents are in Frame objects, never scalar samples. Exact
    // in-place processing is allowed; partial overlap or alias with this object
    // rejects. Pointers must refer to valid caller-owned live arrays. An empty
    // block may use null pointers. No block-time allocation or fallible steps
    // occur after preflight. The owner must preflight its other producers and
    // voices before advancing anything; this component cannot roll them back.
    bool preflight(const Frame* input, std::size_t inputExtent,
                   Frame* output, std::size_t outputExtent,
                   std::size_t frames, std::uint64_t firstFrame) const noexcept;
    bool process(const Frame* input, std::size_t inputExtent,
                 Frame* output, std::size_t outputExtent,
                 std::size_t frames, std::uint64_t firstFrame) noexcept;

    // Read-only numerical diagnostics. Neither grants playback/readiness.
    bool inspectCore(std::size_t core, Observation&) const noexcept;
    bool inspectWorkWord(std::size_t core, std::size_t word,
                         std::int16_t&) const noexcept;

private:
    struct CoreState {
        std::array<std::int16_t, workWordsPerCore> work{};
        std::array<Stereo, firTaps> inputHistory{};
        std::array<Stereo, firTaps> outputHistory{};
        std::uint32_t cursor = 0;
        RawMaster master{};
    };
    std::array<CoreState, coreCount> cores_{};
    std::uint64_t nextFrame_ = 0;
    std::uint64_t effectTicks_ = 0;
    std::uint32_t firWrite_ = 0;
    std::uint8_t halfPhase_ = 0;
    bool configured_ = false;

    static std::uint32_t workIndex(std::uint32_t cursor,
                                   std::int64_t displacement) noexcept;
    static std::int16_t readTap(const CoreState&, std::int64_t) noexcept;
    static void writeTap(CoreState&, std::int64_t, std::int64_t) noexcept;
    static Stereo filter(const std::array<Stereo, firTaps>&,
                         std::uint32_t newest, std::int64_t denominator) noexcept;
    static Stereo effectTick(CoreState&, Stereo) noexcept;
};

} // namespace rr::platform
