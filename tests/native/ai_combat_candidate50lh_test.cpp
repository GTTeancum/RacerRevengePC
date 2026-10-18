#include "../../src/game/ai_combat_candidate50lh.h"
#include <cassert>
#include <cfenv>
#include <iostream>
#include <memory>
using namespace rr::game;
int main(){
    assert(std::fesetround(FE_TONEAREST)==0);
    auto track=std::make_unique<TrackNavigation50EU>();
    auto metadata=std::make_unique<AiTrackMetadata50IU>();
    track->gatesPrepared=true;track->track.count=3;
    metadata->bound=true;metadata->count=3;metadata->trackLength3C=10000.f;
    for(unsigned n=0;n<3;++n){track->nodes[n].gate10[0]=-5.f;
        track->nodes[n].gate20[0]=5.f;metadata->nodes[n].distanceToFinish60=3000.f;}
    float self[3]={0.f,0.f,0.f};
    AiObservedRacer50IV target{};target.bound=true;target.placeCCC=2;
    AiCombatCandidateInput50LH input{};input.trackFF4=track.get();input.metadata=metadata.get();
    input.target=&target;input.selfPosition190=self;input.selfPlaceCCC=1;
    input.combatZoneLength107C=500.f;
    unsigned diagnostics=0;
    input.diagnostic=[](const char*,void* context) noexcept {++*static_cast<unsigned*>(context);};
    input.diagnosticContext=&diagnostics;
    AiCombatCandidateReceipt50LH r{};AiDrivingDiagnostic50IV d{};
    unsigned checks=0;
    auto run=[&](){assert(evaluateAiCombatCandidate50LH(input,r,d));++checks;};
    // Same node, adjacent canonical rank: exact middle mode; all Y included.
    target.position190[1]=20.f;run();assert(r.accepted&&r.mode1050==3&&r.signedDistance==-20.f);
    target.flagsB9C=2;run();assert(!r.accepted&&!r.clearRouteChoices);
    target.flagsB9C=0;target.position190[1]=200.f;run();assert(r.accepted&&r.mode1050==3);
    target.position190[1]=201.f;run();assert(r.accepted&&r.mode1050==4);
    target.position190[1]=0.f;self[1]=10.f;run();assert(r.accepted&&r.mode1050==1&&r.clearRouteChoices);
    self[1]=9.f;run();assert(r.accepted&&r.mode1050==3&&!r.clearRouteChoices);
    self[1]=500.f;run();assert(r.accepted&&r.signedDistance==500.f);
    self[1]=501.f;run();assert(!r.accepted);
    self[1]=0.f;target.placeCCC=3;run();assert(!r.accepted);
    target.placeCCC=0;run();assert(r.accepted); // rank exactly self-1, source comparison
    target.placeCCC=2;target.lapCC8=99;run();assert(r.signedDistance==0.f&&diagnostics==0); // same-node bypass
    target.lapCC8=0;target.vehicleC80=2;metadata->nodes[2].distanceToFinish60=2900.f;
    run();assert(r.accepted&&r.signedDistance==100.f&&r.mode1050==1);
    input.selfLapCC8=1;run();assert(!r.accepted&&r.signedDistance==-9900.f);
    input.selfLapCC8=0;target.vehicleC80=1;metadata->nodes[1].distanceToFinish60=900.f;
    run();assert(!r.accepted&&r.distance.targetLapAdjusted&&diagnostics==1);
    target.vehicleC80=2;input.selfNodeC80=1;run();assert(!r.accepted&&r.distance.selfLapAdjusted&&diagnostics==2);
    input.selfNodeC80=0;target.vehicleC80=3;
    assert(!evaluateAiCombatCandidate50LH(input,r,d)&&d.sourcePC==0x2a2490u);++checks;
    target.vehicleC80=maxUndertrackNodes50ET;input.selfNodeC80=maxUndertrackNodes50ET;
    track->track.count=metadata->count=maxUndertrackNodes50ET+1;
    assert(!evaluateAiCombatCandidate50LH(input,r,d)&&d.sourcePC==0x2a2490u);++checks;
    target.vehicleC80=0;input.selfNodeC80=0;track->track.count=metadata->count=3;
    track->nodes[0].gate20[0]=-5.f;
    assert(!evaluateAiCombatCandidate50LH(input,r,d)&&!r.accepted);++checks;
    track->nodes[0].gate20[0]=5.f;
    assert(std::fesetround(FE_DOWNWARD)==0);
    assert(!evaluateAiCombatCandidate50LH(input,r,d)&&d.sourcePC==0x1706c0u);++checks;
    assert(std::fesetround(FE_TONEAREST)==0);
    // Direct2A2DC0 target correction wins over self correction, even when
    // both nominal node arguments are1; caller normally bypasses equal nodes.
    float distance=0.f;ai_combat_distance50LH::Receipt dr{};
    assert(ai_combat_distance50LH::distanceBetweenNodes2A2DC0(*metadata,1,0,1,0,
        distance,dr,input.diagnostic,input.diagnosticContext));
    assert(dr.targetLapAdjusted&&!dr.selfLapAdjusted&&distance==10000.f&&diagnostics==3);++checks;
    std::cout<<"combat candidate checks="<<checks<<"\n";
}
