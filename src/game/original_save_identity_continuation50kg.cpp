#include "original_save_identity_continuation50kg.h"
#include <cfenv>
#include <cmath>
#include <limits>

namespace rr::game {
static_assert(sizeof(float)==4&&std::numeric_limits<float>::is_iec559&&
    std::numeric_limits<float>::digits==24,"original identity binary32 arithmetic");
OriginalSaveIdentityContinuation50KG::OriginalSaveIdentityContinuation50KG(Purpose purpose) noexcept
    :purpose_(purpose) {}
OriginalSaveIdentityContinuation50KG::Result OriginalSaveIdentityContinuation50KG::issue(
    Call call,std::uint32_t pc,Phase next,std::uint32_t argument) noexcept {
    pending_={call,++serial_,pc,argument};phase_=next;
    return {Status::pending,pending_,pc,0};
}
OriginalSaveIdentityContinuation50KG::Result OriginalSaveIdentityContinuation50KG::terminal(
    Status status,std::uint32_t pc) noexcept {
    phase_=Phase::failed;failure_=status;failurePC_=pc;return {status,{},pc,0};
}
bool OriginalSaveIdentityContinuation50KG::acknowledge(const Request& request,std::uint32_t value) noexcept {
    if(pending_.call==Call::none||request.call!=pending_.call||request.sequence!=pending_.sequence||
       request.sourcePC!=pending_.sourcePC||request.argument!=pending_.argument)return false;
    lastReturn_=value;pending_={};return true;
}
OriginalSaveIdentityContinuation50KG::Result OriginalSaveIdentityContinuation50KG::advance(
    SharedRandomOwner50ID& shared) noexcept {
    const bool save=purpose_==Purpose::dirtySave1869B0;
    if(purpose_!=Purpose::dirtySave1869B0&&purpose_!=Purpose::profileConstructor17DF80)
        return {Status::invalidState,{},0,0};
    if(shared_&&shared_!=&shared)return {Status::invalidState,{},0,0};
    if(!shared_)shared_=&shared;
    if(pending_.call!=Call::none)return {Status::pending,pending_,pending_.sourcePC,0};
    switch(phase_){
    case Phase::entry:
        if(!shared.seed.lazy80A8){shared.seed.seed3D6B30=0;shared.seed.lazy80A8=true;}
        if(shared.seed.seed3D6B30==0)
            return issue(Call::clock101A50,save?0x186a74u:0x17e1acu,Phase::afterClock);
        return issue(Call::draw308030,save?0x186a98u:0x17e1d0u,Phase::afterDraw);
    case Phase::afterClock:
        shared.seed.seed3D6B30=lastReturn_;
        return issue(Call::seed308020,save?0x186a88u:0x17e1c0u,Phase::afterSeed,shared.seed.seed3D6B30);
    case Phase::afterSeed:
        return issue(Call::draw308030,save?0x186a98u:0x17e1d0u,Phase::afterDraw);
    case Phase::afterDraw:{
        if(std::fegetround()!=FE_TONEAREST||lastReturn_>0x7fffffffu)
            return terminal(Status::unsupportedNumeric,save?0x186aa8u:0x17e1dcu);
        const volatile float draw=static_cast<float>(lastReturn_);
        const volatile float fraction=0x1p-31f*draw;
        const volatile float scaled=65536.f*fraction;
        if(!std::isfinite(scaled)||scaled<0.f||scaled>65536.f)
            return terminal(Status::unsupportedNumeric,save?0x186ac0u:0x17e1f8u);
        // Source EE CVT.W.S truncation follows existing49K/50ID treatment.
        // Rounding may yield65536 then identity65537: do NOT clamp/modulo.
        identity_=static_cast<std::uint32_t>(scaled)+1u;
        if(!shared.range.lazy80AC){shared.range.rangeCheck3D6B38=1;shared.range.lazy80AC=true;}
        if(shared.range.rangeCheck3D6B38&&(identity_<1||identity_>65536))
            return terminal(Status::sourceRangeAssertion,save?0x186b1cu:0x17e254u);
        phase_=Phase::done;
        return {Status::complete,{},save?0x186b50u:0x17e298u,identity_};
    }
    case Phase::done:return {Status::complete,{},save?0x186b50u:0x17e298u,identity_};
    case Phase::failed:return {failure_,{},failurePC_,0};
    }
    return {Status::invalidState,{},0,0};
}
}
