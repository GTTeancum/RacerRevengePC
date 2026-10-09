#include <string>
#include "../../src/game/chariot_scene49u.h"
#include "../../evidence/chariotRoot49U/scene/source/scene_machine49u.h"
#include <array>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <new>
#include <random>
#include <sstream>

using namespace rr::game;
using namespace rr::game::detail;
namespace original = scene49u_original;
using U = std::uint32_t;
using I = SceneNodeIndex49P;
using V = VisualVector49R;
using M = BodyMatrix48G;
using original::bits;
using original::fp;
static bool nativeActive = false;
static std::uint64_t allocations = 0, checks = 0, cases = 0, operations = 0, instructions = 0, flagStores = 0;
void* operator new(std::size_t n) { if (nativeActive) ++allocations; if (void* p = std::malloc(n ? n : 1)) return p; throw std::bad_alloc(); }
void* operator new[](std::size_t n) { return ::operator new(n); }
[[gnu::noinline]] void release(void* p) noexcept { std::free(p); }
void operator delete(void* p) noexcept { release(p); }
void operator delete[](void* p) noexcept { release(p); }
void operator delete(void* p, std::size_t) noexcept { release(p); }
void operator delete[](void* p, std::size_t) noexcept { release(p); }
struct Flag { U node, before, after; };
static std::array<Flag, 512> flags{};
static U flagCount = 0;
#ifndef RR_SCENE49P_TRACE_REQUIRED
#define RR_SCENE49P_TRACE_REQUIRED 0
#endif
static constexpr bool traced = RR_SCENE49P_TRACE_REQUIRED != 0;
void rrScene49PFlagStore(U i, std::uint8_t a, std::uint8_t b) noexcept {
    if (flagCount >= flags.size()) std::abort();
    flags[flagCount++] = {i, a, b};
}
static std::array<std::uint64_t, scene49u_words::total_words> hits{}, taken{}, notTaken{};
static U mutationPc = 0, mutationWord = 0;
static void need(bool b, const std::string& s) { ++checks; if (!b) throw std::runtime_error(s); }
static void same(U a, U b, const std::string& s) {
    ++checks; if (a != b) { std::ostringstream x; x << s << " mismatch native=" << std::hex << a << " source=" << b; throw std::runtime_error(x.str()); }
}
static I ix(U i) { return static_cast<I>(i); }
static U pointer(I i) { return i == I::none ? 0 : original::Machine::nodeBase + static_cast<U>(i) * original::Machine::nodeStride; }
static M identity() { M m{}; m.v[0] = m.v[5] = m.v[10] = m.v[15] = 1.0f; return m; }
static M pose(U n) {
    M m = identity();
    if (n % 4 == 1) { m.v[0] = 0; m.v[2] = -1; m.v[8] = 1; m.v[10] = 0; }
    if (n % 4 == 2) { m.v[0] = -1; m.v[5] = -1; }
    if (n % 4 == 3) { m.v[5] = 0; m.v[6] = 1; m.v[9] = -1; m.v[10] = 0; }
    m.v[12] = float(n % 17) - 8.25f; m.v[13] = float(n % 11) * .125f; m.v[14] = float(n % 7) - 4.75f;
    return m;
}
static M randomMatrix(std::mt19937& rng) { M m{}; for (float& x : m.v) x = float(int(rng() % 513) - 256) / 1024.0f; return m; }
struct Fixture {
    std::array<SceneNode49P, 64> nodes{}, initial{};
    U count;
    std::unique_ptr<original::Machine> machine = std::make_unique<original::Machine>();
    SceneVectorContext49U context{0};
    explicit Fixture(U n) : count(n) {
        std::memset(nodes.data(), 0x5a, sizeof nodes);
        for (U i = 0; i < 64; ++i) {
            auto& p = nodes[i]; p.localC0 = pose(i); p.world100 = pose(i + 2);
            p.parent180 = p.firstChild184 = p.nextSibling188 = I::none; p.sceneFlags190 = 0x80;
        }
    }
    SceneGraph49P graph() { return {nodes.data(), count}; }
    void link(U child, U parent) {
        nodes[child].parent180 = ix(parent);
        I* at = &nodes[parent].firstChild184;
        while (*at != I::none) at = &nodes[static_cast<U>(*at)].nextSibling188;
        *at = ix(child);
    }
    void seed(float w = 0) {
        need(validateSceneGraph49P(graph()), "fixture topology"); initial = nodes;
        auto& m = *machine; context.vf9W = w; m.v[9][3] = w;
        for (U i = 0; i < count; ++i) {
            const U p = pointer(ix(i)); m.zeros(p, p + original::Machine::nodeStride);
            for (U j = 0; j < 16; ++j) { m.store(p + 0xc0 + 4 * j, nodes[i].localC0.v[j]); m.store(p + 0x100 + 4 * j, nodes[i].world100.v[j]); }
            m.init(p + 0x180, pointer(nodes[i].parent180)); m.init(p + 0x184, pointer(nodes[i].firstChild184)); m.init(p + 0x188, pointer(nodes[i].nextSibling188));
            m.init(p + 0x190, 0xa5a5a500u | nodes[i].sceneFlags190);
        }
        if (mutationPc) m.substitutions[mutationPc] = mutationWord;
        ++cases;
    }
    void compare() {
        auto& m = *machine;
        for (U i = 0; i < count; ++i) {
            const U p = pointer(ix(i)); SceneNode49P expected = initial[i];
            for (U j = 0; j < 16; ++j) { expected.localC0.v[j] = fp(m.read(p + 0xc0 + 4 * j)); expected.world100.v[j] = fp(m.read(p + 0x100 + 4 * j)); }
            expected.sceneFlags190 = static_cast<std::uint8_t>(m.read(p + 0x190));
            need(std::memcmp(&expected, &nodes[i], sizeof expected) == 0, "full graph local16/world16/flags/links/padding mismatch node" + std::to_string(i));
            same(m.read(p + 0x190) & 0xffffff00u, 0xa5a5a500u, "adjacent flag bytes");
            same(pointer(nodes[i].parent180), m.read(p + 0x180), "parent");
            same(pointer(nodes[i].firstChild184), m.read(p + 0x184), "first child");
            same(pointer(nodes[i].nextSibling188), m.read(p + 0x188), "sibling");
        }
        for (U i = count; i < 64; ++i) need(std::memcmp(&initial[i], &nodes[i], sizeof nodes[i]) == 0, "native outside graph mutation");
        U j = 0;
        if (traced) for (const auto& s : m.stores) {
            if (s.address < pointer(ix(0)) || s.address >= pointer(ix(count)) || (s.address - pointer(ix(0))) % 0x200 != 0x190) continue;
            need(j < flagCount, "missing native flag store mismatch");
            same(flags[j].node, (s.address - pointer(ix(0))) / 0x200, "flag store order node");
            same(flags[j].before, s.before & 255, "flag store order before");
            same(flags[j].after, s.after & 255, "flag store order after"); ++j;
        }
        if (traced) same(j, flagCount, "flag store count"); flagStores += j;
        same(bits(context.vf9W), bits(m.v[9][3]), "VF9.W context");
    }
    enum Op { Axes, Position, Direction, Inverse, Relative, Copy };
    void op(Op op, U target, U relative = 0, const M& supplied = identity()) {
        constexpr U input = 0x5000, output = 0x5100, second = 0x5200;
        auto& m = *machine; const V in{{.125f, -.75f, 2.125f, fp(0x7fc01234)}};
        V a{{-.0f, .0f, 7.0f, fp(0x7fc04321)}}, b = a; M out = supplied;
        for (U i = 0; i < 16; ++i) { m.store(input + 4 * i, supplied.v[i]); m.store(output + 4 * i, supplied.v[i]); }
        for (U i = 0; i < 4; ++i) { if (op != Copy) m.store(input + 4 * i, in.v[i]); m.store(output + 4 * i, a.v[i]); m.store(second + 4 * i, b.v[i]); }
        m.stores.clear(); flagCount = 0;
        U entry = 0;
        switch (op) {
        case Axes: entry = 0x28a700; m.put64(4, pointer(ix(target))); m.put64(5, pointer(ix(relative))); m.put64(6, output); m.put64(7, second); break;
        case Position: entry = 0x28b1a0; m.put64(4, pointer(ix(target))); m.put64(5, pointer(ix(relative))); m.put64(6, output); break;
        case Direction: entry = 0x28b9b0; m.put64(4, output); m.put64(5, pointer(ix(target))); m.put64(6, input); break;
        case Inverse: entry = 0x28bbb0; m.put64(4, output); m.put64(5, pointer(ix(target))); m.put64(6, input); break;
        case Relative: entry = 0x28b550; m.put64(4, pointer(ix(target))); m.put64(5, pointer(ix(relative))); m.put64(6, output); break;
        case Copy: entry = 0x28b3e0; m.put64(4, pointer(ix(target))); m.put64(5, input); break;
        }
        m.run(entry); bool okay = false; nativeActive = true;
        switch (op) {
        case Axes: okay = axes49U(graph(), ix(target), ix(relative), context, a, b); break;
        case Position: okay = position49U(graph(), ix(target), ix(relative), context, a); break;
        case Direction: okay = direction49U(graph(), ix(target), in, context, a); break;
        case Inverse: okay = inverseDirection49U(graph(), ix(target), in, context, a); break;
        case Relative: okay = relativeMatrix49U(graph(), ix(target), ix(relative), context, out); break;
        case Copy: okay = copyLocal49U(graph(), ix(target), supplied); break;
        }
        nativeActive = false; need(okay, "valid fixture rejected op" + std::to_string(op));
        if (op == Relative) for (U i = 0; i < 16; ++i) same(bits(out.v[i]), m.read(output + 4 * i), "relative matrix lane" + std::to_string(i));
        else if (op != Copy) for (U i = 0; i < 4; ++i) same(bits(a.v[i]), m.read(output + 4 * i), "vector op" + std::to_string(op) + " lane" + std::to_string(i));
        if (op == Axes) for (U i = 0; i < 4; ++i) same(bits(b.v[i]), m.read(second + 4 * i), "up lane" + std::to_string(i));
        compare(); ++operations;
    }
    ~Fixture() { instructions += machine->steps; for (U i = 0; i < hits.size(); ++i) { hits[i] += machine->hits[i]; taken[i] += machine->taken[i]; notTaken[i] += machine->notTaken[i]; } }
};

static void graphs() {
    std::mt19937 rng(49055);
    // Each getter is also first in a fresh source context: earlier getters in
    // the complete Chariot ordering must not hide its own cold-root path.
    for (U op = 0; op < 6; ++op) for (U shape = 0; shape < 3; ++shape) for (U mask = 0; mask < 8; ++mask) {
        Fixture f(8);
        if (shape == 1) { f.link(0, 2); f.link(1, 3); }
        if (shape == 2) { for (U i = 0; i < 7; ++i) f.link(i, i + 1); }
        for (U i = 0; i < 8; ++i) f.nodes[i].sceneFlags190 = static_cast<std::uint8_t>(0x64u | ((mask >> (i % 3)) & 1u));
        f.seed(float(mask) * .25f);
        f.op(static_cast<Fixture::Op>(op), 0, 1, pose(mask + 3));
    }
    // The Chariot relation: hidden198 is a separate root;194 has190 as parent;
    // actual cable endpoints are children of1B4, unlike the independent engines.
    for (U mask = 0; mask < 256; ++mask) {
        Fixture f(8); f.link(1, 2); f.link(4, 3); f.link(5, 3);
        for (U i = 0; i < 8; ++i) { f.nodes[i].sceneFlags190 = static_cast<std::uint8_t>((mask >> (i % 4)) | (mask << (8 - i % 4))); f.nodes[i].localC0 = pose(mask + i); f.nodes[i].world100 = pose(mask + i + 2); }
        if (mask % 4 == 0) for (U i = 0; i < 8; ++i) { f.nodes[i].localC0 = randomMatrix(rng); f.nodes[i].world100 = randomMatrix(rng); }
        f.seed(float(int(mask % 9) - 4));
        f.op(Fixture::Axes, 0, 1); f.op(Fixture::Position, 0, 1);
        f.op(Fixture::Position, 6, 1); f.op(Fixture::Position, 4, 1);
        f.op(Fixture::Direction, 1); f.op(Fixture::Inverse, 0);
        f.op(Fixture::Relative, 0, 1); f.op(Fixture::Relative, 0, 6);
        f.op(Fixture::Copy, 3, 0, pose(mask + 7)); f.op(Fixture::Position, 5, 1);
        f.op(Fixture::Copy, 0, 0, f.nodes[3].localC0); f.op(Fixture::Axes, 0, 1);
    }
    // Differing current/cached transforms, mixed readiness, non-parent-first
    // indices, wide/deep forests and non-affine fourth lanes.
    for (U trial = 0; trial < 64; ++trial) {
        Fixture f(64); for (U i = 0; i < 63; ++i) f.link(i, trial % 3 == 0 ? i + 1 : trial % 3 == 1 ? 63 : i + 1 + rng() % (63 - i));
        for (U i = 0; i < 64; ++i) { f.nodes[i].sceneFlags190 = static_cast<std::uint8_t>(rng()); f.nodes[i].localC0 = randomMatrix(rng); f.nodes[i].world100 = randomMatrix(rng); }
        f.seed(-0.0f); f.op(Fixture::Axes, 0, 1); f.op(Fixture::Position, 0, 2);
        f.op(Fixture::Relative, 0, 1); f.op(Fixture::Inverse, 3); f.op(Fixture::Direction, 4);
        f.op(Fixture::Copy, 63, 0, randomMatrix(rng)); f.op(Fixture::Position, 0, 1);
    }
    // Positive/negative-zero input words, including every matrix fourth lane.
    for (U mask = 0; mask < 32; ++mask) {
        Fixture f(3); f.link(1, 2);
        for (U i = 0; i < 3; ++i) { f.nodes[i].localC0 = identity(); for (U j = 0; j < 16; ++j) if (j % 5 != 0) f.nodes[i].localC0.v[j] = fp((mask & (1u << (j % 5))) ? 0x80000000u : 0u); f.nodes[i].sceneFlags190 = mask & 1; f.nodes[i].world100 = f.nodes[i].localC0; }
        f.seed(-0.0f); f.op(Fixture::Axes, 0, 1); f.op(Fixture::Position, 0, 1); f.op(Fixture::Direction, 1); f.op(Fixture::Inverse, 0); f.op(Fixture::Relative, 0, 1);
    }
    //28B3E0 copies payloads and all fourth lanes without arithmetic, even when
    // dirty target has a ready descendant. Its cache and topology stay supplied.
    for (U ready = 0; ready < 2; ++ready) {
        Fixture f(4); f.link(1, 0); f.link(2, 0); f.link(3, 1); f.nodes[0].sceneFlags190 = static_cast<std::uint8_t>(0x6c | ready); f.nodes[1].sceneFlags190 = 0xa5; f.nodes[2].sceneFlags190 = 0xff; f.nodes[3].sceneFlags190 = 1; f.seed();
        M words{}; for (U i = 0; i < 16; ++i) words.v[i] = fp(0x7fc01000u + i); f.op(Fixture::Copy, 0, 0, words);
    }
    for (U depth = 1; depth <= 8; ++depth) {
        Fixture f(depth); for (U i = 1; i < depth; ++i) f.link(i, i - 1);
        f.seed(); f.op(Fixture::Copy, depth - 1, 0, pose(depth));
    }
}
static void authored() {
    std::ifstream file("evidence/authoredGeometry49O/asset-reader/fixtures/52-chariot.psg", std::ios::binary);
    std::vector<char> bytes((std::istreambuf_iterator<char>(file)), {});
    need(bytes.size() >= 12 + 18 * 224, "missing authored Chariot scene");
    auto read = [&](U off) { U value; std::memcpy(&value, bytes.data() + off, 4); return value; };
    need(read(8) == 18, "authored node count");
    for (U variant = 0; variant < 16; ++variant) {
        Fixture f(21); f.link(19, 18);
        for (U i = 0; i < 18; ++i) {
            const auto p = static_cast<std::int32_t>(read(12 + 224 * i + 220));
            if (p >= 0) f.link(i, static_cast<U>(p));
            for (U j = 0; j < 16; ++j) f.nodes[i].localC0.v[j] = fp(read(12 + 224 * i + 128 + j * 4));
            f.nodes[i].sceneFlags190 = 0x80;
        }
        f.nodes[18].localC0 = pose(variant); f.nodes[19].localC0 = pose(variant + 1);
        f.nodes[20].localC0 = identity(); f.nodes[20].world100 = M{}; f.nodes[20].sceneFlags190 = 0x80;
        f.seed(); f.op(Fixture::Copy, 20, 0, f.nodes[0].localC0);
        f.op(Fixture::Axes, 20, 19); f.op(Fixture::Position, 20, 19);
        // Authored cable endpoints are source indices4/5.
        f.op(Fixture::Position, 4, 19); f.op(Fixture::Position, 5, 19);
        f.op(Fixture::Relative, 0, 19); f.op(Fixture::Inverse, 18);
    }
}
static void scalarCases() {
    auto m = std::make_unique<original::Machine>(); if (mutationPc) m->substitutions[mutationPc] = mutationWord;
    std::mt19937 rng(0x142b80u);
    const float angles[] = {0, -0.0f, fp(0x3fc90fdau), fp(0x3fc90fdbu), fp(0x3fc90fdcu), fp(0xbfc90fdau), fp(0xbfc90fdbu), fp(0xbfc90fdcu), fp(0x40490fdbu), fp(0xc0490fdbu), 4.25f, -5.25f, 10.0f, -10.0f};
    for (U trial = 0; trial < 1024; ++trial) {
        V axis{}, input{}, output{{7, 8, 9, fp(0x7fc0beef)}};
        for (U j = 0; j < 3; ++j) { axis.v[j] = float(int(rng() % 1001) - 500) / 500.0f; input.v[j] = float(int(rng() % 1001) - 500) / 32.0f; }
        if (trial < 16) { axis = V{{0, 1, 0, -0.0f}}; input = V{{1, 0, -0.0f, fp(0x7fc01234)}}; }
        if (trial >= 16 && trial < 32) { axis = V{{-0.0f, 0, -0.0f, 1}}; input = V{{-0.0f, -0.0f, 0, 1}}; }
        axis.v[3] = fp(0x7fc01111); input.v[3] = fp(0xff800000);
        const float angle = angles[trial % (sizeof angles / sizeof *angles)];
        for (U j = 0; j < 4; ++j) { m->store(0x5000 + 4 * j, axis.v[j]); m->store(0x5100 + 4 * j, input.v[j]); m->store(0x5200 + 4 * j, output.v[j]); }
        m->put64(4, 0x5000); m->put64(5, 0x5100); m->put64(6, 0x5200); m->f[12] = angle; m->run(0x142b80);
        nativeActive = true; bool okay = rotateVector49U(axis, input, angle, output); nativeActive = false; need(okay, "rotation rejected");
        for (U j = 0; j < 4; ++j) same(bits(output.v[j]), m->read(0x5200 + 4 * j), "rotation lane" + std::to_string(j));
        ++operations;
    }
    // Every largest-component branch, exact ties and unequal near-ties.
    for (U trial = 0; trial < 1024; ++trial) {
        M mat = randomMatrix(rng);
        if (trial < 16) { mat = pose(trial); if (trial & 4) mat.v[0] = mat.v[5] = mat.v[10] = 0; }
        for (U j : {3u, 7u, 11u, 12u, 13u, 14u, 15u}) mat.v[j] = fp(0x7fc00100u + j);
        for (U j = 0; j < 16; ++j) m->store(0x5000 + 4 * j, mat.v[j]);
        for (U j = 0; j < 4; ++j) m->init(0x5200 + 4 * j, 0xdeadbeefu);
        m->put64(4, 0x5200); m->put64(5, 0x5000); m->run(0x14f950);
        Quaternion48C output{}; nativeActive = true; bool okay = matrixQuaternion49U(mat, output); nativeActive = false; need(okay, "quaternion rejected");
        for (U j = 0; j < 4; ++j) same(bits(output.v[j]), m->read(0x5200 + 4 * j), "matrix quaternion lane" + std::to_string(j));
        ++operations;
    }
    instructions += m->steps; for (U i = 0; i < hits.size(); ++i) { hits[i] += m->hits[i]; taken[i] += m->taken[i]; notTaken[i] += m->notTaken[i]; }
}
static void admission() {
    Fixture f(3); f.link(1, 2); f.seed(); V out{{1, 2, 3, 4}}, other = out; const V good{{1, 2, 3, 4}}; M mat = identity(); Quaternion48C q{{1, 2, 3, 4}};
    const auto saved = f.nodes; const V savedOut = out;
    need(!axes49U(f.graph(), ix(0), ix(0), f.context, out, other), "equal axes accepted");
    need(!position49U(f.graph(), ix(0), I::none, f.context, out), "null relative accepted");
    need(!direction49U(f.graph(), ix(3), good, f.context, out), "bad target accepted");
    need(!inverseDirection49U(f.graph(), I::none, good, f.context, out), "negative target accepted");
    need(!relativeMatrix49U(f.graph(), ix(0), ix(0), f.context, mat), "equal matrix accepted");
    need(std::memcmp(saved.data(), f.nodes.data(), sizeof saved) == 0 && std::memcmp(&savedOut, &out, sizeof out) == 0, "early admission changed state");
    need(std::fesetround(FE_DOWNWARD) == 0, "rounding set");
    need(!rotateVector49U(good, good, 0, out), "wrong rounding rotation accepted");
    need(!matrixQuaternion49U(mat, q), "wrong rounding quaternion accepted");
    need(!axes49U(f.graph(), ix(0), ix(1), f.context, out, other), "wrong rounding graph accepted");
    need(std::fesetround(FE_TONEAREST) == 0, "rounding restore");
    V invalid = good; invalid.v[0] = fp(0x7f800000u);
    need(!rotateVector49U(invalid, good, 0, out), "nonfinite rotation accepted");
    need(std::memcmp(&savedOut, &out, sizeof out) == 0, "invalid rotation changed output");
    mat.v[0] = fp(0x7fc01234); need(!matrixQuaternion49U(mat, q), "nonfinite matrix accepted");
}
static void coverage() {
    const char* p = std::getenv("RR_CHARIOT_SCENE49U_COVERAGE"); if (!p) return;
    std::ofstream out(p); need(bool(out), "coverage output");
    out << "{\"fixtures\":" << cases << ",\"operations\":" << operations << ",\"checks\":" << checks << ",\"instructions\":" << instructions << ",\"ordered_flag_tracing\":" << (traced ? "true" : "false") << ",\"flag_stores_compared\":" << flagStores << ",\"native_hot_allocations\":" << allocations << ",\"pcs\":[";
    bool first = true; for (const auto& b : scene49u_words::blocks) for (U i = 0; i < b.count; ++i) { if (!first) out << ','; first = false; U n = b.index + i; out << "{\"pc\":" << b.start + 4 * i << ",\"word\":" << b.words[i] << ",\"hits\":" << hits[n] << ",\"taken\":" << taken[n] << ",\"not_taken\":" << notTaken[n] << '}'; }
    out << "]}\n";
}
int main(int argc, char** argv) {
    try {
        need(std::fesetround(FE_TONEAREST) == 0, "rounding");
        if (argc == 3) { mutationPc = U(std::stoul(argv[1], nullptr, 16)); mutationWord = U(std::stoul(argv[2], nullptr, 16)); }
        graphs(); authored(); scalarCases(); admission(); need(allocations == 0, "native hot allocation"); coverage();
        std::cout << "PASS ChariotScene49U: " << cases << " fixtures, " << operations << " operations, " << checks << " checks, " << instructions << " original instructions, " << flagStores << (traced ? " ordered flag stores" : " flag tracing disabled") << ", zero hot allocations\n";
        return 0;
    } catch (const std::exception& e) { nativeActive = false; std::cerr << "FAIL ChariotScene49U: " << e.what() << '\n'; return 1; }
}
