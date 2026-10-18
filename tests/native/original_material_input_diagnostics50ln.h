#pragma once
#include "../../src/game/original_pod_material_audio.h"
#include <ostream>
#include <cstring>
#include <cstdint>
#if defined(__SSE__) || defined(_M_X64) || defined(_M_IX86_FP)
#include <xmmintrin.h>
#endif

// Failure-only native diagnostics. No arithmetic, owner, RNG, input or dispatch
// mutation. Match the existing179070 preflight; do not widen its admission.
namespace original_material_input_diagnostics50LN {
inline std::uint32_t bits(float value) noexcept {
    std::uint32_t word=0;std::memcpy(&word,&value,sizeof word);return word;
}
inline bool scalarBits(float value) noexcept {
    const auto word=bits(value),exponent=word&0x7f800000u,mantissa=word&0x007fffffu;
    return exponent!=0x7f800000u&&(exponent!=0u||mantissa==0u);
}
inline const char* guardName(rr::game::OriginalPodMaterialInputGuard guard) noexcept {
    using G=rr::game::OriginalPodMaterialInputGuard;
    switch(guard){
    case G::none:return "none";case G::channelArgument:return "channel_argument";
    case G::vehicleClass:return "vehicle_class";case G::contactMaterial:return "contact_a9";
    case G::listenerCount:return "listener_count";case G::listenerPointer:return "listener_pointer";
    case G::soundMode:return "sound_mode";case G::rounding:return "rounding_mode";
    case G::positionScalar:return "position_scalar";case G::velocityScalar:return "velocity_scalar";
    case G::listenerPositionScalar:return "listener_position_scalar";
    case G::listenerForwardScalar:return "listener_forward_scalar";
    }return "unknown";
}
inline const char* bitClass(float value) noexcept {
    const auto word=bits(value),exponent=word&0x7f800000u,mantissa=word&0x007fffffu;
    return exponent==0x7f800000u?(mantissa?"nan":"infinite"):
        exponent==0u?(mantissa?"subnormal":"zero"):"normal";
}
inline void write(std::ostream& out,const rr::game::OriginalPodMaterialAudioInput& in,
    std::uint32_t expectedClass,const rr::game::OriginalAiAudioListener* listeners,
    std::size_t count,const rr::game::OriginalSoundModeFields50KG& mode,int rounding,
    const rr::game::OriginalPodMaterialAudioReceipt& receipt) {
    const auto prior=out.flags();
    struct Restore {std::ostream& out;std::ios_base::fmtflags flags;~Restore(){out.flags(flags);}} restore{out,prior};
    out<<std::dec<<" guard="<<guardName(receipt.inputGuard)
       <<" guard_row="<<receipt.inputListener<<" guard_axis="<<receipt.inputAxis
       <<" channel="<<in.channelArgument<<" class58="<<in.vehicle58<<" expected_class="<<expectedClass
       <<" contact_a9="<<in.contactMaterialA9<<" listener_count="<<count
       <<" listener_pointer="<<(listeners!=nullptr)<<" rounding="<<rounding<<" nearest="<<FE_TONEAREST
       <<" mode_valid="<<rr::game::validOriginalSoundModeFields50KG(mode)
       <<" mode_profile="<<static_cast<std::uint32_t>(mode.profile1101C)
       <<" mode_mono="<<mode.monoE38<<" mode_surround="<<mode.surroundE34
       <<" mode_output="<<mode.outputMode9900<<" mode_reverse="<<mode.reverseAngle9918
       <<" guard_channel_ok="<<(in.channelArgument>=1&&in.channelArgument<=3)
       <<" guard_class_ok="<<(in.vehicle58==expectedClass)
       <<" guard_contact_ok="<<(in.contactMaterialA9>=-1&&in.contactMaterialA9<=255)
       <<" guard_listener_count_ok="<<(count<=4)<<" guard_listener_pointer_ok="<<(!count||listeners)
       <<" guard_rounding_ok="<<(rounding==FE_TONEAREST)
       <<" fenv_exceptions="<<std::fetestexcept(FE_ALL_EXCEPT);
#if defined(__SSE__) || defined(_M_X64) || defined(_M_IX86_FP)
    out<<" mxcsr_available=1 mxcsr="<<std::hex<<_mm_getcsr()<<std::dec;
#else
    out<<" mxcsr_available=0";
#endif
    const auto vector=[&](const char* name,const float* values,unsigned n){
        out<<' '<<name<<"_bits="<<std::hex;
        for(unsigned i=0;i<n;++i){if(i)out<<',';out<<bits(values[i]);}
        out<<std::dec<<' '<<name<<"_bit_class=";
        for(unsigned i=0;i<n;++i){if(i)out<<',';out<<bitClass(values[i]);}
        out<<' '<<name<<"_bit_domain_ok=";
        for(unsigned i=0;i<n;++i){if(i)out<<',';out<<scalarBits(values[i]);}
    };
    vector("position",in.position.v,4);vector("velocity",in.velocity.v,4);
    out<<" speed_ratio_bits="<<std::hex<<bits(in.speedRatio70)<<std::dec;
    // Bounded even when the guard rejects the listener count. Never dereference
    // a missing listener; the actual caller supplies its sole real listener.
    if(listeners&&count<=4)for(std::size_t i=0;i<count;++i){
        out<<" listener_row="<<i;
        vector("listener_position",listeners[i].position,4);
        vector("listener_forward",listeners[i].forward,4);
    }
}
}
