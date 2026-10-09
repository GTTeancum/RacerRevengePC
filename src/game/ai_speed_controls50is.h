#pragma once
#include "ai_speed_braking50ir.h"
#include "authored_parameters49n.h"
#include "vehicle_frame49w.h"
#include <cstring>

namespace rr::game {
// Source-only staged adapter. No live49W admission is widened. The current
//49W frame owner rejects vehicle58=1 and cannot execute this AI route yet.
struct AiColdControls50IS {
    AuthoredBinding49N authored;
    CoupledBodyParameters49M bodies;
    bool bound=false;
};
// Cold ONLY, before this racer runs. Caller supplies its original PHY and real
// class/profile/configuration selection. Original AI constructor16E288 sets58=1.
// Reuse49N's actual AI route: full aero steering and original engine positions,
// rather than preset4's half-steering and synthetic yaw torque bases.
inline bool bindAiColdControls50IS(std::uint32_t vehicle58,
    const AuthoredParameters49N& raw,const AuthoredSelectionContext49N& selection,
    const AuthoredBinding49N& previousAuthored,
    const CoupledBodyParameters49M& previousBodies,AiColdControls50IS& out) noexcept {
    if(vehicle58!=1u||!selection.mode58Nonzero)return false;
    AiColdControls50IS next{};
    if(!bindAuthoredParameters49N(raw,selection,previousAuthored,next.authored)||
       !projectAuthoredCoupledParameters49N(next.authored,previousBodies,next.bodies))return false;
    next.bound=true;out=next;return true;
}

enum class AiSpeedControlPhase50IS : std::uint8_t {
    Empty, AwaitingBoost, BoostIncomplete, AwaitingBinding, Complete
};
enum class AiSpeedControlError50IS : std::uint8_t {
    None, Phase, ColdOwner, VehicleOwner, SourceState, Input,
    MissingBoostOwner, IncompleteBoost, InvalidBoostResult, ChangedOwner, Binding
};
struct AiSpeedControlDiagnostic50IS {
    AiSpeedControlError50IS error=AiSpeedControlError50IS::None;
    std::uint32_t sourcePC=0;
    AiSpeedDiagnostic50IR speed{};
};
struct AiSpeedControlPending50IS {
    AiSpeedControlPhase50IS phase=AiSpeedControlPhase50IS::Empty;
    // Native identity only. No guest address, opaque byte object or copied body.
    const VehicleFrameState49W* owner=nullptr;
    const AiColdControls50IS* cold=nullptr;
    AiSpeedTargetReceipt50IR target{};
    AiNonBoostSpeedReceipt50IR ordinary{};
    PlayerReadResult48X afterDirection{},afterBoost{};
    PlayerControlStep48X priorControls{};
    ControlModelBindings48W priorBindings{};
    float preparedFactor=0.f,cachedSpeed34=0.f,damage7D0=0.f,damage7D4=0.f;
    std::uint32_t vehicleState40=0,observedAiStateFE8=0;
    bool boostHandled=false;
};
struct AiBoostOwner50IS {
    void* context=nullptr;
    // Implement genuine native173180, including its persistent109C timer and
    // genuine shared308030 random owner. The callback must finish once and
    // preserve canonical body/cache/control/damage fields. It may commit the
    // genuine boost timer/RNG owners. A per-racer snapshot RNG is NOT the
    // original cross-racer C-runtime stream; its owner must be explicitly bound.
    // false means an incomplete external boundary,
    // NOT a declined boost. Declined boost is true with sourceReturnNonzero=false.
    bool (*complete173180)(void*,float preparedFactor,float desiredSpeed,
        const PlayerReadResult48X& before,PlayerReadResult48X& after,
        bool& sourceReturnNonzero) noexcept=nullptr;
};
namespace ai_controls_detail50IS {
inline bool fail(AiSpeedControlDiagnostic50IS& d,AiSpeedControlError50IS e,
    std::uint32_t pc) noexcept {d.error=e;d.sourcePC=pc;return false;}
inline bool sameFloat(float a,float b) noexcept {return std::memcmp(&a,&b,sizeof a)==0;}
inline bool unchanged(const VehicleFrameState49W& s,const AiSpeedControlPending50IS& p) noexcept {
    const auto& m=s.bodies.main;
    return p.owner==&s&&s.fields.vehicle58==1u&&m.vehicleState40==p.vehicleState40&&
        sameFloat(m.cachedSpeed34,p.cachedSpeed34)&&sameFloat(s.fields.damage7D0,p.damage7D0)&&
        sameFloat(s.fields.damage7D4,p.damage7D4)&&
        std::memcmp(&m.controls,&p.priorControls,sizeof m.controls)==0&&
        std::memcmp(&m.components.bindings,&p.priorBindings,sizeof p.priorBindings)==0;
}
}
// Call only at the ordinary source AI speed boundary, after navigation/state/
// packing/repair/direction, with prepared49I time. The caller's target fields
// remain genuine source-owned values. Canonical shared flags replace duplicates.
// Snapshot existing canonical histories; never initialize them with guessed zero.
// Cold descriptors must remain alive and immutable through Complete. Frame,
// cold, pending and diagnostic storage must be disjoint, serially accessed.
inline bool prepareAiSpeedControls50IS(const VehicleFrameState49W& s,
    const AiColdControls50IS& cold,const AiSpeedTargetInput50IR& source,
    const PlayerReadResult48X& afterDirection,float preparedFactor,
    AiSpeedControlPending50IS& pending,AiSpeedControlDiagnostic50IS& d) noexcept {
    using namespace ai_controls_detail50IS;d={};
    if(pending.phase!=AiSpeedControlPhase50IS::Empty)
        return fail(d,AiSpeedControlError50IS::Phase,0x172c50);
    if(!cold.bound)return fail(d,AiSpeedControlError50IS::ColdOwner,0x2146c0);
    const auto& m=s.bodies.main;
    if(s.fields.vehicle58!=1u||m.vehicleState40==3u||m.vehicleState40==4u)
        return fail(d,AiSpeedControlError50IS::VehicleOwner,0x22877c);
    // Like the retained base-frame seam, this adapter does not execute217440.
    // Both canonical and supplied boundaries must prove its initialization.
    if(!(m.controls.player.flagsB9C&0x80u)||!(afterDirection.flagsB9C&0x80u))
        return fail(d,AiSpeedControlError50IS::SourceState,0x217440);
    // Actual3A7C20 dispatcher states that reach172C50:0,2,3. State6 has172B50.
    if(source.vehicleAiStateFE8!=0u&&source.vehicleAiStateFE8!=2u&&source.vehicleAiStateFE8!=3u)
        return fail(d,AiSpeedControlError50IS::SourceState,0x16e9fc);
    if(!ai_speed_detail50IR::admitted(preparedFactor)||preparedFactor<0.f||preparedFactor>0.06f)
        return fail(d,AiSpeedControlError50IS::Input,0x16e9c0);
    AiSpeedControlPending50IS next{};
    auto input=source;
    input.trackControlFlags58=s.fields.applicationByte58;
    input.vehicleFlagsCD8=s.fields.impactFlagsCD8;
    if(!selectAiTargetSpeed50IR(input,next.target,d.speed))
        return fail(d,AiSpeedControlError50IS::Input,d.speed.sourcePC);
    next.owner=&s;next.cold=&cold;next.afterDirection=afterDirection;
    std::memcpy(&next.priorControls,&m.controls,sizeof m.controls);
    std::memcpy(&next.priorBindings,&m.components.bindings,sizeof m.components.bindings);
    next.preparedFactor=preparedFactor;next.cachedSpeed34=m.cachedSpeed34;
    next.damage7D0=s.fields.damage7D0;next.damage7D4=s.fields.damage7D4;
    next.vehicleState40=m.vehicleState40;next.observedAiStateFE8=input.vehicleAiStateFE8;
    next.phase=AiSpeedControlPhase50IS::AwaitingBoost;
    // memcpy retains padding bytes used solely by the stale-owner comparison.
    std::memcpy(&pending,&next,sizeof next);return true;
}
// Missing provider is a named blocker; no default "boost declined" is supplied.
// The call is never automatically replayed, even if the provider only partially
// completed. BoostIncomplete requires resolving that owner's exact boundary.
inline bool resolveAiSpeedBoost50IS(const AiBoostOwner50IS& boost,
    AiSpeedControlPending50IS& pending,AiSpeedControlDiagnostic50IS& d) noexcept {
    using namespace ai_controls_detail50IS;d={};
    if(pending.phase!=AiSpeedControlPhase50IS::AwaitingBoost)
        return fail(d,AiSpeedControlError50IS::Phase,0x173180);
    if(!boost.complete173180)return fail(d,AiSpeedControlError50IS::MissingBoostOwner,0x173180);
    if(!pending.owner||!unchanged(*pending.owner,pending))
        return fail(d,AiSpeedControlError50IS::ChangedOwner,0x173180);
    auto after=pending.afterDirection;bool handled=false;
    pending.phase=AiSpeedControlPhase50IS::BoostIncomplete;
    if(!boost.complete173180(boost.context,pending.preparedFactor,pending.target.desiredSpeed,
        pending.afterDirection,after,handled))return fail(d,AiSpeedControlError50IS::IncompleteBoost,0x173180);
    //173180 never changes raw channels or flags other than boost bit0.
    if(std::memcmp(&after.raw,&pending.afterDirection.raw,sizeof after.raw)!=0||
       ((after.flagsB9C^pending.afterDirection.flagsB9C)&0xfeu))
        return fail(d,AiSpeedControlError50IS::InvalidBoostResult,0x173180);
    if(handled&&(after.actionB7C!=1.f||after.actionB80!=0.f||!(after.flagsB9C&1u)))
        return fail(d,AiSpeedControlError50IS::InvalidBoostResult,0x1733b8);
    pending.afterBoost=after;pending.boostHandled=handled;
    pending.phase=AiSpeedControlPhase50IS::AwaitingBinding;return true;
}
// Pure native control join, no player packet/reader and no HOST differential
// approximation. Consumes the ordinary leaf only when real173180 returned0,
// then source217DE0 bypass shaping, rate filtering and2181C4 model binding.
// Updates only canonical controls plus its model-binding mirror; body, pose,
// navigation, clock, frame phase and all parameter records remain untouched.
// A bind failure retains completed boost effects/receipt, so retrying this
// numeric join cannot duplicate random/timer effects. Serial, no reentrancy.
inline bool commitAiSpeedControls50IS(VehicleFrameState49W& s,
    AiSpeedControlPending50IS& pending,AiSpeedControlDiagnostic50IS& d) noexcept {
    using namespace ai_controls_detail50IS;d={};
    if(pending.phase!=AiSpeedControlPhase50IS::AwaitingBinding)
        return fail(d,AiSpeedControlError50IS::Phase,0x217de0);
    if(!pending.cold||!pending.cold->bound||!unchanged(s,pending))
        return fail(d,AiSpeedControlError50IS::ChangedOwner,0x217de0);
    auto actions=pending.afterBoost;
    AiNonBoostSpeedReceipt50IR ordinary{};
    if(!pending.boostHandled&&!applyAiNonBoostSpeed50IR(pending.target.desiredSpeed,
        pending.cachedSpeed34,pending.afterBoost,actions,ordinary,d.speed))
        return fail(d,AiSpeedControlError50IS::Input,d.speed.sourcePC);
    ControlBindingInput48W context{};
    context.bodyCachedSpeed34=pending.cachedSpeed34;context.vehicleState40=pending.vehicleState40;
    context.mode58Nonzero=true; // Admission observed actual vehicle58==1, never set it.
    // The original AI branch short-circuits configurationEntry; this zero is an
    // unused native argument, not a guessed selected controller configuration.
    context.actionB7C=actions.actionB7C;context.actionB80=actions.actionB80;
    context.flagsB9C=actions.flagsB9C;context.source7D0=pending.damage7D0;context.source7D4=pending.damage7D4;
    ControlBoundStep48W bound{};
    if(!stepAndBindControlInputs48W(actions.raw,
        pending.priorControls.bound.control.shaping.smoothedChannel0D28,
        pending.priorControls.bound.control.filtered,pending.cold->authored.rates,
        pending.preparedFactor,context,pending.cold->authored.controls,pending.priorBindings,bound))
        return fail(d,AiSpeedControlError50IS::Binding,0x217de0);
    auto controls=pending.priorControls;controls.player=actions;controls.bound=bound;
    s.bodies.main.controls=controls;s.bodies.main.components.bindings=bound.bindings;
    pending.ordinary=ordinary;pending.phase=AiSpeedControlPhase50IS::Complete;return true;
}
} // namespace rr::game
