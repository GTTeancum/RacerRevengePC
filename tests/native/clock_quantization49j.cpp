#include "../../src/game/clock_quantization49j.h"
#include "../../evidence/clockQuantization49J/sampler-source/sampler_oracle49j.h"
#include "frame_timing_oracle49i.h"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <new>
#include <stdexcept>

// Count successful allocations around hot calls only. No allocation failure,
// ownership, pointer, lifetime, alignment or foreign-memory probes are used.
static std::uint64_t allocations = 0;
void* operator new(std::size_t size) {
    if (void* pointer = std::malloc(size ? size : 1)) { ++allocations; return pointer; }
    throw std::bad_alloc();
}
void* operator new[](std::size_t size) { return ::operator new(size); }
void operator delete(void* pointer) noexcept { std::free(pointer); }
void operator delete[](void* pointer) noexcept { std::free(pointer); }
void operator delete(void* pointer, std::size_t) noexcept { std::free(pointer); }
void operator delete[](void* pointer, std::size_t) noexcept { std::free(pointer); }

using namespace rr::game;
using U = std::uint32_t;
using U64 = std::uint64_t;
using U16 = std::uint16_t;
namespace original = clock49j_sampler_oracle;
static original::Oracle oracle;
static std::array<U64, original::Oracle::word_count> sourceHits{};
static U64 checks = 0, sampleCalls = 0, quantizationCalls = 0, composedCalls = 0;
static U64 helperCalls = 0, sourceSampleCalls = 0, sourceInstructions = 0, maxSourceInstructions = 0;
static U64 rejections = 0, randomSamples = 0, randomCounters = 0;
static U64 saturationCounter = 0, diagnosticCounter = 0;
static const char* stage = "startup";

static void require(bool condition, const char* reason) {
    ++checks;
    if (!condition) throw std::runtime_error(reason);
}
static U bits(const float& value) { U result; std::memcpy(&result, &value, 4); return result; }
static U randomWord(U& state) {
    state ^= state << 13; state ^= state >> 17; state ^= state << 5;
    return state;
}
static U64 randomLong(U& state) {
    const auto high = randomWord(state);
    return (U64{high} << 32) | randomWord(state);
}
static void collectSource() {
    sourceInstructions += oracle.steps;
    if (oracle.steps > maxSourceInstructions) maxSourceInstructions = oracle.steps;
    for (unsigned i = 0; i < sourceHits.size(); ++i) sourceHits[i] += oracle.hits[i];
}
static bool sameClock(const ClockCounterState49J& a, const ClockCounterState49J& b) {
    // The 16-byte state's padding is deliberately never compared.
    return a.accumulatedTicks == b.accumulatedTicks &&
        a.pendingOverflows == b.pendingOverflows && a.previousCount == b.previousCount;
}
static bool sameQuantization(const ClockQuantization49J& a, const ClockQuantization49J& b) {
    return a.roundedTickBits == b.roundedTickBits &&
        a.scaledMillisecondBits == b.scaledMillisecondBits && a.uint32timestamp == b.uint32timestamp;
}
static bool sameOutput(const ClockFrameOutput49J& a, const ClockFrameOutput49J& b) {
    return a.deltaCounterBits == b.deltaCounterBits && sameQuantization(a.quantization, b.quantization) &&
        a.elapsed.deltaMilliseconds == b.elapsed.deltaMilliseconds &&
        bits(a.elapsed.elapsedSeconds) == bits(b.elapsed.elapsedSeconds);
}
static std::array<U, 7> stateWords(const FrameTimingState49I& state) {
    return {{state.current230, state.previous234, bits(state.raw250), bits(state.lastDispatched23C),
        state.phase22C, state.active11C, state.pending120}};
}
static frame49i_oracle::State sourceState(const FrameTimingState49I& state) {
    return {state.current230, state.previous234, bits(state.raw250), bits(state.lastDispatched23C),
        state.phase22C, state.active11C, state.pending120};
}
static bool sameFrame(const FrameTimingState49I& actual, const frame49i_oracle::State& wanted) {
    return stateWords(actual) == std::array<U, 7>{{wanted.current230, wanted.previous234,
        wanted.raw250, wanted.lastDispatched23C, wanted.phase22C, wanted.active11C, wanted.pending120}};
}
static FrameTimingState49I frameSentry() {
    // Construct raw sentry lanes without a floating return/initialization:
    // MSVC may quiet a signaling NaN before the production call sees it.
    const std::array<U,7> words{{0x12345678u, 1u, 0x7f812345u, 0xff854321u,
        0xa5a5a5a5u, 0x01234567u, 0xfedcba98u}};
    FrameTimingState49I result;
    static_assert(sizeof result == sizeof words);
    std::memcpy(&result, words.data(), sizeof result);
    return result;
}
static ClockFrameOutput49J outputSentry() {
    ClockFrameOutput49J result{0xdeadbeefu, {0x7fa12345u, 0xffa54321u, 0xabcdabcdu}, {0x76543210u, 0}};
    const U word=0x7f812345u;
    std::memcpy(&result.elapsed.elapsedSeconds, &word, sizeof word);
    return result;
}

// Separate mathematical evaluation of the source recurrence: widen to signed
// 64-bit first, then reduce the result to a low word and sign-extend explicitly.
// The maximum intermediate is <2^48, so no signed overflow is possible.
static U referenceSample(ClockCounterState49J& state, U16 sample) {
    std::int64_t periods = state.pendingOverflows;
    if (sample < state.previousCount && periods == 0) periods = 1;
    const std::int64_t wide = periods * 65536 + std::int64_t{sample} - state.previousCount;
    const U low = static_cast<U>(wide);
    const std::int64_t signedDelta = low < 0x80000000u ? std::int64_t{low}
        : std::int64_t{low} - 0x100000000ll;
    state.accumulatedTicks += static_cast<U64>(signedDelta);
    state.pendingOverflows = 0;
    state.previousCount = sample;
    return low;
}
static U sample(ClockCounterState49J& state, U16 current) {
    const auto before = allocations;
    const U delta = consumeTimerSample49J(state, current);
    const auto after = allocations;
    ++sampleCalls;
    require(before == after, "sample hot call allocated");
    return delta;
}
static ClockQuantization49J quantize(U64 counter, U16 current = 0, unsigned poison = 0xa5) {
    diagnosticCounter = counter;
    const auto wanted = oracle.quantize(counter, current, static_cast<std::uint8_t>(poison));
    collectSource();
    ClockQuantization49J result{0x12345678u, 0x9abcdef0u, 0xdeadbeefu};
    const auto before = allocations;
    const bool accepted = quantizeCounter49J(counter, result);
    const auto after = allocations;
    ++quantizationCalls;
    require(accepted && before == after, "quantization rejected nearest-even or allocated");
    require(result.roundedTickBits == wanted.roundedTickBits, "original counter helper differential");
    require(result.scaledMillisecondBits == wanted.scaledMillisecondBits, "original binary32 scale differential");
    require(result.uint32timestamp == wanted.uint32timestamp, "original uint32 truncation/saturation differential");
    return result;
}
static void compareSourceSample(const ClockCounterState49J& initial, U16 current,
    U actualDelta, const ClockCounterState49J& actualState, unsigned poison = 0xa5) {
    const auto wanted = oracle.sampleAndQuantize({initial.accumulatedTicks,
        initial.pendingOverflows, initial.previousCount}, current, static_cast<std::uint8_t>(poison));
    collectSource();
    ++sourceSampleCalls;
    require(actualDelta == wanted.delta32 && sameClock(actualState,
        {wanted.state.accumulatedTicks, wanted.state.pendingOverflows, wanted.state.previousCount}),
        "original supplied-sample words differ from native delta/state");
    require(oracle.currentReads == 1 && oracle.deltaObservations == 1 && oracle.fieldWrites == 4,
        "original supplied-sample read/store evidence");
}

static void directedSamples() {
    stage = "23 source-derived supplied-sample cases";
    struct Case { const char* name; U64 accumulator; U pending; U16 previous, current; U delta; U64 after; };
    // Frozen mathematical fixtures from sample-source-investigation.json.
    // Each is also checked against a separate supplied-sample original-word
    // interpreter below. Neither check reads actual MMIO or runtime RAM.
    constexpr Case cases[]{
        {"all_zero", 0x0000000000000000ull, 0x00000000u, 0u, 0u, 0x00000000u, 0x0000000000000000ull},
        {"ordinary_increase", 0x00000000000003e8ull, 0x00000000u, 100u, 125u, 0x00000019u, 0x0000000000000401ull},
        {"stalled_equal_sample", 0x00000000000003e8ull, 0x00000000u, 100u, 100u, 0x00000000u, 0x00000000000003e8ull},
        {"max_u16_increase", 0x0000000000000000ull, 0x00000000u, 0u, 65535u, 0x0000ffffu, 0x000000000000ffffull},
        {"wrap_without_pending_irq", 0x00000000000003e8ull, 0x00000000u, 65535u, 0u, 0x00000001u, 0x00000000000003e9ull},
        {"wrap_with_one_pending_irq", 0x00000000000003e8ull, 0x00000001u, 65535u, 0u, 0x00000001u, 0x00000000000003e9ull},
        {"decrease_zero_correction_max_delta", 0x0000000000000000ull, 0x00000000u, 1u, 0u, 0x0000ffffu, 0x000000000000ffffull},
        {"decrease_with_multiple_overflows", 0x00000000000003e8ull, 0x00000003u, 60000u, 10000u, 0x00023cb0u, 0x0000000000024098ull},
        {"equal_sample_one_full_period", 0x0000000000000000ull, 0x00000001u, 7u, 7u, 0x00010000u, 0x0000000000010000ull},
        {"increasing_sample_multiple_overflows", 0x0000000000000000ull, 0x00000003u, 100u, 125u, 0x00030019u, 0x0000000000030019ull},
        {"largest_positive_delta", 0x0000000000000000ull, 0x00007fffu, 0u, 65535u, 0x7fffffffu, 0x000000007fffffffull},
        {"signed_delta_minimum", 0x0000000080000000ull, 0x00008000u, 0u, 0u, 0x80000000u, 0x0000000000000000ull},
        {"same_overflow_decrease_stays_positive", 0x0000000000000000ull, 0x00008000u, 1u, 0u, 0x7fffffffu, 0x000000007fffffffull},
        {"signed_delta_negative_one", 0x0000000000000000ull, 0x0000ffffu, 0u, 65535u, 0xffffffffu, 0xffffffffffffffffull},
        {"overflow_shift_discards_upper16", 0x00000000000003e8ull, 0x00010000u, 100u, 125u, 0x00000019u, 0x0000000000000401ull},
        {"nonzero_discarded_overflow_disables_correction", 0x00000000000186a0ull, 0x00010000u, 65535u, 0u, 0xffff0001u, 0x00000000000086a1ull},
        {"overflow_word_high_bit_equal", 0x0000000000000063ull, 0x80000000u, 0u, 0u, 0x00000000u, 0x0000000000000063ull},
        {"overflow_word_high_bit_decrease", 0x0000000000000000ull, 0x80000000u, 1u, 0u, 0xffffffffu, 0xffffffffffffffffull},
        {"all_one_overflow_increase", 0x0000000000000000ull, 0xffffffffu, 0u, 65535u, 0xffffffffu, 0xffffffffffffffffull},
        {"all_one_overflow_decrease", 0x00000000000186a0ull, 0xffffffffu, 1u, 0u, 0xfffeffffu, 0x000000000000869full},
        {"accumulator_unsigned64_wrap_forward", 0xfffffffffffffff6ull, 0x00000000u, 100u, 125u, 0x00000019u, 0x000000000000000full},
        {"accumulator_unsigned64_wrap_backward", 0x0000000000000000ull, 0x00010000u, 1u, 0u, 0xffffffffu, 0xffffffffffffffffull},
        {"maximum_decreasing_difference", 0x0000000000000000ull, 0x00000000u, 65535u, 0u, 0x00000001u, 0x0000000000000001ull},
    };
    static_assert(sizeof(cases) / sizeof(cases[0]) == 23, "source fixture count");
    for (const auto& item : cases) {
        ClockCounterState49J state{item.accumulator, item.pending, item.previous};
        const auto initial = state;
        const U delta = sample(state, item.current);
        require(delta == item.delta, item.name);
        require(sameClock(state, {item.after, 0u, item.current}), item.name);
        compareSourceSample(initial, item.current, delta, state);
    }
    U seed = 0x49a7d219u;
    for (unsigned i = 0; i < 250000; ++i) {
        const U64 accumulator = randomLong(seed);
        const U pending = i % 8 == 0 ? 0u : randomWord(seed);
        const U16 previous = static_cast<U16>(randomWord(seed));
        const U16 current = i % 8 == 1 ? previous : static_cast<U16>(randomWord(seed));
        ClockCounterState49J state{accumulator, pending, previous}, expected = state;
        const auto initial = state;
        const U wanted = referenceSample(expected, current);
        const U actualDelta = sample(state, current);
        require(actualDelta == wanted && sameClock(state, expected), "bounded random sample recurrence");
        if (i < 16384) compareSourceSample(initial, current, actualDelta, state, i & 255u);
        ++randomSamples;
    }
}

static void counterDifferentials() {
    stage = "original counter and full quantization differentials";
    for (U64 counter = 0; counter < 4096; ++counter) quantize(counter, static_cast<U16>(counter));
    require(quantize(575).uint32timestamp == 0 && quantize(576).uint32timestamp == 1 &&
        quantize(576).scaledMillisecondBits == 0x3f800000u, "first millisecond boundary");
    for (unsigned power = 0; power < 64; ++power) {
        const U64 center = U64{1} << power;
        for (U64 distance : {0ull, 1ull, 2ull, 3ull, 7ull, 15ull, 127ull, 255ull,
                             511ull, 1023ull, 1024ull, 2047ull, 2048ull, 2049ull, 4095ull}) {
            if (center >= distance) quantize(center - distance);
            if (distance <= std::numeric_limits<U64>::max() - center) quantize(center + distance);
        }
    }
    // Inspect binary32 midpoint neighborhoods, binary64 half-ulp neighborhoods,
    // low-11 sticky behavior and unsigned-wrapper halving at each large binade.
    for (unsigned power = 53; power < 64; ++power) {
        const U64 base = U64{1} << power;
        const U64 ulp = U64{1} << (power - 23);
        for (U64 significand : {0ull, 1ull, 2ull, 3ull, 511ull, 1023ull, 0x3fffffull}) {
            const U64 midpoint = base + significand * ulp + (ulp >> 1);
            for (U64 distance : {0ull, 1ull, 2ull, 3ull, 15ull, 511ull, 1023ull,
                                 1024ull, 1025ull, 2047ull, 2048ull, 2049ull, 4095ull, 4096ull}) {
                if (midpoint >= distance) quantize(midpoint - distance);
                if (distance <= std::numeric_limits<U64>::max() - midpoint) quantize(midpoint + distance);
            }
        }
    }
    for (const U64 counter : {9007200865353727ull, 9007200865351679ull}) {
        const auto result = quantize(counter);
        require(result.roundedTickBits == 0x5a000002u && result.uint32timestamp == 0xffffffffu,
            "original sticky/double-rounding counterexample was lost");
        const U direct = frame49i_oracle::detail::roundDyadic(counter, 0);
        require(direct == 0x5a000001u && direct != result.roundedTickBits,
            "counterexample did not distinguish direct ideal conversion");
    }
    const auto plateau = quantize(U64{1} << 24);
    require(sameQuantization(plateau, quantize((U64{1} << 24) + 1)), "binary32 tick plateau");
    require(quantize((U64{1} << 24) + 2).roundedTickBits != plateau.roundedTickBits, "binary32 plateau end");
    require(quantize(std::numeric_limits<U64>::max()).roundedTickBits == 0x5f800000u,
        "maximum counter must round to exact 2^64");
    U seed = 0xb752149au;
    for (unsigned i = 0; i < 50000; ++i) {
        U64 counter = randomLong(seed);
        if (i % 4 == 0) counter &= (U64{1} << 43) - 1;
        const auto result = quantize(counter, static_cast<U16>(i), i & 255u);
        if (i % 64 == 0) {
            const U converted = oracle.convert(counter, static_cast<std::uint8_t>(255u - (i & 255u)));
            collectSource();
            ++helperCalls;
            require(converted == result.roundedTickBits, "separate helper-entry differential");
        }
        ++randomCounters;
    }
}

static void saturationBoundary() {
    stage = "first saturated timestamp and neighbors";
    // Saturation occurs below 2^53, where the source conversion is monotonic.
    U64 low = 0, high = U64{1} << 53;
    while (low + 1 < high) {
        const U64 middle = low + (high - low) / 2;
        if (quantize(middle).uint32timestamp == 0xffffffffu) high = middle;
        else low = middle;
    }
    saturationCounter = high;
    require(high == 2473901031424ull, "first saturated integer-counter boundary");
    // Tick rounding followed by scaling skips 0x4f7fffff at this boundary.
    require(quantize(low).scaledMillisecondBits == 0x4f7ffffeu &&
        quantize(low).uint32timestamp == 0xfffffe00u, "last reachable unsaturated millisecond bin");
    require(quantize(high).scaledMillisecondBits == 0x4f800000u &&
        quantize(high).uint32timestamp == 0xffffffffu, "first saturated binary32 millisecond bin");
    for (U64 counter = high - 4096; counter <= high + 4096; ++counter) quantize(counter);
}

static void composedDifferentials() {
    stage = "supplied sample to original 49I begin composition";
    U seed = 0x914ac017u;
    ClockCounterState49J clock{0u, 0u, 0u};
    auto frame = frameSentry();
    require(bits(frame.raw250)==0x7f812345u && bits(frame.lastDispatched23C)==0xff854321u,
        "signaling-NaN sentry construction changed before production call");
    frame.previous234 = 0;
    for (unsigned i = 0; i < 8192; ++i) {
        if (i % 127 == 0) frame.previous234 = 0;
        if (i % 257 == 0) clock.accumulatedTicks = randomLong(seed);
        else if (i % 11 == 0) clock.accumulatedTicks = saturationCounter - 50000;
        clock.pendingOverflows = i % 5 == 0 ? randomWord(seed) : 0u;
        const U16 current = i % 7 == 0 ? clock.previousCount : static_cast<U16>(randomWord(seed));
        const U cap = i % 3 == 0 ? 0u : randomWord(seed);
        auto expectedClock = clock;
        const U expectedDelta = referenceSample(expectedClock, current);
        const auto sourceSample = oracle.sampleAndQuantize({clock.accumulatedTicks,
            clock.pendingOverflows, clock.previousCount}, current);
        collectSource();
        ++sourceSampleCalls;
        require(sourceSample.delta32 == expectedDelta && sameClock(expectedClock,
            {sourceSample.state.accumulatedTicks, sourceSample.state.pendingOverflows,
             sourceSample.state.previousCount}), "source sample versus widened recurrence");
        const auto expectedQuantization = sourceSample.quantized;
        auto expectedFrame = sourceState(frame);
        const auto expectedElapsed = frame49i_oracle::begin(expectedFrame, expectedQuantization.uint32timestamp, cap);
        auto output = outputSentry();
        const auto before = allocations;
        const bool accepted = beginFrameFromTimerSample49J(clock, frame, current, cap, output);
        const auto after = allocations;
        ++composedCalls;
        require(accepted && before == after, "composed begin rejected nearest-even or allocated");
        require(sameClock(clock, expectedClock) && output.deltaCounterBits == expectedDelta,
            "composed supplied-sample state/delta");
        require(sameQuantization(output.quantization, {expectedQuantization.roundedTickBits,
            expectedQuantization.scaledMillisecondBits, expectedQuantization.uint32timestamp}), "composed quantization");
        require(sameFrame(frame, expectedFrame), "composed original frame state");
        require(output.elapsed.deltaMilliseconds == expectedElapsed.deltaMilliseconds &&
            bits(output.elapsed.elapsedSeconds) == expectedElapsed.elapsedBits, "composed original frame factor");
        require(bits(frame.lastDispatched23C) == 0xff854321u, "unconsumed signaling-NaN lane changed");
        // The actual manager and callbacks are external. This synthetic check
        // exercises only existing live-tail state, never a fabricated callback.
        finishFrameTiming49I(frame);
    }
}

static void roundingAdmission() {
    stage = "all four host rounding modes and atomic rejection";
    for (const int mode : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
        require(std::fesetround(mode) == 0, "host rounding mode unavailable");
        for (const U64 counter : {0ull, 1ull, 575ull, 576ull, 0xffffffffull,
                                 9007200865353727ull, 0xffffffffffffffffull}) {
            ClockQuantization49J output{0x12345678u, 0x87654321u, 0xabcdef01u};
            const auto initial = output;
            const auto before = allocations;
            const bool accepted = quantizeCounter49J(counter, output);
            const auto after = allocations;
            ++quantizationCalls;
            require(before == after && accepted == (mode == FE_TONEAREST), "quantizer rounding admission");
            if (!accepted) {
                require(sameQuantization(initial, output), "rejected quantizer changed output");
                ++rejections;
            } else {
                const auto wanted = oracle.quantize(counter);
                collectSource();
                require(sameQuantization(output, {wanted.roundedTickBits, wanted.scaledMillisecondBits,
                    wanted.uint32timestamp}), "nearest-even admission quantization");
            }
            ClockCounterState49J clock{counter, 0xdead8000u, 65535u};
            const auto originalClock = clock;
            auto frame = frameSentry();
            const auto originalFrameWords = stateWords(frame);
            auto composed = outputSentry();
            const auto originalOutput = composed;
            const auto beforeComposed = allocations;
            const bool composedAccepted = beginFrameFromTimerSample49J(clock, frame, 0u, 0x80000000u, composed);
            const auto afterComposed = allocations;
            ++composedCalls;
            require(beforeComposed == afterComposed && composedAccepted == (mode == FE_TONEAREST),
                "composed rounding admission");
            if (!composedAccepted) {
                require(sameClock(clock, originalClock) && stateWords(frame) == originalFrameWords &&
                    sameOutput(composed, originalOutput), "unsupported rounding was not atomic across all objects");
                ++rejections;
            }
            auto sampled = originalClock;
            auto wanted = originalClock;
            const U delta = referenceSample(wanted, 0u);
            require(sample(sampled, 0u) == delta && sameClock(sampled, wanted), "integer sampler depends on host rounding");
        }
    }
    require(std::fesetround(FE_TONEAREST) == 0, "restore nearest-even failed");
}

static void writeCoverage(std::ostream& out) {
    unsigned visited = 0;
    for (auto count : sourceHits) if (count) ++visited;
    out << "{\n  \"status\": \"passed\",\n  \"checks\": " << checks
        << ",\n  \"directed_sample_cases\": 23,\n  \"random_sample_cases\": " << randomSamples
        << ",\n  \"sample_hot_calls\": " << sampleCalls
        << ",\n  \"random_counter_cases\": " << randomCounters
        << ",\n  \"quantization_hot_calls\": " << quantizationCalls
        << ",\n  \"composed_hot_calls\": " << composedCalls
        << ",\n  \"separate_helper_oracle_calls\": " << helperCalls
        << ",\n  \"original_sample_and_quantize_calls\": " << sourceSampleCalls
        << ",\n  \"atomic_rejections\": " << rejections
        << ",\n  \"first_saturated_counter\": " << saturationCounter
        << ",\n  \"hot_call_allocations\": 0,\n  \"original_quantization_instructions\": " << sourceInstructions
        << ",\n  \"maximum_instructions_one_call\": " << maxSourceInstructions
        << ",\n  \"frozen_original_words\": " << sourceHits.size()
        << ",\n  \"visited_original_words\": " << visited
        << ",\n  \"sampler_validation\": \"23 static source-derived fixtures,250000 widened-integer recurrence cases,and24599 original-word sample-and-quantize cases; supplied COUNT only,no hardware or runtime RAM\","
        << "\n  \"coverage\": [\n";
    unsigned index = 0;
    bool comma = false;
    for (const auto& block : original::blocks) {
        for (U pc = block.start; pc < block.end; pc += 4, ++index) {
            if (comma) out << ",\n";
            out << "    {\"pc\": " << pc << ", \"hits\": " << sourceHits[index] << "}";
            comma = true;
        }
    }
    out << "\n  ]\n}\n";
}

int main(int argc, char** argv) {
    try {
        require(std::fesetround(FE_TONEAREST) == 0, "initial nearest-even setup failed");
        directedSamples();
        counterDifferentials();
        saturationBoundary();
        composedDifferentials();
        roundingAdmission();
        require(original::Oracle::word_count == 809, "unexpected frozen sample/quantization closure size");
        require(sourceSampleCalls == 24599, "unexpected original-word sample case count");
        unsigned sampledWords = 0;
        for (unsigned i = 772; i < sourceHits.size(); ++i) if (sourceHits[i]) ++sampledWords;
        require(sampledWords == 36 && sourceHits[772 + (0x140b84u - 0x140b30u) / 4] == 0,
            "sample slice coverage differs from 36 reachable words and one unreachable subtraction");
        if (argc != 1) {
            require(argc == 3 && std::strcmp(argv[1], "--coverage-json") == 0,
                "usage: clock_quantization49j [--coverage-json PATH]");
            std::ofstream file(argv[2]);
            require(bool(file), "cannot open coverage output");
            writeCoverage(file);
            file.close();
            require(bool(file), "cannot write coverage output");
        }
        std::cout << "clock_quantization49j: " << checks << " checks passed; "
            << sampleCalls << " sample, " << quantizationCalls << " quantization, "
            << composedCalls << " composed hot calls; " << rejections << " atomic rejections; "
            << sourceInstructions << " original quantization instructions; first saturated counter "
            << saturationCounter << "; zero hot-call allocations\n";
        return 0;
    } catch (const std::exception& error) {
        std::fesetround(FE_TONEAREST);
        std::cerr << "clock_quantization49j FAILED at " << stage << " (counter " << diagnosticCounter
            << "): " << error.what() << '\n';
        return 1;
    }
}
