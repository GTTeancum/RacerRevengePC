#pragma once
#include "tb_cold_sources50la.h"
#include "ma_trigger_authoring_fixture.h"
#include "original_legacy_motion50ig_fixture.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/collision_mesh48i.h"
#include "../../src/game/type2_resource49x.h"
#include <algorithm>
#include <cstring>
#include <limits>
#include <thread>

namespace tb_door_motion50TB {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t ownerCount=1,nodeCount=3,sceneNodeCount=4,vertexCount=314,indexCount=510,surfaceCount=1;
inline constexpr std::size_t collisionVertexCount=72,collisionNodeCount=109,collisionFaceCount=110;
inline constexpr std::size_t tobBytes=106139;
inline constexpr const char* tobSha="12c908874736ea91dcd6d3f8af97413a0cfe3bcd87cfce0798e8ff86fe5fb0bc";
struct Diagnostic {
 std::string message;a::DecodeError48C decode{};a::GeometryDiagnostic49O geometry{};
 a::MotionBindingDiagnostic49O binding{};std::size_t retainedPayloadBytes=0,reservedPeakPayloadBytes=0;
 bool motionLibmCommitted=false;
};
struct Authored {
 std::uint32_t startingZone=4,numberOfMotions=1,numberOfSequences=1,numberInSequence=1,motionSequence=1;
 bool loopSequence=false,sequenceActive=false,oneTimeOnly=true,useCollision=true,useLighting=true;
 bool authoredUseBlendedKeys=true,hasAuthoredUseCollision=false;
 std::array<float,3> offset{};
};
namespace detail {
using ga_trigger_authoring::detail::need;using ga_trigger_authoring::detail::block;
using ga_trigger_authoring::detail::optional;using ga_trigger_authoring::detail::field;
using ga_trigger_authoring::detail::integer;using ga_trigger_authoring::detail::number;
using ga_trigger_authoring::detail::vector;
inline bool boolean(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-'a'+'A');return ga_trigger_authoring::detail::boolean(s);}
inline void add(std::size_t& n,std::size_t x)noexcept{n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;}
template<class T> inline void payload(std::size_t& n,const std::vector<T>& v)noexcept {
 if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(n,v.capacity()*sizeof(T));
}
inline void textPayload(std::size_t& n,const std::string& s)noexcept{add(n,s.capacity());add(n,1);}
}
// Immutable actual source owner. It neither samples motion nor attaches any
// observer/collider. COL0 binds the separate model root, never a visual panel.
class SourceOwner final { public: // host check: retired owner opened for the pose comparison
 std::shared_ptr<const tb_cold_sources50LA::Owner> cold_;
 std::vector<std::uint8_t> psg_,vut_,originalMot_,mot_,mcf_,col_,psm_,cache_;
 a::PsgHierarchyView49O hierarchy_{};a::MotionNodeBindings49O bindings_{};
 a::MotionView49O motion_{};a::Scene48D scene_{};a::CollisionMesh48I collision_{};
 std::array<float,collisionNodeCount> radii_{};std::size_t depth_=0;Authored authored_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false,libmCommitted_=false;
 bool fail(Diagnostic& d,const char* text){d.message=text;d.motionLibmCommitted=libmCommitted_;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
public:
 SourceOwner()=default;SourceOwner(const SourceOwner&)=delete;SourceOwner& operator=(const SourceOwner&)=delete;
 SourceOwner(SourceOwner&&)=delete;SourceOwner& operator=(SourceOwner&&)=delete;
 ~SourceOwner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Source-specific conservative reservation for complete owner + decoded
 // Scene/COL/cache and overlapping parser/converter allocations. It is charged
 // before extraction, then compared with actual retained capacities. Shared
 // cold archive/navigation are already charged by their owner and excluded.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  constexpr std::size_t raw=8880+73+656+660+205+6860+2580+16600;
  constexpr std::size_t scene=3*sizeof(a::SceneNode48D)+sizeof(a::SceneLod48D)+sizeof(a::SceneSurface48D)+
    vertexCount*sizeof(a::SceneVertex48D)+vertexCount*3*sizeof(std::uint32_t)+
    vertexCount*(sizeof(a::SceneStrip48D)+sizeof(a::SceneContinuation48D))+16+vertexCount*24+indexCount*4;
  constexpr std::size_t col=2*collisionVertexCount*sizeof(std::array<float,3>)+
    collisionNodeCount*(sizeof(a::CollisionNode48G)+sizeof(a::CollisionMeshNode48I))+
    collisionFaceCount*(sizeof(a::CollisionFace48G)+sizeof(a::CollisionMeshFace48I));
  return sizeof(SourceOwner)+4*(raw+scene+col)+65536;
 }
 std::size_t retainedPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;using namespace detail;std::size_t n=sizeof(SourceOwner);
  for(const auto* v:{&psg_,&vut_,&originalMot_,&mot_,&mcf_,&col_,&psm_,&cache_})payload(n,*v);
  payload(n,scene_.nodes);for(const auto& row:scene_.nodes)textPayload(n,row.name);
  payload(n,scene_.materials);for(const auto& name:scene_.materials)textPayload(n,name);
  payload(n,scene_.lods);for(const auto& lod:scene_.lods){payload(n,lod.surfaces);for(const auto& s:lod.surfaces){
   payload(n,s.vertices);payload(n,s.indices);payload(n,s.strips);payload(n,s.continuations);}}
  payload(n,scene_.neutral_lod0);payload(n,scene_.spans_lod0);
  payload(n,collision_.vertices);payload(n,collision_.vertex_normals);payload(n,collision_.nodes);payload(n,collision_.faces);return n;
 }
 bool decode(std::shared_ptr<const tb_cold_sources50LA::Owner> cold,g::SourceLibmState49T& actualLibm,
      std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id()){d.message="TB door source thread mismatch";return false;}
  if(attempted_)return fail(d,"TB door source requires a fresh stable owner");attempted_=true;
  d.reservedPeakPayloadBytes=maximumAdditionalPayloadBytes();
  if(availableCpuPayloadBytes<d.reservedPeakPayloadBytes)return fail(d,"TB door source peak exceeds actual remaining payload budget");
  try{using namespace detail;
   need(std::fegetround()==FE_TONEAREST,"TB door source requires nearest arithmetic");
   need(cold&&cold->ready()&&cold->world().zones.size()==10&&cold->world().portals.size()==11&&
     cold->originalTob().size()==tobBytes&&pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tobSha,
     "TB door actual same-source identity");
   cold_=std::move(cold);const auto& p=cold_->triggerParameters();
   need(p.preservesDuplicateLabels&&p.original_bytes==cold_->originalTob()&&
     integer(field(block(p,"animations"),"numberofanimations"))==1,"TB counted animation/complete TOB backing");
   const auto& b=block(p,"animation1");
   need(field(b,"animationtype")=="KeyFramed"&&field(b,"mcf")=="tb_doorframe.mcf"&&
     field(b,"motiontoplay1")=="tb_doorframe"&&integer(field(b,"numberofmotions"))==1&&
     integer(field(b,"numberinsequence1"))==1&&integer(field(b,"motionsequence1"))==1&&
     !optional(b,"numberofsequences")&&!optional(b,"usecollision")&&!optional(b,"lookvector")&&
     !optional(b,"upvector")&&!optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"TB door exact source route");
   Authored next{};next.startingZone=integer(field(b,"startingzone"));
   next.loopSequence=boolean(field(b,"loopsequence1"));next.sequenceActive=boolean(field(b,"sequenceactive1"));
   next.oneTimeOnly=boolean(field(b,"onetimeonly"));next.useLighting=boolean(field(b,"uselighting"));
   next.authoredUseBlendedKeys=boolean(field(b,"useblendedkeys"));const auto v=vector(field(b,"offset"));
   for(unsigned i=0;i<3;++i)next.offset[i]=v.v[i];
   need(next.startingZone==4&&!next.loopSequence&&!next.sequenceActive&&next.oneTimeOnly&&next.useLighting&&
     next.authoredUseBlendedKeys&&next.offset==std::array<float,3>{},"TB door flags/placement changed");
   // Original2D830C..318 default1. Retain absence independently of effective value.
   next.useCollision=true;next.hasAuthoredUseCollision=false;authored_=next;
   const auto member=[&](const char* name,std::size_t bytes,const char* sha,std::vector<std::uint8_t>& out){
    const bool extracted=cold_->extractMember(name,out,d.decode,bytes);
    need(extracted&&out.size()==bytes&&pod_materials50ed_fixture::detail::sha256(out)==sha,"TB door authentic member extraction rejected");
   };
   member("anim\\tb_doorframe.mcf",205,"784cec7935e906b56f63c49ad6d31e3ce46f98dbc201b25e69d8691384d233a1",mcf_);
   member("anim\\tb_doorframe.psg",8880,"e03a70754e57253d7b0a1b3d69f2d807f44f02387e41240c4442030f62e2a776",psg_);
   member("anim\\tb_doorframe.vut",73,"dc9bef4bf347d0423e776d77d57598b1b330ecfd5637806b443607da2f95ba7a",vut_);
   member("textures\\tb_door.psm",2580,"9c0e98e6e9cfad9d63fdbc27c087375094eb281851ffc72172509ac1e36996ad",psm_);
   need(a::readPsgHierarchy49O(psg_.data(),psg_.size(),hierarchy_,d.geometry)&&hierarchy_.nodeCount==nodeCount,"TB actual PSG hierarchy");
   need(a::readMotionNodeBindings49O(hierarchy_,reinterpret_cast<const char*>(vut_.data()),vut_.size(),bindings_,d.binding)&&
     bindings_.count==nodeCount,"TB actual VUT rows");
   constexpr const char* names[]={"TB_DOORFRAME","TB_DOOR_L","TB_DOOR_R"};
   for(std::uint32_t i=0;i<nodeCount;++i){const auto& row=bindings_.rows[i];
    need(row.sceneNode==i&&std::strcmp(row.name,names[i])==0&&row.listedFlag3c&&row.translationFlag38==(i!=0),"TB actual binding identity/order");
    ++bindings_.rows[i].sceneNode; // Separate authentic model root precedes PSG nodes.
   }
   a::SceneLimits48D visualLimits{};visualLimits.input_bytes=psg_.size();visualLimits.vertices_all_lods=vertexCount;
   // Own pinned DiffuseCompressed material plus actual integer packet source.
   visualLimits.allow_signed32_quantized_positions=true;
   need(a::decodeScene48D(psg_,scene_,d.decode,nullptr,visualLimits)&&scene_.nodes.size()==nodeCount&&scene_.lods.size()==1&&
     !scene_.world_space&&scene_.identity_matrix_palette&&scene_.materials.size()==1&&scene_.materials[0]=="tb_door"&&
     scene_.lods[0].surfaces.size()==surfaceCount,"TB actual compressed rigid scene");
   const auto& s=scene_.lods[0].surfaces[0];
   need(s.vertices.size()==vertexCount&&s.indices.size()==indexCount&&s.material_index==0&&s.component_word==0x107&&
     !s.has_original_colors&&s.groups==22&&s.source_strips==72,"TB exact visual source extents");
   std::array<std::size_t,3> boneCounts{};for(const auto& v:s.vertices){need(v.bone<nodeCount,"TB visual palette bound");++boneCounts[v.bone];}
   need(boneCounts==std::array<std::size_t,3>{204,54,56},"TB original visual bone identities");
   member("anim\\tb_doorframe.mot",656,"9c10a89a956fd3e623fd778a7dea47cb9e6f8219d5c7333c9c7bcc8b0e857bb8",originalMot_);
   std::string why;const bool converted=original_legacy_motion50IG::convert(originalMot_,actualLibm,mot_,why);
   if(!converted){d.message=why;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
   libmCommitted_=true;d.motionLibmCommitted=true;
   need(mot_.size()==540&&a::readMot49O(mot_.data(),mot_.size(),motion_,d.geometry)&&motion_.frameCount==5&&
     motion_.positionCount==2&&motion_.rotationCount==2&&motion_.totalKeys==20&&motion_.rotationType==0,"TB original MOT runtime representation");
   std::uint32_t duration=0;std::memcpy(&duration,&motion_.duration,4);need(duration==0x3e888889u,"TB original MOT duration; never MCF retimed");
   for(unsigned i=0;i<2;++i)need(motion_.position[i].rowIndex==i+1&&motion_.rotation[i].rowIndex==i+1&&
     motion_.position[i].keyCount==5&&motion_.rotation[i].keyCount==5,"TB original motion track ordering");
   member("anim\\tb_doorframe.col",6860,"5a781197b79f1e7ba55ed4bab1570dceeca5466eb685f4a65d5e8f250b562919",col_);
   {a::Collision48G tree;a::CollisionLimits48G limits;limits.input_bytes=col_.size();limits.vertices=collisionVertexCount;
    limits.nodes=collisionNodeCount;limits.faces=collisionFaceCount;
    need(a::decodeCollision48G(col_,tree,d.decode,nullptr,limits)&&tree.type==0&&tree.quantized==1&&tree.normals_present==1&&
      tree.vertices.size()==collisionVertexCount&&tree.nodes.size()==collisionNodeCount&&tree.faces.size()==collisionFaceCount,"TB genuine single root-bound COL0");
    const bool encoded=a::encodeCollisionAsset48G(tree,cache_,why);need(encoded,"TB native collision cache projection rejected");
    const bool loaded=a::loadCollisionMesh48I(cache_,collision_,why);need(loaded,"TB compact collision projection rejected");}
   std::array<std::uint32_t,collisionNodeCount+collisionFaceCount> topology{};
   need(g::prepareStaticTargetRadii49X(collision_,radii_.data(),radii_.size(),{topology.data(),topology.size()},depth_)==
     g::Type2ResourceStatus49X::Complete,"TB actual collision radius/topology rejected");
   need(retainedPayloadBytes()<=availableCpuPayloadBytes&&retainedPayloadBytes()<=maximumAdditionalPayloadBytes(),"TB actual source capacities exceeded reservation");
   ready_=true;d.message.clear();d.retainedPayloadBytes=retainedPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,e.what());}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const Authored& authored()const noexcept{return authored_;}
 const a::PsgHierarchyView49O& hierarchy()const noexcept{return hierarchy_;}
 const a::MotionNodeBindings49O& bindings()const noexcept{return bindings_;}
 const a::MotionView49O& motion()const noexcept{return motion_;}
 const a::Scene48D& scene()const noexcept{return scene_;}
 const a::CollisionMesh48I& collision()const noexcept{return collision_;}
 const std::array<float,collisionNodeCount>& nodeRadii()const noexcept{return radii_;}
 std::size_t maxDepth()const noexcept{return depth_;}
 const std::vector<std::uint8_t>& originalMcf()const noexcept{return mcf_;}
 const std::vector<std::uint8_t>& originalMot()const noexcept{return originalMot_;}
 const std::vector<std::uint8_t>& originalCol()const noexcept{return col_;}
};
}
