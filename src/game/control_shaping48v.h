#pragma once
#include "control_filter48b.h"

namespace rr::game {

// Numerical input fields +B5C/B60/B64/B68 after the player's input reads and
// overrides. No physical button, action name, deadzone or inversion is inferred.
// Original +58 == 0 selects nonlinear shaping; any nonzero value bypasses it.
enum class ControlShapeMode48V { Nonlinear, BypassNonlinear };
struct ControlShapingResult48V {
    ControlChannels48B targets;               // Quantized +B5C/B60/B64/B68.
    float smoothedChannel0D28;                 // Updated BEFORE quantization.
};
struct ControlInputStep48V {
    ControlShapingResult48V shaping;
    ControlChannels48B filtered;              // +B6C/B70/B74/B78.
};

// Recovered 0x217E30..0x2180AC, including the original cosine polynomial.
// Native bounded domain: each raw channel in [-1,1], finite prior +D28, known
// mode. Range is an API gate, NOT a proven upstream clamp. Finite intermediates
// required. The two lazy global initializers in the preceding prologue are not
// consumed by this numerical interval and are outside this API's state.
// No allocation; errors preserve out, including when inputs alias out.
// Strict binary32 operation order, no fast math or fused multiply-add.
bool shapeControlInputs48V(const ControlChannels48B& raw,
                          float previousSmoothedChannel0D28,
                          ControlShapeMode48V mode,
                          ControlShapingResult48V& out) noexcept;

// Proven contiguous ordering: nonlinear/bypass -> +D28 smoothing -> quantize
// -> corrected 48B rate limiter. elapsedFactor affects ONLY the limiter.
// Does not bind physical controls, engine/aero inputs, or the vehicle scheduler.
bool stepControlInputs48V(const ControlChannels48B& raw,
                         float previousSmoothedChannel0D28,
                         ControlShapeMode48V mode,
                         const ControlChannels48B& previousFiltered,
                         ControlRateLimits48B rates, float elapsedFactor,
                         ControlInputStep48V& out) noexcept;
}
