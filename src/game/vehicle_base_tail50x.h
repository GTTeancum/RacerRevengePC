#pragma once
#include "vehicle_frame49w.h"
#include "frame_timing49i.h"
#include "engine_tail50t.h"
#include "../assets/pod_flame_runtime50w.h"
namespace rr::game {
enum class Probe128Binding50X:std::uint32_t {Unresolved,Null,Present};
struct BaseTailBindings50X {std::uint32_t vehicleOwnerId;Probe128Binding50X probe128;};
struct BaseTailState50X {
 std::uint32_t flagsACC,flagsAD0,countAD4[2],timerADC[2],timerAE4,activeAE8;
 std::uint32_t engineThrustW[2],flameAC4[2];
 float positionAt228B98[4]; // native continuation of the original stack snapshot
};
struct BaseTailGlobals50X {bool initialized434810;std::uint32_t value434810;};
struct BaseTailWorkspace50X {
 BaseTailState50X next;BaseTailGlobals50X globals;
 rr::assets::PodFlameOwners50W flame;
 FlameWorkspace50V binding;
};
// Fresh source constructor stores: ACC/AD0=1; AD4..AE8 and AC4/AC8=0.
// Unrepresented existing engine W lanes remain explicit caller-owned words.
BaseTailState50X coldBaseTailState50X(std::uint32_t leftThrustW,std::uint32_t rightThrustW) noexcept;
// Source228B8C through2208A0, stopping before diagnostics/pose/history/children.
// Uses actual clock owner's lastDispatched23C, not a guessed root/frame factor.
// Failure preserves frame, supplementary state, globals and material ownership.
bool advanceVehicleBaseThroughEngine50X(VehicleFrameState49W&,const VehicleFrameBindings49W&,
 const FrameTimingState49I&,const BaseTailBindings50X&,BaseTailState50X&,
 BaseTailGlobals50X&,rr::assets::PodFlameOwners50W&,BaseTailWorkspace50X&,
 VehicleFrameWorkspace49W&,VehicleFrameDiagnostic49W&) noexcept;
}
