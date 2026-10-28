#include "original_results104_storage50la.h"
#include "original_time_stats_state50la.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/ra_cold_roster_operation50la.h"
#include <cmath>
#include <string>
#include <string_view>

namespace original_position_stats_adapter50LA {
bool Owner::readResultsLapCC0(const ResultsRecordIdentity& row,std::size_t index,
    float& output,Diagnostic& d)const noexcept {
    d={};
    try {
        if(index>=3||!resultsRecordIdentityCurrent(row)){d.status=Status::identity;return false;}
        const auto& c=*camera_;const auto& bank=c.actual_.bank;
        //This is the same initialized/updated50ID Bank backing, not the HUD
        //copy or a completed-lap count. Original display reads all three slots.
        const auto lane=bank.bindRead(c.actual_.racerTiming.lapTimesCC0[index]);
        const auto value=bank.read(row.managerIndex,lane);
        if(!resultsRecordIdentityCurrent(row)){d.status=Status::phase;return false;}
        output=value;d.status=Status::complete;return true;
    }catch(...){d.status=Status::read;return false;}
}
}

namespace rr::game {
namespace {
float summaryAdd(float a,float b)noexcept {volatile float r=a+b;return r;}
float summaryMul(float a,float b)noexcept {volatile float r=a*b;return r;}
}
OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::prepareCareerSummaryCpuPrefix(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c6f98;return DisplayCpuPrefix::none;
    }
    const auto prefix=prepareCareerWinningsCpuPrefix(race,observer,d);
    if(!impl_||!impl_->displayCpu||prefix!=DisplayCpuPrefix::careerWinnings||
       d.status!=Impl::S::incomplete)return prefix;
    auto& p=*impl_;auto& out=*p.displayCpu;auto& calls=out.careerSummary;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;p.stop(d,status,pc,operation);return out.prefix;
    };
    const auto finish=[&](){
        out.prefix=DisplayCpuPrefix::careerSummary;
        return stop(Impl::S::incomplete,0x1c8a74,
            "Results slot74 CPU effects only; actual slot48/layout/renderer lifecycle still unjoined");
    };
    if(out.nextSourcePC==0x1c8a74)return finish(); //Whole career page source-skipped.
    if(out.nextSourcePC!=0x1c6f98||!out.career.reached)return prefix;
    const float offset=out.transition.offsetF21;
    if(!std::isfinite(offset))return stop(Impl::S::source,0x1c6f9c,"Results finite career summary offset");
    using Observation=::original_position_stats_adapter50LA::Owner;
    const auto operation=p.loadedRosterOperation.lock();const auto generation=p.component.sessionGeneration;
    const auto current=[&](){const auto now=race.profile_->stamp();
        return operation&&operation->aggregate_==race.world74_&&operation->entities_==p.application&&
            operation->domain_==p.domain&&operation->generation_==generation&&
            operation->resultsRuntimeBankCurrent(observer.resultsBankIdentity())&&
            now.generation==out.career.profileGeneration&&now.mode34==out.career.profileMode34&&p.context->live();
    };
    Observation::ResultsRecordIdentity row;Observation::Diagnostic observed;
    if(!current()||!observer.captureResultsRecordIdentity(p.application.get(),p.domain->materials382B38().get(),
            generation,0,row,observed)||row.publication!=out.career.publication||
       row.bank!=observer.resultsBankIdentity()||row.vehicle58!=0||
       !operation->resultsRacerIdentityCurrent(0,row.racer60)||!current())
        return stop(Impl::S::owner,0x1c6f98,"Results same first-human Profile/Bank/publication no longer current");
    const auto samePublication=[&](){return current()&&observer.resultsRecordIdentityCurrent(row);};
    if(!p.text1770)return stop(Impl::S::owner,0x1c6f98,"Results actual three career-message Text owners absent");
    //The positive-bonus edge loaded1780 in1C6A8C's delay slot and lands6F9C;
    //other edges load it at6F98. Native position() performs the equivalent
    //comparison/effect once here; no source load/branch receipt is claimed.
    p.position((*p.text1770)[0],summaryAdd(320.f,offset),305.f);
    p.position((*p.text1770)[1],summaryAdd(320.f,offset),255.f);
    p.position((*p.text1770)[2],summaryAdd(320.f,offset),275.f);
    calls.positionsPrepared=true;
    if(!samePublication()||!race.profile_->resultsCareerProgress219C(calls.progress219C))
        return stop(Impl::S::owner,0x1c710c,"Results actual comparison100C0 career progress219C absent");
    try {
        const auto localized=[&](std::size_t id,std::string& value,std::uint32_t pc){
            out.nextSourcePC=pc;const auto* english=p.sourceData?p.sourceData->english():nullptr;
            const auto* entry=english?english->at(id):nullptr;
            if(!entry)return p.stop(d,Impl::S::source,pc,"Results retained localization entry absent");
            value=entry->text;return true;
        };
        const auto message=[&](std::size_t index,std::string_view bytes,std::uint32_t pc){
            out.nextSourcePC=pc;auto& text=(*p.text1770)[index];
            if(!text.string8)return p.stop(d,Impl::S::owner,pc,"Results owned message string backing absent");
            p.assign(text,bytes.data(),bytes.size());
            if(!p.projectTextCpu2E0170(text,pc,d))return false;
            calls.messages[index]={&text,pc};return true;
        };
        const auto localizedMessage=[&](std::size_t index,std::size_t id,std::uint32_t readPC,std::uint32_t drawPC){
            std::string value;return localized(id,value,readPC)&&message(index,value,drawPC);
        };
        const auto nextCourse=[&](std::uint32_t readPC,std::uint32_t drawPC){
            std::string value,title;if(!localized(444,value,readPC))return false;
            std::int32_t id=-1;out.nextSourcePC=0x187480;
            if(!samePublication()||!race.profile_->resultsNextCourseTitle187480(id))
                return p.stop(d,Impl::S::source,0x187480,"Results actual next-course title absent/null; no invented fallback");
            if(!localized(static_cast<std::size_t>(id),title,0x187590))return false;
            value+=' ';value+=title;return message(2,value,drawPC); //Exact3ACAE0.
        };
        const auto offer=[&](std::uint32_t readPC,std::uint32_t drawPC){
            //Both source occurrences use the INITIAL place/CD0 observations,
            //not qualification's later reads. Perfect1st+7 omits this draw.
            return (out.career.placeCCC==1&&out.career.observedCD0==7)||
                localizedMessage(0,441,readPC,drawPC);
        };
        if(calls.progress219C==out.career.courseOrder){
            if(out.career.retirementCD4){
                if(!localizedMessage(1,440,0x1c7128,0x1c71c4)||
                   !localizedMessage(2,442,0x1c71d8,0x1c7274))return out.prefix;
            }else{
                //187C80 repeats selected order, manager0 CCC, manager0 CD4,
                //then (only if not retired) comparison-career progress219C.
                std::uint32_t order=0,retired=0;std::int32_t place=0;
                if(!samePublication()||!race.profile_->resultsSelectedOrder187B10(order))
                    return stop(Impl::S::owner,0x187ca4,"Results actual187C80 selected-course scan absent");
                if(!observer.readEndPlaceCCC(row,place,observed))
                    return stop(Impl::S::owner,0x187cfc,"Results actual qualification CCC reread absent");
                if(!observer.readEndRetirementCD4(row,retired,observed))
                    return stop(Impl::S::owner,0x187d18,"Results actual qualification CD4 reread absent");
                calls.qualificationReached=true;
                if(!retired){
                    std::uint8_t progress=0;
                    if(!samePublication()||!race.profile_->resultsCareerProgress219C(progress))
                        return stop(Impl::S::owner,0x187d3c,"Results actual qualification progress reread absent");
                    calls.qualified=order==progress&&place<4; //SignedSLTI, no invented positive-place gate.
                }
                if(!calls.qualified){
                    if(!localizedMessage(1,440,0x1c7b00,0x1c7b9c)||
                       !localizedMessage(2,443,0x1c7bb0,0x1c7c4c))return out.prefix;
                }else{
                    const auto oneBased=out.career.courseOrder+1u;
                    const auto kind=out.career.courseClass;
                    const bool boundary=(kind==1u&&oneBased==4u)||(kind==2u&&oneBased==8u)||(kind==3u&&oneBased==13u);
                    if(boundary){
                        if(kind==1u){
                            if(!localizedMessage(1,436,0x1c730c,0x1c73a8)||!nextCourse(0x1c743c,0x1c74f4))return out.prefix;
                        }else if(kind==2u){
                            if(!localizedMessage(1,437,0x1c750c,0x1c75a8)||!nextCourse(0x1c763c,0x1c76f4))return out.prefix;
                        }else if(!localizedMessage(1,439,0x1c770c,0x1c77a8))return out.prefix;
                    }else{
                        if(!localizedMessage(1,438,0x1c77c0,0x1c785c)||!nextCourse(0x1c78f0,0x1c79a8))return out.prefix;
                    }
                    if(!offer(0x1c7a4c,0x1c7ae8))return out.prefix;
                }
            }
        }else if(out.career.retirementCD4){
            if(!message(1,"YOU MUST FINISH THE RACE",0x1c7d00)||
               !message(2,"TO WIN MORE TRUGUTS",0x1c7da4))return out.prefix;
        }else if(!(out.career.placeCCC<out.career.priorMedal)){
            if(!message(1,"YOU MUST BEAT YOUR BEST FINISH",0x1c7e58))return out.prefix;
            constexpr std::array<std::string_view,8> prior{{
                "(1ST PLACE) TO WIN MORE TRUGUTS","(2ND PLACE) TO WIN MORE TRUGUTS",
                "(3RD PLACE) TO WIN MORE TRUGUTS","(4TH PLACE) TO WIN MORE TRUGUTS",
                "(5TH PLACE) TO WIN MORE TRUGUTS","(6TH PLACE) TO WIN MORE TRUGUTS",
                "(7TH PLACE) TO WIN MORE TRUGUTS","(8TH PLACE) TO WIN MORE TRUGUTS"}};
            const auto medal=out.career.priorMedal;
            if(!message(2,prior[medal>=1&&medal<=7?std::size_t(medal-1):7u],0x1c7fe4))return out.prefix;
        }else{
            //Original only writes1850 here; do not clear or draw stale17E0.
            if(!message(2,"YOU HAVE WON MORE TRUGUTS!",0x1c808c)||!offer(0x1c8130,0x1c81cc))return out.prefix;
        }
        if(!samePublication())return stop(Impl::S::owner,0x1c81d4,"Results publication changed after career messages");
        if(!p.statistics1230)return stop(Impl::S::owner,0x1c81e0,"Results actual eight statistics Text owners absent");
        const auto statistic=[&](std::size_t i,std::string_view bytes,float x,float y,std::uint32_t pc){
            out.nextSourcePC=pc;auto& text=(*p.statistics1230)[i];
            if(!text.string8)return p.stop(d,Impl::S::owner,pc,"Results owned statistics string backing absent");
            p.assign(text,bytes.data(),bytes.size());p.position(text,x,y);
            if(!p.projectTextCpu2E0170(text,pc,d))return false;
            calls.statistics[i]={&text,pc};return true;
        };
        for(std::size_t i=0;i<3;++i){
            std::string label;if(!localized(453,label,0x1c8268))return out.prefix;
            label+=' ';label+=char('1'+i);label+=':'; //Original3AD090 "%s %d:".
            const float y=summaryAdd(130.f,summaryMul(float(i),18.f));
            if(!statistic(i,label,summaryAdd(510.f,offset),y,0x1c8410))return out.prefix;
            if(!samePublication()||!observer.readResultsLapCC0(row,i,calls.lapCC0[i],observed))
                return stop(Impl::S::owner,0x1c843c,"Results actual reached CC0 lap-array read absent");
            std::array<char,9> bytes{};out.nextSourcePC=0x1c8440;
            if(!time_stats_detail50LA::time189000(calls.lapCC0[i],bytes))
                return stop(Impl::S::source,0x1c8440,"Results native189000 lap formatting domain");
            if(!statistic(i+4,std::string_view(bytes.data(),8),summaryAdd(600.f,offset),y,0x1c860c))return out.prefix;
        }
        std::string totalLabel;if(!localized(433,totalLabel,0x1c86a4))return out.prefix;
        const float totalY=summaryAdd(130.f,summaryMul(3.f,18.f));
        if(!statistic(3,totalLabel,summaryAdd(510.f,offset),totalY,0x1c87c8))return out.prefix;
        if(!samePublication()||!observer.readResultsRaceTimeCB8(row,calls.timeCB8,observed))
            return stop(Impl::S::owner,0x1c87e8,"Results actual current total CB8 read absent");
        std::array<char,9> bytes{};out.nextSourcePC=0x1c87ec;
        if(!time_stats_detail50LA::time189000(calls.timeCB8,bytes))
            return stop(Impl::S::source,0x1c87ec,"Results native189000 total formatting domain");
        if(!statistic(7,std::string_view(bytes.data(),8),summaryAdd(600.f,offset),totalY,0x1c890c))return out.prefix;
        if(!p.separators1030)return stop(Impl::S::owner,0x1c8914,"Results actual third separator Texture absent");
        auto& separator=(*p.separators1030)[2];out.nextSourcePC=0x1c8914;
        p.position(separator,summaryAdd(457.f,offset),182.f);
        p.dimension(separator,separator.width34,145.f);p.dimension(separator,separator.height38,3.f);
        out.nextSourcePC=0x1c8a6c;
        if(!p.projectTextureCpu2DF694(separator,0x1c8a6c,d))return out.prefix;
        calls.separator={&separator,0x1c8a6c};
        if(!samePublication())return stop(Impl::S::owner,0x1c8a74,"Results publication changed after CPU career summary");
        return finish();
    }catch(...){return stop(Impl::S::allocation,out.nextSourcePC,"Results summary formatting allocation; reached CPU effects retained");}
}
}
