#include "original_results104_storage50la.h"
#include "original_hall_record_queries50kg.h"
#include "../../tests/native/original_results_roster_read50la.h"
#include "../../platform/linux/native/original_persisted_profile_name50kg.h"
#include <string>


namespace rr::game {
OriginalResults10450LA::NameEntryCpuPrefix OriginalResults10450LA::prepareNameEntryCpu1C9B10(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    using P=NameEntryCpuPrefix;
    using Read=::original_results_roster50LA::Read;
    d={};if(!impl_){d.status=Impl::S::owner;d.sourcePC=0x1c9b10;return P::none;}
    auto& p=*impl_;p.nameEntryCpu.reset();
    if(!constructed()||!p.loadComplete||!p.base||!p.derived||!p.keysA94||
       !p.publishedA94||p.keysConstructed!=42||!p.text780||!p.editA9C||
       !p.context||!p.context->live()||!p.domain||!p.sourceData||!p.sourceData->sourceLoaded()||
       !retirementCurrent()||!nameEntryRaceCurrent1C9B10(race,this,p.application,p.component.sessionGeneration)){
        p.stop(d,Impl::S::owner,0x1c9b10,"Results1C9B10 requires its actual retained Race/Profile/resources");
        return P::none;
    }
    for(std::size_t i=0;i<42;++i)if(!p.keysA94[i]){
        p.stop(d,Impl::S::owner,0x1c9b38,"Results actual keyboard Text owner absent");return P::none;
    }
    // Native cache invalidation, not source activation or layout208 mutation.
    p.displayCpu.reset();p.nameEntryCpu.emplace();
    if(!p.nameEntry)p.nameEntry.emplace();
    auto& fields=*p.nameEntry;auto& out=*p.nameEntryCpu;
    out.session=p.component.sessionGeneration;
    const auto before=Read::profileStamp(race);out.profileGeneration=before.generation;out.profileMode34=before.mode34;
    const auto profileCurrent=[&](){const auto now=Read::profileStamp(race);
        return now.generation==before.generation&&now.mode34==before.mode34&&p.context->live();};
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;p.stop(d,status,pc,operation);return out.prefix;
    };
    try {
        // Preserve1C9B38..1C9C64: all42 yellow, ABC/AC0=42, DONE gray,
        // AC4=-1, AB8=0, AB4=1. No text geometry or submit occurs here.
        for(std::size_t i=0;i<42;++i)p.color(*p.keysA94[i],p.base->yellow16C);
        fields.fieldABC=42;fields.fieldAC0=42;
        p.color(*p.keysA94[41],p.base->grey170);
        fields.fieldAC4=-1;fields.fieldAB8=0;fields.fieldAB4=1;
        out.prefix=P::keyboard;
        std::uint32_t course=0;
        if(!Read::profileNameCourse(race,course)||!profileCurrent())
            return stop(Impl::S::owner,0x1c9c80,"Results actual Profile1125F course read absent or changed");
        fields.fieldAD8=1; //1C9C88, AFTER original course read.
        if(p.derived->mode200!=3u)
            return stop(Impl::S::incomplete,0x1ca040,"Results second-human1C9B10 branch lacks an admitted actual two-human owner");

        platform::OriginalPersistedProfileName50KG name;
        if(!Read::profileFirstName(race,name)||!profileCurrent())
            return stop(Impl::S::owner,0x1c9ca4,"Results actual career219D/player10F68 name unavailable");
        std::size_t length=0;
        while(length<name.knownByteCount()&&name.c_str()[length])++length;
        if(length==name.knownByteCount())
            return stop(Impl::S::source,0x1c9cd8,"Results persisted name has no known terminating byte");
        const auto* english=p.sourceData->english();
        const auto* player=english?english->at(172):nullptr;
        const auto* heading=english?english->at(462):nullptr;
        const char* nameBytes=name.c_str();
        const bool fallbackName=length==0;
        if(fallbackName){
            if(!player)return stop(Impl::S::source,0x1c9d58,"Results first-human fallback localization172 absent");
            nameBytes=player->text.c_str();length=std::char_traits<char>::length(nameBytes);
        }
        if(length>23u)
            return stop(Impl::S::source,0x1c9d60,"Results known name exceeds original A9C..AB3 C-string backing");
        out.nextSourcePC=fallbackName?(before.mode34==2u?0x1c9d60u:0x1c9e10u):
            (before.mode34==2u?0x1c9cd8u:0x1c9d88u);
        p.editA9C->assign(nameBytes,length);out.prefix=P::name;
        // The title uses localized PLAYER1 and462, NOT the entered name.
        if(!player||!heading)return stop(Impl::S::source,0x1c9e18,"Results source title localizations172/462 absent");
        out.nextSourcePC=0x1c9f30;
        std::string title(player->text.c_str());title+=" - ";title+=heading->text.c_str(); //3ACAD8="%s - %s".
        p.assign(*p.text780,title.data(),title.size());out.prefix=P::title;
        if(!profileCurrent())return stop(Impl::S::owner,0x1c9fc4,"Results Profile changed after title preparation");

        Read::Record row;
        const auto* materials=p.domain->materials382B38().get();const auto generation=out.session;
        const auto operation=p.loadedRosterOperation.lock();
        const auto current=[&](){
            return profileCurrent()&&Read::loadedOperationOwnerFieldsCurrent(operation,Read::raceWorldIdentity(race),
                p.application,p.domain,generation)&&Read::loadedOperationBankCurrent(operation,Read::observerBankIdentity(observer));
        };
        if(!current()||!Read::captureRecord(observer,p.application.get(),materials,generation,0,row)||
           row.bank!=Read::observerBankIdentity(observer)||row.vehicle58!=0u||
           !Read::loadedOperationRacerCurrent(operation,0,row.racer60)||!current())
            return stop(Impl::S::owner,0x1c9fd4,"Results first-human current loaded Bank/rig identity absent");
        out.managerIndex=0;out.publication=row.publication;
        const auto samePublication=[&](){return current()&&Read::recordCurrent(observer,row);};
        float bestLap=0;
        if(!samePublication()||!Read::readBestLap(observer,row,bestLap))
            return stop(Impl::S::owner,0x1c9fe4,"Results actual current CB4 read absent");
        const auto lap=Read::profileLapQuery(race,course,bestLap);
        if(!lap.completed())return stop(Impl::S::source,lap.sourcePC,"Results actual Profile lap-record query unsupported");
        fields.lapAC8=lap.originalV0;out.prefix=P::lapQuery; //1C9FF8.
        if(!samePublication())return stop(Impl::S::owner,0x1ca000,"Results owners changed after lap-record query");
        float raceSeconds=0;
        if(!Read::readRaceTime(observer,row,raceSeconds))
            return stop(Impl::S::owner,0x1ca004,"Results actual current CB8 read absent");
        const auto raceTime=Read::profileRaceQuery(race,course,raceSeconds);
        if(!raceTime.completed())return stop(Impl::S::source,raceTime.sourcePC,"Results actual Profile race-record query unsupported");
        fields.raceACC=raceTime.originalV0;out.prefix=P::raceQuery; //1CA014.
        if(!samePublication())return stop(Impl::S::owner,0x1ca018,"Results owners changed after race-record query");
        std::uint32_t observedTimingCD0=0;
        if(!Read::readObservedTiming(observer,row,observedTimingCD0))
            return stop(Impl::S::owner,0x1ca020,"Results actual current timing CD0 read absent");
        const auto knockout=Read::profileKnockoutQuery(race,course,observedTimingCD0);
        if(!knockout.completed())return stop(Impl::S::source,knockout.sourcePC,"Results actual Profile knockout-record query unsupported");
        fields.knockoutAD0=knockout.originalV0; //1CA034, no record mutation.
        fields.sideAD4=1u; //Genuine1CA03C producer, NEVER a constructor default.
        out.prefix=P::recordQueries;
        if(!samePublication())return stop(Impl::S::owner,0x1ca314,"Results owners changed after record-query producer");
        // Relevant first-human Results-object stores are represented. Native
        // checked localization omits retail debug/assert globals3D6B20/gp-7F60.
        // Observed CD0 is NOT earned credit; no input/live callback is admitted.
        return stop(Impl::S::incomplete,0x1ca314,"Results first-human name/record CPU producer only; scheduler/input/earned-credit join absent");
    }catch(...){return stop(Impl::S::allocation,out.nextSourcePC,
        "Results native name allocation failed; reached source fields retained, no producer completion");}
}
}
