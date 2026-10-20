#pragma once
#include "original_gc_spray50kg.h"
#include "water_wake50jc.h"
#include <cfenv>
#include <cstring>
#include <limits>

namespace rr::game::original_rc_fog_numeric_detail50RC {
// Numerical leaves for the real RC Type12 owner. These do not register an
// emitter or construct source/attachment authority. Callers authenticate the
// immutable definition and supply the same canonical manager row and controls.
using Status=OriginalGcSprayStatus50KG;
using Receipt=OriginalGcSprayReceipt50KG;
using Math=water_wake_detail50jc::Math;
inline float bits(std::uint32_t word)noexcept {float value;std::memcpy(&value,&word,4);return value;}
inline Receipt result(Status status,std::uint32_t pc)noexcept {Receipt r;r.status=status;r.sourcePC=pc;return r;}

//160160 phase is reached BEFORE common15A7A0, including while Application is
// paused. This changes only the actual registered row; it does not advance
// particles. Manager161180 already consumed the preceding published ratio.
inline Receipt phase(OriginalGcParticleEmitterState50KG& e,float dt)noexcept {
 if(std::fegetround()!=FE_TONEAREST)return result(Status::arithmetic,0x160160u);
 Math m;m.read(dt,0x16019cu);m.read(e.fogPeriod,0x160168u);
 if(m.badPC||dt<0)return result(Status::arithmetic,m.badPC?m.badPC:0x16019cu);
 if(e.fogPeriod==-1.f)return result(Status::complete,0x16028cu);
 const auto next=e.fogIncreasing?m.add(e.fogPhase,dt,0x16019cu):m.sub(e.fogPhase,dt,0x1601acu);
 if(m.badPC)return result(Status::arithmetic,m.badPC);e.fogPhase=next;
 const auto ratio=m.div(e.fogPhase,e.fogPeriod,0x1601c4u);
 if(m.badPC)return result(Status::arithmetic,m.badPC);e.fogPhaseRatio=ratio;
 if(e.fogPhase>=0.f){
  if(e.fogPhaseRatio>1.f){e.fogIncreasing=false;
   const auto correction=m.div(e.fogPeriod,1000.f,0x160224u);
   const auto corrected=m.sub(e.fogPeriod,correction,0x16022cu);
   if(m.badPC)return result(Status::arithmetic,m.badPC);e.fogPhase=corrected;
  }
 }else if(e.fogPhaseRatio<-1.f){e.fogIncreasing=true;
  const auto correction=m.div(e.fogPeriod,1000.f,0x16027cu);
  const auto corrected=m.add(-e.fogPeriod,correction,0x160284u);
  if(m.badPC)return result(Status::arithmetic,m.badPC);e.fogPhase=corrected;
 }
 return result(Status::complete,0x16028cu);
}

//160520 preserves source order, including retirement of this actual row's
// preceding particles and publication BEFORE the shared lazy seed/draw10.
// Period=-1 leaves reused velocity words untouched. No private RNG/cache.
template<class Definition>
Receipt populate(const Definition& f,OriginalGcParticleManager50KG& manager,
 std::uint32_t owner,std::int32_t& currentFrame,const EffectControlState50BC& c,
 std::uint32_t& raw,SharedRandomOwner50ID& shared,HostMonotonicClock50ID& clock)noexcept {
 auto r=result(Status::complete,0x160ad4u);Math m;
 auto fail=[&](Status status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
 auto draw=[&](){++r.randomDraws;return drawOriginalGcParticleRandom50KG(raw);};
 const auto* row=manager.emitter(owner);const auto* group=manager.group(f.particleGroup1B8);
 if(!manager.prepared()||!row||!group||row->particleGroup!=f.particleGroup1B8||
    group->first>group->last||currentFrame<group->first||currentFrame>group->last)
  return fail(Status::storage,0x160520u);
 if(std::fegetround()!=FE_TONEAREST)return fail(Status::arithmetic,0x160520u);
 m.read(f.particlesInVolume228,0x160abcu);
 if(m.badPC||f.particlesInVolume228<0.f||f.particlesInVolume228>float(originalGcParticleCapacity50KG))
  return fail(Status::unsupported,m.badPC?m.badPC:0x160abcu);
 for(std::size_t i=0;i<manager.particleCount();++i){auto* p=manager.particleAt(i);
  if(!p)return fail(Status::storage,0x160570u);
  if(p->ownerId==owner){p->flags&=~0x80u;p->ageMilliseconds=p->lifeMilliseconds;}
 }
 for(std::int32_t i=0;float(i)<f.particlesInVolume228;++i){
  auto* p=manager.nextParticle();if(!p){r.poolFull=true;return r;}
  const auto x=draw(),y=draw(),z=draw();const std::array<float,3> unit{{x,y,z}};
  std::array<float,3> offset{},position{};
  for(unsigned k=0;k<3;++k){const auto pc=0x160664u+28u*k;
   const auto range=m.sub(f.maxRange[k],f.minRange[k],pc);
   const auto scaled=m.mul(unit[k],range,pc+4);
   offset[k]=m.add(f.minRange[k],scaled,pc+8);
  }
  position[2]=m.add(bits(c.position30[2]),offset[2],0x1606c8u);
  position[1]=m.add(bits(c.position30[1]),offset[1],0x1606d8u);
  position[0]=m.add(bits(c.position30[0]),offset[0],0x1606e0u);
  if(m.badPC)return fail(Status::arithmetic,m.badPC);p->position=position;p->positionW=1.f;
  if(f.oscillationPeriod22C!=-1.f){
   const auto dz=m.mul(f.oscillationMagnitude234,draw(),0x16073cu);
   const auto dy=m.mul(f.oscillationMagnitude234,draw(),0x160760u);
   const auto dx=m.mul(f.oscillationMagnitude234,draw(),0x160780u);
   std::array<float,3> velocity{{dx,dy,dz}};
   for(unsigned k=0;k<3;++k)if(draw()>.5f)velocity[k]=m.mul(velocity[k],-1.f,0x1607e0u+80u*k);
   if(m.badPC)return fail(Status::arithmetic,m.badPC);p->velocity=velocity;p->velocityW=1.f;
  }
  p->size=f.initialSize184;p->initialSize=f.initialSize184;p->ownerId=owner;
  p->lifeMilliseconds=f.particleLife17C;p->ageMilliseconds=f.ageAtBirth1A0;p->frame=currentFrame;
  const auto inverse=m.div(1.f,p->lifeMilliseconds,0x1608dcu);
  if(m.badPC)return fail(Status::arithmetic,m.badPC);p->inverseLife=inverse;p->growth=0.f;
  p->flags=f.particleFlags198;
  if(x>.5f&&f.randomMirrorX)p->flags|=0x4000u;else p->flags&=~0x4000u;
  if(y>.5f&&f.randomMirrorY)p->flags|=0x8000u;else p->flags&=~0x8000u;
  if(!manager.commitParticle(owner))return fail(Status::storage,0x160994u);++r.spawned;
  if(!seedOriginalGcParticleRandom50KG(raw,shared,clock))return fail(Status::clock,0x1609c8u);
  const auto span=std::int64_t(group->last)-group->first+1;
  if(span<1||span>std::numeric_limits<std::int32_t>::max())return fail(Status::storage,0x1609ecu);
  const auto range=m.integer(static_cast<std::int32_t>(span),0x1609f8u);
  const auto random=draw();const auto selected=m.cvtWord(m.mul(range,random,0x160a18u),0x160a1cu);
  if(m.badPC)return fail(Status::arithmetic,m.badPC);
  const auto next=std::int64_t(group->first)+selected;
  if(next<std::numeric_limits<std::int32_t>::min()||next>std::numeric_limits<std::int32_t>::max())
   return fail(Status::arithmetic,0x160a28u);
  if(!checkOriginalGcParticleRandomRange50KG(static_cast<std::int32_t>(next),group->first,group->last,shared))
   return fail(Status::sourceRangeAssertion,0x160a7cu);
  currentFrame=static_cast<std::int32_t>(next);
 }
 return r;
}

//1624E8 chooses64 or the actual manager/source GlowScalar BEFORE this branch.
// The RC renderer supplies that selected value (128 for its glowing fog), not
// the GC-only64 shortcut. No output is published on arithmetic rejection.
template<class Definition>
bool brightness(const Definition& f,const OriginalGcParticle50KG& p,
 const std::array<float,3>& camera,float selectedGlow,float& output,std::uint32_t& badPC)noexcept {
 if(std::fegetround()!=FE_TONEAREST){badPC=0x162540u;return false;}
 Math m;const auto age=m.mul(p.ageMilliseconds,p.inverseLife,0x162540u);
 auto birth=m.sub(1.f,age,0x162554u);
 const auto x=m.sub(p.position[0],camera[0],0x16255cu);
 const auto y=m.sub(p.position[1],camera[1],0x162560u);
 const auto xx=m.mul(x,x,0x162564u),yy=m.mul(y,y,0x162568u);
 const auto z=m.sub(p.position[2],camera[2],0x16256cu);
 const auto sum=m.add(xx,yy,0x162570u);
 const auto square=m.add(sum,m.mul(z,z,0x162574u),0x162574u);
 if(square<f.distanceSquared220)birth=m.mul(birth,m.mul(square,f.inverseDistanceSquared224,0x16258cu),0x162590u);
 const auto value=m.mul(birth,selectedGlow,0x162598u);
 if(m.badPC){badPC=m.badPC;return false;}output=value;badPC=0;return true;
}
} // namespace rr::game::original_rc_fog_numeric_detail50RC
