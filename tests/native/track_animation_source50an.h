#pragma once
// Common track (scenery) animation SOURCE owner: one loader for every course.
// Piece 2 of work\scratch\track-data-gaps\anim\ANIM.md. For each row of the track's own
// [Animations] table (src\assets\original_track_animations50an.h) it extracts and binds
// the files the original KeyFramedModel::Create 2D9060 / LpCharacter load 11A8E0 use:
//   <stem>.mcf  -> ContentDirectory, SLTFile (PSG), VUTfilename, [MOTION n] NAME/FPS/
//                  FrameCount/Looping (the motion library; MotionToPlay resolves by name,
//                  1523F0 -> stricmp 308F00)
//   PSG hierarchy + visual scene, VUT bindings, each motion's MOT (legacy v1 MOTs converted
//   as the original 152570 loader does), <stem>.col only when UseCollision.
// Nothing here is keyed on a course name, an archive SHA, a byte size or an owner count:
// counts come from the table, capacities from Limits (the one place that defines them).
// Data the original reads but no stock course authors and this port does not reproduce
// (Procedural models, Look/Up or Yaw/Pitch/Roll orientation, timed sounds) is refused with
// a plain message instead of being mis-loaded.
#include "original_legacy_motion50ig_fixture.h"
#include "../../src/assets/original_track_animations50an.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/model_geometry49o.h"
#include "../../src/assets/collision_mesh48i.h"
#include "../../src/game/type2_resource49x.h"
#include "../../src/game/scene_graph49p.h"
#include <algorithm>
#include <cfenv>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace track_animation_source50AN {
namespace a=rr::assets;namespace g=rr::game;
// The ONE place that defines this path's capacities (reader, source, runtime and window all
// use these). Vertex/index/span ceilings equal the renderer's dynamic-world caps
// (vehicle_window_gl50ds.h maxVehicleWindowDynamic*50DS); the window header static_asserts it.
struct Limits {
 a::TrackAnimationLimits50AN reader{};
 std::uint32_t vertices=8192,indices=32768,spans=64;      // whole course, one dynamic mesh
 std::uint32_t libraryMotions=256;                          // course-wide motion library
 std::uint32_t collisionParts=80;                           // per row (CollisionCompoundLimits50IJ default)
 std::size_t mcfBytes=16u*1024u,vutBytes=16u*1024u,colBytes=4u*1024u*1024u;
 std::size_t psgBytes=a::maxPsgBytes49O,motBytes=a::maxMotBytes49O;
};
inline const char* animationDirectory(){return "anim\\";} // [I] dynamic-model resource folder (all stock rows)
struct Diagnostic {
 std::string message;std::uint32_t row=0;std::string member;
 a::TrackAnimationDiagnostic50AN reader{};a::DecodeError48C decode{};
 a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};
};
// Archive member extraction for the course's own archive (case-insensitive member names, as
// every RES reader in the port). Returns false when the member is absent.
using Extract=std::function<bool(const std::string& member,std::vector<std::uint8_t>& out,std::size_t limit)>;
struct Motion {
 std::string name,mcfMember;                      // MCF [MOTION n] NAME and the MCF that defined it
 std::uint32_t fps=0,frameCount=0;                 // MCF values (diagnostic; MOT header owns duration)
 bool looping=false;                               // MCF Looping (motion+0xB4)
 bool legacy=false;                                // original MOT version1 converted cold
 std::vector<std::uint8_t> original,runtime;       // original bytes / runtime representation
 a::MotionView49O view{};
};
struct Row {
 a::TrackAnimation50AN authored;
 std::string mcfMember,psgMember,vutMember,colMember,contentDirectory;
 std::vector<std::uint8_t> mcf,psg,vut,vutFiltered,col;
 a::PsgHierarchyView49O hierarchy{};a::MotionNodeBindings49O bindings{};std::uint32_t skippedVutLines=0;
 a::Scene48D scene;
 std::vector<std::uint32_t> motions;               // MotionToPlay%d -> library index
 a::CollisionCompoundMesh50IJ collision;
 std::vector<std::vector<float>> radii;std::vector<std::size_t> depth;
 std::uint32_t graphCount=0,vertexCount=0,indexCount=0,surfaceCount=0;
};
namespace detail {
struct Fail {std::string message;};
inline void need(bool ok,const std::string& message){if(!ok)throw Fail{message};}
inline char lower(char c){return c>='A'&&c<='Z'?char(c-'A'+'a'):c;}
inline std::string lowered(std::string s){for(auto& c:s)c=lower(c);return s;}
inline bool iequal(const std::string& x,const std::string& y){return lowered(x)==lowered(y);}
inline std::string trim(const std::string& s){std::size_t b=0,e=s.size();
 while(b<e&&(s[b]==' '||s[b]=='\t'||s[b]=='\r'||s[b]=='\n'))++b;
 while(e>b&&(s[e-1]==' '||s[e-1]=='\t'||s[e-1]=='\r'||s[e-1]=='\n'))--e;return s.substr(b,e-b);}
// Same 158920 rule as the TOB reader: T/Y/F/N first letter, ON/OFF, else strtol(base 0).
inline std::int32_t integerValue(const std::string& s){
 if(!s.empty()){const char c=char(s[0]>='a'&&s[0]<='z'?s[0]-'a'+'A':s[0]);if(c=='T'||c=='Y')return 1;if(c=='F'||c=='N')return 0;}
 const auto u=lowered(s);if(u=="on")return 1;if(u=="off")return 0;return static_cast<std::int32_t>(std::strtol(s.c_str(),nullptr,0));
}
struct McfSection {std::string name;std::vector<std::pair<std::string,std::string>> keys;};
// INI text: [Section] then key=value lines; CR/LF; keys compared case-insensitively.
inline std::vector<McfSection> parseMcf(const std::vector<std::uint8_t>& bytes){
 std::vector<McfSection> out;const std::string text(bytes.begin(),bytes.end());std::size_t at=0;
 while(at<text.size()){auto end=text.find('\n',at);if(end==std::string::npos)end=text.size();
  const auto line=trim(text.substr(at,end-at));at=end+1;
  if(line.empty()||line[0]==';')continue;
  if(line.front()=='['){const auto close=line.find(']');need(close!=std::string::npos,"MCF section header not closed");out.push_back({trim(line.substr(1,close-1)),{}});continue;}
  const auto eq=line.find('=');need(eq!=std::string::npos,"MCF line without '='");need(!out.empty(),"MCF key before any section");
  out.back().keys.emplace_back(trim(line.substr(0,eq)),trim(line.substr(eq+1)));
 }
 return out;
}
inline const McfSection* section(const std::vector<McfSection>& s,const std::string& name){
 const McfSection* found=nullptr;for(const auto& x:s)if(iequal(x.name,name)){need(!found,"MCF duplicate section "+name);found=&x;}return found;}
inline const std::string* key(const McfSection& s,const std::string& name){
 const std::string* found=nullptr;for(const auto& k:s.keys)if(iequal(k.first,name)){need(!found,"MCF duplicate key "+name);found=&k.second;}return found;}
inline std::string stem(const std::string& file){const auto dot=file.find('.');return file.substr(0,dot);}
inline std::string inDirectory(const std::string& directory,const std::string& name){
 return name.find('\\')!=std::string::npos||name.find('/')!=std::string::npos?name:directory+name;}
// VUT text: the original tokenizer turns an empty line into a name that matches no PSG row,
// 153248..15325C then skips the line (SA conrobo06 ends in one). Drop empty lines the same
// way for every row and count them; every non-empty line goes to the strict 49O reader.
inline std::vector<std::uint8_t> filterVut(const std::vector<std::uint8_t>& in,std::uint32_t& skipped){
 std::vector<std::uint8_t> out;skipped=0;std::size_t at=0;
 while(at<in.size()){std::size_t end=at;while(end<in.size()&&in[end]!='\n')++end;
  const std::size_t stop=end<in.size()?end+1:end;bool blank=true;
  for(std::size_t i=at;i<end;++i)if(in[i]!='\r'){blank=false;break;}
  if(blank)++skipped;else out.insert(out.end(),in.begin()+static_cast<std::ptrdiff_t>(at),in.begin()+static_cast<std::ptrdiff_t>(stop));
  at=stop;
 }
 return out;
}
inline std::uint32_t u32(const std::uint8_t* p){return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
} // namespace detail

class Owner final {
 Limits limits_{};
 a::TrackAnimationTable50AN table_{};
 std::vector<std::unique_ptr<Row>> rows_;
 std::vector<std::unique_ptr<Motion>> library_;
 std::uint32_t vertices_=0,indices_=0,spans_=0,conversions_=0;
 bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 // tob: the course's own parsed TOB. zoneCount: the loaded world's zones (2D90DC assert).
 // libm: the session's source libm state; legacy MOT conversion commits into it only on success.
 bool load(const a::ParameterDocument48F& tob,std::uint32_t zoneCount,const Extract& extract,
      g::SourceLibmState49T& libm,Diagnostic& d,const Limits& limits=Limits{}){
  d={};if(attempted_){d.message="track animation source requires a fresh owner";return false;}attempted_=true;limits_=limits;
  try{using namespace detail;
   need(std::fegetround()==FE_TONEAREST,"track animation source requires nearest arithmetic");
   need(static_cast<bool>(extract),"track animation source has no archive extractor");
   // Read first, then build the message: an argument-order-dependent message lost the reason.
   const bool tableRead=a::readTrackAnimations50AN(tob,zoneCount,limits_.reader,table_,d.reader);
   need(tableRead,"track animation table rejected: "+d.reader.message);
   auto nextLibm=libm;
   for(std::size_t i=0;i<table_.animations.size();++i){
    d.row=static_cast<std::uint32_t>(i+1);const auto& authored=table_.animations[i];
    // ProceduralModel 2D97A0/2D9860 (path-following vehicles) is not designed; no stock course uses it.
    need(authored.type==a::TrackAnimationType50AN::keyFramed,"AnimationType Procedural is not supported (no stock course uses it)");
    // 2D9264: Look==(0,0,0) -> Yaw/Pitch/Roll (157100) else Look/Up (289EC0). Neither rotation builder
    // is ported; every stock row authors neither, so a non-zero one is refused, not mis-placed.
    need(authored.orientationFromAngles&&authored.yawDegrees==0.f&&authored.pitchDegrees==0.f&&authored.rollDegrees==0.f,
      "animation orientation from Look/Up or Yaw/Pitch/Roll is not ported yet (no stock course authors it)");
    // Timed one-shot sounds (278F30) have no port owner; no stock row authors one.
    need(authored.timedSoundCount==0,"animation NumberOfSounds (timed sounds) is not supported yet");
    need(authored.mcfStem!="none"&&!authored.mcfStem.empty(),"animation row names no MCF");
    auto row=std::make_unique<Row>();row->authored=authored;
    row->mcfMember=inDirectory(animationDirectory(),authored.mcfStem+".mcf");
    d.member=row->mcfMember;need(extract(row->mcfMember,row->mcf,limits_.mcfBytes)&&!row->mcf.empty(),"MCF member missing: "+row->mcfMember);
    const auto mcf=parseMcf(row->mcf);
    const auto* general=section(mcf,"General info");need(general,"MCF [General info] missing");
    const auto* motionCount=key(*general,"NumberOfMotions");const auto* content=key(*general,"ContentDirectory");
    const auto* slt=key(*general,"SLTFile");const auto* vut=key(*general,"VUTfilename");
    need(content&&slt&&vut,"MCF ContentDirectory/SLTFile/VUTfilename missing");
    row->contentDirectory=*content;
    const std::int32_t motions=motionCount?integerValue(*motionCount):0;
    need(motions>=0&&static_cast<std::uint32_t>(motions)<=limits_.reader.motions,"MCF NumberOfMotions outside limits");
    for(std::int32_t m=1;m<=motions;++m){
     const auto* ms=section(mcf,"MOTION "+std::to_string(m));need(ms,"MCF [MOTION n] block missing");
     const auto* name=key(*ms,"NAME");need(name&&!name->empty(),"MCF motion NAME missing");
     bool known=false;for(const auto& x:library_)if(iequal(x->name,*name)){known=true;break;}
     if(known)continue; // the course-wide library keeps the first motion of a name [I]
     need(library_.size()<limits_.libraryMotions,"track motion library capacity reached");
     auto motion=std::make_unique<Motion>();motion->name=*name;motion->mcfMember=row->mcfMember;
     if(const auto* v=key(*ms,"FPS"))motion->fps=static_cast<std::uint32_t>(std::max(0,integerValue(*v)));
     if(const auto* v=key(*ms,"FrameCount"))motion->frameCount=static_cast<std::uint32_t>(std::max(0,integerValue(*v)));
     if(const auto* v=key(*ms,"Looping"))motion->looping=integerValue(*v)!=0;
     // The motion file is the VUE stem's MOT (MC zbreak1floor.mcf: NAME=zbreak1, VUE=zbreak1floor.vue,
     // member anim\zbreak1floor.mot); NAME is the library key MotionToPlay resolves against.
     const auto* vue=key(*ms,"VUE");
     const auto member=inDirectory(row->contentDirectory,(vue&&!vue->empty()?stem(*vue):*name)+".mot");d.member=member;
     need(extract(member,motion->original,limits_.motBytes)&&motion->original.size()>=4,"MOT member missing: "+member);
     // 152570: version -2 is the runtime form; a version1 (frame count first) MOT is converted.
     if(u32(motion->original.data())==0xfffffffeu)motion->runtime=motion->original;
     else{std::string why;motion->legacy=true;
      need(original_legacy_motion50IG::convert(motion->original,nextLibm,motion->runtime,why),"legacy MOT conversion rejected ("+member+"): "+why);++conversions_;}
     need(a::readMot49O(motion->runtime.data(),motion->runtime.size(),motion->view,d.geometry),"MOT rejected: "+member);
     library_.push_back(std::move(motion));
    }
    row->psgMember=inDirectory(row->contentDirectory,stem(*slt)+".psg");d.member=row->psgMember;
    need(extract(row->psgMember,row->psg,limits_.psgBytes)&&!row->psg.empty(),"PSG member missing: "+row->psgMember);
    row->vutMember=inDirectory(row->contentDirectory,*vut);d.member=row->vutMember;
    need(extract(row->vutMember,row->vut,limits_.vutBytes)&&!row->vut.empty(),"VUT member missing: "+row->vutMember);
    need(a::readPsgHierarchy49O(row->psg.data(),row->psg.size(),row->hierarchy,d.geometry),"PSG hierarchy rejected: "+row->psgMember);
    need(row->hierarchy.nodeCount+1<=g::maxSceneNodes49P,"animation model node count above the scene graph capacity");
    row->vutFiltered=filterVut(row->vut,row->skippedVutLines);
    need(!row->vutFiltered.empty()&&a::readMotionNodeBindings49O(row->hierarchy,reinterpret_cast<const char*>(row->vutFiltered.data()),
      row->vutFiltered.size(),row->bindings,d.binding,a::MotionBindingMissingNode49O::sourceDiagnosticSkip),"VUT bindings rejected: "+row->vutMember);
    // The runtime graph has a separate model root at index 0 (the KeyFramedModel node).
    for(std::uint32_t r=0;r<row->bindings.count;++r){need(row->bindings.rows[r].sceneNode<row->hierarchy.nodeCount,"VUT binding outside the PSG");++row->bindings.rows[r].sceneNode;}
    row->graphCount=row->hierarchy.nodeCount+1;
    a::SceneLimits48D visual{};visual.input_bytes=row->psg.size();visual.allow_signed32_quantized_positions=true;
    need(a::decodeScene48D(row->psg,row->scene,d.decode,nullptr,visual),"PSG visual decode rejected: "+row->psgMember);
    need(!row->scene.lods.empty()&&!row->scene.world_space&&row->scene.identity_matrix_palette&&row->scene.nodes.size()==row->hierarchy.nodeCount,
      "animation PSG is not a rigid node-palette model: "+row->psgMember);
    for(const auto& surface:row->scene.lods[0].surfaces){
     ++row->surfaceCount;row->vertexCount+=static_cast<std::uint32_t>(surface.vertices.size());row->indexCount+=static_cast<std::uint32_t>(surface.indices.size());
     need(surface.material_index<row->scene.materials.size(),"animation surface material outside the PSG material table");
     for(const auto& v:surface.vertices)need(v.bone<row->hierarchy.nodeCount,"animation vertex palette index outside the PSG");
     for(const auto x:surface.indices)need(x<surface.vertices.size(),"animation surface index outside its vertices");
    }
    need(vertices_+row->vertexCount<=limits_.vertices&&indices_+row->indexCount<=limits_.indices&&spans_+row->surfaceCount<=limits_.spans,
      "track animations exceed the one dynamic-world window (vertices/indices/spans)");
    vertices_+=row->vertexCount;indices_+=row->indexCount;spans_+=row->surfaceCount;
    for(const auto& name:authored.motionNames){std::uint32_t found=UINT32_MAX;
     for(std::uint32_t k=0;k<library_.size();++k)if(iequal(library_[k]->name,name)){found=k;break;}
     need(found!=UINT32_MAX,"MotionToPlay names a motion no MCF defines: "+name);row->motions.push_back(found);}
    for(const auto& q:authored.sequences)need(!q.motions.empty(),"animation sequence without motions");
    need(!row->motions.empty(),"animation row has no motion (NumberOfMotions 0)");
    if(authored.useCollision){
     row->colMember=inDirectory(animationDirectory(),authored.mcfStem+".col");d.member=row->colMember;
     need(extract(row->colMember,row->col,limits_.colBytes)&&!row->col.empty(),"COL member missing: "+row->colMember);
     std::string why;a::CollisionCompoundLimits50IJ cl{};cl.members=limits_.collisionParts;cl.tree.input_bytes=row->col.size();
     if(!a::loadCollisionCompoundMesh50IJ(row->col,row->collision,why,cl)){
      // COL0: one root-bound mesh (TA barges, TB door). Carried as a one-member compound on the
      // model root (node 0) with the COL's own local matrix [I: the original container type of a
      // COL0 LpCharacter collision was not traced; the geometry and its root binding are the data's].
      a::Collision48G tree;a::CollisionLimits48G tl;tl.input_bytes=row->col.size();a::DecodeError48C e{};
      std::vector<std::uint8_t> cache;std::string why0;a::CollisionCompoundMeshMember50IJ member{};
      need(a::decodeCollision48G(row->col,tree,e,nullptr,tl)&&tree.type==0,"COL rejected ("+row->colMember+"): "+why+" / COL0: "+e.code);
      need(a::encodeCollisionAsset48G(tree,cache,why0)&&a::loadCollisionMesh48I(cache,member.mesh,why0),"COL0 projection rejected ("+row->colMember+"): "+why0);
      member.scene_node_index=0;row->collision={};row->collision.members.push_back(std::move(member));
     }
     need(row->collision.reverse_mode==0&&!row->collision.members.empty(),"COL has no compound members: "+row->colMember);
     row->radii.resize(row->collision.members.size());row->depth.resize(row->collision.members.size());
     for(std::size_t p=0;p<row->collision.members.size();++p){const auto& mesh=row->collision.members[p].mesh;
      need(mesh.reverse_mode==0,"COL member reverse mode not supported");
      need(row->collision.members[p].scene_node_index<row->graphCount,"COL member names a node outside the model");
      row->radii[p].resize(mesh.nodes.size());std::vector<std::uint32_t> topology(mesh.nodes.size()+mesh.faces.size());
      need(g::prepareStaticTargetRadii49X(mesh,row->radii[p].data(),row->radii[p].size(),{topology.data(),topology.size()},row->depth[p])==
        g::Type2ResourceStatus49X::Complete,"COL radius/topology preparation rejected: "+row->colMember);
     }
    }
    rows_.push_back(std::move(row));
   }
   d.row=0;d.member.clear();libm=nextLibm;ready_=true;return true;
  }catch(const detail::Fail& f){d.message=f.message;}
   catch(const std::exception& e){d.message=e.what();}
  rows_.clear();library_.clear();return false;
 }
 bool ready()const noexcept{return ready_;}
 const Limits& limits()const noexcept{return limits_;}
 const a::TrackAnimationTable50AN& table()const noexcept{return table_;}
 std::size_t rowCount()const noexcept{return rows_.size();}
 const Row& row(std::size_t i)const{return *rows_.at(i);}
 std::size_t motionCount()const noexcept{return library_.size();}
 const Motion& motion(std::size_t i)const{return *library_.at(i);}
 std::uint32_t vertexCount()const noexcept{return vertices_;}
 std::uint32_t indexCount()const noexcept{return indices_;}
 std::uint32_t spanCount()const noexcept{return spans_;}
 std::uint32_t legacyConversions()const noexcept{return conversions_;}
};
} // namespace track_animation_source50AN
