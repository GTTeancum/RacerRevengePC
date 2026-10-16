#pragma once
#include "lap_event_random50id.h"
#include <cfenv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>

namespace rr::game {
// Native source continuations only. These shared owners/cursors are OUTSIDE
// racer snapshots. The caller must serialize them with every existing RNG,
// lap-event, announcer and animated-world consumer. Never shadow sharedRaw.
// A live projection of the real profile field, NOT RR_RACE_MODE or a timing
// declaration. Set bound only after joining the actual application profile.
struct DynamicContactProfile50IW {std::uint32_t mode11258=0;bool bound=false;};
enum class DynamicContactEffectStatus50IW : std::uint8_t {
    Complete,Continue,NeedHelper,NeedBinding,AlreadyActive,WrongReturn,
    WrongEvent,ActorUnavailable,UnsupportedNumeric,RandomRange,
    SequenceExhausted,SourceRejected
};
struct DynamicContactEffectResult50IW {
    DynamicContactEffectStatus50IW status;
    std::uint32_t sourcePC=0;
    LapEventRequest50ID request{};
};
enum class DynamicContactMusicPhase50IW : std::uint8_t {
    Idle,Entry,AfterClock,AfterSeed,AfterDraw,AfterMusic,Complete
};
struct DynamicContactMusicCursor50IW {
    DynamicContactMusicPhase50IW phase=DynamicContactMusicPhase50IW::Idle;
    LapEventRequest50ID pending{};
    std::uint64_t lastSequence=0;
    std::uint32_t lastReturn=0,index=0,song=0;
};
namespace dynamic_contact_events50IW_detail {
inline DynamicContactEffectResult50IW result(DynamicContactEffectStatus50IW s,
    std::uint32_t pc) noexcept {return {s,pc,{}};}
inline DynamicContactEffectResult50IW pending(const DynamicContactMusicCursor50IW& c) noexcept {
    return {DynamicContactEffectStatus50IW::NeedHelper,c.pending.callPC,c.pending};
}
inline DynamicContactEffectResult50IW issue(DynamicContactMusicCursor50IW& c,
    LapEventCall50ID kind,std::uint32_t pc,DynamicContactMusicPhase50IW next,
    std::uint32_t a0=0,std::uint32_t a1=0) noexcept {
    if(c.lastSequence==std::numeric_limits<std::uint64_t>::max())
        return result(DynamicContactEffectStatus50IW::SequenceExhausted,pc);
    c.pending={kind,++c.lastSequence,pc,a0,a1,0,0};c.phase=next;
    return pending(c);
}
inline float mul(float a,float b) noexcept {volatile float v=a*b;return v;}
}
inline DynamicContactEffectResult50IW beginDynamicContactMusic50IW(
    DynamicContactMusicCursor50IW& c) noexcept {
    using P=DynamicContactMusicPhase50IW;using S=DynamicContactEffectStatus50IW;
    if(c.pending.sequence||(c.phase!=P::Idle&&c.phase!=P::Complete))
        return dynamic_contact_events50IW_detail::result(S::AlreadyActive,0x22a400);
    c.phase=P::Entry;c.lastReturn=0;c.index=0;c.song=0;
    return dynamic_contact_events50IW_detail::result(S::Continue,0x22a410);
}
inline bool acceptDynamicContactMusicReturn50IW(DynamicContactMusicCursor50IW& c,
    std::uint64_t sequence,LapEventCall50ID kind,std::uint32_t actualV0) noexcept {
    const bool query=kind==LapEventCall50ID::Clock101A50||kind==LapEventCall50ID::Draw308030;
    if(!c.pending.sequence||c.pending.sequence!=sequence||c.pending.kind!=kind||
        (!query&&actualV0))return false;
    c.lastReturn=actualV0;c.pending={};return true;
}
// Seed/draw requests are directly usable with applyLapEventRandom50ID; the
// clock request uses the one existing101A50 owner/provider. Ignored-return
// helpers are acknowledged with0 only AFTER their real return.
inline DynamicContactEffectResult50IW advanceDynamicContactMusic50IW(
    DynamicContactMusicCursor50IW& c,LapEventRandomOwner50ID& sharedLazy,
    DynamicContactRangeOwner50IW& sharedRange) noexcept {
    using P=DynamicContactMusicPhase50IW;using K=LapEventCall50ID;
    using S=DynamicContactEffectStatus50IW;using namespace dynamic_contact_events50IW_detail;
    if(c.pending.sequence)return pending(c);
    for(;;)switch(c.phase){
    case P::Entry:
        if(!sharedLazy.lazy80A8){sharedLazy.seed3D6B30=0;sharedLazy.lazy80A8=true;}
        if(!sharedLazy.seed3D6B30)return issue(c,K::Clock101A50,0x22a43c,P::AfterClock);
        return issue(c,K::Draw308030,0x22a460,P::AfterDraw);
    case P::AfterClock:
        sharedLazy.seed3D6B30=c.lastReturn;
        return issue(c,K::Seed308020,0x22a450,P::AfterSeed,sharedLazy.seed3D6B30);
    case P::AfterSeed:return issue(c,K::Draw308030,0x22a460,P::AfterDraw);
    case P::AfterDraw:{
        if(std::fegetround()!=FE_TONEAREST||c.lastReturn>0x7fffffffu)
            return result(S::UnsupportedNumeric,0x22a470);
        volatile float draw=static_cast<float>(c.lastReturn);
        const auto fraction=mul(0x1p-31f,draw);
        const auto chosen=mul(6.0f,fraction);
        if(!std::isfinite(chosen)||chosen<0.f||chosen>6.f)
            return result(S::UnsupportedNumeric,0x22a488);
        // EE CVT.W.S truncates toward zero, as in existing49K/50ID owners.
        // Float rounding can produce6 and hence index7: never clamp/modulo.
        c.index=static_cast<std::uint32_t>(chosen)+1u;
        if(!sharedRange.lazy80AC){sharedRange.rangeCheck3D6B38=1;sharedRange.lazy80AC=true;}
        if(sharedRange.rangeCheck3D6B38&&(c.index<1u||c.index>6u))
            return result(S::RandomRange,0x22a4dc);
        if(c.index>6u){c.phase=P::Complete;return result(S::Complete,0x22a604);}
        constexpr std::uint32_t songs[6]{38,42,41,40,39,43};
        constexpr std::uint32_t pcs[6]{0x22a548,0x22a56c,0x22a590,0x22a5b4,0x22a5d8,0x22a5fc};
        c.song=songs[c.index-1];return issue(c,K::Music1A46E0,pcs[c.index-1],P::AfterMusic,0,c.song);
    }
    case P::AfterMusic:c.phase=P::Complete;return result(S::Complete,0x22a604);
    case P::Complete:return result(S::Complete,0x22a604);
    default:return result(S::WrongEvent,0x22a400);
    }
}
//1A46FC->1A4900: actual profile11258!=1 admits ONLY44/45. This completes
//38..43 without touching music128/12C/130, but only after all22A400 effects.
//Mode1 stays pending at the real helper for its timer/stop/play continuation.
inline bool completeSuppressedDynamicContactMusic50IW(DynamicContactMusicCursor50IW& c,
    const DynamicContactProfile50IW& profile) noexcept {
    if(!profile.bound||profile.mode11258==1u||!c.pending.sequence||
       c.pending.kind!=LapEventCall50ID::Music1A46E0||c.pending.argumentA2||
       c.pending.argumentA1<38u||c.pending.argumentA1>43u)return false;
    return acceptDynamicContactMusicReturn50IW(c,c.pending.sequence,c.pending.kind,0);
}

struct DynamicContactEventBindings50IW {
    bool eventOwner33CBound=false;
    float chance138=0.f,chance13C=0.f,chance14C=0.f;
    bool chance138Bound=false,chance13CBound=false,chance14CBound=false;
};
// Cold identity admission for caller-supplied original SPCHPCT.INI. The
// caller computes SHA256 over its actual bytes using the existing asset hash
// owner. Only authenticated length/digest enter this semantic field binder;
// no original file bytes are embedded or reconstructed here.
// Source1A5F90/1A5FAC/1A601C bind these three numeric fields in this asset.
inline bool bindOriginalDynamicContactChances50IW(std::size_t actualBytes,
    const char* actualSha256,DynamicContactEventBindings50IW& out) noexcept {
    constexpr char expectedSha256[]=
        "24c2967388a31d04761fb11208faaae43a4ae7b9b88b40291bf64daf195c018c";
    if(actualBytes!=255u||!actualSha256||std::strcmp(actualSha256,expectedSha256))return false;
    out.chance138=.35f;out.chance13C=.25f;out.chance14C=.90f;
    out.chance138Bound=true;out.chance13CBound=true;out.chance14CBound=true;
    return true;
}
//22ADE0 only skips when BOTH actual58 fields are nonzero. Partner0C and
//self10 are deliberate source order. These actor handles resolve actual
//vehicles; no serial arena address is a persistent identity. The supplied
//queue must be the canonical self33C owner, shared with existing event5/6.
template<std::size_t N> inline RaceEventAppend50ID appendDynamicContactEvent1_50IW(
    RaceEventQueue50ID<N>& queue,const RaceActorResolver50ID& actors,
    RaceActorId50ID self,RaceActorId50ID partner,std::uint32_t word8,
    bool eventOwner33CBound) noexcept {
    RaceActorSnapshot50ID a{},b{};
    if(!self||!actors.read||!actors.read(actors.context,self,a))
        return {RaceEventStatus50ID::ActorUnavailable,0};
    if(a.vehicle58){
        if(!partner||!actors.read(actors.context,partner,b))
            return {RaceEventStatus50ID::ActorUnavailable,0};
        if(b.vehicle58)return {RaceEventStatus50ID::ActorSuppressed,0};
    }
    if(!eventOwner33CBound)return {RaceEventStatus50ID::ActorUnavailable,0};
    return appendRaceEvent50ID(queue,{0,1,word8,partner,self},actors);
}

enum class DynamicContactEventCall50IW : std::uint8_t {
    None,PodTaunt2C1C00,PodCurse2C1A90,Announcer2C2C20
};
struct DynamicContactEventRequest50IW {
    DynamicContactEventCall50IW kind=DynamicContactEventCall50IW::None;
    std::uint64_t sequence=0;
    std::uint32_t callPC=0;
    // Actual source actor: D48 receiver for pod speech, or the racer60
    // supplier for an announcer request. Never infer it from the camera slot.
    RaceActorId50ID actor=0;
    std::uint32_t type=0,racer60=0,variant=0;
};
struct DynamicContactEventResult50IW {
    DynamicContactEffectStatus50IW status;
    std::uint32_t sourcePC=0;
    DynamicContactEventRequest50IW request{};
};
enum class DynamicContactEventPhase50IW : std::uint8_t {
    Idle,Begin,PartnerHuman,PartnerHumanFallback,SelfHuman,SelfHumanFallback,
    PartnerSpeech,SelfSpeech,AttackChance,AnnouncerActor,AnnouncerChance,
    AnnouncerRequest,AfterAnnouncer,Complete
};
struct DynamicContactEventCursor50IW {
    DynamicContactEventPhase50IW phase=DynamicContactEventPhase50IW::Idle;
    DynamicContactEventRequest50IW pending{};
    RaceEvent50ID event{};
    std::uint64_t lastTicket=0,lastSequence=0;
    bool partnerSpeech=false,selfSpeech=false,announcerPartnerHuman=false;
    bool chanceDrawPending=false,announcerNamed=false;
    float chanceFraction=0.f;
    std::uint32_t randomDraws=0;
};
namespace dynamic_contact_events50IW_detail {
inline DynamicContactEventResult50IW eventResult(DynamicContactEffectStatus50IW s,
    std::uint32_t pc) noexcept {return {s,pc,{}};}
inline DynamicContactEventResult50IW eventPending(const DynamicContactEventCursor50IW& c) noexcept {
    return {DynamicContactEffectStatus50IW::NeedHelper,c.pending.callPC,c.pending};
}
inline DynamicContactEventResult50IW eventIssue(DynamicContactEventCursor50IW& c,
    DynamicContactEventCall50IW kind,std::uint32_t pc,DynamicContactEventPhase50IW next,
    RaceActorId50ID actor,std::uint32_t type,std::uint32_t racer=0,std::uint32_t variant=0) noexcept {
    if(c.lastSequence==std::numeric_limits<std::uint64_t>::max())
        return eventResult(DynamicContactEffectStatus50IW::SequenceExhausted,pc);
    c.pending={kind,++c.lastSequence,pc,actor,type,racer,variant};c.phase=next;
    return eventPending(c);
}
// Draw happens BEFORE the source chance read. An unavailable binding retains
//the already committed draw/fraction; retrying never advances RNG twice.
inline DynamicContactEffectStatus50IW chance(DynamicContactEventCursor50IW& c,
    std::uint32_t& actualSharedRaw,bool bound,float threshold,bool& passed) noexcept {
    using S=DynamicContactEffectStatus50IW;
    if(std::fegetround()!=FE_TONEAREST)return S::UnsupportedNumeric;
    if(!c.chanceDrawPending){
        LapEventRequest50ID request{};request.kind=LapEventCall50ID::Draw308030;
        std::uint32_t value=0;
        if(!applyLapEventRandom50ID(request,actualSharedRaw,value))return S::SourceRejected;
        volatile float draw=static_cast<float>(value);
        c.chanceFraction=mul(0x1p-31f,draw);c.chanceDrawPending=true;++c.randomDraws;
    }
    if(!bound)return S::NeedBinding;
    if(!std::isfinite(threshold))return S::UnsupportedNumeric;
    passed=c.chanceFraction<threshold;c.chanceDrawPending=false;return S::Complete;
}
}
template<std::size_t N> inline DynamicContactEventResult50IW beginDynamicContactEvent1Subscriber50IW(
    const RaceEventQueue50ID<N>& queue,DynamicContactEventCursor50IW& c) noexcept {
    using P=DynamicContactEventPhase50IW;using S=DynamicContactEffectStatus50IW;
    using namespace dynamic_contact_events50IW_detail;
    if(c.pending.sequence||(c.phase!=P::Idle&&c.phase!=P::Complete))
        return eventResult(S::AlreadyActive,0x1a5940);
    const auto* entry=frontRaceEvent50ID(queue);
    if(!entry||entry->ticket<=c.lastTicket||entry->event.type4!=1u||entry->event.word0)
        return eventResult(S::WrongEvent,0x1a5940);
    c.lastTicket=entry->ticket;c.event=entry->event;c.phase=P::Begin;
    c.partnerSpeech=false;c.selfSpeech=false;c.announcerPartnerHuman=false;
    c.announcerNamed=false;c.chanceDrawPending=false;c.chanceFraction=0.f;c.randomDraws=0;
    return eventResult(S::Continue,0x1a5964);
}
inline bool acceptDynamicContactEventReturn50IW(DynamicContactEventCursor50IW& c,
    std::uint64_t sequence,DynamicContactEventCall50IW kind) noexcept {
    if(!c.pending.sequence||c.pending.sequence!=sequence||c.pending.kind!=kind||
        kind==DynamicContactEventCall50IW::None)return false;
    c.pending={};return true;
}
//1A4DB4->1A5940 has NO mode2 early return. Actual58 chooses which actor
//gets138 versus13C, and suppresses the second draw only when first passed.
//The source FIFO ticket remains unacknowledged until this returns Complete.
inline DynamicContactEventResult50IW advanceDynamicContactEvent1Subscriber50IW(
    DynamicContactEventCursor50IW& c,const RaceActorResolver50ID& actors,
    const DynamicContactEventBindings50IW& b,std::uint32_t& actualSharedRaw) noexcept {
    using P=DynamicContactEventPhase50IW;using K=DynamicContactEventCall50IW;
    using S=DynamicContactEffectStatus50IW;using namespace dynamic_contact_events50IW_detail;
    if(c.pending.sequence)return eventPending(c);
    for(;;)switch(c.phase){
    case P::Begin:{
        if(!c.event.actor0C||!c.event.actor10){c.phase=P::Complete;return eventResult(S::Complete,0x1a5c10);}
        if(!b.eventOwner33CBound)return eventResult(S::NeedBinding,0x1a5940);
        RaceActorSnapshot50ID partner{};
        if(!actors.read||!actors.read(actors.context,c.event.actor0C,partner))
            return eventResult(S::ActorUnavailable,0x1a598c);
        if(!partner.vehicle58){c.phase=P::PartnerHuman;break;}
        RaceActorSnapshot50ID self{};
        if(!actors.read(actors.context,c.event.actor10,self))
            return eventResult(S::ActorUnavailable,0x1a5a18);
        c.phase=self.vehicle58?P::PartnerSpeech:P::SelfHuman;break;
    }
    case P::PartnerHuman:{
        const auto s=chance(c,actualSharedRaw,b.chance138Bound,b.chance138,c.partnerSpeech);
        if(s!=S::Complete)return eventResult(s,0x1a59b4);
        c.phase=c.partnerSpeech?P::PartnerSpeech:P::PartnerHumanFallback;break;
    }
    case P::PartnerHumanFallback:{
        const auto s=chance(c,actualSharedRaw,b.chance13CBound,b.chance13C,c.selfSpeech);
        if(s!=S::Complete)return eventResult(s,0x1a59f4);
        c.phase=P::PartnerSpeech;break;
    }
    case P::SelfHuman:{
        const auto s=chance(c,actualSharedRaw,b.chance138Bound,b.chance138,c.selfSpeech);
        if(s!=S::Complete)return eventResult(s,0x1a5a3c);
        c.phase=c.selfSpeech?P::PartnerSpeech:P::SelfHumanFallback;break;
    }
    case P::SelfHumanFallback:{
        const auto s=chance(c,actualSharedRaw,b.chance13CBound,b.chance13C,c.partnerSpeech);
        if(s!=S::Complete)return eventResult(s,0x1a5a7c);
        c.phase=P::PartnerSpeech;break;
    }
    case P::PartnerSpeech:
        if(c.partnerSpeech)return eventIssue(c,K::PodTaunt2C1C00,0x1a5aa8,P::SelfSpeech,c.event.actor0C,1);
        c.phase=P::SelfSpeech;break;
    case P::SelfSpeech:
        if(c.selfSpeech)return eventIssue(c,K::PodCurse2C1A90,0x1a5abc,P::AttackChance,c.event.actor10,1);
        c.phase=P::AttackChance;break;
    case P::AttackChance:{
        if(!c.event.word8){c.phase=P::Complete;return eventResult(S::Complete,0x1a5c10);}
        bool passed=false;const auto s=chance(c,actualSharedRaw,b.chance14CBound,b.chance14C,passed);
        if(s!=S::Complete)return eventResult(s,0x1a5ae8);
        if(!passed){c.phase=P::Complete;return eventResult(S::Complete,0x1a5c10);}
        c.phase=P::AnnouncerActor;break;
    }
    case P::AnnouncerActor:{
        RaceActorSnapshot50ID partner{};
        if(!actors.read||!actors.read(actors.context,c.event.actor0C,partner))
            return eventResult(S::ActorUnavailable,0x1a5b00);
        c.announcerPartnerHuman=partner.vehicle58==0;c.phase=P::AnnouncerChance;break;
    }
    case P::AnnouncerChance:{
        const bool human=c.announcerPartnerHuman;
        const auto s=chance(c,actualSharedRaw,human?b.chance138Bound:b.chance13CBound,
            human?b.chance138:b.chance13C,c.announcerNamed);
        if(s!=S::Complete)return eventResult(s,human?0x1a5b24:0x1a5b60);
        c.phase=P::AnnouncerRequest;break;
    }
    case P::AnnouncerRequest:{
        RaceActorSnapshot50ID actor{};
        const auto id=c.announcerNamed?c.event.actor0C:c.event.actor10;
        if(!actors.read||!actors.read(actors.context,id,actor))
            return eventResult(S::ActorUnavailable,c.announcerNamed?0x1a5b94:0x1a5bd4);
        if(c.announcerNamed)return eventIssue(c,K::Announcer2C2C20,
            actor.vehicle60==16u?0x1a5bac:0x1a5bc4,P::AfterAnnouncer,id,2,actor.vehicle60,
            //1A5978 initializes s2=0; ONLY the AI-partner successful
            //chance branch at1A5B88 changes it to2. A human partner keeps
            //variant0, including its genuine generic-probability RNG draw.
            actor.vehicle60==16u?1u:(c.announcerPartnerHuman?0u:2u));
        if(actor.vehicle60==16u){
            RaceActorSnapshot50ID partner{};
            if(!actors.read(actors.context,c.event.actor0C,partner))
                return eventResult(S::ActorUnavailable,0x1a5be8);
            return eventIssue(c,K::Announcer2C2C20,0x1a5bf0,P::AfterAnnouncer,c.event.actor0C,2,partner.vehicle60,2);
        }
        return eventIssue(c,K::Announcer2C2C20,0x1a5c08,P::AfterAnnouncer,id,1,actor.vehicle60,0);
    }
    case P::AfterAnnouncer:c.phase=P::Complete;return eventResult(S::Complete,0x1a5c10);
    case P::Complete:return eventResult(S::Complete,0x1a5c10);
    default:return eventResult(S::WrongEvent,0x1a5940);
    }
}
//This exact2C2DF0 profile predicate precedes owner state/RNG writes and can
//complete the call only when the real profile and event-owner168 are bound.
//Mode1 requires the actual shared50IE owner and authenticated roster banks;
//this convenience suppression must never acknowledge that live request.
inline bool completeSuppressedDynamicContactAnnouncer50IW(DynamicContactEventCursor50IW& c,
    const DynamicContactProfile50IW& profile,bool announcer168Bound) noexcept {
    if(!profile.bound||profile.mode11258==1u||!announcer168Bound||!c.pending.sequence||
       c.pending.kind!=DynamicContactEventCall50IW::Announcer2C2C20||
       (c.pending.type!=1u&&c.pending.type!=2u))return false;
    return acceptDynamicContactEventReturn50IW(c,c.pending.sequence,c.pending.kind);
}

//Resumable adapter to the genuine existing D48 selector. Keep this cursor at
//the event-owner level; select only against request.actor's actual speech
//state/counts/position/listeners. A selected command remains pending until the
//same native audio provider really returns; do not select again on retry.
struct DynamicContactPodEffect50IW {
    std::uint64_t sequence=0;
    RaceActorId50ID actor=0;
    DynamicContactEventCall50IW kind=DynamicContactEventCall50IW::None;
    bool selected=false,pendingAudio=false,complete=false;
    float returnedCooldown224=0.f;
    PodSpeechReceipt50HG receipt{};
    AudioCommand50HE command{};
};
inline DynamicContactEffectResult50IW beginDynamicContactPodSpeech50IW(
    const DynamicContactEventRequest50IW& request,RaceActorId50ID actualBoundActor,
    const PodSpeechCounts50HG& counts,const SpeechPosition50HG& position,
    const SpeechListeners50HG& listeners,std::uint32_t& actualSharedRaw,
    PodSpeechState50HG& actualSpeech,DynamicContactPodEffect50IW& c) noexcept {
    using K=DynamicContactEventCall50IW;using S=DynamicContactEffectStatus50IW;
    using namespace dynamic_contact_events50IW_detail;
    if(!request.sequence||!request.actor||request.actor!=actualBoundActor||request.type!=1u||
       (request.kind!=K::PodTaunt2C1C00&&request.kind!=K::PodCurse2C1A90))
        return result(S::NeedBinding,request.callPC);
    if(c.selected&&c.sequence==request.sequence){
        if(c.actor!=request.actor||c.kind!=request.kind)return result(S::WrongReturn,request.callPC);
        return result(c.complete?S::Complete:S::NeedHelper,c.complete?request.callPC:c.receipt.sourceSite);
    }
    if(c.selected&&!c.complete)return result(S::AlreadyActive,request.callPC);
    AudioCommand50HE command{};std::size_t count=0;
    const auto category=request.kind==K::PodTaunt2C1C00?
        PodSpeechCategory50HG::Attacking:PodSpeechCategory50HG::Attacked;
    const auto priorCooldown224=actualSpeech.cooldown224;
    const auto receipt=selectPodSpeech50HG(category,counts,position,listeners,
        actualSharedRaw,actualSpeech,&command,1,count);
    if(receipt.status!=PodSpeechStatus50HG::Complete)return result(S::SourceRejected,request.callPC);
    c.sequence=request.sequence;c.actor=request.actor;c.kind=request.kind;c.selected=true;
    c.pendingAudio=count!=0;c.complete=!c.pendingAudio;c.receipt=receipt;c.command=command;
    c.returnedCooldown224=actualSpeech.cooldown224;
    // Cursor rotation precedes292360, but the5-second store follows it at
    //2C1BDC/2C1CC4. Keep that store pending with the genuine audio request.
    if(c.pendingAudio)actualSpeech.cooldown224=priorCooldown224;
    return result(c.complete?S::Complete:S::NeedHelper,c.complete?request.callPC:receipt.sourceSite);
}
inline bool acceptDynamicContactPodAudioReturn50IW(DynamicContactPodEffect50IW& c,
    std::uint64_t sequence,RaceActorId50ID actualBoundActor,
    PodSpeechState50HG& actualSpeech) noexcept {
    if(!c.selected||!c.pendingAudio||c.complete||c.sequence!=sequence||
       c.actor!=actualBoundActor||!actualSpeech.initialized)return false;
    actualSpeech.cooldown224=c.returnedCooldown224;
    c.pendingAudio=false;c.complete=true;return true;
}
} // namespace rr::game
