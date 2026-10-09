#pragma once
#include "player_input48x.h"
#include <cstdint>

namespace rr::game {
// Original 24-byte current-state record at device+324+24*code. Auxiliary words
// have proven edge resets but no proven timestamp/duration semantics.
struct DeviceButtonRecord48Y {
    std::uint32_t held;
    float value;
    std::uint32_t auxiliary08,auxiliary0C,auxiliary10,auxiliary14;
};
struct DeviceButtonBank48Y {DeviceButtonRecord48Y records[32];};
struct DeviceButtonDescriptor48Y {
    std::uint8_t flags;               // Bit1 selects pressure byte instead of digital mask.
    std::uint32_t packetOffset;
    std::uint16_t digitalMask;
};
struct DeviceButtonDescriptors48Y {DeviceButtonDescriptor48Y records[32];};
struct DevicePacket48Y {std::uint8_t bytes[32];};
inline constexpr std::uint32_t dualShockActiveButtons48Y=16;
// Original type7 DualShock-named setup, from initially cleared descriptors.
// Numeric codes/masks/pressure offsets only; physical names are not inferred.
DeviceButtonDescriptors48Y initialDualShockDescriptors48Y() noexcept;
struct DeviceQueryContext48Y {
    bool present;                    // Original input manager+34 is nonnull.
    std::uint8_t rootFlags228;        // Bit2.
    std::uint32_t context518,context1898;
};
// Original143E30 gate, or the no-context branch of143600. No inferred menu or
// modifier names; request0x3F passes in both cases. Pure integer operation.
bool permitsDeviceQuery48Y(DeviceQueryContext48Y,std::uint32_t requestMask) noexcept;

// Original143600: match iff supported record.held==1; copy all24 bytes only on
// a match and when requested. A miss preserves copyOut. Gate denial occurs
// before index validation, as in original. Native direct-record domain0..31;
// negative indices alias heterogeneous original history storage and are rejected
// when the gate permits access. Failure preserves matched/copyOut. Typed aliases
// (same record or distinct complete records) are supported.
bool queryDeviceHeld48Y(const DeviceButtonBank48Y&,std::int32_t code,
                       std::uint32_t requestMask,DeviceQueryContext48Y,
                       bool& matched,DeviceButtonRecord48Y* copyOut=nullptr) noexcept;
struct DeviceHeldProvider48Y {
    const DeviceButtonBank48Y* buttons;
    DeviceQueryContext48Y queryContext;
};
// Native callback for48X. No original instruction runtime or external query hook.
PlayerActionResult48X queryHeldPlayerAction48Y(void*,std::int32_t,std::uint32_t) noexcept;

struct DeviceButtonTransition48Y {
    std::uint32_t buttonCode,kind,deviceId,held,contextMask;
};
enum class DevicePacketStatus48Y { NotReady,ReadFailed,Accepted };
struct DeviceButtonSample48Y {
    DeviceButtonBank48Y buttons;
    DeviceButtonTransition48Y transitions[32];
    std::uint32_t transitionCount;
    DevicePacketStatus48Y status;
};
// Bounded button-state portion of original237DF0: readiness selection, clear
// held/value, sample digital/pressure data, then edge resets and ordered event
// records. Hardware calls, axis-list dispatch, global polling, queue ownership
// and rumble are outside this API. Ready read failure clears held/value but
// emits no transitions. Not-ready preserves the bank. activeCount<=32; PSX init
// establishes16. Pressure offsets must fit the32-byte supplied packet only when
// that branch is used. Invalid arguments preserve the entire output.
bool sampleDeviceButtons48Y(bool deviceReady,bool readSucceeded,const DevicePacket48Y&,
                            const DeviceButtonDescriptors48Y&,std::uint32_t activeCount,
                            std::uint32_t deviceId,const DeviceButtonBank48Y& previous,
                            DeviceButtonSample48Y& out) noexcept;
struct HeldPlayerStep48Y {
    DeviceButtonSample48Y device;
    PlayerControlStep48X player;
};
// Supplied ready DualShock frame -> concrete held-state provider -> full48X
// player input/filter/binding path. Requires packet[0]==0; read-failure handling
// remains available separately above rather than inventing scheduler behavior.
// No event queue is mutated: ordered transition records are returned to its owner.
bool stepReadyHeldPlayerControls48Y(const DevicePacket48Y&,const DeviceButtonDescriptors48Y&,
                                   std::uint32_t activeCount,std::uint32_t deviceId,
                                   const DeviceButtonBank48Y& previousButtons,DeviceQueryContext48Y,
                                   const PlayerReadContext48X&,const PlayerActionCodes48X&,
                                   float previousD28,const ControlChannels48B& previousFiltered,
                                   ControlRateLimits48B,float elapsedFactor,const ControlBindingInput48W&,
                                   const ControlBindingParameters48W&,const ControlModelBindings48W&,
                                   HeldPlayerStep48Y& out) noexcept;
}
