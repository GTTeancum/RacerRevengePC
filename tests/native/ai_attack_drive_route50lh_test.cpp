// SOURCE-ONLY PROPOSAL. Authored for review; not compiled or executed.
// Intended standalone native test with source/src and integrated proposal
// headers on the include path. This does not drive an application or emulator.
#include "../../src/game/ai_attack_drive50lh.h"
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace rr::game;
namespace {
unsigned checks=0;
void require(bool condition,const char* text){++checks;if(!condition)throw std::runtime_error(text);}
std::uint32_t bits(float x){std::uint32_t v;std::memcpy(&v,&x,4);return v;}
float add(float x,float y){volatile float z=x+y;return z;}
float sub(float x,float y){volatile float z=x-y;return z;}
float mul(float x,float y){volatile float z=x*y;return z;}
void setGate(TrackNavigation50EU& track,std::uint32_t n,float z){
    track.nodes[n].gate10[0]=-100.f;track.nodes[n].gate20[0]=100.f;
    track.nodes[n].gate10[2]=track.nodes[n].gate20[2]=z;
}
void selectorCases(){
    TrackNavigation50EU track{};track.track.count=1;
    track.nodes[0].left54=125.f;track.nodes[0].right58=125.f;
    AiAttackDriveGlobals50LH globals{};AiAttackDriveSelection50LH pick{};
    AiDrivingDiagnostic50IV d{};std::uint8_t flags=0xff;float field=17.f;
    require(beginAiAttackDrive1711E0(track,0,3,flags,field,globals,pick,d),"mode3 equal-width success");
    require(flags==0xfd&&field==17.f,"171210 clears only repair, mode3 preserves104C");
    require(globals.widthThresholdInitialized&&globals.widthThreshold433CC8==250.f,"lazy433CC8 stores");
    require(!pick.wideSteering16EDD0&&pick.attackSpeed16FB50,"width250 ordinary steering plus attack speed");
    track.nodes[0].right58=126.f;
    require(beginAiAttackDrive1711E0(track,0,3,flags,field,globals,pick,d)&&pick.wideSteering16EDD0,"width251 wide steering");
    require(beginAiAttackDrive1711E0(track,0,4,flags,field,globals,pick,d),"mode4 success");
    require(field==-1.f&&!pick.wideSteering16EDD0&&pick.attackSpeed16FB50,"mode4 exact leaves/reset");
    field=22.f;
    require(beginAiAttackDrive1711E0(track,0,1,flags,field,globals,pick,d),"mode1 success");
    require(field==-1.f&&!pick.wideSteering16EDD0&&!pick.attackSpeed16FB50,"mode1 ordinary source leaves/reset");
    field=44.f;flags=0xff;globals={};track.track.count=maxUndertrackNodes50ET+1;
    require(!beginAiAttackDrive1711E0(track,0,3,flags,field,globals,pick,d),"oversize track boundary");
    require(flags==0xfd&&field==44.f&&!globals.widthThresholdInitialized,"171210 prefix survives later owner boundary");
}
void countDiagnostic(const char*,void* p) noexcept {++*static_cast<unsigned*>(p);}
void speedCases(){
    TrackNavigation50EU track{};track.track.count=3;track.gatesPrepared=true;
    setGate(track,0,0.f);setGate(track,1,100.f);setGate(track,2,100.f);
    AiTrackMetadata50IU metadata{};metadata.bound=true;metadata.count=3;metadata.trackLength3C=1000.f;
    metadata.nodes[0].distanceToFinish60=800.f;metadata.nodes[1].distanceToFinish60=500.f;
    metadata.nodes[2].distanceToFinish60=700.f;
    AiObservedRacer50IV target{};target.bound=true;target.cachedSpeed34=100.f;
    target.forward190[2]=1.f;target.linearAccelerationC0[2]=2.f;target.position190[2]=30.f;
    float position[]{0.f,0.f,10.f};unsigned diagnostics=0;
    AiAttackSpeedInput50LH in{&track,&metadata,&target,position,0,0,0,.25f,200.f,&countDiagnostic,&diagnostics};
    PlayerReadResult48X actions{};actions.flagsB9C=0xfd;actions.raw.values[0]=.25f;actions.raw.values[1]=-.5f;
    AiAttackSpeedReceipt50LH r{};AiDrivingDiagnostic50IV d{};AiSpeedDiagnostic50IR sd{};
    require(applyAiAttackSpeed16FB50(in,actions,r,d,sd),"same-node attack speed");
    const float k=ai_speed_detail50IR::word(0x3f2e8bc1u);
    const float expected=mul(k,add(100.f,add(add(2.f,sub(10.f,30.f)),mul(10.f,mul(2.f,.25f)))));
    require(bits(r.desiredSpeed)==bits(expected),"16FF14 scalar ACC survives VU dot; exact grouped desired");
    require(r.signedDistance==-20.f&&r.targetForwardAcceleration==2.f,"3D gates and actual targetC0 dot forward");
    require(actions.actionB7C==0.f&&actions.actionB80==1.f&&actions.flagsB9C==0xfc,"direct173770 brakes and clears boost");
    require(actions.raw.values[0]==.25f&&actions.raw.values[1]==-.5f,"attack speed preserves raw steering");
    require(diagnostics==0,"equal nodes skip2A2DC0");
    // Unread distance metadata must not be admitted/used for equal nodes.
    metadata.nodes[0].distanceToFinish60=std::numeric_limits<float>::quiet_NaN();
    metadata.trackLength3C=std::numeric_limits<float>::quiet_NaN();
    require(applyAiAttackSpeed16FB50(in,actions,r,d,sd),"same-node path does not read route-distance metadata");
    metadata.nodes[0].distanceToFinish60=800.f;metadata.trackLength3C=1000.f;
    target.vehicleC80=2;target.position190[2]=130.f;
    require(applyAiAttackSpeed16FB50(in,actions,r,d,sd),"different-node attack speed");
    require(r.signedDistance==80.f&&diagnostics==0,"target-first route correction, no warning for nodes0/2");
    require(bits(r.desiredSpeed)==bits(mul(k,187.f)),"target speed plus source feedback");
    target.vehicleC80=1;
    require(applyAiAttackSpeed16FB50(in,actions,r,d,sd),"target node1 diagnostic branch");
    require(diagnostics==1&&r.distance.targetLapAdjusted&&!r.distance.selfLapAdjusted,"exact diagnostic plus target-only lap correction");
    in.myMinSpeed108C=3000;
    require(applyAiAttackSpeed16FB50(in,actions,r,d,sd)&&r.desiredSpeed==3000.f&&r.minimumApplied,"MyMinSpeed signed integer floor");
    metadata.nodes[0].flags79=0x10;metadata.nodes[0].speedLimit5C=7.f;
    require(applyAiAttackSpeed16FB50(in,actions,r,d,sd)&&r.desiredSpeed==7.f&&r.driveFullSpeedOverride,"FullSpeed overrides even minimum");
    unsigned char before[sizeof actions];std::memcpy(before,&actions,sizeof actions);
    target.linearAccelerationC0[2]=std::numeric_limits<float>::quiet_NaN();
    require(!applyAiAttackSpeed16FB50(in,actions,r,d,sd),"invalid consumed acceleration boundary");
    require(std::memcmp(before,&actions,sizeof actions)==0,"failed planner does not pretend173770 ran");
    // No boost cooldown, heat or random owner appears in16FB50's signature;
    // source disassembly must remain the independent no173180/no308030 check.
}
void routeCases(){
    TrackNavigation50EU track{};track.track.count=8;track.gatesPrepared=true;
    auto&node=track.track.nodes[0];node.nextCount=4;
    node.next[0]=2;node.next[1]=3;node.next[2]=4;node.next[3]=5;
    auto&fork=track.track.nodes[1];fork.nextCount=2;fork.next[0]=6;fork.next[1]=7;
    track.track.nodes[6].position[2]=100.f;track.track.nodes[7].position[0]=100.f;
    track.nodes[2].branch68=track.nodes[3].branch68=track.nodes[5].branch68=track.nodes[6].branch68=60;
    track.nodes[4].branch68=track.nodes[7].branch68=70;
    track.nodes[2].flags79=1;track.nodes[3].flags79=0x40;
    AiTrackMetadata50IU metadata{};metadata.bound=true;metadata.count=8;
    AiObservedRacer50IV racers[1]{};auto&target=racers[0];target.bound=true;target.vehicleC84=1;target.vehicleC80=4;target.forward190[2]=1.f;
    std::uint32_t record=0,state=1,packing=0,rng=123,mode=1,targetIndex=0;bool present=true;
    AiRouteChoiceOwners50IY owners{metadata,record,state,packing,rng,&mode,&present,&targetIndex,racers,1};
    AiDrivingDiagnostic50IV d{};std::int32_t ordinal=-1;AiNavigationState50IV nav{};
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==3,"predicted branch60 skips blocked and Pretty");
    target.forward190[0]=1.f;target.forward190[2]=0.f;
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==2,"human actual forward predicts branch70");
    target.forward190[0]=target.forward190[2]=-1.f;
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==3,"all-negative scores default first branch");
    target.forward190[0]=target.forward190[2]=1.f;
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==3,"strict-greater tie keeps first branch");
    target.forward190[0]=0.f;target.forward190[2]=1.f;
    track.track.nodes[6].position[2]=1.e-30f;
    track.track.nodes[7].position[0]=0.f;track.track.nodes[7].position[2]=100.f;
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==3,
        "rounded-zero norm still multiplies delta by saturated Q, no zero-vector substitute");
    track.track.nodes[6].position[2]=100.f;
    track.track.nodes[7].position[0]=100.f;track.track.nodes[7].position[2]=0.f;
    const auto beforeRng=rng;std::uint32_t chosen=0;
    require(ai_driving_detail50IV::next(track,0,nav,owners,chosen,d)&&chosen==5,"171A90 consumes authentic attack ordinal");
    require(nav.choice1080==5&&nav.choice1084==-1&&rng==beforeRng&&!nav.routeRandomDraws,"attack route caches node, no draw");
    target.vehicle58=1;target.vehicleC84=999;target.vehicleC80=999;
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==-1,"AI target returns-1 before target geometry");
    target.vehicle58=0;target.vehicleC84=1;target.vehicleC80=4;fork.nextCount=1;
    require(ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d)&&ordinal==2,"nonfork human uses targetC80 branch");
    track.nodes[4].branch68=99;target.vehicleC80=7;track.nodes[7].branch68=88;nav={};
    const auto expectedRng=rng*0x41c64e6du+0x3039u;
    require(ai_driving_detail50IV::chooseRoute(track,node,nav,owners,ordinal,d),"genuine no-match return-1 reaches weighted route");
    require(rng==expectedRng&&nav.routeRandomDraws==1&&ordinal>=2&&ordinal<=3,"fallback source draw exactly once");
    present=false;const auto stoppedRng=rng;
    require(!ai_driving_detail50IV::chooseRoute(track,node,nav,owners,ordinal,d)&&d.sourcePC==0x171340u,"missing target is boundary, never fallback");
    require(rng==stoppedRng,"failed attack owner consumes no RNG");
    mode=3;
    require(ai_driving_detail50IV::chooseRoute(track,node,nav,owners,ordinal,d),"mode3 does not request target route171320");
    mode=1;present=true;track.track.count=maxUndertrackNodes50ET+1;
    require(!ai_driving_detail50IV::chooseAttackRoute171320(track,node,owners,ordinal,d),"route count capacity before node access");
}
}
int main(){
    try{
        require(std::fesetround(FE_TONEAREST)==0,"nearest rounding available");
        selectorCases();speedCases();routeCases();
        std::cout<<"PASS attack drive/route50LH: "<<checks<<" source-owner checks\n";return 0;
    }catch(const std::exception&e){std::cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
