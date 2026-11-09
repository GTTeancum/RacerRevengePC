#include "original_results_roster_read50la.h"
#include "original_ui_main_entry50kg.h"
#include "original_race_constructor50la.h"
#include "ra_cold_aggregate_prefix50la.h"
#include <utility>

namespace original_results_roster50LA {
ra_cold_aggregate50LA::Prefix Read::completedWorldPrefixes(
    const rr::game::OriginalRaceConstructor50LA& race)noexcept {
    return completedOwnerPrefixes(race.world74_);
}
Read::Read(const original_main_entry50KG::Application& application,
    const rr::game::OriginalRaceConstructor50LA& race,
    std::shared_ptr<ra_cold_roster50LA::Operation> operation)noexcept
    :Read(application,race,std::move(operation),nullptr){}
Read::Read(const original_main_entry50KG::Application& application,
    const rr::game::OriginalRaceConstructor50LA& race,
    std::shared_ptr<ra_cold_roster50LA::Operation> operation,
    const std::array<std::uint32_t,8>* actualHumanRacer60)noexcept
    :application_(&application),race_(&race),operation_(std::move(operation)),
     humanRacer60_(actualHumanRacer60),humanProfileGeneration_(actualHumanRacer60&&application.riderProfile?
         application.riderProfile->stamp().generation:0){}
Read::~Read()=default;

bool Read::applicationCurrent(
    const std::shared_ptr<ra_cold_aggregate50LA::Owner>& aggregate,
    const original_main_entry50KG::Application* const& operationApplication,
    const std::shared_ptr<rr::game::OriginalApplicationEntities50LA>& entities,
    const std::uint64_t& generation,
    const std::shared_ptr<rr::assets::OriginalStandardMaterialDomain50LA>& domain)const {
    const auto& race=*race_;
    if(application_->sourceRace28COwner_.get()!=race_||
           !race.counted_||race.rolledBack_||!race.loadPrefixReached_||
           !race.resourcesPrepared_||race.raceGroupsAttached_!=6u||
           race.application_!=application_->entitiesOwner_||race.profile_!=application_->riderProfile||
           race.world74_!=aggregate||operationApplication!=application_||
           entities!=race.application_||generation!=race.entity_.sessionGeneration||
           domain!=application_->standardMaterialDomainOwner_||
           race.resourceDomain_!=domain||race.resourceContext_!=application_->materialContextOwner_||
           !race.resourceContext_||!race.resourceContext_->live())return false;
    return true;
}

bool Read::humanProfileCurrent(const std::uint32_t& arenaHumanCount,
    const std::uint32_t& resourceHumanCount)const {
    const auto& race=*race_;
        if(humanRacer60_){
            // Private cold-only human path. The profile stamp is a native
            // same-load mutation fence, not a guest manager/class certificate.
            using Profile=rr::platform::OriginalUiRiderProfileOwner50KG;
            Profile::CourseRaceSelection selected{};
            Profile::TournamentCareerSnapshot career{};
            rr::game::OriginalUiCourseRememberedProfile50KG course{};
            const auto* profile=race.profile_.get();
            const auto* target=original_course_targets50KX::target(race.launchSelection_);
            if(arenaHumanCount!=1u||resourceHumanCount!=1u||!profile||
               !target||target->track!=rr::platform::HostTrack50IM::RA||
               !profile->courseRaceSnapshot(selected)||
               !original_course_targets50KX::same(selected,race.launchSelection_)||
               selected.mode34!=2u||selected.subtype!=1u||selected.entrants!=8u||
               ((selected.packed1125C>>5)&3u)!=1u||((selected.packed1125C>>7)&15u)!=7u||
               selected.rider!=(*humanRacer60_)[0]||
               !profile->selectedTournamentCourseSnapshot(career,course)||
               profile->stamp().generation!=humanProfileGeneration_||profile->stamp().mode34!=2u||
               career.stamp.generation!=humanProfileGeneration_||career.stamp.mode34!=2u||course.mode34!=2u||
               career.career100C0!=selected.rider||
               (career.progress219C&31u)>=course.courseOrder100C4.size()||
               selected.course!=course.courseOrder100C4[career.progress219C&31u])return false;
        }else if(arenaHumanCount!=0u||resourceHumanCount!=0u)return false;
    return true;
}

bool Read::launchEntrantsDiffer(const std::size_t& n)const {
    const auto& race=*race_;
    return race.launchSelection_.entrants!=n;
}

bool Read::finalHumanProfileChanged()const {
    const auto& race=*race_;
    return humanRacer60_&&(race.profile_->stamp().generation!=humanProfileGeneration_||
           race.profile_->stamp().mode34!=2u);
}

bool Read::current(std::size_t& count)const noexcept {
    count=0;
    try {
        if(!application_||!race_)return false;
        if(!operation_){
            // The common race (every course but the RA world join): no RA
            // roster Operation exists. The original loader reads only the
            // race manager's count (manager30) and each racer's 60 through
            // 1AF330; here those are the race's launch entrants and its actual
            // racer60 list. The race must be the application's live race.
            const auto& race=*race_;
            if(!humanRacer60_||application_->sourceRace28COwner_.get()!=race_||!race.counted_||
               race.rolledBack_||race.world74_||!race.anyWorld()||!race.profile_||
               race.application_!=application_->entitiesOwner_||race.profile_!=application_->riderProfile||
               race.profile_->stamp().generation!=humanProfileGeneration_)return false;
            const std::size_t n=race.launchSelection_.entrants;
            if(!n||n>8u)return false;
            count=n;return true;
        }
        return currentBank(count);
    }catch(...){return false;}
}

}
