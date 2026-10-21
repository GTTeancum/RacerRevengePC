#pragma once
#include "mc_scene_authoring50mc.h"
namespace mc_trigger_authoring50MC {
namespace a=rr::assets;namespace g=rr::game;namespace detail=mc_scene_authoring50MC::detail;
inline constexpr std::size_t count=21;
enum class Action {LostAI,Animation};
struct Source {
 const a::ParameterBlock48F* original=nullptr;const a::ParameterBlock48F* actionOriginal=nullptr;
 const std::string* name=nullptr;const std::string* authoredRandomTriggerText=nullptr;
 std::uint32_t sourceOrdinal=0,zone=0,actionCount=1,shapeType=5,targetNodeIndex=0,animationIndex=0;
 g::Vector3_48C position{},boxHalf{};float radius=0,yaw=0,pitch=0,roll=0,authoredRandomTriggerPercent=100;
 std::int32_t randomTriggerPercent=100;bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;
 std::optional<bool> authoredStartEnabled,authoredReset,authoredActionOneTimeOnly;
 bool effectiveReset1B5A60=false;Action action=Action::LostAI;
 // Box orientation remains authored Euler data. No std::sin approximation,
 // source-libm transform, coarse sphere or collision admission is fabricated.
};
class Owner final:public mc_scene_authoring50MC::OwnerBase<Source,count> {
public:
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::size_t available,std::string& error){
  return prepare(std::move(cold),available,error,[](const a::ParameterDocument48F& p,std::array<Source,count>& next){
   using namespace detail;
   need(integer(field(block(p,"triggers"),"numberoftriggers"))==count,"MC trigger count");
   constexpr std::uint32_t targets[20]={157,158,37,159,160,161,152,61,64,62,168,112,163,115,164,166,167,164,166,158};
   std::size_t boxes=0,spheres=0;
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto label="trigger"+std::to_string(i+1);const auto& b=block(p,label);
    s.original=&b;s.sourceOrdinal=i+1;s.zone=integer(field(b,"zone"));s.position=vector(field(b,"position"));
    need(integer(field(b,"numberofactions"))==1&&s.zone>=1&&s.zone<=28,"MC trigger zone/action count");
    s.name=optional(b,"name");s.oneTimeOnly=detail::boolean(field(b,"onetimeonly"));
    if(const auto* v=optional(b,"startenabled")){s.authoredStartEnabled=detail::boolean(*v);s.startEnabled=*s.authoredStartEnabled;}
    if(const auto* v=optional(b,"ignoretrigger")){need(*v=="PLAYER","MC ignore selector");s.ignorePlayer=true;}
    s.authoredRandomTriggerText=&field(b,"randomtriggerpercent");s.authoredRandomTriggerPercent=number(*s.authoredRandomTriggerText);
    // 2DBE9C ->158B30 ->158920 ->30AE70/30AC38 consumes the integer
    // prefix and ignores endptr. Exact MC spellings allow bounded projection.
    const auto& probability=*s.authoredRandomTriggerText;
    need(probability=="99.996002"||probability=="100.000000"||probability=="50","MC probability spelling");
    s.randomTriggerPercent=probability=="99.996002"?99:probability=="50"?50:100;
    if(const auto* v=optional(b,"box")){++boxes;s.shapeType=6;s.boxHalf=vector(*v);
     need(!optional(b,"radius"),"MC box/sphere conflict");s.yaw=number(field(b,"yaw"));s.pitch=number(field(b,"pitch"));s.roll=number(field(b,"roll"));
     for(unsigned axis=0;axis<3;++axis)need(s.boxHalf.v[axis]>0,"MC positive box half");
    }else{++spheres;s.radius=number(field(b,"radius"));need(s.radius>0&&!optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"MC sphere authoring");}
    need(s.startEnabled&&s.oneTimeOnly==(i==20)&&s.ignorePlayer==(i<19)&&!optional(b,"parent")&&!optional(b,"attachtosoultree"),"MC trigger source flags");
    const auto& action=block(p,label+"action1");s.actionOriginal=&action;
    need(!optional(action,"reset"),"MC Reset is absent");
    if(i<20){need(field(action,"actiontype")=="LostAI"&&action.fields.size()==2,"MC LostAI action");s.targetNodeIndex=integer(field(action,"targetnodeindex"));
     need(s.targetNodeIndex==targets[i]&&s.targetNodeIndex<173,"MC actual AI target");
    }else{s.action=Action::Animation;need(field(action,"actiontype")=="Animation"&&integer(field(action,"animationnumber"))==1&&action.fields.size()==3,"MC bridge animation action");
     s.authoredActionOneTimeOnly=detail::boolean(field(action,"onetimeonly"));need(*s.authoredActionOneTimeOnly,"MC retained unused action once field");}
   }
   need(boxes==14&&spheres==7,"MC exact trigger shape counts");
  });
 }
 bool decode(std::shared_ptr<const mc_cold_sources50LA::Owner> cold,std::string& error){return decode(std::move(cold),maximumAdditionalPayloadBytes(),error);}
};
}
