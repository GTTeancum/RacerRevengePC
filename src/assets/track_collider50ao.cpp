#include "track_collider50ao.h"
namespace rr::assets {
bool prepareTrackCollider50AO(const CollisionMesh48I&mesh,const std::array<float,3>&p,const std::array<float,3>&look,const std::array<float,3>&up,const char*name,std::size_t bytes,TrackCollisionMasks50AN masks,TrackCollider50AO&out)noexcept{
 if(mesh.nodes.empty())return false;std::array<float,16>placement{};TrackCollider50AO result{};
 if(!classifyTrackCollision50AN(name,bytes,masks,result.category)||!placementCollision48I(p,look,up,placement))return false;
 const auto&n=mesh.nodes[0];rr::game::GroupShapeGeometry48T geometry{{{n.center[0],n.center[1],n.center[2]}},{{n.half[0],n.half[1],n.half[2]}}};
 rr::game::GroupShapeSource48T source{};source.geometry=&geometry;rr::game::GroupShapeState48T initial{},state{};
 for(unsigned i=0;i<16;++i){source.local40.v[i]=mesh.local_matrix[i];initial.world80.v[i]=placement[i];}
 // Only composed matrix/sphere are returned; the second identical11F680 call
 // in the original creator has the same values for these projected outputs.
 if(rr::game::setGroupShape0Bounded48T(source,{{p[0],p[1],p[2]}},initial,state,result.sphere)!=rr::game::GroupShapeStatus48T::Complete)return false;
 result.world=state.composed00;out=result;return true;
}
}
