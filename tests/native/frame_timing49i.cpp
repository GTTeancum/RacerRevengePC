#include "../../src/game/frame_timing49i.h"
#include "../../src/game/control_shaping48v.h"
#include "../../tools/gameplay/input_shaping_oracle48v.h"
#include "frame_timing_oracle49i.h"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <new>
#include <stdexcept>

// Count successful hot-call allocations only. No allocator-failure, ownership,
// lifetime, address-fault, alignment or foreign-memory probes belong here.
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
using State = FrameTimingState49I;
using Elapsed = FrameElapsed49I;
using Entry = FrameSimulationEntry49I;
namespace original = frame49i_oracle;
static U64 checks = 0, begins = 0, entries = 0, resets = 0, leaves = 0, finishes = 0;
static U64 rejected = 0, trajectoryFrames = 0, consumerFrames = 0, consumerInstructions = 0;
static const char* stage = "startup";
static void require(bool condition, const char* reason) {
    ++checks;
    if (!condition) throw std::runtime_error(reason);
}
static U bits(float value) { U word; std::memcpy(&word, &value, 4); return word; }
static float fp(U word) { float value; std::memcpy(&value, &word, 4); return value; }
template<class T> static std::array<unsigned char, sizeof(T)> snapshot(const T& object) {
    std::array<unsigned char, sizeof(T)> result{};
    std::memcpy(result.data(), &object, sizeof(T));
    return result;
}
static State sentry() {
    return {0xabcdefffu, 1u, fp(0x7fc12345u), fp(0xffc54321u),
            0xa5a5a5a5u, 0x12345678u, 0xfedcba98u};
}
static original::State sourceState(const State& state) {
    return {state.current230, state.previous234, bits(state.raw250),
            bits(state.lastDispatched23C), state.phase22C, state.active11C, state.pending120};
}
static void equalSourceState(const State& state, const original::State& wanted) {
    require(snapshot(sourceState(state)) == snapshot(wanted),
            "native state differs from original-word execution");
}
static void sourceStore(unsigned index, U pc, U offset, U word) {
    require(index < original::lastStoreTrace.size(), "missing original store trace");
    const auto& store = original::lastStoreTrace[index];
    require(store.pc == pc && store.offset == offset && store.bits == word,
            "original field-store order/value differs");
}
static U randomWord(U& seed) {
    seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
    return seed;
}

// Independent, integer-only nearest-even arithmetic check. This is not the
// separate original-word interpreter: round the mathematical unsigned integer
// to 24 significant bits, then multiply its exact integer by 0x83126f / 2^33.
// It shares no production float conversion, sticky-half path or cap helper.
static unsigned highestBit(U64 value) {
    unsigned result = 0;
    while (value >>= 1) ++result;
    return result;
}
static U64 nearestEvenShift(U64 value, unsigned shift) {
    if (!shift) return value;
    const U64 quotient = value >> shift;
    const U64 remainder = value & ((U64{1} << shift) - 1);
    const U64 half = U64{1} << (shift - 1);
    return quotient + (remainder > half || (remainder == half && (quotient & 1)));
}
static U referenceRaw(U delta) {
    if (!delta) return 0;
    const unsigned high = highestBit(delta);
    const unsigned discard = high > 23 ? high - 23 : 0;
    const U64 roundedInteger = nearestEvenShift(delta, discard) << discard;
    const U64 numerator = roundedInteger * U64{0x83126f};
    unsigned top = highestBit(numerator);
    U64 significand = nearestEvenShift(numerator, top - 23);
    if (significand == (U64{1} << 24)) { significand >>= 1; ++top; }
    return U((top + 94) << 23) | U(significand & 0x7fffffu);
}
static U referenceElapsed(U raw, U capWord) {
    U result = raw < 0x3d75c28fu ? raw : 0x3d75c28fu;
    if (capWord && result > 0x3d08850au) result = 0x3d08850au;
    return result;
}

static Elapsed begin(State& state, U stamp, U capWord) {
    const bool bootstrap = !state.previous234;
    auto source = sourceState(state);
    const auto expectedSource = original::begin(source, stamp, capWord);
    require(original::lastStoreTrace.size() == (bootstrap ? 3u : 2u),
            "original begin store count differs");
    if (bootstrap) {
        sourceStore(0, 0x140e38u, 0x234u, stamp);
        sourceStore(1, 0x140e3cu, 0x230u, stamp);
    } else sourceStore(0, 0x140be0u, 0x230u, stamp);
    sourceStore(bootstrap ? 2u : 1u, 0x140c50u, 0x250u, source.raw250);
    State wanted = state;
    wanted.current230 = stamp;
    if (!wanted.previous234) wanted.previous234 = stamp;
    const U delta = wanted.current230 - wanted.previous234;
    const U raw = referenceRaw(delta);
    wanted.raw250 = fp(raw);
    Elapsed output{0x1234abcdu, fp(0x7fa23456u)};
    const auto before = allocations;
    const bool accepted = beginFrameTiming49I(state, stamp, capWord, output);
    const auto after = allocations;
    ++begins;
    require(accepted, "nearest-even timestamp rejected");
    require(before == after, "begin allocated");
    require(snapshot(state) == snapshot(wanted), "begin state differs from exact rational check");
    equalSourceState(state, source);
    require(output.deltaMilliseconds == delta, "modulo delta differs");
    require(bits(output.elapsedSeconds) == referenceElapsed(raw, capWord), "capped elapsed differs");
    require(output.deltaMilliseconds == expectedSource.deltaMilliseconds &&
            bits(output.elapsedSeconds) == expectedSource.elapsedBits,
            "begin output differs from original-word execution");
    return output;
}
static Entry enter(State& state, U flags, float elapsed) {
    auto source = sourceState(state);
    const bool sourceSkips = original::gate(flags);
    const auto before = allocations;
    const Entry result = enterFrameSimulation49I(state, flags, elapsed);
    const auto after = allocations;
    ++entries;
    require(before == after, "entry allocated");
    require((result == Entry::Skipped) == sourceSkips, "live gate differs from original words");
    if (result == Entry::Ready) {
        original::enter(source, bits(elapsed));
        require(original::lastStoreTrace.size() == 2, "original entry store count differs");
        sourceStore(0, 0x140ce0u, 0x22cu, 2);
        sourceStore(1, 0x140ce4u, 0x23cu, bits(elapsed));
    }
    equalSourceState(state, source);
    return result;
}
static void reset(State& state, U stamp) {
    auto source = sourceState(state);
    original::reset(source, stamp);
    require(original::lastStoreTrace.size() == 2, "original reset store count differs");
    sourceStore(0, 0x140e38u, 0x234u, stamp);
    sourceStore(1, 0x140e3cu, 0x230u, stamp);
    State wanted = state; wanted.previous234 = stamp; wanted.current230 = stamp;
    const auto before = allocations;
    resetFrameTimestamps49I(state, stamp);
    const auto after = allocations;
    ++resets;
    require(before == after, "reset allocated");
    require(snapshot(state) == snapshot(wanted), "reset touched a non-timestamp field");
    equalSourceState(state, source);
}
static void leave(State& state) {
    auto source = sourceState(state);
    original::leave(source);
    require(original::lastStoreTrace.size() == 1, "original leave store count differs");
    sourceStore(0, 0x140cfcu, 0x22cu, 0);
    State wanted = state; wanted.phase22C = 0;
    const auto before = allocations;
    leaveFrameSimulation49I(state);
    const auto after = allocations;
    ++leaves;
    require(before == after, "leave allocated");
    require(snapshot(state) == snapshot(wanted), "leave changed more than phase");
    equalSourceState(state, source);
}
static void finish(State& state) {
    auto source = sourceState(state);
    const bool activate = state.pending120 != 0;
    original::finish(source);
    require(original::lastStoreTrace.size() == (activate ? 3u : 1u),
            "original tail store count differs");
    sourceStore(0, 0x140d30u, 0x234u, state.current230);
    if (activate) {
        sourceStore(1, 0x140d44u, 0x11cu, 1);
        sourceStore(2, 0x140d48u, 0x120u, 0);
    }
    State wanted = state; wanted.previous234 = wanted.current230;
    if (wanted.pending120) { wanted.active11C = 1; wanted.pending120 = 0; }
    const auto before = allocations;
    finishFrameTiming49I(state);
    const auto after = allocations;
    ++finishes;
    require(before == after, "finish allocated");
    require(snapshot(state) == snapshot(wanted), "finish live state differs");
    equalSourceState(state, source);
}

static void oracleIntegerBoundaries() {
    stage = "integer oracle high-word and rounding boundaries";
    for(unsigned bit=0;bit<64;++bit) {
        const U64 value=U64{1}<<bit;
        require(original::detail::roundDyadic(value,0)==U((bit+127u)<<23),
            "oracle exact power-of-two exponent");
        require(highestBit(value)==bit,"independent shift-before-test highest bit");
    }
    for(unsigned shift=1;shift<=40;++shift)for(U64 base:{0x800000ull,0x800001ull}) {
        const U64 halfway=(base<<shift)+(U64{1}<<(shift-1));
        const U baseWord=0x4b000000u+U(base-0x800000ull);
        require(original::detail::roundDyadic(halfway-1,-int(shift))==baseWord,"oracle below midpoint");
        require(original::detail::roundDyadic(halfway,-int(shift))==baseWord+U(base&1),"oracle nearest-even midpoint");
        require(original::detail::roundDyadic(halfway+1,-int(shift))==baseWord+1u,"oracle above midpoint");
    }
    require(original::detail::multiply(0x41800000u,0x3a83126fu)==0x3c83126fu,
        "source16 millisecond product exact word");
    require(original::detail::roundDyadic(9007200865353727ull,0)==0x5a000001u,
        "direct ideal conversion differs from source sticky conversion");
    require(original::detail::roundDyadic(~U64{0},0)==0x5f800000u,"oracle carry into2^64");
    require(original::detail::roundDyadic(1,-149)==1u&&original::detail::roundDyadic(1,-150)==0u&&
        original::detail::roundDyadic(3,-150)==2u,"oracle subnormal midpoint ties");
}
static void directedNumeric() {
    stage = "numeric directed";
    const std::array<std::array<U, 2>, 24> anchors{{
        {{0x00000000u, 0x00000000u}}, {{0x00000001u, 0x3a83126fu}},
        {{0x00000010u, 0x3c83126fu}}, {{0x00000020u, 0x3d03126fu}},
        {{0x00000021u, 0x3d072b02u}}, {{0x00000022u, 0x3d0b4396u}},
        {{0x0000003bu, 0x3d71a9fdu}}, {{0x0000003cu, 0x3d75c290u}},
        {{0x0000003du, 0x3d79db24u}}, {{0x000003e8u, 0x3f800000u}},
        {{0x00ffffffu, 0x4683126eu}}, {{0x01000000u, 0x4683126fu}},
        {{0x01000001u, 0x4683126fu}}, {{0x01000003u, 0x46831271u}},
        {{0x7fffff7fu, 0x4a03126eu}}, {{0x7fffff80u, 0x4a03126eu}},
        {{0x7fffffffu, 0x4a03126fu}}, {{0x80000000u, 0x4a03126fu}},
        {{0x8000007fu, 0x4a03126fu}}, {{0x80000080u, 0x4a03126fu}},
        {{0x80000081u, 0x4a031270u}}, {{0xffffff7fu, 0x4a83126eu}},
        {{0xffffff80u, 0x4a83126fu}}, {{0xffffffffu, 0x4a83126fu}}
    }};
    for (const auto& anchor : anchors) {
        require(referenceRaw(anchor[0]) == anchor[1], "integer reference anchor differs");
        for (U capWord : {0u, 1u, 7u, 0x80000000u, 0xffffffffu}) {
            auto state = sentry();
            const auto output = begin(state, anchor[0] + 1u, capWord);
            require(bits(state.raw250) == anchor[1], "raw binary32 anchor differs");
            if (anchor[0] == 60u)
                require(bits(state.raw250) == 0x3d75c290u &&
                    bits(output.elapsedSeconds) == (capWord ? 0x3d08850au : 0x3d75c28fu),
                    "60ms must store raw one ULP above the ordinary cap");
        }
    }
    for (U stamp : {0u, 1u, 120u, 0x7fffffffu, 0x80000000u, 0xffffffffu}) {
        auto state = sentry(); state.previous234 = 0;
        const auto output = begin(state, stamp, 1);
        require(!output.deltaMilliseconds && !bits(output.elapsedSeconds) &&
                state.previous234 == stamp && state.current230 == stamp,
                "first timestamp did not seed both fields and return zero");
    }
    auto zero = sentry(); zero.previous234 = 0;
    begin(zero, 0, 0); begin(zero, 0, 0);
    require(!begin(zero, 19, 0).deltaMilliseconds, "zero sentinel did not persist");
    require(begin(zero, 20, 0).deltaMilliseconds == 1, "seeded next frame delta");
    auto wrapped = sentry(); wrapped.previous234 = 0xfffffff0u;
    require(begin(wrapped, 0x10u, 0).deltaMilliseconds == 32, "timestamp wrap");
    auto backwards = sentry(); backwards.previous234 = 100;
    require(begin(backwards, 99, 0).deltaMilliseconds == 0xffffffffu,
            "backward timestamp was incorrectly paused or clamped");
    auto saturated = sentry(); saturated.previous234 = 0xfffffff0u;
    begin(saturated, 0xffffffffu, 0); finish(saturated);
    require(!begin(saturated, 0xffffffffu, 0).deltaMilliseconds,
            "repeated supplied saturated timestamp did not produce zero");

    // Integer mantissa/tie boundaries on both conversion paths.
    for (unsigned power = 24; power <= 31; ++power) {
        const U unit = U{1} << (power - 23);
        for (U base : {U{1} << power, (U{1} << power) + unit, 0xffffffffu - unit}) {
            for (U offset = 0; offset <= 2 * unit + 2; ++offset) {
                auto state = sentry(); begin(state, base + offset + 1u, offset & 1u);
            }
        }
    }
    U seed = 0x49f0140au;
    for (unsigned n = 0; n < 20000; ++n) {
        auto state = sentry();
        state.previous234 = randomWord(seed) | 1u;
        const U stamp = randomWord(seed);
        const U capWord = randomWord(seed) & 3u;
        begin(state, stamp, capWord);
    }
}

static void directedStateAndAdmission() {
    stage = "state and admission";
    for (int mode : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
        require(std::fesetround(mode) == 0, "rounding-mode setup failed");
        for (U previous : {0u, 17u, 0xffffffffu}) {
            auto state = sentry(); state.previous234 = previous;
            Elapsed output{0xfeedabcdu, fp(0x7fa23456u)};
            const auto beforeState = snapshot(state);
            const auto beforeOutput = snapshot(output);
            const auto before = allocations;
            const bool accepted = beginFrameTiming49I(state, 31, 1, output);
            const auto after = allocations;
            ++begins;
            require(before == after, "rounding admission allocated");
            require(accepted == (mode == FE_TONEAREST), "source-entry rounding admission differs");
            if (!accepted) {
                ++rejected;
                require(snapshot(state) == beforeState && snapshot(output) == beforeOutput,
                        "unsupported rounding partially changed timestamps/state/output");
            }
        }
        // No rounded arithmetic occurs in these state-only operations.
        auto state = sentry(); reset(state, 0x1234u); leave(state); finish(state);
        for (U word : {0u, 0x80000000u, 1u, 0x007fffffu, 0x00800000u,
                       0x3d75c28eu, 0x3d75c28fu}) {
            state = sentry(); auto wanted = state;
            wanted.phase22C = 2; wanted.lastDispatched23C = fp(word);
            require(enter(state, 0, fp(word)) == Entry::Ready, "valid factor rejected");
            require(snapshot(state) == snapshot(wanted), "ready state or signed-zero copy differs");
        }
        for (U word : {0x3d75c290u, 0x3f800000u, 0x80000001u, 0xbf800000u,
                       0x7f800000u, 0xff800000u, 0x7fc12345u, 0x7fa12345u,
                       0xffc12345u, 0xffa12345u}) {
            state = sentry(); const auto before = snapshot(state);
            require(enter(state, 0, fp(word)) == Entry::InvalidElapsed,
                    "unsupported consumed elapsed accepted");
            ++rejected;
            require(snapshot(state) == before, "invalid factor changed state");
            require(std::feclearexcept(FE_ALL_EXCEPT) == 0, "exception clear failed");
            require(enter(state, 8, fp(word)) == Entry::Skipped,
                    "bit 3 did not skip before factor admission");
            require(snapshot(state) == before, "skip touched state");
            require(!(std::fetestexcept(FE_INVALID)), "skip consumed invalid factor arithmetically");
        }
    }
    require(std::fesetround(FE_TONEAREST) == 0, "restore nearest-even failed");
    for (U word : {0x7fa12345u, 0xffa54321u}) {
        auto state = sentry();
        state.raw250 = fp(word); state.lastDispatched23C = fp(word);
        const auto before = snapshot(state);
        require(std::feclearexcept(FE_ALL_EXCEPT) == 0, "unused NaN exception clear failed");
        require(enter(state, 8, fp(word)) == Entry::Skipped && snapshot(state) == before,
                "skip consumed or changed unused signaling-NaN fields");
        require(!(std::fetestexcept(FE_INVALID)), "skip inspected unused signaling-NaN fields");
        begin(state, 18, 0);
        require(bits(state.lastDispatched23C) == word,
                "begin changed an unused signaling-NaN field");
        require(!(std::fetestexcept(FE_INVALID)), "begin inspected old signaling-NaN float fields");
        state.raw250 = fp(word);
        reset(state, 37); leave(state); finish(state);
        require(bits(state.raw250) == word && bits(state.lastDispatched23C) == word,
                "state-only stages changed signaling-NaN payloads");
        require(!(std::fetestexcept(FE_INVALID)), "state-only stages inspected signaling-NaN float fields");
    }
    for (unsigned bit = 0; bit < 32; ++bit) {
        auto state = sentry(); const auto before = snapshot(state);
        const auto result = enter(state, U{1} << bit, fp(0x3d000000u));
        require(result == (bit == 3 ? Entry::Skipped : Entry::Ready),
                "root flags changed a bit other than bit 3");
        if (bit == 3) require(snapshot(state) == before, "skipped flags changed state");
    }
    for (U pending : {0u, 1u, 7u, 0x80000000u, 0xffffffffu})
        for (U active : {0u, 1u, 7u, 0xffffffffu}) {
            auto state = sentry(); state.pending120 = pending; state.active11C = active;
            finish(state);
            require(state.active11C == (pending ? 1u : active), "tail activation is not nonzero-based");
        }
}

static void liveStageTrajectories() {
    stage = "live stage trajectories";
    // These explicit caller-owned edits model distinct stage observations, not
    // implementations or discoveries of the opaque manager/root/tail callbacks.
    State state{0, 0, 0, fp(0x80000000u), 1, 0, 0};
    U seed = 0x14ac0491u, stamp = 0xfffff000u;
    for (unsigned frame = 0; frame < 8192; ++frame) {
        if (frame % 47 == 0) reset(state, stamp);
        if (frame % 113 == 0) reset(state, 0);
        stamp += randomWord(seed) % 131u;
        const U capWord = frame % 3 ? 0u : 0x80000000u;
        const auto elapsed = begin(state, stamp, capWord);
        const U factorBeforeStages = bits(elapsed.elapsedSeconds);
        // The actual manager belongs here and receives the capped factor even
        // when its live root flag subsequently suppresses the root stage.
        const float managerInput = elapsed.elapsedSeconds;
        U liveRootFlags = frame % 5 ? 0x80u : 8u;
        if (frame % 7 == 0) liveRootFlags ^= 8u;
        if (frame % 11 == 0) reset(state, randomWord(seed));
        if (frame % 13 == 0) state.raw250 = fp(0x7fc12345u);
        state.pending120 = frame % 4 ? randomWord(seed) : 0u;
        const State beforeEntry = state;
        const auto result = enter(state, liveRootFlags, elapsed.elapsedSeconds);
        require(bits(managerInput) == factorBeforeStages, "manager factor changed after begin");
        if (liveRootFlags & 8u) {
            require(result == Entry::Skipped && snapshot(state) == snapshot(beforeEntry),
                    "post-manager skip overwrote live phase/factor");
        } else {
            require(result == Entry::Ready && state.phase22C == 2 &&
                    bits(state.lastDispatched23C) == factorBeforeStages,
                    "root entry did not use the saved capped factor");
            // A real root call belongs between enter and leave. Its caller can
            // observe phase=2 and mutate caller-owned state before returning.
            if (frame % 17 == 0) state.current230 = randomWord(seed);
            if (frame % 19 == 0) state.lastDispatched23C = fp(0x80000000u);
            state.phase22C = 0x2468ace0u;
            leave(state);
            require(!state.phase22C, "root-return phase clear missing");
            // App virtual/global stages happen after leave and may edit phase.
            if (frame % 23 == 0) state.phase22C = 0xdead0001u;
        }
        // Every path reaches opaque 267BE0 before finish. Supply live edits to
        // demonstrate that the tail cannot use a snapshot from begin/enter.
        if (frame % 29 == 0) reset(state, randomWord(seed));
        if (frame % 31 == 0) state.current230 = 0;
        if (frame % 37 == 0) state.pending120 = 0;
        if (frame % 41 == 0) { state.pending120 = 0x80000000u; state.active11C = 7; }
        finish(state);
        require(state.previous234 == state.current230 && state.pending120 == 0,
                "tail failed live timestamp/pending effects");
        require(bits(elapsed.elapsedSeconds) == factorBeforeStages,
                "state-only stages rewrote elapsed output");
        ++trajectoryFrames;
    }
}

static void capturedDecisionStages() {
    stage = "captured decision stages";
    auto state = sentry();
    U liveCapWord = 0;
    const auto elapsed = begin(state, 61, liveCapWord);
    require(bits(elapsed.elapsedSeconds) == 0x3d75c28fu, "uncapped-mode prepared factor");
    // Explicit caller-owned manager-stage edits, not a manager implementation.
    // The cap read has already happened; the root-flag read has not.
    liveCapWord = 1;
    U liveRootFlags = 0;
    const U capturedAfterManager = liveRootFlags;
    unsigned suppliedPreDispatchStages = 0;
    if (!(capturedAfterManager & 8u)) {
        ++suppliedPreDispatchStages;
        // Supply possible state edits from the already-admitted opaque action
        // that follows the captured bit decision but precedes enter's stores.
        liveRootFlags = 8;
        state.current230 = 1000;
        state.previous234 = 900;
        state.raw250 = fp(0x7fa12345u);
        state.phase22C = 0x13579bdfu;
        state.lastDispatched23C = fp(0xffa54321u);
        state.active11C = 7;
        state.pending120 = 0x80000000u;
    }
    auto wanted = state;
    wanted.phase22C = 2;
    wanted.lastDispatched23C = elapsed.elapsedSeconds;
    require(liveRootFlags == 8 && capturedAfterManager == 0 && suppliedPreDispatchStages == 1,
            "clear-capture fixture missed the later flag flip");
    require(enter(state, capturedAfterManager, elapsed.elapsedSeconds) == Entry::Ready,
            "later opaque-stage flag flip replaced the captured clear decision");
    require(snapshot(state) == snapshot(wanted), "entry overwrote opaque-stage state beyond source stores");
    require(bits(state.lastDispatched23C) == 0x3d75c28fu,
            "manager cap toggle changed an already-prepared factor");
    leave(state); finish(state);
    const auto next = begin(state, state.previous234 + 60u, liveCapWord);
    require(bits(next.elapsedSeconds) == 0x3d08850au,
            "later begin did not consume the changed cap word");

    state = sentry();
    state.raw250 = fp(0x7fa12345u);
    state.lastDispatched23C = fp(0xffa54321u);
    liveRootFlags = 8;
    const U capturedSetAfterManager = liveRootFlags;
    const auto before = snapshot(state);
    suppliedPreDispatchStages = 0;
    if (!(capturedSetAfterManager & 8u)) {
        ++suppliedPreDispatchStages;
        state.phase22C = 123;
    }
    require(std::feclearexcept(FE_ALL_EXCEPT) == 0, "captured-skip exception clear failed");
    require(enter(state, capturedSetAfterManager, fp(0x7fa56789u)) == Entry::Skipped,
            "captured set decision did not skip before factor consumption");
    require(!suppliedPreDispatchStages && snapshot(state) == before,
            "captured set decision performed a pre-dispatch action or state write");
    require(!(std::fetestexcept(FE_INVALID)), "captured skip consumed signaling-NaN factor");
}

static void connectedInputTrajectory() {
    stage = "connected synthetic input";
    State timing{0, 0, 0, 0, 1, 0, 0};
    ControlInputStep48V inputState{};
    U stamp = 0xfffff000u, seed = 0x49b2180au;
    const std::array<U, 10> increments{{0, 1, 8, 16, 17, 32, 33, 34, 60, 250}};
    for (unsigned frame = 0; frame < 4096; ++frame) {
        if (frame % 113 == 0) reset(timing, stamp);
        stamp += increments[frame % increments.size()];
        const auto elapsed = begin(timing, stamp, frame % 2);
        // Supplies numerical channels only. It does not invent device bindings,
        // gameplay ownership, an event scheduler or a world-query connection.
        ControlChannels48B raw{};
        for (float& value : raw.values)
            value = float(int(randomWord(seed) % 2049u) - 1024) / 1024.0f;
        const auto mode = frame % 2 ? ControlShapeMode48V::Nonlinear : ControlShapeMode48V::BypassNonlinear;
        const ControlRateLimits48B rates{4.0f, 3.0f};
        const U flags = frame % 17 == 0 ? 8u : 0u;
        const auto beforeInput = snapshot(inputState);
        const auto result = enter(timing, flags, elapsed.elapsedSeconds);
        if (result == Entry::Ready) {
            // The established source route forwards the factor unchanged. The
            // connection below is a synthetic numeric consumer at that boundary.
            original48v::Machine original;
            const auto wanted = original.run(raw, inputState.shaping.smoothedChannel0D28,
                frame % 2 ? 0u : 1u, inputState.filtered, rates, elapsed.elapsedSeconds);
            consumerInstructions += original.steps;
            ControlInputStep48V next{};
            const auto before = allocations;
            const bool accepted = stepControlInputs48V(raw,
                inputState.shaping.smoothedChannel0D28, mode,
                inputState.filtered, rates, elapsed.elapsedSeconds, next);
            const auto after = allocations;
            require(accepted && before == after, "connected 48V/48B input rejected or allocated");
            require(snapshot(next) == snapshot(wanted), "connected original-input-word differential");
            inputState = next;
            leave(timing);
            ++consumerFrames;
        } else {
            require(result == Entry::Skipped && snapshot(inputState) == beforeInput,
                    "skipped frame incorrectly advanced input state");
        }
        finish(timing);
    }
    require(consumerFrames > 3800 && consumerInstructions > 900000,
            "connected input trajectory did not execute expected bounded work");
}

int main(int argc, char** argv) {
    try {
        require(std::fesetround(FE_TONEAREST) == 0, "initial nearest-even setup failed");
        oracleIntegerBoundaries();
        directedNumeric();
        directedStateAndAdmission();
        capturedDecisionStages();
        liveStageTrajectories();
        connectedInputTrajectory();
        require(original::selectedWordCount == 69 && original::executedWordCount() == 68 &&
                original::allReachableWordsCovered(), "selected original-word coverage incomplete");
        for (U pc : {0x140b14u, 0x140bf0u, 0x140c4cu, 0x140c60u,
                     0x140c80u, 0x140ca4u, 0x140d38u}) {
            require(original::branchTakenCount(pc) && original::branchNotTakenCount(pc),
                    "original conditional branch lacks both paths");
        }
        if (argc != 1) {
            require(argc == 3 && std::strcmp(argv[1], "--coverage-json") == 0,
                    "usage: frame_timing49i [--coverage-json PATH]");
            std::ofstream coverage(argv[2]);
            require(bool(coverage), "cannot open requested original-word coverage output");
            original::writeCoverage(coverage); coverage << '\n'; coverage.close();
            require(bool(coverage), "cannot write requested original-word coverage output");
        }
        std::cout << "frame_timing49i: " << checks << " checks passed; "
                  << begins << " begin, " << entries << " entry, " << resets << " reset, "
                  << leaves << " leave, " << finishes << " finish hot calls; "
                  << rejected << " atomic rejections; " << trajectoryFrames
                  << " live-stage frames; " << consumerFrames << " connected 48V/48B frames, "
                  << consumerInstructions << " original input instructions; "
                  << original::executedWordCount() << "/" << original::selectedWordCount
                  << " timing source words (all 68 reachable); zero hot-call allocations\n";
        return 0;
    } catch (const std::exception& error) {
        std::fesetround(FE_TONEAREST);
        std::cerr << "frame_timing49i FAILED at " << stage << ": " << error.what() << '\n';
        return 1;
    }
}
