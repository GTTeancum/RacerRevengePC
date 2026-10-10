#include "zone_search_authoring50cn.h"
#pragma once
#include "track_collider50ao.h"
#include "track_height_cold50jb_fixture.h"
#include "import_world48f.h"
#include "import_collision48g.h"
#include "track_start50ad.h"
#include "vehicle_visual_authoring49r.h"
#include "grid_center50ac.h"
#include "reset_candidate50am.h"
#include "reset_probe50ag.h"
#include "json.hpp"
#include "pod_materials50ed_fixture.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <cstring>
#include <cstdlib>
#include <new>
#include <bit>
#include <cfenv>
#include <algorithm>
#include <cstddef>

namespace authored_runtime_ground50cp_fixture {
using namespace rr;using namespace rr::game;
inline void need(bool x,const char*m){if(!x){std::cerr<<m<<'\n';std::exit(1);}}
inline std::vector<std::uint8_t> read(const char*p){std::ifstream f(p,std::ios::binary);need(bool(f),p);return{std::istreambuf_iterator<char>(f),{}};}
inline std::string lower(std::string s){for(auto&c:s)if(c>='A'&&c<='Z')c+=32;return s;}
inline std::array<float,3> vec(const assets::ParameterBlock48F&b,const char*key){const std::string*s=nullptr;for(const auto&f:b.fields)if(f.name==key)s=&f.value;need(s,"missing authored vector");std::array<float,3> v{};std::size_t begin=0;for(unsigned i=0;i<3;++i){auto end=s->find(',',begin);if(end==std::string::npos)end=s->size();auto a=begin,z=end;while(a<z&&(*s)[a]==' ')++a;while(z>a&&(*s)[z-1]==' ')--z;need(assets::parseTrackNumber50AD(s->data()+a,z-a,v[i]),"original numeric parser");begin=end+1;}need(begin==s->size()+1,"vector trailing");return v;}
struct Owner{assets::CollisionMesh48I mesh;assets::TrackCollider50AO instance;std::string name;std::uint32_t zone=0,ordinal=0;};

struct ColdScene {std::vector<Owner> owners;BodyMatrix48G pose;float lift;std::size_t stackCount;nlohmann::json audit;assets::World48F world;track_height_cold50JB::Owner height;};
// A single original SearchList has 1024 physical slots. Keep capacity for
// the main, secondary and terrain-query references even if every collider
// and every dynamic reference is registered in the same method/zone.
inline constexpr std::size_t runtimeSearchSlotLimit50IG=1024;
inline constexpr std::size_t runtimeDynamicReferenceReserve50IG=3;
inline constexpr std::size_t hardCollisionOwners50IG=
 runtimeSearchSlotLimit50IG-runtimeDynamicReferenceReserve50IG;
// Keep the existing original-member admission: absent collision members do
// not create an owner. This is not a resource-closure check for a new track.
inline bool hasCollisionMember50IG(const assets::ResArchive48C& archive,
 const std::string& member){
 for(const auto& e:archive.entries)if(lower(e.name)==lower(member))return true;
 return false;
}
// The default remains the authenticated TA development fixture. Callers must
// separately admit a full track binding; this path adapter proves no new track.
struct Request {
 const char* archive="work/native/project/disc/TRACKS/TA.RES";
 const char* worldMember="tracks\\ta.scb";
 const char* mapSpline="tracks\\mta.spl";
 const char* podPhysics="work/impactAuthoring49Y/Pod01.phy";
 unsigned gridRank=4;
 const char* archiveSha256=nullptr;
 std::size_t archiveBytes=0;
 // Explicit optional fresh1A9EE0 source path; no reset/height-query activation.
 bool bindFreshGameTrackHeight=false;

 // Only a lower cold ceiling is supported; never resize owners during play.
 std::size_t collisionOwnerLimit=hardCollisionOwners50IG;
};
inline ColdScene load(assets::TrackCollisionMasks50AN masks,const Request& request=Request{}){
need(request.archive&&request.worldMember&&request.mapSpline&&request.podPhysics,"explicit original track source paths");
need(request.collisionOwnerLimit&&request.collisionOwnerLimit<=hardCollisionOwners50IG,"cold COL owner limit must leave three runtime references");
std::vector<Owner> owners;std::size_t stackCount=1;nlohmann::json audit=nlohmann::json::array();BodyMatrix48G pose{};float lift=0;assets::World48F world;track_height_cold50JB::Owner height;{assets::DecodeError48C error;assets::ResArchive48C archive;auto originalArchive=read(request.archive);
if(request.archiveSha256)need(request.archiveBytes==originalArchive.size()&&
 pod_materials50ed_fixture::detail::sha256(originalArchive)==request.archiveSha256,
 "selected original track archive identity differs");
need(assets::decodeResource48C(originalArchive,archive,error),"selected original track archive");std::vector<std::uint8_t>scb;need(assets::extractResMember48C(archive,request.worldMember,scb,error),"SCB");need(assets::decodeWorld48F(scb,world,error),"world");
if(request.bindFreshGameTrackHeight){
 need(request.archiveSha256&&request.archiveBytes,"height owners require authenticated original archive");
 std::string heightError;
 const bool heightReady=track_height_cold50JB::prepareFreshGameTrack(
     world,archive,request.worldMember,height,heightError);
 need(heightReady,
     heightError.empty()?"fresh source height ownership rejected":heightError.c_str());
}
for(std::size_t zone=0;zone<world.zones.size();++zone){const auto selection=assets::readZoneSearchSelection50CN(world,zone);need(selection.status==assets::ZoneSearchReadStatus50CN::Complete&&selection.selection.collisionStatic118==assets::ZoneSearchKind50CN::List&&selection.selection.collisionDynamic11C==assets::ZoneSearchKind50CN::List,"authored List collision method admission");}
// Count placements, not distinct filenames: repeated original COL members
// still own separate instances. Archive and world are immutable in both passes.
std::size_t collisionOwnerCount=0;
for(const auto& i:world.instances){
 if(!hasCollisionMember50IG(archive,i.collision_member))continue;
 need(collisionOwnerCount<request.collisionOwnerLimit,"original COL placements exceed cold runtime owner ceiling");
 ++collisionOwnerCount;
}
// The sole owners-backing reserve occurs before constructing any Owner. No
// sort, deduplication, second reserve or shrink can move prepared meshes/names.
// Moving the completed vector into ColdScene transfers its backing storage.
// The caller must then keep ColdScene and every owner unchanged/stable for as
// long as any live candidate borrows a mesh, name or other owner subobject.
owners.reserve(collisionOwnerCount);
for(const auto&i:world.instances){if(!hasCollisionMember50IG(archive,i.collision_member))continue;need(owners.size()<collisionOwnerCount,"original COL count changed after cold reserve");owners.emplace_back();auto&o=owners.back();o.name=i.collision_member;o.zone=i.zone;o.ordinal=i.ordinal;std::vector<std::uint8_t>raw,cache;assets::Collision48G decoded;std::string why;need(assets::extractResMember48C(archive,o.name,raw,error),"COL extraction");if(!assets::decodeCollision48G(raw,decoded,error)){std::cerr<<o.name<<' '<<error.code<<'\n';std::exit(1);}need(assets::encodeCollisionAsset48G(decoded,cache,why)&&assets::loadCollisionMesh48I(cache,o.mesh,why),"native COL cache");const auto&b=world.parameters.blocks[i.parameter_block];auto p=vec(b,"position"),look=vec(b,"lookvector"),up=vec(b,"upvector");need(assets::prepareTrackCollider50AO(o.mesh,p,look,up,o.name.data(),o.name.size(),masks,o.instance),"collider source preparation");stackCount=std::max(stackCount,o.mesh.nodes.size()+o.mesh.faces.size()+1);std::array<float,16> placement;need(assets::placementCollision48I(p,look,up,placement),"placement");auto words=[](const float*p,std::size_t n){std::vector<std::uint32_t>v;for(std::size_t k=0;k<n;++k)v.push_back(std::bit_cast<std::uint32_t>(p[k]));return v;};audit.push_back({{"zone",o.zone},{"ordinal",o.ordinal},{"name",o.name},{"local",words(o.mesh.local_matrix.data(),16)},{"placement",words(placement.data(),16)},{"center",words(o.mesh.nodes[0].center.data(),3)},{"half",words(o.mesh.nodes[0].half.data(),3)},{"world",words(o.instance.world.v,16)},{"sphere",words(o.instance.sphere.center.v,4)},{"radius",std::bit_cast<std::uint32_t>(o.instance.sphere.radius)},{"category",o.instance.category}});}
need(owners.size()==collisionOwnerCount,"original COL count changed during cold preparation");
std::vector<std::uint8_t>spline;need(assets::extractResMember48C(archive,request.mapSpline,spline,error),"spline");assets::TrackStartScratch50AD scratch{};assets::TrackStart50AD track{};assets::TrackStartDiagnostic50AD td{};need(assets::readTrackStart50AD(reinterpret_cast<const char*>(spline.data()),spline.size(),request.gridRank,scratch,track,td),"track start");auto phy=read(request.podPhysics);assets::VisualAuthoredValues49R visual{};assets::VisualAuthoringDiagnostic49R vd{};need(assets::readVehicleVisualAuthoring49R(reinterpret_cast<const char*>(phy.data()),phy.size(),visual,vd),"PHY");lift=visual.values[static_cast<unsigned>(assets::VisualAuthoredKey49R::MaxLiftHeight)];need(prepareCenterPose50AC(track.grid,lift,{{0,1,0}},pose),"pose");}return {std::move(owners),pose,lift,stackCount,std::move(audit),std::move(world),std::move(height)};}
inline ResetProbe50AG resolve(const ColdScene&scene,std::uint32_t floorMask,std::uint32_t mode,std::uint32_t*stack,std::size_t capacity){std::uint32_t p[4];for(unsigned i=0;i<4;++i)p[i]=std::bit_cast<std::uint32_t>(scene.pose.v[12+i]);ResetProbe50AG probe{};need(beginResetProbe50AG(p,std::bit_cast<std::uint32_t>(scene.lift),probe),"source probe begin");while(probe.phase==ResetProbePhase50AG::AwaitingQuery){SpringVector4_48N a{},b{};std::memcpy(a.v,probe.start,16);std::memcpy(b.v,probe.end,16);bool hit=false;std::uint32_t y=0;for(const auto&o:scene.owners){if(!(o.instance.category&floorMask))continue;ResetCandidateResult50AM result{};need(queryResetCandidate50AM(a,b,o.instance.world,{{o.instance.sphere.center.v[0],o.instance.sphere.center.v[1],o.instance.sphere.center.v[2]}},o.instance.sphere.radius,1,o.mesh,mode,stack,capacity,result)==ResetTreeStatus50AK::Complete,"source ground candidate");if(result.hit){hit=true;b=result.point;y=std::bit_cast<std::uint32_t>(b.v[1]);}}need(acceptResetProbe50AG(probe,hit,y),"source ground continuation");}return probe;}
}
