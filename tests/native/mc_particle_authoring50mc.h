#pragma once
#include "mc_scene_authoring50mc.h"
#include "course_archive_receipt50ci.h"
#include "../../src/assets/original_particle_emitters50pe.h"
namespace mc_particle_authoring50MC {
namespace a=rr::assets;namespace g=rr::game;namespace detail=mc_scene_authoring50MC::detail;
namespace pe=rr::assets::particle_emitters50PE;
// Capacity (the common reader's store), not a stock count. The live count is
// the TOB's own [ParticleEmitters] table (stock MC: 40, a receipt).
inline constexpr std::size_t count=pe::capacity;
inline constexpr std::size_t stockCount=40;
enum class Kind:std::uint8_t {Sparks2=2,Fire8=8};
struct Source {
 const a::ParameterBlock48F* original=nullptr;const std::string* filename=nullptr;const std::string* attachToSoulTree=nullptr;
 std::uint32_t ordinal=0,startingZone=0,instances=1;Kind kind=Kind::Fire8;
 g::Vector3_48C authoredPositionOrOffset{};bool attached=false,effectiveStartOn=true;
 std::optional<bool> authoredStartOn;
 // No named placement match is silently chosen. Attached descriptor fan-out
 // and emitter-frame offset transforms require the actual stable world graph.
};
// The descriptor table is read by the ONE common reader
// (src\assets\original_particle_emitters50pe.h, 27DE00/27C600/27C7B0). This
// owner only projects the rows onto what the MC native emitter chain implements.
class Owner final:public mc_scene_authoring50MC::OwnerBase<Source,count> {
 std::size_t live_=0;
public:
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,std::string& error){
  const std::size_t zones=cold&&cold->ready()?cold->world().zones.size():0;
  return prepare(std::move(cold),available,error,[this,zones](const a::ParameterDocument48F& p,std::array<Source,count>& next){
   using namespace detail;
   pe::Table table;need(pe::read(p,"MC",table),"MC emitter table rejected by the common reader (see particle_emitters line)");
   course_archive_receipt50CI::countReceipt("particles_MC","emitters",table.live,stockCount);
   std::size_t attached=0,water=0;bool stockLayout=table.live==stockCount;
   for(std::size_t i=0;i<table.live;++i){const auto& e=table.rows[i];auto& s=next[i];s.original=e.block;s.ordinal=e.ordinal;
    // PORT STAND-IN (flagged): the MC native emitter chain implements loaded
    // Fire8/Sparks2 emitters, one instance each, started on, without
    // ChildObject. Any other authored value is refused, not ignored.
    need(e.loaded(),"MC emitter the original does not load (no filename/instances/type) is not supported natively");
    need(e.typeId==2||e.typeId==8,"PORT STAND-IN: MC native chain implements emitter TypeID 2 and 8 only");
    s.kind=static_cast<Kind>(e.typeId);water+=e.typeId==2;
    need(e.instances==1,"PORT STAND-IN: MC native chain implements one instance per emitter");
    need(e.startingZone>=1&&static_cast<std::size_t>(e.startingZone)<=zones,"MC emitter zone outside the world");
    s.startingZone=static_cast<std::uint32_t>(e.startingZone);s.instances=1;
    need(e.childObject=="NONE","PORT STAND-IN: MC native chain has no ChildObject attachment");
    need(e.startOn!=0,"PORT STAND-IN: MC native chain starts every emitter at staging (StartOn 0 not implemented)");
    // Borrow the document's own strings (the TOB outlives this owner).
    s.filename=pe::detail::find(*e.block,"emitterfilename");s.attachToSoulTree=pe::detail::find(*e.block,"attachtosoultree");
    s.attached=e.attached;if(s.attached)++attached;
    for(unsigned k=0;k<3;++k)s.authoredPositionOrOffset.v[k]=e.positionOrOffset[k];
    if(e.authoredStartOn)s.authoredStartOn=e.startOn!=0;s.effectiveStartOn=true;
    // Stock layout receipt (old per-index pins): 2..7 water Sparks2 attached
    // to z27c.slt, 0..1 attached fire, 8..39 placed fire.
    if(stockLayout){const bool w=i>=2&&i<8;
     if(e.typeId!=(w?2:8)||s.attached!=(i<8)||(e.typeId==8&&*s.filename!="mcfire.emt")||
        (w&&(*s.filename!="ma_water0"+std::to_string(i-1)+".emt"||*s.attachToSoulTree!="z27c.slt")))stockLayout=false;}
   }
   if(stockLayout&&(attached!=8||water!=6))stockLayout=false;
   if(!stockLayout)std::cerr<<"course_count site=particles_MC stock_layout=0 emitters="<<table.live<<" attached="<<attached<<"\n";
   live_=table.live;
  });
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::string& error){return decode(std::move(cold),maximumAdditionalPayloadBytes(),error);}
 std::size_t liveCount()const noexcept{return ready()?live_:0;}
};
}
