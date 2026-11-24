#pragma once
#include "engine_damage_audio50ia.h"
#include "gameplay_audio50he.h"
#include "original_ai_engine_emitter.h"

namespace rr::game {
// Concrete lifetime for the represented human19EBA0 reset fields. This owns
// the SAME damage, warning and boost state consumed by the native audio
// subscriber. It does not claim the unrepresented spatial/child scheduler.
// The existing identity record names native session objects, not an AI class.
class OriginalHumanEngineResetOwner50LB final {
    OriginalAiEmitterIdentity identity_{};
    bool bound_=false;
public:
    EngineDamageState50IA damage{};
    PodWarningState50HE warning{};
    PodBoostAudioState50HE boost{};
    //19EBD0,19ECA0,19ECA4; these are source-owned fields, never reset receipts.
    std::uint32_t field4AC=0,field4A8=0;
    std::uint8_t field4A5=3;
    OriginalHumanEngineResetOwner50LB() noexcept=default;
    OriginalHumanEngineResetOwner50LB(const OriginalHumanEngineResetOwner50LB&)=delete;
    OriginalHumanEngineResetOwner50LB& operator=(const OriginalHumanEngineResetOwner50LB&)=delete;
    bool bind(const OriginalAiEmitterIdentity& id) noexcept {
        if(bound_||!id.session||!id.racer||!id.mainBody||!id.emitterD40||
           id.managerOrdinal>=2||!id.pod60)return false;
        identity_=id;bound_=true;return true;
    }
    bool bound()const noexcept{return bound_;}
    const OriginalAiEmitterIdentity& identity()const noexcept{return identity_;}
    std::uint8_t& flags22C() noexcept{return boost.flags22C;}
    // Complete scalar1A0240 on its actual canonical owners. Warning4A4 bits
    //2..5 and22C boost bit1 survive. No voices, samples or mixer clocks reset.
    bool reset1A0240() noexcept {
        if(!bound_)return false;
        damage.cooldown474=0.f;field4AC=0;
        damage.flags4A4=static_cast<std::uint8_t>(damage.flags4A4&~3u);
        damage.scale484=1.f;damage.target490=1.f;
        damage.duration494=0.f;damage.remaining498=0.f;
        damage.velocity49C=0.f;damage.acceleration4A0=0.f;
        field4A5=3;field4A8=0;
        boost.flags22C=static_cast<std::uint8_t>(boost.flags22C&~4u);
        return true;
    }
};
}
