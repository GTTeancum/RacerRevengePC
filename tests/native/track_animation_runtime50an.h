#pragma once
// Common track (scenery) animation RUNTIME: one player for every course and every row.
// Piece 3 of work\scratch\track-data-gaps\anim\ANIM.md, ported from SLUS_202.68:
//   Create 2D9060   placement (Offset), initial sequence 2D9418, motion set 155800, time-0
//                   sample 154480, character stopped (char+0x10=1), collision refresh 120230
//   Tick   101860   pause gate (*382B80)+0x11C, ONE shared 0..100 random per tick (101A50),
//                   dt = scale(+0x34, 1.0) * frame time, then every row in table order:
//          101390   near-end test, pending sequence/position choice (SequenceActive search,
//                   LoopSequence, ProbabilityList against the shared random), commit when the
//                   character is stopped or (near end and the next motion differs), then
//                   advance: wrapped = (time+dt > duration) && looping (151160 + motion+0xB4),
//                   elapsed += dt, 154480 (time += dt, subtract once when looping, sample,
//                   stop at end when not), collision refreshed every played frame (11AD80),
//                   re-homed to StartingZone when a looping motion wraps outside it (11ADC0/11AD20)
//   Fire   101170   SequenceActive[0]=1, pending=-1, pending position=-1
//   Hide   1011A0   character object slot +0x1C [I: clears the +0x29 bit the tick tests]
//   Reset  101DA0   (time clamped to >= 1e-4, force, restore saved SequenceActive) ; 1011D0 calls
//                   it with time 0 for a ResetAnimation action
//   Race   2702E0 -> 101D10 (ResetModel(force, restore) on every row) then 101B20 on every
//                   Animation-action target (its sequence 1 waits for its trigger again)
// Cross-motion blend 155820 is NOT reproduced: a commit to a different motion restarts it like
// 155800 (PORT STAND-IN, flagged; no stock row has two motions, so stock playback is exact).
//
// PORT EXTENSIONS, NOT ORIGINAL (N64 data only, inert on stock data; see
// work\scratch\n64-hazards\NOTES.md section 4, items 3a-3d):
//   (a) per-row signed playback rate, set-time, stop-at-end without loop
//   (b) hide that also unregisters the row's collision (and show that re-registers it)
//   (c) StartHidden row key (reader) and a per-frame world transform override with scale
//   (d) per-row alpha fade: drawn through the dynamic window's per-span alphas
//       (frame.dynamicWorld.spanAlphas, source-alpha blended in the transparent pass).
// All of these are exposed through Control below, the one small interface trigger owners call.
//
// Visual vertices are projected once per shown frame in prepareDraw() from the node worlds the
// step left behind (the original transforms vertices when drawing, 241280 -> VU).
#include "track_animation_source50an.h"
#include "../../src/assets/world_normals50hk.h"
#include "../../src/assets/native_asset48b.h"
#include "../../src/game/model_motion49t.h"
#include "../../src/game/rigid_vertices49s.h"
#include "../../src/game/shared_random_owner50id.h"
#include "../../src/game/compound_collision50ij.h"
#include <cmath>
#include <iostream>
#include <sstream>

namespace track_animation_runtime50AN {
namespace a=rr::assets;namespace g=rr::game;namespace src=track_animation_source50AN;
using Index=g::SceneNodeIndex49P;
// Native registry IDs of the rows' CHARACTER collision candidates (one namespace for every
// course; not claimed original bit positions or guest addresses). Row k with collision = +k.
inline constexpr std::uint32_t firstCollisionIdentity=0x20000000u,firstModelIdentity=0x21000000u;

// The one interface trigger owners call (Animation / ResetAnimation / HideAnimation actions,
// and the N64 BreakObject / AnimationToggle actions). Rows are 0-based table rows.
// trigger: 1-based authored trigger number for the log line, 0 when unknown.
class Control {
public:
 virtual ~Control()=default;
 virtual bool ready()const noexcept=0;
 virtual std::size_t rowCount()const noexcept=0;
 // 2DA190 target rule: the LAST row whose ReferenceName equals name (strcmp), -1 when none.
 virtual std::int64_t findReference(const std::string& name)const noexcept=0;
 virtual bool fire(std::size_t row,std::uint32_t trigger=0)noexcept=0;   // 101170
 virtual bool reset(std::size_t row,std::uint32_t trigger=0)noexcept=0;  // 1011D0 -> 101DA0(0,force,restore)
 // 1011A0; unregisterCollision is PORT EXTENSION (b): false = original hide only.
 virtual bool hide(std::size_t row,bool unregisterCollision=false)noexcept=0;
 // PORT EXTENSIONS, NOT ORIGINAL:
 virtual bool show(std::size_t row)noexcept=0;                                   // (b)/(c) undo hide
 virtual bool setRate(std::size_t row,float rate,bool loop,bool stopAtEnd=true)noexcept=0; // (a)
 virtual bool clearRate(std::size_t row)noexcept=0;                              // (a) back to 101390
 virtual bool setTime(std::size_t row,float time)noexcept=0;                     // (a)
 virtual bool setTransformOverride(std::size_t row,const g::BodyMatrix48G* world)noexcept=0; // (c) null clears
 virtual bool setAlpha(std::size_t row,float alpha)noexcept=0;                   // (d) 0..1
};

struct Span {std::uint32_t vertexStart=0,vertexCount=0,indexStart=0,indexCount=0,row=0,material=0;};
struct Collision {
 std::vector<g::CompoundCollisionPart50IJ> parts;std::vector<Index> boundNodes;
 g::BodyMatrix48G ownerWorld{};g::Vector3_48C center{},half{},sphereCenter{};float radius=0.f;
 g::CompoundCollisionView50IJ view{};
};
struct RowState {
 std::vector<g::SceneNode49P> nodes;
 // KeyFramedModel +0x8C..+0xB0
 std::vector<std::uint8_t> saved8C,active90;
 std::uint32_t initial98=0,current9C=0;std::int32_t pendingA0=-1,positionA4=0,pendingPositionA8=-1;
 bool nextDiffersAC=false;float elapsedB0=0.f;
 // LpCharacter: current motion (library index), stopped +0x10, time +0x34, node enabled +0x29 bit0
 std::uint32_t motion18=0,stopped10=1;float time34=0.f;bool enabled29=true;
 // Collision
 Collision collision;bool hasCollision=false,collisionWanted=true;
 bool rehomePending=false;           // set when a looping motion wrapped (11ADC0/11AD20 check)
 bool collisionDirty=true;           // the step moved this row; the binder refreshes membership
 bool visualPending=true;            // prepareDraw projects it
 // PORT EXTENSIONS, NOT ORIGINAL
 bool rateOverride=false,rateLoop=false,rateStopAtEnd=true;float rate=1.f;bool atEnd=false;
 bool transformOverride=false,overrideScaled=false;g::BodyMatrix48G overrideWorld{};
 float alpha=1.f;
 // Log state
 std::uint64_t startedAtTick=0;std::uint32_t startedByTrigger=0,pendingTrigger=0;bool everStarted=false;
 std::uint32_t vertexStart=0,vertexCount=0;
};
struct Diagnostic {std::string message;std::size_t row=0;std::uint32_t sourcePC=0;
 g::ModelMotionDiagnostic49T motion{};g::RigidVertexDiagnostic49S projection{};};

namespace detail {
inline g::BodyMatrix48G identity()noexcept{g::BodyMatrix48G m{};for(unsigned i=0;i<16;i+=5)m.v[i]=1.f;return m;}
inline float add(float x,float y)noexcept{volatile float r=x+y;return r;}
inline float sub(float x,float y)noexcept{volatile float r=x-y;return r;}
inline float mul(float x,float y)noexcept{volatile float r=x*y;return r;}
inline bool unitScale(const g::BodyMatrix48G& m)noexcept{
 for(unsigned r=0;r<3;++r){const float l=m.v[r*4]*m.v[r*4]+m.v[r*4+1]*m.v[r*4+1]+m.v[r*4+2]*m.v[r*4+2];if(std::fabs(l-1.f)>1e-4f)return false;}return true;}
} // namespace detail

class Owner final:public Control {
 std::shared_ptr<const src::Owner> source_;
 std::string label_;
 std::vector<RowState> rows_;
 a::Mesh48B mesh_{};std::vector<a::WorldNormal50HK> normals_;std::vector<Span> spans_;
 std::vector<g::RigidVertex49S> vertices_;std::vector<std::array<double,3>> localNormals_;std::vector<g::RigidPosition49S> projected_;
 std::vector<std::uint8_t> rowTriggerTargets_; // row is an Animation-action target (101B20 at race reset)
 std::vector<g::BodyMatrix48G> palette_;        // draw scratch, sized at create to the largest PSG (no per-frame allocation)
 g::ModelMotionWorkspace49T workspace_{};
 g::ModelMotionSharedState49T* shared_=nullptr;
 std::uint64_t ticks_=0;
 src::Extract extract_;
 bool attempted_=false,ready_=false,failed_=false,fadeLogged_=false,blendLogged_=false;
 bool fail(Diagnostic& d,const std::string& text,std::uint32_t pc){failed_=true;ready_=false;d.sourcePC=pc;try{d.message=text;}catch(...){}return false;}
 const src::Row& sourceRow(std::size_t i)const{return source_->row(i);}
 const src::Motion& motion(std::uint32_t index)const{return source_->motion(index);}
 g::SceneGraph49P graph(std::size_t i)noexcept{return {rows_[i].nodes.data(),static_cast<std::uint32_t>(rows_[i].nodes.size())};}
 static void enterUpdate(g::ModelMotionSharedState49T& shared)noexcept{
  if(!shared.updateGuard3D5DBC){shared.updateValue433468=0xffffffffu;shared.updateGuard3D5DBC=1;}}
 // 2D9590: motion of the current sequence/position; 2D95D0: motion of the pending ones.
 std::uint32_t motionAt(std::size_t i,std::uint32_t sequence,std::int32_t position)const{
  const auto& row=sourceRow(i);const auto& q=row.authored.sequences.at(sequence);
  const auto p=static_cast<std::size_t>(std::max<std::int32_t>(position,0));
  return row.motions.at(q.motions.at(std::min(p,q.motions.size()-1))-1);
 }
 bool install(std::size_t i,Diagnostic& d){
  const auto& row=sourceRow(i);auto& s=rows_[i];
  s.nodes.assign(row.graphCount,g::SceneNode49P{});
  for(auto& n:s.nodes){n={};n.localC0=n.world100=detail::identity();}s.nodes[0].sceneFlags190=0x98;
  for(std::uint32_t r=0;r<row.hierarchy.nodeCount;++r){a::PsgNode49O original{};
   if(!a::readPsgNode49O(row.hierarchy,r,original))return fail(d,"track animation PSG hierarchy row rejected",0x2d9214);
   const auto target=r+1,parent=original.parent<0?0u:static_cast<unsigned>(original.parent)+1;
   if(parent>=target)return fail(d,"track animation PSG parent order rejected",0x2d9214);
   auto& n=s.nodes[target];std::memcpy(n.localC0.v,original.local,sizeof original.local);n.sceneFlags190=0xa4;n.parent180=static_cast<Index>(parent);
   auto& first=s.nodes[parent].firstChild184;
   if(first==Index::none)first=static_cast<Index>(target);
   else{auto previous=static_cast<unsigned>(first);while(s.nodes[previous].nextSibling188!=Index::none)previous=static_cast<unsigned>(s.nodes[previous].nextSibling188);s.nodes[previous].nextSibling188=static_cast<Index>(target);}
  }
  if(!g::validateSceneGraph49P(graph(i)))return fail(d,"track animation scene graph rejected",0x2d9214);
  // 2D9324: position = Offset (orientation from angles is identity: the source refuses non-zero angles).
  g::BodyMatrix48G placed=detail::identity();for(unsigned k=0;k<3;++k)placed.v[12+k]=row.authored.offset[k];
  if(!g::setLocalMatrix49P(graph(i),static_cast<Index>(0),placed))return fail(d,"track animation placement rejected",0x2d9324);
  s.hasCollision=row.authored.useCollision&&!row.collision.members.empty();
  if(s.hasCollision){auto& c=s.collision;const auto n=row.collision.members.size();
   c.parts.assign(n,{});c.boundNodes.assign(n,Index::none);
   if(!g::bindCompoundSceneNodes50IJ(row.collision,graph(i),static_cast<Index>(0),c.boundNodes.data(),n))
    return fail(d,"track animation compound node map rejected",0x126450);
   for(std::size_t p=0;p<n;++p){auto& part=c.parts[p];part.mesh=&row.collision.members[p].mesh;part.active=1;
    part.nodeRadii=row.radii[p].data();part.nodeRadiusCount=row.radii[p].size();part.maxDepth=row.depth[p];}
  }
  // 2D94D4 saved copy, 2D9418 initial sequence.
  for(const bool v:row.authored.initialActive){s.saved8C.push_back(v?1u:0u);}
  s.active90=s.saved8C;s.initial98=row.authored.initialSequence;s.current9C=s.initial98;
  s.positionA4=0;s.pendingA0=-1;s.pendingPositionA8=-1;
  s.motion18=motionAt(i,s.current9C,0);
  s.enabled29=!row.authored.startHidden;              // PORT EXTENSION (c), false on stock
  s.collisionWanted=!row.authored.startHidden;
  return true;
 }
 bool sample(std::size_t i,float time,Diagnostic& d){
  auto& s=rows_[i];const auto& m=motion(s.motion18);enterUpdate(*shared_);
  if(!g::sampleModelMotionTime50IG(m.view,sourceRow(i).bindings,time,graph(i),*shared_,workspace_,d.motion))
   return fail(d,"track animation motion sample rejected",0x154480);
  s.visualPending=true;s.collisionDirty=true;return true;
 }
 // 155800 start: motion set, time 0, playing.
 void start(std::size_t i,std::uint32_t m)noexcept{auto& s=rows_[i];s.motion18=m;s.time34=0.f;s.stopped10=0;}
 // 154480 advance (the port's direct-time route): time += dt, subtract duration once when the
 // motion loops, sample, stop at end (time back to 0) when it does not.
 bool advanceCharacter(std::size_t i,float dt,Diagnostic& d){
  auto& s=rows_[i];const auto& m=motion(s.motion18);
  s.time34=detail::add(s.time34,dt);
  if(!std::isfinite(s.time34))return fail(d,"track animation motion time not finite",0x154480);
  if(s.time34>m.view.duration&&m.looping)s.time34=detail::sub(s.time34,m.view.duration);
  if(!sample(i,s.time34,d))return false;
  if(!m.looping&&s.time34>m.view.duration){s.stopped10=1;s.time34=0.f;}
  return true;
 }
 // PORT EXTENSION (a): signed rate, optional loop, stop-at-end that HOLDS the end pose.
 bool advanceOverride(std::size_t i,float dt,Diagnostic& d){
  auto& s=rows_[i];const auto& m=motion(s.motion18);const float duration=m.view.duration;
  if(s.atEnd&&s.rateStopAtEnd&&!s.rateLoop)return true;
  float t=detail::add(s.time34,detail::mul(dt,s.rate));
  if(!std::isfinite(t))return fail(d,"track animation override time not finite",0x154480);
  if(s.rateLoop&&duration>0.f){while(t>duration)t=detail::sub(t,duration);while(t<0.f)t=detail::add(t,duration);}
  else if(t<=0.f){t=0.f;s.atEnd=s.rate<0.f;}else if(t>=duration){t=duration;s.atEnd=s.rate>0.f;}
  s.time34=t;return sample(i,t,d);
 }
 bool refreshCollision(std::size_t i,Diagnostic& d){
  auto& s=rows_[i];if(!s.hasCollision)return true;auto& c=s.collision;const auto& row=sourceRow(i);
  if(!g::refreshSceneWorld49P(graph(i),static_cast<Index>(0)))return fail(d,"track animation compound owner refresh rejected",0x11f680);
  c.ownerWorld=s.nodes[0].world100;
  for(std::size_t p=0;p<c.parts.size();++p){
   if(!g::refreshSceneWorld49P(graph(i),c.boundNodes[p]))return fail(d,"track animation compound child refresh rejected",0x11f8bc);
   std::array<float,16> world{},composed{};std::memcpy(world.data(),s.nodes[static_cast<unsigned>(c.boundNodes[p])].world100.v,sizeof(float)*16);
   if(!a::composeCollisionWorld48I(row.collision.members[p].mesh.local_matrix,world,composed))return fail(d,"track animation COL-local composition rejected",0x11f8f0);
   std::memcpy(c.parts[p].worldMatrix.v,composed.data(),sizeof c.parts[p].worldMatrix.v);
  }
  if(!g::compoundBounds50IJ(c.parts.data(),c.parts.size(),c.ownerWorld,c.center,c.half))return fail(d,"track animation compound bounds rejected",0x11e630);
  c.view.parts=c.parts.data();c.view.count=c.parts.size();c.view.ownerWorld=c.ownerWorld;c.view.center=c.center;c.view.half=c.half;
  if(!g::compoundSphere50IJ(c.view,c.sphereCenter,c.radius))return fail(d,"track animation compound sphere rejected",0x11f9b4);
  s.collisionDirty=true;return true;
 }
 void logStart(std::size_t i){auto& s=rows_[i];if(s.everStarted&&s.startedAtTick==ticks_)return;
  s.everStarted=true;s.startedAtTick=ticks_;s.startedByTrigger=s.pendingTrigger;
  std::cerr<<"track_animation_started course="<<label_<<" row="<<(i+1)<<" mcf="<<sourceRow(i).authored.mcfStem
   <<" tick="<<ticks_<<" by=";
  if(s.pendingTrigger)std::cerr<<"trigger"<<s.pendingTrigger;else std::cerr<<"race_start";
  std::cerr<<'\n';
  s.pendingTrigger=0;
 }
 // 101390 for one row (random = the shared 0..100 draw of this tick).
 bool tickRow(std::size_t i,float dt,std::int32_t random,Diagnostic& d){
  auto& s=rows_[i];const auto& row=sourceRow(i);const auto& seqs=row.authored.sequences;
  const auto count=[&](std::uint32_t q){return static_cast<std::int32_t>(seqs.at(q).count);};
  // 1013B8..101420: near end (duration - elapsed < blend window motion+0xD4), false on the last
  // motion of a non-looping sequence. PORT STAND-IN (flagged): the blend window +0xD4 is not
  // recovered, so "near end" is never true and only the stopped-character commit runs. Exact for
  // every stock row (one motion per sequence: the next motion never differs).
  const bool nearEnd=false;
  // 101424..10160C: choose the pending sequence/position once per motion.
  if(s.pendingA0==-1){
   s.pendingA0=static_cast<std::int32_t>(s.current9C);
   const auto start=static_cast<std::size_t>(s.pendingA0);std::size_t a1=start;
   if(!s.active90[a1]){
    for(;;){if(s.active90[a1])break;++a1;if(a1>=seqs.size())a1=0;if(a1==start)break;}
    if(s.active90[a1]==1){s.pendingA0=static_cast<std::int32_t>(a1);s.pendingPositionA8=-1;}
   }
   if(s.active90[static_cast<std::size_t>(s.pendingA0)]!=1){s.pendingA0=-1;s.pendingPositionA8=-1;}
   else{
    bool accepted=false;
    for(int guard=0;!accepted;++guard){
     if(guard>4096)return fail(d,"track animation sequence choice did not terminate",0x1014e8);
     ++s.pendingPositionA8;
     if(s.pendingPositionA8>=count(s.current9C)){
      const auto p=static_cast<std::size_t>(s.pendingA0);
      if(seqs[p].loop)s.pendingPositionA8=0;
      else{s.active90[p]=0;accepted=true;--s.pendingPositionA8;s.pendingA0=-1;}
     }
     if(!accepted){const auto p=static_cast<std::size_t>(s.pendingA0);
      const auto& prob=seqs[p].probability;const auto k=static_cast<std::size_t>(s.pendingPositionA8);
      const std::int32_t value=k<prob.size()?prob[k]:100;
      accepted=value==100||random<value;}
    }
    // 1015B8: the pending row may be -1 here (non-looping end); the original then reads the word
    // before the array (not 1 in practice), i.e. no motion comparison.
    if(s.pendingA0>=0&&s.active90[static_cast<std::size_t>(s.pendingA0)]==1)
     s.nextDiffersAC=motionAt(i,static_cast<std::uint32_t>(s.pendingA0),s.pendingPositionA8)!=motionAt(i,s.current9C,s.positionA4);
   }
  }
  // 101614..1016C4: commit.
  if(s.enabled29&&(s.stopped10||(nearEnd&&s.nextDiffersAC))&&s.pendingA0>=0&&s.active90[static_cast<std::size_t>(s.pendingA0)]==1){
   s.positionA4=s.pendingPositionA8;s.current9C=static_cast<std::uint32_t>(s.pendingA0);s.pendingA0=-1;s.elapsedB0=0.f;
   const auto next=motionAt(i,s.current9C,s.positionA4);
   if(s.nextDiffersAC&&!blendLogged_){blendLogged_=true;
    std::cerr<<"track_animation_blend course="<<label_<<" row="<<(i+1)<<" PORT STAND-IN: 155820 cross-motion blend not reproduced; restarted\n";}
   start(i,next);s.atEnd=false;logStart(i);
  }
  // 1016C8..101794: advance.
  if(!s.enabled29||s.stopped10)return true;
  if(s.rateOverride){s.elapsedB0=detail::add(s.elapsedB0,dt);return advanceOverride(i,dt,d)&&refreshCollision(i,d);}
  const auto& m=motion(s.motion18);
  const bool wrapped=detail::add(s.time34,dt)>m.view.duration&&m.looping;
  s.elapsedB0=detail::add(s.elapsedB0,dt);
  if(!advanceCharacter(i,dt,d))return false;
  if(wrapped)s.rehomePending=true;
  return refreshCollision(i,d);
 }
 // 101DA0
 bool resetRow(std::size_t i,float time,bool force,bool restore,Diagnostic& d){
  auto& s=rows_[i];if(time<1e-4f)time=1e-4f;
  if(s.stopped10||force){ // 101E0C..101E18: a stopped character, or force
   s.positionA4=0;
   if(restore){s.current9C=s.initial98;s.active90=s.saved8C;}
   s.pendingA0=-1;s.pendingPositionA8=-1;
   start(i,motionAt(i,s.current9C,0));s.time34=0.f;s.elapsedB0=time;s.atEnd=false;
   if(s.rateOverride){if(!advanceOverride(i,time,d))return false;}else if(!advanceCharacter(i,time,d))return false;
   s.rehomePending=true;if(!refreshCollision(i,d))return false;
  }
  if(!sourceRow(i).authored.sequences[s.current9C].loop&&!restore){s.stopped10=1;s.active90[s.current9C]=0;}
  if(!s.active90[s.current9C]&&restore)s.stopped10=1;
  return true;
 }
 bool valid(std::size_t row)const noexcept{return ready_&&!failed_&&row<rows_.size();}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 // Create 2D9060 for every row, plus the one dynamic mesh. label = the course code for log lines.
 bool create(std::shared_ptr<const src::Owner> source,g::ModelMotionSharedState49T& shared,const std::string& label,Diagnostic& d){
  d={};if(attempted_)return fail(d,"track animation runtime requires a fresh owner",0x2d9060);attempted_=true;
  try{
   if(!source||!source->ready())return fail(d,"track animation runtime without a ready source",0x2d9060);
   source_=std::move(source);shared_=&shared;label_=label;
   const auto rows=source_->rowCount();rows_.assign(rows,RowState{});rowTriggerTargets_.assign(rows,0);
   for(const auto& action:source_->table().actions)
    if(action.kind==a::TrackAnimationActionKind50AN::animation&&action.target<rows)rowTriggerTargets_[action.target]=1;
   mesh_.vertices.reserve(source_->vertexCount());mesh_.indices.reserve(source_->indexCount());
   normals_.assign(source_->vertexCount(),{});vertices_.reserve(source_->vertexCount());localNormals_.reserve(source_->vertexCount());
   projected_.assign(source_->vertexCount(),{});spans_.reserve(source_->spanCount());
   {std::size_t nodes=0;for(std::size_t i=0;i<rows;++i)nodes=std::max<std::size_t>(nodes,source_->row(i).hierarchy.nodeCount);palette_.assign(nodes,{});}
   for(std::size_t i=0;i<rows;++i){d.row=i;const auto& row=sourceRow(i);auto& s=rows_[i];
    s.vertexStart=static_cast<std::uint32_t>(mesh_.vertices.size());
    for(const auto& surface:row.scene.lods[0].surfaces){
     Span span{static_cast<std::uint32_t>(mesh_.vertices.size()),static_cast<std::uint32_t>(surface.vertices.size()),
      static_cast<std::uint32_t>(mesh_.indices.size()),static_cast<std::uint32_t>(surface.indices.size()),static_cast<std::uint32_t>(i),surface.material_index};
     for(const auto& original:surface.vertices){
      g::RigidVertex49S v{};for(unsigned k=0;k<3;++k)v.positionLocal[k]=original.position_local[k];v.paletteIndex=original.bone;
      vertices_.push_back(v);localNormals_.push_back(original.normal_local);
      a::Vertex48B shown{};for(unsigned k=0;k<2;++k)shown.uv[k]=static_cast<float>(original.uv[k]);
      // Absent source RGB is the port's neutral 255 multiplier (as every per-course owner did).
      for(unsigned k=0;k<4;++k)shown.color[k]=surface.has_original_colors?original.color[k]:255;
      mesh_.vertices.push_back(shown);
     }
     for(const auto x:surface.indices)mesh_.indices.push_back(span.vertexStart+x);
     spans_.push_back(span);
    }
    s.vertexCount=static_cast<std::uint32_t>(mesh_.vertices.size())-s.vertexStart;
    if(!install(i,d))return false;
    // 155800 + 154480(0): time-0 pose; char+0x10 = 1 (stopped) so the first enabled tick starts
    // an active sequence (101614 commit), exactly as the original's first world service.
    if(!sample(i,0.f,d))return false;rows_[i].stopped10=1;rows_[i].time34=0.f;
    if(!refreshCollision(i,d))return false;
   }
   if(mesh_.vertices.size()!=source_->vertexCount()||mesh_.indices.size()!=source_->indexCount()||spans_.size()!=source_->spanCount())
    return fail(d,"track animation mesh extent differs from the source counts",0);
   ready_=true;Diagnostic draw{};if(!prepareDraw(draw)){d=draw;return false;}
   d={};return true;
  }catch(const std::exception& e){return fail(d,e.what(),0x2d9060);}
 }
 // 101860. delta = the step's frame time; paused = (*382B80)+0x11C. One shared random per tick.
 bool tick(float delta,bool paused,std::uint32_t timestamp,std::uint32_t& raw,g::SharedRandomOwner50ID& random,
      g::ModelMotionSharedState49T& shared,Diagnostic& d)noexcept{
  try{d={};
   if(!ready_||failed_||&shared!=shared_||!std::isfinite(delta)||delta<0.f||std::fegetround()!=FE_TONEAREST)
    return fail(d,"track animation tick admission rejected",0x101860);
   if(paused)return true;
   if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
   if(!random.seed.seed3D6B30){random.seed.seed3D6B30=timestamp;raw=timestamp;}
   raw=raw*0x41c64e6du+0x3039u;volatile float converted=static_cast<float>(raw&0x7fffffffu);
   volatile float unit=converted*0x1p-31f;volatile float percent=101.f*unit;
   if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
   if(!std::isfinite(percent)||percent<0.f||percent>101.f)return fail(d,"track animation random conversion rejected",0x101914);
   const auto selected=static_cast<std::int32_t>(percent);
   if(random.range.rangeCheck3D6B38&&(selected<0||selected>=101))return fail(d,"track animation random range rejected",0x101944);
   const float dt=detail::mul(1.f,delta); // +0x34 time scale, 1.0 from 101330
   ++ticks_;
   for(std::size_t i=0;i<rows_.size();++i){d.row=i;if(!tickRow(i,dt,selected,d))return false;}
   return true;
  }catch(const std::exception& e){return fail(d,e.what(),0x101860);}
 }
 // 2702E0 -> 101D10 + 101B20.
 bool raceReset(Diagnostic& d)noexcept{
  try{d={};if(!ready_||failed_)return fail(d,"track animation race reset outside a ready owner",0x2702e0);
   for(std::size_t i=0;i<rows_.size();++i){d.row=i;if(!resetRow(i,0.f,true,true,d))return false;}
   for(std::size_t i=0;i<rows_.size();++i)if(rowTriggerTargets_[i]){auto& s=rows_[i];
    if(!s.active90.empty())s.active90[0]=0;s.pendingA0=-1;s.pendingPositionA8=-1;}
   return true;
  }catch(const std::exception& e){return fail(d,e.what(),0x2702e0);}
 }
 // Once per shown frame, before the dynamic window binds the mesh.
 bool prepareDraw(Diagnostic& d)noexcept{
  try{d={};if(!ready_||failed_)return fail(d,"track animation draw outside a ready owner",0x241280);
   bool any=false;
   for(std::size_t i=0;i<rows_.size();++i){auto& s=rows_[i];if(!s.visualPending)continue;d.row=i;
    const auto& row=sourceRow(i);const auto start=s.vertexStart,count=s.vertexCount;
    // (d) a faded row stays projected; the window hands its alpha per span to the
    // dynamic-world draw (frame.dynamicWorld.spanAlphas). Alpha 0 = not drawn.
    const bool shown=s.enabled29&&s.alpha>0.f;
    if(s.alpha<1.f&&!fadeLogged_){fadeLogged_=true;
     std::cerr<<"track_animation_fade course="<<label_<<" row="<<(i+1)<<" PORT EXTENSION: alpha fade drawn through the dynamic-world span alphas\n";}
    if(row.hierarchy.nodeCount>palette_.size())return fail(d,"track animation draw palette extent",0x241280);
    auto* palette=palette_.data();
    for(std::uint32_t r=0;r<row.hierarchy.nodeCount;++r){
     if(!g::refreshSceneWorld49P(graph(i),static_cast<Index>(r+1)))return fail(d,"track animation visual world rejected",0x2897b0);
     palette[r]=s.nodes[r+1].world100;
     if(s.transformOverride){ // (c) the override replaces the model root's world: re-base each node
      const auto& root=s.nodes[0].world100;const auto& o=s.overrideWorld;g::BodyMatrix48G inv{};
      // world_node = local_to_root * override, local_to_root = world_node * inverse(root) (affine, row vectors)
      const float* m=root.v;const float det=m[0]*(m[5]*m[10]-m[6]*m[9])-m[1]*(m[4]*m[10]-m[6]*m[8])+m[2]*(m[4]*m[9]-m[5]*m[8]);
      if(!std::isfinite(det)||det==0.f)return fail(d,"track animation override root not invertible",0);
      const float id=1.f/det;
      inv.v[0]=(m[5]*m[10]-m[6]*m[9])*id;inv.v[1]=(m[2]*m[9]-m[1]*m[10])*id;inv.v[2]=(m[1]*m[6]-m[2]*m[5])*id;
      inv.v[4]=(m[6]*m[8]-m[4]*m[10])*id;inv.v[5]=(m[0]*m[10]-m[2]*m[8])*id;inv.v[6]=(m[2]*m[4]-m[0]*m[6])*id;
      inv.v[8]=(m[4]*m[9]-m[5]*m[8])*id;inv.v[9]=(m[1]*m[8]-m[0]*m[9])*id;inv.v[10]=(m[0]*m[5]-m[1]*m[4])*id;
      for(unsigned c=0;c<3;++c)inv.v[12+c]=-(m[12]*inv.v[c]+m[13]*inv.v[4+c]+m[14]*inv.v[8+c]);inv.v[15]=1.f;
      auto mulm=[](const g::BodyMatrix48G& x,const g::BodyMatrix48G& y){g::BodyMatrix48G r{};
       for(unsigned a2=0;a2<4;++a2)for(unsigned b=0;b<4;++b){float v=0;for(unsigned k=0;k<4;++k)v+=x.v[a2*4+k]*y.v[k*4+b];r.v[a2*4+b]=v;}return r;};
      palette[r]=mulm(mulm(palette[r],inv),o);
     }
    }
    if(shown){
     if(!g::projectRigidVertices49S({vertices_.data()+start,count},{palette,row.hierarchy.nodeCount},
         {projected_.data()+start,count},d.projection))return fail(d,"track animation rigid projection rejected",0x241280);
     for(std::size_t j=start;j<start+count;++j){std::memcpy(mesh_.vertices[j].position,projected_[j].v,sizeof projected_[j].v);
      const auto& m=palette[vertices_[j].paletteIndex];std::array<double,16> matrix{};for(unsigned k=0;k<16;++k)matrix[k]=m.v[k];
      if(!a::transformWorldNormal50HK(localNormals_[j],matrix,normals_[j]))return fail(d,"track animation normal projection rejected",0x241280);}
    }else{ // hidden or faded: degenerate positions at the model root, zero normals (nothing drawn)
     const auto& root=s.transformOverride?s.overrideWorld:s.nodes[0].world100;
     for(std::size_t j=start;j<start+count;++j){for(unsigned k=0;k<3;++k)mesh_.vertices[j].position[k]=root.v[12+k];normals_[j]={};}
    }
    s.visualPending=false;any=true;
   }
   if(any&&!mesh_.vertices.empty())for(unsigned axis=0;axis<3;++axis){
    mesh_.bounds.minimum[axis]=mesh_.bounds.maximum[axis]=mesh_.vertices[0].position[axis];
    for(const auto& v:mesh_.vertices){mesh_.bounds.minimum[axis]=std::min(mesh_.bounds.minimum[axis],v.position[axis]);
     mesh_.bounds.maximum[axis]=std::max(mesh_.bounds.maximum[axis],v.position[axis]);}}
   return true;
  }catch(...){return fail(d,"track animation draw preparation exception",0x241280);}
 }
 // ---- Control ----
 std::size_t rowCount()const noexcept override{return rows_.size();}
 std::int64_t findReference(const std::string& name)const noexcept override{
  std::int64_t found=-1;if(!source_)return found;
  for(std::size_t i=0;i<rows_.size();++i)if(sourceRow(i).authored.referenceName==name)found=static_cast<std::int64_t>(i);return found;}
 bool fire(std::size_t row,std::uint32_t trigger=0)noexcept override{
  if(!valid(row))return false;auto& s=rows_[row];if(s.active90.empty())return false;
  s.active90[0]=1;s.pendingA0=-1;s.pendingPositionA8=-1;s.pendingTrigger=trigger;return true;}
 bool reset(std::size_t row,std::uint32_t trigger=0)noexcept override{
  if(!valid(row))return false;Diagnostic d{};(void)trigger;
  try{return resetRow(row,0.f,true,true,d);}catch(...){return fail(d,"track animation reset exception",0x1011d0);}}
 bool hide(std::size_t row,bool unregisterCollision=false)noexcept override{
  if(!valid(row))return false;auto& s=rows_[row];s.enabled29=false;s.visualPending=true;
  if(unregisterCollision){s.collisionWanted=false;s.collisionDirty=true;}return true;}
 bool show(std::size_t row)noexcept override{
  if(!valid(row))return false;auto& s=rows_[row];s.enabled29=true;s.visualPending=true;
  if(!s.collisionWanted&&!s.overrideScaled){s.collisionWanted=true;s.collisionDirty=true;}return true;}
 bool setRate(std::size_t row,float rate,bool loop,bool stopAtEnd=true)noexcept override{
  if(!valid(row)||!std::isfinite(rate))return false;auto& s=rows_[row];
  s.rateOverride=true;s.rate=rate;s.rateLoop=loop;s.rateStopAtEnd=stopAtEnd;s.atEnd=false;
  if(s.stopped10)s.stopped10=0; // an overridden row plays under its own rate
  return true;}
 bool clearRate(std::size_t row)noexcept override{if(!valid(row))return false;rows_[row].rateOverride=false;rows_[row].atEnd=false;return true;}
 bool setTime(std::size_t row,float time)noexcept override{
  if(!valid(row)||!std::isfinite(time)||time<0.f)return false;auto& s=rows_[row];const auto& m=motion(s.motion18);
  Diagnostic d{};s.time34=std::min(time,m.view.duration);s.atEnd=false;
  return sample(row,s.time34,d)&&refreshCollision(row,d);}
 bool setTransformOverride(std::size_t row,const g::BodyMatrix48G* world)noexcept override{
  if(!valid(row))return false;auto& s=rows_[row];
  if(!world){s.transformOverride=false;if(s.overrideScaled){s.overrideScaled=false;if(s.enabled29){s.collisionWanted=true;s.collisionDirty=true;}}
   s.visualPending=true;return true;}
  for(float v:world->v)if(!std::isfinite(v))return false;
  s.transformOverride=true;s.overrideWorld=*world;s.visualPending=true;
  // Collision follows only a unit-scale override (compoundSphere50IJ does not rescale);
  // a scaled override unregisters the row's collision while it lasts.
  const bool scaled=!detail::unitScale(*world);
  if(scaled!=s.overrideScaled){s.overrideScaled=scaled;s.collisionWanted=!scaled&&s.enabled29;s.collisionDirty=true;}
  if(!scaled&&s.hasCollision){
   Diagnostic d{};g::BodyMatrix48G local=*world;if(!g::setLocalMatrix49P(graph(row),static_cast<Index>(0),local))return false;
   s.transformOverride=false; // a unit-scale override IS the root placement
   return refreshCollision(row,d);
  }
  return true;}
 bool setAlpha(std::size_t row,float alpha)noexcept override{
  if(!valid(row)||!std::isfinite(alpha))return false;auto& s=rows_[row];s.alpha=std::clamp(alpha,0.f,1.f);s.visualPending=true;return true;}
 // ---- read access for the window, the collision binder, logs and checks ----
 bool ready()const noexcept override{return ready_&&!failed_;}
 bool failed()const noexcept{return failed_;}
 const std::string& label()const noexcept{return label_;}
 const src::Owner& source()const noexcept{return *source_;}
 std::shared_ptr<const src::Owner> sourceOwner()const noexcept{return source_;}
 const RowState& state(std::size_t i)const{return rows_.at(i);}
 RowState& mutableState(std::size_t i){return rows_.at(i);}
 bool isTriggerTarget(std::size_t i)const{return rowTriggerTargets_.at(i)!=0;}
 const a::Mesh48B& mesh()const noexcept{return mesh_;}
 const std::vector<a::WorldNormal50HK>& normals()const noexcept{return normals_;}
 const std::vector<Span>& spans()const noexcept{return spans_;}
 std::uint64_t ticks()const noexcept{return ticks_;}
 // The course archive's member reader, kept only until the window has loaded its materials.
 void retainExtract(src::Extract extract){extract_=std::move(extract);}
 const src::Extract& extract()const noexcept{return extract_;}
 void releaseExtract()noexcept{extract_=nullptr;}
 // One plain line per course and one per row (muted logs-only tests read them).
 void logTable(std::ostream& out)const{
  const auto& t=source_->table();
  out<<"track_animations course="<<label_<<" table="<<t.animations.size()<<" rows="<<rows_.size()
     <<" vertices="<<mesh_.vertices.size()<<" indices="<<mesh_.indices.size()<<" spans="<<spans_.size()
     <<" motions="<<source_->motionCount()<<" legacy_mot="<<source_->legacyConversions()<<'\n';
  for(std::size_t i=0;i<rows_.size();++i){const auto& r=sourceRow(i);const auto& s=rows_[i];
   std::string start="never";std::uint32_t trigger=0;
   for(const auto& x:t.actions)if(x.kind==a::TrackAnimationActionKind50AN::animation&&x.target==i){trigger=x.trigger;break;}
   if(trigger)start="trigger"+std::to_string(trigger);else if(!s.active90.empty()&&s.active90[s.current9C])start="race_start";
   const auto& m=source_->motion(s.motion18);
   out<<"track_animation_row course="<<label_<<" row="<<(i+1)<<" mcf="<<r.authored.mcfStem<<" created=1"
      <<" zone="<<(r.authored.startingZone+1)<<" collision="<<(s.hasCollision?1:0)<<" parts="<<r.collision.members.size()
      <<" loop="<<(r.authored.sequences.empty()?0:r.authored.sequences[0].loop)<<" motion="<<m.name<<" looping="<<m.looping
      <<" duration="<<m.view.duration<<" start="<<start<<" vertices="<<s.vertexCount
      <<" loop_sounds="<<r.authored.loopSounds.size()<<(r.authored.startHidden?" start_hidden=1":"")<<'\n';
   for(const auto& ls:r.authored.loopSounds)
    out<<"track_animation_loop_sound course="<<label_<<" row="<<(i+1)<<" sound="<<ls.soundResourceFile<<" attach="<<ls.attachToObject
       <<" attached=0 PORT STAND-IN: no common positional loop-sound owner yet\n";
  }
 }
};
} // namespace track_animation_runtime50AN
