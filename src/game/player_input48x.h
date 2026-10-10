#pragma once
#include "control_binding48w.h"
#include <cstdint>

namespace rr::game {
struct InputAxisRange48X {
    float minimum,maximum,center,upperDeadband,lowerDeadband;
};
// Exact numeric body 0x127DA4..0x127E38. No invented output clamp. Requires a
// positive denominator and ordered finite range/deadband; finite intermediates.
bool conditionInputAxis48X(float numerator,float denominator,const InputAxisRange48X&,
                          float& out) noexcept;

// Original ready DualShock packet route; bytes are named by original packet
// offsets, not inferred physical stick names. Transport/connection negotiation
// is outside this API. Axis IDs 0/1/2/5 use packet6/7/4/5; only axis2 is inverted.
struct DualShockAxisBytes48X {std::uint8_t packet4,packet5,packet6,packet7;};
bool conditionDualShockPlayerAxes48X(DualShockAxisBytes48X,ControlChannels48B& out) noexcept;

struct PlayerActionCodes48X {
    std::int32_t action694,action690,flag698,flag69C,flag6B4;
};
enum class PlayerActionResult48X { NoMatch,Match,Unavailable };
using PlayerActionQuery48X=PlayerActionResult48X (*)(void* context,std::int32_t code,
                                                  std::uint32_t mask) noexcept;
struct PlayerActionProvider48X {void* context;PlayerActionQuery48X query;};
struct PlayerReadContext48X {
    ControlChannels48B componentValues;
    float source7D0,source7D4,source7D8,source7DC;
    // Gate byte belongs to global382B80 -> +28C -> +B4 object, NOT vehicle+58.
    std::uint8_t gateByte58,previousFlagsB9C;
};
struct PlayerReadResult48X {
    ControlChannels48B raw;
    float actionB7C,actionB80;
    std::uint8_t flagsB9C;
};
// Exact supplied-input reader 0x1A1A20..0x1A1DCC plus gate 0x1B0B80. Provider
// represents the already-resolved device at +FF0; codes are the selected row
// for device index +FFC. Configuration index +5C is separate. Up to seven
// ordered read-only queries use mask0x3F. Match means original nonzero return;
// edge/held semantics and physical labels are NOT inferred. Only action694/690
// skip code -1. Other negative codes are passed through. Unavailable/invalid
// callback results leave out unchanged; completed external reads cannot roll back.
bool readPlayerInput48X(const PlayerReadContext48X&,const PlayerActionCodes48X&,
                       PlayerActionProvider48X,PlayerReadResult48X& out) noexcept;

// Narrow extension for the proven authored-axis endpoint rounding envelope:
// [-bits(3F800001),+bits(3F800001)]. Retains the original cosine reflection when
// the argument slightly crosses +/-PI/2. The preserved48V API still admits only
// [-1,1]; this extension does not clamp or relax that older API's contract.
bool shapePlayerControlChannels48X(const ControlChannels48B& raw,float previousD28,
                                  ControlShapeMode48V,ControlShapingResult48V& out) noexcept;
struct PlayerControlStep48X {
    ControlChannels48B components;
    PlayerReadResult48X player;
    ControlBoundStep48W bound;
};
// Explicit supplied-frame dataflow: ready packet axes -> component values ->
// original reader/overrides -> shaping/filter -> existing numeric model binding.
// Uses the reader's actions, flags and +7D0/+7D4 in the binding context. Other
// vehicle/cached-body state remains supplied. This is not a device transport,
// complete player scheduler, force update, renderer input loop, or playable tick.
bool stepDualShockPlayerControls48X(DualShockAxisBytes48X,const PlayerReadContext48X&,
                                   const PlayerActionCodes48X&,PlayerActionProvider48X,
                                   float previousD28,const ControlChannels48B& previousFiltered,
                                   ControlRateLimits48B,float elapsedFactor,
                                   const ControlBindingInput48W& bindingContext,
                                   const ControlBindingParameters48W&,
                                   const ControlModelBindings48W& previousBindings,
                                   PlayerControlStep48X& out) noexcept;
}
