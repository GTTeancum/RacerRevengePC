#include <string>
#include "../../src/game/chariot_projection49p.h"
#include "../../evidence/sceneProjection49P/source/scene_oracle.h"
#include <array>
#include <cfenv>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace rr::game;
namespace original = scene49p_oracle;
using original::U;
using original::bits;
using original::fp;
using Index = SceneNodeIndex49P;
using Nodes = std::array<SceneNode49P, 8>;
static std::uint64_t checks = 0, instructions = 0;
static unsigned cases = 0, copied = 0, excludedModes = 0;
static std::array<std::uint64_t, scene49p_words::total_words> hits{}, taken{}, notTaken{};
static U mutationPc = 0, mutationWord = 0;
constexpr U nodeBase = 0x2000, nodeStride = 0x200, vehicle = 0xa000;
static Index ix(unsigned n) { return static_cast<Index>(n); }
static U address(unsigned n) { return nodeBase + nodeStride * n; }
static U pointer(Index n) { return n == Index::none ? 0 : address(static_cast<unsigned>(n)); }
static void require(bool condition, const char* message) {
    ++checks;
    if (!condition) throw std::runtime_error(message);
}
static Nodes nodes(unsigned seed, bool opaqueWords = false) {
    Nodes result;
    std::memset(result.data(), 0, sizeof result);
    for (unsigned n = 0; n < result.size(); ++n) {
        auto& node = result[n];
        node.parent180 = node.firstChild184 = node.nextSibling188 = Index::none;
        node.sceneFlags190 = static_cast<std::uint8_t>(((0x26u + n * 18u) & 0xfeu) | ((seed >> n) & 1u));
        for (unsigned lane = 0; lane < 16; ++lane) {
            node.localC0.v[lane] = float(int((seed + n * 17 + lane * 11) % 71) - 35) / 16.f;
            node.world100.v[lane] = float(int((seed + n * 29 + lane * 7) % 83) - 41) / 8.f;
        }
        node.localC0.v[(n + seed) % 16] = fp(0x80000000u);
    }
    // Main physical190 -> current supplied graphics194, separate secondary19C,
    // and Chariot1B4 -> (animated node4 -> node6, sibling node5). Node7 unrelated.
    result[0].firstChild184 = ix(1); result[1].parent180 = ix(0);
    result[3].firstChild184 = ix(4); result[4].parent180 = ix(3);
    result[4].firstChild184 = ix(6); result[4].nextSibling188 = ix(5);
    result[5].parent180 = ix(3); result[6].parent180 = ix(4);
    if (opaqueWords) {
        const U patterns[16] = {0, 0x80000000u, 0x3f800000u, 0xbf800000u,
            1, 0x80000001u, 0x7f7fffffu, 0xff7fffffu, 0x7f800000u, 0xff800000u,
            0x7fc01234u, 0xffc54321u, 0x41234567u, 0xc2345678u, 0x3d654321u, 0x40000000u};
        for (unsigned lane = 0; lane < 16; ++lane)
            result[2].localC0.v[lane] = fp(patterns[(lane + seed) % 16]);
    }
    return result;
}
static ChariotProjectionInput49P input(U live838 = 1, U live83C = 0) {
    ChariotProjectionInput49P value;
    value.phase = ChariotProjectionPhase49P::beforePhysicalCopy21B9A4;
    value.suppliedState = ChariotSuppliedState49P::completeSceneStateThroughChariotPrefix;
    value.vehicle838 = live838; value.vehicle83C = live83C;
    value.secondaryPhysical19C = ix(2); value.chariotModel1B4 = ix(3);
    return value;
}
static void load(original::Machine& machine, const Nodes& source, const ChariotProjectionInput49P& in) {
    for (unsigned n = 0; n < source.size(); ++n) {
        const auto& node = source[n]; const U p = address(n);
        for (unsigned lane = 0; lane < 16; ++lane) {
            machine.write(p + 0xc0 + 4 * lane, bits(node.localC0.v[lane]));
            machine.write(p + 0x100 + 4 * lane, bits(node.world100.v[lane]));
        }
        machine.write(p + 0x180, pointer(node.parent180));
        machine.write(p + 0x184, pointer(node.firstChild184));
        machine.write(p + 0x188, pointer(node.nextSibling188));
        machine.write(p + 0x190, 0xa5a5a500u | node.sceneFlags190);
    }
    machine.write(vehicle + 0x838, in.vehicle838);
    machine.write(vehicle + 0x83c, in.vehicle83C);
    machine.write(vehicle + 0x190, address(0));
    machine.write(vehicle + 0x194, address(1));
    machine.write(vehicle + 0x198, address(7));
    machine.write(vehicle + 0x19c, pointer(in.secondaryPhysical19C));
    machine.write(vehicle + 0x1b4, pointer(in.chariotModel1B4));
    machine.write(vehicle + 0x58, 0x12345678);
    machine.g[16][0] = vehicle;
    machine.g[29][0] = original::Machine::stack;
    if (mutationPc) machine.substitutions[mutationPc] = mutationWord;
}
static U executeBranch(original::Machine& machine) {
    U pc = 0x21b9a4;
    machine.callSteps = 0;
    while (pc != 0x21b9d4 && pc != 0x21beb8) {
        U target = 0, unused = 0;
        if (machine.execute(pc, target)) {
            require(!machine.execute(pc + 4, unused), "branch in delay slot");
            pc = target;
        } else pc += 4;
    }
    return pc;
}
static void compare(const Nodes& actual, original::Machine& expected) {
    for (unsigned n = 0; n < actual.size(); ++n) {
        const auto& node = actual[n]; const U p = address(n);
        for (unsigned lane = 0; lane < 16; ++lane) {
            require(bits(node.localC0.v[lane]) == expected.read(p + 0xc0 + 4 * lane), "local word mismatch");
            require(bits(node.world100.v[lane]) == expected.read(p + 0x100 + 4 * lane), "world word mismatch");
        }
        require(pointer(node.parent180) == expected.read(p + 0x180), "parent changed");
        require(pointer(node.firstChild184) == expected.read(p + 0x184), "child changed");
        require(pointer(node.nextSibling188) == expected.read(p + 0x188), "sibling changed");
        require((0xa5a5a500u | node.sceneFlags190) == expected.read(p + 0x190), "flag byte or adjacent poison mismatch");
        for (unsigned offset = 0; offset < nodeStride; offset += 4) {
            if ((offset >= 0xc0 && offset < 0x140) || offset == 0x180 ||
                offset == 0x184 || offset == 0x188 || offset == 0x190) continue;
            require(expected.read(p + offset) == 0xa5a5a5a5u, "original changed unrepresented scene state");
        }
    }
}
static void collect(const original::Machine& machine) {
    instructions += machine.steps;
    for (unsigned n = 0; n < scene49p_words::total_words; ++n) {
        hits[n] += machine.hits[n]; taken[n] += machine.taken[n]; notTaken[n] += machine.notTaken[n];
    }
}
static void fixture(unsigned seed, U live838, U live83C, bool opaque, bool laterRead) {
    auto actual = nodes(seed, opaque); const auto before = actual;
    const auto in = input(live838, live83C);
    auto machine = std::make_unique<original::Machine>(); load(*machine, before, in);
    const U exit = executeBranch(*machine);
    const bool admitted = live838 != 0 && live83C == 0;
    require(projectChariotPhysicalBranch49P({actual.data(), static_cast<U>(actual.size())}, in) == admitted,
            "live mode admission mismatch");
    require(exit == (admitted ? 0x21beb8u : 0x21b9d4u), "source dispatcher selected different branch");
    compare(actual, *machine);
    if (admitted) {
        ++copied;
        require(machine->g[3][0] == 0x12345678, "source continuation delay slot not executed");
        require(std::memcmp(&actual[2], &before[2], sizeof actual[2]) == 0, "source changed");
        require(std::memcmp(&actual[3].localC0, &before[2].localC0, sizeof(BodyMatrix48G)) == 0,
                "not a complete local-word copy");
        // No source-world reads or target2F0 reads belong to this branch.
        for (unsigned lane = 0; lane < 16; ++lane)
            require(machine->reads[(address(2) + 0x100 + 4 * lane) / 4] == 1,
                    "branch consumed cached source world"); // compare() itself reads each lane once.
        require(machine->reads[(vehicle + 0x2f0) / 4] == 0, "target offset was consumed");
        if (laterRead) {
            // This is an explicit later operation, not part of the copy stage.
            for (unsigned node : {6u, 5u, 3u}) {
                require(refreshSceneWorld49P({actual.data(), static_cast<U>(actual.size())}, ix(node)), "later getter rejected");
                machine->call(0x2897b0, address(node)); compare(actual, *machine);
            }
        }
        // A repeated source-ready/destination-dirty setter must retain the source
        // shortcut rather than force every model descendant dirty.
        machine->g[16][0] = vehicle; machine->g[29][0] = original::Machine::stack;
        require(executeBranch(*machine) == 0x21beb8, "repeat branch exit");
        require(projectChariotPhysicalBranch49P({actual.data(), static_cast<U>(actual.size())}, in), "repeat copy rejected");
        compare(actual, *machine);
    } else {
        ++excludedModes;
        require(std::memcmp(actual.data(), before.data(), sizeof actual) == 0, "unsupported gate mutated graph");
        require(machine->reads[(vehicle + 0x19c) / 4] == 0 &&
                machine->reads[(vehicle + 0x1b4) / 4] == 0, "excluded source route touched model bindings");
    }
    collect(*machine); ++cases;
}
static void admissions() {
    auto n = nodes(255); auto in = input(); SceneGraph49P graph{n.data(), static_cast<U>(n.size())};
    auto reject = [&] {
        const auto before = n;
        require(!projectChariotPhysicalBranch49P(graph, in), "unsupported stage admitted");
        require(std::memcmp(n.data(), before.data(), sizeof n) == 0, "rejected stage changed state");
    };
    for (auto phase : {ChariotProjectionPhase49P::unspecified, ChariotProjectionPhase49P::laterPhysics,
                       ChariotProjectionPhase49P::afterPhysics, static_cast<ChariotProjectionPhase49P>(255)}) {
        in.phase = phase; reject();
    }
    in = input();
    for (auto state : {ChariotSuppliedState49P::unavailable, ChariotSuppliedState49P::localMatricesOnly,
                       static_cast<ChariotSuppliedState49P>(255)}) {
        in.suppliedState = state; reject();
    }
    in = input();
    for (Index invalid : {Index::none, static_cast<Index>(-2), ix(8), ix(64)}) {
        in.secondaryPhysical19C = invalid; reject(); in = input();
        in.chariotModel1B4 = invalid; reject(); in = input();
    }
    in.chariotModel1B4 = in.secondaryPhysical19C; reject(); in = input();
    in.secondaryPhysical19C = ix(1); reject(); in = input();
    in.chariotModel1B4 = ix(4); reject(); in = input();
    graph.nodes = nullptr; reject(); graph.nodes = n.data();
    graph.count = 0; reject(); graph.count = 65; reject(); graph.count = static_cast<U>(n.size());
    n[0].parent180 = ix(1); reject(); n = nodes(255);
    require(std::fesetround(FE_DOWNWARD) == 0, "rounding setup"); reject();
    require(std::fesetround(FE_TONEAREST) == 0, "rounding restore");
    require(projectChariotPhysicalBranch49P(graph, in), "restored valid stage rejected");
}
static void coverage() {
    const char* path = std::getenv("RR_CHARIOT49P_COVERAGE"); if (!path) return;
    std::ofstream out(path); require(bool(out), "coverage open failed");
    out << "{\"cases\":" << cases << ",\"physical_branch_cases\":" << copied
        << ",\"excluded_mode_cases\":" << excludedModes << ",\"checks\":" << checks
        << ",\"instructions\":" << instructions << ",\"pcs\":[";
    bool first = true;
    for (const auto& block : scene49p_words::blocks) for (unsigned n = 0; n < block.count; ++n) {
        const auto slot = block.index + n; if (!hits[slot]) continue;
        if (!first) out << ',';
        first = false;
        out << "{\"pc\":" << block.start + 4 * n << ",\"hits\":" << hits[slot]
            << ",\"taken\":" << taken[slot] << ",\"not_taken\":" << notTaken[slot] << '}';
    }
    out << "]}\n";
}
int main(int argc, char** argv) {
    try {
        require(std::fesetround(FE_TONEAREST) == 0, "rounding");
        if (argc == 3) { mutationPc = std::stoul(argv[1], nullptr, 16); mutationWord = std::stoul(argv[2], nullptr, 16); }
        for (unsigned ready = 0; ready < 256; ++ready)
            for (U live : {1u, 2u, 0x80000000u, 0xffffffffu}) fixture(ready, live, 0, false, ready % 7 == 0);
        for (unsigned ready = 0; ready < 64; ++ready) fixture(ready, 1, 0, true, false);
        for (unsigned ready = 0; ready < 16; ++ready)
            for (auto gates : {std::array<U, 2>{0, 0}, {0, 1}, {0, 0xffffffffu}, {1, 1}, {2, 0x80000000u}})
                fixture(ready, gates[0], gates[1], false, false);
        admissions(); coverage();
        std::cout << "PASS ChariotProjection49P: " << cases << " cases, " << checks << " checks, "
                  << instructions << " original instructions; exact pre-copy admission, local16, gates, lazy caches\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL ChariotProjection49P: " << e.what() << '\n'; return 1;
    }
}
