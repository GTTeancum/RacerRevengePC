// Common rewrite (R1): the Results loader's roster borrow for every race. The
// RA roster Operation / aggregate-world pin retires; the race's one roster
// (common_roster50CR::Owner, R10) is the pin on every course, and the race's
// one World replaces the RA aggregate identity.
#include "original_results_roster_read50la.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "original_race_world_join50la.h"
#include "common_roster50cr.h"
#include <utility>

namespace original_results_roster50LA {
Read::Read(const original_main_entry50KG::Application& application,
    const rr::game::OriginalRaceConstructor50LA& race,
    const common_roster50CR::Owner& roster)noexcept
    :Read(application,race,roster,nullptr){}
Read::Read(const original_main_entry50KG::Application& application,
    const rr::game::OriginalRaceConstructor50LA& race,
    const common_roster50CR::Owner& roster,
    const std::array<std::uint32_t,8>* actualHumanRacer60)noexcept
    :application_(&application),race_(&race),roster_(&roster),
     humanRacer60_(actualHumanRacer60),humanProfileGeneration_(actualHumanRacer60&&application.riderProfile?
         application.riderProfile->stamp().generation:0){}
Read::~Read()=default;

bool Read::applicationCurrent()const {
    const auto& race=*race_;
    const auto domain=application_->standardMaterialDomainOwner_;
    if(application_->sourceRace28COwner_.get()!=race_||
           !race.counted_||race.rolledBack_||!race.loadPrefixReached_||
           !race.resourcesPrepared_||race.raceGroupsAttached_!=6u||
           race.application_!=application_->entitiesOwner_||race.profile_!=application_->riderProfile||
           !race.nativeIntroWorldEntity()||!domain||
           race.resourceDomain_!=domain||race.resourceContext_!=application_->materialContextOwner_||
           !race.resourceContext_||!race.resourceContext_->live())return false;
    return true;
}

bool Read::launchEntrantsDiffer(const std::size_t& n)const {
    const auto& race=*race_;
    return race.launchSelection_.entrants!=n;
}

std::uint32_t Read::rosterHumanCount(const common_roster50CR::Owner* roster)noexcept {
    return roster&&roster->built()?roster->humans34():0u;
}

bool Read::current(std::size_t& count)const noexcept {
    count=0;
    try {
        if(!application_||!race_||!roster_)return false;
        // One rule for every race: the original loader reads the race
        // manager's count (manager30) and each racer's 60 through 1AF330;
        // here those are the race's one roster (built from the launch's field)
        // and its rows. The race must be the application's live race.
        const auto& race=*race_;
        if(!applicationCurrent()||!race.profile_||
           !roster_->built())return false;
        const std::uint32_t humans=rosterHumanCount(roster_);
        if(humanRacer60_){
            // 1AE280 loads the selected human racers into the first manager slots.
            if(humans==0u||humans>2u||humans>roster_->count()||
               race.profile_->stamp().generation!=humanProfileGeneration_)return false;
            for(std::size_t slot=0;slot<humans;++slot){
                const auto* row=roster_->row(slot);
                if(!row||!row->human||row->racer60!=(*humanRacer60_)[slot])return false;
            }
        }else if(humans!=0u)return false; //ALL8 spectator field (humans34 == 0).
        const std::size_t n=roster_->count();
        if(!n||n>8u||launchEntrantsDiffer(n))return false;
        count=n;return true;
    }catch(...){return false;}
}


// Moved here from terrain_interactive_roster_cold50du.cpp (R10's TU; the
// builder deletes those copies): the reads now go through the race's one
// roster instead of the RA roster Operation / Bank arena.
bool Read::racer60At1AF330(std::int32_t index,std::uint32_t& actual60)const noexcept {
    actual60=0;
    try {
        std::size_t count=0;
        // The actual getter rejects negative and index>=manager30. Count and
        // racer60 are read at the reached occurrence, not copied for later use.
        if(!current(count)||index<0||static_cast<std::size_t>(index)>=count)return false;
        const auto slot=static_cast<std::size_t>(index);
        const auto* row=roster_->row(slot);
        if(!row)return false;
        const auto value=humanRacer60_&&slot<humanRacer60_->size()?(*humanRacer60_)[slot]:row->racer60;
        if(value!=row->racer60)return false;
        std::size_t after=0;
        if(!current(after)||after!=count||roster_->row(slot)!=row||row->racer60!=value)return false;
        actual60=value;return true;
    }catch(...){return false;}
}
bool Read::loadedRosterCurrent(const common_roster50CR::Owner* roster,
    const rr::game::OriginalRaceConstructor50LA& race,const native_racer50HN::Bank* bank)noexcept {
    return roster&&roster->built()&&roster->count()&&race.nativeIntroWorldEntity()&&bank;
}
bool Read::loadedRosterRacerCurrent(const common_roster50CR::Owner* roster,
    std::size_t index,std::uint32_t racer60)noexcept {
    const auto* row=roster?roster->row(index):nullptr;
    return row&&row->racer60==racer60;
}
}

namespace rr::game {
// Results104 career pages (W0 round 2, area A): the load-time roster pin over the
// race's one common roster, the same rule on every course. The race's counted
// World (nativeIntroWorldEntity), its Application and the live observed Bank
// are the load-time owners.
bool OriginalResults10450LA::careerRosterCurrentNative(const ::common_roster50CR::Owner* roster,
    const OriginalRaceConstructor50LA& race,const std::shared_ptr<OriginalApplicationEntities50LA>& application,
    const std::shared_ptr<assets::OriginalStandardMaterialDomain50LA>& domain,std::uint64_t generation,
    const ::original_position_stats_adapter50LA::Owner& observer)noexcept {
    using Read=::original_results_roster50LA::Read;
    return application&&domain&&generation&&race.application_==application&&
        Read::loadedRosterCurrent(roster,race,Read::observerBankIdentity(observer));
}
bool OriginalResults10450LA::careerRacerCurrentNative(const ::common_roster50CR::Owner* roster,
    std::uint32_t racer60)noexcept {
    return ::original_results_roster50LA::Read::loadedRosterRacerCurrent(roster,0,racer60);
}
}
