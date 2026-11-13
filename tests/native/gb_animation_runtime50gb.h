#pragma once
#include "gb_animation_source50gb.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/rigid_vertices49s.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/compound_collision50ij.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../src/game/live_zone_membership50en.h"
#include <span>

namespace gb_animation_binding50GB { class Binding; }
namespace gb_collision_world50GB { class Owner; }
namespace gb_animation_runtime50GB {
namespace a=rr::assets;namespace g=rr::game;namespace resource=gb_animation_source50GB;
using Index=g::SceneNodeIndex49P;
inline constexpr auto ownerCount=resource::ownerCount;
struct Span {std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0,owner=0,material=0;};
struct Sequence {
 bool active90=false,loop88=false;std::int32_t nextSequenceA0=-1,nextMotionA8=-1;
 std::uint32_t stopped10=1;float current34=0.f,elapsedB0=0.f;
};
struct CollisionState {
 std::array<g::CompoundCollisionPart50IJ,resource::maximumParts> parts{};
 std::array<Index,resource::maximumParts> boundNodes{};
 std::array<g::BodyMatrix48G,resource::maximumParts> nodeWorld{},previousNodeWorld{};
 g::BodyMatrix48G ownerWorld{},previousOwnerWorld{};
 g::Vector3_48C center{},half{},sphereCenter{};float radius=0.f;
};
struct State {
 std::array<g::SceneNode49P,resource::maximumSceneNodes> nodes{};
 Sequence sequence{};CollisionState collision{};
 bool visualPrepared=false,collisionPrepared=false,initialAttempted=false,initialComplete=false;
 bool motionLoops=false; //MCF MOTION1 Looping, distinct from TOB Sequence.loop88
};
struct Diagnostic {std::string message;std::size_t owner=0;std::uint32_t sourcePC=0;
 g::ModelMotionDiagnostic49T motion{};g::RigidVertexDiagnostic49S projection{};};
class Owner final {
 friend class ::gb_animation_binding50GB::Binding;
 friend class ::gb_collision_world50GB::Owner;
 std::shared_ptr<const resource::Owner> source_;
 std::array<State,ownerCount> state_{};
 std::array<g::CompoundCollisionView50IJ,ownerCount> views_{};
 std::array<g::Type2StaticCandidate49X*,ownerCount> candidates_{};
 std::array<std::size_t,ownerCount> vertexStarts_{},vertexCounts_{};
 a::Mesh48B mesh_{};
 std::array<g::RigidVertex49S,resource::vertexCount> vertices_{};
 std::array<std::array<double,3>,resource::vertexCount> localNormals_{};
 std::array<g::RigidPosition49S,resource::vertexCount> projected_{};
 std::array<a::Vertex48B,resource::vertexCount> nextVertices_{};
 std::array<a::WorldNormal50HK,resource::vertexCount> normals_{},nextNormals_{};
 std::array<Span,resource::surfaceCount> spans_{};
 g::ModelMotionWorkspace49T workspace_{};
 g::ModelMotionSharedState49T* shared_=nullptr;
 const ::gb_animation_binding50GB::Binding* binding_=nullptr;
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,failed_=false,publicationReady_=false,servicing_=false;
 //Owners whose pose moved since the last shown frame. Host-side only.
 std::array<bool,ownerCount> visualPending_{};
 static g::BodyMatrix48G identity()noexcept {g::BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1.f;return m;}
 bool fail(Diagnostic& d,const char* text,std::uint32_t pc){failed_=true;publicationReady_=false;d.sourcePC=pc;d.message=text;return false;}
 static float add(float a,float b)noexcept {volatile float r=a+b;return r;}
 static float sub(float a,float b)noexcept {volatile float r=a-b;return r;}
 static void enterUpdate(g::ModelMotionSharedState49T& shared)noexcept {
  if(!shared.updateGuard3D5DBC){shared.updateValue433468=0xffffffffu;shared.updateGuard3D5DBC=1;}
 }
 g::SceneGraph49P graph(std::size_t i)noexcept {return {state_[i].nodes.data(),static_cast<std::uint32_t>(source_->source(i).graphCount)};}
 bool install(std::size_t i,Diagnostic& d){
  const auto& source=source_->source(i);auto& state=state_[i];
  for(auto& n:state.nodes){n={};n.localC0=n.world100=identity();}state.nodes[0].sceneFlags190=0x98;
  for(std::uint32_t row=0;row<source.hierarchy.nodeCount;++row){a::PsgNode49O original{};
   if(!a::readPsgNode49O(source.hierarchy,row,original))return fail(d,"GB authentic hierarchy row rejected",0x2d9214);
   const auto target=row+1,parent=original.parent<0?0u:static_cast<unsigned>(original.parent)+1;
   auto& n=state.nodes[target];std::memcpy(n.localC0.v,original.local,sizeof original.local);n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
   auto& first=state.nodes[parent].firstChild184;
   if(first==Index::none)first=static_cast<Index>(target);
   else {auto previous=static_cast<unsigned>(first);while(state.nodes[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(state.nodes[previous].nextSibling188);state.nodes[previous].nextSibling188=static_cast<Index>(target);}
  }
  if(!g::validateSceneGraph49P(graph(i))||!g::bindCompoundSceneNodes50IJ(source.collision,graph(i),static_cast<Index>(0),
       state.collision.boundNodes.data(),source.collisionCount))return fail(d,"GB actual root-inclusive compound map rejected",0x126450);
  //11ED60 constructor histories precede authored placement2D9324. The true
  //model root and each originally selected node are independently retained.
  if(!g::refreshSceneWorld49P(graph(i),static_cast<Index>(0)))return fail(d,"GB initial model root world rejected",0x11ed60);
  auto& c=state.collision;c.ownerWorld=c.previousOwnerWorld=state.nodes[0].world100;
  for(std::size_t part=0;part<source.collisionCount;++part){const auto bound=c.boundNodes[part];
   if(!g::refreshSceneWorld49P(graph(i),bound))return fail(d,"GB initial compound child world rejected",0x11ed60);
   c.nodeWorld[part]=c.previousNodeWorld[part]=state.nodes[static_cast<unsigned>(bound)].world100;
   auto& p=c.parts[part];p.mesh=&source.collision.members[part].mesh;p.active=1;
   p.nodeRadii=source.radii[part].data();p.nodeRadiusCount=source.radii[part].size();p.maxDepth=source.depth[part];
  }
  //Use the real placement setter to invalidate the actual graph descendants.
  g::BodyMatrix48G placed=identity();for(unsigned k=0;k<3;++k)placed.v[12+k]=source.authored.offset[k];
  if(!g::setLocalMatrix49P(graph(i),static_cast<Index>(0),placed))return fail(d,"GB real source placement rejected",0x2d9324);
  state.sequence.active90=source.authored.sequenceActive;state.sequence.loop88=source.authored.loopSequence;
  state.motionLoops=i==3; //Same hash-bound MCF: first three Looping=no, windmill=yes.
  return true;
 }
 bool project(std::size_t i,Diagnostic& d){
  const auto& source=source_->source(i);auto& state=state_[i];std::array<g::BodyMatrix48G,resource::maximumPsgNodes> palette{};
  for(std::uint32_t row=0;row<source.hierarchy.nodeCount;++row){
   if(!g::refreshSceneWorld49P(graph(i),static_cast<Index>(row+1)))return fail(d,"GB actual visual world rejected",0x2897b0);
   palette[row]=state.nodes[row+1].world100;
  }
  const auto start=vertexStarts_[i],count=vertexCounts_[i];
  if(!g::projectRigidVertices49S({vertices_.data()+start,static_cast<std::uint32_t>(count)},
      {palette.data(),source.hierarchy.nodeCount},{projected_.data()+start,static_cast<std::uint32_t>(count)},d.projection))
   return fail(d,"GB original rigid vertex projection rejected",0);
  for(std::size_t j=start;j<start+count;++j){std::memcpy(nextVertices_[j].position,projected_[j].v,sizeof projected_[j].v);
   const auto& m=palette[vertices_[j].paletteIndex];std::array<double,16> matrix{};for(unsigned k=0;k<16;++k)matrix[k]=m.v[k];
   if(!a::transformWorldNormal50HK(localNormals_[j],matrix,nextNormals_[j]))return fail(d,"GB native normal projection rejected",0);
  }
  //Publish only a complete source owner's stream; later-owner failure retains
  //this reached prefix and closes aggregate readiness rather than rolling back.
  std::copy_n(nextVertices_.begin()+start,count,mesh_.vertices.begin()+start);
  std::copy_n(nextNormals_.begin()+start,count,normals_.begin()+start);return true;
 }
 void updateMeshBounds()noexcept {
  for(unsigned axis=0;axis<3;++axis){mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=mesh_.vertices[0].position[axis];
   for(const auto& v:mesh_.vertices){mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],v.position[axis]);mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],v.position[axis]);}}
 }
 bool refreshCollision(std::size_t i,Diagnostic& d){
  auto& state=state_[i];auto& c=state.collision;const auto& source=source_->source(i);
  if(!g::refreshSceneWorld49P(graph(i),static_cast<Index>(0)))return fail(d,"GB compound owner refresh rejected",0x11f680);
  c.previousOwnerWorld=c.ownerWorld;c.ownerWorld=state.nodes[0].world100;
  for(std::size_t j=0;j<source.collisionCount;++j){
   if(!g::refreshSceneWorld49P(graph(i),c.boundNodes[j]))return fail(d,"GB compound child refresh rejected",0x11f8bc);
   c.previousNodeWorld[j]=c.nodeWorld[j];c.nodeWorld[j]=state.nodes[static_cast<unsigned>(c.boundNodes[j])].world100;
   std::array<float,16> world{},composed{};std::memcpy(world.data(),c.nodeWorld[j].v,sizeof c.nodeWorld[j].v);
   if(!a::composeCollisionWorld48I(source.collision.members[j].mesh.local_matrix,world,composed))return fail(d,"GB COL-local composition rejected",0x11f8f0);
   std::memcpy(c.parts[j].worldMatrix.v,composed.data(),sizeof c.parts[j].worldMatrix.v);
  }
  if(!g::compoundBounds50IJ(c.parts.data(),source.collisionCount,c.ownerWorld,c.center,c.half))return fail(d,"GB ordered compound local bounds rejected",0x11e630);
  auto& view=views_[i];view.parts=c.parts.data();view.count=source.collisionCount;view.ownerWorld=c.ownerWorld;view.center=c.center;view.half=c.half;
  if(!g::compoundSphere50IJ(view,c.sphereCenter,c.radius))return fail(d,"GB compound world sphere rejected",0x11f9b4);
  if(candidates_[i]){auto& target=*candidates_[i];auto& collision=target.candidate.collision;
   collision.geometry.worldMatrix=c.ownerWorld;collision.sphere.center=c.sphereCenter;collision.sphere.radius=c.radius;}
  return true;
 }
 bool prepareCollisionAfterVisual(std::size_t i,Diagnostic& d){
  if(!binding_||failed_||i>=ownerCount||!state_[i].visualPrepared||state_[i].collisionPrepared||state_[i].initialAttempted)
   return fail(d,"GB collision refresh requires its genuine visual prefix",0x11fe50);
  if(!refreshCollision(i,d))return false;state_[i].collisionPrepared=true;return true;
 }
 bool initialSample(std::size_t i,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  if(!binding_||failed_||i>=ownerCount||shared_!=&shared||!state_[i].collisionPrepared||state_[i].initialAttempted)
   return fail(d,"GB initial sample requires the same registered source prefix",0x2d946c);
  state_[i].initialAttempted=true;enterUpdate(shared);const auto& s=source_->source(i);
  if(!g::sampleModelMotionTime50IG(s.motion,s.bindings,0.f,graph(i),shared,workspace_,d.motion))return fail(d,"GB initial source sample rejected",0x154480);
  //2D9490 stops the actual motion, including the initially active windmill.
  //101390/101728 starts that sequence on the first enabled world service.
  state_[i].sequence.stopped10=1;state_[i].sequence.current34=0.f;
  if(!project(i,d))return false;state_[i].initialComplete=true;return true;
 }
 bool advance(std::size_t i,float delta,g::ModelMotionSharedState49T& shared,bool& entered,Diagnostic& d){
  entered=false;
  auto& q=state_[i].sequence;const auto& s=source_->source(i);
  if(q.nextSequenceA0==-1){q.nextSequenceA0=0;
   if(!q.active90)q.nextSequenceA0=q.nextMotionA8=-1;
   else if(++q.nextMotionA8>=1){if(q.loop88)q.nextMotionA8=0;else{q.active90=false;--q.nextMotionA8;q.nextSequenceA0=-1;}}
  }
  if(q.stopped10&&q.active90){q.nextSequenceA0=-1;q.elapsedB0=0.f;q.stopped10=0;q.current34=0.f;}
  //1016DC..1016E0 skips the entire visual/collision update for a stopped owner.
  if(q.stopped10)return true;entered=true;
  {q.elapsedB0=add(q.elapsedB0,delta);enterUpdate(shared);q.current34=add(q.current34,delta);
   if(!std::isfinite(q.elapsedB0)||!std::isfinite(q.current34))return fail(d,"GB source motion time rejected",0x101728);
   if(q.current34>s.motion.duration&&state_[i].motionLoops)q.current34=sub(q.current34,s.motion.duration); //subtract ONCE
   if(!g::sampleModelMotionTime50IG(s.motion,s.bindings,q.current34,graph(i),shared,workspace_,d.motion))return fail(d,"GB direct source-time sample rejected",0x154480);
   if(!state_[i].motionLoops&&q.current34>s.motion.duration){q.stopped10=1;q.current34=0.f;}
  }
  //Original 101728 moves only the animation nodes here; the visual vertices
  //are transformed when the model is drawn (241280 palette rows -> VU). The
  //native CPU projection of the same node worlds runs in prepareDraw().
  visualPending_[i]=true;return true;
 }
 bool serviceRandom(std::uint32_t timestamp,std::uint32_t& raw,g::SharedRandomOwner50ID& random,Diagnostic& d){
  if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
  if(!random.seed.seed3D6B30){random.seed.seed3D6B30=timestamp;raw=timestamp;}
  raw=raw*0x41c64e6du+0x3039u;volatile float converted=static_cast<float>(raw&0x7fffffffu);
  volatile float unit=converted*0x1p-31f;volatile float percent=101.f*unit;
  if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
  if(!std::isfinite(percent)||percent<0.f||percent>101.f)return fail(d,"GB source RNG conversion rejected",0x101914);
  const auto selected=static_cast<std::uint32_t>(percent);
  return !(random.range.rangeCheck3D6B38&&selected>=101u)||fail(d,"GB source RNG range rejected",0x101944);
 }
 g::LiveZoneBounds50EN collisionBounds(std::size_t i)const noexcept {
  const auto& c=state_[i].collision;g::LiveZoneBounds50EN out{};out.bounds.shapeTag=1;out.bounds.shapeWorld=c.ownerWorld;
  for(unsigned k=0;k<3;++k){out.bounds.localCenter.v[k]=c.center.v[k];out.bounds.localHalf.v[k]=c.half.v[k];out.bounds.cachedCenter.v[k]=c.sphereCenter.v[k];}
  out.bounds.localCenter.v[3]=out.bounds.localHalf.v[3]=out.bounds.cachedCenter.v[3]=1.f;out.bounds.cachedRadius=c.radius;return out;
 }
 bool prepareCandidate(std::size_t i,g::Type2StaticCandidate49X& target,std::uint8_t tag,std::uint8_t flags31,std::uint8_t flagsF8)noexcept {
  if(!stagedSourceReady()||!binding_||i>=ownerCount||!state_[i].collisionPrepared||candidates_[i])return false;
  const auto& s=source_->source(i);target={};target.shapeType=1;target.compound=&views_[i];
  for(std::size_t j=0;j<s.collisionCount;++j)target.maxDepth=std::max(target.maxDepth,s.depth[j]);
  target.candidate.hasMaterialScene=true;target.candidate.sceneName=s.scene.nodes.front().name.data();target.candidate.sceneNameBytes=s.scene.nodes.front().name.size();
  auto& c=target.candidate.collision;c.shapeType=1;c.compound=&views_[i];c.geometry.worldMatrix=state_[i].collision.ownerWorld;
  c.geometry.id=0x3e000000u+static_cast<std::uint32_t>(i);c.filter.identity=c.geometry.id;c.ownerIdentity9C=0x3f000000u+static_cast<std::uint32_t>(i);
  c.typeTag=tag;c.hasResponseCallbackE8=false;c.filter.categoryA4=1u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|5u);
  c.sphere.center=state_[i].collision.sphereCenter;c.sphere.radius=state_[i].collision.radius;c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;
  candidates_[i]=&target;return true;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(binding_||servicing_||thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept {
  return sizeof(Owner)+resource::vertexCount*sizeof(a::Vertex48B)+resource::indexCount*sizeof(std::uint32_t);
 }
 bool beginPrepare(std::shared_ptr<const resource::Owner> source,g::ModelMotionSharedState49T& actualSharedMotion,
      std::size_t remaining,Diagnostic& d){
  d={};if(attempted_){d.message="GB animation runtime requires a fresh stable owner";return false;}attempted_=true;
  try{
   if(thread_!=std::this_thread::get_id()||!source||!source->ready()||remaining<requiredPayloadBytes()||std::fegetround()!=FE_TONEAREST)
    return fail(d,"GB real cold source/budget/arithmetic unavailable",0x2d9190);
   source_=std::move(source);shared_=&actualSharedMotion;mesh_.vertices.resize(resource::vertexCount);mesh_.indices.reserve(resource::indexCount);
   std::size_t vertex=0,surfaceIndex=0;
   for(std::size_t i=0;i<ownerCount;++i){d.owner=i;const auto& source=source_->source(i);vertexStarts_[i]=vertex;
    for(const auto& surface:source.scene.lods[0].surfaces){auto& span=spans_.at(surfaceIndex++);
     span={static_cast<std::uint32_t>(vertex),static_cast<std::uint32_t>(surface.vertices.size()),static_cast<std::uint32_t>(mesh_.indices.size()),
       static_cast<std::uint32_t>(surface.indices.size()),static_cast<std::uint32_t>(i),surface.material_index};
     for(const auto& original:surface.vertices){if(vertex>=resource::vertexCount)return fail(d,"GB source vertex extent changed",0);
      auto& v=vertices_[vertex];for(unsigned k=0;k<3;++k)v.positionLocal[k]=original.position_local[k];v.paletteIndex=original.bone;localNormals_[vertex]=original.normal_local;
      for(unsigned k=0;k<2;++k)nextVertices_[vertex].uv[k]=static_cast<float>(original.uv[k]);
      for(unsigned k=0;k<4;++k)nextVertices_[vertex].color[k]=255; //native neutral multiplier; no authored color stream
      ++vertex;
     }
     for(const auto index:surface.indices)mesh_.indices.push_back(span.vertexStart+index);
    }
    vertexCounts_[i]=vertex-vertexStarts_[i];if(!install(i,d))return false;
   }
   if(vertex!=resource::vertexCount||mesh_.indices.size()!=resource::indexCount||surfaceIndex!=resource::surfaceCount||
      sizeof(Owner)+mesh_.vertices.capacity()*sizeof(a::Vertex48B)+mesh_.indices.capacity()*sizeof(std::uint32_t)>requiredPayloadBytes())
    return fail(d,"GB runtime exact extent/retained budget rejected",0);
   d={};return true;
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
 //Once per shown frame, before the dynamic window binds this mesh: project
 //every owner that moved since the last shown frame from its CURRENT node
 //worlds (the same lazy 2897B0 getter the step's visual/collision refresh
 //already used), then the aggregate bounds. Game state is not read or written
 //beyond those already-refreshed node worlds; 2-4 catch-up steps no longer
 //each project vertices that only the last of them would show.
 bool prepareDraw(Diagnostic& d)noexcept {
  try{d={};if(!ready()||servicing_||thread_!=std::this_thread::get_id())return fail(d,"GB draw projection outside ready owner thread",0x241280);
   bool any=false;
   for(std::size_t i=0;i<ownerCount;++i){if(!visualPending_[i])continue;d.owner=i;
    if(!project(i,d))return false;visualPending_[i]=false;any=true;}
   if(any)updateMeshBounds();return true;
  }catch(...){failed_=true;publicationReady_=false;return false;}
 }
 bool activateSequence(std::size_t i)noexcept {
  if(!ready()||servicing_||(i!=0&&i!=2))return false;auto& q=state_[i].sequence;q.active90=true;q.nextSequenceA0=q.nextMotionA8=-1;return true;
 }
 bool stagedSourceReady()const noexcept {return attempted_&&!failed_&&!servicing_&&thread_==std::this_thread::get_id()&&source_&&source_->ready();}
 bool ready()const noexcept {return stagedSourceReady()&&binding_&&publicationReady_;}
 bool failed()const noexcept {return failed_;}
 bool bindingRetired()const noexcept{return !binding_&&!servicing_;}
 const resource::Owner* sourceOwner()const noexcept {return source_.get();}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept {return source_?source_->coldSource():nullptr;}
 const resource::Source& source(std::size_t i)const {return source_->source(i);}
 const State& state(std::size_t i)const {return state_.at(i);}
 const g::CompoundCollisionView50IJ& collision(std::size_t i)const {return views_.at(i);}
 const a::Mesh48B& mesh()const noexcept {return mesh_;}
 const auto& normals()const noexcept {return normals_;}
 const auto& spans()const noexcept {return spans_;}
};
}
