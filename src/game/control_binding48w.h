#pragma once
#include "control_shaping48v.h"
#include "vehicle_aero48e.h"
#include <cstdint>

namespace rr::game {
struct ControlBindingParameters48W {
    float speed1B84, scale1B88, speed2B8C, scale2B90;
    // Effective values of the original lazy globals. Defaults are proven by
    // 48V's initializer; already-initialized values remain caller-owned inputs.
    float scale4346B8=0.5f, bias4346C0=0.25f;
};
struct ControlBindingInput48W {
    ControlChannels48B filtered;               // +B6C/B70/B74/B78.
    float bodyCachedSpeed34;                   // Linked body, not fresh velocity.
    std::uint32_t vehicleState40;              // State numbers have no invented names.
    bool mode58Nonzero;
    // Resolved entry: global382B80 -> +2DC -> +11040 + 4*vehicle(+5C).
    // Its higher-level option identity and indexing ownership are unproven.
    bool configurationEntryNonzero;
    float actionB7C, actionB80;                  // Nonzero tests; physical actions unknown.
    std::uint8_t flagsB9C;                      // Bit2 becomes exact 1 or 0 at aero+D8.
    float source7D0, source7D4;
};
struct ControlModelBindings48W {
    float engineDrive[2];                      // Engine +68/+6C.
    float aeroDrive[2];                        // Aero +E4/+E8.
    float aeroBrakeEC;
    std::int32_t aeroTriggerD8;
    float aeroAux50, aeroAuxF0, aeroAuxF4;       // Preserved writes, not invented forces.
};

// Exact numerical/state writes in original 0x2181C4..0x218898. Linked models,
// selected configuration entry and cached speed are already resolved. Finite
// float inputs/intermediates required; raw integer states retain original tests.
// Prior binding fields are required because some branches intentionally preserve
// them. Strict binary32, signed-word MIN/MAX. No allocation. Failure is atomic.
bool bindVehicleControls48W(const ControlBindingInput48W&,
                           const ControlBindingParameters48W&,
                           const ControlModelBindings48W& previous,
                           ControlModelBindings48W& out) noexcept;

// Copies ONLY the fields established above into the existing producer APIs.
// All other engine/aero inputs are preserved. Aux50/F0/F4 are retained in the
// binding result because the existing 48E producer API does not consume them.
// Fails without mutating either output if a mapped float is not finite.
bool applyControlModelBindings48W(const ControlModelBindings48W&,
                                 const EngineInput48D& previousEngine,
                                 const AeroInput48E& previousAero,
                                 EngineInput48D& outEngine,
                                 AeroInput48E& outAero) noexcept;

struct ControlBoundStep48W {
    ControlInputStep48V control;
    ControlModelBindings48W bindings;
};
// Connected original input-function order, ending before its epilogue. Uses
// context.mode58Nonzero for BOTH shaping and model-route selection. context's
// filtered member is replaced by the newly computed filter result. Does not
// call engine/aero producers or invent their scheduler/action bindings.
bool stepAndBindControlInputs48W(const ControlChannels48B& raw, float previousD28,
                                const ControlChannels48B& previousFiltered,
                                ControlRateLimits48B rates, float elapsedFactor,
                                const ControlBindingInput48W& context,
                                const ControlBindingParameters48W& parameters,
                                const ControlModelBindings48W& previousBindings,
                                ControlBoundStep48W& out) noexcept;
}
