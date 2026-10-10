#include "../../src/game/source_acos49t.h"
#include "../../evidence/chariotAnimation49T/acos/original_machine.h"
#include "../../evidence/chariotAnimation49T/acos/original_data.h"
#include "../../evidence/chariotAnimation49T/acos/exact_vectors.h"
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <sstream>
#include <string>

using namespace rr::game;
namespace original = acos49t_original;
namespace native = source_acos49t_test;
using original::U;
using original::D;
using original::I;
static std::uint64_t checks = 0, chains = 0, actual = 0, primitives = 0;
static std::uint64_t conversions = 0, roots = 0, mathematical = 0, poisoned = 0;

static void need(bool value, const std::string& reason) {
    ++checks;
    if (!value) throw std::runtime_error(reason);
}
static std::string hex(D x) {
    std::ostringstream out;
    out << std::hex << x;
    return out.str();
}
static D call(original::Machine& m, U entry, D a, D b = 0) {
    m.put64(4, a);
    m.put64(5, b);
    m.run(entry);
    return m.get64(2);
}
static void compact(original::Machine& m) {
    m.trace.clear();
    m.stores.clear();
}
static void chain(original::Machine& m, U input, U* fixtureExpected = nullptr) {
    compact(m);
    m.init(0x385160, 123);
    m.f[12] = original::fp(input);
    m.run(0x311038);
    const D widened = m.get64(2);
    const D inner = call(m, 0x2fea90, widened);
    const D wrapped = call(m, 0x302730, widened);
    call(m, 0x310440, wrapped);
    const U expected = original::bits(m.f[0]);
    const U expectedErrno = m.read(0x385160);
    SourceLibmState49T state;
    state.errnoValue = 123;
    U result = 0xa5a5a5a5;
    SourceAcosTrace49T trace{};
    const std::string context = "input " + hex(input);
    need(sourceAcos49T(input, state, result, &trace), "valid call rejected " + context);
    need(trace.widened == widened, "widened mismatch " + context);
    need(trace.inner == inner, "inner acos mismatch " + context);
    need(trace.wrapped == wrapped, "wrapped acos mismatch " + context);
    need(result == expected, "narrowed result mismatch " + context);
    need(U(state.errnoValue) == expectedErrno, "errno mismatch " + context);
    need(state.mode == 1 && state.defaultMatherr && state.coldNanRecord, "state contract altered");
    if (fixtureExpected) need(result == *fixtureExpected, "retained fixture result mismatch " + context);
    ++chains;
}

int main(int argc, char** argv) {
    try {
        original::Machine m;
        for (auto word : acos49t_words::static_words) m.init(word.address, word.bits);
        m.zeros(0x44beb0, 0x44bec8);
        // All remaining readable non-code state is an explicit caller stack.
        std::ifstream fixtures("evidence/chariotAnimation49T/acos/actual-arguments.tsv");
        need(bool(fixtures), "missing actual argument evidence");
        std::set<U> inputs;
        U input, expected;
        while (fixtures >> std::hex >> input >> expected) {
            chain(m, input, &expected);
            inputs.insert(input);
            ++actual;
        }
        need(actual == 1250 && inputs.size() == 239, "actual fixture count changed");
        for (U x : {0u, 0x80000000u, 1u, 0x007fffffu, 0x80000001u,
                    0x3f000000u, 0x3f7fffffu, 0x3f800000u, 0x3f800001u,
                    0xbf800000u, 0xbf800001u, 0x7f800000u, 0xff800000u,
                    0x7fc00000u, 0x7f800001u, 0x7fffffffu}) inputs.insert(x);
        for (U center : {0x3f000000u, 0x3f800000u, 0xbf000000u, 0xbf800000u,
                         0x23000000u, 0xa3000000u, 0x00800000u, 0x80800000u,
                         0x7f800000u, 0xff800000u})
            for (I delta = -64; delta <= 64; ++delta) inputs.insert(center + U(delta));
        std::mt19937_64 random(0x49ac05);
        for (unsigned i = 0; i < 4096; ++i) {
            inputs.insert(U(random()));
            inputs.insert(0x3e000000 + U(random()) % 0x1800000);
            inputs.insert(0xbe000000 + U(random()) % 0x1800000);
        }
        for (U x : inputs) chain(m, x);

        constexpr U entries[] = {0x30fbb0, 0x30fc08, 0x30fc70, 0x30ff18};
        for (unsigned i = 0; i < 4096; ++i) {
            const D a = (random() & 0x800fffffffffffffull) | D(900 + random() % 220) << 52;
            const D b = (random() & 0x800fffffffffffffull) | D(900 + random() % 220) << 52;
            for (unsigned op = 0; op < 4; ++op) {
                compact(m);
                const D source = call(m, entries[op], a, b);
                const D result = native::arithmetic(native::Operation(op), a, b);
                need(result == source, "elementary arithmetic mismatch op " + std::to_string(op));
                ++primitives;
            }
        }
        for (auto test : acos49t_exact::arithmetic) {
            compact(m);
            const D source = call(m, entries[test.op], test.a, test.b);
            const D result = native::arithmetic(native::Operation(test.op), test.a, test.b);
            need(source == test.result, "original differs from exact rational arithmetic");
            need(result == test.result, "native differs from exact rational arithmetic");
            ++mathematical;
        }
        for (auto test : acos49t_exact::widen) {
            compact(m); m.f[12] = original::fp(U(test.input)); m.run(0x311038);
            need(m.get64(2) == test.result, "original widening exact rational mismatch");
            need(native::widen(U(test.input)) == test.result, "native widening exact rational mismatch");
            ++mathematical; ++conversions;
        }
        for (auto test : acos49t_exact::narrow) {
            compact(m); call(m, 0x310440, test.input);
            need(original::bits(m.f[0]) == test.result, "original narrowing exact rational mismatch");
            need(native::narrow(test.input) == test.result, "native narrowing exact rational mismatch");
            ++mathematical; ++conversions;
        }
        for (auto test : acos49t_exact::roots) {
            compact(m);
            need(call(m, 0x300198, test.input) == test.result, "original sqrt exact integer mismatch");
            need(native::squareRoot(test.input) == test.result, "native sqrt exact integer mismatch");
            ++mathematical; ++roots;
        }
        for (unsigned i = 0; i < 4096; ++i) {
            compact(m);
            const U f = U(random());
            const D d = random();
            m.f[12] = original::fp(f); m.run(0x311038);
            need(native::widen(f) == m.get64(2), "standalone widening mismatch");
            call(m, 0x310440, d);
            need(native::narrow(d) == original::bits(m.f[0]), "standalone narrowing mismatch");
            conversions += 2;
        }
        for (U poison = 0; poison < 256; ++poison) {
            for (U at = m.stack - 0x8000; at < m.stack; at += 4)
                m.init(at, poison * 0x01010101u);
            for (U x : {0u, 0x80000000u, 1u, 0x3f800001u, 0xbf800001u, 0x7fc00000u}) {
                chain(m, x);
                ++poisoned;
            }
        }

        for (unsigned kind = 0; kind < 5; ++kind) {
            SourceLibmState49T state;
            state.errnoValue = -987;
            if (kind < 3) state.mode = kind == 0 ? -1 : kind == 1 ? 0 : 2;
            else if (kind == 3) state.defaultMatherr = false;
            else state.coldNanRecord = false;
            const auto saved = state;
            U output = 0xaabbccdd;
            SourceAcosTrace49T trace{11, 22, 33};
            need(!sourceAcos49T(0x3f800001, state, output, &trace), "unsupported contract accepted");
            need(state.mode == saved.mode && state.errnoValue == saved.errnoValue &&
                 state.defaultMatherr == saved.defaultMatherr && state.coldNanRecord == saved.coldNanRecord,
                 "rejected contract changed state");
            need(output == 0xaabbccdd && trace.widened == 11 && trace.inner == 22 && trace.wrapped == 33,
                 "rejected contract changed output or trace");
        }
        SourceLibmState49T shared;
        shared.errnoValue = -17;
        U result = 99;
        need(sourceAcos49T(0x3f800001, shared, result) && result == 0 && shared.errnoValue == 33,
             "domain +0/errno33 state sequence");
        need(sourceAcos49T(0, shared, result) && result == 0x3fc90fdb && shared.errnoValue == 33,
             "valid call did not preserve preceding errno");
        need(sourceAcos49T(0x7fc00000, shared, result) && result == 0x7fb00000 && shared.errnoValue == 33,
             "source NaN payload or errno sequence");

        std::uint64_t distinct = 0;
        for (auto count : m.hits) distinct += count != 0;
        if (argc == 3 && std::string(argv[1]) == "--coverage") {
            std::ofstream out(argv[2]);
            need(bool(out), "cannot write requested coverage");
            for (const auto& block : acos49t_words::blocks)
                for (U i = 0; i < block.count; ++i) {
                    const auto at = block.index + i;
                    if (m.hits[at]) out << std::hex << block.start + 4 * i << ' ' << block.words[i]
                        << std::dec << ' ' << m.hits[at] << ' ' << m.taken[at] << ' ' << m.notTaken[at] << '\n';
                }
        } else need(argc == 1, "unsupported arguments");
        std::cout << "{\"status\":\"pass\",\"checks\":" << checks
            << ",\"actual_arm_arguments\":" << actual << ",\"unique_synthetic_and_actual_inputs\":" << inputs.size()
            << ",\"full_chains\":" << chains << ",\"primitive_cases\":" << primitives
            << ",\"standalone_conversion_cases\":" << conversions << ",\"exact_sqrt_cases\":" << roots
            << ",\"independent_exact_cases\":" << mathematical << ",\"poisoned_chains\":" << poisoned
            << ",\"poison_patterns\":256,\"original_instructions\":" << m.steps
            << ",\"original_distinct_pcs\":" << distinct << "}\n";
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
}
