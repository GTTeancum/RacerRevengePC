// UNEXECUTED SOURCE-ONLY PROPOSAL. No compile/run was performed.
// Integrate beside ai_attack_wide50lh.h and the parent's observation patch.
// Link the native suite's body_scene48g.cpp/originalAngle48G implementation;
// preserve strict binary32 settings. Call testAiAttackWide50LHProposal(), or
// define RR_AI_ATTACK_WIDE50LH_STANDALONE for this file's standalone main.
#include "../../src/game/ai_attack_wide50lh.h"
#include <cassert>
#include <cfenv>
#include <limits>

namespace rr::game { namespace {
bool sameActions(const PlayerReadResult48X& a,const PlayerReadResult48X& b) {
    for(unsigned i=0;i<sizeof(a.raw.values)/sizeof(float);++i)
        if(a.raw.values[i]!=b.raw.values[i])return false;
    return a.actionB7C==b.actionB7C&&a.actionB80==b.actionB80&&a.flagsB9C==b.flagsB9C;
}
struct Fixture {
    TrackNavigation50EU track{};
    TrackNavigationProgress50EU progress{};
    AiObservedRacer50IV target{};
    AiAttackWideGlobals16EDD0 globals{};
    // Finite near-forward local row with nonzero X keeps normal side cases
    //away from172DF0's special X==0 branch and output saturation.
    float self[3]{0.f,0.f,-1000.f},forward[3]{.03125f,0.f,1.f};
    float field104C=-1.f,speed=0.f;
    PlayerReadResult48X actions{};
    AiDrivingDiagnostic50IV diagnostic{};
    Fixture() {
        track.gatesPrepared=true;track.track.count=3;
        const float zs[3]{100.f,0.f,200.f};
        for(unsigned i=0;i<3;++i){
            track.track.nodes[i].position[2]=zs[i];
            track.nodes[i].gate10[0]=-200.f;track.nodes[i].gate10[2]=zs[i];
            track.nodes[i].gate20[0]=200.f;track.nodes[i].gate20[2]=zs[i];
            track.nodes[i].left54=200.f;track.nodes[i].right58=200.f;
            track.track.nodes[i].nextCount=1;track.track.nodes[i].next[0]=2;
        }
        target.bound=true;target.vehicleC80=0;target.vehicleC84=1;
        target.position190[2]=50.f;target.up190[1]=1.f;target.velocity70[2]=1.f;
        progress.reset.vehicleC80=0;progress.vehicleC84=1;
        for(float& v:actions.raw.values)v=.375f;
        actions.actionB7C=.25f;actions.actionB80=.75f;actions.flagsB9C=0xa5u;
    }
    bool invoke() {
        return aiAttackWideSteering16EDD0(track,progress,target,self,forward,
            1.f,speed,0.f,0.f,0u,globals,field104C,actions,diagnostic);
    }
    void expectAim(float x,float z) {
        const PlayerReadResult48X before=actions;
        const auto progressBefore=progress;
        assert(invoke());
        float direction[3]{x-self[0],-self[1],z-self[2]};
        PlayerReadResult48X expected=before;AiDrivingDiagnostic50IV d{};
        // Reuse the independently retained172E60 only as the final control
        //oracle. The expected world aim is independently hand-derived.
        assert(steerAi50IV(direction,forward,1.f,speed,0.f,0.f,0u,expected,d));
        for(unsigned i=0;i<sizeof(actions.raw.values)/sizeof(float);++i)
            assert(actions.raw.values[i]==expected.raw.values[i]);
        assert(actions.raw.values[1]==0.f);
        assert(actions.actionB7C==before.actionB7C&&actions.actionB80==before.actionB80);
        assert(actions.flagsB9C==before.flagsB9C);
        assert(progress.reset.vehicleC80==progressBefore.reset.vehicleC80);
        assert(progress.vehicleC84==progressBefore.vehicleC84);
    }
};
}

void testAiAttackWide50LHProposal() {
    // Runner must establish the supported arithmetic domain, not change it
    //silently in production. Assertions must be enabled for this proposal.
    assert(std::fegetround()==FE_TONEAREST);
    { // Nearer positive candidate: +40 offset.
        Fixture f;f.self[0]=100.f;f.expectAim(40.f,100.f);
        assert(f.field104C==0.f&&f.globals.guardGP854C==1u);
        assert(f.globals.alpha433CB8==ai_speed_detail50IR::word(0x3dcccccdu));
    }
    { // Nearer negative candidate: -40 offset.
        Fixture f;f.self[0]=-100.f;f.expectAim(-40.f,100.f);
    }
    { // Equal XZ candidate distances choose positive.
        Fixture f;f.expectAim(40.f,100.f);
    }
    { // Edge10 distance30 forces positive despite self on the opposite side.
        Fixture f;f.target.position190[0]=-170.f;f.self[0]=-300.f;
        f.expectAim(-130.f,100.f);assert(f.field104C==-170.f);
    }
    { // Edge20 distance30 forces negative despite self on the opposite side.
        Fixture f;f.target.position190[0]=170.f;f.self[0]=300.f;
        f.expectAim(130.f,100.f);assert(f.field104C==170.f);
    }
    { // Edge10 distance exactly40 does not force positive.
        Fixture f;f.target.position190[0]=-160.f;f.self[0]=-300.f;
        f.expectAim(-200.f,100.f);assert(f.field104C==-160.f);
    }
    { // Sentinel bypasses filtering; ordinary update uses exact0.1 owner.
        Fixture f;f.target.position190[0]=20.f;f.field104C=10.f;
        f.expectAim(-29.f,100.f);assert(f.field104C==11.f);
    }
    { // Existing nonzero guard preserves the canonical changed alpha.
        Fixture f;f.target.position190[0]=20.f;f.field104C=10.f;
        f.globals.guardGP854C=7u;f.globals.alpha433CB8=.25f;
        f.expectAim(-27.5f,100.f);assert(f.field104C==12.5f);
        assert(f.globals.guardGP854C==7u&&f.globals.alpha433CB8==.25f);
    }
    { // Source-outside width uses center, but still writes signed104C.
        Fixture f;f.target.position190[0]=250.f;
        f.expectAim(0.f,100.f);assert(f.field104C==250.f);
    }
    { // Width equality remains inside. Edge20=0 forces sign-1.
        Fixture f;f.target.position190[0]=200.f;
        f.expectAim(160.f,100.f);assert(f.field104C==200.f);
    }
    { // Zero target velocity is real inline VRSQRT behavior, no fallback.
        Fixture f;f.target.velocity70[2]=0.f;f.expectAim(40.f,100.f);
    }
    { // A zero selected gate20-center vector normalizes to zero.
        Fixture f;f.track.nodes[0].gate20[0]=0.f;f.track.nodes[1].gate20[0]=0.f;
        f.expectAim(0.f,100.f);
    }
    { // Speed horizon selects literal Next0 even with multiple successors.
        Fixture f;f.speed=16000.f;
        f.track.track.nodes[0].nextCount=2;
        f.track.track.nodes[0].next[0]=2;f.track.track.nodes[0].next[1]=1;
        f.expectAim(40.f,200.f);
        // No route-choice/global-RNG owner is an argument to this leaf.
        // An integrated runtime test should additionally snapshot the real
        //application sharedRandomState across this branch; no fake RNG is
        //introduced into this isolated fixture merely to claim that check.
    }
    { // Self distance100 == speed800/8: strict comparison stays current.
        Fixture f;f.self[2]=0.f;f.speed=800.f;f.expectAim(40.f,100.f);
    }
    { // A late self-node failure preserves the original committed prefix.
        Fixture f;f.target.position190[0]=20.f;f.progress.reset.vehicleC80=3;
        const auto before=f.actions;
        assert(!f.invoke());assert(f.diagnostic.sourcePC==0x16f75cu);
        assert(f.field104C==20.f&&f.globals.guardGP854C==1u);
        assert(f.globals.alpha433CB8==ai_speed_detail50IR::word(0x3dcccccdu));
        assert(sameActions(f.actions,before));
    }
    { // A missing literal Next0 fails after104C, never ordinary fallback.
        Fixture f;f.speed=16000.f;f.track.track.nodes[0].nextCount=0;
        const auto before=f.actions;
        assert(!f.invoke());assert(f.diagnostic.sourcePC==0x2a26c0u);
        assert(f.field104C==0.f&&f.globals.guardGP854C==1u);
        assert(sameActions(f.actions,before));
    }
    { // Degenerate edge projection is explicitly outside the native domain.
        Fixture f;f.target.vehicleC84=f.target.vehicleC80;
        const auto before=f.actions;
        assert(!f.invoke());assert(f.diagnostic.error==AiDrivingError50IV::Arithmetic);
        assert(f.field104C==-1.f&&f.globals.guardGP854C==0u);
        assert(sameActions(f.actions,before));
    }
    { // Unsupported velocity arithmetic fails before globals/104C.
        Fixture f;f.target.velocity70[0]=std::numeric_limits<float>::infinity();
        assert(!f.invoke());assert(f.diagnostic.error==AiDrivingError50IV::Arithmetic);
        assert(f.field104C==-1.f&&f.globals.guardGP854C==0u);
    }
    { // Capacity rejection precedes every track-array access.
        Fixture f;f.track.track.count=static_cast<std::uint32_t>(maxUndertrackNodes50ET+1u);
        assert(!f.invoke());assert(f.diagnostic.error==AiDrivingError50IV::Owner);
        assert(f.field104C==-1.f&&f.globals.guardGP854C==0u);
    }
    {
        Fixture f;f.track.track.count=0;
        assert(!f.invoke());assert(f.diagnostic.error==AiDrivingError50IV::Owner);
        assert(f.field104C==-1.f&&f.globals.guardGP854C==0u);
    }
}
} // namespace rr::game

#ifdef RR_AI_ATTACK_WIDE50LH_STANDALONE
int main() {rr::game::testAiAttackWide50LHProposal();}
#endif
