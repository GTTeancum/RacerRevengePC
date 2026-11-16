#pragma once
// Course trigger sources for the legacy-ground courses (MA, SA, GA) and MB: ONE rule,
// read from the course's own TOB by the common trigger reader
// (sc_trigger_authoring50la.h, the reader SC already uses). The original's
// trigger reader walks [Triggers] NumberOfTriggers of whatever TOB the course
// archive holds; nothing checks a stock checksum or count. A TOB without a
// [Triggers] block (or NumberOfTriggers 0) has no trigger owners.
//
// The stock archive/TOB identities and counts are receipts
// (course_archive_receipt50ci.h). Storage is capacity-sized (the common
// reader's capacity) with a live count, as SC's; each native chain's own
// trigger group refuses a live set above ITS capacity (GA's 13).
#include "course_archive_receipt50ci.h"
#include "sc_trigger_authoring50la.h"
#include "ga_trigger_authoring_fixture.h"   // Action kinds + shared text helpers (unchanged)
#include "ma_trigger_authoring_fixture.h"   // native box helpers boxWorld/boxRadius (unchanged)
#include "sa_trigger_authoring_fixture.h"   // stock receipt values only (unchanged)
#include "../../src/game/native_trigger_box_geometry50ma.h"
#include "../../src/game/native_trigger_group50ga.h"
#include "../../src/game/terrain_query48j.h"
#include <array>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace course_trigger_sources50CT {
namespace a=rr::assets;namespace g=rr::game;
inline constexpr std::size_t capacity=sc_trigger_authoring50LA::count;
using Action=ga_trigger_authoring::Action;
// What the consuming native chain implements. The reader reads every authored
// value; a chain that does not implement one refuses it here (flagged stand-in)
// instead of ignoring it. Defaults = the MA/SA/GA chains.
struct Support {
 bool reset=false;          // LostAI Reset TRUE (pose reset): MB's chain implements it
 bool ignoreAi=false;       // IgnoreTrigger AI
 bool randomPercent=false;  // RandomTriggerPercent other than 100
};
struct ActionSource {
 Action kind=Action::LostAI;
 std::uint32_t targetNodeIndex=0,animationIndex=0;
 bool reset=false;
 g::Vector3_48C position{};
 float minDistance=0.f,maxDistance=0.f;
 std::string filename;
};
// One native trigger source shape for every course chain (MA, SA and GA used
// three copies of it). shapeType 5 = sphere, 6 = box (sphere = coarse bound).
struct Source {
 std::string name;
 std::uint32_t zone=0,actionCount=0,shapeType=5,targetNodeIndex=0;
 g::CollisionSphere48J sphere{};
 g::NativeTriggerBox50MA box{};
 float yaw=0,pitch=0,roll=0;
 bool startEnabled=true,oneTimeOnly=false,ignorePlayer=false,ignoreAi=false,reset=false;
 std::int32_t randomTriggerPercent=100;
 std::array<ActionSource,sc_trigger_authoring50LA::maxActions> actions{};
};
// Stock values of one course slot: receipts only.
struct Stock {
 const char* code;const char* tobMember;
 const char* archiveSha;std::size_t archiveBytes;
 const char* tobSha;std::size_t tobBytes;
 std::size_t triggers;
};
class Owner final {
 std::vector<std::uint8_t> tob_;
 a::ParameterDocument48F document_;
 sc_trigger_authoring50LA::Owner reader_;
 std::array<Source,capacity> sources_{};
 std::size_t live_=0;std::uint32_t animations_=0;
 bool attempted_=false,ready_=false,stockTob_=false;
 static void need(bool value,const std::string& reason){if(!value)throw std::runtime_error(reason);}
public:
 Owner()=default;Owner(const Owner&)=delete;Owner&operator=(const Owner&)=delete;
 Owner(Owner&&)=delete;Owner&operator=(Owner&&)=delete;
 // zoneCount: the loaded world's zones. aiNodeBound: native AI spline storage
 // here; the exact live AI node count is checked again per target when the
 // action owner binds its destinations (cold contacts).
 bool load(const std::filesystem::path& archivePath,const Stock& stock,std::size_t zoneCount,
     std::uint32_t aiNodeBound,std::string& error,Support support={}){
  if(attempted_){error="course trigger owner requires a fresh stable lifetime";return false;}attempted_=true;
  try{using namespace ga_trigger_authoring::detail;namespace f=pod_materials50ed_fixture::detail;
   const auto site=std::string("triggers_")+stock.code;
   f::Inputs input;input.capacity=course_archive_receipt50CI::maximumArchiveBytes();
   const auto bytes=input.read(archivePath,input.capacity);
   course_archive_receipt50CI::receipt(site.c_str(),stock.code,bytes,stock.archiveSha,stock.archiveBytes);
   a::ResArchive48C archive;a::DecodeError48C why{};
   need(a::decodeResource48C(bytes,archive,why,nullptr,course_archive_receipt50CI::limits()),
       stock.code+std::string(" trigger course archive rejected"));
   need(a::extractResMember48C(archive,stock.tobMember,tob_,why)&&!tob_.empty(),
       stock.code+std::string(" trigger TOB member missing"));
   stockTob_=course_archive_receipt50CI::memberReceipt(site.c_str(),stock.tobMember,tob_,0,0,stock.tobBytes,stock.tobSha);
   need(a::decodeParameters48F(tob_,document_,why),stock.code+std::string(" trigger TOB rejected"));
   // [Animations] NumberOfAnimations of the same TOB (absent block = 0).
   for(const auto& b:document_.blocks)if(b.name=="animations")
    if(const auto* n=optional(b,"numberofanimations")){need(!animations_,stock.code+std::string(" duplicate [Animations]"));animations_=integer(*n);}
   sc_trigger_authoring50LA::Owner::Diagnostic diagnostic;
   need(reader_.decode(document_,zoneCount,aiNodeBound,stock.code,diagnostic),
       stock.code+std::string(" trigger authoring rejected (see course_triggers line)"));
   const auto* input2=reader_.sources();const auto live=reader_.liveCount();
   need(input2&&live<=capacity,stock.code+std::string(" trigger extent exceeds the native trigger group"));
   course_archive_receipt50CI::countReceipt(site.c_str(),"triggers",live,stock.triggers);
   std::array<Source,capacity> next{};
   for(std::size_t i=0;i<live;++i){const auto& raw=(*input2)[i];auto& s=next[i];
    const auto label=stock.code+std::string(" trigger")+std::to_string(i+1)+": ";
    // Native values are converted from the authored text with the same
    // conversion the three course chains used, so stock triggers stay
    // bit-identical. PORT STAND-IN (flagged): the reader's own decimal parse
    // (parseTrackNumber50AD, SC) is the common one; unify when the chains merge.
    const auto& b=*raw.originalTrigger;
    s.name=raw.name;s.zone=raw.sourceZoneOrdinal;s.actionCount=raw.actionCount;
    s.startEnabled=raw.startEnabled;s.oneTimeOnly=raw.oneTimeOnly;
    s.sphere.center=vector(field(b,"position"));
    // PORT STAND-IN (flagged): a native chain without a trigger-chance roll
    // or an AI ignore list refuses those authored values (Support).
    need(support.randomPercent||raw.randomTriggerPercent==100.f,label+"RandomTriggerPercent other than 100 is not supported natively");
    s.randomTriggerPercent=static_cast<std::int32_t>(raw.randomTriggerPercent);
    if(raw.ignoreTrigger){
     if(*raw.ignoreTrigger=="PLAYER")s.ignorePlayer=true;
     else{need(support.ignoreAi&&*raw.ignoreTrigger=="AI",label+"IgnoreTrigger other than PLAYER is not supported natively");s.ignoreAi=true;}
    }
    if(raw.shape==sc_trigger_authoring50LA::Shape::sphere){
     s.shapeType=5;s.sphere.radius=number(field(b,"radius"));need(s.sphere.radius>0.f,label+"sphere radius rejected");
    }else{
     s.shapeType=6;s.box.half=vector(field(b,"box"));
     s.yaw=number(field(b,"yaw"));s.pitch=number(field(b,"pitch"));s.roll=number(field(b,"roll"));
     s.box.world=ma_trigger_authoring::detail::boxWorld(s.sphere.center,s.yaw,s.pitch,s.roll);
     need(g::validNativeTriggerBox50MA(s.box),label+"native box geometry rejected");
     s.sphere.radius=ma_trigger_authoring::detail::boxRadius(s.box);
     need(std::isfinite(s.sphere.radius)&&s.sphere.radius>0.f,label+"box coarse radius rejected");
    }
    for(std::uint32_t k=0;k<raw.actionCount;++k){const auto& in=raw.actions[k];auto& out=s.actions[k];
     const auto& ab=*in.original;
     switch(in.kind){
      case sc_trigger_authoring50LA::ActionKind::LostAI:
       out.kind=Action::LostAI;out.targetNodeIndex=in.targetAiNodeIndex;
       // 1B5A60/1B5A98 initialise Reset=0; 1B27C0 then writes racer.C80 only.
       // PORT STAND-IN (flagged): a chain without the pose reset refuses an
       // authored Reset TRUE (MB's chain implements it: Support::reset).
       need(support.reset||!in.authoredReset.value_or(false),label+"LostAI Reset is not supported natively");
       out.reset=in.authoredReset.value_or(false);
       break;
      case sc_trigger_authoring50LA::ActionKind::Animation:
       out.kind=Action::Animation;out.animationIndex=in.animationNumber-1;break;
      case sc_trigger_authoring50LA::ActionKind::PodSound:
       out.kind=Action::PodSound;out.filename=in.filename;out.position=vector(field(ab,"position"));
       out.minDistance=number(field(ab,"mindistance"));out.maxDistance=number(field(ab,"maxdistance"));break;
      case sc_trigger_authoring50LA::ActionKind::Sound:
      case sc_trigger_authoring50LA::ActionKind::Particles:
       // PORT STAND-IN (flagged): the MA/SA/GA/MB native chains have no
       // ordinary Sound or Particles trigger action; refused, not ignored.
       need(false,label+"Sound/Particles trigger action is not supported on this native chain");break;
     }
    }
    // Action-1 mirror for the LostAI-only chains (MA layout).
    if(s.actions[0].kind==Action::LostAI){s.targetNodeIndex=s.actions[0].targetNodeIndex;s.reset=s.actions[0].reset;}
   }
   sources_=std::move(next);live_=live;ready_=true;error.clear();return true;
  }catch(const std::exception& e){error=e.what();return false;}
 }
 bool ready()const noexcept{return ready_;}
 // The TOB's own animation count, and whether the TOB is the stock bytes.
 std::uint32_t animationCount()const noexcept{return ready_?animations_:0;}
 bool stockTob()const noexcept{return ready_&&stockTob_;}
 std::size_t liveCount()const noexcept{return ready_?live_:0;}
 // Bounds-checked against the live prefix, never the capacity tail.
 const Source& source(std::size_t i)const{
  if(!ready_||i>=live_)throw std::out_of_range("course trigger source outside the live prefix");return sources_[i];
 }
 bool authors(Action kind)const noexcept{
  for(std::size_t i=0;i<liveCount();++i)for(std::uint32_t k=0;k<sources_[i].actionCount;++k)
   if(sources_[i].actions[k].kind==kind)return true;
  return false;
 }
 // First PodSound action authored by the data (its filename names the voice).
 const ActionSource* firstPodSound()const noexcept{
  for(std::size_t i=0;i<liveCount();++i)for(std::uint32_t k=0;k<sources_[i].actionCount;++k)
   if(sources_[i].actions[k].kind==Action::PodSound)return &sources_[i].actions[k];
  return nullptr;
 }
 std::uint32_t highestAnimationIndex()const noexcept{
  std::uint32_t h=0;bool any=false;
  for(std::size_t i=0;i<liveCount();++i)for(std::uint32_t k=0;k<sources_[i].actionCount;++k)
   if(sources_[i].actions[k].kind==Action::Animation){any=true;h=std::max(h,sources_[i].actions[k].animationIndex);}
  return any?h:0;
 }
 const std::vector<std::uint8_t>& originalTob()const{
  if(!ready_)throw std::runtime_error("course trigger TOB owner not ready");return tob_;
 }
};
// Stock receipts for the three slots on this path.
inline constexpr Stock stockMa{"MA","tracks\\ma.tob",ma_trigger_authoring::archiveSha,ma_trigger_authoring::archiveBytes,
 ma_trigger_authoring::tobSha,ma_trigger_authoring::tobBytes,ma_trigger_authoring::count};
inline constexpr Stock stockSa{"SA","tracks\\sa.tob",sa_trigger_authoring::archiveSha,sa_trigger_authoring::archiveBytes,
 sa_trigger_authoring::tobSha,sa_trigger_authoring::tobBytes,sa_trigger_authoring::count};
inline constexpr Stock stockGa{"GA","tracks\\ga.tob",watchtower_source_fixture::archiveSha,watchtower_source_fixture::archiveBytes,
 watchtower_source_fixture::objects.sha,watchtower_source_fixture::objects.bytes,ga_trigger_authoring::count};
// MB (receipts only; values from the MB route's own stock pins).
inline constexpr Stock stockMb{"MB","tracks\\mb.tob","6aae7d59981cae6c741b14e147021960fdb4d8e68e21c217a446f779d3c821ca",4421779,
 "f25f10fff204f124e5525ff2c4a7c1e72f9ce17476e1404d1932f6ab6e8c776d",19909,3};
} // namespace course_trigger_sources50CT
