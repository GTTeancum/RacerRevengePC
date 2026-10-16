#pragma once
#include <array>
#include <cfenv>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
static_assert(sizeof(float)==4 && std::numeric_limits<float>::is_iec559 &&
              std::numeric_limits<float>::digits==24,
              "Controller feedback requires binary32");

// Original237C10 constructor establishes capability subfield2, which does not
// pass2388B0's ==3 gate. Init237D00 zeros both durations, counters and packet.
// The platform owner sets capable from actual device capability. It is distinct
// from current connection/read success; an absent device preserves this state.
struct ControllerFeedbackState50KZ {
    bool capable=false;
    float duration6CC=0.0f,duration6D0=0.0f;
    std::int32_t stopWrites6D8=0,stopWrites6DC=0;
    std::array<std::uint8_t,6> actuator714{};
};
struct ControllerFeedbackCommand50KZ {
    std::uint32_t sourcePC=0;
    std::array<std::uint8_t,6> actuator714{};
};
struct ControllerFeedbackCommands50KZ {
    ControllerFeedbackCommand50KZ values[4]{};
    std::size_t count=0;
};
namespace controller_feedback_detail50KZ {
inline bool ordinary(float value) noexcept {
    std::uint32_t bits;
    std::memcpy(&bits,&value,sizeof bits);
    const auto exponent=bits&0x7f800000u;
    return exponent!=0x7f800000u && (exponent!=0 || !(bits&0x007fffffu));
}
}

// Original238870 (3D1D80+40). No readiness/capability gate or hardware call.
// Channel0 selects6CC,1 selects6D0,2 selects both; other channels do nothing.
// The original return is always0 and is discarded; this function is infallible.
inline void clearControllerFeedback50KZ(ControllerFeedbackState50KZ& state,
                                      std::int32_t channel) noexcept {
    if(channel==0 || channel==2)state.duration6CC=0.0f;
    if(channel==1 || channel==2)state.duration6D0=0.0f;
}

// Original2388B0 (3D1D80+44). The original argument2 is ignored and source
// return is always0. Native true means an accepted operation, including an
// incapable/unsupported-channel no-op. False means unsupported consumed numeric
// input; state is unchanged. No rounding mode is needed for these comparisons.
// This replaces selected durations, clamped to[0,5], without changing counters.
inline bool requestControllerFeedback50KZ(ControllerFeedbackState50KZ& state,
    std::int32_t channel,std::uint32_t /*argument2*/,float duration) noexcept {
    if(!state.capable)return true;
    if(!controller_feedback_detail50KZ::ordinary(duration))return false;
    if(!(duration<=5.0f))duration=5.0f;
    if(duration<0.0f)duration=0.0f;
    if(channel==0 || channel==2)state.duration6CC=duration;
    if(channel==1 || channel==2)state.duration6D0=duration;
    return true;
}

// Original2381D0..238308, after device237DF0 has produced its button transitions.
// physicalReady represents actual stable/connected source-state6 eligibility;
// readSucceeded includes the successful accepted packet (packet[0]==0). False
// for either preserves state, returns true and emits an empty command list.
// Capability is not checked here: source service reads readiness, not6B4.
//
// Ordinary finite-normal/signed-zero consumed operands/results and FE_TONEAREST
// for subtraction only. Native arithmetic failure leaves state AND out unchanged.
// Signed counters preserve the source <=0/>0 tests; construction and this owner
// produce0..2, and decrement only positive values (no signed overflow).
//
// On success commit scalar state before delivering the ordered commands to real
// hardware. Original3139A0 return values are ignored: transport failure must not
// roll back state or replay the service. Each command snapshots all six source
// bytes714..719, so successive start/stop calls can carry different other motors.
inline bool serviceControllerFeedback50KZ(ControllerFeedbackState50KZ& state,
    bool physicalReady,bool readSucceeded,float elapsed,
    ControllerFeedbackCommands50KZ& out) noexcept {
    ControllerFeedbackCommands50KZ commands{};
    if(!physicalReady || !readSucceeded){out=commands;return true;}
    ControllerFeedbackState50KZ next=state;
    const auto send=[&](std::uint32_t sourcePC) noexcept {
        commands.values[commands.count++]={sourcePC,next.actuator714};
    };
    const auto subtract=[&](float& duration) noexcept {
        if(std::fegetround()!=FE_TONEAREST ||
           !controller_feedback_detail50KZ::ordinary(elapsed))return false;
        volatile float result=duration-elapsed;
        if(!controller_feedback_detail50KZ::ordinary(result))return false;
        duration=result;
        return true;
    };
    if(!controller_feedback_detail50KZ::ordinary(next.duration6CC))return false;
    if(!(next.duration6CC<=0.0f) && next.stopWrites6D8<=0) {
        next.actuator714[1]=200;
        send(0x238218u);
        next.stopWrites6D8=2;
    }
    if(next.stopWrites6D8>0) {
        if(!subtract(next.duration6CC))return false;
        if(next.duration6CC<=0.0f) {
            next.actuator714[1]=0;
            send(0x238260u);
            --next.stopWrites6D8;
        }
    }
    if(!controller_feedback_detail50KZ::ordinary(next.duration6D0))return false;
    if(!(next.duration6D0<=0.0f) && next.stopWrites6DC<=0) {
        next.actuator714[0]=1;
        send(0x2382ACu);
        next.stopWrites6DC=2;
    }
    if(next.stopWrites6DC>0) {
        if(!subtract(next.duration6D0))return false;
        if(next.duration6D0<=0.0f) {
            next.actuator714[0]=0;
            send(0x2382F4u);
            --next.stopWrites6DC;
        }
    }
    state=next;
    out=commands;
    return true;
}
} // namespace rr::game
