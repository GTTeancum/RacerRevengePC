#pragma once
#include "lap_progress50gf.h"
#include "lap_completion50gg.h"
#include "vehicle_frame49w.h"
#include "lap_sound_owner50gh_fixture.h"

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
};
struct AudioDispatchResult {
    bool complete=false,started=false,unavailable=false;
    std::int32_t originalSoundV0=0;
};
struct Owner {
    LapProgressTrack50GF track{};
    Continuity* continuity=nullptr;
    VehicleFrameFields49W* fields=nullptr;
    LapProgressContinuation50GF pending{};
    LapProgressDiagnostic50GF diagnostic{};
    LapCompletionCursor50GG completion{};
    LapCompletionResult50GG completionResult{};
    LapCompletionConfig50GG config{2,0,0};
    lap_sound50GH::DisabledAudioProvider provider{};
    lap_sound50GH::Owner sound{};
    // Optional real native provider. Null retains the explicitly disabled
    // baseline. The caller owns the provider and its decoded PCM lifetime.
    void* audioContext=nullptr;
    AudioDispatchResult (*audioDispatch)(void*,std::uint32_t,std::uint32_t,
        std::uint32_t) noexcept=nullptr;
    std::uint64_t audibleStarts=0;
    std::uint64_t markers=0,completedCallbacks=0,unavailableSounds=0;

    static bool accepted(void* context,bool sequential,std::uint32_t node,
                         std::uint32_t current) noexcept {
        return static_cast<Owner*>(context)->crossing(sequential,node,current);
    }
    bool crossing(bool sequential,std::uint32_t node,std::uint32_t current) noexcept {
        auto& c=*continuity;
        c.state.vehicleCD8=fields->impactFlagsCD8;
        LapProgressReceipt50GF receipt{};
        const LapProgressCrossing50GF crossing{
            sequential?LapProgressCrossingKind50GF::Sequential:LapProgressCrossingKind50GF::Scan,
            node,current,c.prefix.flags10CC};
        if(!applyPlayerLapCrossing50GF(track,crossing,c.state,pending,receipt,diagnostic))return false;
        fields->impactFlagsCD8=c.state.vehicleCD8;
        if(receipt.progressMarkerConsumed)++markers;
        if(!receipt.callback.requested)return true;
        completionResult=beginLapCompletion50GG(completion);
        if(completionResult.status!=LapCompletionStatus50GG::NeedEffect||
           completionResult.request.kind!=LapCompletionCall50GG::OwnerSelection292360)return false;
        if(audioDispatch){
            const auto result=audioDispatch(audioContext,completionResult.request.argumentA1,
                completionResult.request.argumentA2,completionResult.request.argumentA3);
            if(!result.complete)return false;
            if(result.started)++audibleStarts;
            if(result.unavailable)++unavailableSounds;
        }else{
            const auto result=lap_sound50GH::dispatch(sound,completionResult.request.argumentA1,
                completionResult.request.argumentA2,completionResult.request.argumentA3);
            if(!result.completed())return false;
            if(static_cast<std::int32_t>(result.providerResult)<0)++unavailableSounds;
        }
        //292360 is a void effect boundary. A positive233410 sound result is
        // recorded by the native provider, not substituted as this helper's V0.
        const LapCompletionReturn50GG returned{completionResult.request.sequence,
            completionResult.request.kind,0,false};
        if(!acceptLapCompletionReturn50GG(completion,returned,completionResult))return false;
        LapCompletionPlayer50GG player{};player.lapCC8=c.state.vehicleCC8;
        completionResult=advanceLapCompletion50GG(player,config,completion);
        if(completionResult.status!=LapCompletionStatus50GG::Complete)return false;
        if(!resumePlayerLapAfterCallback50GF(c.state,pending,diagnostic))return false;
        fields->impactFlagsCD8=c.state.vehicleCD8;++completedCallbacks;return true;
    }
};
}
