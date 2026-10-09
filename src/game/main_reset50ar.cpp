#include "main_reset50ar.h"
#include <cstring>
namespace rr::game {
void resetMainRigidBody50AR(MainBodyState49K&main,MainSphereLanes49M&lanes,
 SecondaryActivationSupplement49V&extra,const SecondaryActivationGlobals49V&globals)noexcept{
 // Reuse the proved concrete body callback with a transient typed projection,
 // not an independently advancing duplicate body. Copy-only, including NaNs.
 SecondaryBodyState49M staging{};
 std::memcpy(&staging.root,&main.components.root,sizeof staging.root);
 std::memcpy(&staging.cachedSpeed34,&main.cachedSpeed34,sizeof(float));
 std::memcpy(&staging.velocity70W,&lanes.velocity70W,sizeof(float));
 std::memcpy(&staging.angularVelocityB0W,&lanes.angularVelocityB0W,sizeof(float));
 std::memcpy(&staging.force180W,&lanes.force180W,sizeof(float));
 std::memcpy(&staging.torque190W,&lanes.torque190W,sizeof(float));
 resetSecondaryActivation49V(staging,extra,globals);
 std::memcpy(&main.components.root,&staging.root,sizeof staging.root);
 std::memcpy(&main.cachedSpeed34,&staging.cachedSpeed34,sizeof(float));
 std::memcpy(&lanes.velocity70W,&staging.velocity70W,sizeof(float));
 std::memcpy(&lanes.angularVelocityB0W,&staging.angularVelocityB0W,sizeof(float));
 std::memcpy(&lanes.force180W,&staging.force180W,sizeof(float));
 std::memcpy(&lanes.torque190W,&staging.torque190W,sizeof(float));
}
}
