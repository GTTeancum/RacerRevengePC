#pragma once
#include "authored_parameters49n.h"
#include "track_navigation50eu.h"
#include <cstdint>

namespace rr::game {
// Typed projection of the writes at16E260/16E630. These defaults are source
// constructor values. Unrequested .ai keys (Skill/MyMaxSpeed/Defense/etc) have
// no destination here. loadedMask records actual successful scalar lookups.
struct AiProfile50IU {
    float combativeness1054=0.f;
    std::int32_t targetPersistence1058=0;
    float healthConsciousness105C=0.f,waitToRepairTime1060=0.f,repairTime1064=3.f;
    float attackRecoveryTime1068=0.f,startRaceNoCombatTime106C=0.f;
    float hoverBeforeAttackTime1070=0.f,frontHoverPosition1074=0.f;
    float sideHoverPosition1078=0.f,combatZoneLength107C=0.f;
    std::int32_t collisionRadius1020=0;
    float driveAggression1088=.5f;
    std::int32_t myMinSpeed108C=0;
    std::uint32_t loadedMask=0;
};
// Source16E580..16E5CC obtains a class from the actual race manager; class1
// overrides RepairTime after loading. This is not the pod's authored Tuning
// class override. The caller must pass the source-selected race class.
inline bool applyAiCreateClass50IU(AuthoredClass49N actualRaceClass,
    AiProfile50IU& profile) noexcept {
    if(actualRaceClass!=AuthoredClass49N::A&&actualRaceClass!=AuthoredClass49N::B&&
       actualRaceClass!=AuthoredClass49N::C)return false;
    if(actualRaceClass==AuthoredClass49N::A)profile.repairTime1064=2.f;
    return true;
}
// Cold scalar writes only; not a second live AI runtime owner. The native
// runtime copies these values once and owns subsequent changes. Native null
// target is represented by absence; no guest address or fabricated racer ID.
struct AiColdState50IU {
    std::uint32_t vehicle58=1,stateFE8=0,fieldFEC=0;
    std::int32_t packingReferenceFF8=-1;
    float speedScaleFFC=1.f,timer1000=0.f;
    std::uint32_t field1004=0;
    bool target1008Present=false;
    float timer100C=0.f,timer1010=0.f,timer1014=0.f,timer1018=0.f,timer101C=0.f;
    std::uint32_t listCounts1038[5]{};
    float field104C=-1.f,timer1050=0.f;
    std::int32_t index1080=-1,index1084=-1;
    float timer1090=1.5f,timer1094=0.f,timer1098=0.f,cooldown109C=6.f;
    std::uint32_t field10A0=0;
};
//16E830 is narrower than construction:1004, list counts and profile fields
// survive. In particular, never clear real list buffers or reload the profile.
inline void resetAiColdState50IU(AiColdState50IU& s,std::uint8_t& flagsB9C) noexcept {
    s.stateFE8=0;s.packingReferenceFF8=-1;s.speedScaleFFC=1.f;s.timer1000=0.f;
    s.target1008Present=false;s.timer100C=s.timer1010=s.timer1014=0.f;
    s.timer1018=s.timer101C=s.timer1050=0.f;s.index1080=s.index1084=-1;
    s.field104C=-1.f;s.cooldown109C=6.f;s.timer1090=1.5f;
    s.timer1094=s.timer1098=0.f;s.field10A0=0;s.fieldFEC=0;
    flagsB9C=static_cast<std::uint8_t>(flagsB9C&0xfdu);
}
struct AiTrackNodeMetadata50IU {
    float speedLimit5C=0.f,distanceToFinish60=0.f,boostScale64=1.f;
    std::int32_t branch68=0;
    std::uint8_t flags79=0;
    bool progressNode=false; // Original track+48/+4C list membership, not79.
};
struct AiTrackMetadata50IU {
    AiTrackNodeMetadata50IU nodes[maxUndertrackNodes50ET]{};
    std::uint32_t count=0;
    bool bound=false;
};
// Source1AAB30 constructor defaults, then1AA980..1AAB04 authored overrides.
struct AiTrackTuning50IU {
    float lookahead78=.7f,racePace7C=0.f,racePaceCutoffTime80=100.f;
    float behindPlayerClearance84=50.f,startLineBoostTime88=10.f;
    std::int8_t numPackingFront8C=4,numPackingRear8D=4;
    float farFrontDist90=1250.f,closeFrontDist94=500.f;
    float farBehindDist98=750.f,closeBehindDist9C=0.f;
    float slowestSpeedPctA0=.5f,fastestSpeedPctA4=1.f;
    std::uint32_t loadedMask=0;
    bool bound=false;
};
struct AiColdTrackBindings50IU {
    const TrackNavigation50EU* mapFF0=nullptr;
    const TrackNavigation50EU* aiFF4=nullptr;
    const AiTrackMetadata50IU* mapMetadata=nullptr;
    const AiTrackMetadata50IU* aiMetadata=nullptr;
};
//16E4EC/16E4F4 borrow distinct real track owners.217394..2173B8 establishes
// C80=1/C84=0/C88=1/C8C=0 only at source full initialization. This helper binds
// the owners and verifies that already-completed initialization; it does not
// silently overwrite progress, initialize bodies, or manufacture a manager.
inline bool bindAiColdTracks50IU(const TrackNavigation50EU& map,
    const TrackNavigation50EU& ai,const AiTrackMetadata50IU& mapMetadata,
    const AiTrackMetadata50IU& aiMetadata,const TrackNavigationProgress50EU& progress,
    AiColdTrackBindings50IU& out) noexcept {
    if(&map==&ai||!map.gatesPrepared||!ai.gatesPrepared||
       !map.indicesPrepared||!ai.indicesPrepared||!mapMetadata.bound||!aiMetadata.bound||
       map.track.count<2||ai.track.count<2||mapMetadata.count!=map.track.count||
       aiMetadata.count!=ai.track.count||progress.reset.vehicleC80!=1||
       progress.vehicleC84!=0||progress.reset.vehicleC88!=1||progress.reset.vehicleC8C!=0)
        return false;
    out={&map,&ai,&mapMetadata,&aiMetadata};return true;
}
} // namespace rr::game
