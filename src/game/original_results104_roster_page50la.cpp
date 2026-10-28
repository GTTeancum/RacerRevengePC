#include "original_results104_storage50la.h"
#include "original_time_stats_state50la.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include "../../tests/native/ra_cold_roster_operation50la.h"
#include <cmath>

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

namespace rr::game {
namespace {
float pageAdd(float a,float b)noexcept {volatile float r=a+b;return r;}
float pageSub(float a,float b)noexcept {volatile float r=a-b;return r;}
float pageMul(float a,float b)noexcept {volatile float r=a*b;return r;}
float pageDiv(float a,float b)noexcept {volatile float r=a/b;return r;}
}

OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::prepareRosterPageCpuPrefix(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    const auto prefix=prepareDisplayCpuPrefix(race,d);
    if(!impl_||!impl_->displayCpu||prefix!=DisplayCpuPrefix::titles||
       d.status!=Impl::S::incomplete||impl_->displayCpu->nextSourcePC!=0x1c28f4)return prefix;
    auto& p=*impl_;auto& out=*p.displayCpu;
    const auto incomplete=[&](std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;p.stop(d,Impl::S::incomplete,pc,operation);return out.prefix;
    };
    // Exact1C28FC C.LE.S and1C2918 C.LT.S; the offscreen block skips ALL
    // header/manager reads. No empty full callback is claimed at its next page.
    const float offset=out.transition.offsetF20;
    if(!(offset<=640.f)||offset<-640.f){
        out.prefix=DisplayCpuPrefix::roster;
        return incomplete(0x1c3a74,"Results roster block source-skipped; later pages/renderer absent");
    }
    if(!p.labels3A0||!p.medals5F0||!p.rows5E0||!p.portraits480||!p.published5E0||
       !p.published480||p.rowsCount!=3*p.extent||p.portraitsCount!=p.extent){
        p.stop(d,Impl::S::owner,0x1c292c,"Results own roster ScreenObjects absent");return out.prefix;
    }
    const auto text=[&](Impl::Text& t,Impl::DisplayCpuPreparation::TextCall& call,
        std::uint32_t pc){
        if(!p.projectTextCpu2E0170(t,pc,d))return false;call={&t,pc};return true;
    };
    const auto texture=[&](Impl::Texture& t,Impl::DisplayCpuPreparation::TextureCall& call,
        std::uint32_t pc){
        if(!p.projectTextureCpu2DF694(t,pc,d))return false;call={&t,pc};return true;
    };
    // Source headers precede the first manager-count read1C3A60.
    p.position((*p.labels3A0)[0],pageAdd(148.f,offset),66.f);
    d.object=0x3a0;
    if(!text((*p.labels3A0)[0],out.rosterHeaders[0],0x1c29e0))return out.prefix;
    ++out.rosterHeaderCount;
    p.position((*p.labels3A0)[1],pageAdd(500.f,offset),66.f);
    d.object=0x410;
    if(!text((*p.labels3A0)[1],out.rosterHeaders[1],0x1c2a5c))return out.prefix;
    ++out.rosterHeaderCount;

    using Observation=::original_position_stats_adapter50LA::Owner;
    Observation::ResultsRows rows;
    Observation::Diagnostic observation;
    const auto* materials=p.domain->materials382B38().get();
    const auto generation=p.component.sessionGeneration;
    // Exact successful-load identity, never merely same Application/count.
    // Lock lives only through this synchronous preparation. The marker itself
    // is weak and cannot extend World74 or partial-load cleanup lifetime.
    const auto operation=p.loadedRosterOperation.lock();
    const auto operationCurrent=[&](){
        return operation&&operation->aggregate_==race.world74_&&
            operation->entities_==p.application&&operation->domain_==p.domain&&
            operation->generation_==generation&&
            operation->resultsRuntimeBankCurrent(observer.resultsBankIdentity());
    };
    if(!operationCurrent()||
       !observer.captureResultsRows(p.application.get(),materials,generation,rows,observation)||
       rows.bank!=observer.resultsBankIdentity()||!operationCurrent()||
       rows.count!=p.extent||rows.count>out.rosterRows.size()){
        p.stop(d,Impl::S::owner,0x1c3a60,"Results current published same-owner manager/Bank rows absent");
        return out.prefix;
    }
    out.rosterPublication=rows.publication;
    const auto before=race.profile_->stamp();const auto* profile=race.profile_->commonProfile();
    if(!profile||!profile->known){p.stop(d,Impl::S::owner,0x1c3058,"Results current Profile absent");return out.prefix;}
    const auto current=[&](){
        const auto stamp=race.profile_->stamp();
        return operationCurrent()&&stamp.generation==before.generation&&stamp.mode34==before.mode34&&
            observer.resultsRowsCurrent(p.application.get(),materials,generation,rows.publication)&&
            p.context->live();
    };
    // This implements actual1B0BB0 arithmetic, NOT a PositionStats display
    // estimate: leaderCB8 / ((leaderLap*length + length-remaining)/(length*laps)),
    // then selectedCBC unless selected IS leader. Source uses timingB4 record3C
    // rows (count4C copied from rosterB0.count30 at1AFB08) in order and
    // overwrites each matching pointer; admitted ranks are a
    // permutation, so its first/last matching identities coincide here.
    const auto projectedTime=[&](std::size_t selected,float& seconds){
        std::size_t leader=rows.count;
        for(std::size_t i=0;i<rows.count;++i)if(rows.rows[i].placeCCC==1)leader=i;
        if(leader==rows.count)return p.stop(d,Impl::S::source,0x1b0c04,"Results actual leader record absent");
        const auto& first=rows.rows[leader];float value=first.timeCB8;
        if(!std::isfinite(value))return p.stop(d,Impl::S::source,0x1b0c10,"Results leaderCB8 finite domain");
        if(!(first.flagsCD8&1u)){
            const float length=rows.trackLength38;
            if(!std::isfinite(length)||length<=0.f||!std::isfinite(first.remainingCC4))
                return p.stop(d,Impl::S::source,0x1b0c20,"Results actual manager length/leaderCC4");
            const float completed=pageMul(float(first.lapCC8),length);
            const float partial=pageSub(length,first.remainingCC4);
            const float distance=pageAdd(completed,partial);
            const float total=pageMul(length,float(profile->playerBits1125C&31u));
            if(!std::isfinite(distance)||!std::isfinite(total)||total==0.f)
                return p.stop(d,Impl::S::source,0x1b0c78,"Results projected race distance finite domain");
            const float fraction=pageDiv(distance,total);
            if(!std::isfinite(fraction)||fraction==0.f)
                return p.stop(d,Impl::S::source,0x1b0c80,"Results projected race fraction finite domain");
            value=pageDiv(value,fraction);
        }
        if(leader!=selected){
            if(!std::isfinite(rows.rows[selected].valueCBC))
                return p.stop(d,Impl::S::source,0x1b0c98,"Results actual selectedCBC finite domain");
            value=pageAdd(value,rows.rows[selected].valueCBC);
        }
        if(!std::isfinite(value))return p.stop(d,Impl::S::source,0x1b0ca0,"Results projected time finite domain");
        seconds=value;return true;
    };
    for(std::size_t rank=0;rank<rows.count;++rank){
        if(!current()){p.stop(d,Impl::S::owner,0x1c3a50,"Results read publication changed between rows");return out.prefix;}
        //1C2A78..2AA0 searches manager order for CCC==rank+1. Never sort by
        //HOST elapsed time, rearrange portraits by rank, or relabel a human.
        std::size_t manager=0;
        for(;manager<rows.count;++manager)if(rows.rows[manager].placeCCC==std::int32_t(rank+1))break;
        if(manager==rows.count){p.stop(d,Impl::S::source,0x1c2a94,"Results rank has no real manager row");return out.prefix;}
        const auto& row=rows.rows[manager];
        if(!operation->resultsRacerIdentityCurrent(manager,row.racer60)){
            p.stop(d,Impl::S::owner,0x1c2ac0,"Results current racer differs from loaded portrait rig identity");
            return out.prefix;
        }
        auto& calls=out.rosterRows[rank];calls.managerIndex=manager;
        auto& place=p.rows5E0[3*rank];auto& name=p.rows5E0[3*rank+1];auto& time=p.rows5E0[3*rank+2];
        const float y=pageAdd(90.f,pageMul(float(rank),30.f));
        const auto color=row.vehicle58==0?p.base->grey170:p.base->yellow16C;
        p.position(place,pageAdd(110.f,offset),y);p.color(place,color);
        const char number[]{char('1'+rank),'.',0}; //Source3ACE08="%d."; native admitted count<=8.
        p.assign(place,number,2);p.justification(place,4); //Actual3C4758.
        d.object=3*rank;
        if(!text(place,calls.place,0x1c2db0))return out.prefix;
        auto& portrait=p.portraits480[manager];
        //1C2DF0 ADDA.S90,-3 then MADD.S row*30; retain those boundaries.
        p.position(portrait,pageAdd(116.f,offset),pageAdd(pageMul(float(rank),30.f),pageAdd(90.f,-3.f)));
        d.object=manager;
        if(!texture(portrait,calls.portrait,0x1c2e80))return out.prefix;
        p.position(name,pageAdd(148.f,offset),y);p.color(name,color);
        std::int32_t nameId=-1;
        if(!race.profile_->resultsRacerName3C(row.racer60,nameId)||
           !p.localized(name,static_cast<std::size_t>(nameId),d)){
            if(nameId<0)p.stop(d,Impl::S::source,0x1c3064,"Results real Profile3C racer name unavailable");
            return out.prefix;
        }
        p.justification(name,0); //Actual3C4738; source"%s" preserves the localized bytes.
        d.object=3*rank+1;
        if(!text(name,calls.name,0x1c323c))return out.prefix;
        p.color(time,color);
        const char* textBytes=nullptr;std::size_t textLength=0;std::array<char,9> formatted{};
        if(row.retirementCD4){
            const auto* english=p.sourceData->english();const auto* entry=english?english->at(466):nullptr;
            if(!entry){p.stop(d,Impl::S::source,0x1c33e8,"Results retirement localization466 absent");return out.prefix;}
            textBytes=entry->text.data();textLength=entry->text.size();
        }else{
            float seconds=row.timeCB8;
            if(!(row.flagsCD8&1u)&&!projectedTime(manager,seconds))return out.prefix;
            if(!time_stats_detail50LA::time189000(seconds,formatted)){
                p.stop(d,Impl::S::source,row.flagsCD8&1u?0x1c3514:0x1c3640,"Results source189000 finite-domain formatting");
                return out.prefix;
            }
            textBytes=formatted.data();textLength=8;
        }
        p.justification(time,1);p.position(time,pageAdd(500.f,offset),y); //Actual3C4740.
        if(!row.retirementCD4&&rank<3){
            auto& medal=(*p.medals5F0)[rank];
            const float medalY=rank?pageAdd(pageMul(float(rank),30.f),pageAdd(90.f,0.f)):90.f;
            p.position(medal,pageAdd(60.f,offset),medalY);
            constexpr std::uint32_t pc[]{0x1c37ec,0x1c389c,0x1c396c};
            d.object=0x5f0+0x60*rank;
            if(!texture(medal,calls.medal,pc[rank]))return out.prefix;
        }
        //Time assignment/submission is AFTER the optional medal callback.
        p.assign(time,textBytes,textLength);d.object=3*rank+2;
        if(!text(time,calls.time,0x1c3a44))return out.prefix;
        ++out.rosterRowCount;
    }
    if(!current()){p.stop(d,Impl::S::owner,0x1c3a68,"Results final publication/Profile changed");return out.prefix;}
    out.prefix=DisplayCpuPrefix::roster;
    return incomplete(0x1c3a74,"Results roster CPU page only; following pages and real renderer join absent");
}
}
