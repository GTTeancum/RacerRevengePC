#pragma once
#include "authored_parameters49n.h"
#include "track_navigation50eu.h"
#include "original_course_descriptor50kg.h"
#include "dlc_course_entries50dl.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>

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
// Fresh ORIGINAL metadata, not a loaded save or a live menu/profile owner.
// Track IDs index the descriptor table at11278 with stride12; the class uses
// the independent course-order table at100C4 with stride40 (187B10).
// TA: ID0, course order0, AI class1, course-record pod19.
// SA: ID11, course order1, AI class1, course-record pod12.
// MA: ID4, course order2, AI class1, course-record pod20.
// GA WatchtowerRun: ID8, course order3, AI class1, course-record pod10.
// RA TheBrightlands: ID7, course order4, AI class1, course-record pod9.
// MB TheBallastComplex: ID5, course order6, AI class2, course-record pod22.
// SC SerresSarrano: ID12, course order8, AI class2, course-record pod16.
// TC: ID2, course order12, AI class3, course-record pod18.
// RC TheNightlands: ID3, course order11, AI class3, course-record pod24.
// TB TheBadlands: ID1, course order5, AI class2, course-record pod7.
// GC TheCitadel: ID10, course order10, AI class3, course-record pod17.
// GB: ID9, course order7, AI class2, course-record pod23.
// MC: ID6, course order9, AI class3, course-record pod8.
// Neither the visible Course control number nor the selected player pod may
// substitute for those source fields. Unknown names are rejected unchanged.
// Metadata binding alone does not admit a native world or change profile locks.
// Registry (add-on) course, 146r. PORT EXTENSION, NOT ORIGINAL: an add-on code
// has no descriptor row (1125F stays 0xff) and no tournament order (187B10
// stays -1). Its race class is the row's ai_class, mapped onto the same three
// values 16E590..16E5B4 produces (A=1, B=2, C=3), and its course-record pod
// is the row's rival (0 = none: no racer60 matches, FE4 stays 0 for all).
struct AiFreshCreateSelection50IU {
    std::uint8_t selectedTrackId1125F=0xff;
    std::int32_t selectedOrder187B10=-1;
    AuthoredClass49N actualRaceClass=AuthoredClass49N::A;
    std::int32_t freshCourseRecordPod11280=0;
    bool bound=false;
    bool registryCourse=false; // true = bound from the add-on registry row
};
inline bool bindRegistryAiCreateSelection50IU(const char* trackCode,
    AiFreshCreateSelection50IU& out,bool log) noexcept {
    const char code[3]={trackCode[0],trackCode[1],'\0'};
    char aiClass=0;unsigned rival=0;
    try{
        dlc50DL::AddonCourseLoad row;
        if(!dlc50DL::addonCourseLoadByCode(code,row))return false;
        aiClass=row.aiClass;rival=row.rivalRacer;
    }catch(...){return false;}
    AiFreshCreateSelection50IU next{};
    if(aiClass=='A')next.actualRaceClass=AuthoredClass49N::A;
    else if(aiClass=='B')next.actualRaceClass=AuthoredClass49N::B;
    else if(aiClass=='C')next.actualRaceClass=AuthoredClass49N::C;
    else return false; // the registry admits A/B/C only; anything else = no owner
    if(rival>0x7fffffffu)return false;
    next.freshCourseRecordPod11280=static_cast<std::int32_t>(rival);
    next.registryCourse=true;next.bound=true;out=next;
    if(log){
        if(rival)std::fprintf(stderr,"course_route_ai code=%s ai_class=%c rival=%u stock=0\n",code,aiClass,rival);
        else std::fprintf(stderr,"course_route_ai code=%s ai_class=%c rival=none stock=0\n",code,aiClass);
    }
    return true;
}
// log=false for a second reader of the same race selection (water grid), so a
// race prints one course_route_ai line.
inline bool bindFreshAiCreateSelection50IU(const char* trackCode,
    std::size_t bytes,AiFreshCreateSelection50IU& out,bool log=true) noexcept {
    if(!trackCode||bytes!=2)return false;
    // Original descriptor rows at profile11278, stride12. Names and record
    // pods are constructor data, independent of human rider and native world
    // readiness. MC:17EDBC..17EDD8 name at112C0;17EDF8/17EE00 record8
    // at112C8. The remaining rows retain their authenticated constructor data.
    const auto& descriptors=originalFreshCourseDescriptors50KG;
    const auto upper=[](char c) noexcept {
        return c>='a'&&c<='z'?static_cast<char>(c-'a'+'A'):c;
    };
    AiFreshCreateSelection50IU next{};
    for(std::size_t id=0;id<descriptors.size();++id){
        const auto& row=descriptors[id];
        if(upper(trackCode[0])==row.first&&upper(trackCode[1])==row.second){
            next.selectedTrackId1125F=static_cast<std::uint8_t>(id);
            next.freshCourseRecordPod11280=row.rival11280;
            break;
        }
    }
    // Not a stock descriptor code: the add-on registry row answers (registry
    // codes are never stock codes); unknown codes are still rejected unchanged.
    if(next.selectedTrackId1125F==0xff)return bindRegistryAiCreateSelection50IU(trackCode,out,log);
    // Original fresh byte stores17EF00..17F6EC. The187B10 scan compares
    // (1125F&15) with these entries; it does not use the track ID as order.
    constexpr std::uint8_t freshCourseOrder[]{0,11,4,8,7,1,5,9,12,6,10,3,2};
    for(std::size_t i=0;i<sizeof(freshCourseOrder);++i)
        if(freshCourseOrder[i]==next.selectedTrackId1125F){
            next.selectedOrder187B10=static_cast<std::int32_t>(i);break;
        }
    if(next.selectedOrder187B10<0)return false;
    //16E590..16E5B4, independent of the PHY filename/class exceptions49N.
    next.actualRaceClass=next.selectedOrder187B10<5?AuthoredClass49N::A:
        next.selectedOrder187B10<9?AuthoredClass49N::B:AuthoredClass49N::C;
    next.bound=true;out=next;
    if(log)std::fprintf(stderr,"course_route_ai code=%c%c ai_class=%c rival=%d stock=1 track_id=%u order=%d\n",
        upper(trackCode[0]),upper(trackCode[1]),"?ABC"[static_cast<unsigned>(next.actualRaceClass)&3u],
        next.freshCourseRecordPod11280,static_cast<unsigned>(next.selectedTrackId1125F),next.selectedOrder187B10);
    return true;
}
// Retained strict GC-only compatibility API. Existing callers keep their
// defaults and cannot accidentally admit GA under a GC-shaped selection.
//17EE2C stores "gc" at112F0: descriptor11278 +12*10.17F5A0 writes
//trackID10 at100C4 +40*10;187B10 returns10;16E590/5A4 select class3.
//17EE64 writes its course-record pod17 at112F8. This is NOT the current
//human-selected pod, and no loaded save/profile override is invented here.
struct AiGcCreateSelection50IU {
    std::uint8_t selectedTrackId1125F=10;
    std::int32_t selectedOrder187B10=10;
    AuthoredClass49N actualRaceClass=AuthoredClass49N::C;
    std::int32_t freshCourseRecordPod11280=originalFreshCourseDescriptors50KG[10].rival11280;
};
inline bool bindFreshGcAiCreateSelection50IU(const char* trackCode,
    std::size_t bytes,AiGcCreateSelection50IU& out) noexcept {
    if(!trackCode||bytes!=2||(trackCode[0]!='g'&&trackCode[0]!='G')||
       (trackCode[1]!='c'&&trackCode[1]!='C'))return false;
    out={};return true;
}
struct AiCreateIdentity50IU {
    std::uint32_t ordinalFE0=0;
    std::int32_t pod60=0;
    std::uint32_t courseRecordMatchFE4=0;
};
//1AE4C4 forms opponent ordinal+human count.16E474 stores FE0,2107E8
//uses it as vehicle5C. FE0 is an ordinal, never a manager pointer.
// Course-record pod 0 = an add-on course with rival "none" (PORT EXTENSION,
// NOT ORIGINAL; every stock row has a pod >0): no racer matches, FE4=0.
// Zero humans is admitted for an explicitly selected native AI roster. This
// does not claim the retail1AE620 human-dependent roster root supports it.
inline bool bindAiCreateIdentity50IU(std::uint32_t actualOpponentOrdinal,
    std::uint32_t actualHumanCount,std::int32_t actualPod60,
    std::int32_t actualCourseRecordPod,AiCreateIdentity50IU& out) noexcept {
    if(actualHumanCount>2||actualOpponentOrdinal>=8||
       actualHumanCount+actualOpponentOrdinal>=8||actualPod60<=0||actualCourseRecordPod<0)
        return false;
    out={actualOpponentOrdinal+actualHumanCount,actualPod60,
        actualPod60==actualCourseRecordPod?1u:0u};return true;
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
    float trackLength3C=0.f;
    std::uint32_t count=0;
    bool bound=false;
};
// Source1AAB30 constructor defaults, then1AA980..1AAB04 authored overrides.
struct AiTrackTuning50IU {
    float waitToRepairEnvTime6C=30.f,repairEnvUntil70=.5f,repairEnvThreshold74=1.f;
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
