#include "original_ui_main_entry50kg.h"
#include "../../src/game/original_results104_50la.h"
#include "original_race_constructor50la.h"
#include "original_results_roster_read50la.h"
#include "ra_cold_aggregate_prefix50la.h"
#include <new>
#include <utility>

namespace original_main_entry50KG {
bool Application::loadConsumedPauseResultsBeforeCollector50LA(
    rr::game::OriginalRaceConstructor50LA& race,original_pause_cold50LA::Owner& pause,
    std::shared_ptr<ra_cold_roster50LA::Operation> operation,
    rr::game::OriginalResults104Diagnostic50LA& diagnostic)const noexcept {
    return loadPauseResultsBeforeCollectorImpl50LA(race,pause,std::move(operation),nullptr,diagnostic);
}
bool Application::loadHumanTournamentPauseResultsBeforeCollector50LA(
    rr::game::OriginalRaceConstructor50LA& race,original_pause_cold50LA::Owner& pause,
    std::shared_ptr<ra_cold_roster50LA::Operation> operation,
    const std::array<std::uint32_t,8>& actualRacer60,
    rr::game::OriginalResults104Diagnostic50LA& diagnostic)const noexcept {
    return loadPauseResultsBeforeCollectorImpl50LA(race,pause,std::move(operation),&actualRacer60,diagnostic);
}
bool Application::loadPauseResultsBeforeCollectorImpl50LA(
    rr::game::OriginalRaceConstructor50LA& race,original_pause_cold50LA::Owner& pause,
    std::shared_ptr<ra_cold_roster50LA::Operation> operation,
    const std::array<std::uint32_t,8>* actualHumanRacer60,
    rr::game::OriginalResults104Diagnostic50LA& diagnostic)const noexcept {
    using Results=rr::game::OriginalResults10450LA;
    using Status=rr::game::OriginalResults104Diagnostic50LA::Status;
    diagnostic={};
    const auto fail=[&](Status status,std::uint32_t pc,const char* why)noexcept {
        diagnostic.status=status;diagnostic.sourcePC=pc;diagnostic.operation=why;return false;
    };
    std::uint32_t allocationPC=0x1A8AE8u;
    try {
        if(sourceRace28COwner_.get()!=&race||race.application_!=entitiesOwner_||
           race.profile_!=riderProfile||!race.world74_||race.results104_||!videoOwner_||
           !Video::materialConstructionPinLive(*videoOwner_)||!materialContextOwner_||
           !materialContextOwner_->live()||materialContextOwner_->videoIdentity()!=videoOwner_.get()||
           materialContextOwner_->materialApplication()!=materialApplication||
           !standardMaterialDomainOwner_||!standardMaterialDomainOwner_->prepared())
            return fail(Status::owner,0x1A8AB0u,"same consumed Race/Application/Video required");
        const auto prefix=original_results_roster50LA::Read::completedWorldPrefixes(race);
        const auto table=standardMaterialDomainOwner_->materials382B38();
        const auto provider=standardMaterialDomainOwner_->archiveProvider3E1A70();
        if(!prefix.staticWorld||!prefix.staticModelResources||!prefix.staticModelObservers||
           !prefix.runtimeQueryWorldOwners||!prefix.trackParticleManager||
           prefix.staticWorldImages||prefix.staticWorldDrawPlan||!table||!provider||!provider->sealed()||
           table->coldCollectorCurrent(standardMaterialDomainOwner_->coldCollector14D440()))
            return fail(Status::phase,0x1A8AB0u,"connected loading must precede the combined collector");
        original_results_roster50LA::Read roster(*this,race,std::move(operation),actualHumanRacer60);
        std::size_t count=0;
        if(!roster.current(count))return fail(Status::owner,0x1A8AB0u,actualHumanRacer60?
            "actual retained human Tournament roster unavailable":"actual retained ALL8 roster unavailable");
        //1A8A6C/1A8AA0 selects the same actual directory across BOTH loads;
        //1A90CC restores it. The nested legacy Pause scope restores this value.
        { //Retain1A8A40 directory scope only through its actual UI loads.
        struct DirectoryScope {
            std::string& actual;std::string saved;
            explicit DirectoryScope(std::string& value):actual(value),saved(value){actual="ui\\textures\\";}
            ~DirectoryScope(){actual.swap(saved);}
        } directory(materialDirectory36B970_);
        // Preserve source caller order: Pause100 construction/publication/load/
        // disable precedes Results104 construction and immediate publication.
        if(!loadConsumedPauseSourceCold50LA(race,pause,true))
            return fail(Status::source,0x1A8ADCu,"same counted race Pause load/disable rejected");
        //1A8B00 publishes the actual object before1A8B08 invokes its load.
        race.results104_.reset(new Results(entitiesOwner_,race.entity_.sessionGeneration));
        if(!race.results104_->constructed())
            return fail(Status::owner,0x1A8AF8u,"Results constructor prefix retained but incomplete");
        if(!race.results104_->load1BEEB0(roster,standardMaterialDomainOwner_,materialContextOwner_,
                fonts328(),pauseSourceDataForResults50LA(pause),materialDirectory36B970_,fontDirectory36B420_,
                static_cast<std::uint32_t>(videoOwner_->packetZMaximum),diagnostic))return false;
        std::size_t after=0;
        if(!roster.current(after)||after!=count||
           table->coldCollectorCurrent(standardMaterialDomainOwner_->coldCollector14D440()))
            return fail(Status::owner,0x1A8B1Cu,"Results lost actual roster or pre-collector custody");
        if(!race.results104_->disable1A8B1C()||!race.results104_->loadedDisabled())
            return fail(Status::phase,0x1A8B1Cu,"Results actual disable rejected");
        } //1A90CC restores textures\ before reached1A9100/F8 polygons.
        if(actualHumanRacer60){
            //Only the existing consumed human Tournament cold route. ALL8's
            //live Pause scheduler/forest remains byte-for-byte unextended.
            if(race.f8Owner_||race.f8Source_||race.f8Attached_||race.nativeRootAttached_)
                return fail(Status::phase,0x1A9100u,"F8 already reached or live root attached");
            allocationPC=0x1A9118u;
            race.f8Owner_.reset(new rr::game::OriginalRaceF850LA(entitiesOwner_,race.entity_.sessionGeneration));
            if(!race.f8Owner_->constructed())
                return fail(Status::owner,0x1A912Cu,"F8 counted constructor prefix incomplete");
            rr::game::OriginalRaceF8Diagnostic50LA f8;
            if(!race.f8Owner_->load1985F0(standardMaterialDomainOwner_,materialContextOwner_,fonts328(),
                    pauseSourceDataForResults50LA(pause),materialDirectory36B970_,fontDirectory36B420_,
                    static_cast<std::uint32_t>(videoOwner_->packetZMaximum),f8)){
                diagnostic.object=f8.object;diagnostic.retainedPrimitives=f8.retainedPrimitives;
                return fail(Status::source,f8.sourcePC,f8.operation);
            }
            //Source1A9144 publishes returned F8 AFTER1985F0, unlike Results104.
            race.f8Source_=race.f8Owner_.get();
            if(!race.f8Owner_->initialVisibility1A9170())
                return fail(Status::phase,0x1A9170u,"F8 actual initial199770 rejected");
            std::size_t afterF8=0;
            if(!roster.current(afterF8)||afterF8!=count||
               table->coldCollectorCurrent(standardMaterialDomainOwner_->coldCollector14D440()))
                return fail(Status::owner,0x1AAF54u,"F8 lost same cold roster/pre-collector custody");
            if(!race.attachF8Cold1AAF54())
                return fail(Status::phase,0x1AAF54u,"F8 actual cold Race child attachment rejected");
        }
        return true;
    }catch(const std::bad_alloc&){return fail(Status::allocation,allocationPC,"cold UI allocation failed; reached prefix retained");}
    catch(...){return fail(Status::source,0x1A8B1Cu,"Pause/Results/F8 loading failed; reached owners retained");}
}
}
