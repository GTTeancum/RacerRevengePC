#pragma once
#include "vehicle_visual49r.h"

namespace rr::game {
struct ChariotRootState49U {
    std::uint32_t vehicle838, vehicle83C, vehicle840;
    float previous85C;
    VisualResponse49R response; //870,880,890,8A0; XYZ and quaternion words.
};
struct ChariotRootShared49U {
    VisualLazyWord49R speed434750, yaw434758;
    VisualVector49R up434650;
};
struct ChariotRootParameters49U {
    float minTimeA3C, maxTimeA40, minAmplitudeA4C, maxAmplitudeA50;
    float effectiveSpeedSquaredA54, orientationScaleA60;
    float cableForceA6C, radiusForceA70, restoringA74, slideRestoringA78;
    float returnGainA7C, linearDampingA80, angularDampingA84;
    float yawInputRangeA88, yawAngleA8C, orientationGainA94;
    float collisionBoundA98, groundBoundA9C;
    float collisionXYAA0, collisionZAA4, groundGainAA8, groundTransitionGainAAC;
    float restLength854;
};
enum class ChariotRootPhase49U : std::uint32_t {
    unspecified, afterEngineResponseBeforePhysics, laterPhysics
};
struct ChariotRootInput49U {
    ChariotRootPhase49U phase;
    std::uint32_t vehicle40, vehicle58, aeroD8, vehicleCFC, vehicleD00, probe10C;
    float frameFactor, cachedBodySpeed34, bodyAngularVelocityYB4, objectD34_10;
    VisualVector49R bodyVelocity80, impactBA0, probeVector1D0;
};
struct ChariotRootBindings49U {
    SceneNodeIndex49P main190, container194, hidden198, physical19C, model1B4;
    SceneNodeIndex49P left1BC, right1B8, leftCable1E0, rightCable1E4;
};
struct ChariotRootWorkspace49U {
    SceneNode49P nodes[maxSceneNodes49P];
    ChariotRootState49U state;
    ChariotRootShared49U shared;
    CoupledNoiseContext49M noise;
    float phase864;
};
enum class ChariotRootError49U : std::uint32_t {
    none, context, rounding_mode, graph, records, input, arithmetic, noise
};
struct ChariotRootDiagnostic49U { ChariotRootError49U error; std::uint32_t sourceStage; };

// Complete admitted21A660 through21C110/114. Present distinct parentless main,
// hidden198, physical19C, displayed1B4 and both primary engine roots;194 is main's
// direct child, both cable nodes are distinct descendants of1B4. At most64 nodes.
// Requires58=40=0 and cached speed >= effective434750 (cold4312AA7F).
// Admits physical838!=0/83C=0, producing1B4 then source-seeding198/870/8A0 and
// switching to virtual; or838=0/83C!=0, integrating198 and publishing1B4. Other
// modes and the reverse transition reject.840 is always published fromprobe10C.
//
// Call after49R and before later physics. Existing animated node locals and
// current body/component/probe/impact data remain supplied phase inputs.
// phase864 MUST be the actual caller-owned vehicle clock also used by49M, never
// a duplicate. zero434630 MUST be the same source shared vector consumed by49R.
// Noise/RNG is the same caller-owned49M context, serially shared across stages
// and vehicles. Six source noise calls occur on every admitted virtual call,
// including zero amplitude. No body integration/reset/contact/spawn is claimed.
//
// Strict nearest/even separate binary32 arithmetic, finite consumed operands
// and intermediates; VU truezero normalization retained, positive subnormal
// squared norms excluded. Source VU scratch consumed by self-subtractions must
// be finite; this does not model arbitrary incoming VU NaNs. Direction-result
// VF9.W and body80.W are dead to represented outputs within that admission.
//49M noise coordinate/table domain applies. All mutable
// records, diagnostic, immutable inputs and graph must be distinct live objects.
// False preserves graph/state/shared/noise/phase; workspace may change and the
// diagnostic is assigned first. Fixed storage; no allocation, callbacks or I/O.
bool updateChariotRoot49U(const ChariotRootInput49U&, const ChariotRootParameters49U&,
    const ChariotRootBindings49U&, const SceneGraph49P&, ChariotRootState49U&,
    ChariotRootShared49U&, const VisualVector49R& zero434630, float& phase864,
    CoupledNoiseContext49M&, ChariotRootWorkspace49U&, ChariotRootDiagnostic49U&) noexcept;
}
