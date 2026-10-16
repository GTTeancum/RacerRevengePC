#pragma once
#include "../../src/game/original_gc_spray50kg.h"
#include "../../src/game/original_gc_pod_dust50kg.h"
#include "../../src/game/original_gc_pod_sparks50kg.h"
#include "../../src/game/original_gc_pod_fire50kg.h"
#include "../../src/game/effect_dispatch50be.h"
#include "../../src/game/original_wreck_components50la.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

// Concrete common and derived calls for the actual14 existing vehicle7FC
// owners. Canonical50BE controls/duration stay in the racer bank; this stable
// per-racer extension contains only missing derived state and manager handles.
// Immutable authoring stays outside the bank. Hot use is allocation free.
// Native GPU ownership replaces141F4C's PSXRenderTarget244D60 under the declared
// completed-render native schedule; no console callbacks are impersonated.
namespace native_gc_other_particle_children50KG {
using namespace rr::game;
enum class Status : std::uint8_t {
    Complete,Duplicate,Unprepared,Owner,Sequence,Common,Derived,ActivatedLateChild
};
struct Result {
    Status status=Status::Unprepared;
    std::uint32_t sourcePC=0;
    unsigned ownerIndex=0,completedChildren=0;
    OriginalGcSprayReceipt50KG common{};
    bool nativeUndefinedCarryPolicy=false;
    bool okay() const noexcept{return status==Status::Complete||status==Status::Duplicate;}
};
inline constexpr std::array<unsigned,14> sourceMember{{0,1,2,3,4,4,5,5,2,3,4,4,5,5}};
// Actual constructor/list append order: primary right, primary left, optional
// bottom right, optional bottom left, then two dust children, then two sprays.
inline constexpr std::array<unsigned,14> childOrder{{3,5,7,2,4,6,9,11,13,8,10,12,0,1}};
inline EffectProfile50BE expectedProfile(unsigned index) noexcept {
    const auto source=sourceMember[index];return source<4?EffectProfile50BE::Particle19A5E0:
        source==4?EffectProfile50BE::Ordinary15F100:EffectProfile50BE::Fire15DE00;
}
inline std::uint32_t derivedEntry(EffectProfile50BE profile) noexcept {
    switch(profile){
    case EffectProfile50BE::Particle19A5E0:return 0x19a820u;
    case EffectProfile50BE::Ordinary15F100:return 0x15f380u;
    case EffectProfile50BE::Fire15DE00:return 0x15e0d0u;
    case EffectProfile50BE::Deferred15E800:return 0x15ead0u;
    }
    return 0;
}
struct SharedAuthoring {
    // Order is the existing effectSources array: L/R dust, L/R smoke,
    // shared sparks, shared fire. The caller authenticates each full member
    // against the already retained effectHashes before this cold parser call.
    std::array<OriginalGcCommonEmitterTiming50KG,6> timing{};
    std::array<OriginalGcPodDustDefinition50KG,4> dust{};
    OriginalGcPodSparksDefinition50KG sparks{};
    OriginalGcPodFireDefinition50KG fire{};
    bool parsed=false;
    Result parse(const std::array<std::vector<std::uint8_t>,6>& effectSources) noexcept {
        if(parsed)return {Status::Owner,0x15aea0u};
        std::array<OriginalGcCommonEmitterTiming50KG,6> next{};
        for(unsigned i=0;i<next.size();++i){rr::assets::OriginalUiPageDiagnostic50KG d{};
            const auto s=readOriginalGcCommonEmitterTiming50KG(effectSources[i],next[i],d);
            if(s!=OriginalGcSprayStatus50KG::complete){
                Result r{Status::Common,d.sourcePC?d.sourcePC:0x15aea0u,i};r.common.status=s;return r;}
        }
        std::array<OriginalGcPodDustDefinition50KG,4> nextDust{};
        OriginalGcPodSparksDefinition50KG nextSparks{};OriginalGcPodFireDefinition50KG nextFire{};
        for(unsigned i=0;i<4;++i){rr::assets::OriginalUiPageDiagnostic50KG d{};
            const auto status=readOriginalGcPodDustDefinition50KG(effectSources[i],
                static_cast<OriginalGcPodDustMember50KG>(i),nextDust[i],d);
            if(status!=OriginalGcSprayStatus50KG::complete){Result r{Status::Derived,d.sourcePC,i};
                r.common.status=status;return r;}}
        rr::assets::OriginalUiPageDiagnostic50KG d{};
        auto status=readOriginalGcPodSparksDefinition50KG(effectSources[4],nextSparks,d);
        if(status!=OriginalGcSprayStatus50KG::complete){Result r{Status::Derived,d.sourcePC,4};
            r.common.status=status;return r;}
        status=readOriginalGcPodFireDefinition50KG(effectSources[5],nextFire,d);
        if(status!=OriginalGcSprayStatus50KG::complete){Result r{Status::Derived,d.sourcePC,5};
            r.common.status=status;return r;}
        timing=next;dust=nextDust;sparks=nextSparks;fire=nextFire;
        parsed=true;return {Status::Complete,0x15b5a4u};
    }
};
struct Runtime {
    std::array<OriginalGcPodDustRuntime50KG,14> dust{};
    std::array<OriginalGcPodSparksRuntime50KG,14> sparks{};
    std::array<OriginalGcPodFireRuntime50KG,14> fire{};
    OriginalGcParticleManager50KG* manager=nullptr;
    std::array<float,14> delay1DC{};
    // Canonical native projection of source child gates. Source141300(a1=1)
    // gives consumed bits29[3:0]=3;141960 writes the actual child20=0402.
    // Upper unconsumed29 bits have native zero initialization only.
    // These stable subobjects now own the actual Entity gate words and links.
    // The cold forest materializes their identities/topology, never a second
    // copy of29/20. Canonical50BE controls still come from the current bank.
    std::array<OriginalWreckComponent50LA,14> children{};
    //814,810, then818[0..9], source Stop84 thenvirtual18=141520. These
    // canonical gates retain the original low29=0 projection. They are NOT
    // proof that their as-yet-unjoined derived constructors/registrations ran;
    // a full cold forest must bind those real owners before completing.
    std::array<OriginalWreckComponent50LA,12> lateChildren{};
    std::size_t ownerCount=0;
    std::uint64_t sequence=0;
    float sequenceDelta=0;
    bool sequencePaused=false,bound=false,haveSequence=false,failed=false;
    unsigned nextOrdinal=0,completedChildren=0;
    Result stoppedAt{};

    // Call after the genuine existing fresh buildEffectControlOwner50BJ and
    // immediate Stop84 operations, before the first source component update.
    // Optional bottom model presence comes from the actual vehicle1C0/1C4
    // binding; it must agree with the already allocated canonical owner count.
    Result bindCold(const SharedAuthoring& shared,EffectOwner50BE* owners,
        std::size_t count,bool actualBottomPairPresent,OriginalGcParticleManager50KG& sharedManager) noexcept {
        if(failed)return stoppedAt;
        if(bound||!shared.parsed)return {Status::Unprepared,0x210f5cu};
        if(!owners||count!=(actualBottomPairPresent?14u:8u))return {Status::Owner,0x2112c8u};
        for(unsigned i=0;i<count;++i){const auto& o=owners[i];const auto& t=shared.timing[sourceMember[i]];
            if(o.id!=i+1||o.profile!=expectedProfile(i)||o.mode1D8!=t.mode1D8||
               (o.state.flags1D4&7u)!=1u)return {Status::Owner,0x15a5b0u,i};
            // Existing cold50BJ source owner supports0/1. Do not pretend it
            // already performed random-mode2 load RNG or mode3 cold binding.
            if(o.mode1D8>1)return {Status::Owner,0x15b148u,i};
            const float initial=o.mode1D8==1?t.maxTimeOn1EC:0.f;
            std::uint32_t expected=0;std::memcpy(&expected,&initial,4);
            if(o.duration1E4!=expected)return {Status::Owner,0x15b0d8u,i};
        }
        // Registration follows the actual141960 append/constructor order and
        // precedes this racer's two spray registrations. Numeric50BE IDs are
        // racer-local; each real child receives a distinct manager handle.
        for(const auto i:childOrder){if(i>=count)continue;
            OriginalGcSprayReceipt50KG r;const auto source=sourceMember[i];
            if(source<4)r=bindOriginalGcPodDust50KG(dust[i],shared.dust[source],sharedManager,owners[i]);
            else if(source==4)r=bindOriginalGcPodSparks50KG(sparks[i],shared.sparks,sharedManager,owners[i]);
            else r=bindOriginalGcPodFire50KG(fire[i],shared.fire,sharedManager,owners[i]);
            if(!r.okay()){stoppedAt={Status::Derived,r.sourcePC,i,0,r};failed=true;return stoppedAt;}
        }
        manager=&sharedManager;
        for(unsigned i=0;i<count;++i){delay1DC[i]=shared.timing[sourceMember[i]].delayTime1E0;
            children[i].flags29=3;children[i].mask20=0x402u;}
        for(auto& child:lateChildren){child.flags29=0;child.mask20=0x402u;}
        ownerCount=count;bound=true;return {Status::Complete,0x212dc4u};
    }
    static bool start(void* context,EffectControlState50BC& state) noexcept {
        auto* owner=static_cast<EffectOwner50BE*>(context);
        if(!owner||&state!=&owner->state)return false;
        startEffect50BC(state);return true;
    }
    static bool stop(void* context,EffectControlState50BC& state) noexcept {
        auto* owner=static_cast<EffectOwner50BE*>(context);
        if(!owner||&state!=&owner->state)return false;
        if(owner->profile==EffectProfile50BE::Deferred15E800)
            return stopDeferredEffect50BD(state,owner->mode1D8,owner->duration1E4)!=EffectStopResult50BD::UnsupportedDuration;
        stopEffect50BC(state);return true;
    }
    // One actual child38, selected by the canonical intrusive7FC traversal.
    // This method does not perform141F4C or traverse siblings; its parent must
    // call the actual renderer first and read next14 only after this returns.
    // Controls are the active bank's genuine50BE owners, not retained pointers
    // to a previous racer's visit. The stable component remains this Runtime's.
    Result advanceOneAfterParentDispatch(const SharedAuthoring& shared,
        EffectOwner50BE* owners,std::size_t count,unsigned index,
        float delta,bool applicationPaused,std::uint32_t& sharedRawRandom,
        SharedRandomOwner50ID& selectors,HostMonotonicClock50ID& clock,
        std::uint32_t& sharedFireSerial,OriginalGcPodSparksGlobals50KG& sparksGlobals) noexcept {
        if(!bound||!shared.parsed||!manager)return {Status::Unprepared,0x141ed0u};
        if(!owners||count!=ownerCount||index>=count)return {Status::Owner,0x141f54u,index};
        if(failed)return stoppedAt;
        auto& owner=owners[index];const auto& timing=shared.timing[sourceMember[index]];
        if(owner.id!=index+1||owner.profile!=expectedProfile(index)||owner.mode1D8!=timing.mode1D8)
            return {Status::Owner,0x141f54u,index};
        const OriginalGcCommonEmitterDispatch50KG calls{&owner,start,stop};
        const auto r=advanceOriginalGcCommonEmitter50KG(owner.state,timing,delay1DC[index],
            owner.duration1E4,false,delta,applicationPaused,sharedRawRandom,&calls);
        if(!r.okay()){stoppedAt={Status::Common,r.sourcePC,index,0,r};failed=true;return stoppedAt;}
        bool nativeCarry=false;
        if(!r.commonSkipped){
            OriginalGcSprayReceipt50KG derived;const auto source=sourceMember[index];
            if(source<4)derived=advanceOriginalGcPodDustAfterCommon50KG(dust[index],
                shared.dust[source],*manager,owner,delta,sharedRawRandom,selectors,clock);
            else if(source==4)derived=advanceOriginalGcPodSparksAfterCommon50KG(sparks[index],
                shared.sparks,*manager,owner,delta,sharedRawRandom,sparksGlobals);
            else {const auto fireResult=advanceOriginalGcPodFireAfterCommon50KG(fire[index],
                shared.fire,*manager,owner,delta,sharedRawRandom,sharedFireSerial,true);
                nativeCarry=fireResult.nativeUndefinedCarryPolicy;derived=fireResult;}
            if(!derived.okay()){stoppedAt={Status::Derived,derived.sourcePC,index,0,derived};
                stoppedAt.nativeUndefinedCarryPolicy=nativeCarry;failed=true;return stoppedAt;}
        }
        Result complete{Status::Complete,0x141f60u,index,1};
        complete.nativeUndefinedCarryPolicy=nativeCarry;return complete;
    }
    // Serial source component-visit identity, not presentation-frame count.
    // Duplicate completed calls are idempotent. Failure is latched even for a
    // newer sequence, so no committed common/RNG prefix can be replayed.
    // Start/Stop callbacks bind existing concrete50BE semantics. Derived
    // continuation executes only after this same child's common returned0.
    Result advanceChildren(const SharedAuthoring& shared,
        EffectOwner50BE* owners,std::size_t count,std::uint64_t visitSequence,
        float delta,bool applicationPaused,std::uint32_t& sharedRawRandom,
        SharedRandomOwner50ID& selectors,HostMonotonicClock50ID& clock,
        std::uint32_t& sharedFireSerial,OriginalGcPodSparksGlobals50KG& sparksGlobals) noexcept {
        if(!bound||!shared.parsed||!manager)return {Status::Unprepared,0x141ed0u};
        if(!owners||count!=ownerCount)return {Status::Owner,0x141ed0u};
        if(failed)return stoppedAt;
        if(haveSequence){
            if(visitSequence<sequence||(visitSequence==sequence&&
               (std::memcmp(&delta,&sequenceDelta,4)||applicationPaused!=sequencePaused)))
                return {Status::Sequence,0x141ed0u};
            if(visitSequence==sequence){
                if(nextOrdinal==childOrder.size())return {Status::Duplicate,0x141f78u,0,completedChildren};
                return {Status::Sequence,0x141ed0u};
            }
            if(nextOrdinal!=childOrder.size())return {Status::Sequence,0x141ed0u};
        }
        // Validate immutable source identity before this visit mutates anyone.
        for(unsigned i=0;i<count;++i)if(owners[i].id!=i+1||owners[i].profile!=expectedProfile(i)||
            owners[i].mode1D8!=shared.timing[sourceMember[i]].mode1D8)
            return {Status::Owner,0x141f54u,i};
        haveSequence=true;sequence=visitSequence;sequenceDelta=delta;sequencePaused=applicationPaused;
        nextOrdinal=0;completedChildren=0;
        bool usedNativeCarryPolicy=false;
        for(;nextOrdinal<childOrder.size();++nextOrdinal){
            const auto index=childOrder[nextOrdinal];if(index>=count)continue;
            if(!originalWreckChildEligible50LA(children[index]))continue; //141EFC..F34.
            const auto child=advanceOneAfterParentDispatch(shared,owners,count,index,delta,
                applicationPaused,sharedRawRandom,selectors,clock,sharedFireSerial,sparksGlobals);
            usedNativeCarryPolicy|=child.nativeUndefinedCarryPolicy;
            if(!child.okay()){stoppedAt=child;stoppedAt.completedChildren=completedChildren;
                stoppedAt.nativeUndefinedCarryPolicy=usedNativeCarryPolicy;failed=true;return stoppedAt;}
            ++completedChildren;
        }
        Result complete{Status::Complete,0x141f78u,0,completedChildren};
        complete.nativeUndefinedCarryPolicy=usedNativeCarryPolicy;return complete;
    }
    // Call at the actual late-child segment AFTER the sprays and all original
    // wreck-collection effect children. The legacy ordinary-only schedule has
    // no constructed wreck graph and retains this explicit projection gate.
    // A later virtual20/24 activation must write these same flags; it cannot
    // be suppressed by a permanent skip list. Report its real update target.
    Result lateChildrenGate() const noexcept {
        if(!bound)return {Status::Unprepared,0x141ed0u};
        for(unsigned i=0;i<lateChildren.size();++i){
            if(originalWreckChildEligible50LA(lateChildren[i]))
                return {Status::ActivatedLateChild,i==0?0x15ead0u:i==1?0x15f380u:0x19bf30u,i};
        }
        return {Status::Complete,0x141f78u};
    }
};
} // namespace native_gc_other_particle_children50KG
