#pragma once
// Test-only instruction oracle. Executes only frozen original ELF words against
// synthetic memory; application/child callbacks are explicit fixture boundaries.
// No production header, gameplay implementation, or earlier oracle is included.
#include "event_dispatch_words48z.h"
#include <array>
#include <cstdint>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>

namespace original48z {
using u32 = std::uint32_t;
inline std::int32_t signedWord(u32 word) {
    std::int32_t value;
    std::memcpy(&value, &word, sizeof(value));
    return value;
}
inline std::int64_t signedLong(std::uint64_t word) {
    std::int64_t value;
    std::memcpy(&value, &word, sizeof(value));
    return value;
}
struct Event { std::array<u32, 5> words{}; };
struct Record { std::array<u32, 6> words{}; };
struct ManagerState {
    std::array<Event, 64> events{};
    std::int32_t count = 0;
    bool contextPresent = false;
    u32 contextMask = 0;
    bool auxiliaryPresent = false;
    std::array<bool, 8> devicePresent{};
    std::array<Record, 32> contextRecords{}, auxiliaryRecords{};
    std::array<std::array<Record, 32>, 8> deviceRecords{};
};
inline constexpr u32 noChild = std::numeric_limits<u32>::max();
struct Node { u32 next = noChild; std::uint8_t flags29 = 0; };
struct RouterState {
    u32 parentFlags24 = 0, firstChild = noChild;
    std::array<Node, 32> nodes{};
    Event event{};
    Record record{};
};
struct CallbackTrace {
    u32 pc = 0;
    bool press = false;
    unsigned eventIndex = 0, category = 0, device = 0, code = 0;
    unsigned childIndex = noChild;
    u32 eventAddress = 0, recordAddress = 0, result = 0;
    Event eventBefore{}, eventAfter{};
    Record recordBefore{}, recordAfter{};
};
struct Stats {
    std::array<unsigned, (rr_manager_48z_end - rr_manager_48z_start) / 4> managerVisits{};
    std::array<unsigned, (rr_release_router_48z_end - rr_release_router_48z_start) / 4> releaseRouterVisits{};
    std::array<unsigned, (rr_press_router_48z_end - rr_press_router_48z_start) / 4> pressRouterVisits{};
    unsigned steps = 0, delaySlots = 0, annulledDelaySlots = 0, callbackCalls = 0;
    std::vector<CallbackTrace> callbacks;
    unsigned visits(u32 pc) const {
        if (pc & 3u) return 0;
        if (pc >= rr_manager_48z_start && pc < rr_manager_48z_end)
            return managerVisits[(pc - rr_manager_48z_start) / 4];
        if (pc >= rr_release_router_48z_start && pc < rr_release_router_48z_end)
            return releaseRouterVisits[(pc - rr_release_router_48z_start) / 4];
        if (pc >= rr_press_router_48z_start && pc < rr_press_router_48z_end)
            return pressRouterVisits[(pc - rr_press_router_48z_start) / 4];
        return 0;
    }
};

struct Machine {
    static constexpr u32 managerAddress = 0x1000, application = 0x2000, applicationTable = 0x2800;
    static constexpr u32 context = 0x4000, auxiliary = 0x6000, devices = 0x8000;
    static constexpr u32 parent = 0x11000, children = 0x12000, routerEvent = 0x14000;
    static constexpr u32 routerRecord = 0x14100, childTable = 0x15000;
    static constexpr u32 initialStack = 0x1f000, sentinel = 0x0f000000;
    static constexpr u32 releaseHook = 0x0e000000, pressHook = 0x0e000004;
    std::array<std::uint64_t, 32> g{}, upper{};
    std::array<u32, 0x20000 / 4> memory{};
    Stats stats{};
    bool managerMode = false;

    static std::uint64_t signExtend(u32 value) {
        return static_cast<std::uint64_t>(static_cast<std::int64_t>(signedWord(value)));
    }
    u32 read(u32 address) const {
        if (address == 0x382b80) return application;
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("48Z read outside aligned synthetic fixture");
        return memory[address / 4];
    }
    void write(u32 address, u32 value) {
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("48Z write outside aligned synthetic fixture");
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
    static u32 deviceAddress(unsigned index) { return devices + 0x1000 * index; }
    static u32 eventAddress(unsigned index) { return managerAddress + 0x3c + 20 * index; }
    static u32 childAddress(u32 index) {
        if (index == noChild) return 0;
        if (index >= 32) throw std::runtime_error("48Z child index outside fixture");
        return children + 0x80 * index;
    }
    static u32 childIndex(u32 address) {
        if (address == 0) return noChild;
        if (address < children || address >= children + 32 * 0x80 || (address - children) % 0x80)
            throw std::runtime_error("48Z child pointer outside fixture");
        return (address - children) / 0x80;
    }
    void encodeManager(const ManagerState& state) {
        // A negative signed count has no iterations in the original loop.
        if (state.count > 64) throw std::runtime_error("48Z event count exceeds fixture");
        write(managerAddress + 0x38, static_cast<u32>(state.count));
        write(managerAddress + 0x34, state.contextPresent ? context : 0);
        write(managerAddress + 0x30, state.auxiliaryPresent ? auxiliary : 0);
        write(context + 0x1898, state.contextMask);
        write(application + 0x27c, applicationTable);
        write(applicationTable + 0x3c, releaseHook);
        write(applicationTable + 0x40, pressHook);
        for (unsigned i = 0; i < state.events.size(); ++i) writeObject(eventAddress(i), state.events[i]);
        for (unsigned i = 0; i < 32; ++i) {
            writeObject(context + 0x20 + 24 * i, state.contextRecords[i]);
            writeObject(auxiliary + 0x1c + 24 * i, state.auxiliaryRecords[i]);
        }
        for (unsigned d = 0; d < 8; ++d) {
            write(managerAddress + 0x10 + 4 * d, state.devicePresent[d] ? deviceAddress(d) : 0);
            for (unsigned i = 0; i < 32; ++i)
                writeObject(deviceAddress(d) + 0x324 + 24 * i, state.deviceRecords[d][i]);
        }
    }
    void decodeManager(ManagerState& state) const {
        state.count = signedWord(read(managerAddress + 0x38));
        state.contextPresent = read(managerAddress + 0x34) != 0;
        state.contextMask = read(context + 0x1898);
        state.auxiliaryPresent = read(managerAddress + 0x30) != 0;
        for (unsigned i = 0; i < state.events.size(); ++i) state.events[i] = readObject<Event>(eventAddress(i));
        for (unsigned i = 0; i < 32; ++i) {
            state.contextRecords[i] = readObject<Record>(context + 0x20 + 24 * i);
            state.auxiliaryRecords[i] = readObject<Record>(auxiliary + 0x1c + 24 * i);
        }
        for (unsigned d = 0; d < 8; ++d) {
            state.devicePresent[d] = read(managerAddress + 0x10 + 4 * d) != 0;
            for (unsigned i = 0; i < 32; ++i)
                state.deviceRecords[d][i] = readObject<Record>(deviceAddress(d) + 0x324 + 24 * i);
        }
    }
    void encodeRouter(const RouterState& state) {
        write(parent + 0x24, state.parentFlags24);
        write(parent + 0x18, childAddress(state.firstChild));
        write(childTable + 0x60, releaseHook);
        write(childTable + 0x64, pressHook);
        for (unsigned i = 0; i < state.nodes.size(); ++i) {
            const auto address = childAddress(i);
            write(address + 8, childTable);
            write(address + 0x14, childAddress(state.nodes[i].next));
            writeByte(address + 0x29, state.nodes[i].flags29);
        }
        writeObject(routerEvent, state.event);
        writeObject(routerRecord, state.record);
    }
    void decodeRouter(RouterState& state) const {
        state.parentFlags24 = read(parent + 0x24);
        state.firstChild = childIndex(read(parent + 0x18));
        for (unsigned i = 0; i < state.nodes.size(); ++i) {
            const auto address = childAddress(i);
            state.nodes[i].next = childIndex(read(address + 0x14));
            state.nodes[i].flags29 = readByte(address + 0x29);
        }
        state.event = readObject<Event>(routerEvent);
        state.record = readObject<Record>(routerRecord);
    }
    u32 word(u32 pc) {
        if (pc & 3u) throw std::runtime_error("48Z unaligned instruction address");
        if (managerMode && pc >= rr_manager_48z_start && pc < rr_manager_48z_end) {
            const auto i = (pc - rr_manager_48z_start) / 4;
            ++stats.managerVisits[i]; return rr_manager_48z[i];
        }
        if (!managerMode && pc >= rr_release_router_48z_start && pc < rr_release_router_48z_end) {
            const auto i = (pc - rr_release_router_48z_start) / 4;
            ++stats.releaseRouterVisits[i]; return rr_release_router_48z[i];
        }
        if (!managerMode && pc >= rr_press_router_48z_start && pc < rr_press_router_48z_end) {
            const auto i = (pc - rr_press_router_48z_start) / 4;
            ++stats.pressRouterVisits[i]; return rr_press_router_48z[i];
        }
        throw std::runtime_error("48Z PC outside executed frozen intervals");
    }
    struct Transfer { bool control = false, delay = false; u32 target = 0; };
    Transfer execute(u32 pc) {
        if (++stats.steps > 200000) throw std::runtime_error("48Z instruction budget exceeded");
        // Guards constrain fixture accesses; they do not replace original branches.
        if (pc == 0x128164 && g[16] >= 64) throw std::runtime_error("48Z event index outside fixture");
        if (pc == 0x12819c && g[4] > 2) throw std::runtime_error("48Z category outside admitted domain");
        if ((pc == 0x128324 || pc == 0x12838c) &&
            (g[16] >= 64 || read(eventAddress(static_cast<unsigned>(g[16])) + 8) >= 8))
            throw std::runtime_error("48Z device index outside fixture");
        const auto w = word(pc), op = w >> 26, rs = (w >> 21) & 31u;
        const auto rt = (w >> 16) & 31u, rd = (w >> 11) & 31u;
        const auto sa = (w >> 6) & 31u, fn = w & 63u;
        const auto imm = signedWord((w & 0x8000u) ? (w | 0xffff0000u) : (w & 0xffffu));
        const auto address = static_cast<u32>(g[rs]) + static_cast<u32>(imm);
        const auto branch = [&](bool taken, bool likely = false) -> Transfer {
            if (likely && !taken) ++stats.annulledDelaySlots;
            return {true, !likely || taken, taken ? pc + 4 + static_cast<u32>(imm * 4) : pc + 8};
        };
        if (w == 0) return {};
        if (op == 9) g[rt] = signExtend(address);
        else if (op == 0xc) g[rt] = g[rs] & (w & 0xffffu);
        else if (op == 0xf) g[rt] = signExtend((w & 0xffffu) << 16);
        else if (op == 0x23) g[rt] = signExtend(read(address));
        else if (op == 0x24) g[rt] = readByte(address);
        else if (op == 0x2b) write(address, static_cast<u32>(g[rt]));
        else if (op == 0x1e || op == 0x1f) {
            if (address & 15u) throw std::runtime_error("48Z unaligned quadword access");
            if (op == 0x1e) {
                g[rt] = read(address) | (std::uint64_t(read(address + 4)) << 32);
                upper[rt] = read(address + 8) | (std::uint64_t(read(address + 12)) << 32);
            } else {
                write(address, static_cast<u32>(g[rt]));
                write(address + 4, static_cast<u32>(g[rt] >> 32));
                write(address + 8, static_cast<u32>(upper[rt]));
                write(address + 12, static_cast<u32>(upper[rt] >> 32));
            }
        }
        else if (op == 0x1c && fn == 0x28 && sa == 0x18) {
            g[rd] = g[rs] | g[rt]; upper[rd] = upper[rs] | upper[rt];
        }
        else if (op == 4) return branch(g[rs] == g[rt]);
        else if (op == 5) return branch(g[rs] != g[rt]);
        else if (op == 0x15) return branch(g[rs] != g[rt], true);
        else if (op == 0 && fn == 8) return {true, true, static_cast<u32>(g[rs])};
        else if (op == 0 && fn == 9) {
            const auto target = static_cast<u32>(g[rs]);
            if (target != releaseHook && target != pressHook)
                throw std::runtime_error("48Z unknown virtual callback target");
            g[rd] = pc + 8;
            return {true, true, target};
        }
        else if (op == 0 && fn == 0) g[rd] = signExtend(static_cast<u32>(g[rt]) << sa);
        else if (op == 0 && fn == 0x21) g[rd] = signExtend(static_cast<u32>(g[rs] + g[rt]));
        else if (op == 0 && fn == 0x2a) g[rd] = signedLong(g[rs]) < signedLong(g[rt]) ? 1 : 0;
        else if (op == 0 && fn == 0x3c) g[rd] = g[rt] << (sa + 32);
        else if (op == 0 && fn == 0x3e) g[rd] = g[rt] >> (sa + 32);
        else throw std::runtime_error("48Z unsupported instruction outside admitted flow");
        g[0] = 0; upper[0] = 0;
        return {};
    }
    template<class Hook> void run(u32 start, Hook&& hook) {
        auto pc = start;
        const auto stop = managerMode ? rr_manager_48z_end : sentinel;
        while (pc != stop) {
            if (pc == releaseHook || pc == pressHook) {
                const auto returnPC = static_cast<u32>(g[31]);
                g[2] = signExtend(hook(returnPC - 8, pc == pressHook));
                ++stats.callbackCalls;
                pc = returnPC;
                continue;
            }
            const auto transfer = execute(pc);
            if (transfer.control) {
                if (transfer.delay) {
                    ++stats.delaySlots;
                    if (execute(pc + 4).control)
                        throw std::runtime_error("48Z control transfer in delay slot");
                }
                pc = transfer.target;
            } else pc += 4;
        }
    }
};

template<class Callback>
void manager(ManagerState& state, Callback&& callback, Stats* statistics = nullptr) {
    Machine machine;
    machine.managerMode = true;
    machine.encodeManager(state);
    machine.g[16] = 0;
    machine.g[18] = Machine::managerAddress;
    machine.g[29] = Machine::initialStack;
    machine.g[31] = Machine::sentinel;
    machine.run(rr_manager_48z_start, [&](u32 pc, bool press) -> u32 {
        machine.decodeManager(state);
        unsigned category;
        if (pc == 0x1281f8 || pc == 0x12824c) category = 0;
        else if (pc == 0x128294 || pc == 0x1282ec) category = 1;
        else if (pc == 0x128340 || pc == 0x1283a0) category = 2;
        else throw std::runtime_error("48Z unknown manager callback site");
        const auto index = static_cast<unsigned>(machine.g[16]);
        if (index >= state.events.size() || machine.g[4] != Machine::application ||
            machine.g[5] != Machine::eventAddress(index))
            throw std::runtime_error("48Z manager callback event identity");
        const auto code = state.events[index].words[0], device = state.events[index].words[2];
        if (code >= 32) throw std::runtime_error("48Z record index outside fixture");
        Record* record;
        u32 recordAddress;
        if (category == 0) {
            if (!state.contextPresent) throw std::runtime_error("48Z null category0 record owner");
            record = &state.contextRecords[code];
            recordAddress = Machine::context + 0x20 + 24 * code;
        } else if (category == 1) {
            if (!state.auxiliaryPresent) throw std::runtime_error("48Z null category1 record owner");
            record = &state.auxiliaryRecords[code];
            recordAddress = Machine::auxiliary + 0x1c + 24 * code;
        } else {
            if (device >= 8 || !state.devicePresent[device])
                throw std::runtime_error("48Z invalid category2 record owner");
            record = &state.deviceRecords[device][code];
            recordAddress = Machine::deviceAddress(device) + 0x324 + 24 * code;
        }
        if (machine.g[6] != recordAddress) throw std::runtime_error("48Z original record pointer mismatch");
        CallbackTrace trace;
        trace.pc = pc; trace.press = press; trace.eventIndex = index;
        trace.category = category; trace.device = device; trace.code = code;
        trace.eventAddress = Machine::eventAddress(index); trace.recordAddress = recordAddress;
        trace.eventBefore = state.events[index]; trace.recordBefore = *record;
        trace.result = callback(state, press, index, category, device, code, *record);
        trace.eventAfter = state.events[index]; trace.recordAfter = *record;
        machine.stats.callbacks.push_back(trace);
        machine.encodeManager(state);
        return trace.result;
    });
    machine.decodeManager(state);
    if (statistics) *statistics = machine.stats;
}

template<class Callback>
bool router(RouterState& state, bool press, Callback&& callback, Stats* statistics = nullptr) {
    Machine machine;
    machine.encodeRouter(state);
    machine.g[4] = Machine::parent;
    machine.g[5] = Machine::routerEvent;
    machine.g[6] = Machine::routerRecord;
    machine.g[29] = Machine::initialStack;
    machine.g[31] = Machine::sentinel;
    machine.upper[31] = 0x1234fedcba987654ull;
    for (unsigned i = 16; i <= 18; ++i) {
        machine.g[i] = 0x1234567800000000ull + i;
        machine.upper[i] = 0xabcdef0100000000ull + i;
    }
    machine.run(press ? rr_press_router_48z_start : rr_release_router_48z_start,
                [&](u32 pc, bool callbackPress) -> u32 {
        machine.decodeRouter(state);
        const auto index = Machine::childIndex(static_cast<u32>(machine.g[4]));
        if (index == noChild || callbackPress != press ||
            pc != (press ? 0x1423b4u : 0x142304u) ||
            machine.g[5] != Machine::routerEvent || machine.g[6] != Machine::routerRecord)
            throw std::runtime_error("48Z router callback identity");
        CallbackTrace trace;
        trace.pc = pc; trace.press = press; trace.childIndex = index;
        trace.category = state.event.words[1]; trace.device = state.event.words[2];
        trace.code = state.event.words[0]; trace.eventAddress = Machine::routerEvent;
        trace.recordAddress = Machine::routerRecord;
        trace.eventBefore = state.event; trace.recordBefore = state.record;
        trace.result = callback(state, index, press);
        trace.eventAfter = state.event; trace.recordAfter = state.record;
        machine.stats.callbacks.push_back(trace);
        machine.encodeRouter(state);
        return trace.result;
    });
    if (machine.g[29] != Machine::initialStack || machine.g[31] != Machine::sentinel ||
        machine.upper[31] != 0x1234fedcba987654ull)
        throw std::runtime_error("48Z router did not restore full return frame");
    for (unsigned i = 16; i <= 18; ++i)
        if (machine.g[i] != 0x1234567800000000ull + i || machine.upper[i] != 0xabcdef0100000000ull + i)
            throw std::runtime_error("48Z router did not restore callee-saved register");
    machine.decodeRouter(state);
    if (statistics) *statistics = machine.stats;
    return machine.g[2] != 0;
}
} // namespace original48z
