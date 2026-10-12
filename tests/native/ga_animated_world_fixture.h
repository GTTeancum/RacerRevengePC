#pragma once
#include "pod_materials50ed_fixture.h"
#include "watchtower_source_fixture.h"
#include "../../src/game/trigger_actions50in.h"
#include "original_legacy_motion50ig_fixture.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/rigid_vertices49s.h"
#include "../../src/game/compound_collision50ij.h"
#include "../../src/game/static_type2_producer49x.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <filesystem>
#include <string>
#include <vector>

namespace ga_animated_world {
namespace a=rr::assets;namespace g=rr::game;
using Index=g::SceneNodeIndex49P;
inline constexpr std::size_t ownerCount=2,vertexCount=605,indexCapacity=1815,surfaceCount=6;
inline constexpr std::size_t maxSourceNodes=3,maxRootInclusiveNodes=4,maxCollisionParts=3;
struct Span {std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0,material=0;};
struct Sequence {
 // Single original motion, one sequence and probability100. These are source
 // fields, not an independent animation timer or a guessed looping policy.
 bool active90=false,loop88=false;std::int32_t nextSequenceA0=-1,nextMotionA8=-1;
 std::uint32_t stopped10=1;float current34=0.f,elapsedB0=0.f;
};
struct PlaybackShared {
 std::uint8_t guard844C=0;std::uint32_t value433468=0;
};
// Existing service API name; the caller must borrow its common selector owner.
using ServiceShared=g::SharedRandomOwner50ID;
struct CollisionState {
 std::array<g::CompoundCollisionPart50IJ,maxCollisionParts> parts{};
 std::array<g::BodyMatrix48G,maxCollisionParts> nodeWorld{},previousNodeWorld{};
 g::BodyMatrix48G ownerWorld{},previousOwnerWorld{};
 g::Vector3_48C center{},half{},sphereCenter{};float radius=0.f;
};
struct State {std::array<g::SceneNode49P,maxRootInclusiveNodes> nodes{};Sequence sequence{};CollisionState collision{};};
struct Source {
 std::vector<std::uint8_t> psg,vut,mot,originalMot,mcf,col;
 a::CollisionCompoundMesh50IJ collision{};
 std::array<Index,maxCollisionParts> collisionNodes{};
 std::array<std::vector<float>,maxCollisionParts> radii;std::array<std::size_t,maxCollisionParts> depth{};
 a::PsgHierarchyView49O hierarchy{};a::MotionNodeBindings49O bindings{};
 a::MotionView49O motion{};a::Scene48D scene{};
 std::vector<g::RigidVertex49S> vertices;
 std::vector<std::array<double,3>> normals;
 std::vector<g::RigidPosition49S> projected;
 std::uint32_t startingZone=0,vertexStart=0,graphCount=0;
 std::size_t collisionCount=0;
 bool motionLoops=false;
};
struct Diagnostic {std::string message;std::size_t owner=0;g::ModelMotionDiagnostic49T motion{};};
namespace detail {
inline void need(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
inline g::BodyMatrix48G identity(){g::BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1.f;return m;}
inline float add(float a,float b){volatile float x=a+b;need(std::isfinite(x),"GA animation time addition");return x;}
inline float sub(float a,float b){volatile float x=a-b;need(std::isfinite(x),"GA animation time subtraction");return x;}
inline const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){for(const auto& b:p.blocks)if(b.name==name)return b;throw std::runtime_error("GA animation authored block absent");}
inline const std::string* optional(const a::ParameterBlock48F& b,const char* name){const std::string* found=nullptr;for(const auto& f:b.fields)if(f.name==name){need(!found,"GA animation duplicate consumed field");found=&f.value;}return found;}
inline const std::string& field(const a::ParameterBlock48F& b,const char* name){const auto* f=optional(b,name);need(f,"GA animation consumed field absent");return *f;}
inline float decimal(const std::string& s){double d=0;const auto r=std::from_chars(s.data(),s.data()+s.size(),d);need(r.ec==std::errc{}&&r.ptr==s.data()+s.size()&&std::isfinite(d),"GA animation decimal rejected");const float f=static_cast<float>(d);need(std::isfinite(f),"GA animation float range");return f;}
inline std::array<float,3> vector(const std::string& s){std::array<float,3> out{};std::size_t start=0;for(unsigned i=0;i<3;++i){const auto end=s.find(',',start);need((end==std::string::npos)==(i==2),"GA animation vector width");out[i]=decimal(s.substr(start,end==std::string::npos?end:end-start));start=end+1;}return out;}
inline void install(const Source& source,const std::array<float,3>& offset,State& state){
 for(auto& n:state.nodes){n={};n.localC0=n.world100=identity();}
 auto& root=state.nodes[0];root.sceneFlags190=0x98; // Fresh root80,2D9214 sets08/10.
 for(unsigned i=0;i<3;++i)root.localC0.v[12+i]=offset[i];
 for(std::uint32_t i=0;i<source.hierarchy.nodeCount;++i){a::PsgNode49O original{};
  need(a::readPsgNode49O(source.hierarchy,i,original),"GA original PSG hierarchy row rejected");
  const auto target=i+1,parent=original.parent<0?0u:static_cast<unsigned>(original.parent)+1;
  auto& n=state.nodes[target];std::memcpy(n.localC0.v,original.local,sizeof original.local);
  n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
  auto& first=state.nodes[parent].firstChild184;
  if(first==Index::none)first=static_cast<Index>(target);
  else {auto previous=static_cast<unsigned>(first);while(state.nodes[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(state.nodes[previous].nextSibling188);state.nodes[previous].nextSibling188=static_cast<Index>(target);}
 }
 need(g::validateSceneGraph49P({state.nodes.data(),source.graphCount}),"GA animation source graph rejected");
}
inline void lazy(PlaybackShared& shared){if(!shared.guard844C){shared.value433468=0xffffffffu;shared.guard844C=1;}}
// Closed101390 one-motion/one-sequence probability100 route. The source service
// draws shared RNG once outside this helper even though100 skips its compare.
inline bool step(const Source& source,Sequence& q,float delta){
 need(std::isfinite(delta)&&delta>=0.f,"GA animation invalid source delta");
 if(q.nextSequenceA0==-1){q.nextSequenceA0=0;
  if(!q.active90){q.nextSequenceA0=q.nextMotionA8=-1;}
  else {++q.nextMotionA8;
   if(q.nextMotionA8>=1){if(!q.loop88){q.active90=false;--q.nextMotionA8;q.nextSequenceA0=-1;}else q.nextMotionA8=0;}
  }
 }
 // Current/next pointer comparison1015EC is always equal for this authored
 // one-motion sequence: AC=0, so cross-motion blend/near-end restart is absent.
 if(q.stopped10&&q.active90){q.nextSequenceA0=-1;q.elapsedB0=0.f;q.stopped10=0;q.current34=0.f;}
 if(q.stopped10)return false;
 q.elapsedB0=add(q.elapsedB0,delta);q.current34=add(q.current34,delta);
 if(q.current34>source.motion.duration&&source.motionLoops)
  q.current34=sub(q.current34,source.motion.duration); // Source subtracts ONCE.
 return true;
}
}
class Owner final {
 std::array<Source,ownerCount> source_{};
 std::array<State,ownerCount> state_{},pending_{};
 a::Mesh48B mesh_{};std::vector<a::Vertex48B> nextVertices_;
 std::vector<a::WorldNormal50HK> normals_,nextNormals_;
 std::array<Span,surfaceCount> spans_{};
 g::ModelMotionWorkspace49T workspace_{};
 PlaybackShared playback_{};
 std::array<g::CompoundCollisionView50IJ,ownerCount> collisionViews_{};
 bool attempted_=false,ready_=false;
 bool updateCollision(std::size_t owner,State& state){
  auto& source=source_[owner];auto& c=state.collision;const g::SceneGraph49P graph{state.nodes.data(),source.graphCount};
  if(!g::refreshSceneWorld49P(graph,static_cast<Index>(0)))return false;
  c.previousOwnerWorld=c.ownerWorld;c.ownerWorld=state.nodes[0].world100;
  for(std::size_t i=0;i<source.collisionCount;++i){
   if(!g::refreshSceneWorld49P(graph,source.collisionNodes[i]))return false;
   c.previousNodeWorld[i]=c.nodeWorld[i];c.nodeWorld[i]=state.nodes[static_cast<unsigned>(source.collisionNodes[i])].world100;
   std::array<float,16> world{},composed{};std::memcpy(world.data(),c.nodeWorld[i].v,sizeof c.nodeWorld[i].v);
   if(!a::composeCollisionWorld48I(source.collision.members[i].mesh.local_matrix,world,composed))return false;
   auto& part=c.parts[i];part.mesh=&source.collision.members[i].mesh;
   std::memcpy(part.worldMatrix.v,composed.data(),sizeof part.worldMatrix.v);
   part.active=1;part.nodeRadii=source.radii[i].data();part.nodeRadiusCount=source.radii[i].size();part.maxDepth=source.depth[i];
  }
  if(!g::compoundBounds50IJ(c.parts.data(),source.collisionCount,c.ownerWorld,c.center,c.half))return false;
  g::CompoundCollisionView50IJ view{};view.parts=c.parts.data();view.count=source.collisionCount;view.ownerWorld=c.ownerWorld;view.center=c.center;view.half=c.half;
  return g::compoundSphere50IJ(view,c.sphereCenter,c.radius);
 }
 void publishCollision() noexcept {
  for(unsigned i=0;i<ownerCount;++i){auto& c=state_[i].collision;auto& v=collisionViews_[i];
   v.parts=c.parts.data();v.count=source_[i].collisionCount;v.ownerWorld=c.ownerWorld;v.center=c.center;v.half=c.half;
  }
 }
 bool project(std::size_t owner,State& state,std::vector<a::Vertex48B>& vertices,
      std::vector<a::WorldNormal50HK>& normals){
  auto& source=source_[owner];const g::SceneGraph49P graph{state.nodes.data(),source.graphCount};
  std::array<g::BodyMatrix48G,maxSourceNodes> palette{};
  for(std::uint32_t i=0;i<source.hierarchy.nodeCount;++i){if(!g::refreshSceneWorld49P(graph,static_cast<Index>(i+1)))return false;palette[i]=state.nodes[i+1].world100;}
  g::RigidVertexDiagnostic49S diagnostic{};
  if(!g::projectRigidVertices49S({source.vertices.data(),static_cast<std::uint32_t>(source.vertices.size())},
      {palette.data(),source.hierarchy.nodeCount},{source.projected.data(),static_cast<std::uint32_t>(source.projected.size())},diagnostic))return false;
  for(std::size_t i=0;i<source.vertices.size();++i){
   std::memcpy(vertices[source.vertexStart+i].position,source.projected[i].v,sizeof source.projected[i].v);
   const auto& m=palette[source.vertices[i].paletteIndex];std::array<double,16> matrix{};
   for(unsigned n=0;n<16;++n)matrix[n]=m.v[n];
   if(!a::transformWorldNormal50HK(source.normals[i],matrix,normals[source.vertexStart+i]))return false;
  }
  return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 bool load(const std::filesystem::path& archivePath,g::SourceLibmState49T& libm,
      g::ModelMotionSharedState49T& shared,Diagnostic& d){
  d={};if(attempted_){d.message="GA animated world requires a fresh stable owner";return false;}attempted_=true;
  try {using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=watchtower_source_fixture::archiveBytes;
   const auto bytes=input.read(archivePath,watchtower_source_fixture::archiveBytes);
   need(bytes.size()==watchtower_source_fixture::archiveBytes&&f::sha256(bytes)==watchtower_source_fixture::archiveSha,"GA animated original archive identity");
   a::ResArchive48C archive;a::DecodeError48C error{};a::DecodeLimits48C archiveLimits{};
   archiveLimits.resource_file_bytes=watchtower_source_fixture::archiveBytes;
   archiveLimits.resource_decoded_bytes=watchtower_source_fixture::decodedBytes;
   archiveLimits.resource_entries=watchtower_source_fixture::archiveEntries;
   need(a::decodeResource48C(bytes,archive,error,nullptr,archiveLimits),"GA animated archive decode");
   need(archive.entries.size()==watchtower_source_fixture::archiveEntries&&archive.decoded.size()==watchtower_source_fixture::decodedBytes,"GA animated archive extent");
   auto member=[&](const std::string& name,const char* sha){std::vector<std::uint8_t> out;need(a::extractResMember48C(archive,name,out,error),"GA animation member missing");need(f::sha256(out)==sha,"GA animation original member identity");return out;};
   auto tob=member("tracks\\ga.tob",watchtower_source_fixture::objects.sha);
   a::ParameterDocument48F params;need(a::decodeParameters48F(tob,params,error),"GA original TOB decode");
   need(field(block(params,"animations"),"numberofanimations")=="2","GA original animated owner count");
   constexpr const char* names[]={"Defensegate","opening"};
   constexpr const char* memberNames[]={"defensegate","opening"};
   constexpr const char* hashes[2][4]={
    {"e716ea1131091a4485be36bdc1e37bca3779bfe3e7408f851410a18bea473e1c","dd4dac1469c946650833ada2885cafa5fb60581ff1503f24efb390d6cf945a1b","548d03b3077f1e6f157a9a67c42a9aa6fa529980bc399b304b285830c1384b4b","693142685291f315ec08b23af578b9268fb372c936a41717594d75d2002722ee"},
    {"5070ad1c3d7435233ff14bf71aae91bc65fa65d9829d9cdeb5b58659756b0af6","7e2c6886fd6f7866967d325050ffef09775cd57abbdacd7e5ad67a08bdd728f9","2461e850de0f00381d73ed0997068160ea2ebf7448e2665b2e4384dc9108a71f","992ef9ca4e4beee1d72f6728c3a9ec2abb3e119bf6c73ae55b4d0ee7ff5c61b8"}};
   auto nextLibm=libm;auto nextShared=shared;
   mesh_.vertices.reserve(vertexCount);mesh_.indices.reserve(indexCapacity);normals_.resize(vertexCount);
   std::size_t spanIndex=0;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& source=source_[i];const std::string name=names[i],prefix="anim\\"+std::string(memberNames[i]);
    source.col=member(prefix+".col",i?"be46bd3c513cea4b1bf18900244f2861f8050a2d7ebe84f2ba3fc845c2c00ed5":"020be4035ec49e008c28583d053e140e047605993e7ee9ad77e350bd177792a8");
    std::string collisionError;need(a::loadCollisionCompoundMesh50IJ(source.col,source.collision,collisionError)&&source.collision.members.size()==(i?2u:3u),"GA original compound COL decode");
    source.collisionCount=source.collision.members.size();
    for(std::size_t part=0;part<source.collisionCount;++part){const auto& mesh=source.collision.members[part].mesh;source.radii[part].resize(mesh.nodes.size());std::vector<std::uint32_t> topology(mesh.nodes.size()+mesh.faces.size());
     need(g::prepareStaticTargetRadii49X(mesh,source.radii[part].data(),source.radii[part].size(),{topology.data(),topology.size()},source.depth[part])==g::Type2ResourceStatus49X::Complete,"GA compound cold radius/topology");}
    source.psg=member(prefix+".psg",hashes[i][0]);source.vut=member(prefix+".vut",hashes[i][1]);source.originalMot=member(prefix+".mot",hashes[i][2]);source.mcf=member(prefix+".mcf",hashes[i][3]);
    const auto& authored=block(params,"animation"+std::to_string(i+1));
    need(field(authored,"animationtype")=="KeyFramed"&&field(authored,"mcf")==name+".mcf"&&
        field(authored,"motiontoplay1")==name&&field(authored,"numberofmotions")=="1"&&
        field(authored,"numberinsequence")=="1"&&field(authored,"motionsequence1")=="1"&&
        field(authored,"useblendedkeys")=="T","GA authored single-motion scope");
    need(!optional(authored,"usecollision"),"GA original default UseCollision=true was overridden");
    need(!optional(authored,"lookvector")&&!optional(authored,"upvector")&&!optional(authored,"yaw")&&
        !optional(authored,"pitch")&&!optional(authored,"roll"),"GA animation nonzero orientation not joined");
    source.startingZone=i?7:11;source.motionLoops=false;source.vertexStart=static_cast<std::uint32_t>(mesh_.vertices.size());
    need(field(authored,"startingzone")==std::to_string(source.startingZone)&&field(authored,"loopsequence1")=="F"&&
        field(authored,"sequenceactive1")=="F"&&field(authored,"onetimeonly")=="T"&&field(authored,"uselighting")=="T","GA animation source activation differs");
    a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};
    need(a::readPsgHierarchy49O(source.psg.data(),source.psg.size(),source.hierarchy,geometry)&&source.hierarchy.nodeCount==(i?2u:3u),"GA animated PSG hierarchy");
    source.graphCount=source.hierarchy.nodeCount+1;
    need(a::readMotionNodeBindings49O(source.hierarchy,reinterpret_cast<const char*>(source.vut.data()),source.vut.size(),source.bindings,binding),"GA actual VUT bindings");
    for(std::uint32_t row=0;row<source.bindings.count;++row)++source.bindings.rows[row].sceneNode;
    if(!i){std::string why;need(original_legacy_motion50IG::convert(source.originalMot,nextLibm,source.mot,why),why.empty()?"GA legacy MOT conversion":why.c_str());}
    else source.mot=source.originalMot;
    need(a::readMot49O(source.mot.data(),source.mot.size(),source.motion,geometry),"GA actual runtime MOT view");
    // Binary MOT is the sampling clock, not MCF FrameCount/FPS. Keep both
    // original MCFs authenticated above; do not retime to 30/20 or 15/20.
    need(source.motion.frameCount==(i?15u:16u)&&source.motion.duration==(i?2.8f:1.f),"GA original MOT time extent");
    // Both authenticated PSGs select DiffuseCompressed with integer packet
    // positions. No general float V4_32 admission is introduced.
    a::SceneLimits48D limits{};limits.allow_signed32_quantized_positions=true;
    need(a::decodeScene48D(source.psg,source.scene,error,nullptr,limits),"GA animated PSG visual decoder");
    need(source.scene.nodes.size()==source.hierarchy.nodeCount&&source.scene.lods.size()==1&&!source.scene.world_space&&source.scene.identity_matrix_palette,"GA animated rigid palette scope");
    need(source.scene.lods[0].surfaces.size()==(i?4u:2u),"GA original animated surface count");
    for(const auto& surface:source.scene.lods[0].surfaces){need(spanIndex<spans_.size(),"GA animated surface ceiling");auto& span=spans_[spanIndex];
     span={static_cast<std::uint32_t>(mesh_.vertices.size()),static_cast<std::uint32_t>(surface.vertices.size()),
       static_cast<std::uint32_t>(mesh_.indices.size()),static_cast<std::uint32_t>(surface.indices.size()),static_cast<std::uint32_t>(spanIndex)};
     need(surface.material_index==(i?spanIndex-2:spanIndex),"GA source material surface order");
     for(const auto& original:surface.vertices){need(original.bone<source.hierarchy.nodeCount,"GA animated vertex palette bound");
      g::RigidVertex49S v{};for(unsigned n=0;n<3;++n)v.positionLocal[n]=original.position_local[n];v.paletteIndex=original.bone;source.vertices.push_back(v);source.normals.push_back(original.normal_local);
      a::Vertex48B shown{};for(unsigned n=0;n<2;++n)shown.uv[n]=static_cast<float>(original.uv[n]);
      // Existing explicit host diffuse compatibility: absent source RGB is
      // neutral255. The genuine texture/normal bank remains required.
      for(unsigned n=0;n<4;++n)shown.color[n]=surface.has_original_colors?original.color[n]:255;
      mesh_.vertices.push_back(shown);
     }
     for(const auto index:surface.indices){need(index<surface.vertices.size(),"GA animated local triangle index");mesh_.indices.push_back(span.vertexStart+index);}++spanIndex;
    }
    source.projected.resize(source.vertices.size());install(source,vector(field(authored,"offset")),state_[i]);
    state_[i].sequence.active90=false;state_[i].sequence.loop88=false;
    lazy(playback_);need(g::sampleModelMotionTime50IG(source.motion,source.bindings,0.f,{state_[i].nodes.data(),source.graphCount},nextShared,workspace_,d.motion),"GA original cold animation pose");
    need(project(i,state_[i],mesh_.vertices,normals_),"GA original animated world projection");
    need(g::bindCompoundSceneNodes50IJ(source.collision,{state_[i].nodes.data(),source.graphCount},static_cast<Index>(0),source.collisionNodes.data(),source.collisionCount),"GA original root-inclusive compound binding");
    for(std::size_t part=0;part<source.collisionCount;++part)
     need(source.collisionNodes[part]==static_cast<Index>(part+1),"GA authored root-inclusive compound node order differs");
    need(updateCollision(i,state_[i]),"GA original compound cold bounds");
   }
   need(mesh_.vertices.size()==vertexCount&&!mesh_.indices.empty()&&mesh_.indices.size()<=indexCapacity&&spanIndex==surfaceCount,"GA animated aggregate exact extent");
   nextVertices_=mesh_.vertices;nextNormals_=normals_;pending_=state_;
   for(unsigned axis=0;axis<3;++axis){mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=mesh_.vertices.front().position[axis];for(const auto& v:mesh_.vertices){mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],v.position[axis]);mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],v.position[axis]);}}
   publishCollision();libm=nextLibm;shared=nextShared;ready_=true;
   d={};return true;
  }catch(const std::exception& e){d.message=e.what();return false;}
 }
 // Caller supplies actual source elapsed, once at the world animation service
 // boundary. Cold source buffers and all outputs retain their backing storage.
 bool advance(float delta,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  d={};if(!ready_){d.message="GA animated world is not ready";return false;}
  try{pending_=state_;auto nextShared=shared;auto playback=playback_;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& source=source_[i];auto& state=pending_[i];auto& q=state.sequence;
    if(detail::step(source,q,delta)){
     detail::lazy(playback);
     detail::need(g::sampleModelMotionTime50IG(source.motion,source.bindings,q.current34,{state.nodes.data(),source.graphCount},nextShared,workspace_,d.motion),"GA source-time animation rejected");
     //154A00 stops only after sampling the overshoot frame. Rotation clamps;
     // original Bezier position tracks extrapolate on that same final call.
     if(!source.motionLoops&&q.current34>source.motion.duration){q.stopped10=1;q.current34=0.f;}
    }
    detail::need(project(i,state,nextVertices_,nextNormals_),"GA animated live projection rejected");
    detail::need(updateCollision(i,state),"GA animated compound live bounds rejected");
   }
   state_=pending_;shared=nextShared;playback_=playback;publishCollision();
   std::copy(nextVertices_.begin(),nextVertices_.end(),mesh_.vertices.begin());
   std::copy(nextNormals_.begin(),nextNormals_.end(),normals_.begin());return true;
  }catch(const std::exception& e){d.message=e.what();return false;}
 }
 //101860 world service: the existing application clock supplies the first
 //seed only. Zero timestamp deliberately reseeds on the following call. Every
 //enabled service advances the same C RNG once, even with probability100.
 bool service(float delta,std::uint32_t sourceTimestamp,std::uint32_t& sharedRaw,
      ServiceShared& lazy,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  // Retain the existing native failure staging; this ownership-only change
  // does not yet preserve original RNG mutation prefixes when advance fails.
  auto next=lazy;auto raw=sharedRaw;
  if(!next.seed.lazy80A8){next.seed.seed3D6B30=0;next.seed.lazy80A8=true;}
  if(!next.seed.seed3D6B30){next.seed.seed3D6B30=sourceTimestamp;raw=sourceTimestamp;}
  raw=raw*0x41c64e6du+0x3039u;
  volatile float converted=static_cast<float>(raw&0x7fffffffu);
  volatile float unit=converted*0x1p-31f;
  volatile float percent=101.f*unit;
  if(!next.range.lazy80AC){next.range.rangeCheck3D6B38=1;next.range.lazy80AC=true;}
  if(!std::isfinite(percent)||percent<0.f||percent>=101.f){d.message="GA animation source RNG range";return false;}
  if(!advance(delta,shared,d))return false;
  sharedRaw=raw;lazy=next;return true;
 }
 //101170 authored action callback, sharing101CD0 sequence stores. Called
 // only after the real authored trigger/action selector has admitted it.
 // This owner never makes a geometric hit, trigger timer, or substitute driver.
 bool activateSequence(std::size_t owner) noexcept {
  if(!ready_||owner>=ownerCount)return false;
  auto& q=state_[owner].sequence;
  g::triggerAnimation50IN(true,q.active90,q.nextSequenceA0,q.nextMotionA8);return true;
 }
 const a::Mesh48B& mesh()const noexcept{return mesh_;}
 const std::vector<a::WorldNormal50HK>& normals()const noexcept{return normals_;}
 const std::array<Span,surfaceCount>& spans()const noexcept{return spans_;}
 const Source& source(std::size_t i)const{return source_.at(i);}
 const State& state(std::size_t i)const{return state_.at(i);}
 const g::CompoundCollisionView50IJ& collision(std::size_t i)const{return collisionViews_.at(i);}
 bool ready()const noexcept{return ready_;}
};
} // namespace ga_animated_world
