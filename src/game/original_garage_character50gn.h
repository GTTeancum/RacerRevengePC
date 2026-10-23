#pragma once
#include "scene_graph49p.h"
#include <array>
#include <cstdint>
namespace rr::game {
// One real Application lifetime, shared by every character11AAE0 caller.
// This is not another RNG or a per-Garage reset of source static state.
struct OriginalGarageCharacterShared50GN {bool lazy816C=false;std::int32_t assertion3E02B0=0;};
struct OriginalGarageCharacterFields50GN {
 std::array<float,4> position220{},velocity230{},position240{},velocity250{},contact260{},normal270{};
 SceneNodeIndex49P firstChild284=SceneNodeIndex49P::none;
};
struct OriginalGarageCharacterDiagnostic50GN {std::uint32_t sourcePC=0;};
// Scalar suffix only. The real enclosing11A750 owner must first construct its
// ONE actual Entity290 and call the1536F0 prefix with a1=0. This operation does
// not construct/register an Entity, model, observer, collision or animation.
void initializeOriginalGarageCharacterFields11A75050GN(OriginalGarageCharacterFields50GN&)noexcept;
// Reached UseCollision=F suffix11AAE0. The source reads actual firstChild284
// world XYZ via28B1A0, computes reciprocal-delta velocity, then commits230/220.
// No animation is sampled here. The enclosing source scheduler calls this
// AFTER all manager101860 actors/post-sample/sound callbacks, then traverses
// this character's actual Entity children. Its model has source mask0.
// Requires a strictly positive finite normal dt. Source dt0 division semantics
// are not replaced by zero velocity. A successful graph-refresh prefix is kept
// on later arithmetic rejection; enclosing owner must latch the failed tick.
bool updateOriginalGarageCharacter11AAE050GN(float actualDelta,
 OriginalGarageCharacterShared50GN&,const SceneGraph49P&,
 OriginalGarageCharacterFields50GN&,OriginalGarageCharacterDiagnostic50GN&)noexcept;
}
