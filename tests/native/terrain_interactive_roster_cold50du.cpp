// Authentic original allocation and roster operations; no owner relocation.
#include "native_last_failure50pd.h"
#include "native_wreck_debris50pd.h"
#include "terrain_interactive_roster_cold50du.h"
#include "original_race_constructor50la.h"
#include "original_results_roster_read50la.h"
#include "original_results_live_owner_read50la.h"
#include "../../src/game/original_results104_50la.h"
#include "original_position_stats_adapter50la.h"
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
    if(!ok){native_last_failure50PD::record(message);std::cerr<<"require_failed "<<(message?message:"")<<std::endl;throw std::runtime_error(message);}
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
    // Stock Pod%02d spelling or an admitted add-on row (player only: an
    // add-on pod never joins a stock AI field).
    require50DU(original_player_identity50IK::admittedPodId(id)&&
        (rr::game::original_racer_selection50IJ::stockPodId(id)||racer==0)&&
        (racer!=0||id==playerPodId),"original roster identity rejected");
    for(std::size_t previous=0;previous<racer;++previous)
     require50DU(roster.at("racers").at(previous).at("pod_id")!=id,"duplicate original racer");
   });
   if(racer||playerUsesRosterRig)coldRacerStage50DU(id,"rig construction",[&](){
    if(directOriginalInputs){
     const auto archive=original_player_identity50IK::podArchive(
         runtime_asset_paths50LB::resolve("work/native/project/disc/PODS"),id);
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
   const auto archive=original_player_identity50IK::podArchive(
       runtime_asset_paths50LB::resolve("work/native/project/disc/PODS"),id);
   std::vector<std::uint8_t> retainedRes;
   coldRacerStage50DU(id,"resource archive verification",[&](){
    retainedRes=f::readBytes(archive);
    require50DU(pod_materials50ed_fixture::detail::sha256(retainedRes)==
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
        {archive.string(),original_player_identity50IK::podFolderMember(id)},endpointPolicy,error);
    require50DU(collisionReady,error.empty()?"original racer collision load failed":error.c_str());
   });
   // Wreckage pieces for the destroy handlers (2261B0/226750). Debris is
   // optional presentation: a rejected piece is logged, never a load failure.
   if(racerRigs[racer]){
    std::string log;bool loaded=false;
    try{loaded=native_wreck_debris50PD::loadRacer(racer,*racerRigs[racer],retainedRes,log);}
    catch(const std::exception& e){log=e.what();}catch(...){log="unknown failure";}
    std::cerr<<"wreck_debris_cold slot="<<racer<<" pod="<<id<<" inventoried="<<loaded<<' '<<log<<'\n';
   }
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
            const auto* source=rr::game::original_racer_selection50IJ::findAnyByPodId(
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
        const auto slot=static_cast<std::size_t>(index);
        if(humanRacer60_){
            const auto value=(*humanRacer60_)[slot];
            std::size_t after=0;
            if(!current(after)||after!=count||(*humanRacer60_)[slot]!=value)return false;
            actual60=value;return true;
        }
        if(!operation_)return false;
        auto& op=*operation_;auto& bank=*op.bank_;
        const auto role=bank.bindRead(op.arena_.live.state.fields.vehicle58);
        const auto bound=bank.bindRead(op.arena_.live.state.fields.nativeAiOwnersBound50IV);
        const auto ready=bank.bindRead(op.arena_.live.state.phase);
        const auto initialized=bank.bindRead(op.arena_.ai.initialized);
        const auto managerIndex=bank.bindRead(op.arena_.ai.managerIndexFE0);
        const auto id=bank.bindRead(op.arena_.ai.racer60);
        const auto* source=rr::game::original_racer_selection50IJ::findAnyByPodId(
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

//Opaque Results ownership reads live beside the existing Read backend and
//already-complete Operation. No new pin, prepared state or cached count.
namespace original_results_roster50LA {
bool Read::loadedOperationOwnerFieldsCurrent(const std::shared_ptr<ra_cold_roster50LA::Operation>& operation,
    const std::shared_ptr<ra_cold_aggregate50LA::Owner>& aggregate,
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& application,
    const std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA>& domain,
    const std::uint64_t& generation)noexcept {
    // The common race has neither an RA roster Operation nor the RA world;
    // its loaded rows come from the live race list (Read::current).
    if(!operation)return !aggregate&&application&&domain&&generation;
    return operation->aggregate_==aggregate&&operation->entities_==application&&
        operation->domain_==domain&&operation->generation_==generation;
}
bool Read::loadedOperationBankCurrent(const std::shared_ptr<ra_cold_roster50LA::Operation>& operation,
    const native_racer50HN::Bank* bank)noexcept {
    //Original caller's left-hand owner match proves operation nonNULL, or the
    //common race (no Operation), whose rows are read from this live Bank.
    if(!operation)return bank!=nullptr;
    return operation->resultsRuntimeBankCurrent(bank);
}
bool Read::loadedOperationRacerCurrent(const std::shared_ptr<ra_cold_roster50LA::Operation>& operation,
    std::size_t index,std::uint32_t racer60)noexcept {
    //Same already-pinned/current row-loop call site, no extra admission/reordering.
    //The common race's row identity was read from its live list this frame.
    if(!operation)return index<8u&&racer60!=0xffffffffu;
    return operation->resultsRacerIdentityCurrent(index,racer60);
}
}

namespace original_position_stats_adapter50LA {
bool Owner::resultsRowsCurrent(const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,
    std::uint64_t publication)const noexcept {
    if(!camera_||!manager_||!application||!materials||!generation||!publication)return false;
    try {
        const auto& c=*camera_;
        return identityCurrent()&&c.actual_.applicationEntities.get()==application&&
            c.actual_.applicationMaterials.get()==materials&&c.generation_==generation&&
            c.frameStage_==Camera::FrameStage::published&&c.completedFrame_&&
            c.publicationCount_==publication&&c.viewport_.prepared&&c.viewport_.matricesReady&&
            c.viewport_.sessionGeneration==generation;
    }catch(...){return false;}
}

bool Owner::captureResultsRows(const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,
    ResultsRows& output,Diagnostic& d)const noexcept {
    d={};if(!camera_||!manager_)return false;
    try {
        const auto& c=*camera_;const auto& m=*manager_;const auto& bank=c.actual_.bank;
        const auto publication=c.publicationCount_;
        if(!resultsRowsCurrent(application,materials,generation,publication)){
            d.status=Status::identity;return false;
        }
        // bindRead rejects active/blocked Bank leases. These exact values are
        // native source-lane owners, NOT HOST standings/elapsed race seconds.
        const auto time=bank.bindRead(c.actual_.racerTiming.timeCB8);
        const auto gap=bank.bindRead(c.actual_.racerTiming.valueCBC);
        const auto flags=bank.bindRead(c.actual_.arena.fields.impactFlagsCD8);
        ResultsRows next;next.count=bank.count();next.trackLength38=m.trackLength38;
        next.generation=generation;next.publication=publication;next.bank=&bank;
        std::array<bool,8> places{};
        for(std::size_t i=0;i<next.count;++i){
            const auto& observed=m.observed[i];const auto& fields=m.fields[i];
            const auto& rank=m.rank[i];const auto& input=m.rankInput[i];
            // The existing manager publishes these actual rank/observation
            // lanes together. Publication here is a native scheduling guard,
            // never proof of original Application phase5 or managerB0 load.
            if(!bank.active(i)||bank.read(i,c.reads_.phase)!=rr::game::VehicleFramePhase49W::Ready||
               !observed.bound||observed.managerIndex!=i||!fields.bound||
               fields.placeCCC!=rank.placeCCC||observed.placeCCC!=rank.placeCCC||
               fields.remainingCC4!=rank.remainingCC4||observed.progressCC4!=rank.remainingCC4||
               observed.lapCC8!=input.lapCC8||rank.placeCCC<1||
               static_cast<std::size_t>(rank.placeCCC)>next.count||places[std::size_t(rank.placeCCC-1)]){
                d.status=Status::identity;return false;
            }
            places[std::size_t(rank.placeCCC-1)]=true;
            auto& row=next.rows[i];
            row.vehicle58=bank.read(i,c.reads_.class58);
            // Actual identity array is checked by identitiesCurrent; never
            // read uninitialized AI racer60 for the genuine human prefix.
            row.racer60=c.actual_.racer60[i];row.retirementCD4=bank.read(i,c.reads_.retirementCD4);
            row.flagsCD8=bank.read(i,flags);row.timeCB8=bank.read(i,time);row.valueCBC=bank.read(i,gap);
            row.placeCCC=rank.placeCCC;row.remainingCC4=rank.remainingCC4;row.lapCC8=observed.lapCC8;
            if(row.vehicle58!=observed.vehicle58||row.retirementCD4!=rank.retirementCD4){
                d.status=Status::identity;return false;
            }
        }
        if(!resultsRowsCurrent(application,materials,generation,publication)){
            d.status=Status::phase;return false;
        }
        // Every field of every active row was read above. Default storage
        // outside count is never a known original row and is never consumed.
        output=next;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
}

namespace original_results_roster50LA {
bool Read::captureRows(const original_position_stats_adapter50LA::Owner& observer,
    const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,Rows& output)noexcept {
    original_position_stats_adapter50LA::Owner::ResultsRows actual;
    original_position_stats_adapter50LA::Owner::Diagnostic diagnostic;
    if(!observer.captureResultsRows(application,materials,generation,actual,diagnostic))return false;
    Rows next;next.count=actual.count;next.bank=actual.bank;next.trackLength38=actual.trackLength38;
    next.generation=actual.generation;next.publication=actual.publication;
    for(std::size_t i=0;i<actual.count;++i){const auto& a=actual.rows[i];auto& r=next.rows[i];
        r.vehicle58=a.vehicle58;r.racer60=a.racer60;r.retirementCD4=a.retirementCD4;
        r.placeCCC=a.placeCCC;r.lapCC8=a.lapCC8;r.flagsCD8=a.flagsCD8;
        r.timeCB8=a.timeCB8;r.valueCBC=a.valueCBC;r.remainingCC4=a.remainingCC4;
    }
    output=next;return true;
}
bool Read::rowsCurrent(const original_position_stats_adapter50LA::Owner& observer,
    const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,
    std::uint64_t publication)noexcept {return observer.resultsRowsCurrent(application,materials,generation,publication);}
bool Read::observerCameraMatches(const original_position_stats_adapter50LA::Owner& observer,
    const original_camera_session50KX::Session& camera)noexcept {return observer.camera_==&camera;}
const native_racer50HN::Bank* Read::observerBankIdentity(const original_position_stats_adapter50LA::Owner& observer)noexcept {
    return observer.resultsBankIdentity();
}
bool Read::observerCurrent(const original_position_stats_adapter50LA::Owner& observer){return observer.identityCurrent();}
std::size_t Read::bankCount(const native_racer50HN::Bank& bank)noexcept {return bank.count();}
}

namespace original_results_roster50LA {
const std::shared_ptr<ra_cold_aggregate50LA::Owner>& Read::raceWorldIdentity(
    const rr::game::OriginalRaceConstructor50LA& race)noexcept {return race.world74_;}
Read::ProfileStamp Read::profileStamp(const rr::game::OriginalRaceConstructor50LA& race)noexcept {
    const auto actual=race.profile_->stamp();return {actual.generation,actual.mode34};
}
const rr::platform::OriginalUiMainProfile50KG* Read::commonProfile(
    const rr::game::OriginalRaceConstructor50LA& race)noexcept {return race.profile_->commonProfile();}
bool Read::profileKnown(const rr::platform::OriginalUiMainProfile50KG& profile)noexcept {return profile.known;}
std::uint8_t Read::profilePlayerBits(const rr::platform::OriginalUiMainProfile50KG& profile)noexcept {return profile.playerBits1125C;}
bool Read::profileRacerName(const rr::game::OriginalRaceConstructor50LA& race,
    std::uint32_t racer,std::int32_t& name)noexcept {return race.profile_->resultsRacerName3C(racer,name);}
}

namespace original_position_stats_adapter50LA {
bool Owner::resultsRecordIdentityCurrent(const ResultsRecordIdentity& row)const noexcept {
    try {
        if(!resultsRowsCurrent(row.application,row.materials,row.generation,row.publication))return false;
        const auto& c=*camera_;const auto& bank=c.actual_.bank;
        return row.bank==&bank&&row.managerIndex<bank.count()&&bank.active(row.managerIndex)&&
            bank.read(row.managerIndex,c.reads_.phase)==rr::game::VehicleFramePhase49W::Ready&&
            bank.read(row.managerIndex,c.reads_.class58)==row.vehicle58&&
            c.actual_.racer60[row.managerIndex]==row.racer60;
    }catch(...){return false;}
}
bool Owner::captureResultsRecordIdentity(const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,
    std::size_t managerIndex,ResultsRecordIdentity& output,Diagnostic& d)const noexcept {
    d={};if(!camera_)return false;
    try {
        const auto& c=*camera_;const auto publication=c.publicationCount_;
        if(!resultsRowsCurrent(application,materials,generation,publication)){
            d.status=Status::identity;return false;
        }
        const auto& bank=c.actual_.bank;
        if(managerIndex>=bank.count()||!bank.active(managerIndex)){
            d.status=Status::phase;return false;
        }
        ResultsRecordIdentity next;
        next.bank=&bank;next.application=application;next.materials=materials;
        next.generation=generation;next.publication=publication;next.managerIndex=managerIndex;
        next.vehicle58=bank.read(managerIndex,c.reads_.class58);next.racer60=c.actual_.racer60[managerIndex];
        if(!resultsRecordIdentityCurrent(next)){d.status=Status::phase;return false;}
        output=next;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
bool Owner::readResultsBestLapCB4(const ResultsRecordIdentity& row,float& output,Diagnostic& d)const noexcept {
    d={};
    try {
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::identity;return false;}
        const auto& c=*camera_;const auto& bank=c.actual_.bank;
        const auto lane=bank.bindRead(c.actual_.racerTiming.bestLapCB4);
        const auto value=bank.read(row.managerIndex,lane);
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::phase;return false;}
        output=value;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
bool Owner::readResultsRaceTimeCB8(const ResultsRecordIdentity& row,float& output,Diagnostic& d)const noexcept {
    d={};
    try {
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::identity;return false;}
        const auto& c=*camera_;const auto& bank=c.actual_.bank;
        const auto lane=bank.bindRead(c.actual_.racerTiming.timeCB8);
        const auto value=bank.read(row.managerIndex,lane);
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::phase;return false;}
        output=value;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
bool Owner::readResultsObservedTimingCD0(const ResultsRecordIdentity& row,std::uint32_t& output,Diagnostic& d)const noexcept {
    d={};
    try {
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::identity;return false;}
        const auto& c=*camera_;const auto& bank=c.actual_.bank;
        const auto lane=bank.bindRead(c.actual_.racerTiming.countCD0);
        const auto value=bank.read(row.managerIndex,lane);
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::phase;return false;}
        output=value;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
}

namespace rr::game {
bool OriginalResults10450LA::nameEntryRaceCurrent1C9B10(const OriginalRaceConstructor50LA& race,
    const OriginalResults10450LA* expected,const std::shared_ptr<OriginalApplicationEntities50LA>& application,
    const std::uint64_t& generation)noexcept {
    return !(race.results104_.get()!=expected||!race.counted_||race.rolledBack_||
       race.application_!=application||race.entity_.sessionGeneration!=generation||
       !application->entityAuthority_.canRetire(race.entity_)||!race.profile_||!race.profile_->initialized());
}
}
namespace original_results_roster50LA {
bool Read::captureRecord(const original_position_stats_adapter50LA::Owner& observer,
    const rr::game::OriginalApplicationEntities50LA* application,
    const rr::assets::OriginalStandardMaterialTable50LA* materials,std::uint64_t generation,
    std::size_t index,Record& output)noexcept {
    original_position_stats_adapter50LA::Owner::ResultsRecordIdentity actual;
    original_position_stats_adapter50LA::Owner::Diagnostic diagnostic;
    if(!observer.captureResultsRecordIdentity(application,materials,generation,index,actual,diagnostic))return false;
    output={actual.bank,actual.application,actual.materials,actual.generation,actual.publication,
        actual.managerIndex,actual.vehicle58,actual.racer60};return true;
}
bool Read::recordCurrent(const original_position_stats_adapter50LA::Owner& observer,const Record& row)noexcept {
    const original_position_stats_adapter50LA::Owner::ResultsRecordIdentity actual{
        row.bank,row.application,row.materials,row.generation,row.publication,row.managerIndex,row.vehicle58,row.racer60};
    return observer.resultsRecordIdentityCurrent(actual);
}
bool Read::readBestLap(const original_position_stats_adapter50LA::Owner& observer,const Record& row,float& output)noexcept {
    const original_position_stats_adapter50LA::Owner::ResultsRecordIdentity actual{
        row.bank,row.application,row.materials,row.generation,row.publication,row.managerIndex,row.vehicle58,row.racer60};
    original_position_stats_adapter50LA::Owner::Diagnostic diagnostic;
    return observer.readResultsBestLapCB4(actual,output,diagnostic);
}
bool Read::readRaceTime(const original_position_stats_adapter50LA::Owner& observer,const Record& row,float& output)noexcept {
    const original_position_stats_adapter50LA::Owner::ResultsRecordIdentity actual{
        row.bank,row.application,row.materials,row.generation,row.publication,row.managerIndex,row.vehicle58,row.racer60};
    original_position_stats_adapter50LA::Owner::Diagnostic diagnostic;
    return observer.readResultsRaceTimeCB8(actual,output,diagnostic);
}
bool Read::readObservedTiming(const original_position_stats_adapter50LA::Owner& observer,const Record& row,std::uint32_t& output)noexcept {
    const original_position_stats_adapter50LA::Owner::ResultsRecordIdentity actual{
        row.bank,row.application,row.materials,row.generation,row.publication,row.managerIndex,row.vehicle58,row.racer60};
    original_position_stats_adapter50LA::Owner::Diagnostic diagnostic;
    return observer.readResultsObservedTimingCD0(actual,output,diagnostic);
}
bool Read::profileNameCourse(const rr::game::OriginalRaceConstructor50LA& race,std::uint32_t& output)noexcept {
    return race.profile_->resultsNameCourse1C9C80(output);
}
bool Read::profileFirstName(const rr::game::OriginalRaceConstructor50LA& race,rr::platform::OriginalPersistedProfileName50KG& output)noexcept {
    return race.profile_->resultsFirstName1C9CA4(output);
}
rr::game::LapRecordQueryResult50ID Read::profileLapQuery(const rr::game::OriginalRaceConstructor50LA& race,std::uint32_t course,float value)noexcept {
    return race.profile_->resultsLapQuery1861A0(course,value);
}
rr::game::LapRecordQueryResult50ID Read::profileRaceQuery(const rr::game::OriginalRaceConstructor50LA& race,std::uint32_t course,float value)noexcept {
    return race.profile_->resultsRaceQuery186280(course,value);
}
rr::game::LapRecordQueryResult50ID Read::profileKnockoutQuery(const rr::game::OriginalRaceConstructor50LA& race,std::uint32_t course,std::uint32_t value)noexcept {
    return race.profile_->resultsKnockoutQuery1863B0(course,value);
}
bool Read::hudRacerName3C(const rr::platform::OriginalUiRiderProfileOwner50KG& profile,
    std::uint32_t vehicle60,std::int32_t& id)noexcept {
    return profile.resultsRacerName3C(vehicle60,id);
}
rr::game::LapRecordQueryResult50ID Read::hudKnockoutRecord1863B0(
    const rr::platform::OriginalUiRiderProfileOwner50KG& profile,std::uint32_t countCD0)noexcept {
    //225FDC..225FF0: a1 = profile 125F low nibble, the course 1C9C80 also reads.
    std::uint32_t course=0;
    if(!profile.resultsNameCourse1C9C80(course))
        return {rr::game::LapRecordQueryStatus50ID::Storage,0,0x225fe8u};
    return profile.resultsKnockoutQuery1863B0(course,countCD0);
}
}

namespace original_results_roster50LA {
Read::RaceAccess Read::raceAccess(const rr::game::OriginalRaceConstructor50LA& race)noexcept {
    return {race,race.resultsFeedbackThread_,race.results104_,race.application_,race.profile_,race.entity_,
        race.counted_,race.rolledBack_,race.flags1B0KnownMask_,race.flags1B0KnownValue_,race.resultsFeedback_};
}
Read::RootAccess Read::rootAccess(const original_main_entry50KG::Application& root)noexcept {
    return {root,root.garageAudioOwnerThread_,root.entitiesOwner_,root.standardMaterialDomainOwner_,root.fonts328Owner_,
        root.nativeTaRootTiming_,root.nativeTaRootPublication_,root.sourceRace28COwner_,root.riderProfile,
        root.nativeTaRootRace_,root.nativeTaRootGeneration_,root.nativeTaRootEntered_,root.nativeTaRootAborted_,
        root.sourceApplicationMode22C_,root.sourceUpdateRenderBypass228Bit3_,root.videoOwner_,
        root.sourceApplicationElapsed23C_,root.sourceLocalization32COwner_};
}
bool Read::RaceAccess::managerFlags(const original_camera_session50KX::Session& camera,std::uint8_t& flags,
    rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {return actual.readResultsManagerFlags58(camera,flags,d);}
bool Read::RootAccess::finalCameraCurrent(const rr::game::OriginalRaceConstructor50LA& race,
    const original_camera_session50KX::Session& camera)const noexcept {return actual.nativeResultsFinalCameraCurrent50LA(race,camera);}
bool Read::RootAccess::publishedRoot(const rr::game::OriginalRaceConstructor50LA& race,
    rr::game::OriginalResults104Diagnostic50LA& d)const noexcept {return actual.resultsPublishedRoot50LA(race,d);}
bool Read::RootAccess::borrowLocalizationText(rr::assets::OriginalApplicationLocalization50KG& dictionary,std::int32_t ordinal,
    std::shared_ptr<const rr::assets::OriginalApplicationLocalization50KG::SelectedText>& output,
    rr::assets::OriginalApplicationLocalizationDiagnostic50KG& d)const noexcept {
    return actual.borrowLocalizationText18AC60(dictionary,ordinal,output,d);
}
bool Read::profileInitialized(const rr::game::OriginalRaceConstructor50LA& race)noexcept {return race.profile_->initialized();}
std::uint32_t Read::profileMode(const rr::platform::OriginalUiMainProfile50KG& profile)noexcept {return profile.mode34;}
// The common human race has its one human (the only route that loads it).
std::uint32_t Read::operationHumanCount(const std::shared_ptr<ra_cold_roster50LA::Operation>& operation)noexcept {return operation?operation->arena_.humanCount:1u;}
}
namespace rr::game {
bool OriginalResults10450LA::liveResultsRaceAuthorityCurrent(const OriginalRaceConstructor50LA& race,
    const std::shared_ptr<OriginalApplicationEntities50LA>& application)noexcept {
    return application->entityAuthority_.canRetire(race.entity_);
}
}

namespace rr::game {
bool OriginalResults10450LA::careerRosterCurrentNative(
    const std::shared_ptr<::ra_cold_roster50LA::Operation>& operation,
    const OriginalRaceConstructor50LA& race,const std::shared_ptr<OriginalApplicationEntities50LA>& application,
    const std::shared_ptr<assets::OriginalStandardMaterialDomain50LA>& domain,std::uint64_t generation,
    const ::original_position_stats_adapter50LA::Owner& observer)noexcept {
    // The common race (no RA roster Operation): its own counted world and
    // the live observed Bank are the load-time owners.
    if(!operation)return !race.world74_&&race.anyWorld()&&race.application_==application&&
        application&&domain&&generation&&observer.resultsBankIdentity()!=nullptr;
    return operation->aggregate_==race.world74_&&operation->entities_==application&&
        operation->domain_==domain&&operation->generation_==generation&&
        operation->resultsRuntimeBankCurrent(observer.resultsBankIdentity());
}
bool OriginalResults10450LA::careerRacerCurrentNative(
    const std::shared_ptr<::ra_cold_roster50LA::Operation>& operation,std::uint32_t racer60)noexcept {
    if(!operation)return racer60!=0xffffffffu; // common race: row read from its live list
    return operation->resultsRacerIdentityCurrent(0,racer60);
}
}

// Out-of-line bridge methods used by the retained model translation unit.
// Keep the fresh-arena definition closure in this existing roster owner TU.
namespace ra_source_camera50LA {
bool OwnershipBridge::adoptSourceAuxiliaryQueries2123C8(std::size_t slot,
    const g::OriginalRacerMainBodyOwner50LA& body)noexcept {
    const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
    if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
       bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
       bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
       terminal_||closed_||!actual_||&actual_->frame!=&fresh->live_.state||
       slot>=sourceAuxiliaryAdopted_.size()||sourceAuxiliaryAdopted_[slot]||
       freshRecovery_||queriesConstructed_||follow_||!actual_->movement||!actual_->recovery||
       actual_->recovery->sourcePrepared||actual_->recovery->selectedSource||
       stagedPods694_[slot].bodyA0!=&body||body.canonicalNumericAddress()!=&actual_->frame.bodies.main||
       body.entity().reference00!=1u||body.entity().sessionGeneration!=stagedPods694_[slot].generation||
       fresh->movementSource_.endpoints||fresh->movementSource_.sceneMetadata)return false;
    sourceAuxiliaryAdopted_[slot]=true; //Before source allocation; never erased by failure or retirement.
    return true;
}
bool OwnershipBridge::sourceContact354Lanes212278(std::size_t slot,
    const g::OriginalRacerMainBodyOwner50LA& body,g::SpringGeneratedParameters48K*& spring,
    g::SpringResponseParameters48L*& response)noexcept {
    spring=nullptr;response=nullptr;
    const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
    if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
       bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
       bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
       terminal_||closed_||!actual_||&actual_->frame!=&fresh->live_.state||
       slot>=stagedPods694_.size()||stagedPods694_[slot].bodyA0!=&body||
       body.canonicalNumericAddress()!=&actual_->frame.bodies.main||!body.entity().reference00||
       !registeredRange(&fresh->mainSpring_,sizeof(fresh->mainSpring_))||
       !registeredRange(&fresh->mainResponse_,sizeof(fresh->mainResponse_)))return false;
    spring=&fresh->mainSpring_;response=&fresh->mainResponse_;return true;
}
bool OwnershipBridge::sourceContact368Lanes2127C8(std::size_t slot,
    const g::OriginalRacerSecondaryBodyOwner50LA& body,const g::OriginalRacerEntityCold50LA& racer,
    g::SpringGeneratedParameters48K*& spring,g::SpringResponseParameters48L*& response)noexcept {
    spring=nullptr;response=nullptr;
    const auto fresh=constructorArena_.lock();const auto pin=bankLifetime_.lock();
    if(!fresh||!fresh->currentNative()||!pin||pin->identity!=bankIdentity_||bankIdentity_!=&fresh->bank_||
       bankIdentity_->raWorldLifetime_.get()!=pin.get()||bankIdentity_->retired_||
       bankIdentity_->visitDepth_!=1u||bankIdentity_->currentSlot_!=slot||
       terminal_||closed_||!actual_||&actual_->frame!=&fresh->live_.state||
       slot>=stagedPods694_.size()||stagedPods694_[slot].generation!=racer.racer.sessionGeneration||
       !racer.sourceBody35C_||*racer.sourceBody35C_!=&body||!racer.sourceScene19C_||!*racer.sourceScene19C_||
       !body.sceneBindingCurrent(**racer.sourceScene19C_)||body.entity().parent1C!=&racer.racer||
       body.entity().reference00!=1u||body.entity().sessionGeneration!=racer.racer.sessionGeneration||
       body.canonicalNumericAddress()!=&actual_->frame.bodies.secondary||
       !registeredRange(&fresh->secondarySpring_,sizeof(fresh->secondarySpring_))||
       !registeredRange(&fresh->secondaryResponse_,sizeof(fresh->secondaryResponse_)))return false;
    spring=&fresh->secondarySpring_;response=&fresh->secondaryResponse_;return true;
}
} // namespace ra_source_camera50LA

#include "ra_native_handling_tail_owners50la.inl"
