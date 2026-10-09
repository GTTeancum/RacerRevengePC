#pragma once
#include "chariot_root49u.h"
#include "secondary_activation49v.h"

namespace rr::game {
// Actual vehicle838 remains caller-owned (for example CoupledBodyState49M's
// field). Actual864 remains SecondaryBodyState49M::time864. Neither is copied
// into this persistent lifecycle record.
struct ChariotLifecycleState49V {
    std::uint32_t vehicle83C, vehicle840;
    float duration848, elapsed84C, previous85C;
    VisualResponse49R response;
};
using ChariotLifecycleParameters49V = ChariotRootParameters49U;
using ChariotLifecycleShared49V = ChariotRootShared49U;
using ChariotLifecycleBindings49V = ChariotRootBindings49U;
using ChariotLifecyclePhase49V = ChariotRootPhase49U;
struct ChariotLifecycleInput49V {
    ChariotLifecyclePhase49V phase;
    std::uint32_t vehicle58, aeroD8, vehicleCFC, vehicleD00;
    float frameFactor, objectD34_10;
    VisualVector49R impactBA0;
};
struct ChariotLifecycleWorkspace49V {
    SceneNode49P nodes[maxSceneNodes49P];
    ChariotLifecycleState49V state;
    ChariotLifecycleShared49V shared;
    SecondaryBodyState49M secondary;
    SecondaryActivationSupplement49V supplement;
    CoupledNoiseContext49M noise;
    std::uint32_t vehicle838;
};
enum class ChariotLifecycleError49V : std::uint32_t {
    none, context, rounding_mode, graph, records, input, arithmetic, noise, body_scene
};
struct ChariotLifecycleDiagnostic49V {
    ChariotLifecycleError49V error;
    std::uint32_t sourceStage;
};
// Complete ordinary21A660 through its original return, after engine response
// and before later child cache refresh/physics. Requires vehicle58=40=0 and
// exactly one of (P,V)=(nonzero,0),(0,nonzero),(nonzero,nonzero), finite positive
// duration848 and finite nonnegative frameFactor. (0,0) has no stack260 producer.
// Main cached34, local80 XYZ, local angularB4, current probe10C/1D0 and state40
// are derived exclusively from the immutable canonical main body. Main70/B0
// copies use its body fields and the actual mainLanes W words after exact reset.
//
// body35C+288 is bound to physical19C. The secondary root.scene must equal its
// cached world100 bit-for-bit; sceneFlags190's low byte must equal the graph
// flag byte. LocalC0 is separately owned by the graph, even for parentless
// roots. Reverse copies local, runs167A50->167AD0, inherits main70/B0, clears
// elapsed, sets physical838=1, retains83C and preserves dirty/stale world state.
// The remaining flag bytes and all unmodified canonical body fields survive.
// Blend reads the physical cached world directly, uses old elapsed/duration,
// rebuilds the source orthogonal basis, publishes, then advances elapsed; only
// updated elapsed>duration clears83C. The ordinary speed tail runs afterward.
//
// The supplied graph is the actual current49P forest with49U's distinct binding
// constraints. No49M childless, observer, scene-ready or phase guard is weakened:
// this is not a complete49M frame bridge. No body integration, contact producer,
// observer, spawn, placement or eager physical-world refresh occurs.
// Shared zero434630, activation globals, noise/RNG and finite surviving VU
// scratch use the retained49U admission. Six noise calls occur per V-enabled
// call, even at zero amplitude. Strict separate binary32 FE_TONEAREST arithmetic;
// finite consumed operands/intermediates, true-zero VU saturation, positive
// subnormal squared VU norms excluded. Copy-only lanes preserve arbitrary bits.
// All records are ordinary distinct initialized live storage; main/mainLanes
// and838/secondary may be disjoint subobjects of the same Coupled49M object.
// False preserves graph/lifecycle/shared/secondary/supplement/838/noise. Workspace
// may change and diagnostic is assigned. Fixed caller storage, no allocation.
bool updateChariotLifecycle49V(const ChariotLifecycleInput49V&,
    const ChariotLifecycleParameters49V&, const ChariotLifecycleBindings49V&,
    const SceneGraph49P&, const MainBodyState49K&, const MainSphereLanes49M&,
    SecondaryBodyState49M&, SecondaryActivationSupplement49V&,
    const SecondaryActivationGlobals49V&, std::uint32_t& vehicle838,
    ChariotLifecycleState49V&, ChariotLifecycleShared49V&,
    const VisualVector49R& zero434630, CoupledNoiseContext49M&,
    ChariotLifecycleWorkspace49V&, ChariotLifecycleDiagnostic49V&) noexcept;
}
