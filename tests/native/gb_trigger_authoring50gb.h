#pragma once
#include "gb_cold_sources50la.h"
#include "ga_trigger_authoring_fixture.h"
#include <array>
#include <limits>
#include <memory>
#include <optional>
#include <thread>

namespace gb_trigger_authoring50GB {
namespace a=rr::assets;namespace g=rr::game;namespace detail=ga_trigger_authoring::detail;
inline constexpr std::size_t count=18,lostCount=15,door=15,secondAnimation=16,particles=17;
inline constexpr std::size_t tobBytes=20617;
inline constexpr const char* tobSha="96469b3953b18d940bd0c8772758c68a75a69c3375b49ef98b966901c2e598d1";
enum class Action {LostAI,Animation,PodSound,Particles};
enum class Ignore {None,Player,AI};
enum class Status {Empty,Input,Capacity,Source,Authoring,Ready};
struct Diagnostic {
 Status status=Status::Empty;std::string message;
 std::size_t ordinal=0,retainedPayloadBytes=0,peakReservedPayloadBytes=0;
};
struct ActionSource {
 Action kind=Action::LostAI;std::uint32_t targetNodeIndex=0,animationIndex=0;
 std::optional<std::uint32_t> authoredTargetNodeIndex;std::optional<bool> authoredReset;
 bool effectiveReset1B5A60=false;
 std::string filename;g::Vector3_48C soundPosition{};float minDistance=0,maxDistance=0;
 std::uint32_t emitterNumber=0,instanceNumber=0; // Authored one-based fields.
 std::size_t emitterIndex=0,instanceIndex=0;bool turnOn=true;
 std::optional<bool> authoredOneTimeOnly; // Retained; source reader does not use it.
 const a::ParameterBlock48F* original=nullptr;
};
struct Source {
 std::string name;std::uint32_t sourceOrdinal=0,zone=0,actionCount=0,shapeType=5;
 g::CollisionSphere48J sphere{};bool startEnabled=true,oneTimeOnly=false;
 Ignore ignore=Ignore::None;std::int32_t randomTriggerPercent=100;
 std::optional<bool> authoredStartEnabled;std::optional<float> authoredRandomTriggerPercent;
 std::optional<std::string> authoredIgnoreTrigger;
 std::array<ActionSource,5> actions{};const a::ParameterBlock48F* original=nullptr;
};
// Exact immutable source projection. No callback, collider/query identity,
// live animation, emission state or C80 authority is manufactured here.
class Owner final {
 std::shared_ptr<const gb_cold_sources50LA::Owner> cold_;std::array<Source,count> source_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
 static void add(std::size_t& n,std::size_t x)noexcept {
  n=x>std::numeric_limits<std::size_t>::max()-n?std::numeric_limits<std::size_t>::max():n+x;
 }
 bool fail(Diagnostic& d,Status status,const char* message)noexcept {
  d.status=status;try{d.message=message;}catch(...){}d.retainedPayloadBytes=descriptorPayloadBytes();return false;
 }
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  // Both staged/retained string sets, actual short labels, SHA copy/padding
  // scratch and diagnostics. Shared original source remains separately owned.
  return sizeof(Owner)+sizeof(std::array<Source,count>)+2u*128u*64u+18u*64u+1024u+4u*(tobBytes+64u);
 }
 bool decode(std::shared_ptr<const gb_cold_sources50LA::Owner> cold,
     std::size_t availableAdditionalCpuPayloadBytes,Diagnostic& d){
  d={};
  if(thread_!=std::this_thread::get_id())return fail(d,Status::Input,"GB trigger source thread mismatch");
  if(attempted_)return fail(d,Status::Input,"GB trigger source requires a fresh owner");attempted_=true;
  if(!cold||!cold->ready())return fail(d,Status::Input,"GB trigger source requires actual cold owner");
  d.peakReservedPayloadBytes=maximumAdditionalPayloadBytes();
  if(availableAdditionalCpuPayloadBytes>32u*1024u*1024u||
     d.peakReservedPayloadBytes>availableAdditionalCpuPayloadBytes)
   return fail(d,Status::Capacity,"GB trigger preparation exceeds remaining source allowance");
  try{using namespace detail;
   need(cold&&cold->ready()&&cold->world().zones.size()==13&&cold->world().portals.size()==17&&
    cold->ai().count()==257&&cold->originalTob().size()==tobBytes&&pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tobSha,
    "GB exact actual trigger source identity");
   const auto& p=cold->triggerParameters();need(!p.preservesDuplicateLabels&&p.blocks.size()==70&&p.original_bytes==cold->originalTob()&&
    integer(field(block(p,"triggers"),"numberoftriggers"))==count,"GB original trigger document/count");
   constexpr std::uint32_t targets[lostCount]={189,240,30,253,40,196,46,247,49,85,206,111,228,234,151};
   constexpr std::uint32_t zones[count]={4,4,4,4,4,4,4,4,4,6,6,9,9,12,12,2,2,1};
   constexpr const char* names[lostCount]={"Branch1Right","Branch1Middle","Branch1Left","Branch2Left","Branch2Right","Branch3Right","Branch3Left","Branch4Right","Branch4Left","Branch5Right","Branch5Left","Branch6Right","Branch6Left","Branch7Left","Branch7Right"};
   std::array<Source,count> next{};
   for(std::size_t i=0;i<count;++i){d.ordinal=i+1;auto& s=next[i];const auto label="trigger"+std::to_string(i+1);const auto& b=block(p,label);
    s.original=&b;s.sourceOrdinal=static_cast<std::uint32_t>(i+1);s.zone=integer(field(b,"zone"));
    s.actionCount=integer(field(b,"numberofactions"));s.sphere.center=vector(field(b,"position"));s.sphere.radius=number(field(b,"radius"));
    s.oneTimeOnly=boolean(field(b,"onetimeonly"));if(const auto* n=optional(b,"name"))s.name=*n;
    if(const auto* v=optional(b,"startenabled")){s.authoredStartEnabled=boolean(*v);s.startEnabled=*s.authoredStartEnabled;}
    if(const auto* v=optional(b,"randomtriggerpercent")){s.authoredRandomTriggerPercent=number(*v);need(*s.authoredRandomTriggerPercent==100.f,"GB exact trigger percent");}
    if(const auto* v=optional(b,"ignoretrigger")){s.authoredIgnoreTrigger=*v;
     if(*v=="PLAYER")s.ignore=Ignore::Player;else{need(*v=="AI","GB actual ignore selector");s.ignore=Ignore::AI;}}
    need(s.zone==zones[i]&&s.actionCount==(i==door?2u:i==particles?5u:1u)&&s.oneTimeOnly==(i>=door)&&
     s.startEnabled==(i!=5&&i!=6)&&s.ignore==(i<lostCount?Ignore::Player:i==particles?Ignore::AI:Ignore::None)&&
     !optional(b,"parent")&&!optional(b,"attachtosoultree")&&!optional(b,"box")&&!optional(b,"yaw")&&
     !optional(b,"pitch")&&!optional(b,"roll")&&std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,
     "GB exact sphere state/attachment fields");
    if(i<lostCount){need(s.name==names[i]&&s.authoredStartEnabled&&s.authoredRandomTriggerPercent&&s.authoredIgnoreTrigger&&b.fields.size()==9,
      "GB authored LostAI activation fields");}
    else need(s.name.empty()&&!s.authoredStartEnabled&&!s.authoredRandomTriggerPercent&&b.fields.size()==(i==particles?6u:5u),
      "GB actual special trigger defaults");
    for(std::size_t j=0;j<s.actionCount;++j){auto& action=s.actions[j];const auto& b=block(p,label+"action"+std::to_string(j+1));action.original=&b;
     if(const auto* once=optional(b,"onetimeonly"))action.authoredOneTimeOnly=boolean(*once);
     if(i<lostCount){
      need(j==0&&b.fields.size()==2&&field(b,"actiontype")=="LostAI"&&!optional(b,"reset"),"GB actual LostAI action");
      action.authoredTargetNodeIndex=integer(field(b,"targetnodeindex"));action.targetNodeIndex=*action.authoredTargetNodeIndex;
      need(action.targetNodeIndex==targets[i]&&action.targetNodeIndex<cold->ai().count(),"GB LostAI actual target bound");
     }else if(i==particles){
      need(field(b,"actiontype")=="Particles"&&b.fields.size()==5&&action.authoredOneTimeOnly==std::optional<bool>{true},"GB actual Particles action shape");
      action.kind=Action::Particles;action.emitterNumber=integer(field(b,"emitternumber"));action.instanceNumber=integer(field(b,"instancenumber"));
      action.turnOn=boolean(field(b,"turnon"));need(action.emitterNumber==8+j&&action.instanceNumber==1&&action.turnOn,"GB Particles exact ordered target");
      action.emitterIndex=action.emitterNumber-1;action.instanceIndex=action.instanceNumber-1;
      const auto& emitter=block(p,"particleemitter"+std::to_string(action.emitterNumber));
      need(integer(field(emitter,"typeid"))==11&&integer(field(emitter,"numberofinstances"))==1&&
       !boolean(field(emitter,"starton")),"GB Particles target is same disabled spark descriptor");
     }else if(j==0){
      need(field(b,"actiontype")=="Animation"&&b.fields.size()==3&&action.authoredOneTimeOnly==std::optional<bool>{true}&&
       integer(field(b,"animationnumber"))==(i==door?1u:3u),"GB actual Animation action");
      action.kind=Action::Animation;action.animationIndex=i==door?0u:2u;
     }else{
      need(i==door&&j==1&&field(b,"actiontype")=="PodSound"&&b.fields.size()==5,"GB original PodSound action shape");
      action.kind=Action::PodSound;action.filename=field(b,"filename");action.soundPosition=vector(field(b,"position"));
      action.minDistance=number(field(b,"mindistance"));action.maxDistance=number(field(b,"maxdistance"));
      need(action.filename=="sfx\\race\\bigdoor.vag"&&action.soundPosition.v[0]==8012.f&&action.soundPosition.v[1]==2073.f&&
       action.soundPosition.v[2]==15896.f&&action.minDistance==0.f&&action.maxDistance==2500.f,"GB own PodSound source identity");
     }
    }
   }
   source_=std::move(next);cold_=std::move(cold);
   need(descriptorPayloadBytes()<=maximumAdditionalPayloadBytes(),"GB descriptor reservation exceeded");
   ready_=true;d.status=Status::Ready;d.message.clear();d.retainedPayloadBytes=descriptorPayloadBytes();return true;
  }catch(const std::exception& e){return fail(d,Status::Authoring,e.what());}
   catch(...){return fail(d,Status::Authoring,"GB trigger authoring failed");}
 }
 // Compatibility for aggregates that already reserve maximumAdditionalPayloadBytes()
 // before calling. This overload does not account for other owners on their behalf.
 bool decode(std::shared_ptr<const gb_cold_sources50LA::Owner> cold,std::string& error){
  Diagnostic d;const bool okay=decode(std::move(cold),maximumAdditionalPayloadBytes(),d);error=d.message;return okay;
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("GB trigger source unavailable");return source_.at(i);}
 const gb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{if(!ready())throw std::runtime_error("GB trigger source unavailable");return cold_->originalTob();}
 std::size_t descriptorPayloadBytes()const noexcept {
  if(thread_!=std::this_thread::get_id())return 0;
  std::size_t n=sizeof(Owner);for(const auto& s:source_){add(n,s.name.capacity());add(n,1);
   if(s.authoredIgnoreTrigger){add(n,s.authoredIgnoreTrigger->capacity());add(n,1);}
   for(const auto& a:s.actions){add(n,a.filename.capacity());add(n,1);}}return n;
 }
};
}
