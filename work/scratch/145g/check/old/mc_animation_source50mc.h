#pragma once
#include "mc_animation_authoring50mc.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/collision_mesh48i.h"
#include "../../src/game/type2_resource49x.h"
#include <cfenv>
#include <cstring>
#include <limits>

namespace mc_animation_source50MC {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t ownerCount=14,maximumPsgNodes=5,maximumSceneNodes=6,maximumParts=2;
inline constexpr std::size_t vertexCount=5806,indexCount=13182,surfaceCount=14,compoundPartCount=2;
struct Diagnostic {
 std::string message;std::size_t owner=0,completedOwners=0,retainedPayloadBytes=0,reservedPeakPayloadBytes=0;
 a::DecodeError48C decode{};a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};
};
struct Source {
 std::vector<std::uint8_t> mcf,psg,vut,originalMot,col;
 a::PsgHierarchyView49O hierarchy{};a::MotionNodeBindings49O bindings{};a::MotionView49O motion{};
 a::Scene48D scene{};a::CollisionCompoundMesh50IJ collision{};
 std::array<std::vector<float>,maximumParts> radii{};std::array<std::size_t,maximumParts> depth{};
 mc_animation_authoring50MC::Source authored{};std::size_t graphCount=0,collisionCount=0;
};
namespace detail {
using mc_scene_authoring50MC::detail::need;
struct Pin {const char* name;std::size_t bytes;const char* sha;};
struct Spec {std::array<Pin,4> members;std::uint32_t frames,durationBits;};
inline constexpr std::array<Spec,ownerCount> specs{{
 {{{{"anim\\zbreak1floor.mcf",193,"a5dc731292eb5a2d95e01de07d80f545893cef3ed655a810e8095389e90fd7b2"},{"anim\\zbreak1floor.psg",4176,"b737166e4d6e34b8550047723d0b736b5627e15a34c653fd793158bc6cbcf36c"},{"anim\\zbreak1.vut",36,"1a274a11bba7b2d645558204b8d6bedc85e8f60ab9577cb5622defed97b89075"},{"anim\\zbreak1floor.mot",2748,"744683ad07732fa102f363daf462fc1161a4e9823566b042a95093ad4d3a98da"}}},28,1076677836u},
 {{{{"anim\\ray15.mcf",173,"9270728541f6e8eb7744fa798fe1ff606d8a4a3d385ccfe48cf1cc8988733650"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray15.mot",12156,"66a394537c811b4906344cfa35920f038162e7446e43f141d0cffd398fe27459"}}},151,1092616192u},
 {{{{"anim\\ray2.mcf",170,"b680be78f2d4fcbc7ca74c57ad064eef457935860146e2d08b87534f989560e3"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray2.mot",4156,"fb7ce5c7d652aaa60554583a67519d2cbb56b3fa4b0fd74389cc50a62676842a"}}},51,1092616192u},
 {{{{"anim\\ray3.mcf",170,"475f3c82fb5a16c0121b413072b9db062943127138fa13a559843c7d3387ef00"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray3.mot",4156,"7eec6da850b1beccfdd4befc668aa843c461386b62c4b6d4893c80a0f4e6f5f8"}}},51,1092616192u},
 {{{{"anim\\ray4.mcf",170,"80022186fdcc415308e053debd9d199798722db2d594c5d57e793aa526d1b886"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray4.mot",4156,"88e66741717242d32ef73919fee104ab2e34c3301515b3c6f87657bd0a88ebe4"}}},51,1090868565u},
 {{{{"anim\\ray5.mcf",170,"10281f249c9b23232a0a91476b0a4996b03c1a95034032731cfeca886cd3fc57"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray5.mot",4156,"a4f49413d019ccd4c18a18ee3c8322a640e3e8e0e946899a64eabc0c7470ee68"}}},51,1092616192u},
 {{{{"anim\\ray6.mcf",169,"7e0071a9c247f0da3ec3faf404bd4c8860058def2b53dd831f48792995d37f15"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray6.mot",4156,"f9d5d9667e3e869f89bfa98a1eb1595bbaa844f1b7d87ef919a3802da7eefd34"}}},51,1090868565u},
 {{{{"anim\\ray7.mcf",170,"2f325e0567ba281a5485d1cdb76077b13efdc82a98a22a5bf3759cea596cf1bf"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray7.mot",12156,"d41e32afef5e322703e6cf4fe76e214d42c3e04ca11b2f8ce7bbd18b937bcbb6"}}},151,1092616192u},
 {{{{"anim\\ray8.mcf",170,"8c0a5287a8162c9e39145dbfd8095a4c0bd17ba5bf3f630eb11aef5e8dc0d797"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray8.mot",12156,"673b3732d39939f88250e18e0fbf45d56490ed73b0919ce683f2c21dd95344eb"}}},151,1093365175u},
 {{{{"anim\\ray10.mcf",171,"6ea419f423dfa45f0bc61e7faf6511f658cea3ce692b4a42fcf2cf21b6aa7517"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray10.mot",4156,"2141a0f9aa54bd460f8a5c6733f23f9bf4762b2fd278244e0ea8b114f847956c"}}},51,1088721481u},
 {{{{"anim\\ray11.mcf",171,"c611741b854e8008ee6fc1cc7ef59014736703108a97c9f42a43423c26a999cd"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray11.mot",4156,"8cb2e3338c29766b1498949aea7e95ab5dd0917de1c8ac4fffb69c4a676e841e"}}},51,1088721481u},
 {{{{"anim\\ray12.mcf",171,"c3bbc64f375498cf53687a261bd77cee63f46d1012798002627cce7ff9d86876"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray12.mot",4156,"1f21445a1d1e752bbc0ffe9c0e061eab2e6372473f34783a76009346a6fe160b"}}},51,1092616192u},
 {{{{"anim\\ray13.mcf",173,"fe313e6582b5c6572b59f376de004ede39cadcd3112eeef535595f7ce16de1ed"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray13.mot",12156,"b2bb51113308f46cec63e16dbeb5496cc9ad58bec55f2ed21847568581415c3b"}}},151,1090868565u},
 {{{{"anim\\ray14.mcf",172,"9950e3726674b2ab7f6114266f43c96a530f4b2f96ee64c87c9d2ec84b704685"},{"anim\\ray.psg",8948,"8fc3db8611ea994b5561a94a03022c5177fe25cf43f472250bb2d72d09b4b29a"},{"anim\\ray.vut",87,"0a7c97f97ae72f52626ad92f70085f162879cdfd021df969622c954c123d264b"},{"anim\\ray14.mot",4156,"ffe200b57b04957be804d5372e1a43f40dfaa524b2ce733ec775ae17198bc769"}}},51,1095237632u},
}};
inline constexpr Pin collisionPin={"anim\\zbreak1floor.col",6148,"569160eadb079c30448b6aa67acfe23e7dae6cfc13487d5d1b35d7494368aba0"};
inline constexpr std::array<Pin,2> materialPins{{{"textures\\mc_coral11.psm",264,"aa9aafc9487487f813d90a3fb3ab166bf06c3039dc664eb2954b814530b3afbe"},{"textures\\mb_ray.psm",2621,"53796ed56017a30e569de209fb6faea3058d70d2344cb787ac69d51e5c60bc5a"}}};
inline void add(std::size_t& n,std::size_t x)noexcept{n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;}
template<class T>inline void payload(std::size_t& n,const std::vector<T>& v)noexcept{
 if(v.capacity()>std::numeric_limits<std::size_t>::max()/sizeof(T))n=std::numeric_limits<std::size_t>::max();else add(n,v.capacity()*sizeof(T));}
inline void text(std::size_t& n,const std::string& s)noexcept{add(n,s.capacity()+1);}
}
// Cold resources and native decoded representations, never a live graph or
// renderer/query issuer. All14 actual counted sources have their OWN storage,
// even the13 ray owners that share original PSG/VUT member identities.
class Owner final { public: // host check: retired owner opened for the pose comparison
 std::shared_ptr<const mc_cold_sources50LA::Owner> cold_;
 mc_animation_authoring50MC::Owner authoring_;
 std::array<Source,ownerCount> source_{};std::array<std::vector<std::uint8_t>,2> materials_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;std::size_t completed_=0;
 bool fail(Diagnostic& d,const char* message){d.message=message;d.completedOwners=completed_;d.retainedPayloadBytes=retainedPayloadBytes();return false;}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept{
  std::size_t raw=detail::collisionPin.bytes;for(const auto& s:detail::specs)for(const auto& p:s.members)raw+=p.bytes;
  for(const auto& p:detail::materialPins)raw+=p.bytes;
  constexpr std::size_t visual=67*sizeof(a::SceneNode48D)+14*sizeof(a::SceneLod48D)+14*sizeof(a::SceneSurface48D)+
   vertexCount*(sizeof(a::SceneVertex48D)+3*sizeof(std::uint32_t)+sizeof(a::SceneStrip48D)+sizeof(a::SceneContinuation48D)+24)+indexCount*4;
  constexpr std::size_t collision=76*2*sizeof(std::array<float,3>)+76*(sizeof(a::CollisionNode48G)+sizeof(a::CollisionMeshNode48I)+sizeof(float))+
   78*(sizeof(a::CollisionFace48G)+sizeof(a::CollisionMeshFace48I))+2*(sizeof(a::CollisionCompoundMember50IJ)+sizeof(a::CollisionCompoundMeshMember50IJ))+154*4;
  return sizeof(Owner)+mc_animation_authoring50MC::Owner::maximumAdditionalPayloadBytes()+4*(raw+visual+collision)+65536;
 }
 std::size_t retainedPayloadBytes()const noexcept{
  if(thread_!=std::this_thread::get_id())return 0;using namespace detail;std::size_t n=sizeof(Owner);
  for(const auto& v:materials_)payload(n,v);
  for(const auto& x:source_){for(const auto* v:{&x.mcf,&x.psg,&x.vut,&x.originalMot,&x.col})payload(n,*v);
   payload(n,x.scene.nodes);for(const auto& r:x.scene.nodes)text(n,r.name);payload(n,x.scene.materials);for(const auto& r:x.scene.materials)text(n,r);
   payload(n,x.scene.lods);for(const auto& l:x.scene.lods){payload(n,l.surfaces);for(const auto& s:l.surfaces){payload(n,s.vertices);payload(n,s.indices);payload(n,s.strips);payload(n,s.continuations);}}
   payload(n,x.scene.neutral_lod0);payload(n,x.scene.spans_lod0);payload(n,x.collision.members);
   for(const auto& p:x.collision.members){payload(n,p.mesh.vertices);payload(n,p.mesh.vertex_normals);payload(n,p.mesh.nodes);payload(n,p.mesh.faces);}
   for(const auto& r:x.radii)payload(n,r);
  }return n;
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,Diagnostic& d){
  d={};if(thread_!=std::this_thread::get_id())return fail(d,"MC animation source thread mismatch");
  if(attempted_)return fail(d,"MC animation source requires fresh stable owner");attempted_=true;
  d.reservedPeakPayloadBytes=maximumAdditionalPayloadBytes();
  if(available>32u*1024u*1024u||available<d.reservedPeakPayloadBytes)return fail(d,"MC animation source exceeds actual remaining budget");
  try{using namespace detail;need(std::fegetround()==FE_TONEAREST,"MC animation source nearest arithmetic required");
   need(cold&&cold->ready(),"MC actual cold source required");
   std::string why;need(authoring_.decode(cold,authoring_.maximumAdditionalPayloadBytes(),why),why.c_str());cold_=std::move(cold);
   const auto member=[&](const Pin& pin,std::vector<std::uint8_t>& out){
    need(cold_->extractMember(pin.name,out,d.decode,pin.bytes)&&out.size()==pin.bytes&&
     pod_materials50ed_fixture::detail::sha256(out)==pin.sha,"MC exact original animation resource");};
   for(std::size_t i=0;i<materials_.size();++i)member(materialPins[i],materials_[i]);
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& x=source_[i];const auto& spec=specs[i];const bool bridge=i==0;
    x.authored=authoring_.source(i);member(spec.members[0],x.mcf);member(spec.members[1],x.psg);member(spec.members[2],x.vut);member(spec.members[3],x.originalMot);
    const std::size_t nodes=bridge?2:5;
    need(a::readPsgHierarchy49O(x.psg.data(),x.psg.size(),x.hierarchy,d.geometry)&&x.hierarchy.nodeCount==nodes,"MC original PSG hierarchy");
    need(a::readMotionNodeBindings49O(x.hierarchy,reinterpret_cast<const char*>(x.vut.data()),x.vut.size(),x.bindings,d.binding)&&x.bindings.count==nodes,"MC original VUT bindings");
    constexpr const char* bridgeNames[2]={"ZBREAK1","ZBREAK2"};constexpr const char* rayNames[5]={"BODY","L_WING","R_WING","L_WING_TIP","R_WING_TIP"};
    for(std::size_t row=0;row<nodes;++row){auto& b=x.bindings.rows[row];need(b.sceneNode==row&&std::strcmp(b.name,bridge?bridgeNames[row]:rayNames[row])==0&&
      b.listedFlag3c&&b.translationFlag38==(bridge||row==0),"MC exact original binding enumeration");++b.sceneNode;}
    x.graphCount=nodes+1; // Separate authentic counted model root at0.
    a::SceneLimits48D limits{};limits.input_bytes=x.psg.size();limits.vertices_all_lods=bridge?203:431;
    // Both actual PSGs use the existing strict signed16 position route.
    need(a::decodeScene48D(x.psg,x.scene,d.decode,nullptr,limits)&&x.scene.nodes.size()==nodes&&x.scene.lods.size()==1&&
      !x.scene.world_space&&x.scene.identity_matrix_palette&&x.scene.materials.size()==1&&x.scene.lods[0].surfaces.size()==1,"MC exact original animated scene");
    const auto& surface=x.scene.lods[0].surfaces[0];need(x.scene.materials[0]==(bridge?"mc_coral11":"mb_ray")&&surface.material_index==0&&
     surface.component_word==0x107&&!surface.has_original_colors&&surface.vertices.size()==(bridge?203:431)&&surface.indices.size()==(bridge?429:981),"MC actual animation surface");
    std::array<std::size_t,5> bones{};for(const auto& v:surface.vertices){need(v.bone<nodes,"MC rigid bone index");++bones[v.bone];}
    const std::array<std::size_t,5> expected=bridge?std::array<std::size_t,5>{100,103,0,0,0}:std::array<std::size_t,5>{287,32,33,44,35};
    need(bones==expected,"MC exact palette population");
    // Every original MC motion is version2. No legacy conversion, copied
    // synthetic MOT serialization or SourceLibm mutation is performed.
    need(a::readMot49O(x.originalMot.data(),x.originalMot.size(),x.motion,d.geometry)&&x.motion.frameCount==spec.frames&&
      x.motion.positionCount==(bridge?2u:1u)&&x.motion.rotationCount==(bridge?2u:5u)&&x.motion.rotationType==0,"MC original version2 MOT");
    std::uint32_t duration=0;std::memcpy(&duration,&x.motion.duration,4);need(duration==spec.durationBits,"MC original binary duration, not MCF retiming");
    need(x.motion.totalKeys==(x.motion.positionCount+x.motion.rotationCount)*spec.frames,"MC original total key count");
    for(std::uint32_t row=0;row<x.motion.positionCount;++row)need(x.motion.position[row].rowIndex==row&&x.motion.position[row].keyCount==spec.frames,"MC position track order");
    for(std::uint32_t row=0;row<x.motion.rotationCount;++row)need(x.motion.rotation[row].rowIndex==row&&x.motion.rotation[row].keyCount==spec.frames,"MC rotation track order");
    if(bridge){member(collisionPin,x.col);a::CollisionCompoundLimits50IJ cl{};cl.members=2;cl.tree.input_bytes=x.col.size();
     need(a::loadCollisionCompoundMesh50IJ(x.col,x.collision,why,cl),why.c_str());need(x.collision.reverse_mode==0&&x.collision.members.size()==2,"MC bridge compound count");x.collisionCount=2;
     for(std::size_t part=0;part<2;++part){const auto& c=x.collision.members[part];const auto& mesh=c.mesh;
      need(c.scene_node_index==part+1&&mesh.vertices.size()==38&&mesh.nodes.size()==(part?37:39)&&mesh.faces.size()==(part?38:40)&&
       mesh.vertex_normals.size()==38&&mesh.reverse_mode==0,"MC actual compound members retain normals");
      x.radii[part].resize(mesh.nodes.size());std::vector<std::uint32_t> topology(mesh.nodes.size()+mesh.faces.size());
      need(g::prepareStaticTargetRadii49X(mesh,x.radii[part].data(),x.radii[part].size(),{topology.data(),topology.size()},x.depth[part])==g::Type2ResourceStatus49X::Complete,"MC actual compound topology/radii");}
    }else need(!x.authored.useCollision&&x.col.empty()&&x.collision.members.empty(),"MC rays cannot invent collision resources");
    ++completed_;d.completedOwners=completed_;
   }
   need(retainedPayloadBytes()<=available&&retainedPayloadBytes()<=maximumAdditionalPayloadBytes(),"MC actual source payload reservation");
   ready_=true;d.message.clear();d.retainedPayloadBytes=retainedPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,e.what());}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready()&&authoring_.ready();}
 const mc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("MC animation resources unavailable");return source_.at(i);}
 const std::vector<std::uint8_t>& materialSource(std::size_t i)const{if(!ready())throw std::runtime_error("MC animation materials unavailable");return materials_.at(i);}
};
}
