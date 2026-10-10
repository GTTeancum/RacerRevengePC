#pragma once
#include "device_held48y.h"
#include "player_events49a.h"
#include <cstdint>

namespace rr::game {
// Original 40-byte row, ascending field offsets. Names retain numeric source
// identity; this module does not infer physical labels or direction inversion.
struct ControlActionRow49D {
    std::int32_t action690,action694,flag698,flag69C,action6A0;
    std::int32_t action6A4,action6A8,action6AC,action6B0,flag6B4;
};
struct ControlPresetSlot49D {
    ControlActionRow49D actions;
    std::uint32_t configuration11040,preset11048;
    // Additional stores in the complete local default loop. Their option
    // meanings and persistence are outside the recovered contract.
    std::uint32_t field10F94,field10F9C,field10FA4,field11030,field11038;
};
inline constexpr std::uint32_t controlPresetSlots49D=2;
struct ControlPresets49D {ControlPresetSlot49D slots[controlPresetSlots49D];};
static_assert(sizeof(ControlActionRow49D)==40,"ten original signed words");
static_assert(sizeof(ControlPresets49D)==136,"bounded two-slot native state");

// Exact setter188880..188C04 for the two slots initialized by1864F8..18658C.
// The original setter has no slot guard; this native domain check preserves
// state on unsupported slots. ALL preset words are admitted: preset>=8 writes
// only preset11048 and deliberately retains the prior row/configuration word.
bool selectControlPreset49D(ControlPresets49D&,std::uint32_t slot,
                           std::uint32_t preset) noexcept;
// Full local loop only, not the enclosing application constructor. Initializes
// slots0 then1 to preset0, plus five additional source-proven numeric fields.
void initializeControlPresets49D(ControlPresets49D&) noexcept;

// Exact188C10..188D10 preview semantics, not a pure lookup: select requested
// preset in slot0, snapshot its row, then reapply the saved preset index. If
// that saved index is invalid, the new row/mode remains. Slot1 is unchanged.
// Returns a value to avoid output/state pointer aliasing outside typed scope.
ControlActionRow49D previewControlPreset49D(ControlPresets49D&,std::uint32_t preset) noexcept;

// Separate indices: player+FFC selects the action row; vehicle/player+5C selects
// configuration11040. No equality between them or deviceId is assumed.
struct SelectedControlPreset49D {
    PlayerActionCodes48X actions;
    PlayerActionRow49A eventAction;
    bool configurationEntryNonzero;
};
bool resolveControlPreset49D(const ControlPresets49D&,std::uint32_t actionIndexFFC,
                            std::uint32_t configurationIndex5C,
                            SelectedControlPreset49D& out) noexcept;
// Caller-owned snapshot for PlayerEventEnvironment49A.actionRows (count2).
// Refresh after selections; lifetime and environment binding stay with caller.
struct PlayerEventPresetRows49D {PlayerActionRow49A rows[controlPresetSlots49D];};
PlayerEventPresetRows49D playerEventPresetRows49D(const ControlPresets49D&) noexcept;

// Real production composition: selected native preset -> ready packet/held
// sampling ->48X reader/shaping/filter ->48W engine/aero numeric binding.
// Replaces only context.configurationEntryNonzero; mode58 remains independent.
// Unsupported indices fail before sampling and preserve out. Existing48Y finite
// numeric/readiness rules apply. This does not invent hardware transport, queue
// dispatch, helper2255B0 effects, persistence, or a full playable scheduler.
bool stepReadyPresetPlayerControls49D(const ControlPresets49D&,
    std::uint32_t actionIndexFFC,std::uint32_t configurationIndex5C,
    const DevicePacket48Y&,const DeviceButtonDescriptors48Y&,
    std::uint32_t activeCount,std::uint32_t deviceId,
    const DeviceButtonBank48Y& previousButtons,DeviceQueryContext48Y,
    const PlayerReadContext48X&,float previousD28,
    const ControlChannels48B& previousFiltered,ControlRateLimits48B,float elapsedFactor,
    const ControlBindingInput48W&,const ControlBindingParameters48W&,
    const ControlModelBindings48W&,HeldPlayerStep48Y& out) noexcept;
}
