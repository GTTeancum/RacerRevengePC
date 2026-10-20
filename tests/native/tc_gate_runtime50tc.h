#pragma once
#include "tc_gate_motion50tc.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/rigid_vertices49s.h"
#include "../../src/game/shared_random_owner50id.h"
#include <algorithm>
#include <cfenv>
#include <thread>

namespace tc_gate_visibility50TC { class Binding; }
namespace tc_gate_runtime50TC {
namespace a=rr::assets;namespace g=rr::game;namespace resources=tc_gate_motion50TC;
using Index=g::SceneNodeIndex49P;
struct Span {std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0,material=0;};
struct Sequence {
 bool active90=false,loop88=false;std::int32_t nextSequenceA0=-1,nextMotionA8=-1;
 std::uint32_t stopped10=1;float current34=0.f,elapsedB0=0.f;
};
struct Diagnostic {std::string message;std::uint32_t sourcePC=0;g::ModelMotionDiagnostic49T motion{};g::RigidVertexDiagnostic49S projection{};};
class Owner final {
 friend class ::tc_gate_visibility50TC::Binding;
 std::shared_ptr<const resources::SourceOwner> source_;
 std::array<g::SceneNode49P,resources::sceneNodeCount> nodes_{};
 Sequence sequence_{};a::Mesh48B mesh_{};
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
   if(!a::readPsgNode49O(source_->hierarchy(),i,actual))return fail(d,"TC actual hierarchy row rejected",0x2d9214u);
   const auto target=i+1,parent=actual.parent<0?0u:static_cast<unsigned>(actual.parent)+1;
   auto& n=nodes_[target];std::memcpy(n.localC0.v,actual.local,sizeof actual.local);n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
   auto& first=nodes_[parent].firstChild184;
   if(first==Index::none)first=static_cast<Index>(target);
   else {auto previous=static_cast<unsigned>(first);while(nodes_[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(nodes_[previous].nextSibling188);nodes_[previous].nextSibling188=static_cast<Index>(target);}
  }
  return g::validateSceneGraph49P({nodes_.data(),resources::sceneNodeCount})||fail(d,"TC actual model graph rejected",0x2d9214u);
 }
 bool project(Diagnostic& d){
  const g::SceneGraph49P graph{nodes_.data(),resources::sceneNodeCount};std::array<g::BodyMatrix48G,resources::nodeCount> palette{};
  for(unsigned i=0;i<resources::nodeCount;++i){
   if(!g::refreshSceneWorld49P(graph,static_cast<Index>(i+1)))return fail(d,"TC actual gate world refresh rejected",0x2897b0u);
   palette[i]=nodes_[i+1].world100;
  }
  if(!g::projectRigidVertices49S({vertices_.data(),resources::vertexCount},{palette.data(),resources::nodeCount},
       {projected_.data(),resources::vertexCount},d.projection))return fail(d,"TC actual rigid vertex projection rejected",0x0u);
  for(std::size_t i=0;i<vertices_.size();++i){
   std::memcpy(nextVertices_[i].position,projected_[i].v,sizeof projected_[i].v);
   const auto& m=palette[vertices_[i].paletteIndex];std::array<double,16> matrix{};
   for(unsigned k=0;k<16;++k)matrix[k]=m.v[k];
   if(!a::transformWorldNormal50HK(localNormals_[i],matrix,nextNormals_[i]))return fail(d,"TC native normal projection rejected",0x0u);
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
   if(!std::isfinite(q.elapsedB0)||!std::isfinite(q.current34))return fail(d,"TC actual animation time arithmetic rejected",0x101728u);
   if(!g::sampleModelMotionTime50IG(source_->motion(),source_->bindings(),q.current34,
      {nodes_.data(),resources::sceneNodeCount},shared,workspace_,d.motion))return fail(d,"TC original direct-time motion rejected",0x154480u);
   //154A00 stops only after the actual nonloop overshoot sample. No MCF22 retime.
   if(q.current34>source_->motion().duration){q.stopped10=1;q.current34=0.f;}
  }
  return project(d);
 }
 // Sole runtime caller is the typed real visibility Binding, after its
 // method2 registration succeeds. Keep that registration on a later failure.
 bool completeInitialSample(g::ModelMotionSharedState49T& shared,Diagnostic& d){
  if(thread_!=std::this_thread::get_id()||!initialSamplePending_||initialSampleAttempted_||
      initialShared_!=&shared||ready_||failed_||servicing_||!source_||!source_->ready()||std::fegetround()!=FE_TONEAREST){
   d.message="TC reached initial sample requires its same pending real owner";return false;
  }
  initialSampleAttempted_=true;
  try{
   //2D946C starts,2D9480 samples delta0 through154480,2D9490 stops.
   enterUpdate(shared);
   if(!g::sampleModelMotionTime50IG(source_->motion(),source_->bindings(),0.f,{nodes_.data(),resources::sceneNodeCount},
        shared,workspace_,d.motion))return fail(d,"TC actual initial gate motion rejected",0x154480u);
   if(!project(d))return false;
   initialSamplePending_=false;initialShared_=nullptr;ready_=true;d={};return true;
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(servicing_||thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t requiredPayloadBytes()noexcept {
  return sizeof(Owner)+resources::vertexCount*sizeof(a::Vertex48B)+resources::indexCount*sizeof(std::uint32_t);
 }
 bool beginPrepare(std::shared_ptr<const resources::SourceOwner> actual,g::ModelMotionSharedState49T& shared,
      std::size_t availableCpuPayloadBytes,Diagnostic& d){
  d={};if(attempted_){d.message="TC gate runtime requires a fresh lifetime";return false;}attempted_=true;
  try{
   if(thread_!=std::this_thread::get_id()||!actual||!actual->ready()||actual->authored().useCollision||actual->authored().loopSequence||!actual->authored().oneTimeOnly||
      availableCpuPayloadBytes<requiredPayloadBytes()||std::fegetround()!=FE_TONEAREST)return fail(d,"TC actual gate source/budget/rounding unavailable",0x2d9190u);
   source_=std::move(actual);mesh_.vertices.resize(resources::vertexCount);mesh_.indices.reserve(resources::indexCount);
   const auto& surface=source_->scene().lods[0].surfaces[0];
   for(std::size_t i=0;i<resources::vertexCount;++i){const auto& original=surface.vertices[i];auto& v=vertices_[i];
    for(unsigned k=0;k<3;++k)v.positionLocal[k]=original.position_local[k];v.paletteIndex=original.bone;localNormals_[i]=original.normal_local;
    for(unsigned k=0;k<2;++k)nextVertices_[i].uv[k]=static_cast<float>(original.uv[k]);
    for(unsigned k=0;k<4;++k)nextVertices_[i].color[k]=original.color[k];
   }
   mesh_.indices.assign(surface.indices.begin(),surface.indices.end());
   const auto actualPayload=sizeof(Owner)+mesh_.vertices.capacity()*sizeof(a::Vertex48B)+mesh_.indices.capacity()*sizeof(std::uint32_t);
   if(actualPayload>availableCpuPayloadBytes)return fail(d,"TC actual gate retained payload exceeds remaining budget",0u);
   if(!install(d))return false;
   initialShared_=&shared;initialSamplePending_=true;d={};return true;
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
 // Preserved source-only numerical checker entry. It creates no observer
 // or visibility authority. Actual root uses beginPrepare -> Binding.prepare.
 bool prepare(std::shared_ptr<const resources::SourceOwner> actual,g::ModelMotionSharedState49T& shared,
      std::size_t availableCpuPayloadBytes,Diagnostic& d){
  return beginPrepare(std::move(actual),shared,availableCpuPayloadBytes,d)&&completeInitialSample(shared,d);
 }
 bool activateSequence(std::size_t owner)noexcept {
  if(!ready()||servicing_||owner!=0)return false;
  sequence_.active90=true;sequence_.nextSequenceA0=sequence_.nextMotionA8=-1;return true;
 }
 // actualWorldDelta30 is the real world-service scaled delta, not MCF FPS.
 // Call only at actual enabled101860 service, outside all query/Bank borrows.
 bool service(float actualWorldDelta30,std::uint32_t sourceTimestamp,std::uint32_t& raw,
      g::SharedRandomOwner50ID& random,g::ModelMotionSharedState49T& motion,Diagnostic& d){
  d={};if(!ready()||servicing_||!std::isfinite(actualWorldDelta30)||actualWorldDelta30<0.f||std::fegetround()!=FE_TONEAREST){d.message="TC actual animation service admission";return false;}
  servicing_=true;struct End{bool& b;~End(){b=false;}}end{servicing_};
  try{
   // Same process RNG/lazy owners as the other actual selectors. Keep reached
   // stores on later failure. This is not GC's older whole-service staging.
   if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
   if(!random.seed.seed3D6B30){random.seed.seed3D6B30=sourceTimestamp;raw=sourceTimestamp;}
   raw=raw*0x41c64e6du+0x3039u;
   volatile float converted=static_cast<float>(raw&0x7fffffffu);volatile float unit=converted*0x1p-31f;volatile float percent=101.f*unit;
   if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
   if(!std::isfinite(percent)||percent<0.f||percent>101.f)return fail(d,"TC animation RNG conversion rejected",0x101914u);
   // Existing EE conversion policy from49K/50ID/50IW: CVT.W.S truncates,
   // separately from the admitted nearest-even binary32 arithmetic.
   const auto selected=static_cast<std::uint32_t>(percent);
   if(random.range.rangeCheck3D6B38&&selected>=101u)return fail(d,"TC actual animation RNG range rejected",0x101944u);
   return advance(actualWorldDelta30,motion,d);
  }catch(const std::exception& e){failed_=true;d.message=e.what();return false;}
 }
 bool ready()const noexcept{return ready_&&!failed_&&thread_==std::this_thread::get_id()&&source_&&source_->ready();}
 bool failed()const noexcept{return failed_;}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return source_?source_->coldSource():nullptr;}
 const resources::SourceOwner* sourceOwner()const noexcept{return source_.get();}
 const resources::SourceOwner& source()const{return *source_;}
 const Sequence& sequence()const noexcept{return sequence_;}
 const std::array<g::SceneNode49P,resources::sceneNodeCount>& nodes()const noexcept{return nodes_;}
 const a::Mesh48B& mesh()const noexcept{return mesh_;}
 const std::array<a::WorldNormal50HK,resources::vertexCount>& normals()const noexcept{return normals_;}
 const std::array<Span,resources::surfaceCount>& spans()const noexcept{return spans_;}
};
}
