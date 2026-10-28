#include "original_results104_storage50la.h"
#include "original_ui_text50kg.h"
#include "original_ui_text_position50kg.h"
#include "../../tests/native/original_position_stats_adapter50la.h"
#include "../../tests/native/original_race_constructor50la.h"
#include <bit>
#include <cfenv>
#include <cmath>
#include <cstring>

namespace rr::game {
namespace {
float keyboardAdd(float a,float b)noexcept {volatile float result=a+b;return result;}
float keyboardMul(float a,float b)noexcept {volatile float result=a*b;return result;}
float keyboardDiv(float a,float b)noexcept {volatile float result=a/b;return result;}
}
bool OriginalResults10450LA::Impl::textWidthCpu2E16F0(
    const Text& t,std::int32_t& width,D& d)noexcept {
    if(!t.font4C||!t.string8||t.length5C>t.capacity5B||t.string8[t.length5C]||
       !std::isfinite(t.field40)||t.field40<=0.f)
        return stop(d,S::source,0x2e16f0,"Results retained name text metric domain");
    const auto* atlas=t.font4C->texture281F90();
    if(!atlas||!atlas->sourceWidth14()||!atlas->sourceHeight18()||
       *atlas->sourceWidth14()<=0||*atlas->sourceHeight18()<=0)
        return stop(d,S::font,0x2e16f0,"Results actual name font dimensions absent");
    OriginalUiTextInput50KG input{&t.font4C->metrics(),nullptr,
        reinterpret_cast<const std::uint8_t*>(t.string8.get()),t.length5C,0.f,0.f};
    OriginalUiTextLayout50KG run;OriginalUiTextDiagnostic50KG glyph;
    if(!layoutOriginalUiTextDimensions50LA(input,
        static_cast<std::uint32_t>(*atlas->sourceWidth14()),
        static_cast<std::uint32_t>(*atlas->sourceHeight18()),run,glyph)||
       !roundOriginalUiTextMetric50KG(run.widthBeforeScaleAndRounding,t.field40,width))
        return stop(d,S::source,0x2e16f0,"Results original name width/rounding domain");
    return true;
}

OriginalResults10450LA::DisplayCpuPrefix OriginalResults10450LA::prepareKeyboardPageCpuPrefix(
    const OriginalRaceConstructor50LA& race,
    const ::original_position_stats_adapter50LA::Owner& observer,
    OriginalResults104Diagnostic50LA& d)noexcept {
    d={};
    if(race.resultsFeedbackThread_!=std::this_thread::get_id()){
        d.status=Impl::S::owner;d.sourcePC=0x1c4644;return DisplayCpuPrefix::none;
    }
    const auto prefix=prepareRecordSummaryCpuPrefix(race,observer,d);
    if(!impl_||!impl_->displayCpu||prefix!=DisplayCpuPrefix::recordSummary||
       d.status!=Impl::S::incomplete||impl_->displayCpu->nextSourcePC!=0x1c4644)return prefix;
    auto& p=*impl_;auto& out=*p.displayCpu;auto& calls=out.keyboard;
    const auto stop=[&](Impl::S status,std::uint32_t pc,const char* operation){
        out.nextSourcePC=pc;p.stop(d,status,pc,operation);return out.prefix;
    };
    const auto samePublication=[&](){return p.context&&p.context->live()&&
        observer.resultsRowsCurrent(p.application.get(),p.domain->materials382B38().get(),
            p.component.sessionGeneration,out.records.publication);};
    if(!samePublication())return stop(Impl::S::owner,0x1c4644,"Results record/keyboard current publication absent");
    // Source initialization is unresolved. The update callback below does
    // NOT justify reading a default zero: it too reads394/398 before writing.
    if(!p.pulse394)return stop(Impl::S::incomplete,0x1c4644,"Results pulse394 initialization remains unknown");
    if(std::fegetround()!=FE_TONEAREST)
        return stop(Impl::S::source,0x1c465c,"Results keyboard requires nearest native arithmetic");
#if defined(__FAST_MATH__) || defined(_M_FP_FAST) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
    return stop(Impl::S::source,0x1c465c,"Results keyboard requires strict native floating point");
#endif
    const float pulse=std::bit_cast<float>(*p.pulse394);
    if(!std::isfinite(pulse))return stop(Impl::S::source,0x1c4644,"Results finite source pulse domain");
    const float ratio=keyboardDiv(pulse,std::bit_cast<float>(std::uint32_t{0x3e99999a}));
    const float amount=keyboardMul(std::bit_cast<float>(std::uint32_t{0x3e23d70a}),ratio);
    const float scale=keyboardAdd(1.f,amount); //DIV.S, MUL.S, ADD.S, never contracted.
    if(!std::isfinite(scale)||scale<=0.f)
        return stop(Impl::S::source,0x1c4678,"Results selected key pulse scale domain");
    if(!p.nameEntry||!p.nameEntry->fieldABC||!p.keysA94||p.keysConstructed!=42||
       *p.nameEntry->fieldABC<1u||*p.nameEntry->fieldABC>42u||
       !p.keysA94[*p.nameEntry->fieldABC-1u])
        return stop(Impl::S::owner,0x1c4674,"Results actual selected key owner/index absent");
    p.scale(*p.keysA94[*p.nameEntry->fieldABC-1u],scale);calls.selectedKeyScaled=true;
    const float offset=out.transition.offsetF31;
    const auto draw=[&](Impl::Text& text,Impl::DisplayCpuPreparation::TextCall& call,std::uint32_t pc){
        out.nextSourcePC=pc;if(!p.projectTextCpu2E0170(text,pc,d))return false;
        call={&text,pc};return true;
    };
    // Source loop always visits42 indices. Ordinary columns are index%13,
    // row=index/13; special keys40/41/42 use columns2/6/10 respectively.
    // SPACE and DONE are omitted for an empty name; BACKSPACE is not omitted.
    for(std::size_t index=0;index<42;++index){
        const auto sourceIndex=index+1;std::size_t column=index%13;
        std::uint32_t pc=0x1c4a9c;
        if(sourceIndex==40u||sourceIndex==42u){
            const auto namePC=sourceIndex==40u?0x1c4710u:0x1c48ecu;
            if(!p.editA9C)return stop(Impl::S::incomplete,namePC,"Results actual edited nameA9C absent");
            if(!std::strlen(p.editA9C->c_str()))continue;
        }
        if(sourceIndex==40u){column=2;pc=0x1c47ec;}
        else if(sourceIndex==41u){column=6;pc=0x1c48d4;}
        else if(sourceIndex==42u){column=10;pc=0x1c49c4;}
        auto* key=p.keysA94[index].get();
        if(!key)return stop(Impl::S::owner,pc,"Results retained keyboard Text absent");
        const float x=keyboardAdd(offset,keyboardAdd(123.f,keyboardMul(33.f,float(column))));
        const float y=keyboardAdd(210.f,keyboardMul(30.f,float(index/13)));
        p.position(*key,x,y);
        if(!draw(*key,calls.keys[calls.keyCount],pc))return out.prefix;
        ++calls.keyCount;
    }
    if(!p.editA9C||!p.nameA90||!p.nameA90->string8)
        return stop(Impl::S::owner,0x1c4abc,"Results retained editable name Text absent");
    // Original strlen/strcmp/capacity-clipped copy. No input action or change
    // to the persistent Profile name occurs at this display occurrence.
    p.assign(*p.nameA90,p.editA9C->c_str(),std::strlen(p.editA9C->c_str()));
    p.position(*p.nameA90,keyboardAdd(320.f,offset),180.f);
    if(!draw(*p.nameA90,calls.name,0x1c4bc8))return out.prefix;
    if(!p.nameEntry->fieldAB4)
        return stop(Impl::S::incomplete,0x1c4bd0,"Results cursor blinkAB4 has no reached producer");
    if(*p.nameEntry->fieldAB4&&std::strlen(p.nameA90->string8.get())<10u){
        std::int32_t width=0;out.nextSourcePC=0x1c4bf4;
        if(!p.textWidthCpu2E16F0(*p.nameA90,width,d))return out.prefix;
        if(!p.cursorA98)return stop(Impl::S::owner,0x1c4c2c,"Results actual cursor Text absent");
        // Source signed width/2 truncates toward zero, then reads raw position
        //10/14 (not projected20/24).1C4C38 adds F31 AGAIN: preserve this exact
        // double transition offset rather than visually correcting it.
        const float end=keyboardAdd(p.nameA90->x10,float(width/2));
        p.position(*p.cursorA98,keyboardAdd(offset,end),p.nameA90->y14);
        if(!draw(*p.cursorA98,calls.cursor,0x1c4c98))return out.prefix;
    }
    if(!samePublication())return stop(Impl::S::owner,0x1c4ca0,"Results publication changed during keyboard preparation");
    out.prefix=DisplayCpuPrefix::keyboard;
    return stop(Impl::S::incomplete,0x1c4ca0,"Results keyboard/name/cursor body only; later pages and renderer absent");
}
}
