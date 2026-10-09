#pragma once
#include "coupled_body_advancement49m.h"
#include "secondary_activation49v.h"
namespace rr::game {
// Concrete main340 PhysicsRigidBody slotAC167A50 -> slotA8/167AD0, as reached
// by216C44 and217248. Same concrete source class as secondary35C. Writes only
// the236-byte represented source reset domain; preserves pose, worldA0, cached
// angles, accumulators, scene flags, component owners and native phase tags.
// No scene mutation, whole reset or live-frame phase admission is supplied.
// Inputs/owners must be distinct. Allocation-free bit copies, all rounding modes.
void resetMainRigidBody50AR(MainBodyState49K&,MainSphereLanes49M&,
 SecondaryActivationSupplement49V&,const SecondaryActivationGlobals49V&) noexcept;
}
