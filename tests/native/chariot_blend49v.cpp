#include <string>
#include "../../src/game/chariot_blend49v.h"
#include "../../evidence/chariotRoot49U/root/source/root_machine.h"
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <random>

using namespace rr::game;
using namespace rr::game::detail;
namespace original = chariot49u_original;
using U = std::uint32_t;
using M = BodyMatrix48G;
using original::bits;
using original::fp;
constexpr U B = original::Machine::vehicle, SP = original::Machine::stack - 0x5d0;
constexpr U secondary = 0x5400, physical = 0x21000, model = 0x21400;
static bool nativeActive = false;
static std::uint64_t allocations = 0, cases = 0, checks = 0, rejections = 0;
static std::uint64_t sourceRows = 0, durationEqual = 0, durationExpired = 0, endpointZeroSigns = 0;
void* operator new(std::size_t n) {
    if (nativeActive) ++allocations;
    if (void* p = std::malloc(n ? n : 1)) return p;
    throw std::bad_alloc();
}
void* operator new[](std::size_t n) { return ::operator new(n); }
[[gnu::noinline]] void release(void* p) noexcept { std::free(p); }
void operator delete(void* p) noexcept { release(p); }
void operator delete[](void* p) noexcept { release(p); }
void operator delete(void* p, std::size_t) noexcept { release(p); }
void operator delete[](void* p, std::size_t) noexcept { release(p); }
struct Row { U source, index; };
static std::array<Row, 6> nativeRows{};
static U nativeRowCount = 0;
void rrChariotBlend49VRow(U source, U row) noexcept {
    if (nativeRowCount >= nativeRows.size()) std::abort();
    nativeRows[nativeRowCount++] = {source, row};
}
static void need(bool yes, const char* s) {
    ++checks;
    if (!yes) throw std::runtime_error(std::string("mismatch ") + s);
}
static void same(U actual, U expected, const char* s) {
    ++checks;
    if (actual != expected) {
        char error[240];
        std::snprintf(error, sizeof error, "mismatch %s actual=%08x expected=%08x case=%llu",
            s, actual, expected, static_cast<unsigned long long>(cases));
        throw std::runtime_error(error);
    }
}
static M pose() { return {{.7f,.1f,-.2f,11, .25f,1.5f,-.5f,12, .5f,.25f,2,13, -2.5f,3.75f,8.5f,14}}; }
static M other() { return {{-7,8,9,-11, -.5f,2,.375f,-12, -1,.5f,.75f,-13, 32.5f,-17.25f,41.5f,-14}}; }
static bool native(const M& a, const M& b, float elapsed, float duration, M& out) {
    nativeRowCount = 0; nativeActive = true;
    const bool okay = blendChariotMatrix49V(a, b, elapsed, duration, out);
    nativeActive = false; return okay;
}
struct Proof {
    std::unique_ptr<original::Machine> machine = std::make_unique<original::Machine>();
    U mutationPc = 0, mutationWord = 0;
    std::uint64_t instructions = 0;
    std::array<std::uint64_t, chariot49u_words::total_words> hits{}, taken{}, notTaken{};
    M compare(const M& a, const M& b, float elapsed, float duration, float dt = .125f) {
        ++cases; auto& m = *machine;
        m.zeros(B, B + 0xe00); m.zeros(secondary, secondary + 0x400);
        m.zeros(physical, physical + 0x200); m.zeros(model, model + 0x200);
        m.zeros(SP - 0x100, SP + 0x5d0);
        // Supplied VU scratch is finite, including W values overwritten by
        // source self-subtraction; it is not inferred to be universally zero.
        for (U i = 1; i < 32; ++i) for (U j = 0; j < 4; ++j) m.v[i][j] = float(int(i * 4 + j) - 60) / 8.0f;
        m.v[0] = {0,0,0,1};
        m.init(B + 0x35c, secondary); m.init(secondary + 0x288, physical);
        // The numeric source follows secondary+288; vehicle19C is never read.
        m.init(B + 0x19c, 0x23000); m.init(B + 0x1b4, model);
        m.init(B + 0x83c, 7); m.init(B + 0x838, 3);
        m.store(B + 0x84c, elapsed); m.store(B + 0x848, duration); m.store(SP + 0x3c, dt);
        const M oldWorld = other();
        for (U i = 0; i < 16; ++i) {
            m.store(SP + 0x260 + 4 * i, a.v[i]);
            m.store(physical + 0x100 + 4 * i, b.v[i]);
            m.store(physical + 0xc0 + 4 * i, oldWorld.v[i]);
            m.store(model + 0x100 + 4 * i, oldWorld.v[i]);
        }
        m.init(physical + 0x190, 0x80); m.init(model + 0x190, 0x81);
        m.put64(16, B); m.put64(29, SP); m.put64(4, SP + 0x4f0);
        m.stores.clear(); m.trace.clear(); m.reads.clear(); m.calls.clear();
        if (mutationPc) m.substitutions[mutationPc] = mutationWord;
        U pc = 0x21ba04; m.callSteps = 0; std::array<Row,6> rows{}; U rowCount = 0;
        const auto oldSteps = m.steps;
        while (pc != 0x21beb4) {
            if (pc == 0x14ebb0) {
                need(rowCount < rows.size(), "extra row getter");
                const U address = m.g[5][0];
                need(address == SP + 0x260 || address == physical + 0x100, "cached-world row pointer");
                rows[rowCount++] = {address == SP + 0x260 ? 0u : 1u, m.g[6][0]};
            }
            U next = 0, unused = 0;
            if (m.execute(pc, next)) {
                const bool delay = m.delay;
                if (delay && m.execute(pc + 4, unused)) throw std::runtime_error("branch in delay slot");
                pc = next;
            } else pc += 4;
        }
        instructions += m.steps - oldSteps;
        need(rowCount == 6 && m.calls[0x14ebb0] == 6, "six original row getters");
        need(m.calls[0x2897b0] == 0 && m.calls[0x28b1a0] == 0 && m.calls[0x28a700] == 0,
            "blend must not refresh/get world");
        need(m.calls[0x28b490] == 1, "single output publication");
        need(m.reads[B + 0x19c] == 0, "numeric blend must follow secondary scene pointer");
        for (U i = 0; i < 16; ++i) {
            need(m.reads[physical + 0xc0 + 4 * i] == 0, "physical local must not be read");
            const bool used = (i >= 4 && i < 7) || (i >= 8 && i < 11) || (i >= 12 && i < 15);
            same(static_cast<U>(m.reads[physical + 0x100 + 4 * i]), used ? 1u : 0u, "cached-world exact read set");
        }
        for (U row = 0; row < 6; ++row)
            same(m.read(SP + 0x1c0 + 16 * row + 12), 0x3f800000u, "row getter forces W one");
        M out = oldWorld;
        need(native(a, b, elapsed, duration, out), "valid blend rejected");
        for (U i = 0; i < 16; ++i) {
            same(bits(out.v[i]), m.read(model + 0xc0 + 4 * i), "blended matrix lane");
            same(m.read(physical + 0x100 + 4 * i), bits(b.v[i]), "cached world preserved");
            same(m.read(physical + 0xc0 + 4 * i), bits(oldWorld.v[i]), "physical local preserved");
            same(m.read(model + 0x100 + 4 * i), bits(oldWorld.v[i]), "display cache not refreshed");
        }
        same(m.read(physical + 0x190), 0x80, "physical dirty bit preserved");
        for (U i = 0; i < 6; ++i) {
            same(rows[i].source, i / 3, "original virtual-before-physical row order");
            same(rows[i].index, 3 - i % 3, "original row3,2,1 order");
#ifdef RR_CHARIOT_BLEND49V_TRACED
            same(nativeRows[i].source, rows[i].source, "native extraction source order");
            same(nativeRows[i].index, rows[i].index, "native extraction row order");
#endif
        }
#ifdef RR_CHARIOT_BLEND49V_TRACED
        same(nativeRowCount, rowCount, "native extraction count");
#endif
        sourceRows += rowCount;
        const M* endpoint = elapsed <= 0.0f ? &a : elapsed >= duration ? &b : nullptr;
        if (endpoint) for (U i = 0; i < 12; ++i)
            if (i % 4 != 3 && out.v[i] == 0.0f && endpoint->v[i] == 0.0f &&
                bits(out.v[i]) != bits(endpoint->v[i])) ++endpointZeroSigns;
        // Independent source-side contract for the integrating wrapper:
        // publication is before E+dt and only updated E>D clears83C.
        volatile float updated = elapsed + dt;
        same(m.read(B + 0x84c), bits(updated), "source updated elapsed");
        same(m.read(B + 0x83c), updated > duration ? 0u : 7u, "source strict expiry");
        same(m.read(B + 0x838), 3u, "source preserves physical selector");
        bool elapsedStored = false; U published = 0;
        for (const auto& s : m.stores) {
            if (s.address >= model + 0xc0 && s.address < model + 0x100) {
                need(!elapsedStored, "matrix published after elapsed"); ++published;
            }
            if (s.address == B + 0x84c) { need(published == 16, "elapsed before full publication"); elapsedStored = true; }
        }
        need(elapsedStored, "elapsed store missing");
        if (updated == duration) ++durationEqual;
        if (updated > duration) ++durationExpired;
        return out;
    }
    void coverage(const char* path) const {
        if (!path) return;
        std::ofstream file(path);
        for (const auto& block : chariot49u_words::blocks) for (U i = 0; i < block.count; ++i) {
            const U ix = block.index + i;
            file << std::hex << block.start + 4*i << std::dec << ' ' << machine->hits[ix] << ' '
                << machine->taken[ix] << ' ' << machine->notTaken[ix] << '\n';
        }
        if (!file) throw std::runtime_error("coverage write");
    }
};
static void accepted(Proof& p) {
    const float elapsed[] = {-2, -.125f, -0.0f, 0, .125f, .5f, .875f, 1, 1.125f, 2};
    for (float e : elapsed) for (float dt : {0.0f, .125f}) p.compare(pose(), other(), e, 1, dt);
    for (float d : {.125f, .3f, 1.0f, 7.5f})
        for (float factor : {-2.0f, 0.0f, .1f, .5f, 1.0f, 2.0f}) p.compare(pose(), other(), d * factor, d);
    // Endpoints still normalize/rebuild and ignore row0 and all input W lanes.
    for (float e : {-1.0f, 0.0f, .5f, 1.0f, 2.0f}) {
        M a = pose(), b = other();
        for (U i : {0u,1u,2u,3u,7u,11u,15u}) { a.v[i] = fp(0x7fc01234u + i); b.v[i] = fp(0xff800000u); }
        M out = p.compare(a, b, e, 1);
        need(bits(out.v[0]) != bits(a.v[0]) && bits(out.v[0]) != bits(b.v[0]), "endpoint basis must rebuild");
        for (U i : {3u,7u,11u}) same(bits(out.v[i]), 0, "basis W positive zero");
        same(bits(out.v[15]), 0x3f800000u, "position W one");
    }
    // Each zero-sign mask exercises finite VU self-subtraction, truezero Q
    // saturation, zero forward, zero up, parallel and antiparallel bases.
    for (U mask = 0; mask < 128; ++mask) for (U type = 0; type < 4; ++type) {
        M a{}, b{};
        for (U i = 0; i < 16; ++i) { a.v[i] = fp(mask & (1u << (i % 7)) ? 0x80000000u : 0u); b.v[i] = fp(mask & (1u << ((i + 2) % 7)) ? 0u : 0x80000000u); }
        if (type == 1) { a.v[4] = b.v[4] = 1; a.v[8] = b.v[8] = 2; }
        if (type == 2) { a.v[5] = b.v[5] = 1; a.v[9] = b.v[9] = -2; }
        if (type == 3) { a.v[6] = b.v[6] = 1; }
        p.compare(a, b, elapsed[mask % 10], 1);
    }
    // Truezero squared-norm underflow is retained even for nonzero vectors;
    // positive-subnormal norm is excluded separately below.
    for (float tiny : {fp(1), fp(0x80000001u), fp(0x10000000u)}) {
        M a{}, b{}; a.v[8] = b.v[8] = tiny; p.compare(a, b, .5f, 1);
    }
    std::mt19937 random(0x49ba08u);
    for (U n = 0; n < 2048; ++n) {
        M a{}, b{};
        for (U i = 0; i < 16; ++i) { a.v[i] = float(int(random() % 4097) - 2048) / 256.0f; b.v[i] = float(int(random() % 4097) - 2048) / 256.0f; }
        const float d = float(1 + random() % 100) / 16.0f;
        const float e = float(int(random() % 301) - 100) / 100.0f * d;
        p.compare(a, b, e, d, n % 4 ? .125f : 0.0f);
    }
}
static void rejected() {
    auto reject = [](const M& a, const M& b, float e, float d) {
        M out = other(), before = out;
        need(!native(a, b, e, d, out), "invalid blend accepted");
        need(std::memcmp(&out, &before, sizeof out) == 0, "rejection changed output"); ++rejections;
    };
    const float nan = fp(0x7fc01234), inf = std::numeric_limits<float>::infinity();
    for (float d : {0.0f,-0.0f,-1.0f,nan,inf}) reject(pose(), other(), .5f, d);
    for (float e : {nan,inf,-inf}) reject(pose(), other(), e, 1);
    reject(pose(), other(), std::numeric_limits<float>::max(), fp(1));
    for (U i : {4u,5u,6u,8u,9u,10u,12u,13u,14u}) {
        M a = pose(), b = other(); a.v[i] = nan; reject(a, b, .5f, 1);
        a = pose(); b.v[i] = inf; reject(a, b, .5f, 1);
    }
    { M a{}, b{}; a.v[8] = b.v[8] = fp(0x1f800000u); reject(a, b, .5f, 1); }
    { M a = pose(), b = other(); a.v[12] = -std::numeric_limits<float>::max(); b.v[12] = std::numeric_limits<float>::max();
      reject(a, b, .5f, 1); reject(a, b, 1, 1); // upper-endpoint delay-slot subtraction
      M out{}; need(native(a, b, 0, 1, out), "lower endpoint must skip difference"); }
    { M a = pose(), b = other(); a.v[4] = b.v[4] = std::numeric_limits<float>::max(); reject(a, b, .5f, 1); }
    need(std::fesetround(FE_DOWNWARD) == 0, "set rounding"); reject(pose(), other(), .5f, 1);
    need(std::fesetround(FE_TONEAREST) == 0, "restore rounding");
}
int main(int argc, char** argv) {
    try {
        need(std::fesetround(FE_TONEAREST) == 0, "nearest rounding");
        Proof proof;
        if (argc == 3) { proof.mutationPc = U(std::stoul(argv[1], nullptr, 16)); proof.mutationWord = U(std::stoul(argv[2], nullptr, 16)); }
        accepted(proof); rejected();
        need(allocations == 0, "native allocated");
        need(durationEqual > 0 && durationExpired > 0, "duration boundaries covered");
        need(endpointZeroSigns > 0, "endpoint rebuild must expose changed zero signs");
        proof.coverage(std::getenv("RR_CHARIOT_BLEND49V_COVERAGE"));
        std::cout << "{\"passed\":true,\"cases\":" << cases << ",\"checks\":" << checks
            << ",\"rejections\":" << rejections << ",\"original_instructions\":" << proof.instructions
            << ",\"source_row_calls\":" << sourceRows << ",\"duration_equal\":" << durationEqual
            << ",\"duration_expired\":" << durationExpired << ",\"endpoint_changed_zero_signs\":" << endpointZeroSigns << ",\"native_allocations\":" << allocations << "}\n";
    } catch (const std::exception& error) { nativeActive = false; std::cerr << error.what() << '\n'; return 1; }
}
