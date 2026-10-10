#pragma once
// Independent test-only fixed-register interpreter of selected original words.
// Synthetic caller-owned fields are not a guest RAM image. No production header,
// production arithmetic, hardware sampler or opaque callback is used here.
#include "../../evidence/frameTiming49I/numeric-source/original_words49i.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <initializer_list>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace frame49i_oracle {
struct State {
    std::uint32_t current230, previous234, raw250, lastDispatched23C;
    std::uint32_t phase22C, active11C, pending120;
};
struct Result { std::uint32_t deltaMilliseconds, elapsedBits; };
struct Store { std::uint32_t pc, offset, bits; };
inline constexpr std::size_t selectedWordCount =
    sizeof(frame49i_source::words) / sizeof(frame49i_source::words[0]);
inline std::array<std::uint64_t, selectedWordCount> executed{};
inline std::array<std::uint64_t, selectedWordCount> branchesTaken{};
inline std::array<std::uint64_t, selectedWordCount> branchesNotTaken{};
inline std::vector<Store> lastStoreTrace;

namespace detail {
inline std::size_t wordIndex(std::uint32_t pc) {
    std::size_t a = 0, z = selectedWordCount;
    while (a < z) {
        const auto m = a + (z-a)/2;
        if (frame49i_source::words[m].pc < pc) a = m+1;
        else z = m;
    }
    if (a == selectedWordCount || frame49i_source::words[a].pc != pc)
        throw std::runtime_error("49I oracle attempted a non-selected original PC");
    return a;
}
inline std::uint64_t signExtend32(std::uint32_t word) {
    return (word & 0x80000000u) ? (0xffffffff00000000ull | word) : word;
}
inline std::uint64_t roundedShift(std::uint64_t value, unsigned shift) {
    if (!shift) return value;
    if (shift > 64) return 0;
    if (shift == 64) return value > 0x8000000000000000ull ? 1 : 0;
    const auto quotient = value >> shift;
    const auto remainder = value & ((std::uint64_t{1} << shift)-1);
    const auto halfway = std::uint64_t{1} << (shift-1);
    return quotient + ((remainder > halfway || (remainder == halfway && (quotient & 1))) ? 1 : 0);
}
// Round the exact signed dyadic integer significand * 2^power to binary32.
// Selected CVT.S.W/MUL.S/ADD.S operands are finite. This performs no host FP.
inline std::uint32_t roundDyadic(std::uint64_t significand, int power, std::uint32_t sign = 0) {
    if (!significand) return sign;
    // MSVC IA32 /O2 miscompiles the uint64 `x > 1; x >>= 1` loop:
    // it can exit when the high word becomes zero, with 32 low bits left.
    // Count in one 32-bit limb after recording the high-word contribution.
    const auto upper = static_cast<std::uint32_t>(significand >> 32);
    unsigned highest = upper ? 32u : 0u;
    for (auto x = upper ? upper : static_cast<std::uint32_t>(significand); x > 1; x >>= 1) ++highest;
    int exponent = static_cast<int>(highest) + power;
    if (exponent < -126) {
        const int shift = -149-power;
        const auto fraction = shift > 0 ? roundedShift(significand, static_cast<unsigned>(shift))
                                       : significand << static_cast<unsigned>(-shift);
        if (fraction >= 0x800000u) return sign | 0x00800000u;
        return sign | static_cast<std::uint32_t>(fraction);
    }
    auto fraction = highest > 23 ? roundedShift(significand, highest-23)
                                 : significand << (23-highest);
    if (fraction == 0x1000000u) { fraction >>= 1; ++exponent; }
    if (exponent > 127) return sign | 0x7f800000u;
    return sign | (static_cast<std::uint32_t>(exponent+127) << 23)
                | (static_cast<std::uint32_t>(fraction) & 0x7fffffu);
}
struct Dyadic { std::uint64_t significand; int power; std::uint32_t sign; };
inline Dyadic unpack(std::uint32_t bits) {
    const auto exponent = (bits >> 23) & 255u;
    if (exponent == 255) throw std::runtime_error("49I oracle finite instruction model exceeded");
    return {exponent ? (0x800000u | (bits & 0x7fffffu)) : (bits & 0x7fffffu),
            exponent ? static_cast<int>(exponent)-150 : -149, bits & 0x80000000u};
}
inline std::uint32_t cvtSignedWord(std::uint32_t bits) {
    const auto sign = bits & 0x80000000u;
    const auto magnitude = sign ? (std::uint64_t{0x100000000ull}-bits) : bits;
    return roundDyadic(magnitude, 0, sign);
}
inline std::uint32_t multiply(std::uint32_t a, std::uint32_t b) {
    const auto x = unpack(a), y = unpack(b);
    return roundDyadic(x.significand*y.significand, x.power+y.power, x.sign^y.sign);
}
inline std::uint32_t addEqual(std::uint32_t a, std::uint32_t b) {
    if (a != b) throw std::runtime_error("49I selected ADD.S must have identical operands");
    const auto x = unpack(a);
    return roundDyadic(x.significand+x.significand, x.power, x.sign);
}
inline bool lessEqual(std::uint32_t a, std::uint32_t b) {
    if ((a | b) & 0x80000000u) throw std::runtime_error("49I selected compare expects nonnegative finite numbers");
    (void)unpack(a); (void)unpack(b);
    return a <= b;
}

struct Machine {
    static constexpr std::uint32_t fixtureBase = 0x10000u;
    State& state;
    std::array<std::uint64_t, 32> r{};
    std::array<std::uint32_t, 32> f{};
    std::uint32_t capWord, flags, delta = 0;
    bool condition = false;
    explicit Machine(State& s, std::uint32_t cap = 0, std::uint32_t suppliedFlags = 0)
        : state(s), capWord(cap), flags(suppliedFlags) {
        r[16] = fixtureBase;
        lastStoreTrace.clear();
    }
    std::uint32_t read(std::uint32_t address, bool byte = false) const {
        if (address == 0x423ed8u && !byte) return capWord;
        if (address < fixtureBase) throw std::runtime_error("49I oracle read outside fixture");
        const auto offset = address-fixtureBase;
        if (byte) {
            if (offset == 0x228u) return flags & 255u;
            throw std::runtime_error("49I oracle byte read outside supplied flags");
        }
        switch (offset) {
        case 0x230: return state.current230;
        case 0x234: return state.previous234;
        case 0x250: return state.raw250;
        case 0x23c: return state.lastDispatched23C;
        case 0x22c: return state.phase22C;
        case 0x11c: return state.active11C;
        case 0x120: return state.pending120;
        default: throw std::runtime_error("49I oracle read outside selected caller fields");
        }
    }
    void write(std::uint32_t pc, std::uint32_t address, std::uint32_t bits) {
        if (address < fixtureBase) throw std::runtime_error("49I oracle store outside fixture");
        const auto offset = address-fixtureBase;
        switch (offset) {
        case 0x230: state.current230 = bits; break;
        case 0x234: state.previous234 = bits; break;
        case 0x250: state.raw250 = bits; break;
        case 0x23c: state.lastDispatched23C = bits; break;
        case 0x22c: state.phase22C = bits; break;
        case 0x11c: state.active11C = bits; break;
        case 0x120: state.pending120 = bits; break;
        default: throw std::runtime_error("49I oracle store outside selected caller fields");
        }
        lastStoreTrace.push_back({pc, offset, bits});
    }
    // Decode control first, execute its delay word exactly once, then transfer.
    // No selected delay word itself contains another control transfer.
    std::uint32_t step(std::uint32_t pc, bool inDelay = false) {
        const auto index = wordIndex(pc);
        const auto word = frame49i_source::words[index].bits;
        ++executed[index];
        const auto op = word >> 26, rs = word >> 21 & 31u, rt = word >> 16 & 31u;
        const auto rd = word >> 11 & 31u, sa = word >> 6 & 31u, fn = word & 63u;
        const auto immediate = static_cast<std::int32_t>(word & 65535u) - ((word & 32768u) ? 65536 : 0);
        const auto address = static_cast<std::uint32_t>(r[rs] + static_cast<std::uint64_t>(immediate));
        bool control = false, taken = false;
        std::uint32_t destination = pc+8;
        if (op == 4 || op == 5 || (op == 1 && rt == 0) || (op == 17 && rs == 8)) {
            control = true;
            if (op == 4) taken = r[rs] == r[rt];
            else if (op == 5) taken = r[rs] != r[rt];
            else if (op == 1) taken = (r[rs] >> 63) != 0;
            else {
                if (rt != 1) throw std::runtime_error("49I unexpected coprocessor branch");
                taken = condition;
            }
            if (taken) destination = static_cast<std::uint32_t>(pc+4+immediate*4);
        } else if (op == 3) {
            control = taken = true;
            destination = ((pc+4) & 0xf0000000u) | ((word & 0x03ffffffu) << 2);
            r[31] = pc+8;
        } else if (op == 35) r[rt] = signExtend32(read(address));
        else if (op == 36) r[rt] = read(address, true);
        else if (op == 43) write(pc, address, static_cast<std::uint32_t>(r[rt]));
        else if (op == 57) write(pc, address, f[rt]);
        else if (op == 15) r[rt] = signExtend32((word & 65535u) << 16);
        else if (op == 13) r[rt] = r[rs] | (word & 65535u);
        else if (op == 12) r[rt] = r[rs] & (word & 65535u);
        else if (op == 9) r[rt] = signExtend32(address);
        else if (op == 28 && fn == 40 && sa == 24) r[rd] = r[rs] | r[rt];
        else if (op == 0) {
            if (fn == 0) r[rd] = signExtend32(static_cast<std::uint32_t>(r[rt]) << sa);
            else if (fn == 2) r[rd] = signExtend32(static_cast<std::uint32_t>(r[rt]) >> sa);
            else if (fn == 35) {
                r[rd] = signExtend32(static_cast<std::uint32_t>(r[rs]-r[rt]));
                if (pc == 0x140becu) delta = static_cast<std::uint32_t>(r[rd]);
            } else if (fn == 37) r[rd] = r[rs] | r[rt];
            else if (fn == 60) r[rd] = r[rt] << (sa+32);
            else if (fn == 62) r[rd] = r[rt] >> (sa+32);
            else throw std::runtime_error("49I unknown selected SPECIAL instruction");
        } else if (op == 17) {
            if (rs == 4) f[rd] = static_cast<std::uint32_t>(r[rt]);
            else if (rs == 20 && fn == 32) f[sa] = cvtSignedWord(f[rd]);
            else if (rs == 16 && fn == 0) f[sa] = addEqual(f[rd], f[rt]);
            else if (rs == 16 && fn == 2) f[sa] = multiply(f[rd], f[rt]);
            else if (rs == 16 && fn == 6) f[sa] = f[rd];
            else if (rs == 16 && fn == 54) condition = lessEqual(f[rd], f[rt]);
            else throw std::runtime_error("49I unknown selected COP1 instruction");
        } else throw std::runtime_error("49I unknown selected instruction");
        r[0] = 0;
        if (!control) return pc+4;
        if (inDelay) throw std::runtime_error("49I unexpected branch inside delay slot");
        if (taken) ++branchesTaken[index]; else ++branchesNotTaken[index];
        if (step(pc+4, true) != pc+8) throw std::runtime_error("49I malformed delay slot");
        return destination;
    }
    std::uint32_t run(std::uint32_t pc, std::initializer_list<std::uint32_t> stops) {
        for (unsigned budget = 0; budget != 128; ++budget) {
            for (auto stop : stops) if (pc == stop) return pc;
            pc = step(pc);
        }
        throw std::runtime_error("49I oracle exceeded bounded instruction budget");
    }
};
} // namespace detail

inline Result core(State& state, std::uint32_t capWord) {
    detail::Machine machine(state, capWord);
    machine.run(0x140be4u, {0x140c8cu});
    return {machine.delta, machine.f[20]};
}
inline void reset(State& state, std::uint32_t suppliedConvertedStamp) {
    detail::Machine machine(state);
    machine.r[2] = detail::signExtend32(suppliedConvertedStamp);
    machine.run(0x140e38u, {0x140e40u});
}
inline Result begin(State& state, std::uint32_t suppliedConvertedStamp, std::uint32_t capWord) {
    detail::Machine machine(state, capWord);
    const auto boundary = machine.run(0x140b10u, {0x140b30u, 0x140d70u});
    // Explicit supplied-clock boundary. Neither original provider body executes.
    machine.r[2] = detail::signExtend32(suppliedConvertedStamp);
    if (boundary == 0x140d70u) {
        machine.run(0x140e38u, {0x140e40u});
        machine.run(0x140b24u, {0x140c8cu});
    } else machine.run(0x140be0u, {0x140c8cu});
    return {machine.delta, machine.f[20]};
}
inline bool gate(std::uint32_t liveRootFlagsAfterManager) {
    State dummy{};
    detail::Machine machine(dummy, 0, liveRootFlagsAfterManager);
    return machine.run(0x140c98u, {0x140cacu, 0x140d1cu}) == 0x140d1cu;
}
inline void enter(State& state, std::uint32_t elapsedBits) {
    detail::Machine machine(state);
    machine.f[20] = elapsedBits;
    machine.run(0x140cdcu, {0x140ce8u});
}
inline void leave(State& state) {
    detail::Machine machine(state);
    machine.run(0x140cfcu, {0x140d00u});
}
inline void finish(State& state) {
    detail::Machine machine(state);
    machine.run(0x140d2cu, {0x140d4cu});
}
inline std::uint64_t coverageCount(std::uint32_t pc) { return executed[detail::wordIndex(pc)]; }
inline std::uint64_t branchTakenCount(std::uint32_t pc) { return branchesTaken[detail::wordIndex(pc)]; }
inline std::uint64_t branchNotTakenCount(std::uint32_t pc) { return branchesNotTaken[detail::wordIndex(pc)]; }
inline bool allDecisionArmsCovered() {
    for (const auto pc : {0x140b14u, 0x140bf0u, 0x140c4cu, 0x140c60u,
                          0x140c80u, 0x140ca4u, 0x140d38u})
        if (!branchTakenCount(pc) || !branchNotTakenCount(pc)) return false;
    return true;
}
inline std::size_t executedWordCount() {
    std::size_t count = 0;
    for (auto n : executed) if (n) ++count;
    return count;
}
inline bool allReachableWordsCovered() {
    for (std::size_t i = 0; i != selectedWordCount; ++i)
        if (!executed[i] && frame49i_source::words[i].pc != frame49i_source::unreachableDuplicateShift) return false;
    return coverageCount(frame49i_source::unreachableDuplicateShift) == 0;
}
inline void writeCoverage(std::ostream& out) {
    const auto oldFlags = out.flags();
    out << "{\"selected_words\":" << std::dec << selectedWordCount
        << ",\"executed_words\":" << executedWordCount()
        << ",\"reachable_words\":68,\"all_decision_arms_covered\":" << (allDecisionArmsCovered() ? "true" : "false")
        << ",\"unreachable_duplicate_shift\":\"0x140c04\",\"context_words_executed\":0,\"words\":[";
    for (std::size_t i = 0; i != selectedWordCount; ++i) {
        if (i) out << ',';
        out << "{\"pc\":\"0x" << std::hex << frame49i_source::words[i].pc
            << "\",\"word\":\"0x" << frame49i_source::words[i].bits << "\",\"executions\":" << std::dec << executed[i]
            << ",\"branch_taken\":" << branchesTaken[i] << ",\"branch_not_taken\":" << branchesNotTaken[i] << '}';
    }
    out << "]}";
    out.flags(oldFlags);
}
} // namespace frame49i_oracle
