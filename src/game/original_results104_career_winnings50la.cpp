#include "original_results104_storage50la.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include <bit>
#include <charconv>
#include <cfenv>
#include <cmath>
#include <cstring>
#include <string>
#include <string_view>

namespace rr::game {
namespace {
float winningsAdd(float a,float b)noexcept {volatile float r=a+b;return r;}
float winningsMul(float a,float b)noexcept {volatile float r=a*b;return r;}
float winningsDiv(float a,float b)noexcept {volatile float r=a/b;return r;}
bool winningsRounded(std::int32_t amount,std::int32_t percent,std::int32_t& out)noexcept {
    //1879C0..A00 /187A80..ABC: integer->S, percent/100, amount*ratio,
    //.5+product, source CVT.W.S. Match the existing native Tournament
    //truncation adaptation; reject unsupported domain, never UB or fusion.
    const float ratio=winningsDiv(float(percent),100.f);
    const float product=winningsMul(float(amount),ratio);
    const float rounded=winningsAdd(.5f,product);
    if(!std::isfinite(ratio)||!std::isfinite(product)||!std::isfinite(rounded)||
       rounded>=2147483648.f||rounded< -2147483648.f)return false;
    out=static_cast<std::int32_t>(rounded);return true;
}
bool signedDecimal(std::int32_t value,std::string& out) {
    std::array<char,12> bytes{};const auto r=std::to_chars(bytes.data(),bytes.data()+bytes.size(),value);
    if(r.ec!=std::errc{})return false;out.assign(bytes.data(),r.ptr);return true;
}
}
OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::prepareCareerWinningsCpuPrefix(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c4ca0;return DisplayCpuPrefix::none;
    }
    const auto prefix=prepareKeyboardPageCpuPrefix(race,observer,d);
    // Source-skipped/cheat record page also lands1C4CA0 without keyboard.
    if(!impl_||!impl_->displayCpu||
       (prefix!=DisplayCpuPrefix::keyboard&&prefix!=DisplayCpuPrefix::recordSummary)||
       d.status!=Impl::S::incomplete||impl_->displayCpu->nextSourcePC!=0x1c4ca0)return prefix;
    auto& p=*impl_;auto& out=*p.displayCpu;auto& calls=out.career;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;p.stop(d,status,pc,operation);return out.prefix;
    };
    const float offset=out.transition.offsetF21;
    if(!std::isfinite(offset))return stop(Impl::S::source,0x1c4ca0,"Results finite career-page transition domain");
    //C.LE F21,640 then C.LT F21,-640. Skipped page does not touch Profile,
    //manager, text, medals or separators and reaches the slot74 return.
    if(!(offset<=640.f)||offset< -640.f){
        out.prefix=DisplayCpuPrefix::careerWinnings;
        return stop(Impl::S::incomplete,0x1c8a74,"Results career page source-skipped; slot48/base/renderer unjoined");
    }
    if(std::fegetround()!=FE_TONEAREST)
        return stop(Impl::S::source,0x1879c0,"Results winnings requires nearest native arithmetic");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return stop(Impl::S::source,0x1879c0,"Results winnings requires strict native floating point");
#endif
    using Observation=::original_position_stats_adapter50LA::Owner;
    using Profile=rr::platform::OriginalUiRiderProfileOwner50KG;
    const auto operation=p.loadedRosterOperation.lock();
    const auto before=race.profile_->stamp();const auto generation=p.component.sessionGeneration;
    const auto current=[&](){const auto now=race.profile_->stamp();
        return careerRosterCurrentNative(operation,race,p.application,p.domain,generation,observer)&&
            now.generation==before.generation&&now.mode34==before.mode34&&p.context->live();
    };
    // A private read-only snapshot of THIS Profile's selected1125F metadata.
    // Eager metadata copy is native preparation only: no original call/award
    // receipt follows. Reached manager reads below retain source order.
    Profile::ResultsCareerPageFields fields;
    if(!current()||!race.profile_->resultsCareerPageFields1C4CD8(fields)||
       fields.stamp.generation!=before.generation||fields.stamp.mode34!=before.mode34)
        return stop(Impl::S::owner,0x1c4cd8,"Results actual selected Profile course/diagonal metadata absent");
    calls.courseOrder=fields.order;calls.courseClass=fields.order+1u<5u?1u:(fields.order+1u<9u?2u:3u);
    calls.profileGeneration=before.generation;calls.profileMode34=before.mode34;
    Observation::ResultsRecordIdentity row;Observation::Diagnostic observed;
    if(!observer.captureResultsRecordIdentity(p.application.get(),p.domain->materials382B38().get(),
            generation,0,row,observed)||row.bank!=observer.resultsBankIdentity()||row.vehicle58!=0||
       !careerRacerCurrentNative(operation,row.racer60)||!current())
        return stop(Impl::S::owner,0x1c4d18,"Results same loaded first-human Bank identity absent");
    calls.publication=row.publication;
    const auto samePublication=[&](){return current()&&observer.resultsRecordIdentityCurrent(row);};
    if(!observer.readEndPlaceCCC(row,calls.placeCCC,observed))
        return stop(Impl::S::owner,0x1c4d20,"Results actual first-human CCC read absent");
    calls.priorMedal=fields.priorMedal1938; //1C4D64, diagonal career[C].pod[C].
    std::uint32_t count=0;
    if(!observer.readResultsObservedTimingCD0(row,count,observed))
        return stop(Impl::S::owner,0x1c4d70,"Results actual first-human observed CD0 read absent");
    calls.observedCD0=std::bit_cast<std::int32_t>(count);
    //187900 re-reads actual manager0 CCC; don't replace it with first read.
    std::int32_t prizePlace=0;
    if(!samePublication()||!observer.readEndPlaceCCC(row,prizePlace,observed))
        return stop(Impl::S::owner,0x187938,"Results current187900 CCC read absent");
    if(prizePlace!=8&&(fields.priorMedal1938==0||prizePlace<fields.priorMedal1938)){
        if(prizePlace<1||prizePlace>8)
            return stop(Impl::S::source,0x1879ac,"Results place percentage source index outside retained1..8");
        if(!winningsRounded(fields.remaining1946,fields.placePercent102D0[prizePlace-1],calls.prize))
            return stop(Impl::S::source,0x1879fc,"Results native187900 arithmetic outside bounded domain");
    }
    //187A20 also re-reads CD0. This is a displayed preview of that actual
    //timing lane, never proof of its earned-destruction producer.
    if(!samePublication()||!observer.readResultsObservedTimingCD0(row,count,observed))
        return stop(Impl::S::owner,0x187a60,"Results current187A20 CD0 read absent");
    const auto bonusCount=std::bit_cast<std::int32_t>(count);
    if(bonusCount>0){
        if(bonusCount>7)return stop(Impl::S::source,0x187a94,"Results bonus percentage index beyond actual seven-entry table");
        if(!winningsRounded(calls.prize,fields.killPercent100CC[bonusCount],calls.bonus))
            return stop(Impl::S::source,0x187ab8,"Results native187A20 arithmetic outside bounded domain");
    }
    calls.total=std::bit_cast<std::int32_t>(std::uint32_t(calls.prize)+std::uint32_t(calls.bonus)); //ADDU1C4DA8.
    if(!samePublication()||!observer.readEndRetirementCD4(row,calls.retirementCD4,observed))
        return stop(Impl::S::owner,0x1c4dc8,"Results actual first-human CD4 read absent");
    calls.reached=true;
    if(calls.retirementCD4){
        out.prefix=DisplayCpuPrefix::careerWinnings;
        return stop(Impl::S::incomplete,0x1c6f98,"Results retired branch skips winnings; following messages/renderer unjoined");
    }
    try {
        const auto localized=[&](std::size_t id,std::string& value,std::uint32_t pc){
            out.nextSourcePC=pc;const auto* english=p.sourceData?p.sourceData->english():nullptr;
            const auto* entry=english?english->at(id):nullptr;
            if(!entry)return p.stop(d,Impl::S::source,pc,"Results actual retained localization entry absent");
            value=entry->text;return true;
        };
        const auto text=[&](Impl::Text& value,Impl::DisplayCpuPreparation::TextCall& call,
            std::string_view bytes,float x,float y,bool positionFirst,std::uint32_t pc){
            out.nextSourcePC=pc;
            if(!value.string8)return p.stop(d,Impl::S::owner,pc,"Results retained Text storage absent");
            if(positionFirst)p.position(value,x,y);
            p.assign(value,bytes.data(),bytes.size());
            if(!positionFirst)p.position(value,x,y);
            if(!p.projectTextCpu2E0170(value,pc,d))return false;
            call={&value,pc};return true;
        };
        if(!p.text1150||!p.tableAF0||!p.medals5F0||!p.separators1030)
            return stop(Impl::S::owner,0x1c4ed8,"Results own winnings ScreenObjects absent");
        std::string value,course;
        const auto place=calls.placeCCC;
        const std::size_t title=place>=1&&place<=7?std::size_t(444+place):452u;
        if(!localized(title,value,0x1c4ee4)||
           !localized(static_cast<std::size_t>(fields.title1127C),course,0x187390))return out.prefix;
        value+=' ';value+=course; //Exact3ACAE0 "%s %s", then source capacity40.
        if(!text((*p.text1150)[1],calls.heading,value,winningsAdd(320.f,offset),100.f,true,0x1c55ac))return out.prefix;
        constexpr std::array<std::string_view,8> places{{"1ST PLACE","2ND PLACE","3RD PLACE","4TH PLACE",
            "5TH PLACE","6TH PLACE","7TH PLACE","8TH PLACE"}}; //Original3ACE18..88.
        const auto placeText=places[place>=1&&place<=7?std::size_t(place-1):7u];
        if(place>=1&&place<=3){
            auto& medal=(*p.medals5F0)[std::size_t(place-1)];
            //Original1C55D0..5794 uses40,130 WITHOUT F21. Do not visually
            //"correct" this medal into the neighboring moving text column.
            p.position(medal,40.f,130.f);
            constexpr std::uint32_t pcs[]{0x1c564c,0x1c56f0,0x1c5794};
            const auto pc=pcs[place-1];out.nextSourcePC=pc;
            if(!p.projectTextureCpu2DF694(medal,pc,d))return out.prefix;
            calls.medal={&medal,pc};
        }
        const auto table=[&](std::size_t i,std::string_view bytes,float x,float y,std::uint32_t pc){
            return text((*p.tableAF0)[i],calls.table[i],bytes,x,y,false,pc);
        };
        //Source first adds40+30, THEN F21; all other columns add base+F21.
        if(!table(0,placeText,(place<4?winningsAdd(offset,winningsAdd(40.f,30.f)):winningsAdd(40.f,offset)),130.f,0x1c59b4))return out.prefix;
        if(!signedDecimal(calls.prize,value))return stop(Impl::S::source,0x1c59c8,"Results signed prize formatting");
        if(!table(1,value,winningsAdd(310.f,offset),130.f,0x1c5ac8))return out.prefix;
        if(!localized(432,value,0x1c5b58)||!table(2,value,winningsAdd(420.f,offset),130.f,0x1c5c5c))return out.prefix;
        const auto separator=[&](std::size_t i,float y,std::uint32_t pc){
            auto& texture=(*p.separators1030)[i];out.nextSourcePC=pc;
            p.position(texture,winningsAdd(40.f,offset),y);
            p.dimension(texture,texture.width34,380.f);p.dimension(texture,texture.height38,3.f);
            if(!p.projectTextureCpu2DF694(texture,pc,d))return false;
            calls.separators[i]={&texture,pc};return true;
        };
        if(calls.bonus>0){
            if(!signedDecimal(calls.observedCD0,value))return stop(Impl::S::source,0x1c5cb0,"Results signed CD0 formatting");
            value+=calls.observedCD0<fields.par100E8?" KO's (BELOW PAR)":
                (calls.observedCD0==fields.par100E8?" KO's (PAR)":" KO's (ABOVE PAR)");
            if(!table(3,value,winningsAdd(40.f,offset),160.f,0x1c5df0)||
               !table(4,"",winningsAdd(310.f,offset),160.f,0x1c5efc)||
               !table(5,"",winningsAdd(420.f,offset),160.f,0x1c6008))return out.prefix;
            if(calls.observedCD0<1||calls.observedCD0>7)
                return stop(Impl::S::source,0x1c6044,"Results current display percentage index outside seven entries");
            if(!signedDecimal(fields.killPercent100CC[calls.observedCD0],value))
                return stop(Impl::S::source,0x1c6048,"Results signed percentage formatting");
            value='+'+value+"% PAYOFF:";
            if(!table(6,value,winningsAdd(40.f,offset),180.f,0x1c6150))return out.prefix;
            if(!signedDecimal(calls.bonus,value))return stop(Impl::S::source,0x1c6164,"Results signed bonus formatting");
            if(!table(7,value,winningsAdd(310.f,offset),180.f,0x1c626c)||
               !localized(432,value,0x1c62fc)||!table(8,value,winningsAdd(420.f,offset),180.f,0x1c6408))return out.prefix;
            if(!table(9,"TOTAL",winningsAdd(40.f,offset),210.f,0x1c6520))return out.prefix;
            if(!signedDecimal(calls.total,value))return stop(Impl::S::source,0x1c6534,"Results signed total formatting");
            if(!table(10,value,winningsAdd(310.f,offset),210.f,0x1c663c)||
               !localized(432,value,0x1c66cc)||!table(11,value,winningsAdd(420.f,offset),210.f,0x1c67d8)||
               !separator(0,153.f,0x1c692c)||!separator(1,203.f,0x1c6a80))return out.prefix;
        }else{
            if(!table(9,"TOTAL",winningsAdd(40.f,offset),160.f,0x1c6b94))return out.prefix;
            if(!signedDecimal(calls.total,value))return stop(Impl::S::source,0x1c6ba8,"Results signed total formatting");
            if(!table(10,value,winningsAdd(310.f,offset),160.f,0x1c6ca8)||
               !localized(432,value,0x1c6d38)||!table(11,value,winningsAdd(420.f,offset),160.f,0x1c6e3c)||
               !separator(0,153.f,0x1c6f90))return out.prefix;
        }
        if(!samePublication())return stop(Impl::S::owner,0x1c6f98,"Results Profile/Bank changed after winnings CPU effects");
        out.prefix=DisplayCpuPrefix::careerWinnings;
        return stop(Impl::S::incomplete,0x1c6f98,"Results winnings CPU page only; career messages/statistics and renderer remain unjoined");
    }catch(...){return stop(Impl::S::allocation,out.nextSourcePC,"Results winnings formatting allocation; reached CPU effects retained");}
}
}
