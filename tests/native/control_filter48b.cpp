// Differential test against the exact original 0x2180AC..0x2181C4 instruction
// interval. This tiny interpreter belongs only to the test executable.
#include "../../src/game/control_filter48b.h"
#include "../../tools/gameplay/control_filter_words48b.h"
#include <array>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

using rr::game::ControlChannels48B;
using rr::game::ControlRateLimits48B;
using rr::game::stepControlRateFilter48B;

static std::uint32_t bits(float x) {
    std::uint32_t b; std::memcpy(&b, &x, 4); return b;
}
static float fp(std::uint32_t b) {
    float x; std::memcpy(&x, &b, 4); return x;
}

static ControlChannels48B original(const ControlChannels48B& target,
                                   const ControlChannels48B& previous,
                                   ControlRateLimits48B rates, float elapsed) {
    std::array<std::uint32_t, 32> g{};
    std::array<float, 32> f{};
    std::array<std::uint32_t, 0x2100 / 4> memory{};
    auto read = [&](std::uint32_t a) {
        if ((a & 3u) || a / 4 >= memory.size()) throw std::runtime_error("fixture read");
        return fp(memory[a / 4]);
    };
    auto write = [&](std::uint32_t a, float value) {
        if ((a & 3u) || a / 4 >= memory.size()) throw std::runtime_error("fixture write");
        memory[a / 4] = bits(value);
    };
    for (unsigned i = 0; i < 4; ++i) {
        write(0xb5c + i * 4, target.values[i]);
        write(0xb6c + i * 4, previous.values[i]);
    }
    write(0xb94, rates.x); write(0xb98, rates.y);
    g[29] = 0x2000; g[3] = g[29] + 0x38; f[20] = elapsed;
    for (const auto word : rrControlFilterWords48B) {
        const auto op = word >> 26, rs = (word >> 21) & 31u, rt = (word >> 16) & 31u;
        const auto immediate = static_cast<std::int16_t>(word & 65535u);
        if (word == 0) continue;
        if (op == 0x31) f[rt] = read(g[rs] + immediate);
        else if (op == 0x39) write(g[rs] + immediate, f[rt]);
        else if (op == 9) g[rt] = g[rs] + immediate;
        else if (op == 0x11 && rs == 16) {
            const auto fs = (word >> 11) & 31u, fd = (word >> 6) & 31u;
            volatile float result = 0.0f;
            switch (word & 63u) {
                case 0: result = f[fs] + f[rt]; break;
                case 1: result = f[fs] - f[rt]; break;
                case 2: result = f[fs] * f[rt]; break;
                case 7: result = -f[fs]; break;
                // Independent original scalar signed-word ordering. Unlike
                // the historical generated float comparator, -0 sorts below +0.
                case 0x28: case 0x29: {
                    const auto x=static_cast<std::int32_t>(bits(f[fs]));
                    const auto y=static_cast<std::int32_t>(bits(f[rt]));
                    const bool chooseMin=((word&63u)==0x29) != (x<0&&y<0);
                    result=fp(static_cast<std::uint32_t>(chooseMin?(x<y?x:y):(x>y?x:y)));
                    break;
                }
                default: throw std::runtime_error("fixture FPU opcode");
            }
            f[fd] = result;
        } else throw std::runtime_error("fixture opcode");
    }
    ControlChannels48B out{};
    for (unsigned i = 0; i < 4; ++i) out.values[i] = read(0xb6c + i * 4);
    return out;
}

int main() {
    std::uint64_t checks = 0;
    auto require = [&](bool pass, const char* message) {
        ++checks;
        if (!pass) throw std::runtime_error(message);
    };
    std::uint32_t random = 0x48b217deu;
    auto next = [&]() { random ^= random << 13; random ^= random >> 17; random ^= random << 5; return random; };
    auto compare = [&](const ControlChannels48B& target, const ControlChannels48B& prior,
                       ControlRateLimits48B rates, float elapsed) {
        const auto expected = original(target, prior, rates, elapsed);
        ControlChannels48B got{};
        require(stepControlRateFilter48B(target, prior, rates, elapsed, got), "finite domain rejected");
        for (unsigned i = 0; i < 4; ++i) require(bits(got.values[i]) == bits(expected.values[i]), "original-word differential");
        return got;
    };
    try {
        for (int rounding : {FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD}) {
            require(std::fesetround(rounding) == 0, "rounding setup");
            for (unsigned n = 0; n != 6000; ++n) {
                ControlChannels48B target{}, prior{};
                for (unsigned i = 0; i < 4; ++i) {
                    target.values[i] = static_cast<float>(static_cast<int>(next() & 65535) - 32768) / 16384.0f;
                    prior.values[i] = static_cast<float>(static_cast<int>(next() & 65535) - 32768) / 16384.0f;
                }
                compare(target, prior, {float(next() & 1023) / 64.0f, float(next() & 1023) / 64.0f},
                        float(next() & 1023) / 4096.0f);
            }
            for (auto zero : {0u, 0x80000000u}) {
                const float z = fp(zero);
                compare({{z, -z, 1.0f, -1.0f}}, {{-z, z, 1.0f, -1.0f}}, {z, -z}, z);
                compare({{z, -z, 1.0f, -1.0f}}, {{-z, z, -1.0f, 1.0f}}, {4, 3}, z);
            }
            // 48O: complete zero-sign Cartesian input/rate/time combinations,
            // through the same authentic70 words in every rounding mode.
            for(unsigned signs=0;signs<32;++signs) {
                const float targetZero=fp((signs&1)?0x80000000u:0u);
                const float priorZero=fp((signs&2)?0x80000000u:0u);
                const float xRate=fp((signs&4)?0x80000000u:0u);
                const float yRate=fp((signs&8)?0x80000000u:0u);
                const float elapsed=fp((signs&16)?0x80000000u:0u);
                compare({{targetZero,targetZero,targetZero,targetZero}},
                        {{priorZero,priorZero,priorZero,priorZero}}, {xRate,yRate},elapsed);
                compare({{targetZero,targetZero,targetZero,targetZero}},
                        {{priorZero,priorZero,priorZero,priorZero}}, {xRate,yRate},1.f);
            }
            ControlChannels48B state{};
            for (unsigned n = 0; n < 512; ++n) {
                const float sign = n % 64 < 32 ? 1.0f : -1.0f;
                state = compare({{sign, -sign, -sign, sign}}, state, {4, 3}, 1.0f / 120.0f);
            }
        }
        require(std::fesetround(FE_TONEAREST) == 0, "rounding restore");
        // Domain failures leave output intact, including failures after channel0.
        const ControlChannels48B sentry{{12, 13, 14, 15}};
        for (float bad : {std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
                          std::numeric_limits<float>::quiet_NaN(), -1.0f}) {
            auto out = sentry;
            require(!stepControlRateFilter48B({}, {}, {4, 3}, bad, out), "bad elapsed accepted");
            require(std::memcmp(&out, &sentry, sizeof out) == 0, "failure mutation");
            require(!stepControlRateFilter48B({}, {}, {bad, 3}, .01f, out), "bad rate accepted");
        }
        for (unsigned channel = 0; channel < 4; ++channel) {
            ControlChannels48B target{}; target.values[channel] = std::numeric_limits<float>::quiet_NaN();
            auto out = sentry;
            require(!stepControlRateFilter48B(target, {}, {4, 3}, .01f, out), "bad channel accepted");
            require(std::memcmp(&out, &sentry, sizeof out) == 0, "late failure mutation");
        }
        auto out = sentry;
        require(!stepControlRateFilter48B({}, {}, {std::numeric_limits<float>::max(), 3}, 2, out), "overflow accepted");
        require(std::memcmp(&out, &sentry, sizeof out) == 0, "overflow mutation");
        ControlChannels48B target{{1, -1, .5f, -.5f}}, prior{};
        const auto expected = original(target, prior, {4, 3}, .01f);
        require(stepControlRateFilter48B(target, prior, {4, 3}, .01f, prior), "aliased state failure");
        require(std::memcmp(&prior, &expected, sizeof prior) == 0, "aliased state mismatch");
        std::cout << "control_filter48b: " << checks << " checks passed; original ELF interval oracle; four rounding modes; 4-channel trajectories\n";
        return 0;
    } catch (const std::exception& e) {
        std::fesetround(FE_TONEAREST);
        std::cerr << "control_filter48b after " << checks << " checks: " << e.what() << '\n';
        return 1;
    }
}
