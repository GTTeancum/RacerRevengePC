#include "native_source_reverb50kg.h"
#include <climits>
#include <limits>

namespace rr::platform {
namespace {
using Reverb = NativeSourceReverb50KG;
static_assert(CHAR_BIT == 8 && sizeof(std::int16_t) == 2,
              "Native wet DSP requires 8-bit bytes and signed PCM16");

// Mathematical coefficients from psx-spx's Reverb Buffer Resampling section.
// This PS1 reference supplies the FIR shape, not a PS2 hardware-phase oracle.
constexpr std::array<std::int32_t, Reverb::firTaps> kFir{{
    -1, 0, 2, 0, -10, 0, 35, 0, -103, 0, 266, 0, -616, 0,
    1332, 0, -2960, 0, 10246, 16384, 10246, 0, -2960, 0,
    1332, 0, -616, 0, 266, 0, -103, 0, 35, 0, -10, 0, 2, 0, -1
}};

constexpr std::int32_t signedCoefficient(std::uint16_t bits) noexcept {
    return bits < 0x8000u ? static_cast<std::int32_t>(bits)
                         : static_cast<std::int32_t>(bits) - 65536;
}
constexpr std::uint32_t words(std::uint16_t sourceHalfword) noexcept {
    return static_cast<std::uint32_t>(sourceHalfword) * 4u;
}
constexpr Reverb::Parameters makeStartupParameters() noexcept {
    Reverb::Parameters p{};
    // Semantic identities come from LIBSD 012C's actual register destinations.
    // Every raw value below is the authenticated mode7/delay48/feedback24 result.
    // Full source offsets, publishing PCs and register addresses: PARAMETER-MAP.md.
    p.allPassDelayWords = {{words(0x0003), words(0x0003)}};
    p.reflectionQ15 = signedCoefficient(0x7fff);
    p.combQ15 = {{signedCoefficient(0x7fff), 0, 0, 0}};
    p.wallQ15 = signedCoefficient(0x1830);
    p.allPassQ15 = {{0, 0}};
    p.inputQ15 = {{signedCoefficient(0x8000), signedCoefficient(0x8000)}};
    p.sameDestinationWords = {{words(0x0c3d), words(0x061d)}};
    p.sameSourceWords = {{words(0x0629), words(0x0009)}};
    p.crossDestinationWords = {{words(0x1fff), words(0x1fff)}};
    p.crossSourceWords = {{words(0x1ffe), words(0x1ffe)}};
    p.combSourceWords = {{{{words(0x0629), words(0x0009)}},
                        {{words(0x0000), words(0x0000)}},
                        {{words(0x1ffe), words(0x1ffe)}},
                        {{words(0x1ffe), words(0x1ffe)}}}};
    p.allPassDestinationWords = {{{{words(0x0628), words(0x0624)}},
                                 {{words(0x0008), words(0x0004)}}}};
    return p;
}
constexpr Reverb::Parameters kStartupParameters = makeStartupParameters();

// C++ division truncates toward zero; explicitly select floor instead of
// implementation-defined negative right shifts or a host rounding mode.
std::int64_t floorDivide(std::int64_t value, std::int64_t denominator) noexcept {
    const auto quotient = value / denominator;
    return quotient - ((value < 0 && value % denominator != 0) ? 1 : 0);
}
std::int64_t multiplyQ15(std::int64_t sample, std::int32_t coefficient) noexcept {
    return floorDivide(sample * coefficient, 32768);
}
std::int16_t saturate(std::int64_t value) noexcept {
    if (value > 32767) return 32767;
    if (value < -32768) return -32768;
    return static_cast<std::int16_t>(value);
}
bool startupSupported(const Reverb::CoreStartup& c) noexcept {
    return c.sourceMode == 0x107 && c.delay == 48 && c.feedback == 24 &&
           c.leftDepth == 0x7fff && c.rightDepth == 0x7fff;
}
bool overlaps(std::uintptr_t a, std::size_t aSize,
              std::uintptr_t b, std::size_t bSize) noexcept {
    // Callers have checked both exclusive endpoints for arithmetic overflow.
    return a < b + bSize && b < a + aSize;
}
} // namespace

const NativeSourceReverb50KG::Parameters&
NativeSourceReverb50KG::startupParameters() noexcept {
    return kStartupParameters;
}

bool NativeSourceReverb50KG::initialize(const Startup& startup) noexcept {
    if (configured_ || startup.outputRate != nativeRate) return false;
    for (const auto& c : startup.core) if (!startupSupported(c)) return false;
    for (std::size_t core = 0; core < coreCount; ++core) {
        auto& c = cores_[core];
        // This is real storage clearing, not a cleared flag/receipt.
        c.work.fill(0);
        c.inputHistory.fill(Stereo{});
        c.outputHistory.fill(Stereo{});
        c.cursor = 0;
        c.master = {startup.core[core].leftDepth, startup.core[core].rightDepth};
    }
    nextFrame_ = startup.firstFrame;
    effectTicks_ = 0;
    firWrite_ = 0;
    halfPhase_ = 0;
    configured_ = true;
    return true;
}

bool NativeSourceReverb50KG::setRawMasters(
    const std::array<RawMaster, coreCount>& masters, std::uint64_t atFrame) noexcept {
    if (!configured_ || atFrame != nextFrame_) return false;
    // Snapshot even if a future internal caller supplies an aliasing array.
    const auto values = masters;
    for (std::size_t core = 0; core < coreCount; ++core) cores_[core].master = values[core];
    return true;
}

bool NativeSourceReverb50KG::clearSourceWorkAreas(
    std::uint8_t coreMask, std::uint64_t atFrame) noexcept {
    if (!configured_ || atFrame != nextFrame_ || coreMask == 0 || coreMask > 3) return false;
    for (std::size_t core = 0; core < coreCount; ++core)
        if ((coreMask & (1u << core)) != 0) cores_[core].work.fill(0);
    return true;
}

bool NativeSourceReverb50KG::preflight(
    const Frame* input, std::size_t inputExtent, Frame* output, std::size_t outputExtent,
    std::size_t frames, std::uint64_t firstFrame) const noexcept {
    if (!configured_ || firstFrame != nextFrame_ || frames > maximumBlockFrames ||
        frames > inputExtent || frames > outputExtent ||
        frames > std::numeric_limits<std::uint64_t>::max() - nextFrame_) return false;
    if (frames == 0) return true;
    if (!input || !output) return false;
    const auto bytes = frames * sizeof(Frame); // bounded by 48,000 frames
    const auto source = reinterpret_cast<std::uintptr_t>(input);
    const auto destination = reinterpret_cast<std::uintptr_t>(output);
    const auto owner = reinterpret_cast<std::uintptr_t>(this);
    const auto addressMax = std::numeric_limits<std::uintptr_t>::max();
    if (source > addressMax - bytes || destination > addressMax - bytes ||
        owner > addressMax - sizeof(*this)) return false;
    if (source != destination && overlaps(source, bytes, destination, bytes)) return false;
    return !overlaps(source, bytes, owner, sizeof(*this)) &&
           !overlaps(destination, bytes, owner, sizeof(*this));
}

std::uint32_t NativeSourceReverb50KG::workIndex(
    std::uint32_t cursor, std::int64_t displacement) noexcept {
    const auto size = static_cast<std::int64_t>(workWordsPerCore);
    auto index = (static_cast<std::int64_t>(cursor) + displacement) % size;
    if (index < 0) index += size;
    return static_cast<std::uint32_t>(index);
}
std::int16_t NativeSourceReverb50KG::readTap(
    const CoreState& core, std::int64_t displacement) noexcept {
    return core.work[workIndex(core.cursor, displacement)];
}
void NativeSourceReverb50KG::writeTap(
    CoreState& core, std::int64_t displacement, std::int64_t value) noexcept {
    core.work[workIndex(core.cursor, displacement)] = saturate(value);
}

NativeSourceReverb50KG::Stereo NativeSourceReverb50KG::filter(
    const std::array<Stereo, firTaps>& history, std::uint32_t newest,
    std::int64_t denominator) noexcept {
    std::int64_t left = 0, right = 0;
    auto index = newest;
    for (std::size_t tap = 0; tap < firTaps; ++tap) {
        left += static_cast<std::int64_t>(history[index].left) * kFir[tap];
        right += static_cast<std::int64_t>(history[index].right) * kFir[tap];
        index = index == 0 ? static_cast<std::uint32_t>(firTaps - 1) : index - 1;
    }
    return {saturate(floorDivide(left, denominator)),
            saturate(floorDivide(right, denominator))};
}

NativeSourceReverb50KG::Stereo NativeSourceReverb50KG::effectTick(
    CoreState& core, Stereo input) noexcept {
    const auto& p = kStartupParameters;
    const std::array<std::int64_t, 2> excitation{{
        multiplyQ15(input.left, p.inputQ15[0]),
        multiplyQ15(input.right, p.inputQ15[1])
    }};
    // Stage read snapshots avoid accidental host evaluation-order dependence.
    // There is one shared L/R ring per core, not two interleaved channel rings.
    std::array<std::int64_t, 2> same{}, crossed{};
    for (std::size_t side = 0; side < 2; ++side) {
        const auto previousSame = readTap(core, p.sameDestinationWords[side] - std::int64_t{1});
        const auto previousCross = readTap(core, p.crossDestinationWords[side] - std::int64_t{1});
        const auto ownWall = multiplyQ15(readTap(core, p.sameSourceWords[side]), p.wallQ15);
        const auto otherWall = multiplyQ15(readTap(core, p.crossSourceWords[1 - side]), p.wallQ15);
        same[side] = previousSame + multiplyQ15(excitation[side] + ownWall - previousSame,
                                               p.reflectionQ15);
        crossed[side] = previousCross + multiplyQ15(excitation[side] + otherWall - previousCross,
                                                   p.reflectionQ15);
    }
    for (std::size_t side = 0; side < 2; ++side) writeTap(core, p.sameDestinationWords[side], same[side]);
    // The supplied cross destinations alias: preserve explicit L then R stores.
    for (std::size_t side = 0; side < 2; ++side) writeTap(core, p.crossDestinationWords[side], crossed[side]);

    std::array<std::int64_t, 2> signal{};
    for (std::size_t comb = 0; comb < p.combQ15.size(); ++comb)
        for (std::size_t side = 0; side < 2; ++side)
            signal[side] += multiplyQ15(readTap(core, p.combSourceWords[comb][side]), p.combQ15[comb]);

    for (std::size_t stage = 0; stage < p.allPassQ15.size(); ++stage) {
        std::array<std::int64_t, 2> delayed{}, feed{};
        for (std::size_t side = 0; side < 2; ++side) {
            const auto displacement = static_cast<std::int64_t>(p.allPassDestinationWords[stage][side]) -
                                      p.allPassDelayWords[stage];
            delayed[side] = readTap(core, displacement);
            feed[side] = signal[side] - multiplyQ15(delayed[side], p.allPassQ15[stage]);
        }
        for (std::size_t side = 0; side < 2; ++side) {
            writeTap(core, p.allPassDestinationWords[stage][side], feed[side]);
            // Memory clips; the explicit wide feed accumulator does not.
            signal[side] = delayed[side] + multiplyQ15(feed[side], p.allPassQ15[stage]);
        }
    }
    core.cursor = core.cursor + 1 == workWordsPerCore ? 0 : core.cursor + 1;
    return {saturate(signal[0]), saturate(signal[1])};
}

bool NativeSourceReverb50KG::process(
    const Frame* input, std::size_t inputExtent, Frame* output, std::size_t outputExtent,
    std::size_t frames, std::uint64_t firstFrame) noexcept {
    if (!preflight(input, inputExtent, output, outputExtent, frames, firstFrame)) return false;
    for (std::size_t frame = 0; frame < frames; ++frame) {
        const Frame incoming = input[frame]; // exact in-place operation is safe
        Frame outgoing{};
        for (std::size_t core = 0; core < coreCount; ++core) {
            auto& c = cores_[core];
            c.inputHistory[firWrite_] = incoming.core[core];
            Stereo halfRate{};
            if (halfPhase_ == 0) halfRate = effectTick(c, filter(c.inputHistory, firWrite_, 32768));
            // Causal zero insertion + FIR reconstruction, with interpolation
            // gain 2 via denominator 16384. Phase/history continue across calls.
            c.outputHistory[firWrite_] = halfRate;
            const auto reconstructed = filter(c.outputHistory, firWrite_, 16384);
            outgoing.core[core] = {
                saturate(multiplyQ15(reconstructed.left, signedCoefficient(c.master.left))),
                saturate(multiplyQ15(reconstructed.right, signedCoefficient(c.master.right)))
            };
        }
        output[frame] = outgoing;
        if (halfPhase_ == 0) ++effectTicks_;
        halfPhase_ ^= 1;
        firWrite_ = firWrite_ + 1 == firTaps ? 0 : firWrite_ + 1;
        ++nextFrame_;
    }
    return true;
}

bool NativeSourceReverb50KG::inspectCore(std::size_t core, Observation& out) const noexcept {
    if (!configured_ || core >= coreCount) return false;
    Observation value{};
    value.nextFrame = nextFrame_;
    value.effectTicks = effectTicks_;
    value.workCursorWord = cores_[core].cursor;
    value.firNextWrite = firWrite_;
    value.nextHalfRatePhase = halfPhase_;
    value.master = cores_[core].master;
    out = value;
    return true;
}
bool NativeSourceReverb50KG::inspectWorkWord(
    std::size_t core, std::size_t word, std::int16_t& out) const noexcept {
    if (!configured_ || core >= coreCount || word >= workWordsPerCore) return false;
    out = cores_[core].work[word];
    return true;
}

} // namespace rr::platform
