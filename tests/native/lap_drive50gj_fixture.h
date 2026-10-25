#pragma once
#include "lap_progress50gf.h"
#include "lap_completion50gg.h"
#include "vehicle_frame49w.h"
#include "lap_sound_owner50gh_fixture.h"
#include "lap_config_ta50id.h"
#include "lap_timing50id.h"
#include "race_event_queue50id.h"
#include "lap_hud_null50id.h"
#include "original_race_hud50kg.h"

namespace lap_drive50GJ {
using namespace rr::game;
// Host development configuration. Mode2 intentionally omits records/timing;
// this is not loaded from applicationByte58 or a claimed original menu choice.
// The live client joins wrong-way1A2E30 after navigation. Race finish/rank
// ownership remains outside this component and is not a completed race claim.
struct Continuity {
    LapProgressState50GF state{};
    UndertrackPlayerPrefix50ET prefix{};
    bool initialized=false;
    // Trivially-copyable per-racer ownership; existing bank.owner(lapContinuity)
    // includes these fields automatically. Recovery preserves them.
    LapTimingFields50ID timing{};
    LapTimingCursor50ID timingCursor{};
    bool timingInitialized=false;
    bool timingEventReturned=false;
    LapHudFields50ID originalHud{};
};
struct AudioDispatchResult {
    bool complete=false,started=false,unavailable=false;
    std::int32_t originalSoundV0=0;
};
struct Owner {
private:
    LapProgressTrackView50MB mbTrack_{};
    bool mbBound_=false;
    LapProgressTrackView50TC tcTrack_{};
    bool tcBound_=false;
    LapProgressTrackView50TB tbTrack_{};
    bool tbBound_=false;
    LapProgressTrackView50GB gbTrack_{};
    bool gbBound_=false;
    LapProgressTrackView50MC mcTrack_{};bool mcBound_=false;
public:
    bool bindMbTrack(const LapProgressTrackView50MB& actual) noexcept {
        if(mbBound_||tcBound_||tbBound_||gbBound_||mcBound_||!actual.runtimeValid()||pending.pending.requested)return false;
        mbTrack_=actual;mbBound_=true;return true;
    }
    const LapProgressTrackView50MB& mbTrack()const noexcept{return mbTrack_;}
    bool releaseMbTrack() noexcept {
        if(pending.pending.requested)return false;
        mbTrack_={};mbBound_=false;return true;
    }
    bool bindTcTrack(const LapProgressTrackView50TC& actual) noexcept {
        if(mbBound_||tcBound_||tbBound_||gbBound_||mcBound_||!actual.runtimeValid()||pending.pending.requested)return false;
        tcTrack_=actual;tcBound_=true;return true;
    }
    const LapProgressTrackView50TC& tcTrack()const noexcept{return tcTrack_;}
    bool releaseTcTrack() noexcept {
        if(pending.pending.requested)return false;
        tcTrack_={};tcBound_=false;return true;
    }
    bool bindTbTrack(const LapProgressTrackView50TB& actual) noexcept {
        if(mbBound_||tcBound_||tbBound_||gbBound_||mcBound_||!actual.runtimeValid()||pending.pending.requested)return false;
        tbTrack_=actual;tbBound_=true;return true;
    }
    const LapProgressTrackView50TB& tbTrack()const noexcept{return tbTrack_;}
    bool releaseTbTrack() noexcept {
        if(pending.pending.requested)return false;
        tbTrack_={};tbBound_=false;return true;
    }
    bool bindGbTrack(const LapProgressTrackView50GB& actual) noexcept {
        if(mbBound_||tcBound_||tbBound_||gbBound_||mcBound_||!actual.runtimeValid()||pending.pending.requested)return false;
        gbTrack_=actual;gbBound_=true;return true;
    }
    const LapProgressTrackView50GB& gbTrack()const noexcept{return gbTrack_;}
    bool releaseGbTrack() noexcept {
        if(pending.pending.requested)return false;
        gbTrack_={};gbBound_=false;return true;
    }
    bool bindMcTrack(const LapProgressTrackView50MC& actual) noexcept {
        if(mbBound_||tcBound_||tbBound_||gbBound_||mcBound_||!actual.runtimeValid()||pending.pending.requested)return false;
        mcTrack_=actual;mcBound_=true;return true;
    }
    const LapProgressTrackView50MC& mcTrack()const noexcept{return mcTrack_;}
    bool releaseMcTrack() noexcept {
        if(pending.pending.requested)return false;
        mcTrack_={};mcBound_=false;return true;
    }
    LapProgressTrack50GF track{};
    Continuity* continuity=nullptr;
    VehicleFrameFields49W* fields=nullptr;
    LapProgressContinuation50GF pending{};
    LapProgressDiagnostic50GF diagnostic{};
    LapCompletionCursor50GG completion{};
    LapCompletionResult50GG completionResult{};
    LapCompletionConfig50GG config{2,0,0};
    // Optional shared fresh-TA configuration. Null preserves mode2 baseline.
    // This owner stays outside racer snapshots. The explicit HOST accepted-
    // world adapter supplies timing/FIFO order; it claims no root scheduler parity.
    LapConfigTa50ID* nativeConfig=nullptr;
    void* completionEffectContext=nullptr;
    bool (*completionEffect)(void*,const LapCompletionRequest50GG&) noexcept=nullptr;
    // Shared race-lifetime original HUD, outside racer snapshots. Only the
    // actual human view is bound. Other effects retain their existing owner.
    OriginalRaceHud50KG* originalHudOwner=nullptr;
    const std::uint32_t* originalHudGroup34=nullptr;
    const std::uint32_t* originalHudConfig11050=nullptr;
    std::uint32_t originalHudView=0;
    bool bindOriginalHud(OriginalRaceHud50KG& hud,std::uint32_t view,
        const std::uint32_t& group34,const std::uint32_t& config11050) noexcept {
        if(!continuity||originalHudOwner||!hud.loaded()||view>=hud.viewCount())return false;
        const auto fields=hud.lapFields(view);if(!fields.known||!fields.lap134||!fields.race130||!fields.lastLap164)return false;
        continuity->originalHud=fields;originalHudOwner=&hud;originalHudView=view;
        originalHudGroup34=&group34;originalHudConfig11050=&config11050;return true;
    }
    lap_sound50GH::DisabledAudioProvider provider{};
    lap_sound50GH::Owner sound{};
    // Optional real native provider. Null retains the explicitly disabled
    // baseline. The caller owns the provider and its decoded PCM lifetime.
    void* audioContext=nullptr;
    AudioDispatchResult (*audioDispatch)(void*,std::uint32_t,std::uint32_t,
        std::uint32_t) noexcept=nullptr;
    std::uint64_t audibleStarts=0;
    std::uint64_t markers=0,completedCallbacks=0,unavailableSounds=0;

    // Cold only, once for each independently owned racer. Native fresh-profile
    // configuration/selection must have been prepared before this binding.
    bool initializeTiming() noexcept {
        if(!continuity||!nativeConfig||!nativeConfig->initialized||
           continuity->timingInitialized||nativeConfig->mode11258<1u||
           nativeConfig->mode11258>3u)return false;
        // Preserve the authored subtype3 best-lap field; use the shared
        // 216690 initializer rather than substituting subtype1 timing.
        initializeLapTimingFields50ID(continuity->timing,nativeConfig->mode11258);
        continuity->timingInitialized=true;return true;
    }
    LapCompletionConfig50GG liveConfig() const noexcept {
        return nativeConfig?completionConfig50ID(*nativeConfig):config;
    }
    // Resume exactly the already-pending50GG call. On missing query/HUD
    // backing, keep its cursor and enclosing50GF request; never replay audio.
    bool drainCompletion() noexcept {
        auto& c=*continuity;
        for(;;){
            if(nativeConfig&&!nativeConfig->initialized)return false;
            const auto currentConfig=liveConfig();
            if(currentConfig.mode11258!=2u&&(!nativeConfig||!nativeConfig->initialized||
               currentConfig.mode11258!=1u||!c.timingInitialized))return false;
            LapCompletionPlayer50GG player{};
            player.lapCC8=c.state.vehicleCC8;
            if(currentConfig.mode11258==1u){
                player.timeCB0=c.timing.timeCB0;player.bestLapCB4=c.timing.bestLapCB4;
                player.lapTimesCC0=c.timing.lapTimesCC0;
                player.lapTimeCount=lapTimingCapacity50ID;
            }
            completionResult=advanceLapCompletion50GG(player,currentConfig,completion);
            if(completionResult.status==LapCompletionStatus50GG::Complete){
                if(!resumePlayerLapAfterCallback50GF(c.state,pending,diagnostic))return false;
                fields->impactFlagsCD8=c.state.vehicleCD8;++completedCallbacks;return true;
            }
            const auto request=completionResult.request;
            std::uint32_t returnedV0=0;
            if(completionResult.status==LapCompletionStatus50GG::NeedQuery){
                if(!nativeConfig||!nativeConfig->initialized)return false;
                const auto recordConfig=recordConfig50ID(*nativeConfig);
                const auto tables=recordTables50ID(*nativeConfig);
                LapRecordQueryResult50ID query{LapRecordQueryStatus50ID::Storage,0,0};
                if(request.kind==LapCompletionCall50GG::LapLookup1861A0)
                    query=queryLapRecord50ID(recordConfig,tables,request.argumentA1,request.argumentF12);
                else if(request.kind==LapCompletionCall50GG::RaceLookup186280)
                    query=queryRaceRecord50ID(recordConfig,tables,request.argumentA1,request.argumentF12);
                else return false;
                if(!query.completed())return false;
                returnedV0=query.originalV0;
            }else if(completionResult.status==LapCompletionStatus50GG::NeedEffect){
                if(request.kind==LapCompletionCall50GG::OwnerSelection292360){
                    if(audioDispatch){
                        const auto result=audioDispatch(audioContext,request.argumentA1,
                            request.argumentA2,request.argumentA3);
                        if(!result.complete)return false;
                        if(result.started)++audibleStarts;
                        if(result.unavailable)++unavailableSounds;
                    }else{
                        const auto result=lap_sound50GH::dispatch(sound,request.argumentA1,
                            request.argumentA2,request.argumentA3);
                        if(!result.completed())return false;
                        if(static_cast<std::int32_t>(result.providerResult)<0)++unavailableSounds;
                    }
                }else{
                    const bool textCall=request.kind==LapCompletionCall50GG::LapRecord22B170||
                        request.kind==LapCompletionCall50GG::RaceRecord22B370||request.kind==LapCompletionCall50GG::LastLap22B760;
                    const auto nullBranch=applyLapHudNullBranch50ID(c.originalHud,request);
                    if(nullBranch==LapHudResult50ID::NullReturned){
                        // A real banked null HUD retains its original early return.
                    }else if(textCall&&originalHudOwner){
                        const auto expected=originalHudOwner->lapFields(originalHudView);
                        if(!originalHudGroup34||!originalHudConfig11050||!expected.known||!c.originalHud.known||
                           c.originalHud.lap134!=expected.lap134||c.originalHud.race130!=expected.race130||
                           c.originalHud.lastLap164!=expected.lastLap164||
                           !originalHudOwner->applyLapCompletion(originalHudView,*originalHudGroup34,
                               static_cast<std::uint8_t>(*originalHudConfig11050),request))return false;
                    }else{
                        const auto hud=applyLapHudNullBranch50ID(c.originalHud,request);
                        if(hud!=LapHudResult50ID::NullReturned&&
                           (!completionEffect||!completionEffect(completionEffectContext,request)))return false;
                    }
                }
            }else return false;
            //Only an actually returned effect/query reaches this acknowledgement.
            if(!acceptLapCompletionReturn50GG(completion,{request.sequence,request.kind,
                returnedV0,request.query},completionResult))return false;
        }
    }
    // Actual1AFF50 racer cut. Invoke at the explicitly chosen manager boundary,
    // with shared manager+30 ALREADY advanced by its real binary32 delta.
    // This method neither reads host50HM seconds nor silently clears lap flags.
    template<std::size_t N> bool managerTimingCut(LapTimingManager50ID& manager,
        float delta,std::uint32_t vehicle58,std::uint32_t vehicle5C,
        RaceActorId50ID actor,const RaceActorResolver50ID& actors,
        RaceEventQueue50ID<N>& sharedEvents) noexcept {
        auto& c=*continuity;
        if(!nativeConfig||!nativeConfig->initialized||!c.timingInitialized||
           pending.pending.requested||!actor)return false;
        LapTimingResult50ID result{LapTimingStatus50ID::NeedLapEvent,0,
            c.timingCursor.pending};
        if(!c.timingCursor.pending.sequence){
            const LapTimingInput50ID input{delta,nativeConfig->mode11258,vehicle58,
                vehicle5C,c.state.vehicleCC8,fields->impactFlagsCD8};
            result=beginRacerLapTiming50ID(c.timing,manager,input,c.timingCursor);
        }
        if(result.status==LapTimingStatus50ID::Complete)return true;
        if(result.status!=LapTimingStatus50ID::NeedLapEvent)return false;
        if(!c.timingEventReturned){
            const auto appended=appendRaceEvent50ID(sharedEvents,{0,6,0,actor,0},actors);
            if(!appended.helperReturned())return false;
            c.timingEventReturned=true;
        }
        //This completes queue helper1A64B0 only. Its FIFO entry still needs
        //the ordered event-manager flush; this is not speech completion.
        result=resumeRacerLapTimingAfterEvent50ID(c.timing,manager,c.timingCursor,
            result.request.sequence);
        if(result.status!=LapTimingStatus50ID::Complete)return false;
        c.timingEventReturned=false;return true;
    }

    static bool accepted(void* context,bool sequential,std::uint32_t node,
                         std::uint32_t current) noexcept {
        return static_cast<Owner*>(context)->crossing(sequential,node,current);
    }
    bool crossing(bool sequential,std::uint32_t node,std::uint32_t current) noexcept {
        if((mbBound_&&!mbTrack_.runtimeValid())||(tcBound_&&!tcTrack_.runtimeValid())||
           (tbBound_&&!tbTrack_.runtimeValid())||(gbBound_&&!gbTrack_.runtimeValid())||(mcBound_&&!mcTrack_.runtimeValid()))return false;
        auto& c=*continuity;
        c.state.vehicleCD8=fields->impactFlagsCD8;
        LapProgressReceipt50GF receipt{};
        const LapProgressCrossing50GF crossing{
            sequential?LapProgressCrossingKind50GF::Sequential:LapProgressCrossingKind50GF::Scan,
            node,current,c.prefix.flags10CC};
        if(!(mcBound_?applyPlayerLapCrossing50GF(mcTrack_,crossing,c.state,pending,receipt,diagnostic):gbBound_?applyPlayerLapCrossing50GF(gbTrack_,crossing,c.state,pending,receipt,diagnostic):
            tbBound_?applyPlayerLapCrossing50GF(tbTrack_,crossing,c.state,pending,receipt,diagnostic):
            tcBound_?applyPlayerLapCrossing50GF(tcTrack_,crossing,c.state,pending,receipt,diagnostic):
            mbBound_?applyPlayerLapCrossing50GF(mbTrack_,crossing,c.state,pending,receipt,diagnostic):
            applyPlayerLapCrossing50GF(track,crossing,c.state,pending,receipt,diagnostic)))return false;
        fields->impactFlagsCD8=c.state.vehicleCD8;
        if(receipt.progressMarkerConsumed)++markers;
        if(!receipt.callback.requested)return true;
        completionResult=beginLapCompletion50GG(completion);
        if(completionResult.status!=LapCompletionStatus50GG::NeedEffect)return false;
        return drainCompletion();
    }
};
}
