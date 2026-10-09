#pragma once
#include "lap_drive50gj_fixture.h"
#include "race_speech_dispatch50id_fixture.h"
#include "native_racer_bank50hn_fixture.h"
#include "race_session50hm_fixture.h"
#include "lap_host_clock50id.h"
#include <array>

namespace race_mode1_host50ID {
using namespace rr::game;
// HOST roster metadata is deliberately separate from arena.vehicle58: current
// bot adapters execute player physics and must retain that existing field.
// racer60 is the source roster identity, never a slot number/pod suffix guess.
// known is required for the human only. Opponent identity/index lanes are
// unrepresented and unconsumed by these human-only event subscribers.
struct RosterEntry {std::uint32_t racer60=0,humanIndex5C=0;bool human=false,known=false;};
enum class Phase:std::uint8_t {Idle,Timing,AwaitHostCommit,Flush};
enum class Status:std::uint8_t {Complete,AwaitHostCommit,NeedFlush,Blocked,InvalidState};
struct Receipt {
    Status status=Status::InvalidState;
    std::uint32_t slot=0,sourcePC=0;
    race_speech50ID::Receipt subscriber{};
};
struct Effects {
    void* context=nullptr;
    // These are the ONLY unrepresented downstream function bodies. A true
    // means genuine synchronous source return, including source suppression
    // or a queued request. It does not mean clip playback has completed.
    bool (*announcer)(void*,std::uint32_t type,std::uint32_t racer,
        std::uint32_t variant) noexcept=nullptr;
    bool (*music)(void*,std::uint32_t event,std::uint32_t argumentA2) noexcept=nullptr;
    // Existing liveAudio->applySourceCommand(...).okay(), not a new sound path.
    bool (*audio)(void*,const AudioCommand50HE&) noexcept=nullptr;
    // Existing speechInput() with represented racer60; called on human arena0.
    bool (*damageInput)(void*,DamageSpeechInput50HG&) noexcept=nullptr;
};
class Owner {
public:
    native_racer50HN::Bank& bank;
    lap_drive50GJ::Owner& lap;
    VehicleFrameFields49W& fields;
    race_session50HM::Session& hostRace;
    LapConfigTa50ID config{};
    LapTimingManager50ID manager{0.0f,2u};
    race_speech50ID::Queue events{};
    race_speech50ID::Owner subscriber{};
    std::array<RosterEntry,native_racer50HN::maximumRacers> roster{};
    Phase phase=Phase::Idle;
    bool initialized=false;
    std::uint64_t acceptedWorld=0;

    Owner(native_racer50HN::Bank& b,lap_drive50GJ::Owner& l,
        VehicleFrameFields49W& f,race_session50HM::Session& h) noexcept:
        bank(b),lap(l),fields(f),hostRace(h){}
    Owner(const Owner&)=delete;
    Owner& operator=(const Owner&)=delete;

    // Cold, after bank.seal/roster installation and before first native step.
    // Current bounded client has one human in slot0; all original HUD fields
    // in its native scene are explicitly absent. HOST overlay remains separate.
    bool initialize(const RosterEntry* entries,std::size_t count) {
        if(initialized||!entries||count!=bank.count()||count<2||count>roster.size()||
           count>16||!entries[0].human||entries[0].humanIndex5C!=0u)return false;
        for(std::size_t i=0;i<count;++i)
            if((entries[i].human&&!entries[i].known)||(i&&entries[i].human))return false;
        if(!initializeFreshTaLapConfig50ID(config,1,3,1,
            static_cast<std::uint8_t>(count-1)))return false;
        for(std::size_t i=0;i<count;++i)roster[i]=entries[i];
        lap.nativeConfig=&config;
        for(std::size_t i=0;i<count;++i)if(!bank.visit(i,[&](){
            lap.continuity->originalHud={0,0,0,true};
            return lap.initializeTiming();
        }))return false;
        initialized=true;return true;
    }

    // Replacement for the selected-player post-repair event5 producer only.
    // Must run during its native step, before this world's timing/flush pass.
    DamageSpeechReceipt50HG produceDamage(const DamageSpeechInput50HG& liveInput) noexcept {
        DamageSpeechReceipt50HG bad{};bad.status=PodSpeechStatus50HG::Input;
        if(!initialized||phase!=Phase::Idle)return bad;
        auto input=liveInput;input.vehicle60=roster[0].racer60;input.vehicle60Known=true;
        input.vehicle58=0; // Explicit HOST human role, not an arena mutation.
        LocalActor local{1,{input.vehicle3C,0,roster[0].racer60,
            lap.continuity->state.vehicleCC8,hostRace.racers[0].place,fields.impactFlagsCD8}};
        return produceQueuedDamageSpeech50ID(input,lap.continuity->prefix,1,
            {&local,&readLocal},events);
    }

    // Explicit HOST accepted-world boundary, after all native/contact work has
    // succeeded, before sampleRaceField/50HM.commitFrame. Caller supplies the
    // SAME binary32 seconds used by physics (.016f in50DU). No wall-clock or
    // interpolated finishSeconds is copied into CB0/CB8. Resume this method
    // on a blocked timing request before admitting another world.
    Receipt beforeHostCommit(std::uint64_t world,float seconds) {
        if(!initialized||!world)return {};
        if(phase==Phase::Idle){
            if(world<=acceptedWorld||!std::isfinite(seconds)||seconds<0.0f||seconds>0.06f)
                return {};
            currentWorld_=world;delta_=seconds;nextSlot_=0;
            for(std::size_t i=0;i<bank.count();++i)consumed_[i]=false;
            const auto clock=beginLapManagerClock50ID(manager,seconds,false);
            if(clock!=LapManagerClockStatus50ID::NeedRacerPass&&
               clock!=LapManagerClockStatus50ID::OtherManagerPath)return {};
            timingPass_=clock==LapManagerClockStatus50ID::NeedRacerPass;
            phase=Phase::Timing;
        }
        if(phase!=Phase::Timing||world!=currentWorld_||seconds!=delta_)return {};
        if(timingPass_)for(;nextSlot_<bank.count();++nextSlot_){
            if(!bank.active(nextSlot_))continue;
            const auto slot=nextSlot_;
            const bool complete=bank.visit(slot,[&](){
                auto snapshot=liveSnapshot(slot);
                LocalActor local{static_cast<RaceActorId50ID>(slot+1),snapshot};
                const bool done=lap.managerTimingCut(manager,delta_,snapshot.vehicle58,
                    roster[slot].humanIndex5C,local.id,{&local,&readLocal},events);
                if(done)consumed_[slot]=true;
                return done;
            });
            if(!complete)return {Status::Blocked,static_cast<std::uint32_t>(slot),0x1aff50u};
        }
        phase=Phase::AwaitHostCommit;
        return {Status::AwaitHostCommit};
    }

    // Call only after existing HOST50HM commit, finished-bit projection and
    // human closeResults. That observer owns rank/finish interpolation; no
    // claim of1B0400/1B02F0/full-manager scheduler equivalence is made here.
    // The consumed CD8 bit1 is cleared only after the corresponding timing
    // cut and host observer have finished, before the same world's FIFO flush.
    Receipt afterHostCommit(std::uint64_t world,std::uint8_t application1B0) {
        if(phase!=Phase::AwaitHostCommit||world!=currentWorld_||
           hostRace.racerCount!=bank.count()||hostRace.error!=race_session50HM::Error::None)return {};
        for(std::size_t i=0;i<bank.count();++i)if(hostRace.racers[i].sampled)return {};
        for(std::size_t i=0;i<bank.count();++i)if(consumed_[i])bank.visit(i,[&](){
            fields.impactFlagsCD8=static_cast<std::uint8_t>(fields.impactFlagsCD8&~2u);
            lap.continuity->state.vehicleCD8=fields.impactFlagsCD8;
        });
        // HOST ends the one-human manager when its observer closes results.
        // It does not set source bit5 or reinterpret it as a generic ended bit.
        if(hostRace.resultsClosed)manager.flags58=static_cast<std::uint8_t>(manager.flags58|1u);
        const auto flush=beginRaceEventFlush50ID(events,manager.flags58,application1B0);
        if(flush==RaceEventStatus50ID::FlushSuppressed){
            acceptedWorld=currentWorld_;phase=Phase::Idle;return {Status::Complete};
        }
        if(flush!=RaceEventStatus50ID::Ready&&flush!=RaceEventStatus50ID::Empty)return {};
        phase=Phase::Flush;return {Status::NeedFlush};
    }

    // All event5/event6 effects use one ordered flush before the shared mix.
    // Runtime clock/lazy/RNG live outside this owner and racer snapshots.
    // The current HOST cold-session policy resets the shared lazy slot together
    // with its freshly constructed native raw RNG. sharedRaw is the existing
    // human RNG outside bank visits; the HOST's independent bot RNGs remain.
    Receipt flush(HostMonotonicClock50ID& clock,LapEventRandomOwner50ID& lazy,
        std::uint32_t& sharedRaw,const PodSpeechCounts50HG& counts,
        PodSpeechState50HG& humanSpeech,const Effects& effects) noexcept {
        if(phase!=Phase::Flush)return {};
        DispatchContext context{*this,clock,sharedRaw,counts,humanSpeech,effects};
        const race_speech50ID::Providers providers{&context,&beginDamage,&damageEffect,&lapEffect};
        const auto done=subscriber.drain(events,{this,&readActor},config,lazy,providers);
        if(done.status!=race_speech50ID::Status::Complete)
            return {Status::Blocked,0,done.sourcePC,done};
        acceptedWorld=currentWorld_;phase=Phase::Idle;return {Status::Complete};
    }
private:
    struct LocalActor {RaceActorId50ID id;RaceActorSnapshot50ID snapshot;};
    std::uint64_t currentWorld_=0;
    std::size_t nextSlot_=0;
    float delta_=0;
    bool timingPass_=false;
    std::array<bool,native_racer50HN::maximumRacers> consumed_{};
    static bool readLocal(void* context,RaceActorId50ID actor,
        RaceActorSnapshot50ID& out) noexcept {
        const auto& local=*static_cast<LocalActor*>(context);
        if(actor!=local.id)return false;out=local.snapshot;return true;
    }
    RaceActorSnapshot50ID liveSnapshot(std::size_t slot) const noexcept {
        return {fields.vehicle3C,roster[slot].human?0u:1u,roster[slot].racer60,
            lap.continuity->state.vehicleCC8,hostRace.racers[slot].place,fields.impactFlagsCD8};
    }
    static bool readActor(void* context,RaceActorId50ID actor,
        RaceActorSnapshot50ID& out) noexcept {
        auto& self=*static_cast<Owner*>(context);
        if(!actor||actor>self.bank.count())return false;
        const auto slot=static_cast<std::size_t>(actor-1u);
        try{out=self.bank.visit(slot,[&](){return self.liveSnapshot(slot);});return true;}
        catch(...){return false;} // Unavailable owner keeps the FIFO request.
    }
    struct DispatchContext {
        Owner& owner;HostMonotonicClock50ID& clock;std::uint32_t& raw;
        const PodSpeechCounts50HG& counts;PodSpeechState50HG& speech;const Effects& effects;
    };
    static DamageEventStatus50ID beginDamage(void* context,
        const race_speech50ID::Queue& queue,DamageEventCursor50ID& cursor) noexcept {
        auto& c=*static_cast<DispatchContext*>(context);
        const auto* entry=frontRaceEvent50ID(queue);
        if(!entry||entry->event.actor0C!=1||!c.effects.damageInput)
            return DamageEventStatus50ID::WrongEvent;
        DamageSpeechInput50HG input{};
        if(!c.effects.damageInput(c.effects.context,input))return DamageEventStatus50ID::SourceRejected;
        input.vehicle60=c.owner.roster[0].racer60;input.vehicle60Known=true;input.vehicle58=0;
        return beginDamageEvent5Subscriber50ID(queue,input,c.counts,c.raw,c.speech,cursor);
    }
    static bool damageEffect(void* context,const DamageEventCursor50ID& pending) noexcept {
        auto& c=*static_cast<DispatchContext*>(context);
        if(pending.pending==DamageEventEffect50ID::AudioCommand)
            return c.effects.audio&&c.effects.audio(c.effects.context,pending.command);
        const auto& r=pending.receipt;
        return pending.pending==DamageEventEffect50ID::Announcer2C2C20&&
            r.announcerArgumentKnown&&c.effects.announcer&&
            c.effects.announcer(c.effects.context,r.announcerType,r.announcerRacer,r.announcerVariant);
    }
    static bool lapEffect(void* context,const LapEventRequest50ID& request,
        std::uint32_t& actualV0) noexcept {
        auto& c=*static_cast<DispatchContext*>(context);actualV0=0;
        if(request.kind==LapEventCall50ID::Announcer2C2C20)
            return c.effects.announcer&&c.effects.announcer(c.effects.context,
                request.argumentA1,request.argumentA2,request.argumentA3);
        if(request.kind==LapEventCall50ID::Music1A46E0)
            return c.effects.music&&c.effects.music(c.effects.context,
                request.argumentA1,request.argumentA2);
        return applyHostLapClockOrRandom50ID(request,c.clock,c.raw,actualV0);
    }
};
}
