#pragma once
#include "tb_door_motion50tb.h"
#include <optional>

namespace tb_trigger_authoring50TB {
namespace a=rr::assets;namespace g=rr::game;namespace detail=tb_door_motion50TB::detail;
inline constexpr std::size_t count=16,lostCount=15,door=15,sphereCount=9,boxCount=7;
enum class Action {LostAI,Animation,PodSound};
struct ActionSource {
 Action kind=Action::LostAI;std::uint32_t targetNodeIndex=0,animationIndex=0;
 std::optional<std::uint32_t> authoredTargetNodeIndex;
 std::optional<bool> authoredReset;bool effectiveReset1B5A60=false;
 std::optional<std::string> unusedTargetNodeIndex135;
 std::string filename;g::Vector3_48C soundPosition{};float minDistance=0,maxDistance=0;
 const a::ParameterBlock48F* original=nullptr;
};
struct Source {
 std::string name;std::uint32_t sourceOrdinal=0,zone=0,actionCount=0,shapeType=5;
 g::CollisionSphere48J sphere{};g::NativeTriggerBox50MA box{};float yaw=0,pitch=0,roll=0;
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false;std::int32_t randomTriggerPercent=100;
 std::optional<bool> authoredStartEnabled;std::optional<float> authoredRandomTriggerPercent;
 std::optional<std::string> authoredIgnoreTrigger;
 std::array<ActionSource,2> actions{};const a::ParameterBlock48F* original=nullptr;
};
// Actual complete source document remains shared. All numeric/shape fields are
// authored projections; this class grants no collider ID, C80 lease or action.
class Owner final {
 std::shared_ptr<const tb_cold_sources50LA::Owner> cold_;std::array<Source,count> source_{};
 const std::thread::id thread_=std::this_thread::get_id();bool attempted_=false,ready_=false;
public:
 Owner()=default;Owner(const Owner&)=delete;Owner& operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner& operator=(Owner&&)=delete;
 ~Owner(){if(thread_!=std::this_thread::get_id())std::terminate();}
 // Exact authenticated16-row projection: at most96 retained string slots,
 // each actual value/label under32 bytes. The selected libstdc++ fresh string
 // path fits64 bytes per slot; charge two full sets plus all16 live labels,
 // the staged Source array and1024 bytes for diagnostic temporaries.
 // SHA256 copies the full authenticated TOB then pads it; separately reserve
 // four padded source lengths for its selected vector copy/reallocation peak.
 // Shared original TOB/World is retained elsewhere, never decoded again here.
 static constexpr std::size_t maximumAdditionalPayloadBytes()noexcept {
  return sizeof(Owner)+sizeof(std::array<Source,count>)+2u*96u*64u+16u*64u+1024u+
    4u*(tb_door_motion50TB::tobBytes+64u);
 }
 bool decode(std::shared_ptr<const tb_cold_sources50LA::Owner> cold,std::string& error){
  if(thread_!=std::this_thread::get_id()){error="TB trigger source thread mismatch";return false;}
  if(attempted_){error="TB trigger source requires a fresh owner";return false;}attempted_=true;
  try{using namespace detail;
   need(cold&&cold->ready()&&cold->world().zones.size()==10&&cold->world().portals.size()==11&&
     cold->ai().count()==269&&cold->originalTob().size()==tb_door_motion50TB::tobBytes&&
     pod_materials50ed_fixture::detail::sha256(cold->originalTob())==tb_door_motion50TB::tobSha,"TB same actual source world/AI identity");
   const auto& p=cold->triggerParameters();need(p.preservesDuplicateLabels&&p.original_bytes==cold->originalTob()&&
     integer(field(block(p,"triggers"),"numberoftriggers"))==count,"TB original trigger document/count");
   constexpr std::uint32_t targets[lostCount]={149,28,53,191,83,218,119,120,135,122,125,126,128,134,0};
   constexpr std::uint32_t zones[count]={3,3,4,5,5,4,7,7,7,7,7,7,7,7,7,4};
   constexpr const char* names[lostCount]={"Fork1Left","Fork1Right","BigJump","ForkLastLeft","ForkLastRight","Pit",
     "Canyon1","Canyon2","Canyon9","Canyon3","Canyon4","Canyon5","Canyon6","Canyon8","Canyon7"};
   std::array<Source,count> next{};std::size_t boxes=0,spheres=0;
   for(std::size_t i=0;i<count;++i){auto& s=next[i];const auto label="trigger"+std::to_string(i+1);const auto& b=block(p,label);
    s.original=&b;s.sourceOrdinal=static_cast<std::uint32_t>(i+1);s.zone=integer(field(b,"zone"));
    s.actionCount=integer(field(b,"numberofactions"));s.sphere.center=vector(field(b,"position"));s.oneTimeOnly=boolean(field(b,"onetimeonly"));
    if(const auto* v=optional(b,"name"))s.name=*v;
    if(const auto* v=optional(b,"startenabled")){s.authoredStartEnabled=boolean(*v);s.startEnabled=*s.authoredStartEnabled;}
    if(const auto* v=optional(b,"randomtriggerpercent")){s.authoredRandomTriggerPercent=number(*v);need(*s.authoredRandomTriggerPercent==100.f,"TB trigger percent route");}
    if(const auto* v=optional(b,"ignoretrigger")){s.authoredIgnoreTrigger=*v;need(*v=="PLAYER","TB actual ignore selector");s.ignorePlayer=true;}
    need(s.zone==zones[i]&&s.actionCount==(i==door?2u:1u)&&s.oneTimeOnly==(i==door)&&s.startEnabled&&
      s.ignorePlayer==(i!=door)&&!optional(b,"parent")&&!optional(b,"attachtosoultree"),"TB exact trigger state/attachment");
    if(i!=door)need(s.name==names[i]&&s.authoredStartEnabled==std::optional<bool>{true}&&s.authoredRandomTriggerPercent&&
      s.authoredIgnoreTrigger,"TB authored LostAI activation fields");
    if(const auto* half=optional(b,"box")){
     need(i==2||(i>=9&&i<=14),"TB exact source box order");++boxes;s.shapeType=6;need(!optional(b,"radius"),"TB box radius conflict");
     s.box.half=vector(*half);s.yaw=number(field(b,"yaw"));s.pitch=number(field(b,"pitch"));s.roll=number(field(b,"roll"));
     s.box.world=ma_trigger_authoring::detail::boxWorld(s.sphere.center,s.yaw,s.pitch,s.roll);
     need(g::validNativeTriggerBox50MA(s.box),"TB native shape6 projection rejected");s.sphere.radius=ma_trigger_authoring::detail::boxRadius(s.box);
    }else{++spheres;need(!optional(b,"yaw")&&!optional(b,"pitch")&&!optional(b,"roll"),"TB sphere orientation route");s.sphere.radius=number(field(b,"radius"));}
    need(std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,"TB coarse sphere rejected");
    const auto& first=block(p,label+"action1");auto& action=s.actions[0];action.original=&first;
    if(i!=door){
     need(field(first,"actiontype")=="LostAI"&&!optional(first,"reset"),"TB LostAI Reset source changed");
     if(const auto* value=optional(first,"targetnodeindex"))action.authoredTargetNodeIndex=integer(*value);
     if(i==14){const auto* typo=optional(first,"targetnodeindex135");need(!action.authoredTargetNodeIndex&&typo&&*typo=="130","TB exact unused target typo");action.unusedTargetNodeIndex135=*typo;}
     else need(action.authoredTargetNodeIndex.has_value()&&!optional(first,"targetnodeindex135"),"TB actual LostAI target field");
     // Exact original1B5A78->158B30 default0; the misspelled field is unconsumed.
     action.targetNodeIndex=action.authoredTargetNodeIndex.value_or(0);
     need(action.targetNodeIndex==targets[i]&&action.targetNodeIndex<cold->ai().count(),"TB LostAI actual target bound");
    }else{
     need(s.name.empty()&&!s.authoredStartEnabled&&!s.authoredRandomTriggerPercent&&!s.authoredIgnoreTrigger,
       "TB door exact omitted activation fields changed");
     need(field(first,"actiontype")=="Animation"&&integer(field(first,"animationnumber"))==1&&boolean(field(first,"onetimeonly"))&&
       !optional(first,"timetillreset")&&!optional(first,"animationnameref")&&!optional(first,"specificobject"),"TB original Animation action");
     action.kind=Action::Animation;action.animationIndex=0;
     const auto& second=block(p,label+"action2");auto& sound=s.actions[1];sound.original=&second;sound.kind=Action::PodSound;
     need(field(second,"actiontype")=="PodSound"&&second.fields.size()==5,"TB original PodSound action shape");
     sound.filename=field(second,"filename");sound.soundPosition=vector(field(second,"position"));
     sound.minDistance=number(field(second,"mindistance"));sound.maxDistance=number(field(second,"maxdistance"));
     need(sound.filename=="sfx\\race\\hugedoor.vag"&&sound.soundPosition.v[0]==15107.f&&sound.soundPosition.v[1]==1673.f&&
       sound.soundPosition.v[2]==14695.f&&sound.minDistance==0.f&&sound.maxDistance==2500.f,"TB own PodSound source identity");
    }
   }
   need(boxes==boxCount&&spheres==sphereCount,"TB exact trigger shape count");
   source_=std::move(next);cold_=std::move(cold);
   need(descriptorPayloadBytes()<=maximumAdditionalPayloadBytes(),"TB trigger descriptor reservation exceeded");
   ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return thread_==std::this_thread::get_id()&&ready_&&cold_&&cold_->ready();}
 const Source& source(std::size_t i)const{if(!ready())throw std::runtime_error("TB trigger source unavailable");return source_.at(i);}
 const tb_cold_sources50LA::Owner* coldSource()const noexcept{return ready()?cold_.get():nullptr;}
 const std::vector<std::uint8_t>& originalTob()const{if(!ready())throw std::runtime_error("TB trigger source unavailable");return cold_->originalTob();}
 std::size_t descriptorPayloadBytes()const noexcept {
  using namespace detail;std::size_t n=sizeof(Owner);
  for(const auto& s:source_){textPayload(n,s.name);if(s.authoredIgnoreTrigger)textPayload(n,*s.authoredIgnoreTrigger);
   for(const auto& a:s.actions){textPayload(n,a.filename);if(a.unusedTargetNodeIndex135)textPayload(n,*a.unusedTargetNodeIndex135);}}
  return n;
 }
};
}
