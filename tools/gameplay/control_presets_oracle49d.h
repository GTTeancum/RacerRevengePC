#pragma once
// Test-only interpreter for frozen original controller instructions. No production
// header, row table, reconstructed formula, device/RAM, or dynamic allocation.
#include "control_presets_words49d.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace original49d {
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using Row = std::array<u32, 10>;
inline constexpr u32 configBase = 0x10000u;
inline constexpr u32 outputBase = 0x50000u;
struct State {
    std::array<Row, 2> rows{};
    std::array<u32, 2> presets{}, modes{};
    std::array<u32, 2> field10F94{}, field10F9C{}, field10FA4{};
    std::array<u32, 2> field11030{}, field11038{};
};
static_assert(sizeof(State) == 136, "49D state is exactly34 original32-bit fields");
struct InstructionTrace { u32 pc = 0, word = 0; bool delaySlot = false; };
struct MemoryTrace { u32 pc = 0, address = 0, value = 0; bool write = false; };
struct BranchTrace { u32 pc = 0, target = 0; bool taken = false; };
struct Stats {
    std::array<InstructionTrace, 512> instructions{};
    std::array<MemoryTrace, 512> memory{};
    std::array<BranchTrace, 128> branches{};
    std::array<unsigned, 225> selectVisits{};
    std::array<unsigned, 37> defaultVisits{};
    std::array<unsigned, 64> getterVisits{};
    std::size_t instructionCount = 0, memoryCount = 0, branchCount = 0;
    unsigned steps = 0, delaySlots = 0, selectorCalls = 0;
    unsigned visits(u32 pc) const {
        if (pc & 3u) return 0;
        if (pc >= rr_select_49d_start && pc < rr_select_49d_end)
            return selectVisits[(pc - rr_select_49d_start) / 4];
        if (pc >= rr_defaults_49d_start && pc < rr_defaults_49d_end)
            return defaultVisits[(pc - rr_defaults_49d_start) / 4];
        if (pc >= rr_getter_49d_start && pc < rr_getter_49d_end)
            return getterVisits[(pc - rr_getter_49d_start) / 4];
        return 0;
    }
};

namespace detail {
inline std::int32_t signed32(u32 value) {
    std::int32_t result; std::memcpy(&result, &value, sizeof(result)); return result;
}
inline std::int64_t signed64(u64 value) {
    std::int64_t result; std::memcpy(&result, &value, sizeof(result)); return result;
}
inline u64 signExtend(u32 value) {
    return static_cast<u64>(static_cast<std::int64_t>(signed32(value)));
}
// Configuration, stack, output and immutable jump table are separate bounded
// synthetic mappings. Only State's two native-admitted slots are entry inputs.
class Machine {
    static constexpr u32 configBytes = 0x11100u;
    static constexpr u32 stackBase = 0x40000u, stackBytes = 0x200u;
    static constexpr u32 initialSP = stackBase + stackBytes;
    static constexpr u32 returnSentinel = 0x0f000000u;
    std::array<u32, configBytes / 4> config{};
    std::array<u32, stackBytes / 4> stack{};
    std::array<u32, 16> output{};
    std::array<u64, 32> g{}, high{};
    std::array<u32, 32> f{};
    Stats stats{};

    u32 readRaw(u32 address) const {
        if (address & 3u) throw std::runtime_error("49D unaligned fixture read");
        if (address >= configBase && address < configBase + configBytes)
            return config[(address - configBase) / 4];
        if (address >= stackBase && address < stackBase + stackBytes)
            return stack[(address - stackBase) / 4];
        if (address >= outputBase && address < outputBase + output.size() * 4)
            return output[(address - outputBase) / 4];
        if (address >= rr_jump_table_49d_start && address < rr_jump_table_49d_end)
            return rr_jump_table_49d[(address - rr_jump_table_49d_start) / 4];
        throw std::runtime_error("49D read outside bounded synthetic mapping");
    }
    void writeRaw(u32 address, u32 value) {
        if (address & 3u) throw std::runtime_error("49D unaligned fixture write");
        if (address >= configBase && address < configBase + configBytes)
            config[(address - configBase) / 4] = value;
        else if (address >= stackBase && address < stackBase + stackBytes)
            stack[(address - stackBase) / 4] = value;
        else if (address >= outputBase && address < outputBase + output.size() * 4)
            output[(address - outputBase) / 4] = value;
        else throw std::runtime_error("49D write outside bounded synthetic mapping");
    }
    void traceMemory(u32 pc, u32 address, u32 value, bool write) {
        if (stats.memoryCount == stats.memory.size())
            throw std::runtime_error("49D fixed memory trace exhausted");
        stats.memory[stats.memoryCount++] = {pc, address, value, write};
    }
    u32 load(u32 pc, u32 address) {
        const u32 value = readRaw(address); traceMemory(pc, address, value, false); return value;
    }
    void store(u32 pc, u32 address, u32 value) {
        writeRaw(address, value); traceMemory(pc, address, value, true);
    }
    u32 fetch(u32 pc, bool delay) {
        if (pc & 3u) throw std::runtime_error("49D unaligned instruction address");
        u32 word = 0;
        if (pc >= rr_select_49d_start && pc < rr_select_49d_end) {
            const auto i = (pc - rr_select_49d_start) / 4;
            ++stats.selectVisits[i]; word = rr_select_49d[i];
            if (pc == rr_select_49d_start) ++stats.selectorCalls;
        } else if (pc >= rr_defaults_49d_start && pc < rr_defaults_49d_end) {
            const auto i = (pc - rr_defaults_49d_start) / 4;
            ++stats.defaultVisits[i]; word = rr_defaults_49d[i];
        } else if (pc >= rr_getter_49d_start && pc < rr_getter_49d_end) {
            const auto i = (pc - rr_getter_49d_start) / 4;
            ++stats.getterVisits[i]; word = rr_getter_49d[i];
        } else throw std::runtime_error("49D PC outside selected original words");
        if (stats.instructionCount == stats.instructions.size())
            throw std::runtime_error("49D fixed instruction trace exhausted");
        stats.instructions[stats.instructionCount++] = {pc, word, delay};
        ++stats.steps; if (delay) ++stats.delaySlots;
        return word;
    }
    void traceBranch(u32 pc, u32 target, bool taken) {
        if (stats.branchCount == stats.branches.size())
            throw std::runtime_error("49D fixed branch trace exhausted");
        stats.branches[stats.branchCount++] = {pc, target, taken};
    }
    struct Flow { bool delayed = false; u32 next = 0; };
    Flow step(u32 pc, bool delay) {
        const u32 w = fetch(pc, delay);
        const unsigned op = w >> 26, rs = (w >> 21) & 31u, rt = (w >> 16) & 31u;
        const unsigned rd = (w >> 11) & 31u, sa = (w >> 6) & 31u, fn = w & 63u;
        const std::int32_t imm = static_cast<std::int32_t>(w & 0xffffu) - ((w & 0x8000u) ? 0x10000 : 0);
        const u32 address = static_cast<u32>(g[rs]) + static_cast<u32>(imm);
        Flow flow{};
        if (op == 4 || op == 5) {
            const bool taken = op == 4 ? g[rs] == g[rt] : g[rs] != g[rt];
            const u32 target = pc + 4u + static_cast<u32>(imm) * 4u;
            flow = {true, taken ? target : pc + 8u};
            traceBranch(pc, target, taken);
        } else if (op == 3) {
            const u32 target = ((pc + 4u) & 0xf0000000u) | ((w & 0x3ffffffu) << 2u);
            g[31] = signExtend(pc + 8u); flow = {true, target};
            traceBranch(pc, target, true);
        } else if (op == 0 && fn == 8) {
            flow = {true, static_cast<u32>(g[rs])};
            traceBranch(pc, flow.next, true);
        } else if (op == 9) g[rt] = signExtend(address);
        else if (op == 10) g[rt] = signed64(g[rs]) < static_cast<std::int64_t>(imm) ? 1u : 0u;
        else if (op == 11) g[rt] = g[rs] < static_cast<u64>(static_cast<std::int64_t>(imm)) ? 1u : 0u;
        else if (op == 13) g[rt] = g[rs] | static_cast<u64>(w & 0xffffu);
        else if (op == 15) g[rt] = signExtend((w & 0xffffu) << 16u);
        else if (op == 0x23) g[rt] = signExtend(load(pc, address));
        else if (op == 0x2b) store(pc, address, static_cast<u32>(g[rt]));
        else if (op == 0x31) f[rt] = load(pc, address);
        else if (op == 0x39) store(pc, address, f[rt]);
        else if (op == 0x1f) {
            if (address & 15u) throw std::runtime_error("49D unaligned128-bit save");
            store(pc, address, static_cast<u32>(g[rt]));
            store(pc, address + 4u, static_cast<u32>(g[rt] >> 32u));
            store(pc, address + 8u, static_cast<u32>(high[rt]));
            store(pc, address + 12u, static_cast<u32>(high[rt] >> 32u));
        } else if (op == 0x1e) {
            if (address & 15u) throw std::runtime_error("49D unaligned128-bit restore");
            const auto lo = load(pc, address), hi = load(pc, address + 4u);
            const auto upperLo = load(pc, address + 8u), upperHi = load(pc, address + 12u);
            g[rt] = u64(lo) | (u64(hi) << 32u);
            high[rt] = u64(upperLo) | (u64(upperHi) << 32u);
        } else if (op == 0 && fn == 0) g[rd] = signExtend(static_cast<u32>(g[rt]) << sa);
        else if (op == 0 && fn == 0x21)
            g[rd] = signExtend(static_cast<u32>(g[rs]) + static_cast<u32>(g[rt]));
        else if (op == 0x1c && fn == 0x28 && sa == 0x18) {
            g[rd] = g[rs] | g[rt]; high[rd] = high[rs] | high[rt];
        } else throw std::runtime_error("49D unsupported selected original instruction");
        g[0] = 0; high[0] = 0;
        if (delay && flow.delayed) throw std::runtime_error("49D control transfer in delay slot");
        return flow;
    }
    void run(u32 pc, u32 stop) {
        while (pc != stop) {
            const auto flow = step(pc, false);
            if (flow.delayed) {
                step(pc + 4u, true);
                pc = flow.next;
            } else pc += 4u;
        }
    }
    void encode(const State& state) {
        for (unsigned i = 0; i < 2; ++i) {
            for (unsigned j = 0; j < 10; ++j)
                writeRaw(configBase + 0x690u + 40u * i + 4u * j, state.rows[i][j]);
            writeRaw(configBase + 0x11048u + 4u * i, state.presets[i]);
            writeRaw(configBase + 0x11040u + 4u * i, state.modes[i]);
            writeRaw(configBase + 0x10f94u + 4u * i, state.field10F94[i]);
            writeRaw(configBase + 0x10f9cu + 4u * i, state.field10F9C[i]);
            writeRaw(configBase + 0x10fa4u + 4u * i, state.field10FA4[i]);
            writeRaw(configBase + 0x11030u + 4u * i, state.field11030[i]);
            writeRaw(configBase + 0x11038u + 4u * i, state.field11038[i]);
        }
    }
    void decode(State& state) const {
        for (unsigned i = 0; i < 2; ++i) {
            for (unsigned j = 0; j < 10; ++j)
                state.rows[i][j] = readRaw(configBase + 0x690u + 40u * i + 4u * j);
            state.presets[i] = readRaw(configBase + 0x11048u + 4u * i);
            state.modes[i] = readRaw(configBase + 0x11040u + 4u * i);
            state.field10F94[i] = readRaw(configBase + 0x10f94u + 4u * i);
            state.field10F9C[i] = readRaw(configBase + 0x10f9cu + 4u * i);
            state.field10FA4[i] = readRaw(configBase + 0x10fa4u + 4u * i);
            state.field11030[i] = readRaw(configBase + 0x11030u + 4u * i);
            state.field11038[i] = readRaw(configBase + 0x11038u + 4u * i);
        }
    }
    void publish(State& state, Stats* out) const {
        decode(state); if (out) *out = stats;
    }
public:
    explicit Machine(const State& state) {
        config.fill(0xc3d25a69u); stack.fill(0xbada5510u); output.fill(0xdeadbeefu);
        for (unsigned i = 1; i < 32; ++i) {
            g[i] = 0x1234567800000000ull + i;
            high[i] = 0xabcdef1200000000ull + i;
        }
        g[29] = initialSP; g[31] = returnSentinel;
        encode(state);
    }
    void select(State& state, u32 slot, u32 preset, Stats* out) {
        // This is a fixture bound, not an invented instruction/native guard.
        if (slot >= 2) throw std::invalid_argument("49D fixture admits exactly slots0/1");
        g[4] = configBase; g[5] = signExtend(slot); g[6] = signExtend(preset);
        run(rr_select_49d_start, returnSentinel);
        publish(state, out);
    }
    void defaults(State& state, Stats* out) {
        g[18] = configBase;
        run(rr_defaults_49d_start, rr_defaults_49d_end);
        if (g[16] != 2u || g[17] != configBase + 8u)
            throw std::runtime_error("49D local default-loop exit registers differ");
        publish(state, out);
    }
    Row getter(State& state, u32 requested, Stats* out) {
        const auto beforeG = g, beforeHigh = high;
        g[4] = outputBase; g[5] = configBase; g[6] = signExtend(requested);
        run(rr_getter_49d_start, returnSentinel);
        for (const unsigned i : {16u, 17u, 18u, 29u, 31u}) {
            if (g[i] != beforeG[i] || high[i] != beforeHigh[i])
                throw std::runtime_error("49D original getter failed128-bit saved-state restore");
        }
        Row result{};
        for (unsigned i = 0; i < result.size(); ++i) result[i] = readRaw(outputBase + 4u * i);
        for (unsigned i = 10; i < output.size(); ++i)
            if (output[i] != 0xdeadbeefu) throw std::runtime_error("49D output tail changed");
        publish(state, out);
        return result;
    }
};
} // namespace detail

inline void select(State& state, u32 slot, u32 preset, Stats* out = nullptr) {
    detail::Machine machine(state); machine.select(state, slot, preset, out);
}
inline void defaults(State& state, Stats* out = nullptr) {
    detail::Machine machine(state); machine.defaults(state, out);
}
inline Row getter(State& state, u32 requested, Stats* out = nullptr) {
    detail::Machine machine(state); return machine.getter(state, requested, out);
}
} // namespace original49d
