// Authentic original allocation and roster operations; no owner relocation.
#include "terrain_interactive_roster_cold50du.h"
#include "original_results_roster_read50la.h"
#include "../../src/game/original_racer_selection50ij.h"
#include "../../src/game/original_opponent_selection50kg.h"
#include "../../platform/linux/native/original_ui_rider_profile50kg.h"
#include "original_ui_main_entry50kg.h"
#include "ra_cold_roster_operation50la.h"
#include "ra_cold_roster_resources50la.h"
#include "original_course_roster50kg_fixture.h"
#include "native_racer_rig50hw_fixture.h"
#include "native_racer_collision50hw_fixture.h"
#include "runtime_startup50fl_fixture.h"
#include "runtime_asset_paths50lb.h"
#include "runtime_allocation_counter50fl.h"
#include <algorithm>
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <utility>
namespace interactive_roster_cold50DU {
namespace f=runtime_startup50fl;
using namespace rr::game;
namespace {
void require50DU(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
// Cold startup only: add resource identity/stage without changing successful
// operations. Keep the original exception nested and its detail in what().
template<class Operation>
decltype(auto) coldRacerStage50DU(const std::string& podId,const char* stage,
    Operation&& operation) {
    try { return operation(); }
    catch(const std::exception& error) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: "+error.what()));
    }
    catch(...) {
        std::throw_with_nested(std::runtime_error(
            "Original racer "+podId+" ["+stage+"]: non-standard exception"));
    }
}
constexpr std::uint32_t raceRacerCapacity=rr::game::defaultHostRaceRacers50HT;
}
std::shared_ptr<ra_cold_roster50LA::SharedOwners> makeSharedOwners() {
 return std::make_shared<ra_cold_roster50LA::SharedOwners>();
}
std::shared_ptr<ra_cold_roster50LA::RuntimeOwners> makeRuntimeOwners() {
 return std::make_shared<ra_cold_roster50LA::RuntimeOwners>();
}
void Context::run() const {
 auto& mainQueryResource=this->mainQueryResource;
 auto& directOriginalInputs=this->directOriginalInputs;
 auto& playerRacer60=this->playerRacer60;
 auto& courseRaceCold=this->courseRaceCold;
 auto& playerPodId=this->playerPodId;
 auto& raceRacerCount=this->raceRacerCount;
 auto& playerUsesRosterRig=this->playerUsesRosterRig;
 auto& racerRigs=this->racerRigs;
 auto& racerCollisions=this->racerCollisions;
 auto& endpointPolicy=this->endpointPolicy;
 auto& racerNames=this->racerNames;
 auto& mainPairCapacity=this->mainPairCapacity;
 auto& secondaryPairCapacity=this->secondaryPairCapacity;
 const auto& matchesEndpointPolicy=this->matchesEndpointPolicy;
{
  require50DU(matchesEndpointPolicy(mainQueryResource),"retained main endpoint-W host policy differs");
  const auto rosterRoot=runtime_asset_paths50LB::resolve("evidence/originalRoster50HW/raw-imports");
  using Profile=rr::platform::OriginalUiRiderProfileOwner50KG;
  Profile::CourseRaceSelection mode2Selection{};
  Profile::TournamentCareerSnapshot mode2Career{};
  rr::game::OriginalUiCourseRememberedProfile50KG mode2Course{};
  rr::platform::OriginalUiRiderProfileStamp50KG mode2Stamp{};
  bool canonicalMode2=false;
  if(sourceUiHumanCourse){
   require50DU(courseProfile&&courseProfile->courseRaceSnapshot(mode2Selection),
       "UI roster requires its actual retained Course profile");
   if(mode2Selection.mode34==2&&directOriginalInputs){
    mode2Stamp=courseProfile->stamp();
    require50DU(directOriginalInputs&&courseRaceCold&&
        courseProfile->selectedTournamentCourseSnapshot(mode2Career,mode2Course)&&
        mode2Stamp.mode34==2&&mode2Career.stamp.mode34==2&&mode2Course.mode34==2&&
        mode2Career.stamp.generation==mode2Stamp.generation&&
        courseProfile->stamp().generation==mode2Stamp.generation&&
        mode2Career.career100C0==playerRacer60&&mode2Selection.rider==playerRacer60&&
        mode2Selection.subtype==1&&mode2Selection.entrants==raceRacerCount&&
        raceRacerCount==raceRacerCapacity&&((mode2Selection.packed1125C>>5)&3u)==1u&&
        ((mode2Selection.packed1125C>>7)&15u)==7u&&
        (mode2Career.progress219C&31u)<mode2Course.courseOrder100C4.size()&&
        mode2Selection.course==mode2Course.courseOrder100C4[mode2Career.progress219C&31u],
        "canonical mode2 field lost selected career/Course identity");
    canonicalMode2=true;
   }
  }
  auto roster=[&](){
   if(!canonicalMode2)return directOriginalInputs?
       original_course_roster50KG::directDiscRoster(playerRacer60):
       native_racer50HW::rig_detail::json(rosterRoot/"roster.json");
   namespace selection=rr::game::original_racer_selection50IJ;
   namespace identity=original_player_identity50IK;
   namespace original=rr::game::original_opponent_selection50KG;
   const auto order=original::courseIndex187B10(mode2Course.courseOrder100C4,
       static_cast<std::uint8_t>(mode2Selection.course))+1u;
   const auto klass=order<5u?1u:(order<9u?2u:3u);
   std::array<original::RacerId,7> opponents{};
   require50DU(original::deterministic188470(klass,static_cast<original::RacerId>(playerRacer60),opponents),
       "original188470 deterministic field rejected");
   native_racer50HW::J result={{"schema","RR_ORIGINAL_DISC_ROSTER_V1"},{"player_index",0},
       {"racers",native_racer50HW::J::array()}};
   // Identity planning is pure: no RNG, profile mutation or racer allocation.
   // Existing per-slot pipeline below authenticates every selected archive/PHY.
   for(std::size_t slot=0;slot<raceRacerCapacity;++slot){
    const auto id=slot?static_cast<std::uint32_t>(opponents[slot-1]):playerRacer60;
    const auto* entry=selection::findByRacer60(id);const auto* source=identity::find(id);
    require50DU(entry&&source,"canonical opponent has no authenticated resource identity");
    result["racers"].push_back(native_racer50HW::J{{"pod_id",std::string(entry->podId)},
        {"resource_sha256",source->archiveSha256},{"phy_sha256",source->phySha256}});
   }
   return result;
  }();
  require50DU(roster.at("player_index")==0&&roster.at("racers").size()==raceRacerCapacity,
      "original roster candidate must contain eight distinct source identities");
  require50DU(directOriginalInputs||roster.at("racers").at(0).at("pod_id")=="POD01",
      "retained original roster player identity differs");
  if(courseRaceCold&&!directOriginalInputs){
   std::string rosterError;
   const bool selected=original_course_roster50KG::selectPreparedRider(".",rosterRoot,roster,playerRacer60,rosterError);
   require50DU(selected,rosterError.empty()?"Course selected original racer cold owner is unavailable":rosterError.c_str());
  }
  std::size_t selectedSlot=raceRacerCapacity;
  for(std::size_t racer=0;racer<raceRacerCapacity;++racer)
   if(roster.at("racers").at(racer).at("pod_id")==playerPodId){
    require50DU(selectedSlot==raceRacerCapacity,"duplicate selected original racer");selectedSlot=racer;
   }
  require50DU(selectedSlot<raceRacerCapacity,"selected original racer absent from retained roster");
  // Swap complete authenticated entries, preserving all eight distinct racers.
  // The selected pod's former opponent slot receives Anakin's original resources.
  if(selectedSlot)std::swap(roster.at("racers").at(0),roster.at("racers").at(selectedSlot));
  // Only the authored active prefix is instantiated, registered or drawn.
  // Inactive capacity does not allocate pods, participate in AI, or become an
  // invisible opponent. Preserve Player0 before trimming the real roster.
  auto& activeRoster=roster.at("racers");
  activeRoster.erase(activeRoster.begin()+raceRacerCount,activeRoster.end());
  for(std::size_t racer=0;racer<raceRacerCount;++racer){
   const auto& entry=roster.at("racers").at(racer);
   const auto id=entry.at("pod_id").get<std::string>();
   coldRacerStage50DU(id,"roster identity",[&](){
    require50DU(id.size()==5&&id.substr(0,3)=="POD"&&
        id[3]>='0'&&id[3]<='9'&&id[4]>='0'&&id[4]<='9'&&
        (racer!=0||id==playerPodId),"original roster identity rejected");
    for(std::size_t previous=0;previous<racer;++previous)
     require50DU(roster.at("racers").at(previous).at("pod_id")!=id,"duplicate original racer");
   });
   if(racer||playerUsesRosterRig)coldRacerStage50DU(id,"rig construction",[&](){
    if(directOriginalInputs){
     const auto archive=runtime_asset_paths50LB::resolve("work/native/project/disc/PODS")/id/(id+".RES");
     racerRigs[racer]=std::make_unique<native_racer50HW::Rig>(".",archive);
    }else{
     const auto relative=std::filesystem::path(entry.at("path").get<std::string>());
     require50DU(!relative.is_absolute()&&relative.lexically_normal()==relative&&
         relative.string().find("..") == std::string::npos,"original roster path rejected");
     racerRigs[racer]=std::make_unique<native_racer50HW::Rig>(".",rosterRoot/relative);
    }
    require50DU(racerRigs[racer]->podId==id,"original rig and roster identity differ");
   });
   coldRacerStage50DU(id,"collision owner construction",[&](){
    racerCollisions[racer]=std::make_unique<native_racer_collision50HW::Owner>();
   });
   const auto archive=runtime_asset_paths50LB::resolve("work/native/project/disc/PODS")/id/(id+".RES");
   coldRacerStage50DU(id,"resource archive verification",[&](){
    require50DU(pod_materials50ed_fixture::detail::sha256(f::readBytes(archive))==
        entry.at("resource_sha256").get<std::string>(),"original racer resource archive identity differs");
   });
   if(!racer)coldRacerStage50DU(id,"original display name",[&](){
    racerNames[racer]=directOriginalInputs?racerRigs[racer]->displayName:native_racer50HW::loadVerifiedDisplayName(
        runtime_asset_paths50LB::resolve("evidence/originalRoster50HW/original-eight-visuals"),id,
        entry.at("resource_sha256").get<std::string>());
   });
   coldRacerStage50DU(id,"collision resource preparation",[&](){
    std::string error;
    const bool collisionReady=racerCollisions[racer]->prepare(
        {archive.string(),"Pods\\Pod"+id.substr(3)+"\\"},endpointPolicy,error);
    require50DU(collisionReady,error.empty()?"original racer collision load failed":error.c_str());
   });
   mainPairCapacity=std::max(mainPairCapacity,racerCollisions[racer]->main().rayCount);
   secondaryPairCapacity=std::max(secondaryPairCapacity,racerCollisions[racer]->secondary().rayCount);
  }
  if(canonicalMode2){
   Profile::CourseRaceSelection current{};
   Profile::TournamentCareerSnapshot career{};
   rr::game::OriginalUiCourseRememberedProfile50KG course{};
   require50DU(courseProfile->courseRaceSnapshot(current)&&
       courseProfile->selectedTournamentCourseSnapshot(career,course)&&
       courseProfile->stamp().generation==mode2Stamp.generation&&
       courseProfile->stamp().mode34==mode2Stamp.mode34&&
       career.stamp.generation==mode2Stamp.generation&&career.career100C0==playerRacer60&&
       current.mode34==2&&current.course==mode2Selection.course&&current.rider==mode2Selection.rider&&
       current.planet==mode2Selection.planet&&current.subtype==mode2Selection.subtype&&
       current.laps==mode2Selection.laps&&current.entrants==mode2Selection.entrants&&
       current.packed1125C==mode2Selection.packed1125C&&
       course.courseOrder100C4==mode2Course.courseOrder100C4,
       "canonical mode2 profile changed during authenticated roster construction");
  }
 }
}
}

// Same retained Operation/Bank reader phases. This provider already owns the
// canonical roster implementation imports; Application/Profile work stays out
// of these method bodies and returns through declaration-only private calls.
namespace original_results_roster50LA {
ra_cold_aggregate50LA::Prefix Read::completedOwnerPrefixes(
    const std::shared_ptr<ra_cold_aggregate50LA::Owner>& world)noexcept {
    return world->completedPrefixes();
}
bool Read::currentBank(std::size_t& count)const {
    auto& op=*operation_;
        // registered() checks the destruction-bound Bank pin before any Bank
        // dereference. Holding Operation is not itself a Bank lifetime lease.
        if(op.terminal_||op.closed_||!op.aggregate_||!op.resources_||!op.bank_||
           op.aggregate_->terminal_||op.aggregate_->closed_||!op.aggregate_->runtimeQueries_||
           op.aggregate_->prefix_.staticWorldImages||op.aggregate_->prefix_.staticWorldDrawPlan||
           !op.registered()||!applicationCurrent(op.aggregate_,op.application_,op.entities_,op.generation_,op.domain_))return false;
        if(!humanProfileCurrent(op.arena_.humanCount,op.resources_->humanCount_))return false;
        ra_cold_aggregate50LA::Diagnostic diagnostic;
        const auto actual=op.aggregate_->retainedRosterForHandoff(*op.bank_,diagnostic);
        if(actual!=operation_)return false;
        const auto n=op.bank_->count();
        if(!n||n>8u||(humanRacer60_&&n!=8u)||op.resources_->racerCount_!=n||op.nextSlot_!=n||
           launchEntrantsDiffer(n))return false;
        const auto managerIndex=op.bank_->bindRead(op.arena_.ai.managerIndexFE0);
        const auto id=op.bank_->bindRead(op.arena_.ai.racer60);
        const auto role=op.bank_->bindRead(op.arena_.live.state.fields.vehicle58);
        const auto bound=op.bank_->bindRead(op.arena_.live.state.fields.nativeAiOwnersBound50IV);
        const auto ready=op.bank_->bindRead(op.arena_.live.state.phase);
        const auto initialized=op.bank_->bindRead(op.arena_.ai.initialized);
        for(std::size_t slot=0;slot<n;++slot){
            if(!op.bank_->active(slot)||!op.completed_[slot]||op.bank_->completedSteps(slot)!=0u||
               !op.resources_->rigs_[slot]||!op.resources_->collisions_[slot])return false;
            const auto* source=rr::game::original_racer_selection50IJ::findByPodId(
                op.resources_->rigs_[slot]->podId);
            // Revalidate the actual whole field before every lookup/final
            // commit; these comparisons publish or retain no future ID list.
            if(!source||op.bank_->read(slot,ready)!=rr::game::VehicleFramePhase49W::Ready||
               (humanRacer60_&&(*humanRacer60_)[slot]!=source->racer60))return false;
            if(humanRacer60_&&slot==0u){
                // The genuine human lane has no initialized AI identity.
                // Never read its AI racer60/managerIndex as a player substitute.
                if(op.bank_->read(slot,role)!=0u||op.bank_->read(slot,bound)||
                   op.bank_->read(slot,initialized))return false;
            }else if(op.bank_->read(slot,managerIndex)!=slot||
               op.bank_->read(slot,id)!=source->racer60||op.bank_->read(slot,role)!=1u||
               !op.bank_->read(slot,bound)||!op.bank_->read(slot,initialized))return false;
        }
        if(finalHumanProfileChanged())return false;
        count=n;return true;
}

bool Read::racer60At1AF330(std::int32_t index,std::uint32_t& actual60)const noexcept {
    actual60=0;
    try {
        std::size_t count=0;
        // The actual getter rejects negative and index>=manager30. Count and
        // racer60 are read at the reached occurrence, not copied for later use.
        if(!current(count)||index<0||static_cast<std::size_t>(index)>=count)return false;
        auto& op=*operation_;const auto slot=static_cast<std::size_t>(index);
        if(humanRacer60_){
            const auto value=(*humanRacer60_)[slot];
            std::size_t after=0;
            if(!current(after)||after!=count||(*humanRacer60_)[slot]!=value)return false;
            actual60=value;return true;
        }
        auto& bank=*op.bank_;
        const auto role=bank.bindRead(op.arena_.live.state.fields.vehicle58);
        const auto bound=bank.bindRead(op.arena_.live.state.fields.nativeAiOwnersBound50IV);
        const auto ready=bank.bindRead(op.arena_.live.state.phase);
        const auto initialized=bank.bindRead(op.arena_.ai.initialized);
        const auto managerIndex=bank.bindRead(op.arena_.ai.managerIndexFE0);
        const auto id=bank.bindRead(op.arena_.ai.racer60);
        const auto* source=rr::game::original_racer_selection50IJ::findByPodId(
            op.resources_->rigs_[slot]->podId);
        if(!source||bank.read(slot,role)!=1u||!bank.read(slot,bound)||
           bank.read(slot,ready)!=rr::game::VehicleFramePhase49W::Ready||
           !bank.read(slot,initialized)||bank.read(slot,managerIndex)!=slot)return false;
        const auto value=bank.read(slot,id);
        if(value!=source->racer60)return false;
        std::size_t after=0;
        if(!current(after)||after!=count)return false;
        actual60=value;return true;
    }catch(...){return false;}
}
} // namespace original_results_roster50LA
