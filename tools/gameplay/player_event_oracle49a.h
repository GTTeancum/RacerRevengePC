#pragma once
// Test-only instruction oracle. No production headers or formula helpers.
// Executes every reachable word of the three frozen complete functions. External
// provider/player/router effects are synchronous fixture callbacks, never guessed.
#include "player_event_words49a.h"
#include <array>
#include <cfenv>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>

namespace original49a {
using u32 = std::uint32_t;
inline u32 bits(float value) { u32 word; std::memcpy(&word, &value, 4); return word; }
inline float fp(u32 word) { float value; std::memcpy(&value, &word, 4); return value; }
inline std::int32_t signedWord(u32 word) {
    std::int32_t value; std::memcpy(&value, &word, 4); return value;
}
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559,
              "49A oracle requires IEEE binary32 floats");
struct Event { std::array<u32, 5> words{}; };
struct Record { std::array<u32, 6> words{}; };
struct PlayerState {
    u32 field40 = 0, source5C = 0, indexFFC = 0;
    u32 value1098 = 0, value1094 = 0, value1090 = 0;
    u32 valueCB4 = 0, valueCB0 = 0, valueCB8 = 0, countCD0 = 0;
};
struct ApplicationState { std::uint8_t flags29 = 0, gate58 = 0; };
struct ManagerState {
    bool contextPresent = false, auxiliaryPresent = false;
    std::array<bool, 8> devicePresent{};
};
struct TuningRow { u32 value102F0 = 0, word10388 = 0; };
struct ConfigState {
    std::array<u32, 8> action6AC{};
    std::uint8_t selection1125F = 0;
    std::array<TuningRow, 16> tuning{};
};
struct State {
    PlayerState player{};
    ApplicationState application{};
    ManagerState manager{};
    ConfigState config{};
    std::uint8_t lazyFlag85F4 = 0;
    u32 lazyWord4340A0 = 0;
    Event event{};
    Record record{};
};
enum class CallKind {
    SourceQuery, Player2255B0, PlayerVirtual,
    Player22B170, Player22B370, Player22B570, Player22B760, Player22BA60,
    Tail142350
};
struct Call {
    CallKind kind = CallKind::SourceQuery;
    u32 pc = 0, target = 0, callerPC = 0;
    u32 category = 0, source = 0, code = 0, mask = 0, slot = 0;
    // Raw arguments actually live at the callback boundary. f12 is meaningful
    // to 22B170/22B370; a1=12 is meaningful to 22BA60. Other live values are not
    // invented helper signatures. Event/record identities are used only at tail.
    u32 a0 = 0, a1 = 0, a2 = 0, a3 = 0, t0 = 0, f12Bits = 0;
    u32 eventAddress = 0, recordAddress = 0;
};
struct CallbackTrace { Call call{}; State before{}, after{}; u32 result = 0; };
struct QueryTrace {
    u32 callerPC = 0, code = 0, category = 0, mask = 0, source = 0;
    u32 result = 0;
    bool providerCalled = false;
};
struct InstructionTrace { u32 pc = 0, word = 0; bool delaySlot = false; };
struct BranchTrace { u32 pc = 0, target = 0; bool taken = false, likely = false; };
struct MemoryTrace { u32 pc = 0, address = 0, value = 0; unsigned bytes = 0; bool write = false; };
struct Stats {
    std::array<unsigned, 204> playerVisits{};
    std::array<unsigned, 56> managerVisits{};
    std::array<unsigned, 12> gateVisits{};
    unsigned steps = 0, delaySlots = 0, annulledDelaySlots = 0;
    unsigned queryCalls = 0, providerCalls = 0, gateCalls = 0, callbackCalls = 0;
    std::vector<InstructionTrace> instructions;
    std::vector<BranchTrace> branches;
    std::vector<MemoryTrace> memory;
    std::vector<QueryTrace> queries;
    std::vector<CallbackTrace> callbacks;
    unsigned visits(u32 pc) const {
        if (pc & 3u) return 0;
        if (pc >= rr_player_49a_start && pc < rr_player_49a_end)
            return playerVisits[(pc - rr_player_49a_start) / 4];
        if (pc >= rr_manager_49a_start && pc < rr_manager_49a_end)
            return managerVisits[(pc - rr_manager_49a_start) / 4];
        if (pc >= rr_gate_49a_start && pc < rr_gate_49a_end)
            return gateVisits[(pc - rr_gate_49a_start) / 4];
        return 0;
    }
};
inline constexpr std::array<u32, 4> unreachablePCs{{0x1a1840, 0x1a1864, 0x12854c, 0x1285f0}};

struct Machine {
    static constexpr u32 player = 0x1000, application = 0x3000, manager = 0x4000;
    static constexpr u32 owner = 0x5000, gateObject = 0x6000;
    static constexpr u32 context = 0x7000, auxiliary = 0x7100, devices = 0x8000;
    static constexpr u32 managerTable = 0x9000, providerTable = 0x9100, playerTable = 0x9200;
    static constexpr u32 config = 0x10000, event = 0x28000, record = 0x28100;
    static constexpr u32 initialStack = 0x30000, globalPointer = 0x80000;
    static constexpr u32 lazyByteAddress = globalPointer - 0x7a0c;
    static constexpr u32 sentinel = 0x0f000000;
    static constexpr u32 query10Hook = 0x0e000010, query18Hook = 0x0e000018;
    static constexpr u32 virtual88Hook = 0x0e000088, virtual8CHook = 0x0e00008c;
    static constexpr u32 virtual90Hook = 0x0e000090, virtual94Hook = 0x0e000094;
    std::array<std::uint64_t, 32> g{}, upper{};
    std::array<u32, 32> f{};
    std::array<u32, 0x40000 / 4> memory{};
    u32 lazyWord = 0;
    std::uint8_t lazyByte = 0;
    bool condition = false, queryActive = false;
    u32 queryReturnPC = 0;
    std::size_t queryIndex = 0;
    Stats stats{};
    static std::uint64_t signExtend(u32 word) {
        return static_cast<std::uint64_t>(static_cast<std::int64_t>(signedWord(word)));
    }
    static u32 deviceAddress(unsigned index) { return devices + 0x100 * index; }
    u32 read(u32 address) const {
        if (address == 0x382b80) return application;
        if (address == 0x4340a0) return lazyWord;
        if (address == lazyByteAddress) return lazyByte;
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("49A read outside aligned synthetic fixture");
        return memory[address / 4];
    }
    void write(u32 address, u32 value) {
        if (address == 0x4340a0) { lazyWord = value; return; }
        if (address == lazyByteAddress) { lazyByte = static_cast<std::uint8_t>(value); return; }
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("49A write outside aligned synthetic fixture");
        memory[address / 4] = value;
    }
    std::uint8_t readByte(u32 address) const {
        return static_cast<std::uint8_t>(read(address & ~3u) >> ((address & 3u) * 8));
    }
    void writeByte(u32 address, std::uint8_t value) {
        const auto shift = (address & 3u) * 8;
        write(address & ~3u, (read(address & ~3u) & ~(0xffu << shift)) | (u32(value) << shift));
    }
    template<class Object> void writeObject(u32 address, const Object& object) {
        for (unsigned i = 0; i < object.words.size(); ++i) write(address + 4 * i, object.words[i]);
    }
    template<class Object> Object readObject(u32 address) const {
        Object object;
        for (unsigned i = 0; i < object.words.size(); ++i) object.words[i] = read(address + 4 * i);
        return object;
    }
    void encode(const State& state) {
        const auto& p = state.player;
        write(player + 0x40, p.field40); write(player + 0x5c, p.source5C); write(player + 0xffc, p.indexFFC);
        write(player + 0x1098, p.value1098); write(player + 0x1094, p.value1094); write(player + 0x1090, p.value1090);
        write(player + 0xcb4, p.valueCB4); write(player + 0xcb0, p.valueCB0); write(player + 0xcb8, p.valueCB8);
        write(player + 0xcd0, p.countCD0);
        write(player + 8, playerTable);
        write(playerTable + 0x88, virtual88Hook); write(playerTable + 0x8c, virtual8CHook);
        write(playerTable + 0x90, virtual90Hook); write(playerTable + 0x94, virtual94Hook);
        write(application + 4, manager); write(application + 0x28c, owner); write(application + 0x2dc, config);
        writeByte(owner + 0x29, state.application.flags29); write(owner + 0xb4, gateObject);
        writeByte(gateObject + 0x58, state.application.gate58);
        write(manager + 0xcc0, managerTable); write(managerTable + 0x10, rr_manager_49a_start);
        write(manager + 0x34, state.manager.contextPresent ? context : 0);
        write(manager + 0x30, state.manager.auxiliaryPresent ? auxiliary : 0);
        write(context + 0x18, providerTable); write(auxiliary + 0x18, providerTable);
        write(providerTable + 0x10, query10Hook); write(providerTable + 0x18, query18Hook);
        for (unsigned i = 0; i < 8; ++i) {
            write(manager + 0x10 + 4 * i, state.manager.devicePresent[i] ? deviceAddress(i) : 0);
            write(deviceAddress(i) + 0x18, providerTable);
            write(config + 0x6ac + 40 * i, state.config.action6AC[i]);
        }
        writeByte(config + 0x1125f, state.config.selection1125F);
        for (unsigned i = 0; i < 16; ++i) {
            write(config + 0x102f0 + 0xe4 * i, state.config.tuning[i].value102F0);
            write(config + 0x10388 + 0xe4 * i, state.config.tuning[i].word10388);
        }
        lazyByte = state.lazyFlag85F4; lazyWord = state.lazyWord4340A0;
        writeObject(event, state.event); writeObject(record, state.record);
    }
    void decode(State& state) const {
        auto& p = state.player;
        p.field40 = read(player + 0x40); p.source5C = read(player + 0x5c); p.indexFFC = read(player + 0xffc);
        p.value1098 = read(player + 0x1098); p.value1094 = read(player + 0x1094); p.value1090 = read(player + 0x1090);
        p.valueCB4 = read(player + 0xcb4); p.valueCB0 = read(player + 0xcb0); p.valueCB8 = read(player + 0xcb8);
        p.countCD0 = read(player + 0xcd0);
        state.application.flags29 = readByte(owner + 0x29); state.application.gate58 = readByte(gateObject + 0x58);
        state.manager.contextPresent = read(manager + 0x34) != 0;
        state.manager.auxiliaryPresent = read(manager + 0x30) != 0;
        for (unsigned i = 0; i < 8; ++i) {
            state.manager.devicePresent[i] = read(manager + 0x10 + 4 * i) != 0;
            state.config.action6AC[i] = read(config + 0x6ac + 40 * i);
        }
        state.config.selection1125F = readByte(config + 0x1125f);
        for (unsigned i = 0; i < 16; ++i) {
            state.config.tuning[i].value102F0 = read(config + 0x102f0 + 0xe4 * i);
            state.config.tuning[i].word10388 = read(config + 0x10388 + 0xe4 * i);
        }
        state.lazyFlag85F4 = lazyByte; state.lazyWord4340A0 = lazyWord;
        state.event = readObject<Event>(event); state.record = readObject<Record>(record);
    }
    u32 word(u32 pc) {
        if (pc & 3u) throw std::runtime_error("49A unaligned instruction address");
        if (pc >= rr_player_49a_start && pc < rr_player_49a_end) {
            const auto i = (pc - rr_player_49a_start) / 4;
            ++stats.playerVisits[i]; return rr_player_49a[i];
        }
        if (pc >= rr_manager_49a_start && pc < rr_manager_49a_end) {
            const auto i = (pc - rr_manager_49a_start) / 4;
            ++stats.managerVisits[i]; return rr_manager_49a[i];
        }
        if (pc >= rr_gate_49a_start && pc < rr_gate_49a_end) {
            const auto i = (pc - rr_gate_49a_start) / 4;
            ++stats.gateVisits[i]; return rr_gate_49a[i];
        }
        throw std::runtime_error("49A PC outside frozen intervals");
    }
    static bool isHook(u32 target) {
        return target == query10Hook || target == query18Hook || target == virtual88Hook ||
               target == virtual8CHook || target == virtual90Hook || target == virtual94Hook ||
               target == 0x2255b0 || target == 0x22b170 || target == 0x22b370 || target == 0x22b570 ||
               target == 0x22b760 || target == 0x22ba60 || target == 0x142350;
    }
    struct Transfer { bool control = false, delay = false; u32 target = 0; };
    Transfer execute(u32 pc, bool delaySlot = false) {
        if (++stats.steps > 10000) throw std::runtime_error("49A instruction budget exceeded");
        // Guards constrain only declared fixture accesses, never implement game branches.
        if (pc == 0x1a1740 && read(player + 0xffc) >= 8)
            throw std::runtime_error("49A action configuration row outside fixture");
        if (pc == 0x128550 && g[8] >= 8)
            throw std::runtime_error("49A category2 source index outside native range0..7");
        const auto w = word(pc), op = w >> 26, rs = (w >> 21) & 31u;
        const auto rt = (w >> 16) & 31u, rd = (w >> 11) & 31u;
        const auto sa = (w >> 6) & 31u, fn = w & 63u;
        const auto imm = signedWord((w & 0x8000u) ? (w | 0xffff0000u) : (w & 0xffffu));
        const auto address = static_cast<u32>(g[rs]) + static_cast<u32>(imm);
        stats.instructions.push_back({pc, w, delaySlot});
        const auto branch = [&](bool taken, bool likely = false) -> Transfer {
            if (likely && !taken) ++stats.annulledDelaySlots;
            const auto target = taken ? pc + 4 + static_cast<u32>(imm * 4) : pc + 8;
            stats.branches.push_back({pc, target, taken, likely});
            return {true, !likely || taken, target};
        };
        const auto load = [&](u32 where, unsigned bytes = 4) -> u32 {
            const auto value = bytes == 1 ? static_cast<u32>(readByte(where)) : read(where);
            stats.memory.push_back({pc, where, value, bytes, false}); return value;
        };
        const auto save = [&](u32 where, u32 value, unsigned bytes = 4) {
            if (bytes == 1) { value &= 0xffu; writeByte(where, static_cast<std::uint8_t>(value)); }
            else write(where, value);
            stats.memory.push_back({pc, where, value, bytes, true});
        };
        if (w == 0) return {};
        if (op == 9) g[rt] = signExtend(address);
        else if (op == 0xc) g[rt] = g[rs] & (w & 0xffffu);
        else if (op == 0xd) g[rt] = g[rs] | (w & 0xffffu);
        else if (op == 0xe) g[rt] = g[rs] ^ (w & 0xffffu);
        else if (op == 0xf) g[rt] = signExtend((w & 0xffffu) << 16);
        else if (op == 0x23) g[rt] = signExtend(load(address));
        else if (op == 0x24) g[rt] = load(address, 1);
        else if (op == 0x20) {
            const auto byte = load(address, 1);
            g[rt] = signExtend(byte & 0x80u ? byte | 0xffffff00u : byte);
        }
        else if (op == 0x28) save(address, static_cast<u32>(g[rt]), 1);
        else if (op == 0x2b) save(address, static_cast<u32>(g[rt]));
        else if (op == 0x31) f[rt] = load(address);
        else if (op == 0x39) save(address, f[rt]);
        else if (op == 0x1e || op == 0x1f) {
            if (address & 15u) throw std::runtime_error("49A unaligned quadword access");
            if (op == 0x1e) {
                g[rt] = load(address) | (std::uint64_t(load(address + 4)) << 32);
                upper[rt] = load(address + 8) | (std::uint64_t(load(address + 12)) << 32);
            } else {
                save(address, static_cast<u32>(g[rt])); save(address + 4, static_cast<u32>(g[rt] >> 32));
                save(address + 8, static_cast<u32>(upper[rt])); save(address + 12, static_cast<u32>(upper[rt] >> 32));
            }
        }
        else if (op == 0x1c && fn == 0x28 && sa == 0x18) {
            g[rd] = g[rs] | g[rt]; upper[rd] = upper[rs] | upper[rt];
        }
        else if (op == 4) return branch(g[rs] == g[rt]);
        else if (op == 5) return branch(g[rs] != g[rt]);
        else if (op == 0x14) return branch(g[rs] == g[rt], true);
        else if (op == 3) {
            const auto target = ((pc + 4) & 0xf0000000u) | ((w & 0x3ffffffu) << 2);
            if (target != rr_gate_49a_start && !isHook(target))
                throw std::runtime_error("49A unexpected direct-call target");
            if (target == rr_gate_49a_start) ++stats.gateCalls;
            g[31] = pc + 8; return {true, true, target};
        }
        else if (op == 0 && (fn == 8 || fn == 9)) {
            const auto target = static_cast<u32>(g[rs]);
            if (fn == 9) {
                if (target != rr_manager_49a_start && !isHook(target))
                    throw std::runtime_error("49A unexpected virtual-call target");
                g[rd] = pc + 8;
            }
            return {true, true, target};
        }
        else if (op == 0 && fn == 0) g[rd] = signExtend(static_cast<u32>(g[rt]) << sa);
        else if (op == 0 && fn == 0x21) g[rd] = signExtend(static_cast<u32>(g[rs] + g[rt]));
        else if (op == 0 && fn == 0x23) g[rd] = signExtend(static_cast<u32>(g[rs] - g[rt]));
        else if (op == 0 && fn == 0x2b) g[rd] = g[rs] < g[rt] ? 1 : 0;
        else if (op == 0 && fn == 0x3c) g[rd] = g[rt] << (sa + 32);
        else if (op == 0 && fn == 0x3e) g[rd] = g[rt] >> (sa + 32);
        else if (op == 0x11) {
            if (rs == 4) f[rd] = static_cast<u32>(g[rt]);
            else if (rs == 8 && (rt == 0 || rt == 1)) return branch(condition == (rt == 1));
            else if (rs == 16) {
                if ((f[rd] & 0x7f800000u) == 0x7f800000u || (f[rt] & 0x7f800000u) == 0x7f800000u)
                    throw std::runtime_error("49A arithmetic operands outside finite binary32 domain");
                const float left = fp(f[rd]), right = fp(f[rt]);
                if (fn == 0x34) condition = left < right;
                else if (fn == 1) {
                    // Volatile operands/result force this single SUB.S rounding step.
                    volatile float lhs = left, rhs = right;
                    volatile float result = lhs - rhs;
                    f[sa] = bits(result);
                    if ((f[sa] & 0x7f800000u) == 0x7f800000u)
                        throw std::runtime_error("49A arithmetic result outside finite binary32 domain");
                } else throw std::runtime_error("49A unsupported floating instruction");
            } else throw std::runtime_error("49A unsupported COP1 format");
        }
        else throw std::runtime_error("49A unsupported original instruction");
        g[0] = 0; upper[0] = 0;
        return {};
    }
    Call call(u32 target) {
        Call call;
        call.pc = static_cast<u32>(g[31]) - 8; call.target = target;
        call.a0 = static_cast<u32>(g[4]); call.a1 = static_cast<u32>(g[5]);
        call.a2 = static_cast<u32>(g[6]); call.a3 = static_cast<u32>(g[7]);
        call.t0 = static_cast<u32>(g[8]); call.f12Bits = f[12];
        if (target == query10Hook || target == query18Hook) {
            if (!queryActive) throw std::runtime_error("49A source callback without original selector");
            auto& q = stats.queries[queryIndex];
            call.kind = CallKind::SourceQuery; call.callerPC = q.callerPC;
            call.code = q.code; call.category = q.category; call.mask = q.mask; call.source = q.source;
            call.slot = q.category == 2 ? 0x10 : 0x18;
            const auto pointer = q.category == 0 ? context : q.category == 1 ? auxiliary : deviceAddress(q.source);
            if (call.a0 != pointer || call.a1 != q.code || call.a2 != q.mask || call.a3 != 0 ||
                target != (q.category == 2 ? query10Hook : query18Hook))
                throw std::runtime_error("49A provider callback argument mismatch");
            q.providerCalled = true; ++stats.providerCalls;
            return call;
        }
        if (call.a0 != player) throw std::runtime_error("49A helper callback player identity mismatch");
        if (target == virtual88Hook || target == virtual8CHook || target == virtual90Hook || target == virtual94Hook) {
            call.kind = CallKind::PlayerVirtual; call.slot = target - 0x0e000000u;
        } else if (target == 0x2255b0) call.kind = CallKind::Player2255B0;
        else if (target == 0x22b170) call.kind = CallKind::Player22B170;
        else if (target == 0x22b370) call.kind = CallKind::Player22B370;
        else if (target == 0x22b570) call.kind = CallKind::Player22B570;
        else if (target == 0x22b760) call.kind = CallKind::Player22B760;
        else if (target == 0x22ba60) {
            call.kind = CallKind::Player22BA60;
            if (call.a1 != 12) throw std::runtime_error("49A 22BA60 fixed argument mismatch");
        } else if (target == 0x142350) {
            call.kind = CallKind::Tail142350;
            call.eventAddress = call.a1; call.recordAddress = call.a2;
            if (call.eventAddress != event || call.recordAddress != record)
                throw std::runtime_error("49A tail incoming event/record identity mismatch");
        } else throw std::runtime_error("49A unknown callback target");
        return call;
    }
    template<class Hook> void run(u32 start, State& state, Hook&& hook) {
        auto pc = start;
        while (true) {
            if (queryActive && pc == queryReturnPC) {
                stats.queries[queryIndex].result = static_cast<u32>(g[2]);
                queryActive = false;
            }
            if (pc == sentinel) break;
            if (pc == rr_manager_49a_start) {
                if (queryActive || g[4] != manager) throw std::runtime_error("49A selector entry identity");
                queryActive = true; queryReturnPC = static_cast<u32>(g[31]);
                queryIndex = stats.queries.size();
                stats.queries.push_back({queryReturnPC == sentinel ? 0 : queryReturnPC - 8,
                    static_cast<u32>(g[5]), static_cast<u32>(g[6]), static_cast<u32>(g[7]), static_cast<u32>(g[8]), 0, false});
                ++stats.queryCalls;
            }
            if (isHook(pc)) {
                const auto returnPC = static_cast<u32>(g[31]);
                decode(state);
                CallbackTrace trace; trace.call = call(pc); trace.before = state;
                trace.result = static_cast<u32>(hook(state, trace.call));
                trace.after = state;
                stats.callbacks.push_back(trace); ++stats.callbackCalls;
                encode(state);
                g[2] = signExtend(trace.result);
                pc = returnPC;
                continue;
            }
            const auto transfer = execute(pc);
            if (transfer.control) {
                if (transfer.delay) {
                    ++stats.delaySlots;
                    if (execute(pc + 4, true).control)
                        throw std::runtime_error("49A control transfer in delay slot");
                }
                pc = transfer.target;
            } else pc += 4;
        }
        if (queryActive) throw std::runtime_error("49A unterminated held query");
    }
    void initialize(const State& state) {
        const auto rounding = std::fegetround();
        if (rounding != FE_TONEAREST && rounding != FE_DOWNWARD && rounding != FE_UPWARD && rounding != FE_TOWARDZERO)
            throw std::runtime_error("49A unsupported floating rounding mode");
        encode(state);
        g[28] = globalPointer; g[29] = initialStack; g[31] = sentinel;
        upper[31] = 0x1234fedcba987654ull;
        for (unsigned i = 16; i <= 18; ++i) {
            g[i] = 0x1234567800000000ull + i;
            upper[i] = 0xabcdef0100000000ull + i;
        }
    }
    void checkFrame() const {
        if (g[29] != initialStack || g[31] != sentinel || upper[31] != 0x1234fedcba987654ull)
            throw std::runtime_error("49A function did not restore full return frame");
        for (unsigned i = 16; i <= 18; ++i)
            if (g[i] != 0x1234567800000000ull + i || upper[i] != 0xabcdef0100000000ull + i)
                throw std::runtime_error("49A function did not restore callee-saved128-bit register");
    }
};

// Hook signature: u32(State& liveState, const Call& originalArguments).
// Return values from player helpers are recorded but the frozen continuation
// ignores them; queries are normalized by original selector words, tail v0 is
// returned unchanged. Only the tail receives incoming event/record identities.
template<class Hook> u32 playerEvent(State& state, Hook&& hook, Stats* statistics = nullptr) {
    Machine machine;
    machine.initialize(state);
    machine.g[4] = Machine::player; machine.g[5] = Machine::event; machine.g[6] = Machine::record;
    machine.run(rr_player_49a_start, state, hook);
    machine.checkFrame(); machine.decode(state);
    if (statistics) *statistics = machine.stats;
    return static_cast<u32>(machine.g[2]);
}

template<class Hook> u32 managerHeld(State& state, u32 code, u32 category, u32 mask, u32 source,
                                    Hook&& hook, Stats* statistics = nullptr) {
    Machine machine;
    machine.initialize(state);
    machine.g[4] = Machine::manager; machine.g[5] = Machine::signExtend(code);
    machine.g[6] = Machine::signExtend(category); machine.g[7] = Machine::signExtend(mask);
    machine.g[8] = Machine::signExtend(source);
    machine.run(rr_manager_49a_start, state, hook);
    machine.checkFrame(); machine.decode(state);
    if (statistics) *statistics = machine.stats;
    return static_cast<u32>(machine.g[2]);
}

inline u32 stateGate(std::uint8_t gateByte, Stats* statistics = nullptr) {
    State state; state.application.gate58 = gateByte;
    Machine machine; machine.initialize(state); machine.g[4] = Machine::gateObject;
    machine.run(rr_gate_49a_start, state, [](State&, const Call&) -> u32 {
        throw std::runtime_error("49A gate unexpectedly invoked external callback");
    });
    machine.checkFrame();
    if (statistics) *statistics = machine.stats;
    return static_cast<u32>(machine.g[2]);
}
} // namespace original49a
