#pragma once
// Test only: original ELF words executed against bounded synthetic memory.
// No production gameplay headers or formulas participate. SDK packet read and
// axis update are explicit supplied-input/trace hooks; gate and queue execute.
#include "device_held_words48y.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace original48y {
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
inline std::int64_t signedLong(std::uint64_t word) {
    std::int64_t value;
    std::memcpy(&value, &word, sizeof(value));
    return value;
}
struct Record { std::array<std::uint32_t, 6> words{}; };
struct Descriptor {
    std::uint8_t flags = 0;
    std::uint32_t packetOffset = 0;
    std::uint16_t mask = 0;
};
struct QueryContext {
    bool present = false;
    std::uint8_t rootFlags228 = 0;
    std::uint32_t mode518 = 0, mask1898 = 0;
};
struct QueryResult { bool matched = false; Record copied{}; };
struct AxisUpdate {
    std::uint32_t axis = 0;
    float numerator = 0, denominator = 0;
};
struct SampleResult {
    std::array<Record, 32> records{};
    std::array<std::array<std::uint32_t, 5>, 32> events{};
    unsigned eventCount = 0;
    bool packetAccepted = false;
    std::array<AxisUpdate, 6> axisUpdates{};
    unsigned axisUpdateCount = 0;
};
struct Stats {
    std::array<unsigned, 61> queryVisits{};
    std::array<unsigned, 49> gateVisits{};
    std::array<unsigned, 188> packetVisits{};
    std::array<unsigned, 35> queueVisits{};
    unsigned steps = 0, gateCalls = 0, readCalls = 0, axisCalls = 0, queueCalls = 0;
    unsigned delaySlots = 0, annulledDelaySlots = 0;
    unsigned visits(std::uint32_t pc) const {
        if (pc & 3u) return 0;
        if (pc >= rr_query_48y_start && pc < rr_query_48y_end)
            return queryVisits[(pc - rr_query_48y_start) / 4];
        if (pc >= rr_gate_48y_start && pc < rr_gate_48y_end)
            return gateVisits[(pc - rr_gate_48y_start) / 4];
        if (pc >= rr_packet_48y_start && pc < rr_packet_48y_end)
            return packetVisits[(pc - rr_packet_48y_start) / 4];
        if (pc >= rr_queue_48y_start && pc < rr_queue_48y_end)
            return queueVisits[(pc - rr_queue_48y_start) / 4];
        return 0;
    }
};

struct Machine {
    static constexpr std::uint32_t device = 0x1000, root = 0x2000;
    static constexpr std::uint32_t manager = 0x3000, context = 0x4000;
    static constexpr std::uint32_t descriptors = 0x6000, copy = 0x7000;
    static constexpr std::uint32_t initialStack = 0xc000;
    static constexpr std::uint32_t sentinel = 0x0f000000;
    static constexpr std::uint32_t sdkReadTarget = 0x313350, axisTarget = 0x1438a0;
    static constexpr std::uint32_t failedPacketStop = 0x23830c;
    std::array<std::uint64_t, 32> g{}, upper{};
    std::array<std::uint32_t, 32> f{};
    std::array<std::uint32_t, 0x10000 / 4> memory{};
    std::array<std::uint8_t, 32> suppliedPacket{};
    bool sampleMode = false, suppliedReadSucceeded = false;
    Stats stats{};
    SampleResult result{};

    static std::uint64_t signExtend(std::uint32_t value) {
        return static_cast<std::uint64_t>(static_cast<std::int64_t>(signedWord(value)));
    }
    std::uint32_t read(std::uint32_t address) const {
        if (address == 0x382b80) return root;
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("48Y read outside aligned synthetic fixture");
        return memory[address / 4];
    }
    void write(std::uint32_t address, std::uint32_t value) {
        if ((address & 3u) || address / 4 >= memory.size())
            throw std::runtime_error("48Y write outside aligned synthetic fixture");
        memory[address / 4] = value;
    }
    std::uint8_t readByte(std::uint32_t address) const {
        return static_cast<std::uint8_t>(read(address & ~3u) >> ((address & 3u) * 8));
    }
    std::uint16_t readHalf(std::uint32_t address) const {
        if (address & 1u) throw std::runtime_error("48Y unaligned halfword read");
        return static_cast<std::uint16_t>(read(address & ~3u) >> ((address & 3u) * 8));
    }
    void writeByte(std::uint32_t address, std::uint8_t value) {
        const auto shift = (address & 3u) * 8;
        write(address & ~3u, (read(address & ~3u) & ~(0xffu << shift)) |
                             (static_cast<std::uint32_t>(value) << shift));
    }
    void writeRecord(std::uint32_t address, const Record& record) {
        for (unsigned i = 0; i < record.words.size(); ++i) write(address + 4 * i, record.words[i]);
    }
    Record readRecord(std::uint32_t address) const {
        Record record;
        for (unsigned i = 0; i < record.words.size(); ++i) record.words[i] = read(address + 4 * i);
        return record;
    }
    void prepareBank(const std::array<Record, 32>& bank) {
        write(root + 4, manager);
        for (unsigned i = 0; i < bank.size(); ++i) writeRecord(device + 0x324 + i * 24, bank[i]);
    }
    std::uint32_t word(std::uint32_t pc) {
        if (pc & 3u) throw std::runtime_error("48Y unaligned instruction address");
        if (pc >= rr_query_48y_start && pc < rr_query_48y_end) {
            const auto index = (pc - rr_query_48y_start) / 4;
            ++stats.queryVisits[index];
            return rr_query_48y[index];
        }
        if (pc >= rr_gate_48y_start && pc < rr_gate_48y_end) {
            const auto index = (pc - rr_gate_48y_start) / 4;
            ++stats.gateVisits[index];
            return rr_gate_48y[index];
        }
        if (pc >= rr_packet_48y_start && pc < rr_packet_48y_end) {
            const auto index = (pc - rr_packet_48y_start) / 4;
            ++stats.packetVisits[index];
            return rr_packet_48y[index];
        }
        if (pc >= rr_queue_48y_start && pc < rr_queue_48y_end) {
            const auto index = (pc - rr_queue_48y_start) / 4;
            ++stats.queueVisits[index];
            return rr_queue_48y[index];
        }
        throw std::runtime_error("48Y PC outside frozen intervals");
    }
    struct Transfer { bool taken = false, delay = false; std::uint32_t target = 0; };
    Transfer execute(std::uint32_t pc) {
        if (++stats.steps > 20000) throw std::runtime_error("48Y instruction budget");
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
        // These are fixture-domain guards at original accesses, not game logic.
        // In particular, a denied gate can return before an invalid code is used.
        if (pc == 0x143684 && (signedWord(static_cast<std::uint32_t>(g[18])) < 0 || g[18] >= 32))
            throw std::runtime_error("48Y query record index outside fixture bank");
        if (pc == 0x2380c0 && (address < initialStack + 0xf0 || address >= initialStack + 0x110))
            throw std::runtime_error("48Y pressure offset outside supplied packet");
        if (w == 0) return {};
        if (op == 9) g[rt] = signExtend(address);
        else if (op == 0x19) g[rt] = g[rs] + static_cast<std::uint64_t>(static_cast<std::int64_t>(immediate));
        else if (op == 0xa) g[rt] = signedLong(g[rs]) < immediate ? 1 : 0;
        else if (op == 0xb) g[rt] = g[rs] < static_cast<std::uint64_t>(static_cast<std::int64_t>(immediate)) ? 1 : 0;
        else if (op == 0xc) g[rt] = g[rs] & (w & 0xffffu);
        else if (op == 0xd) g[rt] = g[rs] | (w & 0xffffu);
        else if (op == 0xe) g[rt] = g[rs] ^ (w & 0xffffu);
        else if (op == 0xf) g[rt] = signExtend((w & 0xffffu) << 16);
        else if (op == 0x23) g[rt] = signExtend(read(address));
        else if (op == 0x24) g[rt] = readByte(address);
        else if (op == 0x25) g[rt] = readHalf(address);
        else if (op == 0x28) writeByte(address, static_cast<std::uint8_t>(g[rt]));
        else if (op == 0x2b) write(address, static_cast<std::uint32_t>(g[rt]));
        else if (op == 0x31) f[rt] = read(address);
        else if (op == 0x39) write(address, f[rt]);
        else if (op == 0x1e || op == 0x1f) {
            if (address & 15u) throw std::runtime_error("48Y unaligned quadword access");
            if (op == 0x1e) {
                g[rt] = read(address) | (static_cast<std::uint64_t>(read(address + 4)) << 32);
                upper[rt] = read(address + 8) | (static_cast<std::uint64_t>(read(address + 12)) << 32);
            } else {
                write(address, static_cast<std::uint32_t>(g[rt]));
                write(address + 4, static_cast<std::uint32_t>(g[rt] >> 32));
                write(address + 8, static_cast<std::uint32_t>(upper[rt]));
                write(address + 12, static_cast<std::uint32_t>(upper[rt] >> 32));
            }
        }
        else if (op == 0x1c && fn == 0x28 && sa == 0x18) {
            g[rd] = g[rs] | g[rt];
            upper[rd] = upper[rs] | upper[rt];
        }
        else if (op == 4) return branch(g[rs] == g[rt]);
        else if (op == 5) return branch(g[rs] != g[rt]);
        else if (op == 0x14) return branch(g[rs] == g[rt], true);
        else if (op == 0x15) return branch(g[rs] != g[rt], true);
        else if (op == 1 && rt == 0) return branch(signedLong(g[rs]) < 0);
        else if (op == 3) {
            const auto target = ((pc + 4) & 0xf0000000u) | ((w & 0x3ffffffu) << 2);
            if (target == rr_gate_48y_start) ++stats.gateCalls;
            else if (target == rr_queue_48y_start) {
                if (!sampleMode || read(manager + 0x38) >= result.events.size())
                    throw std::runtime_error("48Y queue fixture overflow");
                ++stats.queueCalls;
            }
            else if (target != sdkReadTarget && target != axisTarget)
                throw std::runtime_error("48Y unexpected direct call");
            g[31] = pc + 8;
            return {true, true, target};
        }
        else if (op == 0 && fn == 8) return {true, true, static_cast<std::uint32_t>(g[rs])};
        else if (op == 0 && fn == 0) g[rd] = signExtend(static_cast<std::uint32_t>(g[rt]) << sa);
        else if (op == 0 && fn == 2) g[rd] = signExtend(static_cast<std::uint32_t>(g[rt]) >> sa);
        else if (op == 0 && fn == 0x21) g[rd] = signExtend(static_cast<std::uint32_t>(g[rs] + g[rt]));
        else if (op == 0 && fn == 0x24) g[rd] = g[rs] & g[rt];
        else if (op == 0 && fn == 0x25) g[rd] = g[rs] | g[rt];
        else if (op == 0 && fn == 0x26) g[rd] = g[rs] ^ g[rt];
        else if (op == 0 && fn == 0x2a) g[rd] = signedLong(g[rs]) < signedLong(g[rt]) ? 1 : 0;
        else if (op == 0 && fn == 0x2b) g[rd] = g[rs] < g[rt] ? 1 : 0;
        else if (op == 0 && fn == 0x3c) g[rd] = g[rt] << (sa + 32);
        // DSRL32 is an unsigned 64-bit shift, not DSRA32.
        else if (op == 0 && fn == 0x3e) g[rd] = g[rt] >> (sa + 32);
        else if (op == 0x11) {
            if (rs == 4) f[rd] = static_cast<std::uint32_t>(g[rt]);
            else if (rs == 20 && fn == 0x20) {
                volatile float value = static_cast<float>(signedWord(f[rd]));
                f[sa] = bits(value);
            }
            else if (rs == 16) {
                const float left = fp(f[rd]), right = fp(f[rt]);
                volatile float value = 0;
                switch (fn) {
                    case 0: value = left + right; break;
                    case 1: value = left - right; break;
                    case 2: value = left * right; break;
                    case 3: value = left / right; break;
                    default: throw std::runtime_error("48Y unsupported FPU instruction");
                }
                f[sa] = bits(value);
            }
            else throw std::runtime_error("48Y unsupported COP1 format");
        }
        else throw std::runtime_error("48Y unsupported instruction");
        g[0] = 0;
        upper[0] = 0;
        return {};
    }
    std::uint32_t run(std::uint32_t start) {
        auto pc = start;
        while (true) {
            if (!sampleMode && pc == sentinel) return pc;
            if (sampleMode && (pc == rr_packet_48y_end || pc == failedPacketStop)) return pc;
            if (pc == sdkReadTarget) {
                if (!sampleMode || g[6] != initialStack + 0xf0 || stats.readCalls != 0)
                    throw std::runtime_error("48Y SDK read fixture contract");
                for (unsigned i = 0; i < suppliedPacket.size(); ++i)
                    writeByte(static_cast<std::uint32_t>(g[6]) + i, suppliedPacket[i]);
                ++stats.readCalls;
                g[2] = suppliedReadSucceeded ? 1 : 0;
                pc = static_cast<std::uint32_t>(g[31]);
                continue;
            }
            if (pc == axisTarget) {
                if (!sampleMode || g[4] != device || g[5] >= 6 || result.axisUpdateCount >= result.axisUpdates.size())
                    throw std::runtime_error("48Y axis trace fixture contract");
                result.axisUpdates[result.axisUpdateCount++] = {static_cast<std::uint32_t>(g[5]), fp(f[12]), fp(f[13])};
                ++stats.axisCalls;
                pc = static_cast<std::uint32_t>(g[31]);
                continue;
            }
            const auto transfer = execute(pc);
            if (transfer.taken) {
                if (transfer.delay) {
                    ++stats.delaySlots;
                    if (execute(pc + 4).taken)
                        throw std::runtime_error("48Y control transfer in delay slot");
                }
                pc = transfer.target;
            }
            else pc += 4;
        }
    }
};

inline QueryResult query(const std::array<Record, 32>& bank, std::int32_t code,
                         std::uint32_t mask, const QueryContext& input,
                         bool copyRequested, const Record& priorCopy,
                         Stats* statistics = nullptr) {
    Machine machine;
    machine.prepareBank(bank);
    machine.write(Machine::manager + 0x34, input.present ? Machine::context : 0);
    machine.writeByte(Machine::root + 0x228, input.rootFlags228);
    machine.write(Machine::context + 0x518, input.mode518);
    machine.write(Machine::context + 0x1898, input.mask1898);
    machine.writeRecord(Machine::copy, priorCopy);
    machine.g[4] = Machine::device;
    machine.g[5] = Machine::signExtend(static_cast<std::uint32_t>(code));
    machine.g[6] = Machine::signExtend(mask);
    machine.g[7] = copyRequested ? Machine::copy : 0;
    machine.g[29] = Machine::initialStack;
    machine.g[31] = Machine::sentinel;
    // Nontrivial sentinels verify the complete original SQ/LQ prologue/epilogue.
    for (unsigned i = 16; i <= 18; ++i) {
        machine.g[i] = 0x1234567800000000ull + i;
        machine.upper[i] = 0xabcdef0100000000ull + i;
    }
    machine.upper[31] = 0x1234fedcba987654ull;
    machine.run(rr_query_48y_start);
    if (machine.g[29] != Machine::initialStack || machine.g[31] != Machine::sentinel ||
        machine.upper[31] != 0x1234fedcba987654ull)
        throw std::runtime_error("48Y query did not restore original return frame");
    for (unsigned i = 16; i <= 18; ++i)
        if (machine.g[i] != 0x1234567800000000ull + i || machine.upper[i] != 0xabcdef0100000000ull + i)
            throw std::runtime_error("48Y query did not restore callee-saved register");
    if (statistics) *statistics = machine.stats;
    return {machine.g[2] != 0, machine.readRecord(Machine::copy)};
}

inline SampleResult sample(const std::array<Record, 32>& bank,
                           const std::array<Descriptor, 32>& inputDescriptors,
                           std::uint32_t activeCount, std::uint32_t deviceID,
                           const std::array<std::uint8_t, 32>& packet,
                           bool readSucceeded, Stats* statistics = nullptr) {
    if (activeCount > bank.size()) throw std::runtime_error("48Y active count outside fixture bank");
    Machine machine;
    machine.sampleMode = true;
    machine.suppliedPacket = packet;
    machine.suppliedReadSucceeded = readSucceeded;
    machine.prepareBank(bank);
    machine.write(Machine::device + 4, activeCount);
    machine.write(Machine::device + 0x14, Machine::descriptors);
    machine.write(Machine::device + 0x320, deviceID);
    for (unsigned i = 0; i < inputDescriptors.size(); ++i) {
        const auto address = Machine::descriptors + 12 * i;
        machine.writeByte(address, inputDescriptors[i].flags);
        machine.write(address + 4, inputDescriptors[i].packetOffset);
        machine.write(address + 8, inputDescriptors[i].mask);
    }
    constexpr std::array<std::uint32_t, 6> offsets{{6, 7, 4, 0, 0, 5}};
    for (unsigned i = 0; i < offsets.size(); ++i)
        machine.write(Machine::device + 0x6fc + 4 * i, offsets[i]);
    machine.g[16] = Machine::device;
    machine.g[19] = Machine::initialStack + 0x70;
    machine.g[29] = Machine::initialStack;
    machine.g[31] = Machine::sentinel;
    const auto stop = machine.run(rr_packet_48y_start);
    machine.result.packetAccepted = stop == rr_packet_48y_end;
    for (unsigned i = 0; i < bank.size(); ++i)
        machine.result.records[i] = machine.readRecord(Machine::device + 0x324 + 24 * i);
    machine.result.eventCount = machine.read(Machine::manager + 0x38);
    if (machine.result.eventCount > machine.result.events.size())
        throw std::runtime_error("48Y event count outside fixture queue");
    for (unsigned i = 0; i < machine.result.eventCount; ++i)
        for (unsigned j = 0; j < machine.result.events[i].size(); ++j)
            machine.result.events[i][j] = machine.read(Machine::manager + 0x3c + i * 20 + j * 4);
    if (statistics) *statistics = machine.stats;
    return machine.result;
}
} // namespace original48y
