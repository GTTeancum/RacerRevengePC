#include "lap_progress50gf.h"

namespace rr::game { namespace {
using E=LapProgressError50GF;
bool fail(LapProgressDiagnostic50GF& d,E error,std::uint32_t pc,
    std::uint32_t node=0) noexcept {d={error,pc,node};return false;}
bool validTrack(const LapProgressTrack50GF& t) noexcept {
    if(!t.prepared||!t.nodeCount||t.nodeCount>maxUndertrackNodes50ET||
       t.progressCount48>maxLapProgressNodes50GF||
       t.predecessorOfZero>=t.nodeCount)return false;
    for(std::uint32_t i=0;i<t.progressCount48;++i)
        if(t.progressNodes4C[i]>=t.nodeCount||
           (i&&t.progressNodes4C[i]<=t.progressNodes4C[i-1]))return false;
    return true;
}
} // namespace

bool prepareLapProgressTrack50GF(const TrackNavigationScratch50EU& scratch,
    LapProgressTrack50GF& out,LapProgressDiagnostic50GF& d) noexcept {
    const auto& source=scratch.candidate;
    if(!source.gatesPrepared||!source.track.count||
       source.track.count>maxUndertrackNodes50ET)
        return fail(d,E::Track,0x2a1980);
    LapProgressTrack50GF next{};next.nodeCount=source.track.count;
    bool predecessorFound=false;
    for(std::uint32_t i=0;i<next.nodeCount;++i){
        const auto& node=source.track.nodes[i];
        if(!node.nextCount||node.nextCount>maxUndertrackNext50ET)
            return fail(d,E::Track,0x2a273c,i);
        for(std::uint32_t j=0;j<node.nextCount;++j)
            if(node.next[j]>=next.nodeCount)return fail(d,E::Track,0x2a273c,i);
        // 2A26E0(node=0) returns the first authored Next0 pointing to zero.
        if(!predecessorFound&&node.next[0]==0){
            next.predecessorOfZero=i;predecessorFound=true;
        }
        if(scratch.fieldMask[i]&16u){
            if(next.progressCount48==maxLapProgressNodes50GF)
                return fail(d,E::Track,0x2a1948,i);
            next.progressNodes4C[next.progressCount48++]=i;
        }
    }
    if(!predecessorFound)return fail(d,E::MissingPredecessor,0x2a26e0);
    next.prepared=true;out=next;d={};return true;
}

bool applyPlayerLapCrossing50GF(const LapProgressTrack50GF& track,
    const LapProgressCrossing50GF& crossing,LapProgressState50GF& state,
    LapProgressContinuation50GF& continuation,LapProgressReceipt50GF& receipt,
    LapProgressDiagnostic50GF& d) noexcept {
    if(continuation.pending.requested)return fail(d,E::CallbackPending,0x1a0f24);
    if(!validTrack(track))return fail(d,E::Track,0x2a2fc0);
    const bool sequential=crossing.kind==LapProgressCrossingKind50GF::Sequential;
    if((!sequential&&crossing.kind!=LapProgressCrossingKind50GF::Scan)||
       crossing.crossedMapNode>=track.nodeCount||crossing.currentC88>=track.nodeCount||
       (sequential&&crossing.crossedMapNode!=crossing.currentC88)||
       state.vehicleFF8>track.progressCount48)
        return fail(d,E::Input,sequential?0x1a0e60:0x1a10d0,crossing.crossedMapNode);
    auto next=state;auto nextContinuation=continuation;LapProgressReceipt50GF result{};
    // The getter 2A2FD0 is unconditional in the source. At FF8==count its
    // comparison cannot permit any FF8 increment. Elide that effect-free read,
    // which can reach track+74 when count==10; do not fabricate its contents.
    if(next.vehicleFF8<track.progressCount48&&
       crossing.crossedMapNode==track.progressNodes4C[next.vehicleFF8]){
        ++next.vehicleFF8;result.progressMarkerConsumed=true;
    }
    if(sequential){
        // 1A0EA0..1A0F28. Bit2 is consumed before bit0/count are considered.
        if(crossing.currentC88==0){
            if(next.vehicleCD8&4u){
                next.vehicleCD8=static_cast<std::uint8_t>(next.vehicleCD8&~4u);
                result.shortcutSuppressionCleared=true;
            }else if(!(next.vehicleCD8&1u)&&next.vehicleFF8==track.progressCount48){
                ++next.vehicleCC8; // ADDIU/SW: low-word wrap, no signed C++ UB.
                const auto argumentA2=next.vehicleCD8; // 1A0F18, before SB.
                next.vehicleCD8=static_cast<std::uint8_t>((next.vehicleCD8&~2u)|2u);
                result.lapCounterIncremented=true;
                result.callback={true,0x1a0f24,0x1a2b80,0x1a0f2c,0,
                    argumentA2,next.vehicleCC8,next.vehicleFF8,next.vehicleCD8};
                nextContinuation.pending=result.callback;
                // Do not run 1A0F2C until the original callback boundary returns.
            }
        }
    }else{
        // 1A1108 checks live C88==1, NOT the accepted scan node==1.
        if(crossing.currentC88==1&&(crossing.vehicle10CC&2u)&&
           crossing.crossedMapNode==track.predecessorOfZero){
            next.vehicleCD8=static_cast<std::uint8_t>((next.vehicleCD8&~4u)|4u);
            result.shortcutSuppressionSet=true;
        }
    }
    state=next;continuation=nextContinuation;receipt=result;d={};return true;
}

bool resumePlayerLapAfterCallback50GF(LapProgressState50GF& state,
    LapProgressContinuation50GF& continuation,LapProgressDiagnostic50GF& d) noexcept {
    if(!continuation.pending.requested)return fail(d,E::NoPendingCallback,0x1a0f2c);
    state.vehicleFF8=0;continuation.pending={};d={};return true;
}
void initializePlayerLapRaceFields50GF(LapProgressState50GF& state) noexcept {
    state.vehicleCC8=0;
    state.vehicleCD8=static_cast<std::uint8_t>(state.vehicleCD8&~7u);
}
void initializePlayerLapCursor50GF(LapProgressState50GF& state) noexcept {
    state.vehicleFF8=0;
}
}
