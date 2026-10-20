#pragma once
#include "tc_cold_sources50la.h"
#include "ma_trigger_authoring_fixture.h"
#include <memory>
#include <optional>

namespace tc_trigger_authoring50TC {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t count=22,lostCount=21,gate=10,sphereCount=13,boxCount=9;
inline constexpr std::size_t tobBytes=25512;
inline constexpr const char* tobSha="e080f96e2657e1331ac1f829faa880a5e984ebf1c5fab393daa6539534dd0573";
enum class Action {LostAI,Animation,Sound};
struct ActionSource {
 Action kind=Action::LostAI;
 std::uint32_t targetNodeIndex=0,animationIndex=0;
 std::optional<bool> authoredReset;
 bool effectiveReset1B5A60=false;
 std::string filename;
 bool positional=false,isGlobalSound=false;
};
struct Source {
 std::string name;
 std::uint32_t sourceOrdinal=0,zone=0,actionCount=0,shapeType=5;
 g::CollisionSphere48J sphere{};g::NativeTriggerBox50MA box{};
 float yaw=0,pitch=0,roll=0;
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;
 std::int32_t randomTriggerPercent=100;
 std::array<ActionSource,3> actions{};
};
namespace detail {
using ga_trigger_authoring::detail::need;
using ga_trigger_authoring::detail::block;
using ga_trigger_authoring::detail::optional;
using ga_trigger_authoring::detail::field;
using ga_trigger_authoring::detail::integer;
using ga_trigger_authoring::detail::number;
using ga_trigger_authoring::detail::vector;
inline bool boolean(std::string s){
 for(auto& c:s)if(c>='a'&&c<='z')c=static_cast<char>(c-'a'+'A');
 return ga_trigger_authoring::detail::boolean(s);
}
}
// Distinct TC authoring, strongly retaining the single actual cold source.
// No archive, parameter, MAP/AI, libm or world decode is repeated here.
class Owner final {
 std::shared_ptr<const tc_cold_sources50LA::Owner> cold_;
 std::array<Source,count> source_{};bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const tc_cold_sources50LA::Owner> cold,std::string& error){
  if(attempted_){error="TC trigger source requires a fresh lifetime";return false;}attempted_=true;
  try{using namespace detail;
   need(cold&&cold->ready()&&cold->world().zones.size()==25&&cold->world().portals.size()==26&&
       cold->ai().count()==293,"TC trigger requires its actual ready source world and AI");
   need(cold->originalTob().size()==tobBytes&&pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tobSha,
       "TC trigger original TOB identity");
   const auto& p=cold->triggerParameters();
   need(p.preservesDuplicateLabels&&p.original_bytes==cold->originalTob()&&
       integer(field(block(p,"triggers"),"numberoftriggers"))==count,"TC original trigger document/count");
   constexpr std::uint32_t targets[count]={249,252,253,255,257,258,261,263,248,7,0,227,27,36,237,142,268,282,151,260,261,84};
   constexpr std::uint32_t zones[count]={2,2,2,2,2,2,2,2,2,2,8,3,3,4,4,16,17,16,16,2,2,8};
   std::array<Source,count> next{};std::size_t boxes=0,spheres=0;
   for(std::size_t i=0;i<count;++i){auto& s=next[i];s.sourceOrdinal=static_cast<std::uint32_t>(i+1);
    const auto name="trigger"+std::to_string(i+1);const auto& b=block(p,name);
    if(const auto* value=optional(b,"name"))s.name=*value;
    s.zone=integer(field(b,"zone"));s.actionCount=integer(field(b,"numberofactions"));
    s.sphere.center=vector(field(b,"position"));s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    if(const auto* value=optional(b,"startenabled"))s.startEnabled=boolean(*value);
    if(const auto* value=optional(b,"randomtriggerpercent"))need(number(*value)==100.f,"TC non100 trigger RNG route");
    if(const auto* value=optional(b,"ignoretrigger")){need(*value=="PLAYER","TC trigger ignore selector");s.ignorePlayer=true;}
    need(s.zone==zones[i]&&s.actionCount==(i==gate?3u:1u)&&s.oneTimeOnly==(i==gate)&&
        s.startEnabled==(i!=21)&&s.ignorePlayer==(i!=gate),"TC trigger original activation/order changed");
    need(!optional(b,"parent")&&!optional(b,"attachtosoultree"),"TC attached trigger route not admitted");
    if(const auto* half=optional(b,"box")){
     need(i<7||i==19||i==20,"TC box source order changed");++boxes;s.shapeType=6;
     need(!optional(b,"radius"),"TC box radius conflict");s.box.half=vector(*half);
     s.yaw=number(field(b,"yaw"));s.pitch=number(field(b,"pitch"));s.roll=number(field(b,"roll"));
     // Reuse only the admitted native shape6 arithmetic, not MA identity.
     s.box.world=ma_trigger_authoring::detail::boxWorld(s.sphere.center,s.yaw,s.pitch,s.roll);
     need(g::validNativeTriggerBox50MA(s.box),"TC native box rejected");
     s.sphere.radius=ma_trigger_authoring::detail::boxRadius(s.box);
    }else{
     ++spheres;need(!optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"TC sphere orientation route");
     s.sphere.radius=number(field(b,"radius"));
    }
    need(std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,"TC trigger coarse sphere rejected");
    const auto& first=block(p,name+"action1");auto& action=s.actions[0];
    if(i!=gate){
     need(field(first,"actiontype")=="LostAI"&&!optional(first,"reset"),"TC LostAI source/reset changed");
     action.targetNodeIndex=integer(field(first,"targetnodeindex"));
     need(action.targetNodeIndex==targets[i]&&action.targetNodeIndex<cold->ai().count(),"TC LostAI actual target bound");
     // Original1B5A60 default0, not a runtime Reset1 omission.
     action.effectiveReset1B5A60=false;
    }else{
     need(s.name.empty()&&!optional(b,"startenabled")&&!optional(b,"randomtriggerpercent")&&!optional(b,"ignoretrigger"),
         "TC gate exact omitted source fields changed");
     need(s.sphere.center.v[0]==4119.f&&s.sphere.center.v[1]==2430.f&&s.sphere.center.v[2]==12738.f&&s.sphere.radius==50.f,
         "TC original gate geometry changed");
     need(field(first,"actiontype")=="Animation"&&integer(field(first,"animationnumber"))==1&&
         boolean(field(first,"onetimeonly"))&&!optional(first,"timetillreset")&&!optional(first,"animationnameref")&&
         !optional(first,"specificobject"),"TC gate animation action changed");
     action.kind=Action::Animation;action.animationIndex=0; // Original one-based resolver2DA190.
     constexpr const char* names[]={"sfx\\race\\beambrk2","sfx\\race\\latichit"};
     for(std::size_t j=1;j<3;++j){const auto& b=block(p,name+"action"+std::to_string(j+1));auto& sound=s.actions[j];
      need(field(b,"actiontype")=="Sound"&&b.fields.size()==4&&!optional(b,"specificobject"),"TC ordinary Sound action scope");
      sound.kind=Action::Sound;sound.filename=field(b,"filename");sound.positional=boolean(field(b,"positional"));
      sound.isGlobalSound=boolean(field(b,"isglobalsound"));
      need(sound.filename==names[j-1]&&!sound.positional&&sound.isGlobalSound,"TC Sound identity/order/flags changed");
     }
    }
   }
   need(boxes==boxCount&&spheres==sphereCount,"TC trigger exact shape extent");
   source_=std::move(next);cold_=std::move(cold);ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_&&cold_&&cold_->ready();}
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("TC trigger source not ready");return source_.at(i);}
 const tc_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{if(!ready())throw std::runtime_error("TC trigger source not ready");return cold_->originalTob();}
};
}
