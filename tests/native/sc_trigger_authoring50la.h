#pragma once
#include "sc_cold_sources50la.h"
#include "../../src/game/native_trigger_group50ga.h"
#include <array>
#include <iostream>
#include <charconv>
#include <cmath>
#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>

namespace sc_trigger_authoring50LA {
namespace a=rr::assets;
// ONE trigger reader for every course that reads its triggers from its own TOB
// (SC, MA, SA, GA): the original's trigger reader walks [Triggers]
// NumberOfTriggers and the TriggerN / TriggerNActionK blocks of whatever TOB
// the course archive holds. Capacity, not a stock count: the reader's own
// store holds the largest stock trigger set (TC, 22 triggers; MC 21, GB 18,
// TB 16) and the most actions any stock trigger authors (GB trigger 18, 5).
// The live count is the TOB's own NumberOfTriggers (absent block or 0 = no
// triggers). Each native trigger group then bounds the live set by ITS own
// capacity (GA's 13, native_trigger_group50ga.h) and refuses a larger one.
// This is the ONE place the reader's capacities are defined.
inline constexpr std::size_t count=22;
inline constexpr std::size_t maxActions=5;
inline constexpr std::size_t stockCount=9; // SC's stock count (receipt only)
// Authoring values only. No geometry kernel, transform, collider, C80 lease,
// action callback, native Entity ID or source-manager PLAYER resolution.
struct Vector3 {std::array<float,3> values{};};
enum class Shape {sphere,box};
enum class ActionKind {LostAI,Animation,PodSound,Sound,Particles};
struct Action {
 ActionKind kind=ActionKind::LostAI;
 std::uint32_t targetAiNodeIndex=0;  // LostAI
 std::optional<bool> authoredReset;  // LostAI
 std::uint32_t animationNumber=0;    // Animation (1-based, as authored)
 std::string filename;               // PodSound, Sound
 Vector3 position;                   // PodSound
 float minDistance=0,maxDistance=0;  // PodSound
 bool positional=false,isGlobalSound=false; // Sound
 std::uint32_t emitterNumber=0,instanceNumber=0; // Particles (1-based, as authored)
 bool turnOn=true;                   // Particles
 std::optional<bool> authoredOneTimeOnly; // any kind (retained; per-action once flag)
 const a::ParameterBlock48F* original=nullptr;
};
struct Source {
 std::string name; // absent Name = empty (GA's animation triggers omit it)
 std::uint32_t sourceZoneOrdinal=0,targetAiNodeIndex=0;
 Shape shape=Shape::sphere;
 Vector3 position;
 std::optional<float> radius,yaw,pitch,roll;
 std::optional<Vector3> box; // Original local Box fields, not transformed corners.
 std::optional<std::string> ignoreTrigger;
 // Omitted StartEnabled = TRUE and RandomTriggerPercent = 100 (2A0290/2DBDA0).
 bool startEnabled=true,oneTimeOnly=false;
 bool startEnabledAuthored=false,randomTriggerPercentAuthored=false;
 float randomTriggerPercent=100;
 std::optional<bool> authoredReset;   // action 1 when LostAI
 bool resetDefault1B5A60=false;       // Original default 0 when Reset is omitted.
 std::uint32_t actionCount=0;
 std::array<Action,maxActions> actions{};
 // Borrow exact blocks; absent fields and unused authoring remain observable.
 const a::ParameterBlock48F* originalTrigger=nullptr;
 const a::ParameterBlock48F* originalAction=nullptr; // action 1
};
class Owner final {
 std::shared_ptr<const sc_cold_sources50LA::Owner> source_;
 std::array<Source,count> sources_{};
 std::size_t live_=0;
 bool attempted_=false,decoded_=false;
 static void need(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
 static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
  const a::ParameterBlock48F* found=nullptr;
  for(const auto& b:p.blocks)if(b.name==name){need(!found,"duplicate trigger TOB block");found=&b;}
  need(found,"missing trigger TOB block");return *found;
 }
 static const std::string* optional(const a::ParameterBlock48F& b,const char* name){
  const std::string* found=nullptr;
  for(const auto& f:b.fields)if(f.name==name){need(!found,"duplicate trigger TOB field");found=&f.value;}return found;
 }
 static const std::string& field(const a::ParameterBlock48F& b,const char* name){
  const auto* value=optional(b,name);need(value,"missing trigger TOB field");return *value;
 }
 static std::uint32_t integer(const std::string& text){
  std::uint32_t value=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),value);
  need(r.ec==std::errc{}&&r.ptr==text.data()+text.size(),"trigger integer text");return value;
 }
 // The original's TOB integer reader (158B30 -> 158920 -> 30AE70, strtol
 // base 0) consumes the integer prefix and ignores the end pointer: MB's
 // stock TargetNodeIndex "148d" reads 148 and MC's RandomTriggerPercent
 // "99.996002" reads 99. Used for exactly those fields here; the other
 // integer fields keep the strict parse (stock text is plain digits).
 static std::int32_t integer158B30(const std::string& text){
  const long value=std::strtol(text.c_str(),nullptr,0);
  return static_cast<std::int32_t>(value);
 }
 // Reset: TRUE/FALSE/T/F, or an integer (MB's stock UpShaft authors Reset 1,
 // read as a reset by the MB route's 1B5A88 trace).
 static bool resetValue(const std::string& text){
  if(text=="TRUE"||text=="FALSE"||text=="T"||text=="F")return text=="TRUE"||text=="T";
  return integer158B30(text)!=0;
 }
 static float number(const std::string& text){
  float value=0;need(a::parseTrackNumber50AD(text.data(),text.size(),value)&&std::isfinite(value),"trigger source decimal");return value;
 }
 static bool boolean(const std::string& text){
  need(text=="TRUE"||text=="FALSE"||text=="T"||text=="F","trigger boolean text");return text=="TRUE"||text=="T";
 }
 static Vector3 vector(const std::string& text){
  Vector3 result;std::size_t begin=0;
  for(std::size_t i=0;i<3;++i){const auto end=text.find(',',begin);
   need((end==std::string::npos)==(i==2),"trigger vector extent");
   result.values[i]=number(text.substr(begin,end==std::string::npos?end:end-begin));begin=end+1;
  }return result;
 }
 static const char* kindName(ActionKind k)noexcept{
  return k==ActionKind::LostAI?"LostAI":k==ActionKind::Animation?"Animation":k==ActionKind::PodSound?"PodSound":
   k==ActionKind::Sound?"Sound":"Particles";
 }
public:
 struct Diagnostic {const char* failure=nullptr;std::size_t trigger=0;};
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 // Common reader. `p` must outlive this owner (sources borrow its blocks).
 // zoneCount: the course world's zone count. aiNodeBound: every LostAI target
 // must be below it (the AI spline's node count, or the native spline storage
 // when the live spline is bound later and checks its exact count itself).
 bool decode(const a::ParameterDocument48F& p,std::size_t zoneCount,std::uint32_t aiNodeBound,
     const char* site,Diagnostic& d)noexcept {
  d={};if(attempted_){d.failure="trigger owner already attempted";return false;}attempted_=true;
  try{
   const a::ParameterBlock48F* triggers=nullptr;
   for(const auto& b:p.blocks)if(b.name=="triggers"){need(!triggers,"duplicate trigger TOB block");triggers=&b;}
   const auto* authoredCount=triggers?optional(*triggers,"numberoftriggers"):nullptr;
   const std::uint32_t n=authoredCount?integer(*authoredCount):0;
   need(n<=count,"NumberOfTriggers exceeds the native trigger group");
   std::array<Source,count> next{};std::string kinds;
   for(std::size_t i=0;i<n;++i){d.trigger=i+1;
    const auto label="trigger"+std::to_string(i+1);const auto& b=block(p,label);auto& out=next[i];
    if(const auto* name=optional(b,"name"))out.name=*name;
    out.sourceZoneOrdinal=integer(field(b,"zone"));
    need(out.sourceZoneOrdinal>=1&&out.sourceZoneOrdinal<=zoneCount,"trigger zone outside the world");
    if(const auto* v=optional(b,"startenabled")){out.startEnabled=boolean(*v);out.startEnabledAuthored=true;}
    out.oneTimeOnly=boolean(field(b,"onetimeonly"));
    if(const auto* v=optional(b,"randomtriggerpercent")){out.randomTriggerPercent=static_cast<float>(integer158B30(*v));out.randomTriggerPercentAuthored=true;}
    if(const auto* ignore=optional(b,"ignoretrigger"))out.ignoreTrigger=*ignore;
    need(!optional(b,"parent")&&!optional(b,"attachtosoultree"),"trigger attachment is not supported natively");
    out.position=vector(field(b,"position"));
    // Shape from the authored fields: Radius = sphere, Box(+Yaw/Pitch/Roll) = box.
    const bool sphere=optional(b,"radius")!=nullptr;
    if(sphere){
     need(!optional(b,"box"),"trigger authors both Radius and Box");
     out.radius=number(field(b,"radius"));need(*out.radius>0,"trigger sphere radius");out.shape=Shape::sphere;
    }else{
     out.shape=Shape::box;out.box=vector(field(b,"box"));
     for(const auto extent:out.box->values)need(extent>0,"trigger authored box extent");
     out.yaw=number(field(b,"yaw"));out.pitch=number(field(b,"pitch"));out.roll=number(field(b,"roll"));
    }
    out.actionCount=integer(field(b,"numberofactions"));
    need(out.actionCount>=1&&out.actionCount<=maxActions,"trigger action count outside the native action slots");
    if(i)kinds+=',';
    for(std::uint32_t k=0;k<out.actionCount;++k){
     const auto& action=block(p,label+"action"+std::to_string(k+1));auto& act=out.actions[k];act.original=&action;
     const auto& type=field(action,"actiontype");
     if(k)kinds+='+';
     if(const auto* once=optional(action,"onetimeonly"))act.authoredOneTimeOnly=boolean(*once);
     if(type=="LostAI"){
      const auto target=integer158B30(field(action,"targetnodeindex"));
      need(target>=0,"LostAI target below node 0");
      act.kind=ActionKind::LostAI;act.targetAiNodeIndex=static_cast<std::uint32_t>(target);
      need(act.targetAiNodeIndex<aiNodeBound,"LostAI target outside the AI spline");
      if(const auto* reset=optional(action,"reset"))act.authoredReset=resetValue(*reset);
     }else if(type=="Animation"){
      act.kind=ActionKind::Animation;act.animationNumber=integer(field(action,"animationnumber"));
      need(act.animationNumber>=1,"Animation action number");
      // FLAG stand-in: the native Animation action implements plain
      // activation only; these authored routes are refused, not ignored.
      need(!optional(action,"timetillreset")&&!optional(action,"animationnameref")&&!optional(action,"specificobject"),
          "Animation action TimeTillReset/AnimationNameRef/SpecificObject is not supported natively");
     }else if(type=="PodSound"){
      act.kind=ActionKind::PodSound;act.filename=field(action,"filename");act.position=vector(field(action,"position"));
      act.minDistance=number(field(action,"mindistance"));act.maxDistance=number(field(action,"maxdistance"));
      need(!act.filename.empty()&&act.minDistance>=0&&act.maxDistance>=act.minDistance,"PodSound action fields");
      need(!optional(action,"specificobject"),"PodSound action SpecificObject is not supported natively");
     }else if(type=="Sound"){
      // Ordinary (non-pod) trigger sound: FileName, Positional, IsGlobalSound.
      act.kind=ActionKind::Sound;act.filename=field(action,"filename");
      act.positional=boolean(field(action,"positional"));act.isGlobalSound=boolean(field(action,"isglobalsound"));
      need(!act.filename.empty(),"Sound action FileName");
      need(!optional(action,"specificobject"),"Sound action SpecificObject is not supported natively");
     }else if(type=="Particles"){
      // Emitter switch: EmitterNumber / InstanceNumber (1-based), TurnOn.
      act.kind=ActionKind::Particles;act.emitterNumber=integer(field(action,"emitternumber"));
      act.instanceNumber=integer(field(action,"instancenumber"));act.turnOn=boolean(field(action,"turnon"));
      need(act.emitterNumber>=1&&act.instanceNumber>=1,"Particles action emitter/instance number");
     }else need(false,"trigger action type other than LostAI/Animation/PodSound/Sound/Particles is not supported natively");
     kinds+=kindName(act.kind);
    }
    // Action-1 mirror kept for the LostAI-only consumers.
    const auto& first=out.actions[0];out.originalAction=first.original;
    if(first.kind==ActionKind::LostAI){out.targetAiNodeIndex=first.targetAiNodeIndex;out.authoredReset=first.authoredReset;}
    // Original default when Reset is omitted: 0 (1B5A60).
    out.resetDefault1B5A60=out.authoredReset.value_or(false);out.originalTrigger=&b;
   }
   // One line per trigger set: count and action kinds (stock equality check).
   std::cerr<<"course_triggers site="<<(site?site:"?")<<" count="<<n<<" kinds="<<(n?kinds:std::string("-"))<<"\n";
   live_=n;
   sources_=std::move(next);decoded_=true;return true;
  }catch(const std::exception& e){
   std::cerr<<"course_triggers site="<<(site?site:"?")<<" trigger="<<d.trigger<<" refused: "<<e.what()<<"\n";
   d.failure="trigger authoring rejected; private prefix retained";return false;
  }catch(...){d.failure="trigger authoring rejected; private prefix retained";return false;}
 }
 // SC entry: its cold source owner holds the TOB, world and AI spline.
 bool decode(std::shared_ptr<const sc_cold_sources50LA::Owner> source,Diagnostic& d)noexcept {
  d={};
  const auto* s=source?source->sourcesDecoded():nullptr;
  if(attempted_||!(s&&source->zeroSkyReceipt()&&s->ai.gatesPrepared&&s->ai.track.count)){
   attempted_=true;d.failure="SC trigger authoring rejected; private prefix retained";return false;
  }
  source_=std::move(source);
  if(!decode(s->triggerParameters,s->world.zones.size(),s->ai.track.count,"SC",d))return false;
  if(live_!=stockCount)std::cerr<<"sc_trigger_authoring triggers="<<live_<<" stock="<<stockCount<<"\n";
  // FLAG stand-in (SC native action owner): one LostAI action per trigger is
  // what the SC action owner implements; another count or type is refused.
  for(std::size_t i=0;i<live_;++i)if(sources_[i].actionCount!=1||sources_[i].actions[0].kind!=ActionKind::LostAI){
   d.trigger=i+1;d.failure="SC trigger action other than one LostAI is not supported natively";
   decoded_=false;live_=0;return false;
  }
  return true;
 }
 // Live prefix [0,liveCount()) of a capacity-sized store.
 const std::array<Source,count>* sources()const noexcept{return decoded_?&sources_:nullptr;}
 std::size_t liveCount()const noexcept{return decoded_?live_:0;}
};
} // namespace sc_trigger_authoring50LA
