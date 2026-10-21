#pragma once
#include "mc_scene_authoring50mc.h"
namespace mc_animation_authoring50MC {
namespace a=rr::assets;namespace g=rr::game;namespace detail=mc_scene_authoring50MC::detail;
inline constexpr std::size_t count=14;
struct Source {
 const a::ParameterBlock48F* original=nullptr;const std::string* mcf=nullptr;const std::string* motionToPlay=nullptr;
 std::uint32_t sourceOrdinal=0,startingZone=0,numberOfMotions=1,numberOfSequences=1,numberInSequence=1,motionSequence=1;
 bool loopSequence=false,sequenceActive=false,oneTimeOnly=true,useCollision=true,useLighting=true,authoredUseBlendedKeys=true;
 bool hasAuthoredNumberOfSequences=false,hasAuthoredNumberInSequence1=false;
 const std::string* unconsumedNumberInSequence=nullptr;g::Vector3_48C offset{};
};
class Owner final:public mc_scene_authoring50MC::OwnerBase<Source,count> {
public:
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,std::string& error){
  return prepare(std::move(cold),available,error,[](const a::ParameterDocument48F& p,std::array<Source,count>& next){
   using namespace detail;
   need(integer(field(block(p,"animations"),"numberofanimations"))==count,"MC animation count");
   constexpr std::uint32_t zones[count]={7,20,7,7,7,7,7,20,12,23,23,7,7,7};
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto& b=block(p,"animation"+std::to_string(i+1));s.original=&b;s.sourceOrdinal=i+1;
    need(b.fields.size()==14&&field(b,"animationtype")=="KeyFramed","MC keyframed animation shape");
    s.mcf=&field(b,"mcf");s.motionToPlay=&field(b,"motiontoplay1");s.startingZone=integer(field(b,"startingzone"));
    s.numberOfMotions=integer(field(b,"numberofmotions"));s.motionSequence=integer(field(b,"motionsequence1"));
    s.offset=vector(field(b,"offset"));s.loopSequence=detail::boolean(field(b,"loopsequence1"));s.sequenceActive=detail::boolean(field(b,"sequenceactive1"));
    s.oneTimeOnly=detail::boolean(field(b,"onetimeonly"));s.useCollision=detail::boolean(field(b,"usecollision"));s.useLighting=detail::boolean(field(b,"uselighting"));
    s.authoredUseBlendedKeys=detail::boolean(field(b,"useblendedkeys"));s.unconsumedNumberInSequence=&field(b,"numberinsequence");
    // 2D8B3C/2D8BD0 read NumberOfSequences and NumberInSequence%d,
    // both default1. The unsuffixed field is retained, not substituted.
    need(!optional(b,"numberofsequences")&&!optional(b,"numberinsequence1")&&*s.unconsumedNumberInSequence=="1","MC actual sequence defaults");
    need(s.startingZone==zones[i]&&s.numberOfMotions==1&&s.motionSequence==1&&s.loopSequence==(i!=0)&&s.sequenceActive==(i!=0)&&
     s.useCollision==(i==0)&&s.oneTimeOnly&&s.useLighting&&s.authoredUseBlendedKeys,"MC actual animation state");
    for(unsigned axis=0;axis<3;++axis)need(s.offset.v[axis]==0,"MC zero animation offset");
   }
  });
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::string& error){return decode(std::move(cold),maximumAdditionalPayloadBytes(),error);}
};
}
