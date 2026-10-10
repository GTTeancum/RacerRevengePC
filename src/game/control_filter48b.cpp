#include "control_filter48b.h"
#include "scalar_minmax48o.h"
#include <cstdint>
#include <cstring>

namespace rr::game {
namespace {
bool finite(float value) noexcept {
    std::uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    return (bits & 0x7f800000u) != 0x7f800000u;
}
}

bool stepControlRateFilter48B(const ControlChannels48B& target,
                            const ControlChannels48B& previous,
                            ControlRateLimits48B rates, float elapsedFactor,
                            ControlChannels48B& output) noexcept {
    if (!finite(elapsedFactor) || elapsedFactor < 0.0f ||
        !finite(rates.x) || rates.x < 0.0f || !finite(rates.y) || rates.y < 0.0f)
        return false;
    // Explicit binary32 stores preserve each original operation's rounding even
    // if a future x86 build uses extended-precision intermediate registers.
    volatile float xLimit = rates.x * elapsedFactor;
    volatile float yLimit = rates.y * elapsedFactor;
    if (!finite(xLimit) || !finite(yLimit)) return false;
    ControlChannels48B result{};
    for (unsigned i = 0; i != 4; ++i) {
        if (!finite(target.values[i]) || !finite(previous.values[i])) return false;
        const float limit = (i & 1) ? yLimit : xLimit;
        volatile float delta = target.values[i] - previous.values[i];
        if (!finite(delta)) return false;
        const float lower = detail::scalarMax48O(delta,-limit);
        const float change = detail::scalarMin48O(lower,limit);
        volatile float next = previous.values[i] + change;
        if (!finite(next)) return false;
        result.values[i] = next;
    }
    output = result;
    return true;
}

}
