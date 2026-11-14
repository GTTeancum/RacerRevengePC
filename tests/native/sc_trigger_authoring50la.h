#pragma once
#include "sc_cold_sources50la.h"
#include "../../src/game/native_trigger_group50ga.h"
#include <array>
#include <iostream>
#include <charconv>
#include <cmath>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace sc_trigger_authoring50LA {
namespace a=rr::assets;
// Capacity, not the stock count (9): the native trigger group's storage. The
// live count is the TOB's own [Triggers] NumberOfTriggers (0 = no triggers).
inline constexpr std::size_t count=rr::game::nativeTriggerCapacity50GA;
inline constexpr std::size_t stockCount=9;
// Authoring values only. No geometry kernel, transform, collider, C80 lease,
// action callback, native Entity ID or source-manager PLAYER resolution.
struct Vector3 {std::array<float,3> values{};};
enum class Shape {sphere,box};
struct Source {
 std::string name;
 std::uint32_t sourceZoneOrdinal=0,targetAiNodeIndex=0;
 Shape shape=Shape::sphere;
 Vector3 position;
 std::optional<float> radius,yaw,pitch,roll;
 std::optional<Vector3> box; // Original local Box fields, not transformed corners.
 std::optional<std::string> ignoreTrigger;
 bool startEnabled=false,oneTimeOnly=false;
 float randomTriggerPercent=0;
 std::optional<bool> authoredReset;
 bool resetDefault1B5A60=false; // All nine omit Reset; actual original default0.
 // Borrow exact blocks; absent fields and unused authoring remain observable.
 const a::ParameterBlock48F* originalTrigger=nullptr;
 const a::ParameterBlock48F* originalAction=nullptr;
};
class Owner final {
 std::shared_ptr<const sc_cold_sources50LA::Owner> source_;
 std::array<Source,count> sources_{};
 std::size_t live_=0;
 bool attempted_=false,decoded_=false;
 static void need(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
 static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
  const a::ParameterBlock48F* found=nullptr;
  for(const auto& b:p.blocks)if(b.name==name){need(!found,"duplicate SC block");found=&b;}
  need(found,"missing SC block");return *found;
 }
 static const std::string* optional(const a::ParameterBlock48F& b,const char* name){
  const std::string* found=nullptr;
  for(const auto& f:b.fields)if(f.name==name){need(!found,"duplicate SC field");found=&f.value;}return found;
 }
 static const std::string& field(const a::ParameterBlock48F& b,const char* name){
  const auto* value=optional(b,name);need(value,"missing SC field");return *value;
 }
 static std::uint32_t integer(const std::string& text){
  std::uint32_t value=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),value);
  need(r.ec==std::errc{}&&r.ptr==text.data()+text.size(),"SC integer text");return value;
 }
 static float number(const std::string& text){
  float value=0;need(a::parseTrackNumber50AD(text.data(),text.size(),value)&&std::isfinite(value),"SC source decimal");return value;
 }
 static Vector3 vector(const std::string& text){
  Vector3 result;std::size_t begin=0;
  for(std::size_t i=0;i<3;++i){const auto end=text.find(',',begin);
   need((end==std::string::npos)==(i==2),"SC vector extent");
   result.values[i]=number(text.substr(begin,end==std::string::npos?end:end-begin));begin=end+1;
  }return result;
 }
public:
 struct Diagnostic {const char* failure=nullptr;std::size_t trigger=0;};
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 bool decode(std::shared_ptr<const sc_cold_sources50LA::Owner> source,Diagnostic& d)noexcept {
  d={};if(attempted_){d.failure="SC trigger owner already attempted";return false;}attempted_=true;
  try{
   const auto* s=source?source->sourcesDecoded():nullptr;
   need(s&&source->zeroSkyReceipt()&&s->ai.gatesPrepared&&s->ai.track.count,
        "SC source and AI spline owner required");
   source_=std::move(source);const auto& p=s->triggerParameters;
   // Every value is read from the course's own TOB, as the original's trigger
   // reader does (stock SC: 9 triggers, all LostAI, StartEnabled TRUE ...).
   const a::ParameterBlock48F* triggers=nullptr;
   for(const auto& b:p.blocks)if(b.name=="triggers"){need(!triggers,"duplicate SC block");triggers=&b;}
   const auto* authoredCount=triggers?optional(*triggers,"numberoftriggers"):nullptr;
   const std::uint32_t n=authoredCount?integer(*authoredCount):0;
   need(n<=count,"SC NumberOfTriggers exceeds the native trigger group");
   if(n!=stockCount)std::cerr<<"sc_trigger_authoring triggers="<<n<<" stock="<<stockCount<<"\n";
   const auto boolean=[](const std::string& text){
    need(text=="TRUE"||text=="FALSE","SC trigger boolean text");return text=="TRUE";};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<n;++i){d.trigger=i+1;
    const auto label="trigger"+std::to_string(i+1);const auto& b=block(p,label);auto& out=next[i];
    // FLAG stand-in: one LostAI action per trigger is what the native action
    // owner implements; another action count or type is refused here.
    need(field(b,"numberofactions")=="1","SC trigger action count other than 1 is not supported natively");
    const auto& action=block(p,label+"action1");
    out.name=field(b,"name");out.sourceZoneOrdinal=integer(field(b,"zone"));
    need(out.sourceZoneOrdinal>=1&&out.sourceZoneOrdinal<=s->world.zones.size(),"SC trigger zone outside the world");
    out.startEnabled=boolean(field(b,"startenabled"));out.oneTimeOnly=boolean(field(b,"onetimeonly"));
    out.randomTriggerPercent=number(field(b,"randomtriggerpercent"));
    if(const auto* ignore=optional(b,"ignoretrigger"))out.ignoreTrigger=*ignore;
    need(!optional(b,"parent")&&!optional(b,"attachtosoultree"),"SC trigger attachment is not supported natively");
    out.position=vector(field(b,"position"));
    // Shape from the authored fields: Radius = sphere, Box(+Yaw/Pitch/Roll) = box.
    const bool sphere=optional(b,"radius")!=nullptr;
    if(sphere){
     need(!optional(b,"box"),"SC trigger authors both Radius and Box");
     out.radius=number(field(b,"radius"));need(*out.radius>0,"SC sphere radius");out.shape=Shape::sphere;
    }else{
     out.shape=Shape::box;out.box=vector(field(b,"box"));
     for(const auto extent:out.box->values)need(extent>0,"SC authored box extent");
     out.yaw=number(field(b,"yaw"));out.pitch=number(field(b,"pitch"));out.roll=number(field(b,"roll"));
    }
    need(field(action,"actiontype")=="LostAI","SC trigger action type other than LostAI is not supported natively");
    out.targetAiNodeIndex=integer(field(action,"targetnodeindex"));
    need(out.targetAiNodeIndex<s->ai.track.count,"SC LostAI target outside the AI spline");
    if(const auto* reset=optional(action,"reset"))out.authoredReset=boolean(*reset);
    // Original default when Reset is omitted: 0 (1B5A60).
    out.resetDefault1B5A60=out.authoredReset.value_or(false);out.originalTrigger=&b;out.originalAction=&action;
   }
   live_=n;
   sources_=std::move(next);decoded_=true;return true;
  }catch(...){d.failure="SC trigger authoring rejected; private prefix retained";return false;}
 }
 // Live prefix [0,liveCount()) of a capacity-sized store.
 const std::array<Source,count>* sources()const noexcept{return decoded_?&sources_:nullptr;}
 std::size_t liveCount()const noexcept{return decoded_?live_:0;}
};
} // namespace sc_trigger_authoring50LA
