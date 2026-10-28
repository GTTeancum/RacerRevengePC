#pragma once
// Private detached source-owner proposal. No controller parent, call site,
// playback, source-factor producer or complete177750 return is supplied.
#include <bit>
namespace rr::platform {
inline bool RaceAudioGroups50IC::constructDetachedColdTaIntroEmitter177750(
    std::shared_ptr<const assets::OriginalTaIntroAuthoringCandidate> authoring)noexcept {
    std::shared_ptr<const OriginalApplicationAural50KG> manager;
    if(coldTaIntroEmitter_||!prepareColdTaIntroConstruction(authoring,manager)||
       coldApplication_->objectCount423F60()==std::numeric_limits<std::uint32_t>::max())return false;
    std::unique_ptr<ColdTaIntroEmitter> next;
    std::unique_ptr<ColdTaIntroEmitter::Event*[]> initial;
    // Native allocation policy: reserve emitter and initial one-row backing
    // before the first counted/source effect. Not original heap/OOM fidelity.
    try{next=std::make_unique<ColdTaIntroEmitter>();
        initial=std::make_unique<ColdTaIntroEmitter::Event*[]>(1);}catch(...){return false;}
    const auto key=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&next->entity));
    if(!coldApplication_->entityAuthority_.reserveAdditional(1)||
       !coldApplication_->entityAuthority_.canEnroll(next->entity,key))return false;
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    p.taIntroAuthoring=std::move(authoring);p.taIntroManager=std::move(manager);p.taIntroStarted=true;
    coldTaIntroEmitter_=std::move(next);auto& e=*coldTaIntroEmitter_;
    const auto made=game::initializeOriginalWreckComponent50LA(e.entity,
        // Existing legacy-name tag denotes THIS exact shared2921C0 constructor;
        // it grants no Garage scope, widget authority or virtual dispatch.
        game::OriginalWreckComponentKind50LA::wattoEmitter2921C0,
        coldSession_,key,true,coldApplication_->objectCount423F60());
    if(made.status!=game::OriginalWreckComponentStatus50LA::complete||
       !coldApplication_->entityAuthority_.enroll(e.entity))std::terminate();
    //290F10 initial vf0 stores precede its shared assertion and112-byte memset.
    constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    e.previous40=vf0;
    for(std::size_t n=0;n<4;++n)e.configuration160[4+n]=vf0[n];
    for(std::size_t n=0;n<4;++n)e.configuration160[8+n]=vf0[n];
    for(std::size_t n=0;n<4;++n)e.configuration160[16+n]=vf0[n];
    auto& shared=coldApplication_->garageEmitterShared_;
    if(!shared.guard8C5C){shared.assertion434C0=1;shared.guard8C5C=1;}
    // The actual nonnull same-Application Aural proves the reached assertion.
    for(auto& word:e.configuration160)word=0; //290FF4 memset160..1CF.
    e.manager1D8=coldApplication_->aural00_; // Aural00 Entity, NOT interfaceA0.
    e.group30=&p;e.sound1F0=nullptr;
    e.previous40={{0x7f7fffffu,0x7f7fffffu,0x7f7fffffu,0x3f800000u}};
    for(std::size_t n=0;n<4;++n)e.configuration160[4+n]=e.previous40[n];
    e.flags158=0;e.trigger1DCBits=0;e.chance1E0Bits=0x42c80000u;e.name54='\0';
    e.type1E4=6;
    // Only the source-established low five bits are represented; upper bits unknown.
    e.flags1D4Low5&=static_cast<std::uint8_t>(~1u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~2u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~4u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~8u);
    e.flags1D4Low5&=static_cast<std::uint8_t>(~16u);
    e.serial50=shared.serial434B8; //291120, historical value, not global-1 identity.
    ++shared.serial434B8; //291134 wrapping32-bit process history, never rollback.
    e.distance1D0Bits=0;e.word1E8=0;e.word1EC=1;
    for(std::size_t n=0;n<4;++n)e.previous40[n]=std::bit_cast<std::uint32_t>(shared.origin43470[n]);
    //2921C0 ->294060 then294090(1,16). The original offsets matter:
    //growth208=16, capacity20C=1. Auxiliary list assertion globals unrepresented.
    e.members204.reset();e.count200=0;e.growth208=0;e.capacity20C=0;e.allocated210=false;
    e.members204=std::move(initial);e.growth208=16;e.capacity20C=1;e.allocated210=true;
    // Reached1777C8/292300->2911E0 mode2,a3=0,t0=0 configuration projection.
    //141C20 returns this nonnull receiver directly (not an activity test);
    //the actual nonnull manager1D8 independently proves the configuration arm.
    e.flags1D4Low5&=static_cast<std::uint8_t>(~8u); //2912AC; bit2-set branch skipped.
    e.configuration160[0]=0x70u;
    for(std::size_t n=0;n<4;++n)e.configuration160[4+n]=vf0[n];
    for(std::size_t n=0;n<4;++n)e.configuration160[8+n]=vf0[n];
    e.configuration160[12].reset();e.configuration160[13].reset(); //190/194 unknown descriptor.
    for(std::size_t n=0;n<4;++n)e.configuration160[16+n]=vf0[n];
    e.configuration160[20].reset();e.configuration160[21].reset(); //1B0/1B4 unknown.
    e.configuration160[22]=0;e.configuration160[23]=0x43e10000u; //Near0, far450.
    e.configuration160[24].reset(); //1C0 unknown, not a fabricated zero.
    e.flags158=2;e.trigger1DCBits=0x41a00000u;e.chance1E0Bits=0x42c80000u;
    e.configured=true;
    // Remains genuinely detached. No141960, controller80, cue map or completion.
    return true;
}
inline bool RaceAudioGroups50IC::coldTaIntroEmitterIdentity()const noexcept {
    if(!coldTaIntroEmitter_||!coldTaIntroGroupIdentity())return false;
    const auto& e=*coldTaIntroEmitter_;const auto& node=e.entity;
    const auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(!p.taIntroStarted||!p.taIntroAuthoring||!p.taIntroManager||!e.configured||
       node.kind!=game::OriginalWreckComponentKind50LA::wattoEmitter2921C0||node.reference00!=1||
       node.sessionGeneration!=coldSession_||node.objectKey!=static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(&node))||
       node.next14||node.firstChild18||node.descendants24||
       node.parent1C!=e.controllerParent||(!e.controllerParent&&node.previous10)||
       node.mask20!=(e.everAttached?0xffffffffu:0u)||
       (e.controllerParent&&(!e.everAttached||
         e.controllerParent->kind!=game::OriginalWreckComponentKind50LA::cameraController174A30||
         e.controllerParent->sessionGeneration!=coldSession_||
         !coldApplication_->entityAuthority_.canRetire(*e.controllerParent)))||
       !(node.flags29&1u)||!coldApplication_->entityAuthority_.canRetire(node)||
       e.manager1D8!=coldApplication_->aural00_||e.manager1D8!=p.taIntroManager->inspect().entity||
       e.group30!=&p||(!taIntroLive_&&e.sound1F0)||e.type1E4!=6||e.flags158!=2||e.flags1D4Low5||
       e.trigger1DCBits!=0x41a00000u||e.chance1E0Bits!=0x42c80000u||
       e.distance1D0Bits||e.word1E8||e.word1EC!=1||e.name54||
       !e.members204||!e.allocated210||e.growth208!=16||e.count200>p.taIntroAuthoring->eventCount()||
       e.capacity20C!=(e.count200<2?1u:17u)||p.count48<e.count200||p.count48>e.count200+1||
       p.taIntroConstructed!=(e.count200==p.taIntroAuthoring->eventCount()))return false;
    constexpr std::array<std::uint32_t,4> vf0{{0,0,0,0x3f800000u}};
    for(std::size_t n=0;n<4;++n)
        if(e.previous40[n]!=std::bit_cast<std::uint32_t>(coldApplication_->garageEmitterShared_.origin43470[n]))return false;
    // All descriptor words have known zero/configured values except the five
    // actual caller-unwritten words. No complete stack-descriptor claim.
    for(std::size_t n=0;n<e.configuration160.size();++n){
        const bool unknown=n==12||n==13||n==20||n==21||n==24;
        if(unknown){if(e.configuration160[n])return false;continue;}
        std::uint32_t expected=0;
        if(n==0)expected=0x70u;
        else if(n>=4&&n<8)expected=vf0[n-4];
        else if(n>=8&&n<12)expected=vf0[n-8];
        else if(n>=16&&n<20)expected=vf0[n-16];
        else if(n==23)expected=0x43e10000u;
        if(!e.configuration160[n]||*e.configuration160[n]!=expected)return false;
    }
    for(std::size_t n=0;n<p.count48;++n)if(!coldTaIntroSoundIdentity(n))return false;
    for(std::size_t n=0;n<e.ownedEvents.size();++n){
        if(n<e.count200){
            if(!e.ownedEvents[n]||e.members204[n]!=e.ownedEvents[n].get()||
               e.ownedEvents[n]->id00!=n+1||!e.ownedEvents[n]->sound04||
               *e.ownedEvents[n]->sound04!=p.taIntroSounds[n].get())return false;
        }else if(e.ownedEvents[n])return false;
    }
    if(e.pendingEvent){
        if(e.nativeAppendAborted||e.count200==p.taIntroAuthoring->eventCount()||e.pendingEvent->id00!=e.count200+1)return false;
        if(e.pendingEvent->sound04){
            if(p.count48!=e.count200+1||!*e.pendingEvent->sound04||
               *e.pendingEvent->sound04!=p.taIntroSounds[e.count200].get())return false;
        }else if(p.count48!=e.count200)return false;
    }else if(!e.nativeAppendAborted&&p.count48!=e.count200)return false;
    return true;
}
inline RaceAudioGroups50IC::ColdTaEmitterAppendResult
RaceAudioGroups50IC::appendNextColdTaIntroEvent292650()noexcept {
    if(!coldTaIntroEmitterIdentity()||coldRetiring_||nativeBindingsRetired_||
       !groups_.initialized||!mixer_||mixer_!=coldMixer_||epoch_!=coldEpoch_||
       mixer_->count()!=count_||!mixer_->requiresVoiceGroupGains50IC())return ColdTaEmitterAppendResult::ownerRejected;
    auto& e=*coldTaIntroEmitter_;
    auto& p=coldGroups_[static_cast<std::size_t>(game::RaceAudioGroup50IC::Announcer120)];
    if(e.nativeAppendAborted||e.count200==p.taIntroAuthoring->eventCount()||!p.taIntroManager->current())return ColdTaEmitterAppendResult::ownerRejected;
    if(taIntroConstructionAdmission_){
        if(!taIntroExistingBindingsCurrent())return ColdTaEmitterAppendResult::ownerRejected;
    }else for(std::size_t n=0;n<count_;++n)
        if(mixer_->active(n)||mixer_->hasVoiceGroupGain50IC(n)||
           bindings_[n]!=game::RaceAudioGroup50IC::Unrepresented)return ColdTaEmitterAppendResult::ownerRejected;
    if(!e.pendingEvent){
        try{e.pendingEvent=std::make_unique<ColdTaIntroEmitter::Event>();}
        catch(...){return ColdTaEmitterAppendResult::allocationPending;}
        e.pendingEvent->id00=e.count200+1; //29275C, BEFORE Sound allocation/construction.
    }
    if(!e.pendingEvent->sound04){
        std::unique_ptr<ColdSound> sound;
        try{sound=std::make_unique<ColdSound>();}
        catch(...){return ColdTaEmitterAppendResult::allocationPending;} // Retain wrapper/ID.
        //232DB0 source-field commit registers THIS Sound in the actual group.
        auto* returned=appendPreparedColdTaIntroSound232ED0(std::move(sound));
        e.pendingEvent->sound04=returned; //2927BC, BEFORE2941F0 append.
    }
    if(e.count200==e.capacity20C){
        const auto nextCapacity=e.count200+e.growth208; //Actual1+16=17.
        std::unique_ptr<ColdTaIntroEmitter::Event*[]> grown;
        try{grown=std::make_unique<ColdTaIntroEmitter::Event*[]>(nextCapacity);}
        catch(...){return ColdTaEmitterAppendResult::allocationPending;} //Retain wrapper AND registered Sound.
        //294500 copies OLD CAPACITY pointers, not an invented live-count size.
        //Here the actual full one-row list proves every copied slot initialized.
        for(std::size_t n=0;n<e.capacity20C;++n)grown[n]=e.members204[n];
        auto old=std::move(e.members204);
        e.members204=std::move(grown); //294568 delay-slot publication BEFORE old free.
        old.reset();e.capacity20C=nextCapacity; //294564/294570.
    }
    const auto index=e.count200;auto* wrapper=e.pendingEvent.get();
    e.members204[index]=wrapper; //294320 pointer first.
    ++e.count200; //29432C count next.
    e.ownedEvents[index]=std::move(e.pendingEvent); //Native ownership only, no Sound ref bump.
    if(e.count200==p.taIntroAuthoring->eventCount()){p.taIntroConstructed=true;return ColdTaEmitterAppendResult::eventsComplete;}
    return ColdTaEmitterAppendResult::eventAppended;
}
inline bool RaceAudioGroups50IC::abortPendingColdTaIntroEventNative()noexcept {
    if(!coldTaIntroEmitterIdentity()||!coldTaIntroEmitter_->pendingEvent)return false;
    auto& e=*coldTaIntroEmitter_;
    // Explicit bounded native abort of an UNPUBLISHED wrapper. This allocation
    // is not in source list200 and is not falsely attributed to292220's loop.
    // Its optional Sound remains owned by the actual group, even on growth failure.
    e.pendingEvent.reset();e.nativeAppendAborted=true;return true;
}
inline bool RaceAudioGroups50IC::retireDetachedColdTaIntroEmitter142590()noexcept {
    if(!coldTaIntroEmitterIdentity()||coldTaIntroEmitter_->controllerParent||coldTaIntroEmitter_->pendingEvent||
       !coldApplication_->objectCount423F60())return false;
    auto& e=*coldTaIntroEmitter_;
    //142590's actual admitted detached/no-children/no-sibling arm. No invented
    //parent unlink, callback, controller80 clearing or attachment receipt.
    if(!coldApplication_->entityAuthority_.retire(e.entity))return false;
    --e.entity.reference00; //104970 reaches the concrete292220 destructor.
    for(std::size_t n=0;n<e.count200;++n)e.ownedEvents[n].reset(); //292268, published order only.
    for(std::size_t n=0;n<e.count200;++n)e.members204[n]=nullptr; //2943A0.
    e.count200=0;
    e.members204.reset(); //29229C..A0 after wrapper clear, before1413E0.
    --coldApplication_->objectCount423F60(); //2922BC->1413E0, not Sound release.
    coldTaIntroEmitter_.reset(); //2922D4 native holding allocation retirement.
    // Shared assertion/serial and every group Sound survive. Existing prefix
    // clear2353D0 must now retire the actual0..3 group members separately.
    return true;
}
} // namespace rr::platform
