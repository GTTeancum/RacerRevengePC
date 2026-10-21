#pragma once
#include "mc_scene_authoring50mc.h"
namespace mc_particle_authoring50MC {
namespace a=rr::assets;namespace g=rr::game;namespace detail=mc_scene_authoring50MC::detail;
inline constexpr std::size_t count=40;
enum class Kind:std::uint8_t {Sparks2=2,Fire8=8};
struct Source {
 const a::ParameterBlock48F* original=nullptr;const std::string* filename=nullptr;const std::string* attachToSoulTree=nullptr;
 std::uint32_t ordinal=0,startingZone=0,instances=1;Kind kind=Kind::Fire8;
 g::Vector3_48C authoredPositionOrOffset{};bool attached=false,effectiveStartOn=true;
 std::optional<bool> authoredStartOn;
 // No named placement match is silently chosen. Attached descriptor fan-out
 // and emitter-frame offset transforms require the actual stable world graph.
};
class Owner final:public mc_scene_authoring50MC::OwnerBase<Source,count> {
public:
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,std::string& error){
  return prepare(std::move(cold),available,error,[](const a::ParameterDocument48F& p,std::array<Source,count>& next){
   using namespace detail;
   need(integer(field(block(p,"particleemitters"),"numberofemitters"))==count,"MC emitter count");
   std::size_t attached=0,water=0;
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto& b=block(p,"particleemitter"+std::to_string(i+1));s.original=&b;s.ordinal=i+1;
    const auto kind=integer(field(b,"typeid"));need(kind==(i>=2&&i<8?2u:8u),"MC exact emitter subtype");s.kind=static_cast<Kind>(kind);water+=kind==2;
    s.filename=&field(b,"emitterfilename");s.startingZone=integer(field(b,"startingzone"));s.instances=integer(field(b,"numberofinstances"));
    need(s.instances==1&&s.startingZone>=1&&s.startingZone<=28,"MC emitter zone/instance count");
    s.attachToSoulTree=optional(b,"attachtosoultree");s.attached=s.attachToSoulTree!=nullptr;
    if(s.attached){++attached;need(i<8&&!optional(b,"position"),"MC attachment route");s.authoredPositionOrOffset=vector(field(b,"offsetfromsoultree"));}
    else{need(i>=8&&!optional(b,"offsetfromsoultree"),"MC world-position route");s.authoredPositionOrOffset=vector(field(b,"position"));}
    if(const auto* v=optional(b,"starton")){s.authoredStartOn=detail::boolean(*v);s.effectiveStartOn=*s.authoredStartOn;}
    // Original27C720..72C default1, not a newly invented startup effect.
    need(s.effectiveStartOn&&s.authoredStartOn.has_value()==(kind==2)&&!optional(b,"childobject"),"MC original emitter flags");
    if(kind==8)need(*s.filename=="mcfire.emt","MC original fire definition");
    else need(*s.filename=="ma_water0"+std::to_string(i-1)+".emt"&&*s.attachToSoulTree=="z27c.slt","MC original water definition/attachment");
   }
   need(attached==8&&water==6,"MC descriptor roles");
  });
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::string& error){return decode(std::move(cold),maximumAdditionalPayloadBytes(),error);}
};
}
