#pragma once
#include "mc_particle_source50mc.h"
#include "../../src/game/original_gc_fire_emission50kg.h"

namespace mc_type2_numeric50MC {
namespace g=rr::game;namespace source=mc_particle_source50MC;
using Status=g::OriginalGcSprayStatus50KG;
// Source15D938..15DCD4 derived emission only. The concrete scene scheduler must
// first execute common15A7A0, the actual19C<=.25 test and attachment visibility
// gate15D8EC..934. This kernel does not manufacture those admission facts.
// The actual source2 particles use Group6/type2 even though EMT names say water.
inline g::OriginalGcSprayReceipt50KG emitAfterCommon(const source::Owner& authored,
 std::size_t definitionIndex,g::OriginalGcParticleManager50KG& manager,std::uint32_t owner,
 g::EffectControlState50BC& controls,std::int32_t& currentFrame190,float& carriedSize18C,
 float dt,std::uint32_t& sharedRawRandom)noexcept{
 using Math=g::water_wake_detail50jc::Math;g::OriginalGcSprayReceipt50KG r;
 const auto fail=[&](Status status,std::uint32_t pc){r.status=status;r.sourcePC=pc;return r;};
 if(!authored.ready()||definitionIndex>=authored.definitionCountLive()||!manager.prepared())return fail(Status::unprepared,0x15d938);
 const auto& f=authored.definition(definitionIndex);const auto* row=manager.emitter(owner);const auto* group=manager.group(f.particleGroup1B8);
 if(!row||!group||f.kind!=source::Kind::Sparks2||group->type!=6||row->particleGroup!=f.particleGroup1B8||
    row->framesPerSet!=f.framesPerSet||row->gravity!=f.gravityMultiplier||row->bouncePlane!=f.groundLevel||row->bounceFactor!=f.restitution||
    currentFrame190<group->first||currentFrame190>group->last)return fail(Status::storage,0x15dd98);
 if(std::fegetround()!=FE_TONEAREST||!std::isfinite(dt)||dt<0||dt>1)return fail(Status::arithmetic,0x15d880);
 Math m;const auto word=[](std::uint32_t value){float f;std::memcpy(&f,&value,4);return f;};
 std::array<float,3> velocity{};for(unsigned k=0;k<3;++k)velocity[k]=m.read(word(controls.velocity60[k]),0x15d938+4*k);
 const float square=m.add(m.add(m.mul(velocity[0],velocity[0],0x15d948),m.mul(velocity[1],velocity[1],0x15d94c),0x15d950),
   m.mul(velocity[2],velocity[2],0x15d954),0x15d954);
 if(m.badPC)return fail(Status::arithmetic,m.badPC);
 volatile float speed=std::sqrt(square);m.read(speed,0x15d958);
 float requested=m.mul(f.emitRate188,dt,0x15d980);
 if(speed<5.f&&f.movingEmitter200){if(speed<.5f)requested=0;else requested=m.mul(requested,m.div(speed,5.f,0x15d9c0),0x15d9c4);}
 if(m.badPC||!std::isfinite(requested)||requested<0||requested>5000)return fail(Status::arithmetic,m.badPC?m.badPC:0x15dca8);
 r.status=Status::complete;r.sourcePC=0x15dcd8;
 const auto finish=[&](){carriedSize18C=f.initialSize184;controls.word19C=0;return r;};
 // Original compares float(integer counter)<requested, therefore a fractional
 // positive count emits ceil(requested), not truncation and not a carry bank.
 for(std::int32_t n=0;static_cast<float>(n)<requested;++n){
  const float x=g::drawOriginalGcParticleRandom50KG(sharedRawRandom);
  const float y=g::drawOriginalGcParticleRandom50KG(sharedRawRandom);
  const float z=g::drawOriginalGcParticleRandom50KG(sharedRawRandom);r.randomDraws+=3;
  const std::array<float,3> random{x,y,z};std::array<float,3> spread{};
  for(unsigned k=0;k<3;++k)spread[k]=m.add(f.minRange[k],m.mul(random[k],m.sub(f.maxRange[k],f.minRange[k],0x15da54+k*28),0x15da58+k*28),0x15da5c+k*28);
  if(m.badPC)return fail(Status::arithmetic,m.badPC);
  // Unlike Fire8, original source2 consumes three spread draws BEFORE testing
  // pool capacity. Preserve that reached RNG prefix on a full-pool exit.
  auto* particle=manager.nextParticle();if(!particle){r.poolFull=true;r.sourcePC=0x15dc64;return finish();}
  particle->ageMilliseconds=0;for(unsigned k=0;k<3;++k)particle->position[k]=word(controls.position30[k]);
  particle->positionW=word(controls.position30[3]);particle->frame=currentFrame190;
  const float growthRandom=g::drawOriginalGcParticleRandom50KG(sharedRawRandom);++r.randomDraws;
  particle->growth=m.mul(f.growth180,growthRandom,0x15daf4);if(particle->growth==0)particle->flags&=~8u;
  particle->size=particle->initialSize=f.initialSize184;particle->ownerId=owner;
  const float lifeRandom=g::drawOriginalGcParticleRandom50KG(sharedRawRandom);++r.randomDraws;
  particle->lifeMilliseconds=m.mul(f.particleLife17C,lifeRandom,0x15db5c);
  particle->inverseLife=m.div(1.f,particle->lifeMilliseconds,0x15db70);
  if(m.badPC)return fail(Status::arithmetic,m.badPC); // Preserve unpublished prefix, no retry/clamp.
  for(unsigned k=0;k<3;++k)particle->velocity[k]=m.add(velocity[k],spread[k],0x15db90+4*k);particle->velocityW=1;
  if(m.badPC)return fail(Status::arithmetic,m.badPC);
  particle->flags=f.particleFlags198;
  if(x>.5f&&f.randomMirrorX)particle->flags|=0x4000u;else particle->flags&=~0x4000u;
  if(y>.5f&&f.randomMirrorY)particle->flags|=0x8000u;else particle->flags&=~0x8000u;
  if(!manager.commitParticle(owner))return fail(Status::storage,0x15dc60);++r.spawned;
  ++currentFrame190;if(currentFrame190>group->last)currentFrame190=group->first;
 }
 return finish();
}
}
