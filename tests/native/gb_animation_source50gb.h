#pragma once
#include "gb_cold_sources50la.h"
#include "ma_trigger_authoring_fixture.h"
#include "original_legacy_motion50ig_fixture.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/collision_mesh48i.h"
#include "../../src/game/type2_resource49x.h"
#include <algorithm>
#include <array>
#include <cfenv>
#include <cstring>
#include <limits>
#include <thread>

namespace gb_animation_source50GB {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t ownerCount=4,maximumPsgNodes=3,maximumSceneNodes=4,maximumParts=3;
inline constexpr std::size_t vertexCount=1795,indexCount=3393,surfaceCount=10,compoundPartCount=10;
inline constexpr std::size_t tobBytes=20617;
inline constexpr const char* tobSha="96469b3953b18d940bd0c8772758c68a75a69c3375b49ef98b966901c2e598d1";
struct Diagnostic {
 std::string message;std::size_t owner=0,completedOwners=0;
 a::DecodeError48C decode{};a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};
 std::size_t retainedPayloadBytes=0,reservedPeakPayloadBytes=0;std::uint32_t legacyConversionsCommitted=0;
};
struct Authored {
 std::uint32_t startingZone=0,numberOfMotions=1,numberOfSequences=1,numberInSequence=1,motionSequence=1;
 bool loopSequence=false,sequenceActive=false,oneTimeOnly=true,useCollision=true,useLighting=true;
 bool authoredUseBlendedKeys=true,hasAuthoredUseCollision=false,hasAuthoredNumberOfSequences=false;
 bool hasAuthoredNumberInSequence1=false,hasUnconsumedNumberInSequence=true;
 std::array<float,3> offset{};
};
struct Source {
 // Pointer-bearing views refer only into these same stable, immutable vectors.
 // Each counted source owns its original MOT and runtime representation even
 // when another counted source has byte-identical files.
 std::vector<std::uint8_t> mcf,psg,vut,originalMot,mot,col;
 a::PsgHierarchyView49O hierarchy{};a::MotionNodeBindings49O bindings{};a::MotionView49O motion{};
 a::Scene48D scene{};a::CollisionCompoundMesh50IJ collision{};
 std::array<std::vector<float>,maximumParts> radii{};std::array<std::size_t,maximumParts> depth{};
 Authored authored{};std::size_t collisionCount=0,graphCount=0;
};
namespace detail {
using ga_trigger_authoring::detail::need;using ga_trigger_authoring::detail::block;
using ga_trigger_authoring::detail::optional;using ga_trigger_authoring::detail::field;
using ga_trigger_authoring::detail::integer;using ga_trigger_authoring::detail::vector;
inline bool boolean(std::string s){for(auto& c:s)if(c>='a'&&c<='z')c=char(c-'a'+'A');return ga_trigger_authoring::detail::boolean(s);}
inline void add(std::size_t& n,std::size_t x)noexcept{n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;}
template<class T>inline void payload(std::size_t& n,const std::vector<T>& v)noexcept{
 if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(n,v.capacity()*sizeof(T));}
inline void textPayload(std::size_t& n,const std::string& s)noexcept{add(n,s.capacity());add(n,1);}
struct Pin {const char* name;std::size_t bytes;const char* sha;};
struct Spec {
 const char* name;std::uint32_t nodes,parts,surfaces,zone;std::array<float,3> offset;
 std::array<Pin,5> members; // MCF,PSG,VUT,MOT,COL in original names.
 std::array<const char*,3> nodeNames;std::array<const char*,4> materials;
 std::array<std::size_t,4> vertices,indices;std::array<std::size_t,3> bones;
 std::array<std::array<std::uint32_t,4>,3> collision; // original index,V,N,F.
 std::uint32_t frames,durationBits,positions,rotations,keys;bool legacy,loop;
};
inline constexpr std::array<Spec,ownerCount> specs{{
 {"Defensegate",3,3,2,2,
  {8006.125488f,2045.225220f,15852.375977f},
  {{{"anim\\defensegate.mcf",203,"693142685291f315ec08b23af578b9268fb372c936a41717594d75d2002722ee"},{"anim\\defensegate.psg",8596,"ec08fdbad87a0e9d0a262aac7d9a06a2daa4e22d55966187c8e7705a6dc28544"},{"anim\\defensegate.vut",115,"dd4dac1469c946650833ada2885cafa5fb60581ff1503f24efb390d6cf945a1b"},{"anim\\defensegate.mot",680,"548d03b3077f1e6f157a9a67c42a9aa6fa529980bc399b304b285830c1384b4b"},{"anim\\defensegate.col",12180,"020be4035ec49e008c28583d053e140e047605993e7ee9ad77e350bd177792a8"}}},
  {"DEFENSEGATE","DEFENSEGATE_LEFT_DOOR","DEFENSEGATE_RIGHT_DOOR"},{"fience02","fience_opening","",""},
  {292,8,0,0},{516,12,0,0},{292,4,4},
  {{{1,113,169,170},{2,8,11,12},{3,8,11,12}}},
  16,0x3f800000u,0,2,16,true,false},
 {"opening",2,2,4,2,
  {11453.098633f,2023.981812f,13247.233398f},
  {{{"anim\\opening.mcf",185,"992ef9ca4e4beee1d72f6728c3a9ec2abb3e119bf6c73ae55b4d0ee7ff5c61b8"},{"anim\\opening.psg",8764,"5070ad1c3d7435233ff14bf71aae91bc65fa65d9829d9cdeb5b58659756b0af6"},{"anim\\opening.vut",30,"7e2c6886fd6f7866967d325050ffef09775cd57abbdacd7e5ad67a08bdd728f9"},{"anim\\opening.mot",1500,"2461e850de0f00381d73ed0997068160ea2ebf7448e2665b2e4384dc9108a71f"},{"anim\\opening.col",11806,"be46bd3c513cea4b1bf18900244f2861f8050a2d7ebe84f2ba3fc845c2c00ed5"}}},
  {"OPENING","DOOR",""},{"market_hut_a01","opening_a","house_stand_e01","fience_opening"},
  {48,176,73,8},{78,366,111,12},{297,8,0},
  {{{1,107,178,179},{2,8,11,12},{0,0,0,0}}},
  15,0x40333333u,2,2,15,false,false},
 {"Defensegate2",3,3,2,3,
  {8006.125488f,2045.225220f,15852.375977f},
  {{{"anim\\defensegate2.mcf",205,"8591cb92f8ad955c11cf3f85331c86463ef740a7571f1b98a750fe498e33ed29"},{"anim\\defensegate.psg",8596,"ec08fdbad87a0e9d0a262aac7d9a06a2daa4e22d55966187c8e7705a6dc28544"},{"anim\\defensegate.vut",115,"dd4dac1469c946650833ada2885cafa5fb60581ff1503f24efb390d6cf945a1b"},{"anim\\defensegate2.mot",680,"548d03b3077f1e6f157a9a67c42a9aa6fa529980bc399b304b285830c1384b4b"},{"anim\\defensegate2.col",12180,"020be4035ec49e008c28583d053e140e047605993e7ee9ad77e350bd177792a8"}}},
  {"DEFENSEGATE","DEFENSEGATE_LEFT_DOOR","DEFENSEGATE_RIGHT_DOOR"},{"fience02","fience_opening","",""},
  {292,8,0,0},{516,12,0,0},{292,4,4},
  {{{1,113,169,170},{2,8,11,12},{3,8,11,12}}},
  16,0x3f800000u,0,2,16,true,false},
 {"windmill",2,2,2,13,
  {15091.685547f,1840.959229f,17813.195313f},
  {{{"anim\\windmill.mcf",189,"087f5bd9d8bad4da6386b100e4e8a6e08f5ac2aa7ecf0f27e29e1acea0b0cb8d"},{"anim\\windmill.psg",16420,"ad54673f60c3fd4c44f3fbcdd48073398a9a6caa1d1eeecdb5f5c726ca9d5856"},{"anim\\windmill.vut",48,"95e0e2874a2c48c39ef284ef22378cff891af9a55fab3c1058578fca9f68b5ee"},{"anim\\windmill.mot",352,"8dc3f486548ab0598ee4c56798fcbbc24f6bc9c4c8c61afa33b17d7ea159d29d"},{"anim\\windmill.col",36340,"94575aa9f6cf7b434e049ad706208e53c6ad7dec4f135c3494333d62f18f8e77"}}},
  {"WINDMILL","WINDMILL_BLADE",""},{"market_hut08","market_hut07","",""},
  {192,698,0,0},{456,1314,0,0},{426,464,0},
  {{{0,192,281,282},{1,190,303,304},{0,0,0,0}}},
  16,0x41700000u,0,1,16,true,true}
}};
inline constexpr std::array<Pin,7> materialPins{{
 {"textures\\fience02.psm",262,"d2ee13243e765b4131cc6e1aaad15ef06b6253c09e6a3a78f6e6e04c13df6391"},
 {"textures\\fience_opening.psm",569,"43c2defc92a04c61066e3f69138270603215f02f7bb104495fbb0efefa923eea"},
 {"textures\\market_hut_a01.psm",268,"869194ef061f1f557407021956df0ccf22f7c006ee5886411a54a9cd544cf938"},
 {"textures\\opening_a.psm",263,"6d49201a0a7b1af8bd37254217b941faecbe8256becf24df9fe233cf6d4e0f08"},
 {"textures\\house_stand_e01.psm",269,"627b6c6b47ce6b45661b7cf413dc0cbf961928664a133a24ead2b14cdf1a8243"},
 {"textures\\market_hut08.psm",362,"0e2d4f3f7b92dd67f9fad0a525a98c186caea4b25abb967a30a938b57813d6bf"},
 {"textures\\market_hut07.psm",268,"3e86af7165d97152fbd9cec5e6e565a5d18a971f24554cbe9170eb541bed3534"}
}};
}
// Authenticated cold resources only. No sample, dynamic registration, action,
// renderer authority or final world/navigation lease is created here.
class Owner final {
 std::shared_ptr<const gb_cold_sources50LA::Owner> cold_;
 std::array<Source,ownerCount> source_{};
 std::array<std::vector<std::uint8_t>,detail::materialPins.size()> materials_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false;std::size_t completed_=0;std::uint32_t conversions_=0;
 bool fail(Diagnostic& d,const char* text){d.message=text;d.completedOwners=completed_;
  d.legacyConversionsCommitted=conversions_;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Conservative complete source-specific phase reservation, charged before
 // extraction. Includes repeated actual resources, decoded native + parser
 // staging, conversion buffers, radii/topology scratch and SHA's full copied
 // input plus possible reallocation. Existing shared archive is excluded.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  std::size_t raw=4*(tobBytes+64);
  for(const auto& s:detail::specs){for(const auto& p:s.members)raw+=p.bytes;raw+=s.members[3].bytes+4;}
  for(const auto& p:detail::materialPins)raw+=p.bytes;
  constexpr std::size_t scene=10*sizeof(a::SceneNode48D)+4*sizeof(a::SceneLod48D)+10*sizeof(a::SceneSurface48D)+
    vertexCount*(sizeof(a::SceneVertex48D)+3*sizeof(std::uint32_t)+sizeof(a::SceneStrip48D)+sizeof(a::SceneContinuation48D)+24)+indexCount*4;
  constexpr std::size_t col=755*2*sizeof(std::array<float,3>)+1155*(sizeof(a::CollisionNode48G)+sizeof(a::CollisionMeshNode48I)+sizeof(float))+
    1165*(sizeof(a::CollisionFace48G)+sizeof(a::CollisionMeshFace48I))+compoundPartCount*(sizeof(a::CollisionCompoundMember50IJ)+sizeof(a::CollisionCompoundMeshMember50IJ))+(1155+1165)*4;
  return sizeof(Owner)+4*(raw+scene+col)+65536;
 }
 std::size_t retainedPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;using namespace detail;std::size_t n=sizeof(Owner);
  for(const auto& v:materials_)payload(n,v);
  for(const auto& x:source_){for(const auto* v:{&x.mcf,&x.psg,&x.vut,&x.originalMot,&x.mot,&x.col})payload(n,*v);
   payload(n,x.scene.nodes);for(const auto& row:x.scene.nodes)textPayload(n,row.name);
   payload(n,x.scene.materials);for(const auto& name:x.scene.materials)textPayload(n,name);
   payload(n,x.scene.lods);for(const auto& lod:x.scene.lods){payload(n,lod.surfaces);for(const auto& s:lod.surfaces){
    payload(n,s.vertices);payload(n,s.indices);payload(n,s.strips);payload(n,s.continuations);}}
   payload(n,x.scene.neutral_lod0);payload(n,x.scene.spans_lod0);payload(n,x.collision.members);
   for(const auto& part:x.collision.members){payload(n,part.mesh.vertices);payload(n,part.mesh.vertex_normals);payload(n,part.mesh.nodes);payload(n,part.mesh.faces);}
   for(const auto& r:x.radii)payload(n,r);
  }return n;
 }
 bool decode(std::shared_ptr<const gb_cold_sources50LA::Owner> cold,g::SourceLibmState49T& actualLibm,
      std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id()){d.message="GB animation source thread mismatch";return false;}
  if(attempted_)return fail(d,"GB animation source requires a fresh stable owner");attempted_=true;
  d.reservedPeakPayloadBytes=maximumAdditionalPayloadBytes();
  if(availableCpuPayloadBytes<d.reservedPeakPayloadBytes)return fail(d,"GB animation source peak exceeds actual remaining payload budget");
  try{using namespace detail;
   need(std::fegetround()==FE_TONEAREST,"GB animation source requires nearest arithmetic");
   need(cold&&cold->ready()&&cold->world().zones.size()==13&&cold->world().portals.size()==17&&
     cold->originalTob().size()==tobBytes&&pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tobSha,
     "GB animation actual same-source identity");
   cold_=std::move(cold);const auto& p=cold_->triggerParameters();
   need(!p.preservesDuplicateLabels&&p.original_bytes==cold_->originalTob()&&
     integer(field(block(p,"animations"),"numberofanimations"))==ownerCount,"GB counted animations/complete strict TOB backing");
   const auto member=[&](const Pin& pin,std::vector<std::uint8_t>& out){
    const bool extracted=cold_->extractMember(pin.name,out,d.decode,pin.bytes);
    need(extracted&&out.size()==pin.bytes&&pod_materials50ed_fixture::detail::sha256(out)==pin.sha,"GB authentic animation member rejected");};
   // Own authenticated DiffuseCompressed PSMs justify the exact existing
   // signed32 packet admission. No texture/GPU/material readiness is issued.
   for(std::size_t i=0;i<materials_.size();++i)member(materialPins[i],materials_[i]);
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& x=source_[i];const auto& s=specs[i];
    const auto& b=block(p,"animation"+std::to_string(i+1));
    need(field(b,"animationtype")=="KeyFramed"&&field(b,"mcf")==std::string(s.name)+".mcf"&&
      field(b,"motiontoplay1")==s.name&&integer(field(b,"numberofmotions"))==1&&
      integer(field(b,"numberinsequence"))==1&&integer(field(b,"motionsequence1"))==1&&
      !optional(b,"numberinsequence1")&&!optional(b,"numberofsequences")&&!optional(b,"usecollision")&&
      !optional(b,"probabilitylist1")&&!optional(b,"lookvector")&&!optional(b,"upvector")&&
      !optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"GB exact source animation/default route");
    auto& authored=x.authored;authored.startingZone=integer(field(b,"startingzone"));
    authored.loopSequence=boolean(field(b,"loopsequence1"));authored.sequenceActive=boolean(field(b,"sequenceactive1"));
    authored.oneTimeOnly=boolean(field(b,"onetimeonly"));authored.useLighting=boolean(field(b,"uselighting"));
    authored.authoredUseBlendedKeys=boolean(field(b,"useblendedkeys"));const auto v=vector(field(b,"offset"));
    for(unsigned k=0;k<3;++k)authored.offset[k]=v.v[k];
    need(authored.startingZone==s.zone&&authored.loopSequence==s.loop&&authored.sequenceActive==s.loop&&
      authored.oneTimeOnly!=s.loop&&authored.useLighting&&authored.authoredUseBlendedKeys&&authored.offset==s.offset,
      "GB actual animation flags/placement changed");
    // Original numbered lookup defaults1; unsuffixed raw field is retained,
    // not silently renamed. UseCollision default1 is2D830C..8318.
    member(s.members[0],x.mcf);member(s.members[1],x.psg);member(s.members[2],x.vut);
    need(a::readPsgHierarchy49O(x.psg.data(),x.psg.size(),x.hierarchy,d.geometry)&&x.hierarchy.nodeCount==s.nodes,"GB actual PSG hierarchy");
    need(a::readMotionNodeBindings49O(x.hierarchy,reinterpret_cast<const char*>(x.vut.data()),x.vut.size(),x.bindings,d.binding)&&x.bindings.count==s.nodes,"GB actual VUT bindings");
    x.graphCount=s.nodes+1;
    for(std::uint32_t row=0;row<s.nodes;++row){const auto& r=x.bindings.rows[row];
     need(r.sceneNode==row&&std::strcmp(r.name,s.nodeNames[row])==0&&r.listedFlag3c&&r.translationFlag38==(i==1),"GB source binding identity/order");
     ++x.bindings.rows[row].sceneNode; // Separate actual model root is index0.
    }
    a::SceneLimits48D visual{};visual.input_bytes=x.psg.size();visual.vertices_all_lods=static_cast<std::uint32_t>(vertexCount);
    visual.allow_signed32_quantized_positions=true;
    need(a::decodeScene48D(x.psg,x.scene,d.decode,nullptr,visual)&&x.scene.nodes.size()==s.nodes&&x.scene.lods.size()==1&&
      !x.scene.world_space&&x.scene.identity_matrix_palette&&x.scene.materials.size()==s.surfaces&&
      x.scene.lods[0].surfaces.size()==s.surfaces,"GB exact rigid animated scene");
    std::array<std::size_t,3> bones{};
    for(std::size_t j=0;j<s.surfaces;++j){const auto& surface=x.scene.lods[0].surfaces[j];
     need(x.scene.materials[j]==s.materials[j]&&surface.material_index==j&&surface.component_word==0x107&&
       !surface.has_original_colors&&surface.vertices.size()==s.vertices[j]&&surface.indices.size()==s.indices[j],"GB authentic visual surface order/extents");
     for(const auto& vertex:surface.vertices){need(vertex.bone<s.nodes,"GB actual rigid palette index");++bones[vertex.bone];}
    }
    need(bones==s.bones,"GB source visual palette populations");
    member(s.members[3],x.originalMot);
    if(s.legacy){std::string why;const bool converted=original_legacy_motion50IG::convert(x.originalMot,actualLibm,x.mot,why);
     if(!converted)return fail(d,why.c_str());++conversions_;d.legacyConversionsCommitted=conversions_;
    }else x.mot=x.originalMot;
    // Each successful converter commits its real libm prefix immediately.
    // Later owner/decode failure does not roll that prefix back or retry it.
    need(x.mot.size()==(i==1?1500u:i==3?164u:300u)&&a::readMot49O(x.mot.data(),x.mot.size(),x.motion,d.geometry)&&x.motion.frameCount==s.frames&&
      x.motion.positionCount==s.positions&&x.motion.rotationCount==s.rotations&&x.motion.totalKeys==(s.positions+s.rotations)*s.keys&&
      x.motion.rotationType==0,"GB original MOT runtime representation");
    std::uint32_t duration=0;std::memcpy(&duration,&x.motion.duration,4);need(duration==s.durationBits,"GB binary MOT duration; no MCF retime");
    for(std::uint32_t k=0;k<s.positions;++k)need(x.motion.position[k].rowIndex==k&&x.motion.position[k].keyCount==s.keys,"GB original position track order");
    for(std::uint32_t k=0;k<s.rotations;++k)need(x.motion.rotation[k].rowIndex==(i==1?k:k+1)&&x.motion.rotation[k].keyCount==s.keys,"GB original rotation track order");
    member(s.members[4],x.col);std::string why;a::CollisionCompoundLimits50IJ limits{};
    limits.members=s.parts;limits.tree.input_bytes=x.col.size();
    const bool loaded=a::loadCollisionCompoundMesh50IJ(x.col,x.collision,why,limits);
    need(loaded&&x.collision.reverse_mode==0&&x.collision.members.size()==s.parts,loaded?"GB exact compound source extent":why.c_str());
    x.collisionCount=s.parts;
    for(std::size_t part=0;part<s.parts;++part){const auto& c=x.collision.members[part];const auto& mesh=c.mesh;const auto& expected=s.collision[part];
     need(c.scene_node_index==expected[0]&&mesh.vertices.size()==expected[1]&&mesh.nodes.size()==expected[2]&&
       mesh.faces.size()==expected[3]&&mesh.vertex_normals.empty()&&mesh.reverse_mode==0,"GB actual ordered compound member mapping");
     x.radii[part].resize(mesh.nodes.size());std::vector<std::uint32_t> topology(mesh.nodes.size()+mesh.faces.size());
     need(g::prepareStaticTargetRadii49X(mesh,x.radii[part].data(),x.radii[part].size(),{topology.data(),topology.size()},x.depth[part])==g::Type2ResourceStatus49X::Complete,"GB actual compound radius/topology");
    }
    ++completed_;d.completedOwners=completed_;
   }
   need(retainedPayloadBytes()<=availableCpuPayloadBytes&&retainedPayloadBytes()<=maximumAdditionalPayloadBytes(),"GB actual source capacities exceeded reservation");
   ready_=true;d.message.clear();d.retainedPayloadBytes=retainedPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,e.what());}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const Source& source(std::size_t index)const{return source_.at(index);}
 const std::vector<std::uint8_t>& materialSource(std::size_t index)const{return materials_.at(index);}
};
}
