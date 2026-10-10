#pragma once
#include "collision_mesh48i.h"
#include "track_collision_tags50an.h"
#include "../game/group_shape48t.h"
namespace rr::assets {
struct TrackCollider50AO {rr::game::BodyMatrix48G world{};rr::game::GroupShapeSphere48T sphere{};std::uint32_t category=0;};
// Cold prepared-instance boundary: original track classification, static
// placement axes, COL composition and cached sphere. Requires loaded mesh root
// and supplied original numeric placement fields; no registry/world creation.
// Cached radius uses unscaled root half extents, as11F680 does. No broad sphere
// replacement. Caller-supplied registry masks retain their source identities.
bool prepareTrackCollider50AO(const CollisionMesh48I&,const std::array<float,3>&position,
 const std::array<float,3>&look,const std::array<float,3>&up,const char*name,
 std::size_t nameBytes,TrackCollisionMasks50AN,TrackCollider50AO&) noexcept;
}
