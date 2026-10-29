#include "original_results104_storage50la.h"
#include "original_time_stats_state50la.h"
#include "../../tests/native/original_results_roster_read50la.h"
#include <bit>
#include <charconv>
#include <cmath>

namespace rr::game {
namespace {
float recordAdd(float a,float b)noexcept {volatile float result=a+b;return result;}
}
OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::prepareRecordSummaryCpuPrefix(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    const auto prefix=prepareRosterPageCpuPrefix(race,observer,d);
    if(!impl_||!impl_->displayCpu||prefix!=DisplayCpuPrefix::roster||
       d.status!=Impl::S::incomplete||impl_->displayCpu->nextSourcePC!=0x1c3a74)return prefix;
    auto& p=*impl_;auto& out=*p.displayCpu;auto& calls=out.records;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;p.stop(d,status,pc,operation);return out.prefix;
    };
    const auto endPage=[&](const char* operation){
        out.prefix=DisplayCpuPrefix::recordSummary;
        return stop(Impl::S::incomplete,0x1c4ca0,operation);
    };
    float offset=out.transition.offsetF31;
    const float entering=out.transition.stackD4;
    if(!std::isfinite(offset)||!std::isfinite(entering))
        return stop(Impl::S::source,0x1c3a74,"Results finite record-page transition domain");
    //1C3A7C..3AD4: F31 in[-640,640] OR stackD4 in[0,640]. A skipped
    //page performs no ADC, name-entry, manager or text reads.
    if(!(offset<=640.f&&!(offset<-640.f))&&!(entering>=0.f&&entering<=640.f))
        return endPage("Results record page source-skipped; later pages/renderer absent");
    const auto text=[&](Impl::Text& t,Impl::DisplayCpuPreparation::TextCall& call,std::uint32_t pc){
        out.nextSourcePC=pc;
        if(!p.projectTextCpu2E0170(t,pc,d))return false;
        call={&t,pc};return true;
    };
    const float cheat=std::bit_cast<float>(p.derived->fieldADC);
    if(!std::isfinite(cheat))return stop(Impl::S::source,0x1c3adc,"Results finite ADC cheat-display timer domain");
    if(!(cheat<=0.f)){
        if(!p.text710)return stop(Impl::S::owner,0x1c3af8,"Results retained cheat Text710 absent");
        p.position(*p.text710,320.f,200.f);
        if(!text(*p.text710,calls.cheat,0x1c3b6c))return out.prefix;
        return endPage("Results source cheat-text branch only; later pages/renderer absent");
    }

    // Original row selection, never force mode3 to make this path reachable.
    // Mode3 uses0; otherwise previous3 consults genuine AD4; all other paths
    //use1 and remain unsupported until a real two-human owner is admitted.
    std::size_t manager=1;std::uint32_t managerPC=0x1c3bec;
    if(p.derived->mode200==3u){manager=0;managerPC=0x1c3b98;}
    else if(p.derived->previous204==3u){
        if(!p.nameEntry||!p.nameEntry->sideAD4)
            return stop(Impl::S::incomplete,0x1c3bb4,"Results AD4 has no reached1C9B10 producer");
        if(*p.nameEntry->sideAD4){manager=0;managerPC=0x1c3bd0;}
    }
    if(manager!=0)
        return stop(Impl::S::incomplete,managerPC,"Results record page requires unjoined actual second-human row1");
    using Read=::original_results_roster50LA::Read;
    Read::Record row;
    const auto* materials=p.domain->materials382B38().get();const auto generation=p.component.sessionGeneration;
    const auto operation=p.loadedRosterOperation.lock();const auto before=Read::profileStamp(race);
    const auto current=[&](){const auto now=Read::profileStamp(race);
        return Read::loadedOperationOwnerFieldsCurrent(operation,Read::raceWorldIdentity(race),p.application,p.domain,generation)&&
            Read::loadedOperationBankCurrent(operation,Read::observerBankIdentity(observer))&&
            now.generation==before.generation&&now.mode34==before.mode34&&p.context->live();
    };
    if(!current()||!Read::captureRecord(observer,p.application.get(),materials,generation,manager,row)||
       row.bank!=Read::observerBankIdentity(observer)||row.vehicle58!=0u||
       !Read::loadedOperationRacerCurrent(operation,manager,row.racer60)||!current())
        return stop(Impl::S::owner,managerPC,"Results actual first-human loaded Bank/rig/publication absent");
    calls.managerIndex=manager;calls.publication=row.publication;
    const auto samePublication=[&](){return current()&&Read::recordCurrent(observer,row);};
    //1C3C10 BC1TL executes MOV.S F31,F0 only if stackD4<=640. The outer
    //window gate does NOT add a lower-bound test to this particular overwrite.
    if(entering<=640.f)out.transition.offsetF31=offset=entering;
    if(!p.text780)return stop(Impl::S::owner,0x1c3c18,"Results retained title Text780 absent");
    p.position(*p.text780,recordAdd(320.f,offset),70.f);
    if(!text(*p.text780,calls.title,0x1c3ce0))return out.prefix;

    // The title may precede an unavailable query-state suffix. Only genuine
    //completed1C9B10 state, same Profile/session/first-human selection, supplies
    //AC8/ACC/AD0. Its original publication may be older: source state persists
    //across frames; displayed numeric lanes below are freshly read NOW.
    if(!p.nameEntry||!p.nameEntryCpu||p.nameEntryCpu->prefix!=NameEntryCpuPrefix::recordQueries||
       p.nameEntryCpu->session!=generation||p.nameEntryCpu->profileGeneration!=before.generation||
       p.nameEntryCpu->profileMode34!=before.mode34||p.nameEntryCpu->managerIndex!=manager||
       !p.nameEntry->lapAC8||!p.nameEntry->raceACC||
       !p.nameEntry->knockoutAD0||!samePublication())
        return stop(Impl::S::incomplete,0x1c3ce8,"Results current same-owner1C9B10 query-state producer absent");
    const auto timeValue=[&](Impl::Text& t,float value,float y,
        Impl::DisplayCpuPreparation::TextCall& call,std::uint32_t formatPC,std::uint32_t callPC){
        out.nextSourcePC=formatPC;std::array<char,9> bytes{};
        if(!time_stats_detail50LA::time189000(value,bytes))
            return p.stop(d,Impl::S::source,formatPC,"Results source189000 record time formatting domain");
        p.assign(t,bytes.data(),8);p.position(t,recordAdd(390.f,offset),y);
        return text(t,call,callPC);
    };
    if(*p.nameEntry->lapAC8){
        if(!p.text7F0||!p.text940)return stop(Impl::S::owner,0x1c3cf4,"Results own best-lap text resources absent");
        out.nextSourcePC=0x1c3d78;
        if(!p.localized(*p.text7F0,463,d))return out.prefix;
        p.position(*p.text7F0,recordAdd(320.f,offset),100.f);
        if(!text(*p.text7F0,calls.lapLabel,0x1c3e7c))return out.prefix;
        float bestLap=0;
        if(!samePublication()||!Read::readBestLap(observer,row,bestLap))
            return stop(Impl::S::owner,0x1c3e84,"Results actual current CB4 read absent");
        if(!timeValue(*p.text940,bestLap,100.f,calls.lapValue,0x1c3e88,0x1c3f88))return out.prefix;
    }
    if(!samePublication())return stop(Impl::S::owner,0x1c3f90,"Results publication changed after best-lap row");
    if(*p.nameEntry->raceACC){
        if(!p.text860||!p.text9B0)return stop(Impl::S::owner,0x1c3f9c,"Results own race-time text resources absent");
        out.nextSourcePC=0x1c4020;
        if(!p.localized(*p.text860,464,d))return out.prefix;
        p.position(*p.text860,recordAdd(320.f,offset),120.f);
        if(!text(*p.text860,calls.raceLabel,0x1c4124))return out.prefix;
        float raceSeconds=0;
        if(!samePublication()||!Read::readRaceTime(observer,row,raceSeconds))
            return stop(Impl::S::owner,0x1c412c,"Results actual current CB8 read absent");
        if(!timeValue(*p.text9B0,raceSeconds,120.f,calls.raceValue,0x1c4130,0x1c4230))return out.prefix;
    }
    if(!samePublication())return stop(Impl::S::owner,0x1c4238,"Results publication changed after race-time row");
    if(*p.nameEntry->knockoutAD0){
        if(!p.text8D0||!p.textA20)return stop(Impl::S::owner,0x1c4244,"Results own knockout text resources absent");
        out.nextSourcePC=0x1c42c8;
        if(!p.localized(*p.text8D0,465,d))return out.prefix;
        p.position(*p.text8D0,recordAdd(320.f,offset),140.f);
        if(!text(*p.text8D0,calls.knockoutLabel,0x1c43cc))return out.prefix;
        std::uint32_t count=0;
        if(!samePublication()||!Read::readObservedTiming(observer,row,count))
            return stop(Impl::S::owner,0x1c43d4,"Results actual current timing CD0 read absent");
        // Source3ACE10 is signed%d. Preserve its signed32 interpretation and
        //the actual TextA20 capacity2 truncation; do not invent a cap7 count.
        std::array<char,12> bytes{};
        const auto result=std::to_chars(bytes.data(),bytes.data()+bytes.size(),std::bit_cast<std::int32_t>(count));
        if(result.ec!=std::errc{})return stop(Impl::S::source,0x1c43e0,"Results signed32 knockout formatting");
        p.assign(*p.textA20,bytes.data(),static_cast<std::size_t>(result.ptr-bytes.data()));
        p.position(*p.textA20,recordAdd(390.f,offset),140.f);
        if(!text(*p.textA20,calls.knockoutValue,0x1c44e0))return out.prefix;
    }
    if(!samePublication())return stop(Impl::S::owner,0x1c44e8,"Results publication changed after record rows");
    // Reached keyboard-highlight prefix. Its subsequent pulse reads394; no
    //constructor/default or caller timer is substituted for that unknown lane.
    if(!p.keysA94||p.keysConstructed!=42||!p.nameEntry->fieldAC0||
       *p.nameEntry->fieldAC0<1u||*p.nameEntry->fieldAC0>42u)
        return stop(Impl::S::incomplete,0x1c44e8,"Results known previous keyboard indexAC0 absent");
    auto* previous=p.keysA94[*p.nameEntry->fieldAC0-1u].get();
    if(!previous)return stop(Impl::S::owner,0x1c44fc,"Results actual previous keyboard Text absent");
    p.color(*previous,p.base->yellow16C);p.scale(*previous,1.f);calls.previousKeyReset=true;
    if(!p.nameEntry->fieldABC||*p.nameEntry->fieldABC<1u||*p.nameEntry->fieldABC>42u)
        return stop(Impl::S::incomplete,0x1c45c8,"Results known selected keyboard indexABC absent");
    auto* selected=p.keysA94[*p.nameEntry->fieldABC-1u].get();
    if(!selected)return stop(Impl::S::owner,0x1c45dc,"Results actual selected keyboard Text absent");
    p.color(*selected,p.base->grey170);calls.currentKeyColored=true;
    out.prefix=DisplayCpuPrefix::recordSummary;
    return stop(Impl::S::incomplete,0x1c4644,"Results record-summary/highlight prefix only; pulse394 producer and remaining page/renderer absent");
}
}
