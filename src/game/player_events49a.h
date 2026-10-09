#pragma once
#include "event_dispatch48z.h"
#include <cstdint>

namespace rr::game {
// Original manager128520 source selection. Context/auxiliary concrete virtual
// implementations are not yet proven; presence is distinct from a missing binding.
using HeldSourceQuery49A=EventCallbackResult48Z (*)(void*,std::int32_t code,std::uint32_t mask) noexcept;
struct HeldSourceBinding49A {bool present;void* context;HeldSourceQuery49A query;};
struct ManagerHeldSources49A {
    HeldSourceBinding49A currentContext,auxiliary;
    const DeviceButtonBank48Y* devices[8];
    std::uint8_t rootFlags228;
    std::uint32_t context518,context1898;
};
enum class ManagerHeldStatus49A {Complete,UnsupportedDeviceIndex,UnsupportedDeviceCode,
                                 MissingSourceBinding,SourceUnavailable};
struct ManagerHeldResult49A {ManagerHeldStatus49A status;bool matched;};
// Unknown category/absent source return a successful miss without querying.
// Only category2 uses sourceIndex. It is a direct0..7 index, never an all-device
// selector. Present category2 uses the concrete48Y held bank and context gate.
// The original manager passes a null scratch-record pointer and normalizes v0.
ManagerHeldResult49A queryManagerHeld49A(const ManagerHeldSources49A&,std::int32_t code,
                                      std::uint32_t category,std::uint32_t mask,
                                      std::uint32_t sourceIndex) noexcept;

struct PlayerEventState49A {
    std::uint32_t field40,source5C,indexFFC;
    float value1098,value1094,value1090;
    float valueCB4,valueCB0,valueCB8;
    std::uint32_t countCD0;
};
struct PlayerActionRow49A {std::int32_t action6AC;};
struct PlayerTuningRow49A {float value102F0;std::uint32_t word10388;};
struct PlayerEventEnvironment49A {
    ManagerHeldSources49A held;
    // Original global application ->28C flags29 and ->28C->B4 gate58.
    std::uint8_t flags29,gate58;
    const PlayerActionRow49A* actionRows;
    std::uint32_t actionRowCount;
    std::uint8_t selection1125F;
    const PlayerTuningRow49A* tuningRows;
    std::uint32_t tuningRowCount;
    std::uint8_t lazyFlag85F4;
    std::uint32_t lazyWord4340A0;
};
enum class PlayerEffectKind49A {Function2255B0,Virtual94,Virtual90,Virtual8C,Virtual88,
                              Function22B170,Function22B370,Function22B570,
                              Function22B760,Function22BA60};
struct PlayerEffectCall49A {
    PlayerEffectKind49A kind;
    float scalarArgument;          // f12 only for22B170/22B370; otherwise zero.
    std::uint32_t integerArgument;  // a1 only for22BA60 (12); otherwise zero.
};
// Mandatory synchronous effect boundary. true means the bound effect executed;
// false means unavailable, with any already-produced external effects retained.
// No command buffering, deferred action, no-op fallback or recovered helper
// internals are implied. Context can own the mutable player/environment bindings.
using PlayerEffectApply49A=bool (*)(void*,const PlayerEffectCall49A&) noexcept;
struct PlayerEffectProvider49A {void* context;PlayerEffectApply49A apply;};
struct PlayerEventBindings49A {
    PlayerEffectProvider49A effects;
    EventChildRouter48Z children;
    EventChildCallback48Z childCallback;
    void* childContext;
    std::uint32_t childVisitBudget;
};
enum class PlayerEventStatus49A {Complete,InvalidActionRow,InvalidTuningRow,
                                UnsupportedNumeric,QueryUnavailable,
                                MissingEffectBinding,EffectUnavailable,ChildRouteUnavailable};
struct PlayerEventResult49A {
    PlayerEventStatus49A status;
    bool handled;
    std::uint32_t queriesCompleted,effectsInvoked;
    ManagerHeldStatus49A lastQuery;
    EventRouteResult48Z childRoute;
};
// Full1A16F0 handler sequencing and direct stores, plus128520 held selection.
// Event contents do not select these actions: three ordered manager held queries
// do. Actions never consume; the final142350 child-router result is returned.
// Numeric subtraction follows strict host binary32, finite admitted operands.
// Missing helpers stop before dependent stores; earlier effects remain in place.
// Enclosing player/environment/bindings and borrowed event/record objects remain
// alive throughout callbacks. Rebound row spans must contain valid complete typed
// rows; no heterogeneous byte overlays or overlapping player/configuration state.
PlayerEventResult49A handlePlayerPress49A(PlayerEventState49A&,PlayerEventEnvironment49A&,
                                        const PlayerEventBindings49A&,
                                        InputEvent48Z&,DeviceButtonRecord48Y&) noexcept;

// Explicit binding for one caller-selected48Z child identity. No runtime object
// topology is inferred. Press invokes the handler; proven player vtable+60
// release uses the unchanged48Z release router without held queries/effects.
struct PlayerEventChildBinding49A {
    std::uint32_t childIndex;
    PlayerEventState49A* player;
    PlayerEventEnvironment49A* environment;
    const PlayerEventBindings49A* bindings;
    PlayerEventResult49A lastResult;
};
EventCallbackResult48Z playerEventChild49A(void*,std::uint32_t child,bool press,
                                         InputEvent48Z&,DeviceButtonRecord48Y&) noexcept;
}
