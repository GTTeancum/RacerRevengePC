#include "secondary_activation49v.h"
#include "../../evidence/chariotLifecycle49V/secondary/source/secondary_words49v.h"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <type_traits>
using namespace rr::game;
using U = std::uint32_t;
using Q = std::uint64_t;
static unsigned checks = 0, allocations = 0;
static bool countAllocations = false;
void* operator new(std::size_t n) { if (countAllocations) ++allocations; if (void* p = std::malloc(n)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
void operator delete(void* p) noexcept { std::free(p); }
void operator delete[](void* p) noexcept { std::free(p); }
void operator delete(void* p, std::size_t) noexcept { std::free(p); }
void operator delete[](void* p, std::size_t) noexcept { std::free(p); }
static void check(bool ok, const char* message) {
    ++checks;
    if (!ok) { std::fprintf(stderr, "check failed: %s\n", message); std::exit(1); }
}
template<class T> static bool same(const T& a, const T& b) { return std::memcmp(&a, &b, sizeof(T)) == 0; }
static std::array<Q, secondary49v_words::total_words> allHits{}, allTaken{}, allNotTaken{};

// Test-only fixed synthetic address space. Expected writes come solely from
// authenticated instruction words, including the real six zero-helper calls.
struct Machine {
    static constexpr U bodyAddress = 0x10000000, mainAddress = 0x10001000;
    static constexpr U vehicleAddress = 0x10002000, stackAddress = 0x20001000;
    static constexpr U stop = 0xdead0000;
    std::array<unsigned char, 0x330> body{}, main{};
    std::array<unsigned char, 0x880> vehicle{};
    std::array<unsigned char, 0x100> stack{};
    std::array<unsigned char, 0x40> vectors{};
    std::array<unsigned char, 16> quaternion{};
    std::array<std::array<U, 4>, 32> g{};
    std::array<U, 32> f{};
    std::array<bool, 0x330> bodyWrites{};
    std::array<U, 8> helperCalls{};
    unsigned helperCount = 0, steps = 0;
    U pc = 0;

    template<std::size_t N> static const unsigned char* at(const std::array<unsigned char, N>& bytes, U base, U address) {
        return address >= base && Q(address) < Q(base) + N ? &bytes[address - base] : nullptr;
    }
    unsigned char readByte(U address) const {
        if (auto p = at(body, bodyAddress, address)) return *p;
        if (auto p = at(main, mainAddress, address)) return *p;
        if (auto p = at(vehicle, vehicleAddress, address)) return *p;
        if (auto p = at(stack, stackAddress - 0x100, address)) return *p;
        if (auto p = at(vectors, 0x433b80, address)) return *p;
        if (auto p = at(quaternion, 0x382b60, address)) return *p;
        for (const auto& block : secondary49v_words::blocks) {
            if (address >= block.start && Q(address) < Q(block.start) + block.count * 4)
                return static_cast<unsigned char>(block.words[(address - block.start) / 4] >> ((address & 3u) * 8));
        }
        check(false, "oracle read outside initialized fixed records"); return 0;
    }
    Q read(U address, unsigned n) const {
        Q value = 0; for (unsigned i = 0; i < n; ++i) value |= Q(readByte(address + i)) << (i * 8); return value;
    }
    void write(U address, Q value, unsigned n) {
        for (unsigned i = 0; i < n; ++i) {
            const U a = address + i; unsigned char* p = nullptr;
            if (a >= bodyAddress && a < bodyAddress + body.size()) { p = &body[a - bodyAddress]; bodyWrites[a - bodyAddress] = true; }
            else if (a >= vehicleAddress && a < vehicleAddress + vehicle.size()) p = &vehicle[a - vehicleAddress];
            else if (a >= stackAddress - stack.size() && a < stackAddress) p = &stack[a - (stackAddress - stack.size())];
            else if (a >= 0x433b80 && a < 0x433bc0) p = &vectors[a - 0x433b80];
            check(p != nullptr, "oracle write outside callback/caller fixed records");
            *p = static_cast<unsigned char>(value >> (i * 8));
        }
    }
    Q low64(unsigned reg) const { return Q(g[reg][0]) | Q(g[reg][1]) << 32; }
    void set64(unsigned reg, Q value) { g[reg][0] = U(value); g[reg][1] = U(value >> 32); }
    std::size_t index(U address) const {
        for (const auto& block : secondary49v_words::blocks)
            if (address >= block.start && Q(address) < Q(block.start) + block.count * 4 && !(address & 3))
                return block.index + (address - block.start) / 4;
        check(false, "oracle PC outside authenticated source"); return 0;
    }
    // Return0=ordinary,1=branch/jump with delay,2=annulled likely delay slot.
    unsigned step(U address, U& target) {
        pc = address; check(++steps < 5000, "bounded original instruction count");
        const auto ix = index(pc); ++allHits[ix];
        const U w = U(read(pc, 4)), op = w >> 26, rs = (w >> 21) & 31u, rt = (w >> 16) & 31u;
        const U rd = (w >> 11) & 31u, sa = (w >> 6) & 31u, fn = w & 63u;
        const auto immediate = static_cast<std::int16_t>(w & 65535u);
        const U a = g[rs][0] + immediate;
        unsigned flow = 0;
        if (!w) {}
        else if (op == 15) g[rt][0] = (w & 65535u) << 16;
        else if (op == 9) g[rt][0] = a;
        else if (op == 12) g[rt][0] = g[rs][0] & (w & 65535u);
        else if (op == 13) g[rt][0] = g[rs][0] | (w & 65535u);
        else if (op == 10) g[rt][0] = static_cast<std::int32_t>(g[rs][0]) < immediate;
        else if (op == 11) g[rt][0] = low64(rs) < static_cast<Q>(static_cast<std::int64_t>(immediate));
        else if (op == 35) g[rt][0] = U(read(a, 4));
        else if (op == 30) { for (unsigned i = 0; i < 4; ++i) g[rt][i] = U(read(a + i * 4, 4)); }
        else if (op == 31) { for (unsigned i = 0; i < 4; ++i) write(a + i * 4, g[rt][i], 4); }
        else if (op == 40) write(a, g[rt][0], 1);
        else if (op == 43) write(a, g[rt][0], 4);
        else if (op == 63) write(a, low64(rt), 8);
        else if (op == 49) f[rt] = U(read(a, 4));
        else if (op == 57) write(a, f[rt], 4);
        else if (op == 4 || op == 5 || op == 20) {
            bool taken = g[rs][0] == g[rt][0]; if (op == 5) taken = !taken;
            ++(taken ? allTaken[ix] : allNotTaken[ix]);
            target = taken ? pc + 4 + static_cast<std::int32_t>(immediate) * 4 : pc + 8;
            flow = op == 20 && !taken ? 2 : 1;
        } else if (op == 3) {
            target = ((pc + 4) & 0xf0000000u) | ((w & 0x3ffffffu) << 2); g[31][0] = pc + 8;
            check(target == 0x306478 && helperCount < helperCalls.size(), "only actual fixed-size zero helper calls");
            helperCalls[helperCount++] = pc; flow = 1;
        } else if (op == 0 && fn == 8) { target = g[rs][0]; flow = 1; }
        else if (op == 0 && fn == 9) { target = g[rs][0]; g[rd][0] = pc + 8; flow = 1; }
        else if (op == 0 && fn == 0x2d) set64(rd, low64(rs) + low64(rt));
        else if (op == 0 && fn == 0x25) set64(rd, low64(rs) | low64(rt));
        else if (op == 0 && fn == 0x38) set64(rd, low64(rt) << sa);
        else if (op == 28 && (w & 0x7ffu) == 0x628) { for (unsigned i = 0; i < 4; ++i) g[rd][i] = g[rs][i] | g[rt][i]; }
        else if (w == 0x70081ee9u) {
            const Q x = low64(rt), y = low64(rs);
            for (unsigned i = 0; i < 4; ++i) g[rd][i] = U((x >> (i * 16)) & 65535u) | (U((y >> (i * 16)) & 65535u) << 16);
        } else { std::fprintf(stderr, "unsupported original instruction %08x at %08x\n", w, pc); check(false, "bounded interpreter instruction"); }
        g[0] = {}; return flow;
    }
    void run(U entry, U end = stop) {
        steps = 0; g[31][0] = stop; g[29][0] = stackAddress;
        U address = entry;
        while (address != end) {
            U target = 0, unused = 0; const unsigned flow = step(address, target);
            if (flow == 1) { check(step(address + 4, unused) == 0, "no branch in original delay slot"); address = target; }
            else address += flow == 2 ? 8 : 4;
        }
        check(g[29][0] == stackAddress, "original fixed stack restored");
    }
    explicit Machine(unsigned seed) {
        for (std::size_t i = 0; i < body.size(); ++i) { body[i] = static_cast<unsigned char>((i * 37 + seed * 53) ^ (i >> 3)); main[i] = static_cast<unsigned char>((i * 71 + seed * 29) ^ (i >> 2)); }
        for (std::size_t i = 0; i < vehicle.size(); ++i) vehicle[i] = static_cast<unsigned char>(i * 23 + seed * 19);
        const U table = 0x3ce000, mainPointer = mainAddress, secondaryPointer = bodyAddress;
        std::memcpy(body.data() + 8, &table, 4);
        std::memcpy(vehicle.data() + 0x340, &mainPointer, 4);
        std::memcpy(vehicle.data() + 0x35c, &secondaryPointer, 4);
        for (unsigned i = 0; i < 4; ++i) { const U word = secondary49v_words::quaternion_zero_constant[i]; std::memcpy(quaternion.data() + i * 4, &word, 4); }
        run(0x3c9290); // Real source initializer proves433B80=(0,0,0,1).
        g = {}; f = {}; bodyWrites = {}; helperCount = 0;
    }
};

// This explicit test-only bridge keeps canonical fields in their one owner.
// Both directions use memcpy to include arbitrary payloads and signed zeros.
template<class T> static void field(bool load, std::array<unsigned char, 0x330>& bytes, U offset, T& native) {
    if (load) std::memcpy(&native, bytes.data() + offset, sizeof(T));
    else std::memcpy(bytes.data() + offset, &native, sizeof(T));
}
static void project(bool load, std::array<unsigned char, 0x330>& bytes, SecondaryBodyState49M& secondary, SecondaryActivationSupplement49V& extra) {
    auto& root = secondary.root; auto& body = root.body;
    field(load, bytes, 0x34, secondary.cachedSpeed34);
    field(load, bytes, 0x60, body.position);
    field(load, bytes, 0x70, body.velocity); field(load, bytes, 0x7c, secondary.velocity70W);
    field(load, bytes, 0x80, root.localVelocity80); field(load, bytes, 0x8c, extra.localVelocity80W);
    field(load, bytes, 0x90, root.previousVelocity90); field(load, bytes, 0x9c, extra.previousVelocity90W);
    field(load, bytes, 0xa0, root.worldAngularVelocityA0);
    field(load, bytes, 0xb0, body.angularVelocity); field(load, bytes, 0xbc, secondary.angularVelocityB0W);
    field(load, bytes, 0xc0, body.linearAcceleration); field(load, bytes, 0xcc, extra.linearAccelerationC0W);
    field(load, bytes, 0xd0, body.angularAcceleration); field(load, bytes, 0xdc, extra.angularAccelerationD0W);
    field(load, bytes, 0xe0, secondary.cachedAnglesE0);
    field(load, bytes, 0x180, root.loads.force); field(load, bytes, 0x18c, secondary.force180W);
    field(load, bytes, 0x190, root.loads.torque); field(load, bytes, 0x19c, secondary.torque190W);
    field(load, bytes, 0x1a0, root.positionCorrection1a0); field(load, bytes, 0x1b0, root.rotationCorrection1b0);
    field(load, bytes, 0x1c0, root.accumulator1c0); field(load, bytes, 0x1d0, root.accumulator1d0);
    field(load, bytes, 0x1f0, body.orientation);
    field(load, bytes, 0x20c, secondary.body20C); field(load, bytes, 0x238, root.callbackState238);
    field(load, bytes, 0x2b8, extra.positionDerivative2B8); field(load, bytes, 0x2c4, body.previousPositionDerivative);
    field(load, bytes, 0x2d0, extra.linearAcceleration2D0); field(load, bytes, 0x2dc, body.previousLinearAcceleration);
    field(load, bytes, 0x2e8, body.orientationDerivative); field(load, bytes, 0x2f8, body.previousOrientationDerivative);
    field(load, bytes, 0x308, extra.angularAcceleration308); field(load, bytes, 0x314, body.previousAngularAcceleration);
}
static bool inWriteSet(unsigned i) {
    return (i >= 0x34 && i < 0x38) || (i >= 0x70 && i < 0xa0) || (i >= 0xb0 && i < 0xe0)
        || (i >= 0x180 && i < 0x1a0) || (i >= 0x2b8 && i < 0x320);
}
static void oneCase(unsigned seed, unsigned pattern, bool caller, int rounding) {
    Machine oracle(seed); const auto originalBody = oracle.body, originalMain = oracle.main;
    const auto originalVehicle = oracle.vehicle;
    SecondaryActivationGlobals49V globals{}; initializedSecondaryActivationGlobals49V(globals);
    check(std::memcmp(globals.vector433B80, oracle.vectors.data(), 16) == 0, "source initializer matches native433B80");
    check(std::memcmp(globals.quaternion382B60, oracle.quaternion.data(), 16) == 0, "source file constant matches native382B60");
    if (pattern == 1) {
        const U vector[4] = {0x80000000u, 0x7fa12345u, 0xff800000u, 0x12345678u};
        const U quaternion[4] = {0x7fc54321u, 0x80000000u, 0x00000001u, 0xffa56789u};
        std::memcpy(globals.vector433B80, vector, 16); std::memcpy(globals.quaternion382B60, quaternion, 16);
    } else if (pattern == 2) {
        const U vector[4] = {0x3f000000u, 0xbf800000u, 0x40800000u, 0x80000000u};
        const U quaternion[4] = {0x41200000u, 0x42200000u, 0x43200000u, 0x44200000u};
        std::memcpy(globals.vector433B80, vector, 16); std::memcpy(globals.quaternion382B60, quaternion, 16);
    }
    std::memcpy(oracle.vectors.data(), globals.vector433B80, 16); std::memcpy(oracle.quaternion.data(), globals.quaternion382B60, 16);
    SecondaryBodyState49M secondary; SecondaryActivationSupplement49V extra;
    std::memset(&secondary, int(seed & 255u), sizeof(secondary)); std::memset(&extra, int((seed + 41) & 255u), sizeof(extra));
    secondary.scenePresent = (seed & 1u) != 0; secondary.enabled = (seed & 2u) != 0;
    secondary.phase = static_cast<SecondaryPhase49M>(seed % 3); // No invented phase transition.
    auto nativeImage = oracle.body; project(true, nativeImage, secondary, extra);
    const auto beforeSecondary = secondary; const auto beforeExtra = extra; const auto beforeGlobals = globals;
    SecondaryBodyState49M mainState{}; SecondaryActivationSupplement49V mainExtra{};
    auto mainImage = oracle.main; project(true, mainImage, mainState, mainExtra);
    const auto beforeMainState = mainState;
    if (caller) { oracle.g[16][0] = Machine::vehicleAddress; oracle.run(0x21bf08, 0x21c0f4); }
    else { oracle.g[4][0] = Machine::bodyAddress; oracle.run(0x167a50); }
    check(oracle.helperCount == 6, "six original zero helper calls");
    constexpr U expectedCalls[] = {0x167af8, 0x167b38, 0x167b48, 0x167af8, 0x167b38, 0x167b48};
    check(std::memcmp(oracle.helperCalls.data(), expectedCalls, sizeof(expectedCalls)) == 0, "exact helper call order");
    unsigned written = 0;
    for (unsigned i = 0; i < oracle.body.size(); ++i) {
        check(oracle.bodyWrites[i] == inWriteSet(i), "exact236-byte source write set");
        written += oracle.bodyWrites[i];
        if (!inWriteSet(i)) check(oracle.body[i] == originalBody[i], "every other original body byte untouched");
    }
    check(written == 236, "source writes exactly236 body bytes");
    check(oracle.main == originalMain, "source main body read-only");
    auto expectedVehicle = originalVehicle;
    if (caller) { const U zero = 0, one = 1; std::memcpy(expectedVehicle.data() + 0x84c, &zero, 4); std::memcpy(expectedVehicle.data() + 0x838, &one, 4); }
    check(oracle.vehicle == expectedVehicle, "caller exact elapsed/gate writes only");
    auto expectedSecondary = beforeSecondary; auto expectedExtra = beforeExtra;
    auto expectedImage = oracle.body; project(true, expectedImage, expectedSecondary, expectedExtra);
    std::fesetround(rounding); std::feclearexcept(FE_ALL_EXCEPT);
    const unsigned allocationBefore = allocations; countAllocations = true;
    resetSecondaryActivation49V(secondary, extra, globals);
    if (caller) copySecondaryMainVelocities49V(mainState.root, mainState.velocity70W, mainState.angularVelocityB0W, secondary);
    countAllocations = false;
    check(allocations == allocationBefore, "no allocation in native callback or copy");
    check(std::fetestexcept(FE_ALL_EXCEPT) == 0, "copy-only payloads cause no floating exception");
    check(std::fegetround() == rounding, "rounding mode preserved");
    check(same(secondary, expectedSecondary), "whole canonical native record exact including untouched bytes");
    check(same(extra, expectedExtra), "all52 supplemental bytes exact");
    check(same(globals, beforeGlobals) && same(mainState, beforeMainState), "globals and canonical main read-only");
    project(false, nativeImage, secondary, extra);
    check(nativeImage == oracle.body, "all816 original body bytes equal independent original-word result");
    if (caller) {
        U speedBits; std::memcpy(&speedBits, &secondary.cachedSpeed34, 4);
        check(speedBits == 0, "caller keeps reset speed before later frame cache refresh");
        check(std::memcmp(nativeImage.data() + 0x70, originalMain.data() + 0x70, 16) == 0, "caller copies all four main70 lanes after reset");
        check(std::memcmp(nativeImage.data() + 0xb0, originalMain.data() + 0xb0, 16) == 0, "caller copies main localB0, not worldA0");
    }
}
static void coverage() {
    for (const auto& block : secondary49v_words::blocks) {
        unsigned reached = 0; for (std::size_t i = 0; i < block.count; ++i) reached += allHits[block.index + i] != 0;
        if (std::strcmp(block.name, "secondary_slot_AC") == 0) check(reached == 29, "all29 slotAC words reached");
        if (std::strcmp(block.name, "secondary_slot_A8") == 0) check(reached == 43, "all43 slotA8 words reached");
        if (std::strcmp(block.name, "fixed_zero_helper") == 0) check(reached == 38, "all38 applicable fixed-zero words reached");
        if (std::strcmp(block.name, "vector_global_initializer") == 0) check(reached == 32, "all32 vector initializer words reached");
        if (reached) {
            std::printf("coverage %s: %u/%zu; unreached:", block.name, reached, block.count);
            for (std::size_t i = 0; i < block.count; ++i) if (!allHits[block.index + i]) std::printf(" %08x", block.start + U(i * 4));
            std::putchar('\n');
        }
    }
    for (const auto& block : secondary49v_words::blocks) for (std::size_t i = 0; i < block.count; ++i) {
        auto ix = block.index + i;
        if (allTaken[ix] || allNotTaken[ix]) std::printf("branch %08x: taken=%llu not-taken=%llu\n", block.start + U(i * 4),
            static_cast<unsigned long long>(allTaken[ix]), static_cast<unsigned long long>(allNotTaken[ix]));
    }
}
int main() {
    static_assert(std::is_trivially_copyable<SecondaryBodyState49M>::value, "exact byte comparisons require a trivial record");
    static_assert(sizeof(SecondaryActivationSupplement49V) == 52, "supplement size");
    constexpr int rounding[] = {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO};
    unsigned cases = 0;
    for (unsigned seed : {1u, 17u, 89u, 213u, 254u, 0u})
        for (unsigned pattern = 0; pattern < 3; ++pattern)
            for (bool caller : {false, true})
                for (int mode : rounding) { oneCase(seed, pattern, caller, mode); ++cases; }
    std::fesetround(FE_TONEAREST); coverage();
    std::printf("secondary_activation49v: PASS %u cases, %u checks, %u allocations, %u full-object bytes compared\n", cases, checks, allocations, cases * 0x330);
    return 0;
}
