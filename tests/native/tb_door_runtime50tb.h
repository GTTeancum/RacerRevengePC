#pragma once
#include "tb_door_motion50tb.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/rigid_vertices49s.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/pod_shape49w.h"
#include "../../src/game/static_type2_producer49x.h"
#include "../../src/game/live_zone_membership50en.h"
#include <algorithm>
#include <cfenv>
#include <thread>

namespace tb_door_binding50TB { class Binding; }
namespace tb_collision_world50TB { class Owner; }
namespace tb_door_candidates50TB { class Candidates; }
namespace tb_door_runtime50TB {
namespace a=rr::assets;namespace g=rr::game;namespace resources=tb_door_motion50TB;
using Index=g::SceneNodeIndex49P;
struct Span {std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0,material=0;};
struct Sequence {
 bool active90=false,loop88=false;std::int32_t nextSequenceA0=-1,nextMotionA8=-1;
 std::uint32_t stopped10=1;float current34=0.f,elapsedB0=0.f;
};
struct Diagnostic {std::string message;std::uint32_t sourcePC=0;g::ModelMotionDiagnostic49T motion{};g::RigidVertexDiagnostic49S projection{};};
class Owner final {
 friend class ::tb_door_binding50TB::Binding;
 friend class ::tb_collision_world50TB::Owner;
 friend class ::tb_door_candidates50TB::Candidates;
 std::shared_ptr<const resources::SourceOwner> source_;
 std::array<g::SceneNode49P,resources::sceneNodeCount> nodes_{};
 Sequence sequence_{};a::Mesh48B mesh_{};
 g::PodShapeSource49W collisionSource_{};g::PodShapeState49W collisionState_{};
 const ::tb_door_binding50TB::Binding* binding_=nullptr;
 bool collisionPrepared_=false,publicationReady_=false,posePending_=false;
 g::Type2StaticCandidate49X* candidate_=nullptr;
 std::array<g::RigidVertex49S,resources::vertexCount> vertices_{};
 std::array<std::array<double,3>,resources::vertexCount> localNormals_{};
 std::array<g::RigidPosition49S,resources::vertexCount> projected_{};
 std::array<a::Vertex48B,resources::vertexCount> nextVertices_{};
 std::array<a::WorldNormal50HK,resources::vertexCount> normals_{},nextNormals_{};
 std::array<Span,resources::surfaceCount> spans_{{{0,resources::vertexCount,0,resources::indexCount,0}}};
 g::ModelMotionWorkspace49T workspace_{};
 const std::thread::id thread_=std::this_thread::get_id();
 bool attempted_=false,ready_=false,failed_=false,servicing_=false;
 bool initialSamplePending_=false,initialSampleAttempted_=false;
 g::ModelMotionSharedState49T* initialShared_=nullptr;
 static g::BodyMatrix48G identity()noexcept{g::BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1.f;return m;}
 bool fail(Diagnostic& d,const char* text,std::uint32_t pc){failed_=true;d.sourcePC=pc;d.message=text;return false;}
 static float add(float a,float b)noexcept{volatile float v=a+b;return v;}
 static void enterUpdate(g::ModelMotionSharedState49T& shared)noexcept {
  if(!shared.updateGuard3D5DBC){shared.updateValue433468=0xffffffffu;shared.updateGuard3D5DBC=1;}
 }
 bool install(Diagnostic& d){
  for(auto& node:nodes_){node={};node.localC0=node.world100=identity();}
  nodes_[0].sceneFlags190=0x98;
  for(unsigned i=0;i<3;++i)nodes_[0].localC0.v[12+i]=source_->authored().offset[i];
  for(std::uint32_t i=0;i<resources::nodeCount;++i){a::PsgNode49O actual{};
   if(!a::readPsgNode49O(source_->hierarchy(),i,actual))return fail(d,"TB actual hierarchy row rejected",0x2d9214u);
   const auto target=i+1,parent=actual.parent<0?0u:static_cast<unsigned>(actual.parent)+1;
   auto& n=nodes_[target];std::memcpy(n.localC0.v,actual.local,sizeof actual.local);n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
   auto& first=nodes_[parent].firstChild184;
   if(first==Index::none)first=static_cast<Index>(target);
   else {auto previous=static_cast<unsigned>(first);while(nodes_[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(nodes_[previous].nextSibling188);nodes_[previous].nextSibling188=static_cast<Index>(target);}
  }
  return g::validateSceneGraph49P({nodes_.data(),resources::sceneNodeCount})||fail(d,"TB actual model graph rejected",0x2d9214u);
 }
 bool project(Diagnostic& d){
  const g::SceneGraph49P graph{nodes_.data(),resources::sceneNodeCount};std::array<g::BodyMatrix48G,resources::nodeCount> palette{};
  for(unsigned i=0;i<resources::nodeCount;++i){
   if(!g::refreshSceneWorld49P(graph,static_cast<Index>(i+1)))return fail(d,"TB actual door world refresh rejected",0x2897b0u);
   palette[i]=nodes_[i+1].world100;
  }
  if(!g::projectRigidVertices49S({vertices_.data(),resources::vertexCount},{palette.data(),resources::nodeCount},
       {projected_.data(),resources::vertexCount},d.projection))return fail(d,"TB actual rigid vertex projection rejected",0x0u);
  for(std::size_t i=0;i<vertices_.size();++i){
   std::memcpy(nextVertices_[i].position,projected_[i].v,sizeof projected_[i].v);
   const auto& m=palette[vertices_[i].paletteIndex];std::array<double,16> matrix{};
   for(unsigned k=0;k<16;++k)matrix[k]=m.v[k];
   if(!a::transformWorldNormal50HK(localNormals_[i],matrix,nextNormals_[i]))return fail(d,"TB native normal projection rejected",0x0u);
  }
  // Native render publication: never publish half a vertex/normal stream.
  std::copy(nextVertices_.begin(),nextVertices_.end(),mesh_.vertices.begin());normals_=nextNormals_;
  for(unsigned axis=0;axis<3;++axis){mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=mesh_.vertices[0].position[axis];
   for(const auto& v:mesh_.vertices){mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],v.position[axis]);mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],v.position[axis]);}}
  return true;
 }
 bool advance(float delta,g::ModelMotionSharedState49T& shared,Diagnostic& d){
  auto& q=sequence_;
  // Closed101390: exactly one actual sequence/motion,probability100,no cross-motion blend.
  if(q.nextSequenceA0==-1){q.nextSequenceA0=0;
   if(!q.active90)q.nextSequenceA0=q.nextMotionA8=-1;
   else if(++q.nextMotionA8>=1){q.active90=false;--q.nextMotionA8;q.nextSequenceA0=-1;}
  }
  if(q.stopped10&&q.active90){q.nextSequenceA0=-1;q.elapsedB0=0.f;q.stopped10=0;q.current34=0.f;}
  if(!q.stopped10){
   q.elapsedB0=add(q.elapsedB0,delta);
   //101728 elapsed store precedes154480 entry sentinel and current34 addition.
   enterUpdate(shared);q.current34=add(q.current34,delta);
   if(!std::isfinite(q.elapsedB0)||!std::isfinite(q.current34))return fail(d,"TB actual animation time arithmetic rejected",0x101728u);
   if(!g::sampleModelMotionTime50IG(source_->motion(),source_->bindings(),q.current34,
      {nodes_.data(),resources::sceneNodeCount},shared,workspace_,d.motion))return fail(d,"TB original direct-time motion rejected",0x154480u);
   //154A00 stops only after the actual nonloop overshoot sample. No MCF10 retime.
   if(q.current34>source_->motion().duration){q.stopped10=1;q.current34=0.f;}
  }
  return project(d);
 }
 // Only the typed Binding reaches11FE50 after its actual visual attachment.
 // The state is the same COL0 root projection; moving panel nodes are never
 // assigned to this shape. Its constructor world history is the real model
 // root at construction, retained before later placement/publication.
 bool prepareCollisionAfterVisual(Diagnostic& d)noexcept {
  if(!binding_||collisionPrepared_||failed_||!initialSamplePending_||initialSampleAttempted_)
   return fail(d,"TB collision preparation requires its attached visual prefix",0x11fe50u);
  if(!g::refreshSceneWorld49P({nodes_.data(),resources::sceneNodeCount},static_cast<Index>(0))||
      !g::preparePodShape50IZ(collisionSource_,nodes_[0].world100,collisionState_))
   return fail(d,"TB actual root-bound type0 preparation rejected",0x11f680u);
  collisionPrepared_=true;return true;
 }
 g::LiveZoneBounds50EN collisionBounds()const noexcept {
  g::LiveZoneBounds50EN out{};out.bounds.shapeTag=0;out.type0Resource=&collisionSource_;
  out.bounds.shapeWorld=collisionState_.composed00;
  std::memcpy(out.bounds.cachedCenter.v,collisionState_.center60.v,16);
  out.bounds.cachedRadius=collisionState_.radius80;return out;
 }
 // Private aggregate-only cold publication, after actual visual attach and
 //11FE50. The exact address then stays fixed until aggregate retirement.
 bool prepareCollisionCandidate(g::Type2StaticCandidate49X& target,std::uint8_t tag,
      std::uint8_t flags31,std::uint8_t flagsF8)noexcept {
  if(!stagedSourceReady()||!binding_||!collisionPrepared_||candidate_)return false;
  target={};target.shapeType=0;target.nodeRadii=source_->nodeRadii().data();
  target.nodeRadiusCount=source_->nodeRadii().size();target.maxDepth=source_->maxDepth();
  target.candidate.hasMaterialScene=true;
  target.candidate.sceneName=source_->scene().nodes.front().name.data();
  target.candidate.sceneNameBytes=source_->scene().nodes.front().name.size();
  auto& c=target.candidate.collision;c.shapeType=0;c.geometry.mesh=&source_->collision();
  c.geometry.worldMatrix=collisionState_.composed00;c.geometry.id=0x3c000000u;c.filter.identity=c.geometry.id;
  c.ownerIdentity9C=0x3d000000u;c.typeTag=tag;c.hasResponseCallbackE8=false;
  c.filter.categoryA4=1u;c.filter.acceptMaskB8=0xffffffffu;c.filter.categoryModeD4=1;
  c.filter.identityLists[0].mode=c.filter.identityLists[1].mode=1;
  c.filter.flags31=static_cast<std::uint8_t>((flags31&0xf0u)|3u);
  c.filter.flagsF8=static_cast<std::uint8_t>((flagsF8&0xc0u)|5u);
  std::memcpy(c.sphere.center.v,collisionState_.center60.v,sizeof c.sphere.center.v);c.sphere.radius=collisionState_.radius80;
  c.copied40={0.f,0.f,0.f,1.f};c.copied50=c.copied40;candidate_=&target;return true;
 }
 // Sole runtime caller is the typed real Binding, after BOTH actual
 // method2 registrations succeed. Keep those memberships on later failure.
 bool completeInitialSample(g::ModelMotionSharedState49T& shared,Diagnostic& d){
  if(thread_!=std::this_thread::get_id()||!initialSamplePending_||initialSampleAttempted_||
      initialShared_!=&shared||!binding_||!collisionPrepared_||!collisionState_.registrationE0||ready_||failed_||servicing_||!source_||!source_->ready()||std::fegetround()!=FE_TONEAREST){
   d.message="TB reached initial sample requires its same pending real owner";return false;
  }
  initialSampleAttempted_=true;
  try{
   //2D946C starts,2D9480 samples delta0 through154480,2D9490 stops.
   enterUpdate(shared);
   if(!g::sampleModelMotionTime50IG(source_->motion(),source_->bindings(),0.f,{nodes_.data(),resources::sceneNodeCount},
        shared,workspace_,d.motion))return fail(d,"TB actual initial door motion rejected",0x154480u);
   if(!project(d))return false;
   initialSamplePending_=false;initialShared_=nullptr;ready_=true;d={};return true;
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(binding_||servicing_||thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept {
  return sizeof(Owner)+resources::vertexCount*sizeof(a::Vertex48B)+resources::indexCount*sizeof(std::uint32_t);
 }
 bool beginPrepare(std::shared_ptr<const resources::SourceOwner> actual,g::ModelMotionSharedState49T& shared,
      std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(attempted_){d.message="TB door runtime requires a fresh lifetime";return false;}attempted_=true;
  try{
   if(thread_!=std::this_thread::get_id()||!actual||!actual->ready()||!actual->authored().useCollision||actual->authored().loopSequence||!actual->authored().oneTimeOnly||
      availableCpuPayloadBytes<requiredPayloadBytes()||std::fegetround()!=FE_TONEAREST)return fail(d,"TB actual door source/budget/rounding unavailable",0x2d9190u);
   source_=std::move(actual);mesh_.vertices.resize(resources::vertexCount);mesh_.indices.reserve(resources::indexCount);
   const auto& surface=source_->scene().lods[0].surfaces[0];
   for(std::size_t i=0;i<resources::vertexCount;++i){const auto& original=surface.vertices[i];auto& v=vertices_[i];
    for(unsigned k=0;k<3;++k)v.positionLocal[k]=original.position_local[k];v.paletteIndex=original.bone;localNormals_[i]=original.normal_local;
    for(unsigned k=0;k<2;++k)nextVertices_[i].uv[k]=static_cast<float>(original.uv[k]);
    // This authentic component107 has no color stream. Match the existing
    // explicit native compressed-mesh presentation policy;255 is a neutral
    // renderer multiplier, not a recovered authored color or lighting cache.
    for(unsigned k=0;k<4;++k)nextVertices_[i].color[k]=255;
   }
   mesh_.indices.assign(surface.indices.begin(),surface.indices.end());
   const auto actualPayload=sizeof(Owner)+mesh_.vertices.capacity()*sizeof(a::Vertex48B)+mesh_.indices.capacity()*sizeof(std::uint32_t);
   if(actualPayload>availableCpuPayloadBytes)return fail(d,"TB actual door retained payload exceeds remaining budget",0u);
   if(!install(d))return false;
   const auto& col=source_->collision();
   if(col.nodes.size()!=resources::collisionNodeCount||col.vertices.size()!=resources::collisionVertexCount||
      !g::refreshSceneWorld49P({nodes_.data(),resources::sceneNodeCount},static_cast<Index>(0)))
    return fail(d,"TB original collision root unavailable",0x11f570u);
   std::memcpy(collisionSource_.local40.v,col.local_matrix.data(),sizeof collisionSource_.local40.v);
   for(unsigned i=0;i<3;++i){collisionSource_.rootCenter.v[i]=col.nodes[0].center[i];collisionSource_.rootHalf.v[i]=col.nodes[0].half[i];}
   collisionState_.type88=0;collisionState_.registrationE0=0;
   //11EDB4..EE04 stores the actual model root world into both histories.
   // Other constructor temporaries are not exposed by this native projection;
   // actual11FE50 later prepares every query/bounds-consumed value.
   collisionState_.world80=collisionState_.previousC0=nodes_[0].world100;
   initialShared_=&shared;initialSamplePending_=true;d={};return true;
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
 bool activateSequence(std::size_t owner)noexcept {
  if(!ready()||servicing_||owner!=0)return false;
  sequence_.active90=true;sequence_.nextSequenceA0=sequence_.nextMotionA8=-1;return true;
 }
 // actualWorldDelta30 is the real world-service scaled delta, not MCF FPS.
 // Call only at actual enabled101860 service, outside all query/Bank borrows.
 bool service(float actualWorldDelta30,std::uint32_t sourceTimestamp,std::uint32_t& raw,
      g::SharedRandomOwner50ID& random,g::ModelMotionSharedState49T& motion,Diagnostic& d){
  d={};if(!ready()||servicing_||!std::isfinite(actualWorldDelta30)||actualWorldDelta30<0.f||std::fegetround()!=FE_TONEAREST){d.message="TB actual animation service admission";return false;}
  servicing_=true;struct End{bool& b;~End(){b=false;}}end{servicing_};
  try{
   // Same process RNG/lazy owners as the other actual selectors. Keep reached
   // stores on later failure. This is not GC's older whole-service staging.
   if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
   if(!random.seed.seed3D6B30){random.seed.seed3D6B30=sourceTimestamp;raw=sourceTimestamp;}
   raw=raw*0x41c64e6du+0x3039u;
   volatile float converted=static_cast<float>(raw&0x7fffffffu);volatile float unit=converted*0x1p-31f;volatile float percent=101.f*unit;
   if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
   if(!std::isfinite(percent)||percent<0.f||percent>101.f)return fail(d,"TB animation RNG conversion rejected",0x101914u);
   // Existing EE conversion policy from49K/50ID/50IW: CVT.W.S truncates,
   // separately from the admitted nearest-even binary32 arithmetic.
   const auto selected=static_cast<std::uint32_t>(percent);
   if(random.range.rangeCheck3D6B38&&selected>=101u)return fail(d,"TB actual animation RNG range rejected",0x101944u);
   if(!advance(actualWorldDelta30,motion,d))return false;
   posePending_=true;return true;
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
 // Structural cold ownership only. This does not grant query, animation or
 // observer/collision readiness. It is stable through the staged bind.
 bool stagedSourceReady()const noexcept{return attempted_&&!failed_&&!servicing_&&thread_==std::this_thread::get_id()&&source_&&source_->ready();}
 bool ready()const noexcept{return ready_&&publicationReady_&&!posePending_&&binding_&&!failed_&&thread_==std::this_thread::get_id()&&source_&&source_->ready();}
 // Pure Storage admission for this exact already-prepared single root COL.
 // A caller cannot issue this authority with shape0 bits or a copied candidate.
 bool admitsColdCollisionCandidate(const g::Type2StaticCandidate49X& target,std::uint32_t zone)const noexcept {
  if(!stagedSourceReady()||!binding_||!collisionPrepared_||candidate_!=&target||zone!=4||
     ready_||!initialSamplePending_||initialSampleAttempted_||collisionState_.registrationE0)return false;
  const auto& c=target.candidate.collision;
  return target.shapeType==0&&!target.compound&&!target.gate&&!target.nativeTriggers&&!target.nativeTriggersTC&&!target.nativeTriggersTB&&
   !target.nativeTriggerIndex&&!target.raOriginalTriggerSegments&&target.nodeRadii==source_->nodeRadii().data()&&
   target.nodeRadiusCount==source_->nodeRadii().size()&&target.maxDepth==source_->maxDepth()&&
   c.shapeType==0&&!c.compound&&c.geometry.mesh==&source_->collision()&&c.geometry.id==0x3c000000u&&
   c.filter.identity==c.geometry.id&&c.ownerIdentity9C==0x3d000000u&&c.filter.categoryA4==1u&&!c.hasResponseCallbackE8&&
   !std::memcmp(c.geometry.worldMatrix.v,collisionState_.composed00.v,sizeof collisionState_.composed00.v)&&
   !std::memcmp(c.sphere.center.v,collisionState_.center60.v,sizeof c.sphere.center.v)&&
   !std::memcmp(&c.sphere.radius,&collisionState_.radius80,sizeof(float));
 }
 bool failed()const noexcept{return failed_;}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return source_?source_->coldSource():nullptr;}
 const resources::SourceOwner* sourceOwner()const noexcept{return source_.get();}
 const resources::SourceOwner& source()const{return *source_;}
 const Sequence& sequence()const noexcept{return sequence_;}
 const std::array<g::SceneNode49P,resources::sceneNodeCount>& nodes()const noexcept{return nodes_;}
 const a::Mesh48B& mesh()const noexcept{return mesh_;}
 const std::array<a::WorldNormal50HK,resources::vertexCount>& normals()const noexcept{return normals_;}
 const std::array<Span,resources::surfaceCount>& spans()const noexcept{return spans_;}
};
}
