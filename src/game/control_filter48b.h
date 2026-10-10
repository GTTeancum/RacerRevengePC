#pragma once

namespace rr::game {

// Original fields B5C/B60/B64/B68 and B6C/B70/B74/B78 respectively.
// Channel numbers are deliberate: button/action mapping is still being recovered.
struct ControlChannels48B { float values[4]; };
struct ControlRateLimits48B { float x; float y; };

// Native extraction of ELF 0x2180AC..0x2181C4, after original shaping/quantization.
// Rate order is X,Y,X,Y. elapsedFactor is original f12; units are not yet proven.
// Requires finite inputs and nonnegative rates/elapsedFactor. Also rejects
// overflowing intermediates. Failure leaves output unchanged. This domain gate
// is a native API requirement, not a claim the original game rejects these inputs.
// Compile with strict float semantics; do not reassociate or enable fast math.
bool stepControlRateFilter48B(const ControlChannels48B& target,
                            const ControlChannels48B& previous,
                            ControlRateLimits48B rates, float elapsedFactor,
                            ControlChannels48B& output) noexcept;

}
