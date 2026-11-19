#pragma once
#include "tc_trigger_authoring50tc.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/model_geometry49o.h"
#include <cstring>
#include <limits>

namespace tc_gate_motion50TC {
namespace a=rr::assets;
inline constexpr std::size_t ownerCount=1,nodeCount=7,sceneNodeCount=8,vertexCount=490,indexCount=912,surfaceCount=1;
struct Diagnostic {std::string message;a::DecodeError48C decode{};a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};};
struct Authored {
 std::uint32_t startingZone=8,numberOfMotions=1,numberOfSequences=1,numberInSequence=1,motionSequence=1;
 bool loopSequence=false,sequenceActive=false,oneTimeOnly=true,useCollision=false,useLighting=true;
 bool authoredUseBlendedKeys=true; // Retained authoring; no invented parser consumer.
 std::array<float,3> offset{};
};
// Read-only resource owner. It does not sample a pose, advance an animation,
// fabricate a collider, instantiate dormant Animation2..8, or replay TOB/nav.
class SourceOwner final { public: // host check: retired owner opened for the pose comparison
 std::shared_ptr<const tc_cold_sources50LA::Owner> cold_;
 std::vector<std::uint8_t> psg_,vut_,mot_,mcf_;
 a::PsgHierarchyView49O hierarchy_{};a::MotionNodeBindings49O bindings_{};
 a::MotionView49O motion_{};a::Scene48D scene_{};Authored authored_{};
 bool attempted_=false,ready_=false;
 static std::string canonical(std::string s){for(auto& c:s){if(c=='\\')c='/';else if(c>='A'&&c<='Z')c=char(c+32);}return s;}
public:
 SourceOwner()=default;SourceOwner(const SourceOwner&)=delete;SourceOwner&operator=(const SourceOwner&)=delete;
 SourceOwner(SourceOwner&&)=delete;SourceOwner&operator=(SourceOwner&&)=delete;
 bool decode(std::shared_ptr<const tc_cold_sources50LA::Owner> cold,Diagnostic& d){
  d={};if(attempted_){d.message="TC gate source requires a fresh stable owner";return false;}attempted_=true;
  try{using namespace tc_trigger_authoring50TC::detail;
   need(cold&&cold->ready()&&cold->world().zones.size()==25&&cold->world().portals.size()==26&&
       cold->originalTob().size()==tc_trigger_authoring50TC::tobBytes&&pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tc_trigger_authoring50TC::tobSha,
       "TC gate same actual source identity");
   const auto& p=cold->triggerParameters();need(p.original_bytes==cold->originalTob(),"TC gate original parameter backing");
   need(integer(field(block(p,"animations"),"numberofanimations"))==ownerCount,"TC gate actual counted animation extent");
   const auto& b=block(p,"animation1");
   need(field(b,"animationtype")=="KeyFramed"&&field(b,"mcf")=="tc_gatepost.mcf"&&
       field(b,"motiontoplay1")=="tc_gatepost"&&integer(field(b,"numberofmotions"))==1&&
       integer(field(b,"numberinsequence1"))==1&&integer(field(b,"motionsequence1"))==1&&
       !optional(b,"numberofsequences")&&!optional(b,"lookvector")&&!optional(b,"upvector")&&
       !optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"TC gate exact single-motion source route");
   Authored next{};next.startingZone=integer(field(b,"startingzone"));
   next.loopSequence=boolean(field(b,"loopsequence1"));next.sequenceActive=boolean(field(b,"sequenceactive1"));
   next.oneTimeOnly=boolean(field(b,"onetimeonly"));next.useCollision=boolean(field(b,"usecollision"));
   next.useLighting=boolean(field(b,"uselighting"));next.authoredUseBlendedKeys=boolean(field(b,"useblendedkeys"));
   const auto offset=vector(field(b,"offset"));for(unsigned axis=0;axis<3;++axis)next.offset[axis]=offset.v[axis];
   need(next.startingZone==8&&!next.loopSequence&&!next.sequenceActive&&next.oneTimeOnly&&!next.useCollision&&
       next.useLighting&&next.authoredUseBlendedKeys&&next.offset==std::array<float,3>{},"TC gate source flags/placement changed");
   auto member=[&](const char* name,std::size_t bytes,const char* sha,std::vector<std::uint8_t>& out){
    std::size_t matches=0;for(const auto& e:cold->archive().entries)if(canonical(e.name)==canonical(name))++matches;
    need(matches==1&&a::extractResMember48C(cold->archive(),name,out,d.decode,bytes)&&out.size()==bytes&&
      pod_materials50ed_fixture::detail::sha256(out)==sha,"TC original gate member identity/decode");
   };
   for(const auto& e:cold->archive().entries)need(canonical(e.name)!="anim/tc_gatepost.col","TC explicit collision-disabled source gained COL");
   member("anim\\tc_gatepost.psg",12564,"ef788cf7271d97bdcc2216046230787968ef0addf54189c7cd6cce407e6e92e4",psg_);
   member("anim\\tc_gatepost.vut",163,"04c27906141db49c713e828abc0ad458c1f36431acd906bdf7ac103d5927ca39",vut_);
   member("anim\\tc_gatepost.mot",3580,"dccf6dd7c9e078d48ccd117ffb1b8a8efcb58741488d3ca1eed271878d0ad532",mot_);
   member("anim\\tc_gatepost.mcf",199,"1bfbd683cd30a66ce394d836cbfcc73ad6192ccc6a975b5b0482a21587eb3f04",mcf_);
   need(a::readPsgHierarchy49O(psg_.data(),psg_.size(),hierarchy_,d.geometry)&&hierarchy_.nodeCount==nodeCount,"TC gate actual PSG hierarchy");
   need(a::readMotionNodeBindings49O(hierarchy_,reinterpret_cast<const char*>(vut_.data()),vut_.size(),bindings_,d.binding)&&
        bindings_.count==nodeCount,"TC gate actual seven-row VUT");
   // Preserve original binding row order. The live graph owns one synthetic
   // model root before the seven actual PSG nodes, like the existing rigid path.
   for(std::uint32_t i=0;i<bindings_.count;++i){need(bindings_.rows[i].sceneNode<nodeCount,"TC gate original binding bound");++bindings_.rows[i].sceneNode;}
   need(a::readMot49O(mot_.data(),mot_.size(),motion_,d.geometry)&&motion_.frameCount==12&&
        motion_.positionCount==6&&motion_.rotationCount==6&&motion_.totalKeys==144&&motion_.rotationType==0,
        "TC gate direct ByteAxis/Bezier MOT extent");
   std::uint32_t duration=0;std::memcpy(&duration,&motion_.duration,sizeof duration);need(duration==0x3f8cccccu,"TC gate original MOT duration bits");
   for(std::uint32_t i=0;i<6;++i)need(motion_.position[i].rowIndex==i+1&&motion_.rotation[i].rowIndex==i+1&&
        motion_.position[i].keyCount==12&&motion_.rotation[i].keyCount==12,"TC gate original ordered motion tracks");
   need(a::decodeScene48D(psg_,scene_,d.decode)&&scene_.nodes.size()==nodeCount&&scene_.lods.size()==1&&
        !scene_.world_space&&scene_.identity_matrix_palette&&scene_.materials.size()==1&&scene_.materials[0]=="tc_gate"&&
        scene_.lods[0].surfaces.size()==surfaceCount,"TC gate genuine rigid visual owner");
   const auto& surface=scene_.lods[0].surfaces[0];
   need(surface.vertices.size()==vertexCount&&surface.indices.size()==indexCount&&surface.material_index==0&&
        surface.component_word==0x0000010f&&surface.has_original_colors,"TC gate exact original geometry/material extent");
   for(const auto& v:surface.vertices)need(v.bone<nodeCount,"TC gate actual vertex palette bound");
   for(const auto i:surface.indices)need(i<vertexCount,"TC gate actual source index bound");
   cold_=std::move(cold);authored_=next;ready_=true;d={};return true;
  }catch(const std::exception& e){d.message=e.what();return false;}
 }
 bool ready()const noexcept{return ready_&&cold_&&cold_->ready();}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const Authored& authored()const noexcept{return authored_;}
 const a::PsgHierarchyView49O& hierarchy()const noexcept{return hierarchy_;}
 const a::MotionNodeBindings49O& bindings()const noexcept{return bindings_;}
 const a::MotionView49O& motion()const noexcept{return motion_;}
 const a::Scene48D& scene()const noexcept{return scene_;}
 const std::vector<std::uint8_t>& originalMcf()const noexcept{return mcf_;}
 const std::vector<std::uint8_t>& originalMot()const noexcept{return mot_;}
};
}
