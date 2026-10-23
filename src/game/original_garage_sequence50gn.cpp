#include "original_garage_manager50gn.h"
#include "original_garage_sequence50gn.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>
namespace rr::game {namespace {
using Def=GarageActorDefinition50GN;
const std::array<Def,garageActorCount50GN> definitions{{
 {"pd_01.mcf","none","none",1,1,true,false,{"pd_01"},{0x41000000},{1},{100}},
 {"pd_02.mcf","none","none",1,1,true,false,{"pd_02"},{0x41400000},{1},{100}},
 {"pd_03.mcf","none","none",1,1,true,false,{"pd_03"},{0x40c00000},{1},{100}},
 {"pd_bx.mcf","none","none",4,14,true,true,{"pd_bx_1","pd_bx_2","pd_bx_3","pd_bx_4"},
  {0x40400000,0x40400000,0x40600000,0x40000000},
  {1,2,3,4,4,4,4,2,2,1,1,1,1,4},{75,75,50,100,100,100,75,50,75,50,75,50,75,50}},
 {"pd_lt.mcf","none","none",4,14,true,true,{"pd_lt_1","pd_lt_2","pd_lt_3","pd_lt_4"},
  {0x40400000,0x40400000,0x40400000,0x40000000},
  {1,2,3,4,1,3,1,2,1,2,3,1,1,4},{50,50,100,50,50,100,75,50,75,50,75,50,75,50}},
 {"pd_fire.mcf","pd_fire","RUN",1,1,false,false,{"pd_fire"},{0x40333333},{1},{100}},
 {"pd_ext.mcf","pd_ext","RUN",1,1,false,false,{"pd_ext"},{0x40400000},{1},{100}},
 {"tank.mcf","tank","RUN",1,1,false,false,{"tank"},{0x40400000},{1},{100}},
 {"nozzle.mcf","nozzle","RUN",1,1,false,false,{"nozzle"},{0x40400000},{1},{100}},
 {"pd_fall.mcf","pd_fall","FALL",1,1,false,false,{"pd_fall"},{0x414aaaab},{1},{100}}
}};
float add(float a,float b)noexcept{volatile float r=a+b;return r;}
float sub(float a,float b)noexcept{volatile float r=a-b;return r;}
float mul(float a,float b)noexcept{volatile float r=a*b;return r;}
bool validFloat(float f)noexcept{return std::isfinite(f)&&std::fpclassify(f)!=FP_SUBNORMAL;}
std::size_t motionIndex(const Def& d,std::int32_t entry)noexcept{return d.entries[static_cast<std::size_t>(entry)]-1u;}
}
const std::array<Def,garageActorCount50GN>& garageActorDefinitions50GN()noexcept{return definitions;}
bool OriginalGarageSequence50GN::fail(GarageSequenceDiagnostic50GN& d,GarageSequenceError50GN e,std::uint32_t pc,std::size_t actor)noexcept {
 d.error=e;d.sourcePC=pc;d.actor=static_cast<std::uint32_t>(actor);failed_=true;return false;
}
bool OriginalGarageSequence50GN::bind(const std::array<GarageActorBinding50GN,garageActorCount50GN>& b,
 ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,ModelPlaybackWorkspace50KG& work,
 GarageSequenceDiagnostic50GN& d)noexcept {
 d={};if(bound_||initialPoseCount_||failed_||servicing_)return fail(d,GarageSequenceError50GN::alreadyBound,0x2d9060);
 if(std::fegetround()!=FE_TONEAREST)return fail(d,GarageSequenceError50GN::input,0x2d946c);
 // Validate entire borrow set before first real initial-pose sample.
 for(std::size_t i=0;i<b.size();++i){const auto& x=b[i];const auto& a=definitions[i];
  if(!x.resources||!x.resources->loaded153610()||x.resources->count14()!=a.motionCount||!x.playback||!x.rows||!x.ik||x.ik->enabled204||!validateSceneGraph49P(x.graph))
   return fail(d,GarageSequenceError50GN::input,0x2d9060,i);
  for(std::size_t j=0;j<i;++j)if(x.playback==b[j].playback||x.graph.nodes==b[j].graph.nodes)
   return fail(d,GarageSequenceError50GN::input,0x2d9060,i);
  for(unsigned j=0;j<a.motionCount;++j){const auto* r=x.resources->at(j);const auto* m=x.motions[j];
   if(!r||!m||m->motion!=&r->motion()||m->looping||r->fields().loopingB4||r->fields().name00!=a.motionNames[j]||
      std::bit_cast<std::uint32_t>(r->motion().duration)!=a.durationBits[j]||r->fields().transitionTimeD4!=.5f)
    return fail(d,GarageSequenceError50GN::resource,0x2d9384,i);
  }
 }
 for(std::size_t i=0;i<b.size();++i)
  if(!bindNextInitialPose(i,b[i],shared,ikShared,work,d))return false;
 return true;
}
bool OriginalGarageSequence50GN::bindNextInitialPose(std::size_t i,const GarageActorBinding50GN& x,
 ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,ModelPlaybackWorkspace50KG& work,
 GarageSequenceDiagnostic50GN& d)noexcept {
 d={};if(bound_||failed_||servicing_||i!=initialPoseCount_||i>=definitions.size())
  return fail(d,GarageSequenceError50GN::alreadyBound,0x2d9060,i);
 if(std::fegetround()!=FE_TONEAREST)return fail(d,GarageSequenceError50GN::input,0x2d946c,i);
 const auto& a=definitions[i];
 if(!x.resources||!x.resources->loaded153610()||x.resources->count14()!=a.motionCount||!x.playback||!x.rows||!x.ik||x.ik->enabled204||!validateSceneGraph49P(x.graph))
  return fail(d,GarageSequenceError50GN::input,0x2d9060,i);
 for(std::size_t j=0;j<i;++j)if(x.playback==bindings_[j].playback||x.graph.nodes==bindings_[j].graph.nodes)
  return fail(d,GarageSequenceError50GN::input,0x2d9060,i);
 for(unsigned j=0;j<a.motionCount;++j){const auto* r=x.resources->at(j);const auto* m=x.motions[j];
  if(!r||!m||m->motion!=&r->motion()||m->looping||r->fields().loopingB4||r->fields().name00!=a.motionNames[j]||
     std::bit_cast<std::uint32_t>(r->motion().duration)!=a.durationBits[j]||r->fields().transitionTimeD4!=.5f)
   return fail(d,GarageSequenceError50GN::resource,0x2d9384,i);
 }
 bindings_[i]=x;states_[i]={};states_[i].active90=a.initiallyActive;servicing_=true;
 if(!startModelPlayback50KG(*x.motions[0],*x.playback,d.playback)||
    !advanceModelPlayback50KG(0.f,*x.rows,x.graph,*x.ik,*x.playback,shared,ikShared,work,d.playback)){
  servicing_=false;return fail(d,GarageSequenceError50GN::playback,d.playback.sourceStage,i);}
 x.playback->stopped10=true; //2D9490, including inactive authentic actors.
 ++initialPoseCount_;bound_=initialPoseCount_==definitions.size();servicing_=false;return true;
}

bool OriginalGarageSequence50GN::trigger(const char* actor,const char* sequence,GarageSequenceDiagnostic50GN& d)noexcept {
 d={};if(!ready()||servicing_||!actor||!sequence){d.error=GarageSequenceError50GN::input;d.sourcePC=0x101c10;return false;}
 // Only explicit Garage callbacks are admitted, not incidental default "none".
 for(std::size_t i=5;i<definitions.size();++i)if(std::strcmp(actor,definitions[i].referenceName)==0){
  if(std::strcmp(sequence,definitions[i].sequenceName)){d.error=GarageSequenceError50GN::input;d.sourcePC=0x2d9610;return false;}
  auto& q=states_[i];q.active90=true;q.nextA0=q.nextA8=-1;return true;
 }
 d.error=GarageSequenceError50GN::input;d.sourcePC=0x101c10;return false;
}
bool OriginalGarageSequence50GN::service(bool paused,float dt,float scale,
 const std::array<bool,garageActorCount50GN>& enabled,GarageReadTimestamp50GN clock,void* clockContext,
 CoupledNoiseContext49M& noise,SharedRandomOwner50ID& random,ModelMotionSharedState49T& shared,
 ModelMode0IkShared50KG& ikShared,ModelPlaybackWorkspace50KG& work,GarageAfterSample50GN after,void* afterContext,
 GarageSequenceReceipt50GN& receipt,GarageSequenceDiagnostic50GN& d)noexcept {
 return serviceImpl(nullptr,paused,dt,scale,enabled,clock,clockContext,noise,random,
  shared,ikShared,work,after,afterContext,receipt,d);
}
bool OriginalGarageSequence50GN::serviceManager101860(OriginalGarageAnimationManagerFields50GN& manager,
 bool paused,float dt,const std::array<bool,garageActorCount50GN>& enabled,
 GarageReadTimestamp50GN clock,void* clockContext,CoupledNoiseContext49M& noise,
 SharedRandomOwner50ID& random,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,
 ModelPlaybackWorkspace50KG& work,GarageAfterSample50GN after,void* afterContext,
 GarageSequenceReceipt50GN& receipt,GarageSequenceDiagnostic50GN& d)noexcept {
 return serviceImpl(&manager,paused,dt,manager.scale34,enabled,clock,clockContext,noise,random,
  shared,ikShared,work,after,afterContext,receipt,d);
}
bool OriginalGarageSequence50GN::serviceImpl(OriginalGarageAnimationManagerFields50GN* manager,
 bool paused,float dt,float scale,const std::array<bool,garageActorCount50GN>& enabled,
 GarageReadTimestamp50GN clock,void* clockContext,CoupledNoiseContext49M& noise,
 SharedRandomOwner50ID& random,ModelMotionSharedState49T& shared,ModelMode0IkShared50KG& ikShared,
 ModelPlaybackWorkspace50KG& work,GarageAfterSample50GN after,void* afterContext,
 GarageSequenceReceipt50GN& receipt,GarageSequenceDiagnostic50GN& d)noexcept {
 d={};receipt={};if(!ready()||servicing_)return fail(d,GarageSequenceError50GN::state,0x101860);
 if(paused){receipt.paused=true;return true;} //No RNG, callbacks or dt use.
 if(!validFloat(dt)||dt<0||!validFloat(scale)||scale<0||!after||std::fegetround()!=FE_TONEAREST)
  return fail(d,GarageSequenceError50GN::input,0x101860);
 servicing_=true;
 struct Guard {bool& b;~Guard(){b=false;}} guard{servicing_};
 if(!random.seed.lazy80A8){random.seed.seed3D6B30=0;random.seed.lazy80A8=true;}
 if(!random.seed.seed3D6B30){std::uint32_t timestamp=0;
  if(!clock||!clock(clockContext,timestamp))return fail(d,GarageSequenceError50GN::clock,0x1018c8);
  random.seed.seed3D6B30=timestamp;seedOriginalSharedRng50IU(noise,timestamp);receipt.seeded=true;
 }
 receipt.seedCache=random.seed.seed3D6B30;receipt.draw308030=drawOriginalSharedRng50IU(noise);receipt.drew=true;receipt.rawAfter=noise.sharedRandomState;
 // EE positive CVT.S.W truncates low bits beyond24 significant bits. Multiplication
 // by101 and CVT.W.S have the same integer floor as this exact rational: every
 // integer0..101 is representable, so toward-zero binary32 cannot cross it.
 auto quantized=receipt.draw308030;unsigned shift=0;
 for(auto magnitude=quantized;magnitude>=0x01000000u;magnitude>>=1)++shift;
 if(shift)quantized&=~((std::uint32_t{1}<<shift)-1u);
 receipt.percentile=static_cast<std::int32_t>((std::uint64_t{quantized}*101u)>>31);
 if(!random.range.lazy80AC){random.range.rangeCheck3D6B38=1;random.range.lazy80AC=true;}
 if(random.range.rangeCheck3D6B38&&(receipt.percentile<0||receipt.percentile>=101))return fail(d,GarageSequenceError50GN::range,0x101944);
 receipt.delta30=mul(scale,dt);if(!validFloat(receipt.delta30))return fail(d,GarageSequenceError50GN::input,0x1019a0);
 if(manager)manager->delta30=receipt.delta30; // Actual1019A8 publication.
 for(std::size_t i=0;i<states_.size();++i){auto& q=states_[i];const auto& a=definitions[i];const auto& x=bindings_[i];auto& p=*x.playback;auto& out=receipt.actors[i];
  if(!x.resources->loaded153610()||q.current9C!=0||q.currentA4<0||q.currentA4>=a.entryCount||q.nextA0< -1||q.nextA0>0||q.nextA8< -1||q.nextA8>=a.entryCount)
   return fail(d,GarageSequenceError50GN::state,0x101390,i);
  const auto current=motionIndex(a,q.currentA4);
  if(p.current18!=x.motions[current]||!validFloat(q.elapsedB0))return fail(d,GarageSequenceError50GN::state,0x1013b8,i);
  bool transition=sub(p.current18->motion->duration,q.elapsedB0)<x.resources->at(current)->fields().transitionTimeD4;
  if(q.currentA4==a.entryCount-1&&!a.loopSequence)transition=false;
  if(q.nextA0==-1){q.nextA0=0;
   if(!q.active90){q.nextA0=q.nextA8=-1;}
   else {unsigned scanned=0;bool accepted=false;
    while(!accepted){++q.nextA8;
     if(q.nextA8>=a.entryCount){if(a.loopSequence)q.nextA8=0;else{q.active90=false;--q.nextA8;q.nextA0=-1;break;}}
     const auto probability=a.probabilities[static_cast<std::size_t>(q.nextA8)];
     accepted=probability==100||receipt.percentile<probability;
     if(++scanned>a.entryCount)return fail(d,GarageSequenceError50GN::state,0x1015b0,i);
    }
    if(q.nextA0>=0&&q.active90)q.differentAC=x.resources->at(motionIndex(a,q.nextA8))!=x.resources->at(current);
   }
  }
  // Source1015CC/10165C active[-1] reads malloc's size/flag header. Valid
  //305990 chunks>=16, pointer=chunk+8, hence header!=1. Represent that proven
  //predicate as absent candidate; NEVER fabricate zero or read host[-1].
  const bool candidateActive=q.nextA0==0&&q.active90;
  if(enabled[i]&&(p.stopped10||(transition&&q.differentAC))&&candidateActive){
   q.currentA4=q.nextA8;q.current9C=q.nextA0;q.nextA0=-1;q.elapsedB0=0;
   const auto selected=motionIndex(a,q.currentA4);out.started=true;out.transitioned=q.differentAC;
   // The concrete Garage owner has joined the ordinary previous-motion path,
   // not155920's new source allocation1F0. Never let the numeric fixed scratch
   // silently stand in for that missing retained allocation owner.
   if(manager&&q.differentAC&&x.resources->at(selected)->fields().transitionTimeD4!=0.f&&
      (!p.current18||p.blending30))return fail(d,GarageSequenceError50GN::state,0x155894,i);
   const bool okay=q.differentAC?transitionModelPlayback50KG(*x.motions[selected],x.resources->at(selected)->fields().transitionTimeD4,*x.rows,x.graph,p,d.playback):startModelPlayback50KG(*x.motions[selected],p,d.playback);
   if(!okay)return fail(d,GarageSequenceError50GN::playback,d.playback.sourceStage,i);
  }
  if(enabled[i]&&!p.stopped10){q.elapsedB0=add(q.elapsedB0,receipt.delta30);
   if(!validFloat(q.elapsedB0))return fail(d,GarageSequenceError50GN::input,0x101728,i);
   if(!advanceModelPlayback50KG(receipt.delta30,*x.rows,x.graph,*x.ik,p,shared,ikShared,work,d.playback))return fail(d,GarageSequenceError50GN::playback,d.playback.sourceStage,i);
   out.sampled=true;
   if(!after(afterContext,i,x,d)){if(d.error==GarageSequenceError50GN::none)d.error=GarageSequenceError50GN::postSample;
    d.actor=static_cast<std::uint32_t>(i);if(!d.sourcePC)d.sourcePC=0x101788;failed_=true;return false;}
  }
  out.currentEntry=q.currentA4;out.nextEntry=q.nextA8;
 }
 return true;
}
}
