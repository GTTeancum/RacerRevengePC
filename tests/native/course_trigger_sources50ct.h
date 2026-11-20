#pragma once
// Common rewrite (R5, PLAN B-C13): the ONE trigger list reader of the race path,
// for every course and every source (stock disc or registry dlc:/ row).
//
// Original 274E90 walks [Triggers] NumberOfTriggers of the course's own TOB and,
// per trigger, the TriggerN block and its TriggerNActionM blocks (2A00A0 entry,
// 2A0290 defaults, 2DBDA0 RandomTriggerPercent, 2DC630 IgnoreTrigger, 1B5E70 ->
// 2DBB60 action classes). Nothing in the original checks a stock checksum or a
// stock count, so nothing here does either: the stock trigger counts are
// receipts (stock_course_receipts50CR "triggers"), logged as
// common_track_data role=triggers ... receipt=match|differs(<stock>)|none.
//
// Storage rule (PLAN 2.2): the trigger rows are sized ONCE from the TOB's
// NumberOfTriggers through common_track50CR::Block (BlockId::triggers, logged
// common_storage); each trigger's actions are sized from its own
// NumberOfActions. Every write is bounds-checked first; a row that would not fit
// is refused (common_storage_refused), never written.
//
// This reader replaces the per-course readers (sc_trigger_authoring50la.h, the
// 14 *_trigger_authoring* headers) and the per-chain "Support" refusals: it
// reads every authored action kind. Whether the race can EXECUTE a kind is the
// action owner's business (course_trigger_actions50ct.h), which refuses a kind
// with no bound destination instead of ignoring it.
//
// N64 hazard rows (TriggerN with N64Volume) are a PORT EXTENSION, NOT ORIGINAL
// (A11): they are kept out of the Revenge rows and read by the N64 reader.
#include "common_track50cr.h"
#include "stock_course_receipts50cr.h"
// Kept for the W0 context headers that reach these through this header
// (terrain_interactive_{setup,continuation,cold_owners,cold_contacts}50du.h used
// pod_materials50ed_fixture through the old reader's include chain).
#include "pod_materials50ed_fixture.h"
#include "course_archive_receipt50ci.h"
#include "../../src/assets/import_world48f.h"
#include "../../src/assets/track_start50ad.h"
#include "../../src/game/native_trigger_box_geometry50ma.h"
#include "../../src/game/n64_track_hazards50nh.h"
#include "../../src/game/terrain_query48j.h"
#include "../../src/game/trigger_entry50in.h"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace course_trigger_sources50CT {
namespace a=rr::assets;namespace g=rr::game;namespace ct=common_track50CR;

// The original's own action types (resolver 1B5E70 -> 2DBB60). The reader reads
// the five kinds below; another original type is a port gap (refused, named as
// such); any other name is unknown to the original, which asserts on it (2DB760
// -> 2DBAE4 "CreateActionClass undefined trigger type", 2DC16C "Error in .toy:
// Undefined ActionType requested in Trigger%dAction%d."). The port keeps the
// shipped behaviour: the race load is refused with that message.
enum class ActionKind : std::uint8_t {LostAI,Animation,PodSound,Sound,Particles};
using Action=ActionKind;
inline const char* actionKindName(ActionKind k)noexcept{
 return k==ActionKind::LostAI?"LostAI":k==ActionKind::Animation?"Animation":k==ActionKind::PodSound?"PodSound":
  k==ActionKind::Sound?"Sound":"Particles";
}
inline bool originalActionType(std::string_view t)noexcept{
 static constexpr const char* names[]={"Animation","ResetAnimation","HideAnimation","EnableTrigger",
  "AnimTexmapTrigger","Sound","PhysicsObject","Scoring","ShaderPass","Particles",
  "Shortcut","CameraShake","LostAI","PodSound"};
 for(const char* n:names)if(t==n)return true;
 return false;
}

struct ActionSource {
 ActionKind kind=ActionKind::LostAI;
 std::uint32_t targetNodeIndex=0;            // LostAI (1B5A78 -> 158B30, default node 0)
 std::optional<bool> authoredReset;          // LostAI Reset as authored
 bool reset=false;                           // 1B5A60 default 0 when omitted
 std::uint32_t animationNumber=0;            // Animation, 1-based as authored
 std::uint32_t animationIndex=0;             // Animation, 0-based row of [Animations]
 std::string filename;                       // PodSound, Sound
 g::Vector3_48C position{};                  // PodSound
 float minDistance=0.f,maxDistance=0.f;      // PodSound
 bool positional=false,isGlobalSound=false;  // Sound
 std::uint32_t emitterNumber=0,instanceNumber=0; // Particles, 1-based as authored
 bool turnOn=true;                           // Particles
 std::optional<bool> authoredOneTimeOnly;    // any kind (per-action once flag, retained)
 const a::ParameterBlock48F* original=nullptr; // borrowed from the Track's decoded TOB
};

// One trigger row. shapeType 5 = sphere, 6 = box (the sphere is then the coarse bound).
struct Source {
 std::string name;                           // absent Name = empty
 std::uint32_t sourceOrdinal=0;              // 1-based TriggerN
 std::uint32_t zone=0;                       // 1-based SCB zone
 std::uint32_t actionCount=0;
 std::uint32_t shapeType=5;
 g::CollisionSphere48J sphere{};
 g::NativeTriggerBox50MA box{};
 float yaw=0.f,pitch=0.f,roll=0.f;
 // Omitted StartEnabled = TRUE and RandomTriggerPercent = 100 (2A0290/2DBDA0).
 bool startEnabled=true,oneTimeOnly=false;
 bool startEnabledAuthored=false,randomTriggerPercentAuthored=false;
 std::int32_t randomTriggerPercent=100;
 // 2DC630: strstr(IgnoreTrigger, "PLAYER" / "AI" / "ANIMATIONS").
 std::optional<std::string> ignoreTrigger;
 bool ignorePlayer=false,ignoreAi=false,ignoreAnimations=false;
 std::vector<ActionSource> actions;          // sized from NumberOfActions
 // Action-1 mirror for LostAI consumers.
 std::uint32_t targetNodeIndex=0;bool reset=false;
 const a::ParameterBlock48F* original=nullptr; // borrowed from the Track's decoded TOB
};

struct Diagnostic {
 std::size_t trigger=0;   // 1-based trigger being read when refused (0 = list level)
 std::string message;
};

class Owner final {
 ct::Block<Source> sources_;
 std::vector<g::N64HazardRow50NH> n64Rows_;
 std::string code_="??";
 std::uint32_t animations_=0,actionTotal_=0;
 bool attempted_=false,ready_=false;
 static void need(bool value,const std::string& reason){if(!value)throw std::runtime_error(reason);}
 static const a::ParameterBlock48F& block(const a::ParameterDocument48F& p,const std::string& name){
  const a::ParameterBlock48F* found=nullptr;
  for(const auto& b:p.blocks)if(b.name==name){need(!found,"duplicate trigger TOB block "+name);found=&b;}
  need(found,"missing trigger TOB block "+name);return *found;
 }
 static const std::string* optional(const a::ParameterBlock48F& b,const char* name){
  const std::string* found=nullptr;
  for(const auto& f:b.fields)if(f.name==name){need(!found,std::string("duplicate trigger TOB field ")+name);found=&f.value;}
  return found;
 }
 static const std::string& field(const a::ParameterBlock48F& b,const char* name){
  const auto* v=optional(b,name);need(v,std::string("missing trigger TOB field ")+name);return *v;
 }
 static std::uint32_t integer(const std::string& text){
  std::uint32_t value=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),value);
  need(r.ec==std::errc{}&&r.ptr==text.data()+text.size(),"trigger integer text");return value;
 }
 // The original's TOB integer reader (158B30 -> 158920 -> 30AE70, strtol base 0)
 // consumes the integer prefix: MB's TargetNodeIndex "148d" reads 148 and MC's
 // RandomTriggerPercent "99.996002" reads 99.
 static std::int32_t integer158B30(const std::string& text){
  return static_cast<std::int32_t>(std::strtol(text.c_str(),nullptr,0));
 }
 // Reset: TRUE/FALSE/T/F or an integer (MB's UpShaft authors Reset 1).
 static bool resetValue(const std::string& text){
  if(text=="TRUE"||text=="FALSE"||text=="T"||text=="F")return text=="TRUE"||text=="T";
  return integer158B30(text)!=0;
 }
 static bool boolean(const std::string& text){
  need(text=="TRUE"||text=="FALSE"||text=="T"||text=="F","trigger boolean text");return text=="TRUE"||text=="T";
 }
 // Reader decimal (parseTrackNumber50AD, the common track number parse).
 static float number(const std::string& text){
  float value=0;need(a::parseTrackNumber50AD(text.data(),text.size(),value)&&std::isfinite(value),"trigger source decimal");
  return value;
 }
 // PORT STAND-IN (flagged): native geometry values (position, radius, box,
 // yaw/pitch/roll, PodSound position/distances) keep the binary64 -> binary32
 // conversion the per-course chains used, so stock triggers stay bit-identical
 // to the recorded logs. Unify with number() once a run proves both agree.
 static float nativeNumber(const std::string& text){
  double value=0.;const auto r=std::from_chars(text.data(),text.data()+text.size(),value);
  need(r.ec==std::errc{}&&r.ptr==text.data()+text.size()&&std::isfinite(value),"trigger native number");
  const float out=static_cast<float>(value);need(std::isfinite(out),"trigger native float range");return out;
 }
 static g::Vector3_48C nativeVector(const std::string& text){
  g::Vector3_48C out{};std::size_t begin=0;
  for(unsigned i=0;i<3;++i){const auto end=text.find(',',begin);
   need((end==std::string::npos)==(i==2),"trigger vector extent");
   out.v[i]=nativeNumber(text.substr(begin,end==std::string::npos?end:end-begin));begin=end+1;
  }
  return out;
 }
 // 2DBF98..C4 degree factor 0x3c8efa34; 157100 look/up by roll Z, pitch X, yaw Y.
 // PORT STAND-IN (flagged): native std trig/normalisation, not 13BFE0/VU parity.
 static g::BodyMatrix48G boxWorld(const g::Vector3_48C& position,float yaw,float pitch,float roll){
  constexpr float radians=0x1.1df468p-6f;
  const double y=double(yaw*radians),p=double(pitch*radians),r=double(roll*radians);
  const double sy=std::sin(y),cy=std::cos(y),sp=std::sin(p),cp=std::cos(p),sr=std::sin(r),cr=std::cos(r);
  using namespace g::native_gate_geometry50kg_detail;
  V look{cp*sy,sp,cp*cy},up{sr*cy-cr*sp*sy,cr*cp,-cr*sp*cy-sr*sy};
  look=scale(look,1.0/std::sqrt(dot(look,look)));up=scale(up,1.0/std::sqrt(dot(up,up)));
  const V right=cross(up,look);const V axes[3]={right,up,look};g::BodyMatrix48G out{};
  for(unsigned c=0;c<3;++c){out.v[c*4]=static_cast<float>(axes[c].x);out.v[c*4+1]=static_cast<float>(axes[c].y);
   out.v[c*4+2]=static_cast<float>(axes[c].z);out.v[12+c]=position.v[c];}
  out.v[15]=1.f;return out;
 }
 // Conservative native coarse sphere from the 8 transformed corners (never a
 // substitute for the oriented-box narrowphase).
 static float boxRadius(const g::NativeTriggerBox50MA& box){
  double maximum=0;
  for(unsigned bits=0;bits<8;++bits){double p[3]{};
   for(unsigned k=0;k<3;++k)for(unsigned c=0;c<3;++c)p[k]+=double(box.world.v[c*4+k])*box.half.v[c]*((bits&(1u<<c))?1.0:-1.0);
   maximum=std::max(maximum,p[0]*p[0]+p[1]*p[1]+p[2]*p[2]);
  }
  return std::nextafter(static_cast<float>(std::sqrt(maximum)),std::numeric_limits<float>::infinity());
 }
 void readAction(const a::ParameterDocument48F& p,const std::string& label,std::uint32_t trigger,std::uint32_t k,ActionSource& act){
  const auto& action=block(p,label+"action"+std::to_string(k+1));act.original=&action;
  const auto& type=field(action,"actiontype");
  if(const auto* once=optional(action,"onetimeonly"))act.authoredOneTimeOnly=boolean(*once);
  if(type=="LostAI"){
   // 1B5A78 -> 158B30 default 0: an omitted TargetNodeIndex reads node 0
   // (stock TB trigger 15 authors only the misspelled TargetNodeIndex135).
   const auto* targetText=optional(action,"targetnodeindex");
   const auto target=targetText?integer158B30(*targetText):0;
   need(target>=0,"LostAI target below node 0");
   act.kind=ActionKind::LostAI;act.targetNodeIndex=static_cast<std::uint32_t>(target);
   if(const auto* reset=optional(action,"reset"))act.authoredReset=resetValue(*reset);
   act.reset=act.authoredReset.value_or(false);
  }else if(type=="Animation"){
   act.kind=ActionKind::Animation;act.animationNumber=integer(field(action,"animationnumber"));
   need(act.animationNumber>=1,"Animation action number");act.animationIndex=act.animationNumber-1;
   // PORT STAND-IN (flagged): the Animation action implements plain activation
   // (101170); these authored routes are refused, not ignored.
   need(!optional(action,"timetillreset")&&!optional(action,"animationnameref")&&!optional(action,"specificobject"),
       "Animation action TimeTillReset/AnimationNameRef/SpecificObject is not supported natively");
  }else if(type=="PodSound"){
   act.kind=ActionKind::PodSound;act.filename=field(action,"filename");act.position=nativeVector(field(action,"position"));
   act.minDistance=nativeNumber(field(action,"mindistance"));act.maxDistance=nativeNumber(field(action,"maxdistance"));
   need(!act.filename.empty()&&act.minDistance>=0&&act.maxDistance>=act.minDistance,"PodSound action fields");
   need(!optional(action,"specificobject"),"PodSound action SpecificObject is not supported natively");
  }else if(type=="Sound"){
   act.kind=ActionKind::Sound;act.filename=field(action,"filename");
   act.positional=boolean(field(action,"positional"));act.isGlobalSound=boolean(field(action,"isglobalsound"));
   need(!act.filename.empty(),"Sound action FileName");
   need(!optional(action,"specificobject"),"Sound action SpecificObject is not supported natively");
  }else if(type=="Particles"){
   act.kind=ActionKind::Particles;act.emitterNumber=integer(field(action,"emitternumber"));
   act.instanceNumber=integer(field(action,"instancenumber"));act.turnOn=boolean(field(action,"turnon"));
   need(act.emitterNumber>=1&&act.instanceNumber>=1,"Particles action emitter/instance number");
  }else if(g::isN64HazardActionType50NH(type)){
   need(false,"ExplodePod/BreakObject/AnimationToggle need N64Volume on their TriggerN row");
  }else if(originalActionType(type)){
   throw std::runtime_error("original trigger action "+type+" is not implemented natively ("+label+"action"+std::to_string(k+1)+")");
  }else{
   throw std::runtime_error("Error in .toy: Undefined ActionType requested in Trigger"+std::to_string(trigger)+
       "Action"+std::to_string(k+1)+". ("+type+")");
  }
 }
 void readRow(const a::ParameterDocument48F& p,const a::ParameterBlock48F& b,std::uint32_t ordinal,std::uint32_t zoneCount,
     Source& out,std::string& kinds){
  const auto label="trigger"+std::to_string(ordinal);
  out.sourceOrdinal=ordinal;out.original=&b;
  if(const auto* name=optional(b,"name"))out.name=*name;
  out.zone=integer(field(b,"zone"));need(out.zone>=1&&out.zone<=zoneCount,"trigger zone outside the world");
  if(const auto* v=optional(b,"startenabled")){out.startEnabled=boolean(*v);out.startEnabledAuthored=true;}
  out.oneTimeOnly=boolean(field(b,"onetimeonly"));
  if(const auto* v=optional(b,"randomtriggerpercent")){out.randomTriggerPercent=integer158B30(*v);out.randomTriggerPercentAuthored=true;}
  if(const auto* ignore=optional(b,"ignoretrigger")){out.ignoreTrigger=*ignore;const auto& text=*ignore;
   out.ignorePlayer=text.find("PLAYER")!=std::string::npos;
   out.ignoreAi=text.find("AI")!=std::string::npos;
   out.ignoreAnimations=text.find("ANIMATIONS")!=std::string::npos;
  }
  // PORT STAND-IN (flagged): attached triggers (Parent / AttachToSoulTree) are refused.
  need(!optional(b,"parent")&&!optional(b,"attachtosoultree"),"trigger attachment is not supported natively");
  // Position is an authored vector (x,y,z): the native vector reader checks each component.
  out.sphere.center=nativeVector(field(b,"position"));
  if(optional(b,"radius")){
   need(!optional(b,"box"),"trigger authors both Radius and Box");
   out.shapeType=5;out.sphere.radius=nativeNumber(field(b,"radius"));need(out.sphere.radius>0.f,"trigger sphere radius");
  }else{
   out.shapeType=6;out.box.half=nativeVector(field(b,"box"));
   for(unsigned k=0;k<3;++k)need(out.box.half.v[k]>0.f,"trigger authored box extent");
   out.yaw=nativeNumber(field(b,"yaw"));out.pitch=nativeNumber(field(b,"pitch"));out.roll=nativeNumber(field(b,"roll"));
   out.box.world=boxWorld(out.sphere.center,out.yaw,out.pitch,out.roll);
   need(g::validNativeTriggerBox50MA(out.box),"native box geometry rejected");
   out.sphere.radius=boxRadius(out.box);
   need(std::isfinite(out.sphere.radius)&&out.sphere.radius>0.f,"box coarse radius rejected");
  }
  out.actionCount=integer(field(b,"numberofactions"));
  // PORT STAND-IN (flagged): allocation guard per trigger (the contact layer's
  // own action limit, trigger_entry50in.h), not a size; stock max is 5 (receipt).
  need(out.actionCount>=1&&out.actionCount<=g::triggerActionCapacity50IN,"trigger NumberOfActions outside 1..action limit");
  out.actions.resize(out.actionCount);
  for(std::uint32_t k=0;k<out.actionCount;++k){
   readAction(p,label,ordinal,k,out.actions[k]);
   if(k)kinds+='+';kinds+=actionKindName(out.actions[k].kind);
  }
  if(out.actions[0].kind==ActionKind::LostAI){out.targetNodeIndex=out.actions[0].targetNodeIndex;out.reset=out.actions[0].reset;}
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;

 // 274E90 over a decoded TOB. `tob` must outlive this owner (rows borrow its
 // blocks). zoneCount: the course world's SCB zones (Counts::zones). One attempt.
 bool load(const a::ParameterDocument48F& tob,std::string_view code,std::uint32_t zoneCount,Diagnostic& d)noexcept {
  d={};
  if(attempted_){d.message="trigger owner already attempted";return false;}attempted_=true;
  try{
   code_=std::string(code.empty()?std::string_view("??"):code);
   // [Animations] NumberOfAnimations of the same TOB (absent block = 0).
   for(const auto& b:tob.blocks)if(b.name=="animations")
    if(const auto* n=optional(b,"numberofanimations")){need(!animations_,"duplicate [Animations]");animations_=integer(*n);}
   const a::ParameterBlock48F* triggers=nullptr;
   for(const auto& b:tob.blocks)if(b.name=="triggers"){need(!triggers,"duplicate trigger TOB block");triggers=&b;}
   const auto* authored=triggers?optional(*triggers,"numberoftriggers"):nullptr;
   const std::uint32_t n=authored?integer(*authored):0u;
   // Count the Revenge rows first (N64 rows go to the N64 reader), then size once.
   std::uint32_t revenge=0;
   for(std::uint32_t i=0;i<n;++i)if(!g::isN64HazardTrigger50NH(block(tob,"trigger"+std::to_string(i+1))))++revenge;
   need(sources_.size(ct::BlockId::triggers,revenge,0x274E90u),"trigger rows above the trigger store ceiling");
   std::string kinds;std::vector<g::N64HazardRow50NH> n64;
   for(std::uint32_t i=0;i<n;++i){d.trigger=i+1;
    const auto& b=block(tob,"trigger"+std::to_string(i+1));
    if(g::isN64HazardTrigger50NH(b)){
     // PORT EXTENSION, NOT ORIGINAL: its own reader and its own bound.
     need(n64.size()<g::N64HazardLimits50NH::rows,"N64 trigger rows exceed the N64 hazard owner");
     g::N64HazardRow50NH row;std::string why;
     if(!g::readN64HazardRow50NH(tob,b,i+1,row,why))throw std::runtime_error("N64 trigger row: "+why);
     const auto z=integer(field(b,"zone"));need(z>=1&&z<=zoneCount,"trigger zone outside the world");
     if(i)kinds+=',';
     kinds+=row.actions[0].kind==g::N64HazardKind50NH::explodePod?"N64ExplodePod":
         row.actions[0].kind==g::N64HazardKind50NH::breakObject?"N64BreakObject":"N64AnimationToggle";
     n64.push_back(std::move(row));continue;
    }
    Source* slot=sources_.slot(sources_.size(),0x2A00A0u);   // bounds-checked; refused when it would not fit
    need(slot!=nullptr,"trigger row outside the sized trigger store");
    if(i)kinds+=',';
    readRow(tob,b,i+1,zoneCount,*slot,kinds);
    actionTotal_+=slot->actionCount;
   }
   d.trigger=0;
   std::cerr<<"course_triggers site="<<code_<<" count="<<sources_.size()<<" kinds="<<(n?kinds:std::string("-"))<<'\n';
   if(!n64.empty())std::cerr<<"course_triggers_n64 site="<<code_<<" rows="<<n64.size()<<'\n';
   n64Rows_=std::move(n64);ready_=true;return true;
  }catch(const std::exception& e){
   try{d.message=e.what();}catch(...){}
   std::cerr<<"course_triggers site="<<code_<<" trigger="<<d.trigger<<" refused: "<<d.message<<'\n';
   return false;
  }catch(...){d.message="trigger reader allocation failure";return false;}
 }
 // 274E90 for the race's Track: its decoded TOB and SCB zone count.
 bool load(const ct::Track& track,Diagnostic& d)noexcept {
  return load(track.documents.tobDocument,track.code(),track.counts.zones,d);
 }
 // The LostAI targets index the AI spline (2A1260), which is read after this
 // step (PLAN 2.1 step 8). The C80 write the action performs indexes that
 // spline, so every target is bounds-checked against the live node count before
 // the action owner can bind (refused, never clamped).
 bool checkTargets(std::uint32_t aiNodeCount,Diagnostic& d)const noexcept {
  d={};
  if(!ready_){d.message="trigger sources not ready";return false;}
  for(const auto& s:sources_.live())for(const auto& act:s.actions)
   if(act.kind==ActionKind::LostAI&&act.targetNodeIndex>=aiNodeCount){
    d.trigger=s.sourceOrdinal;
    try{d.message="LostAI target "+std::to_string(act.targetNodeIndex)+" outside the AI spline ("+std::to_string(aiNodeCount)+" nodes)";}catch(...){}
    return false;
   }
  return true;
 }
 bool ready()const noexcept{return ready_;}
 const char* code()const noexcept{return code_.c_str();}
 std::size_t liveCount()const noexcept{return ready_?sources_.size():0u;}
 std::span<const Source> sources()const noexcept{return ready_?sources_.live():std::span<const Source>{};}
 // Bounds-checked against the live rows.
 const Source& source(std::size_t i)const{
  const auto* s=ready_?sources_.at(i):nullptr;
  if(!s)throw std::out_of_range("course trigger source outside the live rows");return *s;
 }
 std::uint32_t actionTotal()const noexcept{return ready_?actionTotal_:0u;}
 // The TOB's own animation count (absent [Animations] = 0).
 std::uint32_t animationCount()const noexcept{return ready_?animations_:0u;}
 // N64 hazard rows of the same TOB (PORT EXTENSION, empty on every stock course).
 const std::vector<g::N64HazardRow50NH>& n64Rows()const noexcept{
  static const std::vector<g::N64HazardRow50NH> none;return ready_?n64Rows_:none;}
 bool authors(ActionKind kind)const noexcept{
  for(const auto& s:sources())for(const auto& act:s.actions)if(act.kind==kind)return true;
  return false;
 }
 // Index of the first live trigger authoring `kind`, or liveCount() when none.
 std::size_t firstTriggerWith(ActionKind kind)const noexcept{
  const auto rows=sources();
  for(std::size_t i=0;i<rows.size();++i)for(const auto& act:rows[i].actions)if(act.kind==kind)return i;
  return rows.size();
 }
 const ActionSource* firstPodSound()const noexcept{
  for(const auto& s:sources())for(const auto& act:s.actions)if(act.kind==ActionKind::PodSound)return &act;
  return nullptr;
 }
 std::uint32_t highestAnimationIndex()const noexcept{
  std::uint32_t h=0;
  for(const auto& s:sources())for(const auto& act:s.actions)if(act.kind==ActionKind::Animation)h=std::max(h,act.animationIndex);
  return h;
 }
 // common_track_data role=triggers (and triggers.actions): receipt only for a stock course.
 void logTrackData(std::string_view course,common_race_log50CR::Source source,std::string_view file)const{
  namespace L=common_race_log50CR;
  const bool stock=source==L::Source::stock;
  const auto* r=stock?stock_course_receipts50CR::find(course,"triggers"):nullptr;
  L::trackData(L::Role::triggers,course,file,liveCount(),r!=nullptr,r?r->value:0u);
  L::trackDataNamed("triggers.actions",course,file,actionTotal(),false,0u);
 }
};
} // namespace course_trigger_sources50CT
