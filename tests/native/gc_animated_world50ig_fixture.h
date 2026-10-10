#pragma once
#include "pod_materials50ed_fixture.h"
#include "original_legacy_motion50ig_fixture.h"
#include "../../src/assets/import_scene48d.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/model_bindings49o.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/rigid_vertices49s.h"
#include "../../src/game/compound_collision50ij.h"
#include "../../src/game/static_type2_producer49x.h"
#include <algorithm>
#include <array>
#include <charconv>
#include <filesystem>
#include <string>
#include <vector>

namespace gc_animated_world50IG {
namespace a=rr::assets;namespace g=rr::game;
using Index=g::SceneNodeIndex49P;
inline constexpr std::size_t ownerCount=2,vertexCount=1195,indexCount=2337,surfaceCount=6;
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
struct ServiceShared {std::uint8_t guard80A8=0,guard80AC=0;std::uint32_t seed3D6B30=0,rangeCheck3D6B38=0;};
struct CollisionState {
 std::array<g::CompoundCollisionPart50IJ,2> parts{};
 std::array<g::BodyMatrix48G,2> nodeWorld{},previousNodeWorld{};
 g::BodyMatrix48G ownerWorld{},previousOwnerWorld{};
 g::Vector3_48C center{},half{},sphereCenter{};float radius=0.f;
};
struct State {std::array<g::SceneNode49P,3> nodes{};Sequence sequence{};CollisionState collision{};};
struct Source {
 std::vector<std::uint8_t> psg,vut,mot,originalMot,mcf,col;
 a::CollisionCompoundMesh50IJ collision{};
 std::array<Index,2> collisionNodes{};
 std::array<std::vector<float>,2> radii;std::array<std::size_t,2> depth{};
 a::PsgHierarchyView49O hierarchy{};a::MotionNodeBindings49O bindings{};
 a::MotionView49O motion{};a::Scene48D scene{};
 std::vector<g::RigidVertex49S> vertices;
 std::vector<std::array<double,3>> normals;
 std::vector<g::RigidPosition49S> projected;
 std::uint32_t startingZone=0,vertexStart=0;
 bool motionLoops=false;
};
struct GateSource {
 g::CollisionSphere48J sphere{};
 std::uint32_t startingZone=0,animationOwner=0;
 std::string soundFilename;
 std::array<float,3> soundPosition{};
 float soundMinDistance=0,soundMaxDistance=0;
 bool authenticated=false;
};
struct Diagnostic {std::string message;std::size_t owner=0;g::ModelMotionDiagnostic49T motion{};};
namespace detail {
inline void need(bool ok,const char* text){if(!ok)throw std::runtime_error(text);}
inline g::BodyMatrix48G identity(){g::BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1.f;return m;}
inline float add(float a,float b){volatile float x=a+b;need(std::isfinite(x),"GC animation time addition");return x;}
inline float sub(float a,float b){volatile float x=a-b;need(std::isfinite(x),"GC animation time subtraction");return x;}
inline const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){for(const auto& b:p.blocks)if(b.name==name)return b;throw std::runtime_error("GC animation authored block absent");}
inline const std::string* optional(const a::ParameterBlock48F& b,const char* name){const std::string* found=nullptr;for(const auto& f:b.fields)if(f.name==name){need(!found,"GC animation duplicate consumed field");found=&f.value;}return found;}
inline const std::string& field(const a::ParameterBlock48F& b,const char* name){const auto* f=optional(b,name);need(f,"GC animation consumed field absent");return *f;}
inline float decimal(const std::string& s){double d=0;const auto r=std::from_chars(s.data(),s.data()+s.size(),d);need(r.ec==std::errc{}&&r.ptr==s.data()+s.size()&&std::isfinite(d),"GC animation decimal rejected");const float f=static_cast<float>(d);need(std::isfinite(f),"GC animation float range");return f;}
inline std::array<float,3> vector(const std::string& s){std::array<float,3> out{};std::size_t start=0;for(unsigned i=0;i<3;++i){const auto end=s.find(',',start);need((end==std::string::npos)==(i==2),"GC animation vector width");out[i]=decimal(s.substr(start,end==std::string::npos?end:end-start));start=end+1;}return out;}
inline void install(const Source& source,const std::array<float,3>& offset,State& state){
 for(auto& n:state.nodes){n={};n.localC0=n.world100=identity();}
 auto& root=state.nodes[0];root.sceneFlags190=0x98; // Fresh root80,2D9214 sets08/10.
 for(unsigned i=0;i<3;++i)root.localC0.v[12+i]=offset[i];
 for(std::uint32_t i=0;i<source.hierarchy.nodeCount;++i){a::PsgNode49O original{};
  need(a::readPsgNode49O(source.hierarchy,i,original),"GC original PSG hierarchy row rejected");
  const auto target=i+1,parent=original.parent<0?0u:static_cast<unsigned>(original.parent)+1;
  auto& n=state.nodes[target];std::memcpy(n.localC0.v,original.local,sizeof original.local);
  n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
  auto& first=state.nodes[parent].firstChild184;
  if(first==Index::none)first=static_cast<Index>(target);
  else {auto previous=static_cast<unsigned>(first);while(state.nodes[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(state.nodes[previous].nextSibling188);state.nodes[previous].nextSibling188=static_cast<Index>(target);}
 }
 need(g::validateSceneGraph49P({state.nodes.data(),3}),"GC animation source graph rejected");
}
inline void lazy(PlaybackShared& shared){if(!shared.guard844C){shared.value433468=0xffffffffu;shared.guard844C=1;}}
// Closed101390 one-motion/one-sequence probability100 route. The source service
// draws shared RNG once outside this helper even though100 skips its compare.
inline bool step(const Source& source,Sequence& q,float delta){
 need(std::isfinite(delta)&&delta>=0.f,"GC animation invalid source delta");
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
 GateSource gateSource_{};g::GateTriggerBinding50KG gate_{};
 std::array<g::CompoundCollisionView50IJ,ownerCount> collisionViews_{};
 bool attempted_=false,ready_=false;
 bool updateCollision(std::size_t owner,State& state){
  auto& source=source_[owner];auto& c=state.collision;const g::SceneGraph49P graph{state.nodes.data(),3};
  if(!g::refreshSceneWorld49P(graph,static_cast<Index>(0)))return false;
  c.previousOwnerWorld=c.ownerWorld;c.ownerWorld=state.nodes[0].world100;
  for(unsigned i=0;i<2;++i){
   if(!g::refreshSceneWorld49P(graph,source.collisionNodes[i]))return false;
   c.previousNodeWorld[i]=c.nodeWorld[i];c.nodeWorld[i]=state.nodes[static_cast<unsigned>(source.collisionNodes[i])].world100;
   std::array<float,16> world{},composed{};std::memcpy(world.data(),c.nodeWorld[i].v,sizeof c.nodeWorld[i].v);
   if(!a::composeCollisionWorld48I(source.collision.members[i].mesh.local_matrix,world,composed))return false;
   auto& part=c.parts[i];part.mesh=&source.collision.members[i].mesh;
   std::memcpy(part.worldMatrix.v,composed.data(),sizeof part.worldMatrix.v);
   part.active=1;part.nodeRadii=source.radii[i].data();part.nodeRadiusCount=source.radii[i].size();part.maxDepth=source.depth[i];
  }
  if(!g::compoundBounds50IJ(c.parts.data(),2,c.ownerWorld,c.center,c.half))return false;
  g::CompoundCollisionView50IJ view{};view.parts=c.parts.data();view.count=2;view.ownerWorld=c.ownerWorld;view.center=c.center;view.half=c.half;
  return g::compoundSphere50IJ(view,c.sphereCenter,c.radius);
 }
 void publishCollision() noexcept {
  for(unsigned i=0;i<ownerCount;++i){auto& c=state_[i].collision;auto& v=collisionViews_[i];
   v.parts=c.parts.data();v.count=2;v.ownerWorld=c.ownerWorld;v.center=c.center;v.half=c.half;
  }
 }
 bool project(std::size_t owner,State& state,std::vector<a::Vertex48B>& vertices,
      std::vector<a::WorldNormal50HK>& normals){
  auto& source=source_[owner];const g::SceneGraph49P graph{state.nodes.data(),3};
  std::array<g::BodyMatrix48G,2> palette{};
  for(unsigned i=0;i<2;++i){if(!g::refreshSceneWorld49P(graph,static_cast<Index>(i+1)))return false;palette[i]=state.nodes[i+1].world100;}
  g::RigidVertexDiagnostic49S diagnostic{};
  if(!g::projectRigidVertices49S({source.vertices.data(),static_cast<std::uint32_t>(source.vertices.size())},
      {palette.data(),2},{source.projected.data(),static_cast<std::uint32_t>(source.projected.size())},diagnostic))return false;
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
  d={};if(attempted_){d.message="GC animated world requires a fresh stable owner";return false;}attempted_=true;
  try {using namespace detail;namespace f=pod_materials50ed_fixture::detail;
   f::Inputs input;input.capacity=4205067;const auto bytes=input.read(archivePath,4205067);
   need(bytes.size()==4205067&&f::sha256(bytes)=="69dcbd97f010e462895f8712056e0ac9a21fe56dbae6d6b6c090c97c5e7a4cc9","GC animated original archive identity");
   a::ResArchive48C archive;a::DecodeError48C error{};need(a::decodeResource48C(bytes,archive,error),"GC animated archive decode");
   auto member=[&](const std::string& name,const char* sha){std::vector<std::uint8_t> out;need(a::extractResMember48C(archive,name,out,error),"GC animation member missing");need(f::sha256(out)==sha,"GC animation original member identity");return out;};
   auto tob=member("tracks\\gc.tob","e677f80d12da2c2225c8ba030e2a29e3a358c52bcfd4f9c981b79c3c53b72454");
   a::ParameterDocument48F params;need(a::decodeParameters48F(tob,params,error),"GC original TOB decode");
   need(field(block(params,"animations"),"numberofanimations")=="2","GC original animated owner count");
   // Read Trigger9 from this same SHA-authenticated original TOB, not a host
   // threshold, invented waypoint, or replacement animation configuration.
   need(field(block(params,"triggers"),"numberoftriggers")=="9","GC authored trigger count");
   const auto& gate=block(params,"trigger9");
   need(field(gate,"name")=="Gate"&&field(gate,"zone")=="11"&&
        field(gate,"numberofactions")=="2"&&field(gate,"onetimeonly")=="T",
        "GC authored Gate identity/one-shot differs");
   need(!optional(gate,"startenabled")&&!optional(gate,"ignoretrigger")&&
        !optional(gate,"randomtriggerpercent"),"GC Gate constructor defaults differ");
   const auto gatePosition=vector(field(gate,"position"));
   for(unsigned axis=0;axis<3;++axis)gateSource_.sphere.center.v[axis]=gatePosition[axis];
   gateSource_.sphere.radius=decimal(field(gate,"radius"));
   need(gateSource_.sphere.radius==300.f,"GC authored Gate radius differs");
   gateSource_.startingZone=11;gateSource_.animationOwner=0;
   const auto& animationAction=block(params,"trigger9action1");
   need(field(animationAction,"actiontype")=="Animation"&&
        field(animationAction,"animationnumber")=="1"&&
        !optional(animationAction,"timetillreset")&&!optional(animationAction,"specificobject")&&
        !optional(animationAction,"animationnameref"),"GC authored Gate animation action differs");
   // Source parser ignores Animation action's OneTimeOnly field. The Gate's
   // own OneTimeOnly above supplies the actual one-shot policy.
   const auto& soundAction=block(params,"trigger9action2");
   need(field(soundAction,"actiontype")=="PodSound","GC authored Gate second action differs");
   gateSource_.soundFilename=field(soundAction,"filename");
   need(gateSource_.soundFilename=="sfx\\race\\bigdoor.vag","GC original Gate sound differs");
   gateSource_.soundPosition=vector(field(soundAction,"position"));
   gateSource_.soundMinDistance=decimal(field(soundAction,"mindistance"));
   gateSource_.soundMaxDistance=decimal(field(soundAction,"maxdistance"));
   need(gateSource_.soundMinDistance==0.f&&gateSource_.soundMaxDistance==2500.f,
        "GC original Gate sound range differs");
   gateSource_.authenticated=true;
   constexpr const char* names[]={"opening","windmill"};
   constexpr const char* hashes[2][4]={
    {"5070ad1c3d7435233ff14bf71aae91bc65fa65d9829d9cdeb5b58659756b0af6","7e2c6886fd6f7866967d325050ffef09775cd57abbdacd7e5ad67a08bdd728f9","2461e850de0f00381d73ed0997068160ea2ebf7448e2665b2e4384dc9108a71f","992ef9ca4e4beee1d72f6728c3a9ec2abb3e119bf6c73ae55b4d0ee7ff5c61b8"},
    {"8090ff54d142d6090036f4b930e7158a5a7bc8e07f7959f464b7720186662fc8","95e0e2874a2c48c39ef284ef22378cff891af9a55fab3c1058578fca9f68b5ee","8dc3f486548ab0598ee4c56798fcbbc24f6bc9c4c8c61afa33b17d7ea159d29d","087f5bd9d8bad4da6386b100e4e8a6e08f5ac2aa7ecf0f27e29e1acea0b0cb8d"}};
   auto nextLibm=libm;auto nextShared=shared;
   mesh_.vertices.reserve(vertexCount);mesh_.indices.reserve(indexCount);normals_.resize(vertexCount);
   std::size_t spanIndex=0;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& source=source_[i];const std::string name=names[i],prefix="anim\\"+name;
    source.col=member(prefix+".col",i?"94575aa9f6cf7b434e049ad706208e53c6ad7dec4f135c3494333d62f18f8e77":"be46bd3c513cea4b1bf18900244f2861f8050a2d7ebe84f2ba3fc845c2c00ed5");
    std::string collisionError;need(a::loadCollisionCompoundMesh50IJ(source.col,source.collision,collisionError)&&source.collision.members.size()==2,"GC original compound COL decode");
    for(unsigned part=0;part<2;++part){const auto& mesh=source.collision.members[part].mesh;source.radii[part].resize(mesh.nodes.size());std::vector<std::uint32_t> topology(mesh.nodes.size()+mesh.faces.size());
     need(g::prepareStaticTargetRadii49X(mesh,source.radii[part].data(),source.radii[part].size(),{topology.data(),topology.size()},source.depth[part])==g::Type2ResourceStatus49X::Complete,"GC compound cold radius/topology");}
    source.psg=member(prefix+".psg",hashes[i][0]);source.vut=member(prefix+".vut",hashes[i][1]);source.originalMot=member(prefix+".mot",hashes[i][2]);source.mcf=member(prefix+".mcf",hashes[i][3]);
    const auto& authored=block(params,"animation"+std::to_string(i+1));
    need(field(authored,"animationtype")=="KeyFramed"&&field(authored,"mcf")==name+".mcf"&&
        field(authored,"motiontoplay1")==name&&field(authored,"numberofmotions")=="1"&&
        field(authored,"numberinsequence")=="1"&&field(authored,"motionsequence1")=="1"&&
        field(authored,"useblendedkeys")=="T","GC authored single-motion scope");
    need(!optional(authored,"usecollision"),"GC original default UseCollision=true was overridden");
    need(!optional(authored,"lookvector")&&!optional(authored,"upvector")&&!optional(authored,"yaw")&&
        !optional(authored,"pitch")&&!optional(authored,"roll"),"GC animation nonzero orientation not joined");
    source.startingZone=i?2:11;source.motionLoops=i==1;source.vertexStart=static_cast<std::uint32_t>(mesh_.vertices.size());
    need(field(authored,"startingzone")==std::to_string(source.startingZone)&&field(authored,"loopsequence1")== (i?"T":"F")&&
        field(authored,"sequenceactive1")==(i?"T":"F")&&field(authored,"onetimeonly")==(i?"F":"T"),"GC animation source activation differs");
    a::GeometryDiagnostic49O geometry{};a::MotionBindingDiagnostic49O binding{};
    need(a::readPsgHierarchy49O(source.psg.data(),source.psg.size(),source.hierarchy,geometry)&&source.hierarchy.nodeCount==2,"GC animated PSG hierarchy");
    need(a::readMotionNodeBindings49O(source.hierarchy,reinterpret_cast<const char*>(source.vut.data()),source.vut.size(),source.bindings,binding),"GC actual VUT bindings");
    for(std::uint32_t row=0;row<source.bindings.count;++row)++source.bindings.rows[row].sceneNode;
    if(i){std::string why;need(original_legacy_motion50IG::convert(source.originalMot,nextLibm,source.mot,why),why.empty()?"GC legacy MOT conversion":why.c_str());}
    else source.mot=source.originalMot;
    need(a::readMot49O(source.mot.data(),source.mot.size(),source.motion,geometry),"GC actual runtime MOT view");
    a::SceneLimits48D limits{};limits.allow_signed32_quantized_positions=i==0;
    need(a::decodeScene48D(source.psg,source.scene,error,nullptr,limits),"GC animated PSG visual decoder");
    need(source.scene.nodes.size()==2&&source.scene.lods.size()==1&&!source.scene.world_space&&source.scene.identity_matrix_palette,"GC animated rigid palette scope");
    need(source.scene.lods[0].surfaces.size()==(i?2u:4u),"GC original animated surface count");
    for(const auto& surface:source.scene.lods[0].surfaces){need(spanIndex<spans_.size(),"GC animated surface ceiling");auto& span=spans_[spanIndex];
     span={static_cast<std::uint32_t>(mesh_.vertices.size()),static_cast<std::uint32_t>(surface.vertices.size()),
       static_cast<std::uint32_t>(mesh_.indices.size()),static_cast<std::uint32_t>(surface.indices.size()),static_cast<std::uint32_t>(spanIndex)};
     need(surface.material_index==(i?spanIndex-4:spanIndex),"GC source material surface order");
     for(const auto& original:surface.vertices){need(original.bone<2,"GC animated vertex palette bound");
      g::RigidVertex49S v{};for(unsigned n=0;n<3;++n)v.positionLocal[n]=original.position_local[n];v.paletteIndex=original.bone;source.vertices.push_back(v);source.normals.push_back(original.normal_local);
      a::Vertex48B shown{};for(unsigned n=0;n<2;++n)shown.uv[n]=static_cast<float>(original.uv[n]);
      // Existing explicit host diffuse compatibility: absent source RGB is
      // neutral255. The genuine texture/normal bank remains required.
      for(unsigned n=0;n<4;++n)shown.color[n]=surface.has_original_colors?original.color[n]:255;
      mesh_.vertices.push_back(shown);
     }
     for(const auto index:surface.indices){need(index<surface.vertices.size(),"GC animated local triangle index");mesh_.indices.push_back(span.vertexStart+index);}++spanIndex;
    }
    source.projected.resize(source.vertices.size());install(source,vector(field(authored,"offset")),state_[i]);
    state_[i].sequence.active90=i==1;state_[i].sequence.loop88=i==1;
    lazy(playback_);need(g::sampleModelMotionTime50IG(source.motion,source.bindings,0.f,{state_[i].nodes.data(),3},nextShared,workspace_,d.motion),"GC original cold animation pose");
    need(project(i,state_[i],mesh_.vertices,normals_),"GC original animated world projection");
    need(g::bindCompoundSceneNodes50IJ(source.collision,{state_[i].nodes.data(),3},static_cast<Index>(0),source.collisionNodes.data(),2),"GC original root-inclusive compound binding");
    need(source.collisionNodes[0]==static_cast<Index>(i?0:1)&&source.collisionNodes[1]==static_cast<Index>(i?1:2),"GC authored compound node order differs");
    need(updateCollision(i,state_[i]),"GC original compound cold bounds");
   }
   need(mesh_.vertices.size()==vertexCount&&mesh_.indices.size()==indexCount&&spanIndex==surfaceCount,"GC animated aggregate exact extent");
   nextVertices_=mesh_.vertices;nextNormals_=normals_;pending_=state_;
   for(unsigned axis=0;axis<3;++axis){mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=mesh_.vertices.front().position[axis];for(const auto& v:mesh_.vertices){mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],v.position[axis]);mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],v.position[axis]);}}
   publishCollision();libm=nextLibm;shared=nextShared;ready_=true;
   gate_.callbacks.context=this;
   gate_.callbacks.activateAnimation=[](void* context,std::size_t owner) noexcept {
    return static_cast<Owner*>(context)->activateSequence(owner);
   };
   // PodSound has no bound native owner here. The one-shot receipt explicitly
   // records its omission rather than claiming playback or retrying each frame.
   d={};return true;
  }catch(const std::exception& e){d.message=e.what();return false;}
 }
 // Caller supplies actual source elapsed, once at the world animation service
 // boundary. Cold source buffers and all outputs retain their backing storage.
 bool advance(float delta,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  d={};if(!ready_){d.message="GC animated world is not ready";return false;}
  try{pending_=state_;auto nextShared=shared;auto playback=playback_;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;auto& source=source_[i];auto& state=pending_[i];auto& q=state.sequence;
    if(detail::step(source,q,delta)){
     detail::lazy(playback);
     detail::need(g::sampleModelMotionTime50IG(source.motion,source.bindings,q.current34,{state.nodes.data(),3},nextShared,workspace_,d.motion),"GC source-time animation rejected");
     //154A00 stops only after sampling the overshoot frame. Rotation clamps;
     // original Bezier position tracks extrapolate on that same final call.
     if(!source.motionLoops&&q.current34>source.motion.duration){q.stopped10=1;q.current34=0.f;}
    }
    detail::need(project(i,state,nextVertices_,nextNormals_),"GC animated live projection rejected");
    detail::need(updateCollision(i,state),"GC animated compound live bounds rejected");
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
  auto next=lazy;auto raw=sharedRaw;
  if(!next.guard80A8){next.seed3D6B30=0;next.guard80A8=1;}
  if(!next.seed3D6B30){next.seed3D6B30=sourceTimestamp;raw=sourceTimestamp;}
  raw=raw*0x41c64e6du+0x3039u;
  volatile float converted=static_cast<float>(raw&0x7fffffffu);
  volatile float unit=converted*0x1p-31f;
  volatile float percent=101.f*unit;
  if(!next.guard80AC){next.rangeCheck3D6B38=1;next.guard80AC=1;}
  if(!std::isfinite(percent)||percent<0.f||percent>=101.f){d.message="GC animation source RNG range";return false;}
  if(!advance(delta,shared,d))return false;
  sharedRaw=raw;lazy=next;return true;
 }
 //101170 authored action callback, sharing101CD0 sequence stores. Called
 // only after the real authored
 // trigger/action selector has admitted it. Does not fake a trigger hit.
 bool activateSequence(std::size_t owner) noexcept {if(!ready_||owner>=ownerCount)return false;auto& q=state_[owner].sequence;q.active90=true;q.nextSequenceA0=q.nextMotionA8=-1;return true;}
 const GateSource& gateSource()const noexcept{return gateSource_;}
 g::GateTriggerBinding50KG& gateBinding() noexcept{return gate_;}
 const g::GateTriggerBinding50KG& gateBinding()const noexcept{return gate_;}
 const a::Mesh48B& mesh()const noexcept{return mesh_;}
 const std::vector<a::WorldNormal50HK>& normals()const noexcept{return normals_;}
 const std::array<Span,surfaceCount>& spans()const noexcept{return spans_;}
 const Source& source(std::size_t i)const{return source_.at(i);}
 const State& state(std::size_t i)const{return state_.at(i);}
 const g::CompoundCollisionView50IJ& collision(std::size_t i)const{return collisionViews_.at(i);}
 bool ready()const noexcept{return ready_;}
};
} // namespace gc_animated_world50IG
