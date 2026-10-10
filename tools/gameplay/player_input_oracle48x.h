#pragma once
// Test only. Execute exact frozen ELF words against bounded synthetic fixtures.
// No native gameplay headers or formula helpers participate in this oracle.
// The virtual device query is explicitly unknown external input supplied by the
// test. The direct state-gate call executes its original instructions instead.
#include "player_input_words48x.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace original48x {
inline std::uint32_t bits(float value) {
    std::uint32_t word;
    std::memcpy(&word, &value, sizeof(word));
    return word;
}
inline float fp(std::uint32_t word) {
    float value;
    std::memcpy(&value, &word, sizeof(value));
    return value;
}
inline std::int32_t signedWord(std::uint32_t word) {
    std::int32_t value;
    std::memcpy(&value, &word, sizeof(value));
    return value;
}

struct AxisParams {
    float minimum = -1.0f, maximum = 1.0f, center = 0.0f;
    float upper = 0.1f, lower = -0.1f;
};
struct ReaderInput {
    std::array<float, 4> components{};
    // Original settings slots, in this order: 694, 690, 698, 69c, 6b4.
    std::array<std::int32_t, 5> codes{{-1, -1, -1, -1, -1}};
    std::uint8_t gateByte = 0, previousFlags = 0;
    float source7D0 = 0, source7D4 = 0, source7D8 = 0, source7DC = 0;
    std::int32_t deviceIndex = 0;
};
struct Query {
    std::int32_t code = 0;
    std::uint32_t mask = 0;
    bool matched = false;
};
struct ReaderResult {
    std::array<float, 4> raw{};
    float actionB7C = 0, actionB80 = 0;
    std::uint8_t flags = 0;
    std::array<Query, 7> trace{};
    unsigned queryCount = 0;
};
struct Stats {
    std::array<unsigned, 37> axisVisits{};
    std::array<unsigned, 235> readerVisits{};
    std::array<unsigned, 12> gateVisits{};
    unsigned steps = 0, queryCalls = 0, gateCalls = 0;
    unsigned delaySlots = 0, annulledDelaySlots = 0;

    unsigned visits(std::uint32_t pc) const {
        if (pc & 3u) return 0;
        if (pc >= rr_axis_48x_start && pc < rr_axis_48x_end)
            return axisVisits[(pc - rr_axis_48x_start) / 4];
        if (pc >= rr_reader_48x_start && pc < rr_reader_48x_end)
            return readerVisits[(pc - rr_reader_48x_start) / 4];
        if (pc >= rr_gate_48x_start && pc < rr_gate_48x_end)
            return gateVisits[(pc - rr_gate_48x_start) / 4];
        return 0;
    }
};

struct Machine {
    static constexpr std::uint32_t object = 0x400;
    static constexpr std::uint32_t components = 0x1800;
    static constexpr std::uint32_t device = 0x1a00;
    static constexpr std::uint32_t vtable = 0x1b00;
    static constexpr std::uint32_t root = 0x1c00;
    static constexpr std::uint32_t stateOwner = 0x2000;
    static constexpr std::uint32_t state = 0x2200;
    static constexpr std::uint32_t initialStack = 0x7800;
    static constexpr std::uint32_t settings = 0x10000000;
    static constexpr std::uint32_t queryTarget = 0x0f000010;
    std::array<std::uint64_t, 32> g{};
    // EE GPR upper halves are only copied by POR and saved by SQ in this slice.
    std::array<std::uint64_t, 32> upper{};
    std::array<std::uint32_t, 32> f{};
    std::array<std::uint32_t, 0x8000 / 4> memory{};
    std::array<std::uint32_t, 5> settingsAddresses{}, settingsValues{};
    bool hasSettings = false, condition = false;
    Stats stats{};
    ReaderResult result{};

    std::uint32_t read(std::uint32_t address) const {
        if (address & 3u) throw std::runtime_error("48X unaligned synthetic read");
        if (hasSettings) {
            if (address == 0x382b80) return root;
            for (std::size_t i = 0; i < settingsAddresses.size(); ++i)
                if (address == settingsAddresses[i]) return settingsValues[i];
        }
        if (address / 4 >= memory.size())
            throw std::runtime_error("48X read outside synthetic fixture");
        return memory[address / 4];
    }
    void write(std::uint32_t address, std::uint32_t value) {
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("48X write outside synthetic fixture");
        memory[address / 4] = value;
    }
    std::uint8_t readByte(std::uint32_t address) const {
        return static_cast<std::uint8_t>(read(address & ~3u) >> ((address & 3u) * 8));
    }
    void writeByte(std::uint32_t address, std::uint8_t value) {
        const auto shift = (address & 3u) * 8;
        write(address & ~3u, (read(address & ~3u) & ~(0xffu << shift)) |
                                 (static_cast<std::uint32_t>(value) << shift));
    }
    std::uint32_t word(std::uint32_t pc) {
        if (pc & 3u) throw std::runtime_error("48X unaligned instruction address");
        if (pc >= rr_axis_48x_start && pc < rr_axis_48x_end) {
            const auto index = (pc - rr_axis_48x_start) / 4;
            ++stats.axisVisits[index];
            return rr_axis_48x[index];
        }
        if (pc >= rr_reader_48x_start && pc < rr_reader_48x_end) {
            const auto index = (pc - rr_reader_48x_start) / 4;
            ++stats.readerVisits[index];
            return rr_reader_48x[index];
        }
        if (pc >= rr_gate_48x_start && pc < rr_gate_48x_end) {
            const auto index = (pc - rr_gate_48x_start) / 4;
            ++stats.gateVisits[index];
            return rr_gate_48x[index];
        }
        throw std::runtime_error("48X PC outside frozen intervals");
    }
    static std::uint64_t signExtend(std::uint32_t value) {
        return static_cast<std::uint64_t>(static_cast<std::int64_t>(signedWord(value)));
    }
    struct Transfer {
        bool taken = false;
        bool delay = false;
        std::uint32_t target = 0;
    };

    Transfer execute(std::uint32_t pc) {
        if (++stats.steps > 2000) throw std::runtime_error("48X instruction budget");
        const auto w = word(pc), op = w >> 26, rs = (w >> 21) & 31u;
        const auto rt = (w >> 16) & 31u, rd = (w >> 11) & 31u;
        const auto sa = (w >> 6) & 31u, fn = w & 63u;
        const auto immediate = signedWord((w & 0x8000u) ? (w | 0xffff0000u) : (w & 0xffffu));
        const auto address = static_cast<std::uint32_t>(g[rs]) + static_cast<std::uint32_t>(immediate);
        const auto branch = [&](bool take, bool likely = false) -> Transfer {
            if (likely && !take) ++stats.annulledDelaySlots;
            return {true, !likely || take,
                    take ? pc + 4 + static_cast<std::uint32_t>(immediate * 4) : pc + 8};
        };
        if (w == 0) return {};
        if (op == 9) g[rt] = signExtend(address);
        else if (op == 0x19) g[rt] = g[rs] + static_cast<std::uint64_t>(static_cast<std::int64_t>(immediate));
        else if (op == 0xc) g[rt] = g[rs] & (w & 0xffffu);
        else if (op == 0xd) g[rt] = g[rs] | (w & 0xffffu);
        else if (op == 0xe) g[rt] = g[rs] ^ (w & 0xffffu);
        else if (op == 0xf) g[rt] = signExtend((w & 0xffffu) << 16);
        else if (op == 0x23) g[rt] = signExtend(read(address));
        else if (op == 0x24) g[rt] = readByte(address);
        else if (op == 0x28) writeByte(address, static_cast<std::uint8_t>(g[rt]));
        else if (op == 0x2b) write(address, static_cast<std::uint32_t>(g[rt]));
        else if (op == 0x31) f[rt] = read(address);
        else if (op == 0x39) write(address, f[rt]);
        else if (op == 0x1f) {
            if (address & 15u) throw std::runtime_error("48X unaligned quadword save");
            write(address, static_cast<std::uint32_t>(g[rt]));
            write(address + 4, static_cast<std::uint32_t>(g[rt] >> 32));
            write(address + 8, static_cast<std::uint32_t>(upper[rt]));
            write(address + 12, static_cast<std::uint32_t>(upper[rt] >> 32));
        }
        else if (op == 0x1c && fn == 0x28 && sa == 0x18) {
            g[rd] = g[rs] | g[rt];
            upper[rd] = upper[rs] | upper[rt];
        }
        else if (op == 4) return branch(g[rs] == g[rt]);
        else if (op == 5) return branch(g[rs] != g[rt]);
        else if (op == 0x14) return branch(g[rs] == g[rt], true);
        else if (op == 3) {
            const auto target = ((pc + 4) & 0xf0000000u) | ((w & 0x3ffffffu) << 2);
            // There is no direct-call stub, even for the small gate helper.
            if (target != rr_gate_48x_start) throw std::runtime_error("48X unexpected direct call");
            ++stats.gateCalls;
            g[31] = pc + 8;
            return {true, true, target};
        }
        else if (op == 0 && (fn == 8 || fn == 9)) {
            const auto target = static_cast<std::uint32_t>(g[rs]);
            if (fn == 9) {
                if (target != queryTarget) throw std::runtime_error("48X unexpected virtual call");
                g[rd] = pc + 8;
            }
            g[0] = 0;
            return {true, true, target};
        }
        else if (op == 0 && fn == 0) g[rd] = signExtend(static_cast<std::uint32_t>(g[rt]) << sa);
        else if (op == 0 && fn == 0x21) g[rd] = signExtend(static_cast<std::uint32_t>(g[rs] + g[rt]));
        else if (op == 0 && fn == 0x24) g[rd] = g[rs] & g[rt];
        else if (op == 0 && fn == 0x25) g[rd] = g[rs] | g[rt];
        else if (op == 0 && fn == 0x2b) g[rd] = g[rs] < g[rt] ? 1 : 0;
        else if (op == 0 && fn == 0x3c) g[rd] = g[rt] << (sa + 32);
        // fn 0x3e is DSRL32, an unsigned 64-bit shift, NOT DSRA32.
        else if (op == 0 && fn == 0x3e) g[rd] = g[rt] >> (sa + 32);
        else if (op == 0x11) {
            if (rs == 4) f[rd] = static_cast<std::uint32_t>(g[rt]);
            else if (rs == 8 && (rt == 0 || rt == 1)) return branch(condition == (rt == 1));
            else if (rs == 16) {
                const float left = fp(f[rd]), right = fp(f[rt]);
                // Each arithmetic instruction rounds separately to binary32.
                volatile float value = 0;
                switch (fn) {
                    case 0: value = left + right; break;
                    case 1: value = left - right; break;
                    case 2: value = left * right; break;
                    case 3: value = left / right; break;
                    case 0x34: condition = left < right; return {};
                    case 0x36: condition = left <= right; return {};
                    default: throw std::runtime_error("48X unsupported FPU instruction");
                }
                f[sa] = bits(value);
            }
            else throw std::runtime_error("48X unsupported COP1 format");
        }
        else throw std::runtime_error("48X unsupported instruction");
        g[0] = 0;
        upper[0] = 0;
        return {};
    }

    template<class QueryProvider>
    void run(std::uint32_t start, std::uint32_t stop, QueryProvider& query) {
        auto pc = start;
        while (pc != stop) {
            if (pc == queryTarget) {
                if (!hasSettings || g[4] != device || g[6] != 0x3fu ||
                    g[7] != initialStack - 0x20 || result.queryCount >= result.trace.size())
                    throw std::runtime_error("48X external-query fixture contract");
                const auto code = signedWord(static_cast<std::uint32_t>(g[5]));
                const auto mask = static_cast<std::uint32_t>(g[6]);
                const bool matched = static_cast<bool>(query(code, mask));
                result.trace[result.queryCount++] = {code, mask, matched};
                ++stats.queryCalls;
                g[2] = matched ? 1 : 0;
                pc = static_cast<std::uint32_t>(g[31]);
                continue;
            }
            const auto transfer = execute(pc);
            if (transfer.taken) {
                if (transfer.delay) {
                    ++stats.delaySlots;
                    if (execute(pc + 4).taken)
                        throw std::runtime_error("48X control transfer in delay slot");
                }
                pc = transfer.target;
            }
            else pc += 4;
        }
    }

    void prepareReader(const ReaderInput& input) {
        hasSettings = true;
        constexpr std::array<std::uint32_t, 5> offsets{{0x694, 0x690, 0x698, 0x69c, 0x6b4}};
        for (std::size_t i = 0; i < offsets.size(); ++i) {
            // Modular 32-bit addresses permit independent index-routing tests.
            // Reject only indices that collide with another synthetic region.
            settingsAddresses[i] = settings + 40u * static_cast<std::uint32_t>(input.deviceIndex) + offsets[i];
            if (settingsAddresses[i] < memory.size() * 4 || settingsAddresses[i] == 0x382b80)
                throw std::runtime_error("48X settings index aliases synthetic memory");
            settingsValues[i] = static_cast<std::uint32_t>(input.codes[i]);
        }
        g[4] = object;
        g[29] = initialStack;
        for (unsigned i = 0; i < 4; ++i) {
            const auto component = components + 0x40 * i;
            write(object + 0xfe0 + 4 * i, component);
            write(component + 0x24, bits(input.components[i]));
        }
        write(object + 0xff0, device);
        write(object + 0xffc, static_cast<std::uint32_t>(input.deviceIndex));
        write(device + 0x18, vtable);
        write(vtable + 0x10, queryTarget);
        write(root + 0x2dc, settings);
        write(root + 0x28c, stateOwner);
        write(stateOwner + 0xb4, state);
        writeByte(state + 0x58, input.gateByte);
        writeByte(object + 0xb9c, input.previousFlags);
        write(object + 0x7d0, bits(input.source7D0));
        write(object + 0x7d4, bits(input.source7D4));
        write(object + 0x7d8, bits(input.source7D8));
        write(object + 0x7dc, bits(input.source7DC));
    }
};

inline float axis(float numerator, float denominator, const AxisParams& parameters,
                  Stats* statistics = nullptr) {
    Machine machine;
    machine.g[16] = Machine::object;
    machine.f[21] = bits(numerator);
    machine.f[20] = bits(denominator);
    machine.write(Machine::object + 0x2c, bits(parameters.minimum));
    machine.write(Machine::object + 0x30, bits(parameters.maximum));
    machine.write(Machine::object + 0x28, bits(parameters.center));
    machine.write(Machine::object + 0x34, bits(parameters.upper));
    machine.write(Machine::object + 0x38, bits(parameters.lower));
    auto noQuery = [](std::int32_t, std::uint32_t) -> bool {
        throw std::runtime_error("48X axis unexpectedly called external query");
    };
    machine.run(rr_axis_48x_start, rr_axis_48x_end, noQuery);
    if (statistics) *statistics = machine.stats;
    return fp(machine.read(Machine::object + 0x24));
}

template<class QueryProvider>
ReaderResult reader(const ReaderInput& input, QueryProvider&& query, Stats* statistics = nullptr) {
    Machine machine;
    machine.prepareReader(input);
    machine.run(rr_reader_48x_start, rr_reader_48x_end, query);
    for (unsigned i = 0; i < 4; ++i)
        machine.result.raw[i] = fp(machine.read(Machine::object + 0xb5c + 4 * i));
    machine.result.actionB7C = fp(machine.read(Machine::object + 0xb7c));
    machine.result.actionB80 = fp(machine.read(Machine::object + 0xb80));
    machine.result.flags = machine.readByte(Machine::object + 0xb9c);
    if (statistics) *statistics = machine.stats;
    return machine.result;
}
} // namespace original48x
